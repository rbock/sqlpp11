# FindMySQL.cmake

if(DEFINED MSVC)
        find_path(MySQL_INCLUDE_DIR
            NAMES mariadb_version.h
            PATH_SUFFIXES include
        )
        find_library(MySQL_LIBRARY
            NAMES libmariadb
            PATH_SUFFIXES lib
        )
else()
        find_path(MySQL_INCLUDE_DIR
            NAMES mariadb_version.h
            PATH_SUFFIXES mariadb mysql
        )
        find_library(MySQL_LIBRARY NAMES mariadb)
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
