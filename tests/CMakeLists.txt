macro (build_and_run arg)
	# Add headers to sources to enable file browsing in IDEs
	add_executable(${arg} ${arg}.cpp ${sqlpp_headers})
	add_test(${arg} ${arg})
endmacro ()

build_and_run(InsertTest)
build_and_run(RemoveTest)
build_and_run(UpdateTest)
build_and_run(SelectTest)
build_and_run(FunctionTest)

