cmake_minimum_required(VERSION 3.22)
project(PID_Tuning_GA)

set(CMAKE_CXX_STANDARD 14)

include_directories("C:\\Program Files\\MATLAB\\R2022a\\extern\\include\\")

add_executable(PID_Tuning_GA main.cpp Chromosome.cpp GeneticAlgorithm.cpp)

target_link_libraries(PID_Tuning_GA "C:/Program Files/MATLAB/R2022a/extern/lib/win64/mingw64/libMatlabEngine.lib")
target_link_libraries(PID_Tuning_GA "C:/Program Files/MATLAB/R2022a/extern/lib/win64/mingw64/libMatlabDataArray.lib")