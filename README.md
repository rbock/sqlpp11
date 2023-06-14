sqlpp11
=======
A type safe embedded domain specific language for SQL queries and results in C++.

```diff
!If you are a tenured user of sqlpp11, please note that
!  - with 0.61 the connector libraries for mysql/sqlite/postgresql got merged into the main repo.
!  - master has been renamed to main and is now the default branch
```

Documentation is found in [docs](docs/Home.md).

So what is this about?
----------------------
SQL and C++ are both strongly typed languages. Still, most C/C++ interfaces to SQL are based on constructing queries as strings and on interpreting arrays or maps of strings as results.

sqlpp11 is a templated library representing an embedded domain specific language (EDSL) that allows you to

  * define types representing tables and columns,
  * construct type safe queries checked at compile time for syntax errors, type errors, name errors and even some semantic errors,
  * interpret results by iterating over query-specific structs with appropriately named and typed members.

This results in several benefits, e.g.

  * the library user operates comfortably on structs and functions,
  * the compiler reports many kinds of errors long before the code enters unit testing or production,
  * the library hides the gory details of string construction for queries and interpreting results returned by select calls.

The library supports both static and dynamic queries. The former offers greater benefit in terms of type and consistency checking. The latter makes it easier to construct queries in flight.

sqlpp11’s core is vendor-neutral.
Specific traits of databases (e.g. unsupported or non-standard features) are handled by connector libraries.
Connector libraries can inform the developer of missing features at compile time.
They also interpret expressions specifically where needed.
For example, the connector could use the operator|| or the concat method for string concatenation without the developer being required to change the statement.

Connectors for MariaDB, MySQL, PostgreSQL, sqlite3, sqlcipher are included in this repository.

The library is already used in production but it is certainly not complete yet. Feature requests, bug reports, contributions to code or documentation are most welcome.

Examples:
---------
For the examples, lets assume you have a table class representing something like

```SQL
CREATE TABLE foo (
    id bigint,
    name varchar(50),
    hasFun bool
);
```

And we assume to have a database connection object:

```C++
TabFoo foo;
Db db(/* some arguments*/);

// selecting zero or more results, iterating over the results
for (const auto& row : db(select(foo.name, foo.hasFun).from(foo).where(foo.id > 17 and foo.name.like("%bar%"))))
{
    if (row.name.is_null())
        std::cerr << "name is null, will convert to empty string" << std::endl;
    std::string name = row.name;   // string-like fields are implicitly convertible to string
    bool hasFun = row.hasFun;          // bool fields are implicitly convertible to bool
}

// selecting ALL columns of a table
for (const auto& row : db(select(all_of(foo)).from(foo).where(foo.hasFun or foo.name == "joker")))
{
    int64_t id = row.id; // numeric fields are implicitly convertible to numeric c++ types
}

// selecting zero or one row, showing off with an alias:
SQLPP_ALIAS_PROVIDER(cheese);
if (const auto& row = db(select(foo.name.as(cheese)).from(foo).where(foo.id == 17)))
{
    std::cerr << "found: " << row.cheese << std::endl;
}

// selecting a single row with a single result:
return db(select(count(foo.id)).from(foo).unconditionally()).front().count;

Of course there are joins and subqueries, more functions, order_by, group_by etc.
These will be documented soon.

// A sample insert
db(insert_into(foo).set(foo.id = 17, foo.name = "bar", foo.hasFun = true));

// A sample update
db(update(foo).set(foo.hasFun = not foo.hasFun).where(foo.name != "nobody"));

// A sample delete
db(remove_from(foo).where(not foo.hasFun));
```

