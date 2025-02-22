# Copyright (c) 2021, Leon De Andrade
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this
#    list of conditions and the following disclaimer in the documentation and/or
#    other materials provided with the distribution.
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

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

function(add_component)
    set(options)
    set(oneValueArgs NAME)
    set(multiValueArgs DEPENDENCIES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_library(sqlpp23_${ARG_NAME} INTERFACE)
    add_library(sqlpp23::${ARG_NAME} ALIAS sqlpp23_${ARG_NAME})
    set_target_properties(sqlpp23_${ARG_NAME} PROPERTIES EXPORT_NAME ${ARG_NAME})
    target_link_libraries(sqlpp23_${ARG_NAME} INTERFACE sqlpp23 ${ARG_DEPENDENCIES})
endfunction()

function(install_component)
    set(options)
    set(oneValueArgs NAME DIRECTORY)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    install(FILES ${PROJECT_SOURCE_DIR}/cmake/configs/${ARG_NAME}Config.cmake
        DESTINATION ${SQLPP11_INSTALL_CMAKEDIR}
    )

    install(TARGETS ${ARG_TARGETS}
        EXPORT Sqlpp23Targets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT Sqlpp23Targets
        DESTINATION ${SQLPP11_INSTALL_CMAKEDIR}
        NAMESPACE   sqlpp23::
    )

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/sqlpp23/${ARG_DIRECTORY}
        DESTINATION include/sqlpp23
        FILES_MATCHING
        PATTERN *.h
    )
endfunction()
