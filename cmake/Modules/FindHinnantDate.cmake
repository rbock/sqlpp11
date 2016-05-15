#.rst:
# FindHinnantDate
# ---------------
#
# This module finds Howard Hinnant's date and time library for C++11 and beyond
# See https://github.com/HowardHinnant/date for details.
#
# This will define the following variables::
#
#   HinnantDate_FOUND       - True if the system has the library
#   HinnantDate_INCLUDE_DIR - The directory which includes the header
#   HinnantDate_ROOT_DIR    - The base directory of the library
#
# and the following imported targets::
#
#   HinnantDate::Date   - The target to use date.h
#
# You can set HinnantDate_ROOT_DIR as hint to the location of the library.
#
# The target will enable the required C++11 standard in your compiler. You can
# use any later standard but you have to enable them explicitly.

# Copyright (c) 2016 Christian Dávid
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
#   Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.1)

# ensure cache entry
set(HinnantDate_ROOT_DIR "${HinnantDate_ROOT_DIR}" CACHE PATH "Root directory of Howard Hinnant's date library")
set(HinnantDate_NOT_FOUND_MESSAGE "Could NOT find HinnantDate.
Maybe you need to adjust the search paths or HinnantDate_ROOT_DIR.")

find_file(HinnantDate_INCLUDE_FILE
    date.h
    HINTS ${HinnantDate_ROOT_DIR}
)
mark_as_advanced(HinnantDate_INCLUDE_FILE)

if (HinnantDate_INCLUDE_FILE)
    # Validate that correct file is found
    file(STRINGS ${HinnantDate_INCLUDE_FILE} check_result
        LIMIT_COUNT 1
        REGEX "^ *// Copyright [(]c[)] 2015.* Howard Hinnant *$"
    )

    if("${check_result}" STREQUAL "")
        string(APPEND HinnantDate_NOT_FOUND_MESSAGE "\nRejecting found '${HinnantDate_INCLUDE_FILE}', it seems to be a name twin.")
        unset(HinnantDate_INCLUDE_FILE CACHE)
    else()
        # Check succeeded, create target
        get_filename_component(HinnantDate_INCLUDE_DIR "${HinnantDate_INCLUDE_FILE}" DIRECTORY CACHE)
        mark_as_advanced(HinnantDate_INCLUDE_DIR)
        set(HinnantDate_ROOT_DIR "${HinnantDate_INCLUDE_DIR}")
        unset(HinnantDate_NOT_FOUND_MESSAGE)

        if(NOT TARGET HinnantDate::Date)
            add_library(HinnantDate::Date INTERFACE IMPORTED)
            set_target_properties(HinnantDate::Date PROPERTIES
              INTERFACE_INCLUDE_DIRECTORIES "${HinnantDate_INCLUDE_DIR}"
              INTERFACE_COMPILE_FEATURES "cxx_auto_type;cxx_static_assert;cxx_decltype;cxx_alias_templates;cxx_strong_enums"
# Due to cmake restrictions the standard cannot be set directly to interface imported targets. Instead required compile
# features are set (list maybe incomplete). Please note that this list shall be a minimal set of required features.
#              CXX_STANDARD 11
#              CXX_STANDARD_REQUIRED true
            )
        endif()
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HinnantDate
    REQUIRED_VARS HinnantDate_ROOT_DIR HinnantDate_INCLUDE_DIR
    FAIL_MESSAGE ${HinnantDate_NOT_FOUND_MESSAGE}
)
