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

#ifndef SQLPP_PREPARED_UPDATE_H
#define SQLPP_PREPARED_UPDATE_H

#include <sqlpp11/parameter_list.h>
#include <sqlpp11/result.h>
#include <sqlpp11/data_types/no_value.h>

namespace sqlpp
{
  template <typename Db, typename Update>
  struct prepared_update_t
  {
    using _traits = make_traits<no_value_t, tag::is_prepared_statement>;
    using _nodes = detail::type_vector<>;

    using _parameter_list_t = make_parameter_list_t<Update>;
    using _prepared_statement_t = typename Db::_prepared_statement_t;

    using _run_check = consistent_t;

    auto _run(Db& db) const -> size_t
    {
      return db.run_prepared_insert(*this);
    }

    void _bind_params() const
    {
      params._bind(_prepared_statement);
    }

    _parameter_list_t params;
    mutable _prepared_statement_t _prepared_statement;
  };
}

#endif
