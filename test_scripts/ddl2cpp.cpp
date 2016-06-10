#include <sqlpp11/sqlpp11.h>
//#include "MockDb.h" //todo 1
#include "ddl2cpp_test.h"
#include <iostream>
int ddl2cpp(int, char* [])
{
  std::cout << test_scripts_pythonPath; //included from cmake-generated header if python is found
  /* maybe todo 1: test compile / db_mock, for now we're just testing that ddl2cpp generates header without errors
   */
  /*
  MockDb db = {};
  MockDb::_serializer_context_t printer = {};

  const auto f = test::TabFoo{};
  const auto t = test::TabBar{};
  const auto tab_a = f.as(sqlpp::alias::a);

  getColumn(db, t._epsilon); //underscore bug check
  */

  return 0;
}



