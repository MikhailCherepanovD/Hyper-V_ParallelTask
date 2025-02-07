#include "functions.h"

pair<string,string>getJobNameAndStatusFromString(const string& str){
    stringstream a;
    stringstream b;
    int divider_counter=0;
    for(int i=0;i<str.size();i++){
        if(str[i]=='|'){
            divider_counter++;
            continue;
        }
        if(divider_counter==3){
            a<<str[i];
        }
        if(divider_counter==20){
            b<<str[i];
        }
    }
    return make_pair(a.str(),b.str());
}

pair<long,string>getJobIdAndStatusFromString(const string& str){
    stringstream a;
    stringstream b;
    int divider_counter=0;
    for(int i=0;i<str.size();i++){
        if(str[i]=='|'){
            divider_counter++;
            continue;
        }
        if(divider_counter==0){
            a<<str[i];
        }
        if(divider_counter==20){
            b<<str[i];
        }
    }
    long job_id;
    a>>job_id;
    return make_pair(job_id,b.str());
}


void printPairString_LongLong(pair<string,pair<long,long>> p){
    cout<<p.first<<" = "<<"["<<p.second.first<<","<<p.second.second<<"]"<<endl;
}
void printMp(unordered_map<string,pair<long,long>> mp){
    cout<<endl<<"Печатаю хэш таблицу:"<<endl;
    for(auto e: mp){
        printPairString_LongLong(e);
    }
    cout<<endl<<endl;
}

long getAmountString(string file_name){
    string command = "wc -l < " + file_name;

    FILE* fp = popen(command.c_str(), "r");
    if (fp == nullptr) {
        cerr << "Ошибка при выполнении команды!" << endl;
        return -1;  
    }

    char buffer[128];  
    if (fgets(buffer, sizeof(buffer), fp) != nullptr) {
        int line_count = std::atoi(buffer);  
        pclose(fp);  
        return line_count;  
    }

    pclose(fp);  
    return -1;  
}

void DataJobStructure::print(){
    cout<<"Job id: "<<this->job_name<<", completed: "<<this->amount_completed<<", uncompleted: "<<this->amount_uncompleted<<endl;
}

void writeOutputData(multimap<double,string,greater<double>>mp){
    ofstream outFile(OUTPUT_FILE_NAME);

    if (!outFile) {
        cerr << "Ошибка при открытии файла: "<< OUTPUT_FILE_NAME << endl;
        return;
    }
    for(const auto& e:mp){
        outFile<<e.second<<"|"<<e.first<<endl;
    }
    outFile.close();
}