License:
-------------
sqlpp11 is distributed under the [BSD 2-Clause License](https://github.com/rbock/sqlpp11/blob/master/LICENSE).

Status:
-------
Branch / Compiler | clang,  gcc |  MSVC  | Test Coverage
------------------|-------------|--------|---------------
master | [![Build Status](https://travis-ci.com/rbock/sqlpp11.svg?branch=master)](https://travis-ci.com/rbock/sqlpp11?branch=master) | [![Build status](https://ci.appveyor.com/api/projects/status/eid7mwqgavo0h61h/branch/master?svg=true)](https://ci.appveyor.com/project/rbock/sqlpp11/branch/master) | [![Coverage Status](https://coveralls.io/repos/rbock/sqlpp11/badge.svg?branch=master)](https://coveralls.io/r/rbock/sqlpp11?branch=master)
develop | [![Build Status](https://travis-ci.com/rbock/sqlpp11.svg?branch=develop)](https://travis-ci.com/rbock/sqlpp11?branch=develop) | [![Build status](https://ci.appveyor.com/api/projects/status/eid7mwqgavo0h61h/branch/develop?svg=true)](https://ci.appveyor.com/project/rbock/sqlpp11/branch/develop) | [![Coverage Status](https://coveralls.io/repos/rbock/sqlpp11/badge.svg?branch=develop)](https://coveralls.io/r/rbock/sqlpp11?branch=develop)

Additional information available:
---------------------------------
Past talks about sqlpp11 and some coding concepts used within the library:

  * [CppCast:](http://cppcast.com)
   * 2015-05-07: http://cppcast.com/2015/05/roland-bock/
  * [CppCon:](http://cppcon.org)
   * 2015-09-24: [Pruning Error Messages From Your C++ Template Code](https://www.youtube.com/watch?v=2ISqFW9fRws), with examples from sqlpp11
   * 2014-09-11: [sqlpp11, An SQL Library Worthy Of Modern C++](https://www.youtube.com/watch?v=cJPAjhBm-HQ)
  * [Meeting C++:](http://meetingcpp.com)
   * 2014-12-05: [sqlpp11, An EDSL For Type-Safe SQL In C++11](https://www.youtube.com/watch?v=9Hjfg9IfzhU)
  * [MUC++:](http://www.meetup.com/MUCplusplus/)
   * 2014-02-27: [Selected C++11 Template Toffees From sqlpp11, Part1](https://www.youtube.com/watch?v=hXnGFYNbmXg), [Part2](https://www.youtube.com/watch?v=WPCV6dvxZ_U), [Part 3](https://www.youtube.com/watch?v=eB7hd_KjTig), [Part 4](https://www.youtube.com/watch?v=NBfqzcN0_EQ)


Requirements:
-------------
__Compiler:__
sqlpp11 makes heavy use of C++11 and requires a recent compiler and STL. The following compilers are known to compile the test programs:

  * clang-3.4+ on Ubuntu-12.4
  * g++-4.8+ on Ubuntu-12.4
  * g++-4.8+ on cygwin 64bit
  * g++-4.9+ on Debian Unstable
  * Xcode-7 on OS X
  * MSVC 2015 Update 1 on Windows Server 2012

__Database Connector:__
sqlpp11 requires a certain api in order to connect with the database, see database/api.h.

This repository includes the following connectors:

* MySQL
* MariaDB
* SQLite3
* SQLCipher
* PostgreSQL

Other connectors can be found here:

  * ODBC: https://github.com/Erroneous1/sqlpp11-connector-odbc (experimental)

__Date Library:__
sqlpp11 requires [Howard Hinnant’s date library](https://github.com/HowardHinnant/date) for `date` and `date_time` data types. By default, sqlpp11 uses FetchContent to pull the library automatically in the project. If you want to use an already installed version of the library with `find_package`, set `USE_SYSTEM_DATE` option to `ON`.

Build and Install
-----------------

**Note**: Depending on how you use the lib, you might not need to install it (see Basic Usage)

__Build from Source:__

Download and unpack the latest release from https://github.com/rbock/sqlpp11/releases or clone the repository. Inside the directory run the following commands:

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

__Install via Homebrew (MacOS):__

```bash
brew install marvin182/zapfhahn/sqlpp11
```

Some connectors can be installed with the formula. See `brew info marvin182/zapfhahn/sqlpp11` for available options.

__Build via vcpkg:__

You can download and install sqlpp11 using the [vcpkg](https://github.com/Microsoft/vcpkg) dependency manager:
   
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
vcpkg install sqlpp11
```
    
The sqlpp11 port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.

The following connector libraries for sqlpp11 are maintained as a separate package in vcpkg:

  * [sqlpp11-connector-sqlite3](https://github.com/microsoft/vcpkg/tree/master/ports/sqlpp11-connector-sqlite3) 
  * [sqlpp11-connector-mysql](https://github.com/microsoft/vcpkg/tree/master/ports/sqlpp11-connector-mysql)

Basic usage:
-------------
__Use with cmake__:
The library officially supports two ways how it can be used with cmake. 
You can find examples for both methods in the examples folder. 

1. FetchContent (Recommended, no installation required)
1. FindPackage (installation required, see above)

Both methods will provide the `sqlpp11::sqlpp11` target as well as targets for each connector: 

* sqlpp11::mysql
* sqlpp11::mariadb
* sqlpp11::sqlite3
* sqlpp11::sqlcipher
* sqlpp11::postgresql

These targets will make sure all required dependencies are available and correctly linked and include directories are set correctly.

__Create DDL files__:
``` 
mysql: 'show create table MyDatabase.MyTable' #or
mysqldump --no-data MyDatabase > MyDatabase.sql

```
Create headers for them with provided Python script:
```
%sqlpp11_dir%/scripts/ddl2cpp ~/temp/MyTable.ddl  ~/temp/MyTable %DatabaseNamespaceForExample%
```

In case you’re getting notes about unsupported column type consider:
  - Take a look at the other datatypes in sqlpp11/data_types. They are not hard to implement.
  - Use the `--datatype-file` command line argument as described below.

Include generated header (MyTable.h), that’s all.

If you prefer Ruby over Python, you might want to take a look at https://github.com/douyw/sqlpp11gen

Unsupported column types:
-------------
__Map unsupported column types to supported column types with a csv file__:

One can use the `--datatype-file` command line argument for the ddl2cpp script to map unsupported column types to supported column types.

The format of the csv file is:
```
<dataType>, <col_type1>, <col_type2>
<dataType>, <col_type3>
```

Where `<dataType>` is one or more of the following internal types:

  - `Boolean`
  - `Integer`
  - `Serial`
  - `FloatingPoint`
  - `Text`
  - `Blob`
  - `Date`
  - `DateTime`
  - `Time`

Example:

```
Boolean, one_or_zero
Text, url, uuid
```

Contact:
--------
  * Issues at https://github.com/rbock/sqlpp11/issues
  * email at rbock at eudoxos dot de
  * [![Join the chat at https://gitter.im/sqlpp11/Lobby](https://badges.gitter.im/sqlpp11/Lobby.svg)](https://gitter.im/sqlpp11/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

