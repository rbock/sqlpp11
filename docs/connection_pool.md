# Introduction

SQLPP11 has support for connection pools which are centralized caches of database connections. When you need a database connection, you can fetch one from the connection pool, use the connection to make SQL
queries and when you no longer need the connection object, destroy it, usually by letting it go out of scope. When a connection object is destroyed, the actual connection to the database server is not closed,
but put in a cache instead and next time when you need a database connection object, you will be handed one that reuses a cached connection. If there are no connections is the cache, then a new connection
will be created, wrapped in a connection object and handed to you.

## Creating connection pools

Each connector has its own connection pool class. Currently we have

* sqlpp::mysql::connection_pool
* sqlpp::postgresql::connection_pool
* sqlpp::sqlite3::connection_pool

The connection pool constructors accept two parameters

* Shared pointer to a configuration object. This is the same configuration as the one that you use when creating regular (non-pooled) database connections.
* An integer specifying the initial size of the connection cache. This cache size grows automatically when necessary, so this value is not very important as pretty much any small positive value will do. In our example below we use 5.

In this example we create a PostgreSQL connection pool:

```
auto config = std::make_shared<sqlpp::postgresql::connection_config>();
config->dbname = "my_database";
config->user = "my_user";
config->password = "my_password";
config->debug = true;
auto pool = sqlpp::postgresql::connection_pool{config, 5};
```
You can also create a pool object without a configuration and initialize it later.

```
auto pool = sqlpp::postgresql::connection_pool{}
....
....
....
auto config = std::make_shared<sqlpp::postgresql::connection_config>();
config->dbname = "my_database";
config->user = "my_user";
config->password = "my_password";
config->debug = true;
pool.initialize(config, 5);
```
## Getting connections from the connection pool

Once the connection pool object is established we can use the _get()_ method to fetch connections

```
auto db = pool.get();
for (row : db(select(....))) {
  ....
}
```

## Returning connections to the connection pool

We don't really need to do anything to return the connection to the pool. Once the connection object's destructor is called the connection is not really destroyed but instead is returned automatically to the connection
pool's cache. This means that we can use the connection pool in the following way

```
for (row : pool.get()(select(....))) {
  ....
}
```
In the above example we fetch a connection from the pool, use it to make an SQL query and then return the connection to the pool.

## Ensuring that connections handed out by the connection pool are valid

Connection pools handle out connections that are either newly created or fetched from the connection cache. For connections that are fetched from the cache an optional check can be made to ensure that the connection is still active.
If the cached connection is no longer active, then it is discarded and the user is handed a newly created connection.

The check type is specified as an optional parameter of the _get()_ method. Currently the following check types are supported:

* **sqlpp::connection_check::none** Don't check the connection
* **sqlpp::connection_check::passive** A passive check which does not send anything to the server but just checks if the server side has already closed their side of the connection. This check type is supported only for PostgreSQL, for the other connector types it is treated as _none_.
* **sqlpp::connection_check::ping** Send a dummy request to the server to check if the connection is still alive. For MySQL connections this check uses the `mysql_ping` library function. For the other connector types
this check sends `SELECT 1` to the server.

For example:

```
auto db = pool.get(sqlpp::connection_check::ping);
for (row : db(select(....))) {
  ....
}
```

## Working around connection thread-safety issues

Connection pools can be used to work around [thread-safety issues](Threads.md) by ensuring that no connection is used simultaneously by multiple threads.

### Getting a new connection for each request

One possible usage pattern is getting a new connection handle for each request. For example:

```
for (row : pool.get()(select(....))) {
  ....
}
pool.get()(insert_into(mytable)....)
pool.get()(remove_from(mytable)....)
```

This usage pattern works well provided that you don't use transactions. If you use transactions then you must make sure that the transaction object and all queries inside the transaction use the same database connection.

### Using one connection per thread

Another usage pattern that works around the multi-threading problems is keeping a connection handle in a global thread_local object. This global object is not a real connection, but a wrapper which lazily fetches a connection from the
thread pool the first time when it is used to execute a database query. The wrapper will expose all the relevant methods and will forward them to the real connection. This way each thread can use the global wrapper as a database
connection and the thread_local storage class specifier will make sure that each thread accesses its own database connection. You can see examples/connection_pool as an example of this usage pattern.
