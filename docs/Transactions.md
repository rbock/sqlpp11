# Transactions

Transactions are simple in sqlpp11. Assuming you have a connection called `db`,
you can start and commit a transaction just like this:

```C++
auto tx = start_transaction(db);
// do something
tx.commit();
```

If you need to rollback the transaction, you can call it's `rollback()` member,
for instance like this:

```C++
auto tx = start_transaction(db);
try
{
  // do something
  tx.commit();
}
catch (...)
{
  tx.rollback();
}
```

In case you call neither `commit()` nor `rollback()` on a transaction before it
goes out of scope, it will call `rollback()` in its destructor. This automatic
rollback will be reported by the connection.

## More options
You can turn off reporting for automatic rollback in the destructor by passing
`sqlpp::quiet_auto_rollback`.

```C++
auto tx = start_transaction(db, sqlpp::quiet_auto_rollback);
```

You can also specify the isolation level for the transaction:

```C++
auto tx = start_transaction(db, ::sqlpp::isolation_level::repeatable_read);
```
