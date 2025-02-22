# Copyright (c) 2021-2021, Roland Bock
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this
#    list of conditions and the following disclaimer in the documentation and/or
#    other materials provided with the distribution.
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

add_library(sqlpp23_sqlite3_testing INTERFACE)
target_include_directories(sqlpp23_sqlite3_testing INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

function(sqlpp_slite3_test_add_target_link_libraries name)
target_link_libraries(${name} PRIVATE sqlpp23_testing sqlpp23_sqlite3_testing)
if (BUILD_SQLCIPHER_CONNECTOR)
    target_link_libraries(${name} PRIVATE sqlpp23::sqlcipher)
    if (SQLPP_DYNAMIC_LOADING)
        target_include_directories(${name} PRIVATE ${SQLCIPHER_INCLUDE_DIRS})
    endif()
else()
    target_link_libraries(${name} PRIVATE sqlpp23::sqlite3)
endif()
endfunction()

add_subdirectory(constraints)
add_subdirectory(types)
add_subdirectory(serialize)
add_subdirectory(usage)
