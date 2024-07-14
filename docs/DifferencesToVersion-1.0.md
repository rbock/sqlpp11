# New data types

## optional
Nullable values are represented as `std::optional` if you are using C++17 or later, `sqlpp::compat::optional` otherwise.

## string_view
Text results are represented as `std::string_view` if you are using C++17 or later, `sqlpp::compat::string_view` otherwise.

# span<uint8_t>
Blob results are represented as `std::span<uint8_t>` if you are using C++20 or later, `sqlpp::compat::span<uint8_t>` otherwise.

# Result values
Result rows are represented as structs with the respective columns represented as data members. In version 1.0, these data members used to have a type that wrapped the actual data and provided conversion operators and functions to check for NULL.

Now data members have the correct data type, e.g. `int64_t` or `optional<string_view>`.

# No read-only columns
Version 1.0 had the concept of read-only columns, e.g. you could not modify a column with auto-increment values.

This concept has been removed. In most cases, you will still not want to modify columns with auto-increment values, but you can do it now, if you want to.

# IS DISCTINCT FROM
Version 1.0 used to have `is_equal_to_or_null` which translated to either `=` or `IS NULL`. While useful, this did not work with parameters.

The library now offers `is_distinct_from` and `is_not_distinct_from` which safely compares with actual values and `NULL`.

# Selecting aggregate functions
In version 1.0, many functions automatically came with a name. This was convenient for selecting, but also rather unspecific, e.g. `select(max(tab.price)).from(tab).unconditionally()` would yield rows with data member called `count_`.

Now, functions do not come with names. While this will lead to lightly more code, the assumption is that this will lead to slightly more readable code, e.g.

```
SQLPP_ALIAS_PROVIDER(max_price);
// ...
  for (const auto& row : db(select(max(tab.price)).from(tab).unconditionally()))
  {
    std::cout << row.max_price << '\n';
  }
```


