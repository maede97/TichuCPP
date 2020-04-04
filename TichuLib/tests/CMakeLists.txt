cmake_minimum_required(VERSION 3.10)

project(TichuLibTests)

set(ALL_TESTS
	Cards
	Namespace
)

foreach(test ${ALL_TESTS})
	add_executable(Test_${test} Test_${test}.cpp)

	add_test(${test} Test_${test})

	target_link_libraries(Test_${test} TichuLib)

	target_include_directories(Test_${test} PUBLIC TichuLib)

	set_property(TARGET Test_${test} PROPERTY FOLDER "Tests") #For solution explorer in visual studios
endforeach()
