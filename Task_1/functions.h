#pragma once
#include <iostream>
#include <mpi.h>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iomanip>
using namespace std;
using namespace std::chrono;
const int TAG=0;
const string FILE_NAME = "test_data";
const string COMPLETED = "COMPLETED";
const string OUTPUT_FILE_NAME = "output_result";

long getAmountString(string file_name);
void printPairString_LongLong(pair<string,pair<long,long>> p);
void printMp(unordered_map<string,pair<long,long>> mp);
pair<string,string>getJobNameAndStatusFromString(const string& str);

pair<long,string>getJobIdAndStatusFromString(const string& str);

template<typename T, typename V>
void printMpT(map<T,V> mp){
    for(auto it = mp.begin(); it != mp.end(); ++it){
        cout<<it->first<<": "<<it->second<<";  ";
    }
    cout<<endl;
}

template<typename T, typename V>
void printMpT(multimap<T,V,greater<T>> mp){
    for(auto it = mp.begin(); it != mp.end(); ++it){
        cout<<it->first<<": "<<it->second<<";  ";
    }
    cout<<endl;
}

template<typename T, typename V>
void printMpT(multimap<T,V> mp){
    for(auto it = mp.begin(); it != mp.end(); ++it){
        cout<<it->first<<": "<<it->second<<";  ";
    }
    cout<<endl;
}
void writeOutputData(multimap<double,string,greater<double>>);

struct DataJobStructure{
    char job_name[100];
    long amount_completed;
    long amount_uncompleted;
    void print();
};

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
