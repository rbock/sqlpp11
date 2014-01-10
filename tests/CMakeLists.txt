macro (build_and_run arg)
	add_executable(${arg} ${arg}.cpp)
	add_test(${arg} ${arg})
endmacro ()

build_and_run(InterpretTest)
#build_and_run(InsertTest)
#build_and_run(RemoveTest)
#build_and_run(UpdateTest)
#build_and_run(SelectTest)
#build_and_run(FunctionTest)
#build_and_run(PreparedTest)

