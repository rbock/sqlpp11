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
The automatic name for selected aggregate functions drops the `_`.

# Aggregates and non-aggregates
They must not be mixed in a select.

`group_by` accepts columns only (there is an escape hatch: `group_by_column`)
`select` requires either
- all selected columns are aggregate expressions (either an aggregate function or a group by column), or
- no selected column is an aggregate expression

If group_by is specified in the select, then all columns have to be aggregate expressions.

# Join
unconditional joins are cross joins. The "unconditional" function is dropped.
Added join tables (other than the left-most) can be dynamic.

# Dynamic queries
We don't always have a completely fixed structure for our queries. For instance, there might columns that we only want to select under certain circumstances. In version 1.0, this was handled by dynamic queries. Now we introduce conditional query parts that may or may not be used at runtime:

## Select optional columns
select(tab.id, dynamic(condition, tab.bigData)).from(tab).where(tab.id == 17);

If `condition == true` then `bigData` will be selected, otherwise `NULL` will be selected.

## Join optional table
select(tabA.id).from(tabA.cross_join(dynamic(condition, tabB))).where(tab.id == 17);

If `condition == true` then the cross join will be part of the query, otherwise not. Obviously, that means that you need to make sure that query parts that rely on `tabB` in this example also depend on the same condition.

## Optional AND operand
select(tab.id).from(tab).where(tab.id == 17 and dynamic(condition, tab.desert != "cheesecake"));

If `condition == true`, then the dynamic part will evaluate to `tab.desert != "cheesecake")`. Otherwise it will be treated as `true` (and the AND expression will be collapsed).

## Optional OR operand
select(tab.id).from(tab).where(tab.id == 17 or dynamic(condition, tab.desert != "cheesecake"));

If `condition == true`, then the dynamic part will evaluate to `tab.desert != "cheesecake")`. Otherwise it will be treated as `false` (and the OR expression will be collapsed).

## DELETE FROM
This used to be called `remove_from` in 1.0, it has been renamed to `delete_from` to be closer to SQL.

Dropped features
Unary operator+()
