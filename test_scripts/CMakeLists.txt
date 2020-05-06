# Copyright (c) 2013-2016, Roland Bock, Alexey Elymanov
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
#   Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
#   Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include(FindPython3)

if (${Python3_Interpreter_FOUND})
  execute_process(
          COMMAND ${Python3_EXECUTABLE} -c "import pyparsing"
	                RESULT_VARIABLE PythonRESULT
									OUTPUT_VARIABLE PythonOUTPUT
          ERROR_VARIABLE PythonERROR
  )

	if (${PythonRESULT})
    message(WARNING "Pyparsing is not installed. Disabling ddl2cpp tests")

	else()
    message(STATUS "Pyparsing is installed: Enabling ddl2cpp tests.")

		add_test(NAME sqlpp11.test.ddl2cpp.bad_will_fail
		         COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/../scripts/ddl2cpp" -fail-on-parse
						         "${CMAKE_CURRENT_LIST_DIR}/ddl2cpp_sample_bad.sql"
										 "${CMAKE_CURRENT_BINARY_DIR}/fail"
										 test)
		set_tests_properties(sqlpp11.test.ddl2cpp.bad_will_fail PROPERTIES WILL_FAIL 1)

		add_test(NAME sqlpp11.test.ddl2cpp.bad_has_parse_error
		         COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/../scripts/ddl2cpp" -fail-on-parse
						         "${CMAKE_CURRENT_LIST_DIR}/ddl2cpp_sample_bad.sql"
										 "${CMAKE_CURRENT_BINARY_DIR}/fail"
										 test)
		set_tests_properties(sqlpp11.test.ddl2cpp.bad_has_parse_error PROPERTIES PASS_REGULAR_EXPRESSION "Parsing error,.*")

		add_test(NAME sqlpp11.test.ddl2cpp.good_succeeds
		         COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/../scripts/ddl2cpp" -fail-on-parse
						         "${CMAKE_CURRENT_LIST_DIR}/ddl2cpp_sample_good.sql"
										 "${CMAKE_CURRENT_BINARY_DIR}/fail"
										 test)

    foreach(sample_name sample sample_identity_naming)
      set(sqlpp.test.generated.sample.include "${CMAKE_CURRENT_BINARY_DIR}/${sample_name}")
      include_directories(${CMAKE_CURRENT_BINARY_DIR})
      set(use_identity_naming)
      if(sample_name STREQUAL "sample_identity_naming")
        set(use_identity_naming -identity-naming)
      endif()
      add_custom_command(
          OUTPUT "${sqlpp.test.generated.sample.include}.h"
          COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/../scripts/ddl2cpp"
                  ${use_identity_naming}
                  "${CMAKE_CURRENT_LIST_DIR}/ddl2cpp_sample_good.sql"
                  "${sqlpp.test.generated.sample.include}"
                  test
          DEPENDS "${CMAKE_CURRENT_LIST_DIR}/ddl2cpp_sample_good.sql"
          VERBATIM)

      add_executable(sqlpp.test.compiled.${sample_name} ${sample_name}.cpp "${sqlpp.test.generated.sample.include}.h")
      target_link_libraries(sqlpp.test.compiled.${sample_name} PRIVATE sqlpp11)
    endforeach()

  endif()
endif()

