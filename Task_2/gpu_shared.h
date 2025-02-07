#pragma once
#include <vector>
#include <iostream>
using namespace std;
extern "C" double* normalization( long* vec_time,long vec_size, long max_value, int amount_threads, int amount_blocks);