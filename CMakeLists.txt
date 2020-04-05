cmake_minimum_required(VERSION 3.10)

project(TichuCPP)

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
find_package(TGUI 0.8 REQUIRED)

option(TICHULIB_ENABLE_TESTING "Enable Testing" ON)

if(TICHULIB_ENABLE_TESTING)
	enable_testing()
endif()

add_subdirectory(TichuClient)
add_subdirectory(TichuServer)
add_subdirectory(TichuLib)