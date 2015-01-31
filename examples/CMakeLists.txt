
macro (build arg)
	# Add headers to sources to enable file browsing in IDEs
	include_directories("${CMAKE_SOURCE_DIR}/tests")
	add_executable("Sqlpp11Example${arg}" "${arg}.cpp" ${sqlpp_headers} "${CMAKE_SOURCE_DIR}/tests/MockDb.h" "${CMAKE_CURRENT_LIST_DIR}/Sample.h")
	add_test("${arg}" "${arg}")
endmacro ()

#build(sample)
build(insert)
build(update)
build(remove)
build(select)
find_package(Boost 1.50)
if(Boost_FOUND)
  MESSAGE(${Boost_INCLUDE_DIRS})
  include_directories(${Boost_INCLUDE_DIRS})
  build(ppgen)
endif()


#find_package(PythonInterp REQUIRED)

#add_custom_command(
#	OUTPUT ${CMAKE_CURRENT_LIST_DIR}/Sample.h 
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/ddl2cpp ${CMAKE_CURRENT_LIST_DIR}/sample.sql Sample test
#	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/sample.sql
#	)

