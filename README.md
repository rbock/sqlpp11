sqlpp11
=======

A type safe template library for SQL queries and results in C++


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
  * the library hides the gory details of string construction for queries and interpreting string based results returned by select calls.

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
for (const auto& row: db.run(select(foo.name, foo.hasFun).from(foo).where(foo.id > 17 and foo.name.like("%bar%"))))
{
    if (row.name.is_null())
        std::cerr << "name is null, will convert to empty string" << std::endl;
    std::string name = row.name;   // string-like fields are implicitly convertible to string
    bool hasFun = hasFun;          // bool fields are implicitly convertible to bool
}

// selecting ALL columns of a table
for (const auto& row: db.run(select(all_of(foo)).from(foo).where(foo.hasFun or foo.name == "joker")))
{
    int64_t id = row.id; // numeric fields are implicitly convertible to numeric c++ types
}

// selecting zero or one row, showing off with an alias:
SQLPP_ALIAS_PROVIDER_GENERATOR(cheese);
if (const auto& row = db.run(select(foo.name.as(cheese)).from(foo).where(foo.id == 17)))
{
    std::cerr << "found: " << row.cheese << std::endl;
}

// selecting exactly one row:
return db.run(select(count(foo.id)).from(foo))->count;

Of course there are joins and subqueries, more functions, order_by, group_by etc.
These will be documented soon.

// A sample insert
db.run(insert_into(foo).set(foo.id = 17, foo.name = "bar", foo.hasFun = true));

// A sample update
db.run(update(foo).set(foo.hasFun = not foo.hasFun).where(foo.name != "nobody"));

// A sample delete
db.run(remove_from(foo).where(not foo.hasFun));
```

Requirements:
-------------
__Compiler:__
sqlpp11 makes heavy use of C++11 and requires a recent compiler and STL. The following compilers are known to compile the test programs:

    * clang-3.2 on Ubuntu-12.4
    * g++-4.8 on Ubuntu-12.4

__Database Connector:__
sqlpp11 requires a certain api in order to connect with the database, see database/api.h.

Links to sample implementations will follow soon.



