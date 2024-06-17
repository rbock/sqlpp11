# Introduction
Database NULL is a strange beast. It can be compared to anything but that comparison never returns true. It also never returns false, it returns NULL. Even

```SQL
NULL != NULL    -> NULL
NULL = NULL     -> NULL
```

A value like that would be pretty unusual in C++. Especially since fields in a result could be either a decent value or NULL. And this can change from result row to result row.

Also, in `where` or `having` conditions, you have to change the expression (not just parameters) if you want to NULL instead of a value:

```SQL
a = 7
a IS NULL
```

The library tries to make the usage of NULL reasonably safe in C++:

# Assigning NULL in INSERT or UPDATE
In assignments in `insert` or `update` statements, NULL can be represented by `sqlpp::null`.

```C++
db(insert_into(t).set(t.gamma = sqlpp::null);
```

Note that you cannot assign `sqlpp::null` to columns that are not nullable.

# Comparing to NULL
Columns that can be NULL can be compared to NULL using the `is_null` and `is_not_null` member functions.

```C++
const auto s = select(all_of(tab)).from(tab).where(tab.alpha == 17 and tab.beta.is_null());
```

# Obtaining potential NULL values from a select
sqlpp11 can determine whether a result field can be null, based on the columns involved and the structure of your query. If in doubt (for instance due to dynamic parts), it will assume that a field can be NULL.

You can check if a field is NULL by calling the `is_null()` method.

The value can be accessed by the `.value()` method. It will return a default constructed value in case `is_null() == true`.

```C++
for (const auto& row :  db(select(all_of(tab)).from(tab).unconditionally()))
{
  if (not row.alpha.is_null())
  {
    const auto a = row.alpha.value();
  }
}
```

# Future:
We might switch to `std::optional` to represent nullable columns or values.

