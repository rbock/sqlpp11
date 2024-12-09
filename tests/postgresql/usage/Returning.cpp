#include <iostream>

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>
#include "../../include/test_helpers.h"

#include "Tables.h"
#include "make_test_connection.h"

int Returning(int, char*[])
{
  namespace sql = sqlpp::postgresql;

  sql::connection db = sql::make_test_connection();

  test::TabFoo foo = {};

  try
  {
    test::createTabFoo(db);

    std::cout
        << db(sqlpp::postgresql::insert_into(foo).set(foo.textNnD = "dsa").returning(foo.doubleN)).front().doubleN
        << std::endl;

    std::cout
        << db(sqlpp::postgresql::insert_into(foo).set(foo.textNnD = "asd").returning(std::make_tuple(foo.doubleN))).front().doubleN
        << std::endl;

#warning need to add optinal insert tests

    auto updated =
        db(sqlpp::postgresql::update(foo).set(foo.intN = 0).unconditionally().returning(foo.textNnD, foo.intN));
    for (const auto& row : updated)
      std::cout << "Gamma: " << row.textNnD << " Beta: " << row.intN << std::endl;

    auto removed =
        db(sqlpp::postgresql::delete_from(foo).where(foo.intN == 0).returning(foo.textNnD, foo.intN));
    for (const auto& row : removed)
      std::cout << "Gamma: " << row.textNnD << " Beta: " << row.intN << std::endl;

    auto multi_insert = sqlpp::postgresql::insert_into(foo).columns(foo.intN).returning(foo.id, foo.intN);
    multi_insert.add_values(foo.intN = 1);
    multi_insert.add_values(foo.intN = 2);
    auto inserted = db(multi_insert);

    for (const auto& row : inserted)
      std::cout << row.intN << std::endl;

}

  catch (const sql::failure&)
  {
    return 1;
  }

  return 0;
}
