# Setup

In order to do anything really useful with the library, you will also need a
database backend and library to talk to it. The following backends are currently
supported:

- MySQL
- MariaDB
- SQLite3
- SQLCipher
- PostgreSQL

## Basic usage with CMake

The library supports two ways how it can be integrated with cmake.

You can find examples for both methods in the examples folder.

1. FetchContent (Recommended, no installation required)
1. FindPackage (installation required, see below)

Both methods will provide the `sqlpp23::sqlpp23` target as well as targets for
each connector:

- sqlpp23::mysql
- sqlpp23::mariadb
- sqlpp23::sqlite3
- sqlpp23::sqlcipher
- sqlpp23::postgresql

These targets will make sure all required dependencies are available and
correctly linked and include directories are set correctly.

## Build and install

**Note**: Depending on how you use the lib, you might not need to install it
(see Basic Usage)

Download and unpack the latest release from
https://github.com/rbock/sqlpp23/releases or clone the repository. Inside the
directory run the following commands:

```bash
cmake -B build <options>
cmake --build build --target install
```

The last step will build the library and install it system wide, therefore it
might need admins rights.

By default only the core library will be installed. To also install connectors
set the appropriate variable to `ON`:

- `BUILD_MYSQL_CONNECTOR`
- `BUILD_MARIADB_CONNECTOR`
- `BUILD_POSTGRESQL_CONNECTOR`
- `BUILD_SQLITE3_CONNECTOR`
- `BUILD_SQLCIPHER_CONNECTOR`

The library will check if all required dependencies are installed on the system.
If connectors should be installed even if the dependencies are not yet available
on the system, set `DEPENDENCY_CHECK` to `OFF`.

Example: Install the core library, sqlite3 connector and postgresql connector.
Don’t check if the dependencies are installed and don’t build any tests:

```bash
cmake -B build -DBUILD_POSTGRESQL_CONNECTOR=ON -DBUILD_SQLITE3_CONNECTOR=ON -DDEPENDENCY_CHECK=OFF -DBUILD_TESTING=OFF
cmake --build build --target install
```
