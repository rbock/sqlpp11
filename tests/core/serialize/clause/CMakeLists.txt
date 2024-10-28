# Copyright (c) 2024, Roland Bock
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

function(create_test name)
    set(target sqlpp11_core_serialize_clause_${name})
    add_executable(${target} ${name}.cpp)
    target_link_libraries(${target} PRIVATE sqlpp11::sqlpp11 sqlpp11_testing)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

create_test(cte)
create_test(delete_from)
create_test(group_by)
create_test(for_update)
create_test(from)
create_test(having)
create_test(insert)
create_test(insert_columns)
create_test(insert_default_values)
create_test(insert_set)
create_test(limit)
create_test(offset)
create_test(order_by)
create_test(select)
create_test(select_as)
create_test(select_columns)
create_test(select_flags)
create_test(single_table)
create_test(union)
create_test(update)
create_test(update_set)
create_test(where)
create_test(with)

