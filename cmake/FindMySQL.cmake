# FindMySQL.cmake

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
        find_library(MySQL_LIBRARY NAMES mysqlclient mysqlclient_r)
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
