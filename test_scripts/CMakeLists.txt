# Copyright (c) 2013-2016, Roland Bock, Alexey Elymanov
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


include(FindPythonInterp)
if (${PYTHONINTERP_FOUND})
  message( STATUS "Python found: " ${PYTHON_EXECUTABLE} )

  execute_process(
          COMMAND ${PYTHON_EXECUTABLE} -c "import pyparsing"
          RESULT_VARIABLE PythonRESULT
          OUTPUT_VARIABLE PythonOUTPUT
          ERROR_VARIABLE PythonERROR
  )
  if (${PythonRESULT})
    message( STATUS "Pyparsing is not installed." ${PythonRESULT} )
  else()
    message( STATUS "Pyparsing is installed." )
    message( STATUS "Enabling sql2ddl tests.")
    set(test_scripts_names ddl2cpp)


    create_test_sourcelist(test_scripts_sources test_scripts_main.cpp ${test_scripts_names})
    add_executable(sqlpp11_test_scripts ${test_scripts_sources})
    target_link_libraries(sqlpp11_test_scripts PRIVATE sqlpp11 sqlpp11_testing)

    foreach(test_scripts IN LISTS test_scripts_names)
      add_test(NAME sqlpp11.test_scripts.${test_scripts}
              COMMAND sqlpp11_test_scripts ${test_scripts}
              )
    endforeach()

  endif()
  #MESSAGE( STATUS "Python found:" ${PythonRESULT} )
  #MESSAGE( STATUS "Python found:" ${PythonOUTPUT} )
  #MESSAGE( STATUS "Python found:" ${PythonERROR} )
ENDIF()



