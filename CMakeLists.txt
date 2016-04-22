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

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(sqlpp11 VERSION 0.1 LANGUAGES CXX)

enable_testing()

add_library(sqlpp11 INTERFACE)

set(DATE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../date" CACHE FILEPATH "Path to Howard Hinnant's date library")

if(NOT EXISTS ${DATE_INCLUDE_DIR}/date.h)
	message(SEND_ERROR "Can't find file date.h and cannot compile date_test/date_test.cpp")
	message("Can't find date.h in ${DATE_INCLUDE_DIR} ")
	message("Please either")
	message("  - git clone https://github.com/howardhinnant/date ${DATE_INCLUDE_DIR}")
	message("  - download and unzip a current version from https://github.com/howardhinnant/date to ${DATE_INCLUDE_DIR}")
	message("  - set DATE_INCLUDE_DIR to point to the dir containing date.h from the date library")
	message("")
endif()

target_include_directories(sqlpp11 INTERFACE
  $<BUILD_INTERFACE:${DATE_INCLUDE_DIR}>
  $<BUILD_INTERFACE:${sqlpp11_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )

if (NOT DEFINED MSVC)
target_compile_features(sqlpp11 INTERFACE
	cxx_alias_templates
	cxx_auto_type
	cxx_constexpr
	cxx_decltype
	cxx_defaulted_functions
	cxx_defaulted_move_initializers
	cxx_deleted_functions
	cxx_explicit_conversions
	cxx_lambdas
	cxx_noexcept
	cxx_nullptr
	cxx_range_for
	cxx_right_angle_brackets
	cxx_rvalue_references
	cxx_static_assert
	cxx_trailing_return_types
	cxx_uniform_initialization
	cxx_template_template_parameters
  cxx_variadic_templates
  )
endif ()

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/sqlpp11"
        DESTINATION include
)

install(TARGETS sqlpp11
        EXPORT Sqlpp11Targets
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp11ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT Sqlpp11Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp11Targets.cmake"
)

configure_file(cmake/Sqlpp11Config.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp11Config.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/Sqlpp11)
install(EXPORT Sqlpp11Targets
  FILE
    Sqlpp11Targets.cmake
  DESTINATION
    ${ConfigPackageLocation}
)

install(
  FILES
    cmake/Sqlpp11Config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp11ConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
)


add_subdirectory(tests)
add_subdirectory(test_types)
add_subdirectory(test_serializer)
add_subdirectory(test_static_asserts)
add_subdirectory(test_constraints)
add_subdirectory(examples)
