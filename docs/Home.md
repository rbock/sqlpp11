# Introduction
Let's see:
* You know C++?
* You know some SQL?
* You want to use SQL in your C++ program?
* You think C++ and SQL should play well together?
* You know which tables you want to use in a database?
* You can cope with a few template error messages in case something is wrong?

You have come to the right place!

sqlpp11 offers you to code SQL in C++ almost naturally. You can use tables, columns and functions. Everything has strong types which allow the compiler to help you a lot. At compile time, it will tell about most of those pesky oversight errors you can make (typos, comparing apples with oranges, forgetting tables in a select statement, etc). And it does not stop at query construction. Results have ranges, and strongly typed members, so that you can browse through results in a type-safe manner, worthy of modern C++.

The following pages will tell you how to use it:
* [Database Connectors](Database.md)
* [Tables](Tables.md)
* [Insert](Insert.md)
* [Select](Select.md) <- You might want to read this first as an appetizer
* [Update](Update.md)
* [Remove](Remove.md)
* [Functions](Functions.md)
* [Prepared Statements](Prepared-Statements.md)
* [Transactions](Transactions.md)
* [Thread Safety](Threads.md)
* [NULL](NULL.md)
* [Connection Pools](Connection-Pools.md)
* [New Features](New-Features.md)
