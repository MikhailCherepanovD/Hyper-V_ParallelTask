nvcc -shared -o gpu_shared.so gpu_shared.cu -Xcompiler -fPIC
g++ -shared -o cpu_shared.so cpu_shared.c
mpic++ -o main_work main_work.cpp
mpirun -np 1 -hostfile hosts.txt -v ./main_work
