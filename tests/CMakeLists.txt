
macro (build_and_run arg)
	# Add headers to sources to enable file browsing in IDEs
	include_directories(${CMAKE_BINARY_DIR}/tests)
	add_executable(${arg} ${arg}.cpp ${sqlpp_headers} ${CMAKE_CURRENT_LIST_DIR}/Sample.h)
	add_test(${arg} ${arg})
endmacro ()

build_and_run(InterpretTest)
build_and_run(InsertTest)
build_and_run(RemoveTest)
build_and_run(UpdateTest)
build_and_run(SelectTest)
build_and_run(FunctionTest)
build_and_run(PreparedTest)

find_package(PythonInterp REQUIRED)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_LIST_DIR}/Sample.h 
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/ddl2cpp ${CMAKE_CURRENT_LIST_DIR}/sample.sql Sample test
	)

