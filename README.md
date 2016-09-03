sqlpp11
=======
A type safe embedded domain specific language for SQL queries and results in C++

Documentation is found in the [wiki](https://github.com/rbock/sqlpp11/wiki)

Contact:
--------
  * Meet me at [CppCon 2016](https://cppcon2016.sched.org/event/7nKk)
  * Issues at https://github.com/rbock/sqlpp11/issues
  * email at rbock at eudoxos dot de
  * [![Join the chat at https://gitter.im/sqlpp11/Lobby](https://badges.gitter.im/sqlpp11/Lobby.svg)](https://gitter.im/sqlpp11/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Breaking changes in 0.36:
-------------------------
See [Changes](ChangeLog.md)

Status:
-------

Branch / Compiler | clang-3.4,  gcc-4.9, Xcode-7   |  MSVC 2015  | Test Coverage
------------------| -------------------------------|-------------|---------------
master | [![Build Status](https://travis-ci.org/rbock/sqlpp11.svg?branch=master)](https://travis-ci.org/rbock/sqlpp11?branch=master) | [![Build status](https://ci.appveyor.com/api/projects/status/eid7mwqgavo0h61h/branch/master?svg=true)](https://ci.appveyor.com/project/rbock/sqlpp11/branch/master) | [![Coverage Status](https://coveralls.io/repos/rbock/sqlpp11/badge.svg?branch=master)](https://coveralls.io/r/rbock/sqlpp11?branch=master)
develop | [![Build Status](https://travis-ci.org/rbock/sqlpp11.svg?branch=develop)](https://travis-ci.org/rbock/sqlpp11?branch=develop) | [![Build status](https://ci.appveyor.com/api/projects/status/eid7mwqgavo0h61h/branch/develop?svg=true)](https://ci.appveyor.com/project/rbock/sqlpp11/branch/develop) | [![Coverage Status](https://coveralls.io/repos/rbock/sqlpp11/badge.svg?branch=develop)](https://coveralls.io/r/rbock/sqlpp11?branch=develop)

Motivation:
-----------
SQL and C++ are both strongly typed languages. Still, most C/C++ interfaces to SQL are based on constructing queries as strings and on interpreting arrays or maps of strings as results.

sqlpp11 is a templated library representing an embedded domain specific language (EDSL) that allows you to

  * define types representing tables and columns,
  * construct type safe queries checked at compile time for syntax errors, type errors, name errors and even some semantic errors,
  * interpret results by iterating over query-specific structs with appropriately named and typed members.

This results in several benefits, e.g.

  * the library user operates comfortably on structs and functions,
  * the compiler reports many kinds of errors long before the code enters unit testing or production,
  * the library hides the gory details of string construction for queries and interpreting results returned by select calls.

The library supports both static and dynamic queries. The former offers greater benefit in terms of type and consistency checking. The latter makes it easier to construct queries on the flight.

sqlpp11 is vendor-neutral. Specific traits of databases (e.g. unsupported or non-standard features) are handled by connector libraries. Connector libraries can inform the developer of missing features at compile time. They also interpret expressions specifically where needed. For example, the connector could use the operator|| or the concat method for string concatenation without the developer being required to change the statement.

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
return db(select(count(foo.id)).from(foo).where(true)).front().count;

Of course there are joins and subqueries, more functions, order_by, group_by etc.
These will be documented soon.

// A sample insert
db(insert_into(foo).set(foo.id = 17, foo.name = "bar", foo.hasFun = true));

// A sample update
db(update(foo).set(foo.hasFun = not foo.hasFun).where(foo.name != "nobody"));

// A sample delete
db(remove_from(foo).where(not foo.hasFun));
```

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

  * MySQL: https://github.com/rbock/sqlpp11-connector-mysql
  * Sqlite3: https://github.com/rbock/sqlpp11-connector-sqlite3
  * PostgreSQL: https://github.com/matthijs/sqlpp11-connector-postgresql
  * ODBC: https://github.com/Erroneous1/sqlpp11-connector-odbc (experimental)

To demonstrate that sqlpp11 can work with other backends as well, here is an experimental backend for structs in standard containers:

  * STL Container: https://github.com/rbock/sqlpp11-connector-stl

__Date Library:__
sqlpp11 requires [Howard Hinnant's date library](https://github.com/HowardHinnant/date) for `date` and `date_time` data types. Sqlpp11 includes CMake search module for this, but if you didn't install this library system-wide, you should modify CMakeLists.txt or add some flags to compile sqlpp11:

```
cmake -DHinnantDate_ROOT_DIR=/%PATH_TO_HinnantDate_SOURCE%/ -DHinnantDate_INCLUDE_DIR=/%PATH_TO_HinnantDate_SOURCE%/
```

Basic usage:
-------------
__Linux install:__
git clone date library, needed connectors, cmake and make install them.


__Create DDL files__:
``` 
mysql: 'show create table MyDatabase.MyTable' #or
mysqldump --no-data MyDatabase > MyDatabase.sql

```
Create headers for them with provided Python script:
```
%sqlpp11_dir%/scripts/ddl2cpp ~/temp/MyTable.ddl  ~/temp/MyTable %DatabaseNamespaceForExample%
```
(In case you're getting notes about unsupported column type take a look at the other datatypes in sqlpp11/data_types. They are not hard to implement.)

Include generated header (MyTable.h), that's all.

If you prefer Ruby over Python, you might want to take a look at https://github.com/douyw/sqlpp11gen

License:
-------------
sqlpp11 is distributed under the [BSD 2-Clause License](https://github.com/rbock/sqlpp11/blob/master/LICENSE).

