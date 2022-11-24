# Copyright (c) 2013-2015, Roland Bock
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

function(test_constraint name pattern)
    set(test sqlpp11.core.constraints.${name})
    set(target sqlpp11_${name})
    add_executable(${target} EXCLUDE_FROM_ALL ${name}.cpp)
    target_link_libraries(${target} PRIVATE sqlpp11::sqlpp11 sqlpp11_testing)
    add_test(NAME ${test}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${target}
    )
    set_property(TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION ${pattern})
    # conditionally bump to a higher C++ standard to test compatibility
    if (SQLPP11_TESTS_CXX_STD)
        set_property(TARGET ${target} PROPERTY CXX_STANDARD ${SQLPP11_TESTS_CXX_STD})
        set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED yes)
        set_property(TARGET ${target} PROPERTY CXX_EXTENSIONS no)
    endif()
endfunction()

# Compiling these is required to fail (testing some static_assert)
test_constraint(count_of_count "count\\(\\) cannot be used on an aggregate function")
test_constraint(max_of_max "max\\(\\) cannot be used on an aggregate function")
test_constraint(require_insert "required column is missing")
test_constraint(must_not_insert "one assignment is prohibited")
test_constraint(must_not_update "one assignment is prohibited")