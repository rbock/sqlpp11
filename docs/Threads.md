# Thread Safety

sqlpp11 aspires to have no influence on thread safety itself, but offers
no particular guarantees (PRs welcome).

The recommendation therefore is to **not share** connections between threads
and to read about thread safety of the underlying database for more
information.


