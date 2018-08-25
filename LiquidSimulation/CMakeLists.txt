cmake_minimum_required(VERSION 3.7.0)
project(LiquidSimulation VERSION 1.0.0 LANGUAGES CXX)

# Qt specific
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Qt5Core and Qt5Gui is already included in Qt5Widgets
find_package(Qt5 COMPONENTS Widgets REQUIRED)

file (GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(LiquidSimulation ${sources})

target_include_directories(LiquidSimulation PUBLIC src)
target_compile_features(LiquidSimulation PRIVATE cxx_std_14)
target_link_libraries(LiquidSimulation PUBLIC Qt5::Widgets LiquidPhysicsEngine)

# This runs after every build
# Copy OpenCL kernels into the build directory
set (cl_source "${CMAKE_SOURCE_DIR}/LiquidPhysicsEngine/src/OpenCLKernels")
set (cl_destination "${CMAKE_BINARY_DIR}/LiquidSimulation/cl_kernels")

add_custom_target(CLKernels)
add_custom_command(
		TARGET CLKernels POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${cl_source} ${cl_destination})

add_dependencies(LiquidSimulation CLKernels)

# Copy shaders into the build direcotry
set (shaders_source "${CMAKE_SOURCE_DIR}/LiquidSimulation/src/OpenGLShader")
set (shaders_destination "${CMAKE_BINARY_DIR}/LiquidSimulation/shaders")

add_custom_target(Shaders)
add_custom_command(
		TARGET Shaders POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${shaders_source} ${shaders_destination})

add_dependencies(LiquidSimulation Shaders)