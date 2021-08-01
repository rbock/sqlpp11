# Copyright (c) 2013 - 2016, Roland Bock
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

macro (build_and_run arg)
	# Add headers to sources to enable file browsing in IDEs
    add_executable(Sqlpp11Sqlite3${arg} ${arg}.cpp)

	target_link_libraries(Sqlpp11Sqlite3${arg} PRIVATE sqlpp11-connector-sqlite3)
	if (NOT MSVC)
		target_compile_options(Sqlpp11Sqlite3${arg} INTERFACE -Wall -Wextra -pedantic)
	endif ()
	add_test(NAME Sqlpp11Sqlite3${arg} COMMAND Sqlpp11Sqlite3${arg})
endmacro ()

build_and_run(DateTimeTest)
build_and_run(SampleTest)
build_and_run(SelectTest)
build_and_run(UnionTest)
build_and_run(WithTest)
build_and_run(AttachTest)
build_and_run(DynamicSelectTest)
build_and_run(AutoIncrementTest)
build_and_run(TransactionTest)
build_and_run(FloatingPointTest)
build_and_run(IntegralTest)
build_and_run(BlobTest)

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
