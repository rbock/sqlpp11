# Copyright (c) 2023, Vesselin Atanasov
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

cmake_minimum_required(VERSION 3.14)
set (APP_NAME "connection_pool")
project ("${APP_NAME}" CXX)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED true)
set (CMAKE_CXX_EXTENSIONS false)

# Executable file and its build settings
add_executable ("${APP_NAME}")
target_include_directories ("${APP_NAME}" PRIVATE "${GEN_HEADERS_DIR}" "${PROJECT_SOURCE_DIR}/src")

# Linked libraries
find_package (Sqlpp11 REQUIRED COMPONENTS PostgreSQL)
target_link_libraries ("${APP_NAME}" PRIVATE sqlpp11::postgresql)

# Project sources
target_sources (
	"${APP_NAME}" PRIVATE
	"src/db_connection.h"
	"src/db_connection.cpp"
	"src/db_global.h"
	"src/db_global.cpp"
	"src/main.cpp"
)
