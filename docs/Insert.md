Haven't found the time to document this in any detail, yet, but this is an example:

```C++
db(insert_into(tab).set(tab.gamma = true));
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