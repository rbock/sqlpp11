# Tables

In order to build meaningful SQL statements with sqlpp11, you need to represent tables and their columns as structs that can be understood by the library.

The default way to do so is by using a code generator to translate DDL to C++. A code generator covering a lot of use cases can be found [here](https://github.com/rbock/sqlpp11/blob/main/scripts/ddl2cpp). There is also a specific one for [sqlite3](https://github.com/rbock/sqlpp11/blob/main/scripts/sqlite2cpp.py).

If you look at the output, you will see why a generator is helpful. Here is a [sample](https://github.com/rbock/sqlpp11/blob/main/tests/core/usage/Sample.h).

