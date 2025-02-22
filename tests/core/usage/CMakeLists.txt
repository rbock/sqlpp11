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

set(test_files
    CustomQuery.cpp
    DateTime.cpp
    DateTimeParser.cpp
    Interpret.cpp
    Insert.cpp
    Remove.cpp
    Update.cpp
    Select.cpp
    SelectType.cpp
    Function.cpp
    Prepared.cpp
    Minimalistic.cpp
    Result.cpp
    Union.cpp
    With.cpp
)

create_test_sourcelist(test_sources test_main.cpp ${test_files})
add_executable(sqlpp23_core_tests ${test_sources})
target_link_libraries(sqlpp23_core_tests PRIVATE sqlpp23::sqlpp23 sqlpp23_testing)

foreach(test_file IN LISTS test_files)
    get_filename_component(test ${test_file} NAME_WLE)
    add_test(NAME sqlpp23.core.usage.${test}
        COMMAND sqlpp23_core_tests ${test}
    )
endforeach()

