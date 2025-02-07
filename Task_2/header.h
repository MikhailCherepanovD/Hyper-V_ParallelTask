#pragma once
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm> 
using namespace std;
using namespace std::chrono;

const string FILE_NAME = "test_data";
const string OUTPUT_FILE_NAME = "output_result";

long parseDateTimeToSeconds(const string& dt) {
    if (dt.size() < 19) return 0;
    std::tm tmStruct = {};
    tmStruct.tm_year = std::stoi(dt.substr(0, 4)) - 1900;
    tmStruct.tm_mon = std::stoi(dt.substr(5, 2)) - 1;
    tmStruct.tm_mday = std::stoi(dt.substr(8, 2));
    tmStruct.tm_hour = std::stoi(dt.substr(11, 2));
    tmStruct.tm_min = std::stoi(dt.substr(14, 2));
    tmStruct.tm_sec = std::stoi(dt.substr(17, 2));
    time_t t = std::mktime(&tmStruct);
    if (t == -1) return 0;
    return (long long)t;
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
template<typename T>
void printVec(vector<T> vec){
    for(auto e:vec){
        cout<<e<<" ";
    }
    cout<<endl;
}

long findMaxElement(std::vector<long>& vec) {
    if (vec.empty()) {
        throw std::invalid_argument("Вектор пуст. Максимальный элемент невозможно найти.");
    }
    long ret = *max_element(vec.begin(),vec.end());
    return ret;
}

pair<long,long>getSubmitAndStartFromString(const string& str){
    stringstream job_id;
    stringstream a;
    stringstream b;
    int divider_counter=0;
    for(int i=0;i<str.size();i++){
        if(str[i]=='|'){
            divider_counter++;
            continue;
        }
         if(divider_counter==0){
            job_id<<str[i];
        }
        if(divider_counter==11){
            a<<str[i];
        }
        if(divider_counter==13){
            b<<str[i];
        }
    }
    long begin_waiting = parseDateTimeToSeconds(a.str());
    long end_waiting = parseDateTimeToSeconds(b.str());
    long id;
    job_id>>id;
    return make_pair(id,end_waiting-begin_waiting);
}

void outputThreeVecToFile(vector<long> vec_job_name, vector<long>vec_time, vector<double> vec_normal_time, int marker_overwrite){
    ofstream outFile(OUTPUT_FILE_NAME, marker_overwrite ? ios::trunc : ios::app); // Если marker_overwrite == 1, файл перезаписывается, иначе данные добавляются в конец

    if (!outFile) {
        cerr << "Ошибка при открытии файла: "<< OUTPUT_FILE_NAME << endl;
        return;
    }
    long size=vec_job_name.size();
    if(marker_overwrite){
        outFile<<"job_id"<<"|"<<"time"<<"|"<<"normal_time"<<endl;
    }
    for(long index=0;index<size;index++){
        outFile<<vec_job_name[index]<<"|"<<vec_time[index]<<"|"<<vec_normal_time[index]<<endl;
    }
    outFile.close();
}
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

class Timer {
public:
    Timer() {
        // Сохраняем текущее время в момент создания объекта
        start_time = high_resolution_clock::now();
    }

    ~Timer() {
        // В момент уничтожения объекта вычисляем разницу
        auto end_time = high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
        
        // Переводим разницу в секунды и миллисекунды
        double seconds = duration.count() / 1000000.0;
        
        // Выводим результат в формате сек.миллисек
        cout << "Time elapsed: " 
             << fixed << setprecision(6) << seconds 
             << " seconds" << endl;
    }

private:
    high_resolution_clock::time_point start_time;
};
