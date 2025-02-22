If you want to help with the development of the library, you might want to consider one of these items:

# Better documentation
The documentation in the Wiki is incomplete and outdated. It is also not part of source control, which makes it a bit harder to contribute. 
It might make sense to write a couple of markdown files here.

Throw in chapters of how to add features and how to write connector libraries.

## Better tests and examples
Many of the current tests were born when the library was in infant state. Often they test implementation details. And they often just print stuff, instead of checking said stuff.

## More special functions and types for connectors
GROUP CONCAT or JSON support for mysql, for instance
INSERT OR UPDATE for postgresl and mysql
INSERT INTO ... SELECT ... for postgresql

Exception types that carry more specific error information, e.g. the native error code, see #227

Find a way to handle column names that are keywords in the vendor's dialect, see #199

Handle specific data types, including JSON for the connectors.

More test cases for DDL files.

## Converter to and from SQL to C++ structs, to allow for more ORM like code
Assuming that you have converter functions from struct to sqlpp23 and back, we could have something like this:
```C++
struct Person;
insert_into(tab).set(Person{});
update(tab).set(Person{});
for (const auto& person : db(select(Person{}).from(tab).unconditionally()))
{
  // ...
}
```

## Async support
Obtain results in an asynchronous fashion, see #35, for instance.

## Suppress export of symbols
Some compilers tend to export all the generated symbols, which is a bit annoying in case of template-heavy libraries like sqlpp23 (leads to larger files and longer compile/link/startup times, I believe).
There are ways to suppress this in most compilers, afaik.

