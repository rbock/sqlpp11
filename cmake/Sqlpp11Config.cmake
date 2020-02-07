# Copyright (c) 2016, Christian David
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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

include(CMakeFindDependencyMacro)
find_dependency(HinnantDate REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/Sqlpp11Targets.cmake")

# Import "ddl2cpp" script
if(NOT TARGET sqlpp11::ddl2cpp)
  get_filename_component(sqlpp11_ddl2cpp_location "${CMAKE_CURRENT_LIST_DIR}/../../../bin/sqlpp11-ddl2cpp" REALPATH)
  if(NOT EXISTS "${sqlpp11_ddl2cpp_location}")
    message(FATAL_ERROR "The imported target sqlpp11::ddl2cpp references the file '${sqlpp11_ddl2cpp_location}' but this file does not exists.")
  endif()
  add_executable(sqlpp11::ddl2cpp IMPORTED)
  set_target_properties(sqlpp11::ddl2cpp PROPERTIES
    IMPORTED_LOCATION "${sqlpp11_ddl2cpp_location}"
  )
  unset(sqlpp11_ddl2cpp_location)
endif()
