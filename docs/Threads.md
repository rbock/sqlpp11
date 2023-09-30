# Thread Safety

sqlpp11 aspires to have no influence on thread safety itself, but offers
no particular guarantees (PRs welcome). This means that in the general case
your program may have problems if it does one of the following

* Creates a connection in one thread and then uses it in another thread..
* Uses the same connection simultaneously in multiple threads.

The exact level of thread-safety depends on the underlying client library, for
example with MySQL, PostgreSQL and SQLite3 it is generally safe to create a
connection in one thread and then use it in another thread, but attempting to
use the same connection simultaneously in multiple threads causes crashes,
lock-ups and SQL errors.

The recommendation therefore is to **not share** connections between threads
and to read about thread safety of the underlying database for more
information. You may also look into [Connection Pools](Connection-Pools.md)
as a way to make SQL queries simultaneously in multiple threads.
