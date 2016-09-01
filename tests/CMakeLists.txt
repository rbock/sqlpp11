# Copyright (c) 2013-2016, Roland Bock
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

add_library(sqlpp11_testing INTERFACE)
target_include_directories(sqlpp11_testing INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT MSVC)
target_compile_options(sqlpp11_testing INTERFACE -Wall -Wextra -pedantic)
endif ()

set(test_names
  BooleanExpression
  CustomQuery
  DateTime
  Interpret
  Insert
  Remove
  Update
  Select
  SelectType
  Function
  Prepared
  Minimalistic
  Result
  Union
  With
  )

create_test_sourcelist(test_sources test_main.cpp ${test_names})
add_executable(sqlpp11_tests ${test_sources})
target_link_libraries(sqlpp11_tests PRIVATE sqlpp11 sqlpp11_testing)

foreach(test IN LISTS test_names)
  add_test(NAME sqlpp11.tests.${test}
    COMMAND sqlpp11_tests ${test}
    )
endforeach()

# if you want to use the generator, you can do something like this:
#find_package(PythonInterp REQUIRED)
#add_custom_command(
#	OUTPUT "${CMAKE_CURRENT_LIST_DIR}/Sample.h"
#	COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/ddl2cpp" "${CMAKE_CURRENT_LIST_DIR}/sample.sql" Sample test
#	DEPENDS "${CMAKE_CURRENT_LIST_DIR}/sample.sql"
#	VERBATIM)
