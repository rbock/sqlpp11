set(PUBLIC_HEADERS_DIR "${PROJECT_SOURCE_DIR}/include/")

add_library(sqlpp-mysql
    bind_result.cpp
    char_result.cpp
    connection.cpp
    prepared_statement.cpp
    detail/connection_handle.cpp
)

target_include_directories(sqlpp-mysql PUBLIC
    $<BUILD_INTERFACE:${PUBLIC_HEADERS_DIR}>
)

find_package(date REQUIRED)
find_package(Sqlpp11 REQUIRED)

target_link_libraries(sqlpp-mysql PUBLIC sqlpp11::sqlpp11)
target_link_libraries(sqlpp-mysql PRIVATE MySQL::MySQL)
target_link_libraries(sqlpp-mysql PRIVATE date::date)

# Installation

include(GNUInstallDirs)

add_library(sqlpp11::mysql ALIAS sqlpp-mysql)
set_target_properties(sqlpp-mysql PROPERTIES EXPORT_NAME sqlpp11::mysql)

install(
    TARGETS sqlpp-mysql
    EXPORT sqlpp-mysql-targets
    COMPONENT sqlpp11::mysql
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(
    DIRECTORY ${PUBLIC_HEADERS_DIR}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT sqlpp11::mysql
)

include(CMakePackageConfigHelpers)

set(cmake_config_path "${CMAKE_INSTALL_LIBDIR}/cmake/sqlpp-mysql")

configure_package_config_file(
    sqlpp-mysql-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/sqlpp-mysql-config.cmake"
    INSTALL_DESTINATION "${cmake_config_path}"
)

write_basic_package_version_file(
    sqlpp-mysql-config-version.cmake
    VERSION ${VERSION}
    COMPATIBILITY SameMajorVersion
    ${extra_version_file_args}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/sqlpp-mysql-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/sqlpp-mysql-config-version.cmake"
    DESTINATION "${cmake_config_path}"
    COMPONENT sqlpp11::mysql
)

install(FILES
    "${PROJECT_SOURCE_DIR}/cmake/FindMySQL.cmake"
    DESTINATION "${cmake_config_path}"
    COMPONENT sqlpp11::mysql
)

install(
    EXPORT sqlpp-mysql-targets
    FILE sqlpp-mysql-targets.cmake
    DESTINATION "${cmake_config_path}"
    COMPONENT sqlpp11::mysql
)
