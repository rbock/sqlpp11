sqlpp11
=======

A type safe SQL template library for C++


Motivation:
-----------
SQL and C++ are both strongly typed languages. They should play well together. Still, most C/C++ interface are based on constructing strings for querys and on interpreting arrays or maps of strings as results. Typicaly there are a bunch of helper functions and classes that help to construct queries, to interpret results or to avoid code injection.

sqlpp11 is a templated library representing an embedded domain specific language (EDSL) that allows you to

  * define types representing tables and columns
  * construct typesafe queries checked at compile time for syntax errors, type errors, name errors and even some semantic errors
  * interpret results by iterating over query-specific structs with appropriately named and typed members

Under the hood, it still communicates with the database via strings, but the library user operates on structs and functions. And the compiler will report many kinds of errors long before the code enters unit testing or production.

Examples:
---------
For the examples, lets assume you had a table class representing something like 

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
	std::string name = row.name; // string-like fields are implicitly convertible to string
	bool hasFun = hasFun;        // bool fields are implicitly convertible to bool
}

// selecting ALL columns of a table
for (const auto& row: db.run(select(all_of(foo)).from(foo).where(hasFun or foo.name == "joker")))
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
db.run(update(foo).set(foo.hasFun = not foo.hasFun).where(foo.name != "nobody");

// A sample delete
db.run(remove_from(tab).where(not tab.hasFun));
```

Requirements:
-------------
sqlpp11 makes heavy use of C++11. It has been developed using 

clang-3.2 on Ubuntu-10.4 with matching libc++

It also requires a database library with a matching interface, see database/api.h

Links to sample implementations will follow soon.



