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
        "$ENV{ProgramFiles}/MariaDB/*"
        "$ENV{ProgramFiles\(x86\)}/MariaDB/*"
    )
    find_path(MariaDB_INCLUDE_DIR
        NAMES mariadb_version.h
        PATHS ${SEARCH_PATHS}
        PATH_SUFFIXES include
    )
    find_library(MariaDB_LIBRARY
        NAMES libmariadb
        PATHS ${SEARCH_PATHS}
        PATH_SUFFIXES lib
    )
else()
    find_path(MariaDB_INCLUDE_DIR
        NAMES mariadb_version.h
        PATH_SUFFIXES mariadb mysql
    )
    find_library(MariaDB_LIBRARY 
        NAMES mariadb
        PATH_SUFFIXES mariadb mysql
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    MariaDB
    MariaDB_INCLUDE_DIR
    MariaDB_LIBRARY
)

if(MariaDB_FOUND AND NOT TARGET MariaDB::MariaDB)
    add_library(MariaDB::MariaDB UNKNOWN IMPORTED)
    target_include_directories(MariaDB::MariaDB INTERFACE "${MariaDB_INCLUDE_DIR}")
    set_target_properties(MariaDB::MariaDB PROPERTIES
        IMPORTED_LOCATION "${MariaDB_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LANGUAGES "C")
endif()

mark_as_advanced(MariaDB_INCLUDE_DIR MariaDB_LIBRARY)
