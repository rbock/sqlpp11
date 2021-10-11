# Copyright (c) 2013 - 2021, Roland Bock
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

find_package(Threads REQUIRED)

add_library(sqlpp11_mysql_testing INTERFACE)
target_include_directories(sqlpp11_mysql_testing INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set(test_names
Json
CustomQuery
DateTime
Sample
Select
Union
DynamicSelect
MoveConstructor
Prepared
Truncated
Update
Remove
  )

create_test_sourcelist(test_sources test_main.cpp ${test_names})
add_executable(sqlpp11_mysql_tests ${test_sources})
target_link_libraries(sqlpp11_mysql_tests PRIVATE sqlpp11 sqlpp11_testing sqlpp11_mysql_testing)
target_link_libraries(sqlpp11_mysql_tests PRIVATE sqlpp11::sqlpp11)
target_link_libraries(sqlpp11_mysql_tests PRIVATE Threads::Threads)
target_link_libraries(sqlpp11_mysql_tests PRIVATE MySQL::MySQL)
target_link_libraries(sqlpp11_mysql_tests PRIVATE date::date)
if(NOT MSVC)
    target_compile_options(sqlpp11_mysql_tests PRIVATE -Wall -Wextra -pedantic)
endif()

# conditionally bump to a higher C++ standard to test compatibility
if (SQLPP11_TESTS_CXX_STD)
set_property(TARGET sqlpp11_mysql_tests PROPERTY CXX_STANDARD ${SQLPP11_TESTS_CXX_STD})
set_property(TARGET sqlpp11_mysql_tests PROPERTY CXX_STANDARD_REQUIRED yes)
  set_property(TARGET sqlpp11_mysql_tests PROPERTY CXX_EXTENSIONS no)
endif()

foreach(test IN LISTS test_names)
  add_test(NAME sqlpp11.mysql.tests.${test}
    COMMAND sqlpp11_mysql_tests ${test}
    )
endforeach()

