#Introduction
sqlpp11 is a library mainly for constructing queries and interpreting results. It does not know how to talk to a database on its own. It needs database connectors for that. Depending on the database you want to use, you can use an existing connector, for instance

  * MySQL: https://github.com/rbock/sqlpp11-connector-mysql
  * sqlite3: https://github.com/rbock/sqlpp11-connector-sqlite3
  * PostgreSQL: https://github.com/matthijs/sqlpp11-connector-postgresql
  * STL Container (highly experimental): https://github.com/rbock/sqlpp11-connector-stl

Or you have to write your own connector. Don't worry, it is not that hard.

The api is documented [here](https://github.com/rbock/sqlpp11/blob/master/connector_api/connection.h).