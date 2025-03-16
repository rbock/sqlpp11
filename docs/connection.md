# Database Connection

In order to execute any statements your code needs to construct one or more
connections to the backend.

## MySQL and MariaDB

```c++
// This will call `mysql_library_init` once (even if called multiple times).
// It will also ensure that `mysql_library_end()` is called during shutdown.
sqlpp::mysql::global_library_init();

// Create a connection configuration.
auto config = std::make_shared<sqlpp::mysql::connection_config>();
config->user = "some_user";
config->database = "some_database";
config->debug = true; // Will log a lot of debug output.

// Create a connection
sqlpp::mysql::::connection db;
db.connectUsing(config); // This can throw an exception.
```

## Postgresql

```c++
// Create a connection configuration.
auto config = std::make_shared<sqlpp::postgresql::connection_config>();
config->user = "some_user";
config->database = "some_database";
config->debug = true; // Will log a lot of debug output.

// Create a connection
sqlpp::postgresql::connection db;
db.connectUsing(config); // This can throw an exception.
```

## Sqlite3 and SQLCipher

```c++
// Create a connection configuration.
auto config = std::make_shared<sqlpp::sqlite3::connection_config>();
config->path_to_database = ":memory:";
config->flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
config->debug = true; // Will log a lot of debug output.

// Create a connection
sqlpp::sqlite3::connection db;
db.connectUsing(config); // This can throw an exception.
```

## Other connectors

If you want to use other databases, you would have to write your own connector.
Don't worry, it is not that hard, following the existing examples.
