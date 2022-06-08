cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(tests)

#add_executable(0001_queues_copy_direct 0001_queues_copy_direct.cpp device.h device.cpp)
#target_link_libraries(0001_queues_copy_direct PUBLIC d3d12.lib dxgi.lib dxcompiler.lib)

#add_executable(0002_queues_copy_compute 0002_queues_copy_compute.cpp device.h device.cpp)
#target_link_libraries(0002_queues_copy_compute PUBLIC d3d12.lib dxgi.lib dxcompiler.lib)

#add_executable(0003_queues_compute_direct 0003_queues_compute_direct.cpp device.h device.cpp)
#target_link_libraries(0003_queues_compute_direct PUBLIC d3d12.lib dxgi.lib dxcompiler.lib)

add_executable(hlsl_res_array_on_multiple_tables hlsl_res_array_on_multiple_tables.cpp device.h device.cpp)
target_link_libraries(hlsl_res_array_on_multiple_tables PUBLIC d3d12.lib dxgi.lib dxcompiler.lib)