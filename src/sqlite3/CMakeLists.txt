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

add_library(sqlpp11-connector-sqlite3)
add_library(sqlpp11::sqlite3 ALIAS sqlpp11-connector-sqlite3)

target_sources(sqlpp11-connector-sqlite3 
    PRIVATE 
        connection.cpp
		bind_result.cpp
		prepared_statement.cpp
        detail/connection_handle.cpp
)
target_link_libraries(sqlpp11-connector-sqlite3 PUBLIC sqlpp11::sqlpp11)

if (SQLPP_DYNAMIC_LOADING)
    add_library(sqlpp11-connector-sqlite3-dynamic
                    connection.cpp
                    bind_result.cpp
                    prepared_statement.cpp
                    detail/connection_handle.cpp
                    detail/dynamic_libsqlite3.cpp
        )
    add_library(sqlpp11::sqlite3-dynamic ALIAS sqlpp11-connector-sqlite3-dynamic)

    get_filename_component(SQLite3_LIB_FILE ${SQLite3_LIBRARY} NAME)

    if (WIN32)
        # the import library (.lib) needs to be replaced by .dll
        get_filename_component(SQLite3_LIB_FILE ${SQLite3_LIB_FILE} NAME_WE)
        string(APPEND SQLite3_LIB_FILE ".dll")
    endif()

    set_target_properties(sqlpp11-connector-sqlite3 PROPERTIES DEFINE_SYMBOL SQLPP11_COMPILING_DLL)
    set_target_properties(sqlpp11-connector-sqlite3-dynamic PROPERTIES DEFINE_SYMBOL SQLPP11_COMPILING_DLL)

    target_compile_definitions(sqlpp11-connector-sqlite3-dynamic
                               PUBLIC SQLPP_DYNAMIC_LOADING
                               PRIVATE SQLPP_DYNAMIC_LOADING_FILENAME=${SQLite3_LIB_FILE})

    target_include_directories(sqlpp11-connector-sqlite3-dynamic PUBLIC
                               $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                               $<INSTALL_INTERFACE:include>)

    target_link_libraries(sqlpp11-connector-sqlite3-dynamic PUBLIC sqlpp11::sqlpp11)
endif()

target_include_directories(sqlpp11-connector-sqlite3 PUBLIC
                           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)

if (SQLCIPHER)
    target_compile_definitions(sqlpp11-connector-sqlite3 PUBLIC SQLPP_USE_SQLCIPHER)
    target_link_libraries(sqlpp11-connector-sqlite3 PUBLIC SQLCipher::SQLCipher)
    if (SQLPP_DYNAMIC_LOADING)
        target_compile_definitions(sqlpp11-connector-sqlite3-dynamic PUBLIC SQLPP_USE_SQLCIPHER)
        target_include_directories(sqlpp11-connector-sqlite3-dynamic PUBLIC ${SQLCIPHER_INCLUDE_DIRS})
    endif()
else()
    target_link_libraries(sqlpp11-connector-sqlite3 PUBLIC SQLite::SQLite3)
endif()

install(TARGETS sqlpp11-connector-sqlite3
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

if (SQLPP_DYNAMIC_LOADING)
    install(TARGETS sqlpp11-connector-sqlite3-dynamic
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)
endif()
