cmake_minimum_required(VERSION 3.10)

project(TichuLib)

find_package(SFML 2.5 COMPONENTS graphics network REQUIRED)

add_subdirectory(tests)

file(GLOB sources
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.h
	)

add_library(${PROJECT_NAME} STATIC ${sources})

target_include_directories (
	${PROJECT_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
)

target_link_libraries(
	${PROJECT_NAME}
		sfml-system sfml-window sfml-graphics sfml-network sfml-audio
)