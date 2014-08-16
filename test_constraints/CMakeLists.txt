include_directories(${CMAKE_BINARY_DIR}/tests)

add_custom_target(test_sqlpp_constraints COMMAND true)

function(test_constraint name pattern)

	add_executable(
		${name}
		EXCLUDE_FROM_ALL
		${name}.cpp
		)

	add_custom_command(OUTPUT ${name}.out
				COMMAND ${CMAKE_MAKE_PROGRAM} ${name} > ${CMAKE_CURRENT_BINARY_DIR}/${name}.out 2>&1  || true
				COMMAND grep ${pattern} ${CMAKE_CURRENT_BINARY_DIR}/${name}.out > /dev/null
				DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp
				COMMENT "${name}"
				)

	add_custom_target(test_${name} DEPENDS ${name}.out COMMAND true)


	add_dependencies(test_sqlpp_constraints test_${name})

endfunction(testconstraint)


test_constraint(no_conversion_operator_if_null_not_trivial "int i = row.alpha")
test_constraint(require_insert "required column is missing")
test_constraint(must_not_insert "one assignment is prohibited")

