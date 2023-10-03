#include <db_global.h>

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

#include <memory>
#include <thread>

int main()
{
  // Initialize the global connection variable
  auto config = std::make_shared<sqlpp::postgresql::connection_config>();
  config->dbname = "my_database";
  config->user = "my_username";
  config->password = "my_password";
  config->debug = false;
  db_global_init(config);

  // Spawn 10 threads and make them send SQL queries in parallel
  int num_threads = 10;
  int num_queries = 5;
  std::vector<std::thread> threads {};
  for (int i = 0; i < num_threads; ++i)
  {
    threads.push_back(std::thread([&] () {
      for (int j = 0; j < num_queries; ++j)
      {
        g_dbc(select (sqlpp::value (1).as(sqlpp::alias::a)));
      }
    }));
  }
  for (auto&& t : threads)
  {
    t.join();
  }

  return 0;
}
