#include "gpu_shared.h"

__global__ void normalizationGPU(long* vec_time, double* vec_normal_time, long max_value, long size) {
    long idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < size) {
        vec_normal_time[idx] = (double)vec_time[idx] / (double)max_value; // Нормализация
    }
}

extern "C" double* normalization( long* vec_time,long vec_size, long max_value, int amount_blocks, int amount_threads) {
    long size = vec_size;
    double *vec_normal_time = (double*)malloc(size * sizeof(double));
    long *d_vec_time;
    double *d_vec_normal_time;
    cudaMalloc(&d_vec_time, size * sizeof(long));
    cudaMalloc(&d_vec_normal_time, size * sizeof(double));
    cudaMemcpy(d_vec_time, vec_time, size * sizeof(long),cudaMemcpyHostToDevice);
    
    normalizationGPU<<<amount_threads, amount_blocks>>>(d_vec_time,d_vec_normal_time,max_value, size);

    cudaMemcpy(vec_normal_time, d_vec_normal_time, size * sizeof(double),cudaMemcpyDeviceToHost);
    cudaFree(d_vec_time);
    cudaFree(d_vec_normal_time);
    return vec_normal_time;
}


