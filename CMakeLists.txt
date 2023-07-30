# Copyright (c) 2013-2021, Roland Bock
# Copyright (c) 2016 Christian Dávid
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

### Preamble
cmake_minimum_required(VERSION 3.14)
project(sqlpp11 VERSION 0.1 LANGUAGES CXX)

### Project Wide Setup
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include(GNUInstallDirs)
include(CTest)

option(BUILD_MYSQL_CONNECTOR "Build MySQL Connector" OFF)
option(BUILD_MARIADB_CONNECTOR "Build MariaDB Connector" OFF)
option(BUILD_POSTGRESQL_CONNECTOR "Build PostgreSQL Connector" OFF)
option(BUILD_SQLITE3_CONNECTOR "Build SQLite3 Connector" OFF)
option(BUILD_SQLCIPHER_CONNECTOR "Build SQLite3 Connector with SQLCipher" OFF)

option(DEPENDENCY_CHECK "Check for dependencies of connector and the library" ON)

option(USE_SYSTEM_DATE "\
    Use find_package to find installed HowardHinnant's \
    date library instead of fetching it from github" OFF
)

set(SQLPP11_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Sqlpp11 CACHE STRING "Path to sqlpp11 cmake files")

### Dependencies
if(DEPENDENCY_CHECK AND BUILD_MYSQL_CONNECTOR)
    find_package(MySQL REQUIRED)
endif()

if(DEPENDENCY_CHECK AND BUILD_MARIADB_CONNECTOR)
    find_package(MariaDB REQUIRED)
endif()

if(DEPENDENCY_CHECK AND BUILD_POSTGRESQL_CONNECTOR)
    find_package(PostgreSQL REQUIRED)
endif()

if(DEPENDENCY_CHECK AND BUILD_SQLITE3_CONNECTOR)
    find_package(SQLite3 REQUIRED)
endif()

if(DEPENDENCY_CHECK AND BUILD_SQLCIPHER_CONNECTOR)
    find_package(SQLCipher REQUIRED)
endif()

if(DEPENDENCY_CHECK AND USE_SYSTEM_DATE)
    find_package(date REQUIRED)
endif()

add_subdirectory(dependencies)

### Core targets
include(Sqlpp11TargetHelper)

add_library(sqlpp11 INTERFACE)
add_library(sqlpp11::sqlpp11 ALIAS sqlpp11)

target_link_libraries(sqlpp11 INTERFACE date::date)
target_include_directories(sqlpp11 INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_compile_features(sqlpp11 INTERFACE cxx_std_11)


if(BUILD_SQLITE3_CONNECTOR)
    add_component(NAME sqlite3 DEPENDENCIES SQLite::SQLite3)
endif()

if(BUILD_SQLCIPHER_CONNECTOR)
    add_component(NAME sqlcipher DEPENDENCIES SQLCipher::SQLCipher)
    target_compile_definitions(sqlpp11_sqlcipher INTERFACE SQLPP_USE_SQLCIPHER)
endif()

if(BUILD_MYSQL_CONNECTOR)
    add_component(NAME mysql DEPENDENCIES MySQL::MySQL)
endif()

if(BUILD_MARIADB_CONNECTOR)
    add_component(NAME mariadb DEPENDENCIES MariaDB::MariaDB)
endif()

if(BUILD_POSTGRESQL_CONNECTOR)
    add_component(NAME postgresql DEPENDENCIES PostgreSQL::PostgreSQL)
endif()

### Packaging
install(PROGRAMS ${PROJECT_SOURCE_DIR}/scripts/ddl2cpp
    RENAME sqlpp11-ddl2cpp
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

write_basic_package_version_file(Sqlpp11ConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Sqlpp11ConfigVersion.cmake
    DESTINATION ${SQLPP11_INSTALL_CMAKEDIR}
)

install_component(NAME Sqlpp11 TARGETS sqlpp11 DIRECTORY)

if(BUILD_SQLITE3_CONNECTOR)
    install_component(NAME Sqlpp11SQLite3 TARGETS sqlpp11_sqlite3 DIRECTORY sqlite3)
endif()

if(BUILD_SQLCIPHER_CONNECTOR)
    install_component(NAME Sqlpp11SQLCipher TARGETS sqlpp11_sqlcipher DIRECTORY sqlite3)

    install(FILES ${PROJECT_SOURCE_DIR}/cmake/modules/FindSQLCipher.cmake
        DESTINATION ${SQLPP11_INSTALL_CMAKEDIR}
    )
endif()

if(BUILD_MYSQL_CONNECTOR)
    install_component(NAME Sqlpp11MySQL TARGETS sqlpp11_mysql DIRECTORY mysql)

    install(FILES ${PROJECT_SOURCE_DIR}/cmake/modules/FindMySQL.cmake
        DESTINATION ${SQLPP11_INSTALL_CMAKEDIR}
    )
endif()

if(BUILD_MARIADB_CONNECTOR)
    install_component(NAME Sqlpp11MariaDB TARGETS sqlpp11_mariadb DIRECTORY mysql)

    install(FILES ${PROJECT_SOURCE_DIR}/cmake/modules/FindMariaDB.cmake
        DESTINATION ${SQLPP11_INSTALL_CMAKEDIR}
    )
endif()

if(BUILD_POSTGRESQL_CONNECTOR)
    install_component(NAME Sqlpp11PostgreSQL TARGETS sqlpp11_postgresql DIRECTORY postgresql)
endif()


### Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
