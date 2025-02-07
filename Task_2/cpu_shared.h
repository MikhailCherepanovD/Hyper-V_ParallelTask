#pragma once
// #include <iostream>
// using namespace std;
#include <stdlib.h>
#include <cstdio>
extern "C" double* normalization( long* vec_time,long vec_size, long max_value, int amount_threads, int amount_blocks);
extern "C" void printSomething();