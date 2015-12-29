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

#ifndef SQLPP_NOOP_H
#define SQLPP_NOOP_H

#include <type_traits>
#include <sqlpp11/data_types/no_value.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/prepared_execute.h>

namespace sqlpp
{
  struct noop
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

    struct _alias_t
    {
    };

    template <typename Statement>
    struct _result_methods_t
    {
      using _statement_t = Statement;

      const _statement_t& _get_statement() const
      {
        return static_cast<const _statement_t&>(*this);
      }

      // Execute
      template <typename Db, typename Composite>
      auto _run(Db& db, const Composite& composite) const -> size_t
      {
        return db.execute(composite);
      }

      template <typename Db>
      auto _run(Db& db) const -> size_t
      {
        return db.execute(_get_statement());
      }

      // Prepare
      template <typename Db, typename Composite>
      auto _prepare(Db& db, const Composite& composite) const -> prepared_execute_t<Db, Composite>
      {
        return {{}, db.prepare_execute(composite)};
      }

      template <typename Db>
      auto _prepare(Db& db) const -> prepared_execute_t<Db, _statement_t>
      {
        return {{}, db.prepare_execute(_get_statement())};
      }
    };
  };

  template <typename Context>
  struct serializer_t<Context, noop>
  {
    using _serialize_check = consistent_t;
    using T = noop;

    static Context& _(const T&, Context& context)
    {
      return context;
    }
  };

  template <typename T>
  struct is_noop : std::is_same<T, noop>
  {
  };
}
#endif
