#include <iostream>

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

#include "TabFoo.h"
#include "make_test_connection.h"

int Returning(int, char*[])
{
  namespace sql = sqlpp::postgresql;

  sql::connection db = sql::make_test_connection();

  model::TabFoo foo = {};

  try
  {
    db.execute(R"(DROP TABLE IF EXISTS tabfoo;)");
    db.execute(R"(CREATE TABLE tabfoo
                   (
                   alpha bigserial NOT NULL,
                   beta smallint,
                   gamma text,
                   c_bool boolean,
                   c_timepoint timestamp with time zone DEFAULT now(),
                   c_day date
                   ))");

    std::cout
        << db(sqlpp::postgresql::insert_into(foo).set(foo.gamma = "dsa").returning(foo.c_timepoint)).front().c_timepoint
        << std::endl;

    std::cout
        << db(sqlpp::postgresql::insert_into(foo).set(foo.gamma = "asd").returning(std::make_tuple(foo.c_timepoint))).front().c_timepoint
        << std::endl;

    auto i = sqlpp::postgresql::dynamic_insert_into(db, foo).dynamic_set().returning(foo.c_timepoint);
    i.insert_list.add(foo.gamma = "blah");

    std::cout << db(i).front().c_timepoint << std::endl;

    auto updated =
        db(sqlpp::postgresql::update(foo).set(foo.beta = 0).unconditionally().returning(foo.gamma, foo.beta));
    for (const auto& row : updated)
      std::cout << "Gamma: " << row.gamma << " Beta: " << row.beta << std::endl;

    auto removed =
        db(sqlpp::postgresql::remove_from(foo).where(foo.beta == 0).returning(foo.gamma, foo.beta));
    for (const auto& row : removed)
      std::cout << "Gamma: " << row.gamma << " Beta: " << row.beta << std::endl;

    auto multi_insert = sqlpp::postgresql::insert_into(foo).columns(foo.beta).returning(foo.alpha, foo.beta);
    multi_insert.values.add(foo.beta = 1);
    multi_insert.values.add(foo.beta = 2);
    auto inserted = db(multi_insert);

    for (const auto& row : inserted)
      std::cout << row.beta << std::endl;

}

  catch (const sql::failure&)
  {
    return 1;
  }

  return 0;
}
