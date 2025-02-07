#include "header.h"


void sendDataJobStructureVec(vector<DataJobStructure>& data) {
    int size = data.size();
    cout<<"Отправляю "<< size * sizeof(DataJobStructure)<<" байт"<<endl;
    MPI_Send(&size, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);  // Отправляем размер вектора
    MPI_Send(data.data(), size * sizeof(DataJobStructure), MPI_BYTE, 0, TAG, MPI_COMM_WORLD);  // Отправляем данные
}

void receiveDataJobStructureVec(vector<DataJobStructure>& data, int proccess_id) {
    int num_elements;
    MPI_Recv(&num_elements, 1, MPI_INT, proccess_id, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  // Принимаем размер данных
    cout<<"Принимаю  "<<  num_elements * sizeof(DataJobStructure) <<" байт"<<endl;
    data.resize(num_elements);  // Резервируем память для данных
    MPI_Recv(data.data(), num_elements * sizeof(DataJobStructure), MPI_BYTE, proccess_id, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  // Принимаем данные
}


int main(int argc, char *argv[]) {
    Timer timer;
    int rank, size;
    long interval_preparing[2];// [строка с которой начинам обрабатывать, строка которой заканчиваем обработку]
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int size_calculate = size-1;
    //std::cout << "Процесс " << rank << " из " << size << std::endl;
    if(rank==0){
        long amount_str_general;
        long amount_str_for_each;
        long amount_str_for_last;

        amount_str_general = getAmountString(FILE_NAME);
        cout<<"amount  proccess = "<<size<<endl;
        cout<<"amount string general = "<<amount_str_general<<endl;
        amount_str_for_each = amount_str_general/size;
        amount_str_for_last = amount_str_general - (amount_str_for_each * (size-1));
        cout<<"Количество строк для каждого процесса кроме последнего: "<<amount_str_for_each<<endl;
        cout<<"Количество строк для последнего процесса: "<<amount_str_for_last<<endl;
        for(int i=1;i<size-1;i++){
            interval_preparing[0] = (amount_str_for_each*i+1);
            interval_preparing[1] = (amount_str_for_each*(i+1));
            MPI_Send(interval_preparing, 2, MPI_LONG, i, TAG, MPI_COMM_WORLD);
        }
        interval_preparing[0] = (amount_str_for_each*(size-1)+1);
        interval_preparing[1] = amount_str_general;
        MPI_Send(interval_preparing, 2, MPI_LONG, size-1, TAG, MPI_COMM_WORLD); // TAG - тег сообщения , MPI_COMM_WORLD - коммутатор
        interval_preparing[0] = 1;
        interval_preparing[1] = amount_str_for_each;
    }
    else{
        MPI_Recv(&interval_preparing, 2, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // MPI_STATUS_IGNORE - принимаем с любым статусом
    }

    ifstream file(FILE_NAME);
    unordered_map<string,pair<long,long>> mp; //(имя_работы) = [кол-во успешных, кол-во неуспешных]
    long curr_line_number=0;
    string curr_line;
    
    //cout<<"Перед циклом "<<"rank proccess = "<<rank<<"  "<<interval_preparing[0]<<"  "<<interval_preparing[1]<<endl;
    while(getline(file,curr_line)){
        if(curr_line_number<interval_preparing[0]){
            curr_line_number++;
            continue;
        }
        else if(curr_line_number<=interval_preparing[1]){
            pair<string, string>p = getJobNameAndStatusFromString(curr_line);
            
            if(p.second==COMPLETED){
                mp[p.first].first++;
            }else{
                mp[p.first].second++;
            }
            curr_line_number++;
        }
        else{
            break;
        }
        
    }


    if(rank!=0){
        vector<DataJobStructure> vec_data(mp.size());
        int index_counter=0;
        for( auto it = mp.begin();it!=mp.end(); ++it){
            DataJobStructure djs;
            strncpy(djs.job_name, it->first.c_str(), sizeof(djs.job_name) - 1); // куда, откуда, макс кол-во символов для копирования
            djs.job_name[sizeof(djs.job_name) - 1] = '\0';  
            djs.amount_completed=it->second.first;
            djs.amount_uncompleted=it->second.second;
            vec_data[index_counter]=djs;
            index_counter++;
            //cout<<it->first<<" "<< it->first.size()<<" " << vec_data[index_counter].job_name<<endl;
        }
        for(auto e: vec_data){
            //e.print();
        }
        sendDataJobStructureVec(vec_data);
    }

    if(rank==0){
        for(int i=1;i<size;i++){
            vector<DataJobStructure> recieved_data;
            receiveDataJobStructureVec(recieved_data,i);
            for(auto it = recieved_data.begin();it!=recieved_data.end();++it){
                if(mp.find(it->job_name)==mp.end()){
                    mp[it->job_name]=make_pair(it->amount_completed, it->amount_uncompleted);
                }
                else{
                    mp[it->job_name].first+=it->amount_completed;
                    mp[it->job_name].second+=it->amount_uncompleted;
                }
            }
        }
        multimap<double,string, greater<double>> mp_proporcion_jobName;
        for(auto it =mp.begin();it!=mp.end();it++){
            double key = (double)it->second.first/(double)(it->second.first+it->second.second);
            string val = it->first;
            mp_proporcion_jobName.insert(make_pair(key,val));
        }
        writeOutputData(mp_proporcion_jobName);
    }

    MPI_Finalize();
    return 0;

}
