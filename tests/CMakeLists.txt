
macro (build_and_run arg)
	# Add headers to sources to enable file browsing in IDEs
	include_directories("${CMAKE_BINARY_DIR}/tests")
	add_executable("${arg}" "${arg}.cpp" ${sqlpp_headers} "${CMAKE_CURRENT_LIST_DIR}/Sample.h")
	add_test("${arg}" "${CMAKE_BINARY_DIR}/tests/${arg}")
endmacro ()

build_and_run(BooleanExpression)
build_and_run(CustomQuery)
build_and_run(Interpret)
build_and_run(Insert)
build_and_run(Remove)
build_and_run(Update)
build_and_run(Select)
build_and_run(SelectType)
build_and_run(Function)
build_and_run(Prepared)
build_and_run(Minimalistic)
build_and_run(Result)
build_and_run(Union)
build_and_run(With)

# if you want to use the generator, you can do something like this:
#find_package(PythonInterp REQUIRED)
#add_custom_command(
#	OUTPUT "${CMAKE_CURRENT_LIST_DIR}/Sample.h"
#	COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/ddl2cpp" "${CMAKE_CURRENT_LIST_DIR}/sample.sql" Sample test
#	DEPENDS "${CMAKE_CURRENT_LIST_DIR}/sample.sql"
#	VERBATIM)
