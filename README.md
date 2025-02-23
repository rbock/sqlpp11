sqlpp23
=======
A type safe embedded domain specific language for SQL queries and results in C++.

Documentation is found in [docs](docs/index.md).

What is this about?
----------------------
sqlpp23 allows you to write SQL in the form of C++ expression:

  * tables, columns, result fields are represented as structs or data members
  * statements and clauses are constructed by functions
  * Your IDE can help you write SQL in C++
  * Your compiler will find a lot of typical mistakes long before they hit integration tests or even production, e.g.
    * typos
    * comparing apples to oranges
    * a missing table in `from()`
    * missing a non-default column in `insert_into()`
    * selecting a mix of aggregates and non-aggregates
    * differences in SQL dialects from one database backend to the next, see below

sqlpp23’s core is vendor-neutral.

Specific traits of databases (e.g. unsupported or non-standard features) are handled by connector libraries.
Connector libraries can inform you and your IDE of missing features at compile time.
They also interpret expressions specifically where needed.
For example, the connector could use the `operator||` or the `concat` method for string concatenation without your being required to change the statement.

Connectors for MariaDB, MySQL, PostgreSQL, sqlite3, sqlcipher are included in this repository.

Examples:
---------
Let's assume we have a database connection object `db` and a table object `foo` representing something like

```SQL
CREATE TABLE foo (
    id bigint NOT NULL,
    name varchar(50),
    hasFun bool NOT NULL
);
```

[insert](docs/insert.md)
```C++
db(insert_into(foo).set(foo.id = 17, foo.name = "bar", foo.hasFun = true));
```

[update](docs/update.md)
```C++
db(update(foo).set(foo.name = std::nullopt).where(foo.name != "nobody"));
```

[delete](docs/delete.md)
```C++
db(remove_from(foo).where(not foo.hasFun));
```

[select](docs/select.md)
```C++
// selecting zero or more results, iterating over the results
for (const auto& row : db(select(foo.id, foo.name, foo.hasFun)
                          .from(foo)
                          .where(foo.id > 17 and foo.name.like("%bar%"))))
{
  std::cout << row.id << '\n';                    // int64_t
  std::cout << row.name.value_or("NULL") << '\n'; // std::optional<std::string_view>
  std::cout << row.hasFun() << '\n';              // bool
}
```

[database specfic](connectors.md)
```C++
  for (const auto &row : db(sql::insert_into(foo)
                                .set(foo.id = 7, foo.name = std::nullopt, foo.hasFun = false)
                                .on_conflict(foo.id)
                                .do_update(foo.name = "updated")
                                .where(foo.id > 17)
                                .returning(foo.name))) {
    std::cout << row.name.value_or("NULL") << '\n';
  }
```


The library is already used in production but it is certainly not complete yet. Feature requests, bug reports, contributions to code or documentation are most welcome.

License:
-------------
sqlpp23 is distributed under the [BSD 2-Clause License](https://github.com/rbock/sqlpp23/blob/main/LICENSE).

Requirements:
-------------
__Compiler:__
sqlpp23 requires C++23 and requires a recent compiler and standard library.

__Database Connector:__
In order to do anything really useful with the library, you will also need a database backend and library to talk to it:

* MySQL
* MariaDB
* SQLite3
* SQLCipher
* PostgreSQL

Build and Install
-----------------

**Note**: Depending on how you use the lib, you might not need to install it (see Basic Usage)

__Build from Source:__

Download and unpack the latest release from https://github.com/rbock/sqlpp23/releases or clone the repository. Inside the directory run the following commands:

```bash
cmake -B build
cmake --build build --target install
```

The last step will build the library and install it system wide, therefore it might need admins rights.

By default only the core library will be installed. To also install connectors set the appropriate variable to `ON`: 

* `BUILD_MYSQL_CONNECTOR`
* `BUILD_MARIADB_CONNECTOR`
* `BUILD_POSTGRESQL_CONNECTOR`
* `BUILD_SQLITE3_CONNECTOR`
* `BUILD_SQLCIPHER_CONNECTOR`

The library will check if all required dependencies are installed on the system. If connectors should be installed even if the dependencies are not yet available on the system, set `DEPENDENCY_CHECK` to `OFF`. 

Example: Install the core library, sqlite3 connector and postgresql connector. Don’t check if the dependencies such as Sqlite3 are installed and don’t build any tests:

```bash
cmake -B build -DBUILD_POSTGRESQL_CONNECTOR=ON -DBUILD_SQLITE3_CONNECTOR=ON -DDEPENDENCY_CHECK=OFF -DBUILD_TESTING=OFF
cmake --build build --target install
```

Basic usage:
-------------
__Use with cmake__:
The library officially supports two ways how it can be used with cmake. 
You can find examples for both methods in the examples folder. 

1. FetchContent (Recommended, no installation required)
1. FindPackage (installation required, see above)

Both methods will provide the `sqlpp23::sqlpp23` target as well as targets for each connector: 

* sqlpp23::mysql
* sqlpp23::mariadb
* sqlpp23::sqlite3
* sqlpp23::sqlcipher
* sqlpp23::postgresql

These targets will make sure all required dependencies are available and correctly linked and include directories are set correctly.

Contact:
--------
  * Issues at https://github.com/rbock/sqlpp23/issues
  * email at rbock at eudoxos dot de

