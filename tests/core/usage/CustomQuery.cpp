/*
 * Copyright (c) 2013-2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp23/sqlpp23.h>
#include <sqlpp23/tests/core/MockDb.h>
#include <sqlpp23/tests/core/result_helpers.h>
#include <sqlpp23/tests/core/tables.h>
#include <iostream>

namespace {
struct on_duplicate_key_update {
  std::string _serialized;

  template <typename Db, typename Assignment>
  on_duplicate_key_update(Db&, Assignment assignment) {
    typename Db::_context_t context;
    _serialized =
        " ON DUPLICATE KEY UPDATE " + to_sql_string(context, assignment);
  }

  template <typename Db, typename Assignment>
  auto operator()(Db&, Assignment assignment) -> on_duplicate_key_update& {
    typename Db::_context_t context;
    _serialized += ", " + to_sql_string(context, assignment);
    return *this;
  }

  auto get() const -> sqlpp::verbatim_t<::sqlpp::no_value_t> {
    return ::sqlpp::verbatim(_serialized);
  }
};
}  // namespace

int CustomQuery(int, char*[]) {
  MockDb db = {};
  MockDb::_context_t printer = {};

  const auto f = test::TabFoo{};
  const auto t = test::TabBar{};

  // A void custom query
  auto x = sqlpp::statement_t{}
           << sqlpp::verbatim("PRAGMA writeable_schema = 1");
  std::cerr << to_sql_string(printer, x) << std::endl;
  db(x);

  // Syntactically, it is possible to use this void query as a prepared
  // statement, too, not sure, whether this makes sense very often...
  db(db.prepare(x));

  // A prepared custom select
  // The return type of the custom query is determined from the last argument
  // which does have a return type, in this case the select
  auto p = db.prepare(select(all_of(t))
                      << from(t) << where(t.id > sqlpp::parameter(t.id)));
  p.params.id = 8;
  for (const auto& row : db(p)) {
    std::cerr << row.id << std::endl;
  }

  // Create a custom "insert or ignore"
  db(sqlpp::insert() << sqlpp::verbatim(" OR IGNORE") << into(t)
                     << insert_set(t.textN = "sample", t.boolNn = true));

  // Create a custom multi-row "insert or ignore"
  auto batch = insert_columns(t.textN, t.boolNn);
  batch.add_values(t.textN = "sample", t.boolNn = true);
  batch.add_values(t.textN = "ample", t.boolNn = false);
  db(sqlpp::insert() << sqlpp::verbatim(" OR IGNORE") << into(t) << batch);

  // Create a MYSQL style custom "insert on duplicate update"
  db(sqlpp::insert_into(t).set(t.textN = "sample", t.boolNn = true)
     << on_duplicate_key_update(db, t.textN = "sample")(db, t.boolNn = false)
            .get());

  // A custom (select ... into) with adjusted return type
  // The first argument with a return type is the select, but the custom query
  // is really an insert. Thus, we tell it so.
  auto c = select(all_of(t)).from(t)
           << into(f) << with_result_type_of(insert_into(f));
  std::cerr << to_sql_string(printer, c) << std::endl;
  auto i = db(c);
  static_assert(std::is_integral<decltype(i)>::value,
                "insert yields an integral value");

  auto d = sqlpp::statement_t{}
           << sqlpp::verbatim("INSERT INTO tab_sample VALUES()")
           << with_result_type_of(sqlpp::insert());
  auto j = db(d);
  static_assert(std::is_integral<decltype(j)>::value,
                "insert yields an integral value");

  for (const auto& row :
       db(sqlpp::statement_t{} << sqlpp::verbatim("PRAGMA user_version")
                               << with_result_type_of(select(all_of(t))))) {
    (void)row.id;
  }

  return 0;
}
