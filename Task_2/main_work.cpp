#include "header.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <dlfcn.h>
#include <mpi.h>
#include <algorithm> 
using namespace std;
void getGeneralMaxTimeElapsed(long& max_time, int rank, int size){
    if(rank!=0){
        MPI_Send(&max_time, 1, MPI_LONG, 0, 2, MPI_COMM_WORLD);
    }
    else{
        vector<long> vec_of_max_val;
        vec_of_max_val.push_back(max_time);
        for(int i=1;i<rank;i++){
            long help_val;
            MPI_Recv(&help_val, 1, MPI_LONG, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vec_of_max_val.push_back(help_val);
        }
        max_time = *max_element(vec_of_max_val.begin(),vec_of_max_val.end());
    }
    MPI_Bcast(&max_time, 1, MPI_LONG, 0, MPI_COMM_WORLD);
}
void writeOutputInOrder(int rank, int size,vector<long>& vec_job_id, vector<long>& vec_time, vector<double>& vec_normal_time) {
    // Каждый процесс будет ждать своей очереди
    for (int i = 0; i < size; ++i) {
        if (rank == i) {
            if(rank==0){
                outputThreeVecToFile(vec_job_id,vec_time,vec_normal_time,1);
            }
            else{
                outputThreeVecToFile(vec_job_id,vec_time,vec_normal_time,0);
            }
            cout << "Процесс№ " << rank << " записал в файл." << endl;
        }
        MPI_Barrier(MPI_COMM_WORLD); // Все процессы ждут друг друга
    }
}


int main(int argc, char* argv[]) {
    int rank, size;
    Timer *timer =nullptr;
    int amount_blocks=256;
    int amount_threads;
    long interval_preparing[2];// [строка с которой начинам обрабатывать, строка которой заканчиваем обработку]
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(rank==0){
        timer=new Timer();
    }
    int size_calculate = size-1;
    if(rank==0){
        long amount_str_general;
        long amount_str_for_each;
        long amount_str_for_last;

        amount_str_general = getAmountString(FILE_NAME);
        amount_threads = amount_str_general/amount_blocks + 1;

        cout<<"amount  proccess = "<<size<<endl;
        cout<<"amount string general = "<<amount_str_general<<endl;
        amount_str_for_each = amount_str_general/size;
        amount_str_for_last = amount_str_general - (amount_str_for_each * (size-1));
        cout<<"Количество строк для каждого процесса кроме последнего: "<<amount_str_for_each<<endl;
        cout<<"Количество строк для последнего процесса: "<<amount_str_for_last<<endl;
        for(int i=1;i<size-1;i++){
            interval_preparing[0] = (amount_str_for_each*i+1);
            interval_preparing[1] = (amount_str_for_each*(i+1));
            //cout<<i<<"   "<<interval_preparing[0]<<" "<<interval_preparing[1]<<endl;
            MPI_Send(interval_preparing, 2, MPI_LONG, i, 0, MPI_COMM_WORLD);
        }
        interval_preparing[0] = (amount_str_for_each*(size-1)+1);
        interval_preparing[1] = amount_str_general;
        MPI_Send(interval_preparing, 2, MPI_LONG, size-1, 0, MPI_COMM_WORLD); // интервал
        interval_preparing[0] = 1;
        interval_preparing[1] = amount_str_for_each;
        for(int i=1;i<size;i++){
            MPI_Send(&amount_threads, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        }
    }
    else{
        MPI_Recv(&interval_preparing, 2, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // MPI_STATUS_IGNORE - принимаем с любым статусом
        MPI_Recv(&amount_threads, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    ifstream file(FILE_NAME);
    string curr_line;
    long curr_line_number=0;
    long index_counter=0;
    long amount_string_for_this_process = interval_preparing[1]-interval_preparing[0]+1;
    vector<long> vec_job_id(amount_string_for_this_process);
    vector<long> vec_time(amount_string_for_this_process);

    while(getline(file,curr_line)){
         if(curr_line_number<interval_preparing[0]){
            curr_line_number++;
            continue;
        }
        else if(curr_line_number<=interval_preparing[1]){
            pair<long, long> p= getSubmitAndStartFromString(curr_line);
            vec_job_id[index_counter]=p.first;
            vec_time[index_counter]=p.second;
            curr_line_number++;
            index_counter++;
        }
        else{
            break;
        }
    }

    long max_time_elapsed = findMaxElement(vec_time);
    getGeneralMaxTimeElapsed(max_time_elapsed,rank,size);

    if(rank==0){
        cout<<"Max value for normalization: "<<max_time_elapsed<<endl;
    }

    void* handle;
    const char* host_type = std::getenv("HOST_TYPE");
    if(strcmp(host_type,"GPU")==0){
        cout<<"Процесс№: "<<rank<<" исполняется на GPU "<<endl;
        handle = dlopen("/home/user1/mpi/cuda_calculate_shared/gpu_shared.so", RTLD_LAZY);
    }
    else{
        cout<<"Процесс№: "<<rank<<" исполняется на CPU "<<endl;
        handle = dlopen("/home/user1/mpi/cuda_calculate_shared/cpu_shared.so", RTLD_LAZY);
    }
    double* (*normalization)(long*,long,long,int,int);
    normalization = (double* (*)(long*,long,long,int,int)) dlsym(handle, "normalization"); 

    double* vec_normal_time_ptr = normalization(vec_time.data(),amount_string_for_this_process,max_time_elapsed,amount_blocks,amount_threads);//
    vector<double> vec_normal_time(vec_normal_time_ptr,vec_normal_time_ptr+amount_string_for_this_process);
    writeOutputInOrder(rank,size,vec_job_id,vec_time,vec_normal_time);
    free(vec_normal_time_ptr);    
    dlclose(handle);
    if(rank==0){
        delete timer;
    }
    MPI_Finalize();
    return 0;
}

