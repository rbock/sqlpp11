#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/parameter_list.h>
#include <sqlpp11/result.h>
#include <sqlpp11/data_types/no_value.h>

namespace sqlpp
{
  template <typename Database, typename Statement, typename Composite = Statement>
  struct prepared_select_t
  {
    using _traits = make_traits<no_value_t, tag::is_prepared_statement>;
    using _nodes = detail::type_vector<>;

    using _result_row_t = typename Statement::template _result_row_t<Database>;
    using _parameter_list_t = make_parameter_list_t<Composite>;
    using _dynamic_names_t = typename Statement::_dynamic_names_t;
    using _prepared_statement_t = typename Database::_prepared_statement_t;

    using _run_check = consistent_t;

    auto _run(Database& db) const -> result_t<decltype(db.run_prepared_select(*this)), _result_row_t>
    {
      return {db.run_prepared_select(*this), _dynamic_names};
    }

    void _bind_params() const
    {
      params._bind(_prepared_statement);
    }

    _parameter_list_t params;
    _dynamic_names_t _dynamic_names;
    mutable _prepared_statement_t _prepared_statement;
  };
}  // namespace sqlpp
