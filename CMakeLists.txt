cmake_minimum_required(VERSION 3.17)

project(LiquidSimulation 
	VERSION 0.0.1
	LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(LiquidPhysics)
add_subdirectory(LiquidSimulation)