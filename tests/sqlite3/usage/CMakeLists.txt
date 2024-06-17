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

add_library(sqlpp11_sqlite3_testing INTERFACE)
target_include_directories(sqlpp11_sqlite3_testing INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set(test_files
    DateTime.cpp
    Sample.cpp
    Select.cpp
    Union.cpp
    With.cpp
    Attach.cpp
    DynamicSelect.cpp
    AutoIncrement.cpp
    Transaction.cpp
    FloatingPoint.cpp
    Integral.cpp
    Blob.cpp
    Connection.cpp
    ConnectionPool.cpp
    Execute.cpp
)

create_test_sourcelist(test_sources test_main.cpp ${test_files})

add_executable(sqlpp11_sqlite3_tests ${test_sources})
target_link_libraries(sqlpp11_sqlite3_tests PRIVATE sqlpp11_testing sqlpp11_sqlite3_testing)
if (BUILD_SQLCIPHER_CONNECTOR)
    target_link_libraries(sqlpp11_sqlite3_tests PRIVATE sqlpp11::sqlcipher)
    if (SQLPP_DYNAMIC_LOADING)
        target_include_directories(sqlpp11_sqlite3_tests PRIVATE ${SQLCIPHER_INCLUDE_DIRS})
    endif()
else()
    target_link_libraries(sqlpp11_sqlite3_tests PRIVATE sqlpp11::sqlite3)
endif()

# conditionally bump to a higher C++ standard to test compatibility
if (SQLPP11_TESTS_CXX_STD)
    set_property(TARGET sqlpp11_sqlite3_tests PROPERTY CXX_STANDARD ${SQLPP11_TESTS_CXX_STD})
    set_property(TARGET sqlpp11_sqlite3_tests PROPERTY CXX_STANDARD_REQUIRED ON)
    set_property(TARGET sqlpp11_sqlite3_tests PROPERTY CXX_EXTENSIONS OFF)
endif()

foreach(test_file IN LISTS test_files)
    get_filename_component(test ${test_file} NAME_WLE)
    add_test(NAME sqlpp11.sqlite3.usage.${test}
        COMMAND sqlpp11_sqlite3_tests ${test}
    )
endforeach()

# the dynamic loading test needs the extra option "SQLPP_DYNAMIC_LOADING" and does NOT link the sqlite libs
if (SQLPP_DYNAMIC_LOADING)
    add_executable(Sqlpp11Sqlite3DynamicLoadingTest "DynamicLoadingTest.cpp" ${sqlpp_headers})
    target_link_libraries(Sqlpp11Sqlite3DynamicLoadingTest sqlpp11-connector-sqlite3-dynamic)
    if (NOT MSVC)
        target_link_libraries(Sqlpp11Sqlite3DynamicLoadingTest dl)
        target_compile_options(Sqlpp11Sqlite3DynamicLoadingTest INTERFACE -Wall -Wextra -pedantic)
    endif ()
    add_test(NAME Sqlpp11Sqlite3DynamicLoadingTest COMMAND Sqlpp11Sqlite3DynamicLoadingTest)
endif()
