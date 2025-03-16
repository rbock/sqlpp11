# Introduction

Let's see:

- You know C++?
- You know some SQL?
- You want to use SQL in your C++ program?
- You think C++ and SQL should play well together?
- You know which tables you want to use in a database?
- You can cope with a few template error messages in case something is wrong?

You have come to the right place!

sqlpp23 offers you to code SQL in C++ almost naturally. You can use tables,
columns and functions. Everything has strong types which allow the compiler to
help you a lot. At compile time, it will tell about most of those pesky
oversight errors you can might make (typos, comparing apples with oranges,
forgetting tables in a select statement, etc). And it does not stop at query
construction. Results have ranges, and strongly typed members, so that you can
browse through results in a type-safe manner, worthy of modern C++.

The following pages will tell you how to use it:

- **Basics**
  - [Setup](setup.md)
  - [Code generation](ddl2cpp.md)
- **Statements**
  - [Connection](connection.md)
  - [Select](select.md)
  - [Insert](insert.md)
  - [Update](update.md)
  - [Delete](delete.md)
  - [NULL](null.md)
  - [Static vs. Dynamic](dynamic.md)
- **Building Blocks**
  - [Tables, Joins, and CTEs](tables.md)
  - [Functions](functions.md)
  - [Sub Selects](sub_select.md)
- **Invoking Statements**
  - [Direct Execution](direct_execution.md)
  - [Prepared Statements](prepared_statement.md)
  - [Transaction](transaction.md)
- **Advanced Topics**
  - [Thread Safety](thread_safety.md)
  - [Connection Pool](connection_pool.md)
