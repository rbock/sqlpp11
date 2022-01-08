sqlpp11 offers equivalents for quite a few SQL functions. It also has some additional functions to make the developer's life easier, see [Misc Functions](#Misc-Functions).

If functions can be used as select columns, the column's name is the name of the function. You can use `.as()` of course to give it another name, see [Select](Select.md). There are just a few exceptions like `any()` which cannot be named.

# Member Functions
## in and not_in
The methods `in()` and `not_in()` can be used are available for pretty much any expression. They take zero or more value expressions. You can make the list dynamic by using a container of values.

```C++
tab.a.in(); // not valid SQL but certainly useful for generic code, evaluates to a false expression (version 0.35 and later).
tab.a.in(x, ...); // x, ... being one or more value expressions
tab.a.in(sub_select); // sub_select being a select expression with one result column of appropriate type.
tab.a.in(sqlpp::value_list(some_container_of_values)); // evaluates to a false expression in case of an empty container
```

## is_null
## like

# Aggregate Functions
The following aggregate functions are supported
  * avg
  * count
  * max
  * min
  * sum

For example:
```C++
for (const auto& row : db(select(tab.name, avg(tab.value))
                             .from(tab)
                             .where(tab.id > 17)
                             .group_by(tab.name)))
{
   std::cerr <<  row.name << ": " << row.avg << std::endl;
}
```

Use `count(1)` for simply query row count:
```C++
const int64_t n = db(select(count(1)).from(tab).unconditionally()).front().count;
``` 

# Text Functions
## concat
Just use the + operator :-)

# Sub-Query Functions
## exists
## any
## some

# Misc Functions
sqlpp11 offers a few functions that do not mimic SQL but are there to make your life easier.

## value
The `value` function turns the argument into an unnamed SQL expression, e.g. an `int` into an `sqlpp::integral` or a `std::string` into an `sqlpp::text`. You probably don't need to use this function too often, because in most cases, sqlpp expressions do the conversion themselves. For instance
```C++
tab.foo + 17
```
is a perfectly valid expression if `tab.foo` represents an SQL integral value. But when doing some generic query programming you might get into the situation that you want to select a constant value. For instance:
```C++
for (const auto& row : select(sqlpp::value<sqlpp::bigint>(7).as(sql::alias::a)).from(tab)))
{
   int64_t a = row.a;
}
```

##value_list
`value_list` is a helper function that takes a container of values and turns it into an sqlpp11 construct that is understood by the `in()` member function of expressions, see above.

## verbatim
sqlpp11 supports quite a few aspects of SQL. But it does certainly not cover everything, nor will it ever. So what if you need to use something that is not supported? At least for expressions there is an easy way to use unsupported features, the `verbatim()` method. It requires a template parameter to determine the SQL type and a string argument containing the expression, for instance:
```C++
select(all_of(tab)).from(tab).where(tab.foo == 42 and sqlpp::verbatim<sqlpp::boolean>("mighty special feature"));
```
_Use with care_, as sqlpp11 does not bother to look into the string you provide. That means you have to handle type checking, syntax checking, escaping of injected evil data from your users, etc.

## flatten
Say `tab.foo` and `tab.bar` represent two bigint columns. Then the type of `tab.foo` and `tab.bar` is different. Also `tab.foo + 17` and `tab.bar + 17` are expressions of different type. This is because the expression carries information about required tables for instance (and that is going to be used to check if `from()` contains all the required tables.
The expression templates can get in your way though, if you want to create parts of your query dynamically:
```C++
auto e = (tab.foo == 17);
if (userWantsBar)
   e = (tab.bar == 17);  // won't compile
```
You can use [dynamic select](Dynamic-Select.md), but there is an alternative, the `flatten()` method, which turns the expression into a more generic expression type (it just "remembers" whether it is a text, boolean etc).
```C++
auto e = flatten(tab.foo == 17);
if (userWantsBar)
   e = flatten(tab.bar == 17); // will compile
```
