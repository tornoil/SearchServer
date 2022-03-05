cmake_minimum_required(VERSION 3.0.0)
project(final_project_8 VERSION 0.1.0)


add_executable(final_project_8 main.cpp document.cpp read_input_functions.cpp request_queue.cpp search_server.cpp string_processing.cpp test_example_functions.cpp)
target_compile_features(final_project_8 PRIVATE cxx_std_17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(final_project_8 PRIVATE Threads::Threads)

