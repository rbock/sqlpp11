If you want to help with the development of the library, you might want to consider one of these items:

# Better documentation
The documentation in the Wiki is incomplete and outdated. It is also not part of source control, which makes it a bit harder to contribute. 
It might make sense to write a couple of markdown files here.

Throw in chapters of how to add features and how to write connector libraries.

## Better tests and examples
Many of the current tests were born when the library was in infant state. Often they test implementation details. And they often just print stuff, instead of checking said stuff.

# Connectors
## Merge stable connectors into sqlpp11
Having all connector libraries in place, makes maintenance considerably simpler, see #174.
It would also reduce the cognitive overhead for newcomers.

## New connectors
There are a couple of connector libraries already. Here are some that have been requested in the past

- Sybase
- Oracle
- SQL Server
- Google Spanner

## More special functions and types for connectors
GROUP CONCAT or JSON support for mysql, for instance
INSERT OR UPDATE for postgresl and mysql
INSERT INTO ... SELECT ... for postgresql
CREATE TABLE for all of them, including PPGEN

Exception types that carry more specific error information, e.g. the native error code, see #227

Find a way to handle column names that are keywords in the vendor's dialect, see #199

Handle specific data types, including JSON for the connectors.

More test cases for DDL files.

# EDSL features
Multi-line insert in prepared statements, see #68

## optional std::optional support
Instead of sqlpp::value_or_null, std::optional would be nice, see #238

## Converter to and from SQL to C++ structs, to allow for more ORM like code
Assuming that you have converter functions from struct to sqlpp11 and back, we could have something like this:
```C++
struct Person;
insert_into(tab).set(Person{});
update(tab).set(Person{});
for (const auto& person : db(select(Person{}).from(tab).unconditionally()))
{
  // ...
}
```

# Runtime improvements
## Connection pools and caching interface 
In order to support high load scenarios, 
  - connection pools (to avoid creating and destroying connections)
  - caching (hash the query and answer it from the cache for some time), see #86

## Async support
Obtain results in an asynchronous fashion, see #35, for instance.

# Compile time improvements
## Simplify code
See Seventeenification talk. Some of the simplifications can be ported back easily.

## Suppress export of symbols
Some compilers tend to export all the generated symbols, which is a bit annoying in case of template-heavy libraries like sqlpp11 (leads to larger files and longer compile/link/startup times, I believe).
There are ways to suppress this in most compilers, afaik.

