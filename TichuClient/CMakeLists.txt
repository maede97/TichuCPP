cmake_minimum_required(VERSION 3.10)
project(TichuClient)

file(GLOB sources
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.h
	)

add_executable(${PROJECT_NAME} ${sources})

target_include_directories (
	${PROJECT_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
	TichuLib
	tgui
)

target_link_libraries (${PROJECT_NAME} TichuLib tgui sfml-graphics sfml-audio)