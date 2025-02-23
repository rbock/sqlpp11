# Introduction
_This page explains insert statements with a static structure. If you want to learn about constructing insert statements at runtime, you should still read this page first and then move on to [dynamic insert statements](Dynamic-Insert.md)._

# A Basic example

Haven't found the time to document this in any detail, yet, but this is an example:

```C++
db(insert_into(tab).set(tab.gamma = true));
db(insert_into(tabDateTime)
      .set(tabDateTime.colTimePoint = std::chrono::system_clock::now()));
```

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

See also [dynamic insert statements](Dynamic-Insert.md).