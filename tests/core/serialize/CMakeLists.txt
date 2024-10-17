# Copyright (c) 2013-2021, Roland Bock
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
    DynamicWhere.cpp
    Insert.cpp
    Lower.cpp
    Parameter.cpp
    ParameterizedVerbatim.cpp
    SelectAs.cpp
    SelectColumns.cpp
    TableAlias.cpp
    Trim.cpp
    Upper.cpp
)

create_test_sourcelist(test_sources test_serializer_main.cpp ${test_files})
add_executable(sqlpp11_core_serialize ${test_sources})
target_link_libraries(sqlpp11_core_serialize PRIVATE sqlpp11::sqlpp11 sqlpp11_testing)

foreach(test_file IN LISTS test_files)
    get_filename_component(test ${test_file} NAME_WLE)
    add_test(NAME sqlpp11.core.serialize.${test}
        COMMAND sqlpp11_core_serialize ${test}
    )
endforeach()

add_subdirectory(aggregate_function)
add_subdirectory(basic)
add_subdirectory(clause)
add_subdirectory(operator)
add_subdirectory(query)
add_subdirectory(value_types)
