cmake_minimum_required(VERSION 3.20)  
project(standalone_datalake LANGUAGES CXX)  
  
set(CMAKE_CXX_STANDARD 20)  
set(CMAKE_CXX_STANDARD_REQUIRED ON)  
  
find_package(Threads REQUIRED)  
find_package(fmt REQUIRED)  
find_package(nlohmann_json REQUIRED)  





# clickhouse-cpp
# find_package(clickhouse-cpp REQUIRED)   # or use add_subdirectory if built from source  
# clickhouse-cpp
find_library(CLICKHOUSE_CPP_LIB
    NAMES clickhouse-cpp-lib
    PATHS /usr/local/lib
    REQUIRED
)

include_directories(/usr/local/include)




  
# ZMQ 
find_library(ZMQ_LIB zmq REQUIRED)  
  
# Data Lake library 
add_library(data_lake SHARED  
    src/data_lake.cpp  
    src/e3_agent.cpp  
)  
  
target_include_directories(data_lake PUBLIC  
    ${CMAKE_CURRENT_SOURCE_DIR}/src        # data_lake.hpp, e3_agent.hpp  
    ${CMAKE_CURRENT_SOURCE_DIR}/include    # all stubs  
)  


find_library(CITYHASH_LIB
    NAMES cityhash
    PATHS /usr/local/lib
    REQUIRED
)

find_library(LZ4_LIB
    NAMES lz4
    PATHS /usr/local/lib
    REQUIRED
)

find_library(ZSTD_LIB
    NAMES zstdstatic
    PATHS /usr/local/lib
    REQUIRED
)

find_library(ABSL_INT128_LIB
    NAMES absl_int128
    PATHS /usr/local/lib
    REQUIRED
)




target_link_libraries(data_lake PUBLIC
    Threads::Threads
    fmt::fmt
    nlohmann_json::nlohmann_json
    ${CLICKHOUSE_CPP_LIB}
    ${CITYHASH_LIB}
    ${LZ4_LIB}
    ${ZSTD_LIB}
    ${ABSL_INT128_LIB}
    ${ZMQ_LIB}
)  
# Standalone executable 
add_executable(standalone_datalake main.cpp)  
target_link_libraries(standalone_datalake PRIVATE data_lake)
