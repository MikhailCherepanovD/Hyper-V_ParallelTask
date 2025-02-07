# Virtual Heterogeneous Computing Cluster in Hyper-V  

This project involved creating a virtual heterogeneous computing cluster using the Microsoft Hyper-V hypervisor, where the nodes are virtual machines running Ubuntu Server as the guest OS.  

Number of virtual machines: 4. Two of them have access to the host's Nvidia GPU resources.  

After setting up and configuring the virtual cluster, its performance was tested by developing a program for statistical analysis on a provided dataset (about 1.5 million rows). The program was implemented using MPI and CUDA technologies.  

## Main Stages of Work  

* Creating cluster nodes;  
* Configuring networking between virtual machines;  
* Installing and setting up the necessary software;  
* Writing code to solve a practical task: The ability to run the same program on different configurations (GPU-only nodes, CPU-only nodes, GPU and CPU together) was achieved using dynamically linked libraries.  

Full report in Russian: <https://github.com/MikhailCherepanovD/Hyper-V_ParallelTask/blob/master/Report.pdf>