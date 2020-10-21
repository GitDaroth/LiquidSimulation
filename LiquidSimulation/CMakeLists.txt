set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

set(miscFiles
	include/LiquidSimulation.h
	src/LiquidSimulation.cpp
	include/SPHLiquidWorld.h
	src/SPHLiquidWorld.cpp
	include/SPHParticleDrawable.h
	src/SPHParticleDrawable.cpp
	include/StaticCollisionObjectDrawable.h
	src/StaticCollisionObjectDrawable.cpp
	src/main.cpp)

set(renderingFiles
	include/Rendering/Camera3D.h
	src/Rendering/Camera3D.cpp
	include/Rendering/Mesh.h
	src/Rendering/Mesh.cpp
	include/Rendering/OpenGLWidget.h
	src/Rendering/OpenGLWidget.cpp)

set(scenarioFiles
	include/Scenarios/LiquidScenario.h
	src/Scenarios/LiquidScenario.cpp
	include/Scenarios/DamBreakScenario.h
	src/Scenarios/DamBreakScenario.cpp
	include/Scenarios/SphereWavesScenario.h
	src/Scenarios/SphereWavesScenario.cpp
	include/Scenarios/WaterDropsScenario.h
	src/Scenarios/WaterDropsScenario.cpp
	include/Scenarios/WaterfallScenario.h
	src/Scenarios/WaterfallScenario.cpp
	include/Scenarios/WaveBreakerScenario.h
	src/Scenarios/WaveBreakerScenario.cpp)

set(shaderFiles
	assets/shaders/simplePhong.frag
	assets/shaders/simplePhong.vert
	assets/shaders/instancedRendering.vert)

source_group("" FILES ${miscFiles})
source_group("\\Rendering" FILES ${renderingFiles})
source_group("\\Scenario" FILES ${scenarioFiles})
source_group("\\Shader" FILES ${shaderFiles})

add_executable(LiquidSimulation 
	${miscFiles}
	${renderingFiles}
	${scenarioFiles} 
	${shaderFiles})

target_link_libraries(LiquidSimulation PRIVATE LiquidPhysics Qt5::Core Qt5::Gui Qt5::Widgets)
target_include_directories(LiquidSimulation PUBLIC "include")


add_custom_target(copyAssets)
set_target_properties(copyAssets PROPERTIES FOLDER PostBuild)

FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets" source)
FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/assets" destination)

add_custom_command(
	TARGET copyAssets POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${source} ${destination}
	DEPENDS ${destination}
	COMMENT "copy assets folder from ${source} to ${destination}")

add_dependencies(LiquidSimulation copyAssets)


add_custom_target(copyCLKernels)
set_target_properties(copyCLKernels PROPERTIES FOLDER PostBuild)

FILE(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/LiquidPhysics/cl_kernels" source)
FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/cl_kernels" destination)

add_custom_command(
	TARGET copyCLKernels POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${source} ${destination}
	DEPENDS ${destination}
	COMMENT "copy OpenCL kernels folder from ${source} to ${destination}")

add_dependencies(LiquidSimulation copyCLKernels)