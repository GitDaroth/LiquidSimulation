cmake_minimum_required(VERSION 3.7.0)
project(LiquidPhysicsEngine VERSION 1.0.0 LANGUAGES CXX)

find_package(OpenCL REQUIRED)

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(LiquidPhysicsEngine STATIC ${sources})

target_include_directories(LiquidPhysicsEngine PUBLIC src)
target_compile_features(LiquidPhysicsEngine PRIVATE cxx_std_14)
target_link_libraries(LiquidPhysicsEngine PUBLIC OpenCL::OpenCL)

# testing
enable_testing()

#catch2
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE third_party/catch)

file(GLOB_RECURSE test_sources "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
add_executable(LiPhEnTest ${test_sources})

target_include_directories(LiPhEnTest PUBLIC test)
target_compile_features(LiPhEnTest PRIVATE cxx_std_14)
target_link_libraries(LiPhEnTest LiquidPhysicsEngine Catch)