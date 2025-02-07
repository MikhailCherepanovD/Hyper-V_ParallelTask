#include "cpu_shared.h"
void normalizationCPU(long* vec_time, double* vec_normal_time, long max_value,long size){
    for(int i=0;i<size;i++){
        vec_normal_time[i]=(double)vec_time[i]/(double)max_value;
    }
}
extern "C" double* normalization( long* vec_time,long vec_size, long max_value, int amount_threads, int amount_blocks) {
    long size = vec_size;
    double *vec_normal_time = (double*)malloc(size * sizeof(double));
    normalizationCPU(vec_time,vec_normal_time,max_value, size);
    return vec_normal_time;
}
