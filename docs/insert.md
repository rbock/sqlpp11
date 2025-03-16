# Insert

# Single row insert

You can insert single rows like this:

```C++
db(insert_into(tab).set(tab.gamma = true));

db(insert_into(tabDateTime)
      .set(tabDateTime.colTimePoint = std::chrono::system_clock::now()));
```

Note that the library will force you to provide assignments for every column that does not have a default value, i.e.

- does not have an explicit default,
- is not auto-incremented, and
- cannot be NULL.

# Default values

Default values are implicitly assigned if you don't specify an assignment for the respective column.
You can also make it explicit by assigning `sqlpp::default_value`, e.g.

```c++
db(insert_into(tab).set(tab.gamma = true, tab.foo = sqlpp::default_value));
```

If you want to insert a row with all columns having the default value, you can also use the `.default_values()` clause.

```c++
db(insert_into(tab).default_values());
```

Of course, this only works if all columns actually have a default value.

# Multi row insert

This is how you could insert multiple rows at a time:

```C++
auto multi_insert = insert_into(t).columns(t.gamma, t.beta, t.delta);
multi_insert.values.add(t.gamma = true, t.beta = "cheesecake", t.delta = 1);
multi_insert.values.add(t.gamma = sqlpp::default_value, t.beta = sqlpp::default_value,
                        t.delta = sqlpp::default_value);
multi_insert.values.add(t.gamma = sqlpp::value_or_null(true),
                        t.beta = sqlpp::value_or_null("pie"),
                        t.delta = sqlpp::value_or_null<sqlpp::integer>(sqlpp::null));
db(multi_insert);
```

Note that `add` currently requires precise value types, equal to the respective column's value
type. For instance, time point columns are represented as
`std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>`.

Thus, when using such a column in a `multi_insert`, you might have to cast to the right
time point.

```
  auto multi_time_insert = insert_into(tabDateTime).columns(tabDateTime.colTimePoint);
  multi_time_insert.values.add(tabDateTime.colTimePoint = std::chrono::time_point_cast<std::chrono::microseconds>(
                                   std::chrono::system_clock::now()));
```

Similar for other data types.


