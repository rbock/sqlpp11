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

if(DEFINED MSVC)
    set(SEARCH_PATHS
        "$ENV{ProgramFiles}/MySQL/MySQL Server 8.0"
        "$ENV{ProgramFiles}/MySQL/MySQL Server 5.7"
        "$ENV{ProgramFiles}/MySQL/MySQL Server 5.6"
        "$ENV{ProgramFiles\(x86\)}/MySQL/MySQL Server 8.0"
        "$ENV{ProgramFiles\(x86\)}/MySQL/MySQL Server 5.7"
        "$ENV{ProgramFiles\(x86\)}/MySQL/MySQL Server 5.6"
    )
    find_path(MySQL_INCLUDE_DIR
        NAMES mysql_version.h
        PATHS ${SEARCH_PATHS}
        PATH_SUFFIXES include
    )
    find_library(MySQL_LIBRARY
        NAMES libmysql
        PATHS ${SEARCH_PATHS}
        PATH_SUFFIXES lib
    )
else()
    find_path(MySQL_INCLUDE_DIR
        NAMES mysql_version.h
        PATH_SUFFIXES mysql
    )
    find_library(MySQL_LIBRARY
        NAMES mysqlclient mysqlclient_r
        PATH_SUFFIXES mysql  # for CentOS 7
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    MySQL
    MySQL_INCLUDE_DIR
    MySQL_LIBRARY
)

if(MySQL_FOUND AND NOT TARGET MySQL::MySQL)
    add_library(MySQL::MySQL UNKNOWN IMPORTED)
    target_include_directories(MySQL::MySQL INTERFACE "${MySQL_INCLUDE_DIR}")
    set_target_properties(MySQL::MySQL PROPERTIES
        IMPORTED_LOCATION "${MySQL_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LANGUAGES "C")
endif()

mark_as_advanced(MySQL_INCLUDE_DIR MySQL_LIBRARY)