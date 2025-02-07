cmake_minimum_required(VERSION 3.10)
project(mpi_calculate)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Укажите компилятор для MPI
find_package(MPI)
include_directories(${MPI_INCLUDE_PATH})

# Добавьте исполняемый файл и его зависимости
add_executable(my_program main.cpp functions.cpp)

# Свяжите MPI-библиотеки
target_link_libraries(my_program ${MPI_LIBRARIES})