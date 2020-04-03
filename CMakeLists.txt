cmake_minimum_required(VERSION 3.10)

project(TichuCPP)

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)

add_subdirectory(TichuClient)
add_subdirectory(TichuServer)
add_subdirectory(TichuLib)