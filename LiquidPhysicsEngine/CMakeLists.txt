cmake_minimum_required(VERSION 3.7.0)
project(LiquidPhysicsEngine VERSION 1.0.0 LANGUAGES CXX)

find_package(OpenCL REQUIRED)

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(LiquidPhysicsEngine STATIC ${sources})

target_include_directories(LiquidPhysicsEngine PUBLIC src)
target_compile_features(LiquidPhysicsEngine PRIVATE cxx_std_14)
target_link_libraries(LiquidPhysicsEngine PUBLIC OpenCL::OpenCL)