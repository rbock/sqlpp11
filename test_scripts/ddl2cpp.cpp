#include <sqlpp11/sqlpp11.h>
#include <cassert>
//#include "MockDb.h" //todo 1
#include "ddl2cpp_test.h"
#include <iostream>
#include <string>
#include <fstream>


int testSqlFile(const std::string pathToSqlFile ){

#if defined _WIN64 || defined _WIN32
  std::string nullOutput = " > nul 2>&1";
#else
  std::string nullOutput = " > /dev/null 2>&1";
#endif

  std::string ddlHeaderPath = "../test_scripts/ddl2cpp_test_result_header";

  std::string args =
    " ../scripts/ddl2cpp  -fail-on-parse " +
    pathToSqlFile + " " +
    ddlHeaderPath +
    "  ddlcpp2_test_namespace "+
    nullOutput
    ;

  auto python_args = test_scripts_pythonPath + args.c_str();
  return system(python_args.c_str());
}

int ddl2cpp(int, char* [])
{
  std::ifstream file("../scripts/ddl2cpp");
  if (!file)
  {
    std::cout << "script tests should be started from the test_scripts directory.\n";
    exit(1);
  }

  assert(0 == testSqlFile ("../test_scripts/ddl2cpp_sample_good.sql"));
  assert(0 < testSqlFile ("../test_scripts/ddl2cpp_sample_bad.sql"));

  /* maybe todo 1: test compile / use db_mock, for now we're just checking that ddl2cpp generates header without errors
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



