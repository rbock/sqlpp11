include_directories(${CMAKE_BINARY_DIR}/tests)

add_executable(
	no_conversion_operator_if_null_not_trivial
	EXCLUDE_FROM_ALL
	no_conversion_operator_if_null_not_trivial.cpp
	)

add_custom_command(OUTPUT no_conversion_operator_if_null_not_trivial.out
      COMMAND ${CMAKE_MAKE_PROGRAM} no_conversion_operator_if_null_not_trivial > ${CMAKE_CURRENT_BINARY_DIR}/no_conversion_operator_if_null_not_trivial.out 2>&1  || true
			COMMAND grep "int i = row.alpha" ${CMAKE_CURRENT_BINARY_DIR}/no_conversion_operator_if_null_not_trivial.out > /dev/null
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/no_conversion_operator_if_null_not_trivial.cpp
			COMMENT "no_conversion_operator_if_null_not_trivial"
			)

add_custom_target(test_constraints DEPENDS no_conversion_operator_if_null_not_trivial.out COMMAND true)

