# Introduction
_This page explains select statements with a static structure. If you want to learn about constructing select statements at runtime, you should still read this page first and then move on to [dynamic select statements](Dynamic-Select.md)._

Lets assume we have a table representing

```SQL
CREATE TABLE foo (
    id bigint,
    name varchar(50),
    hasFun bool
);
```
(This is SQL for brevity, not C++, see [here](Tables.md) for details on how to define types representing the tables and columns you want to work with)

Lets also assume we have an object `db` representing a connection to your [database](Database.md).

# A Basic example
This shows how you can select some data from table and iterate over the results:
```C++
for (const auto& row : db(select(foo.name, foo.hasFun)
                            .from(foo)
                            .where(foo.id > 17 and foo.name.like("%bar%"))))
{
    if (row.name.is_null())
        std::cerr << "name is null" << std::endl;
    else
       std::string name = row.name;   // string-like fields are implicitly convertible to string
    bool hasFun = row.hasFun;          // bool fields are implicitly convertible to bool
}
```
So, what's happening here? Lets ignore the gory details for a moment. Well, there is a select statement.
```C++
select(foo.name, foo.hasFun)
  .from(foo)
  .where(foo.id > 17 and foo.name.like("%bar%"))
```
It selects two columns `name` and `hasFun` from table `foo` for rows which match the criteria given in the where condition. That's about as close to _real_ SQL as it can get...

The select expression is fed into the call operator of the connection object `db`. This method sends the select statement to the database and returns an object representing the results. In the case of select statements, the result object represents zero or more rows.

One way of accessing the rows is to iterate over them in a range-based for loop.

```C++
for (const auto& row : ...)
```
Ok, so the variable row is an object that represents a single result row. You really want to use `auto` here, because you don't want to write down the actual type. Trust me. But the wonderful thing about the `row` object is that it has appropriately named and typed members representing the columns you selected. This is one of the utterly cool parts of this library.

Note that a `select` query requires a `where` condition or a call to `.unconditionally()` to express the intent to omit the `where` condition.

# The Select Statement
## Select
The `select` method takes zero or more named expression arguments.

Named expressions are expressions with a name. No surprise there. But what kind of expressions have a name? Table columns, for instance. In our example, that would be `foo.id`, `foo.name` and `foo.hasFun`. Most [function](Functions.md) calls also result in named expressions, like `count(foo.id)`.

So what about unnamed expressions? Results of binary operators like `(foo.id + 17) * 4` have no name. But you can give them a name using the `as(alias)` method. The easiest way is to use a named expression as alias, for instance `((foo.id + 17) * 4).as(foo.id)`, e.g.

```C++
for (const auto& row : db(select(((foo.id + 17) * 4).as(foo.id)).from(tab).where(foo.id > 42))
{
   std::cout << row.id << std::endl;
}
```

Another option is to define an alias like this:

```C++
SQLPP_ALIAS_PROVIDER(total);
for (const auto& row : db(select(sum(id).as(total)).as(foo.id)).from(tab)))
{
   std::cout << row.total << std::endl;
}
```
Using aliases also comes in handy when you join tables and have several columns of the same name, because no two named expressions in a select must have the same name. So if you want to do something like

```C++
select(foo.id, bar.id); // compile error
```

One of the columns needs an alias.
```C++
SQLPP_ALIAS_PROVIDER(barId);
select(foo.id, bar.id.as(barId));
```
### Select Columns
All examples above called the `select()` function with one or more arguments, but `select()` can also be called with no arguments. In that case, the selected columns have to be added afterwards

```C++
sqlpp::select().columns(foo.id, foo.name);
```

See also [dynamic select statements](Dynamic-Select.md).

### Select Flags
The following flags are currently supported:

* sqlpp::all
* sqlpp::distinct

Flags are added via the `flags()` method:

```C++
sqlpp::select().flags(sqlpp::all).columns(foo.id, foo.name);
```

or

```C++
select(foo.id, foo.name).flags(sqlpp::all);
```

The latter is shorter than the former, but the former is closer to SQL syntax and probably easier to read.

### Sub-Select
A select statement with one column also is named expression. This means you can use one select as a sub-select column of another select. For example:
```
SQLPP_ALIAS_PROVIDER(cheese_cake); // Declared outside of function
// ...
for (const auto& row : db(
               select(all_of(foo),
                      select(sum(bar.value)).from(bar).where(bar.id > foo.id)),
                      select(bar.value.as(cheese_cake)).from(bar).where(bar.id > foo.id))
              .from(foo)))
{
    const int x = row.id;
    const int64_t a = row.sum;
    const int b = row.cheese_cake;
  }
```
The name of the sub select is the name of the one column. If required, you can rename it using `as()`, as usual.

### Select All Columns
Statements like `SELECT * from foo` is used pretty often in SQL. sqlpp11 offers something similar:

```C++
select(all_of(foo));
```

## From
The `from` method expects one argument. The following subsections expand on the types of valid arguments:
* tables
* tables with an alias (via the `as` method)
* sub-selects with an alias
* joins

### Tables
This is the most simple case.
```C++
select(all_of(foo)).from(foo).where(foo.id == 17);
```

### Aliased Tables
Table aliases are useful in self-joins.
```C++
SQLPP_ALIAS_PROVIDER(left);
SQLPP_ALIAS_PROVIDER(right);
auto l = foo.as(left);
auto r = foo.as(right);
select(all_of(l)).from(l.join(r).on(l.x == r.y)).unconditionally();
```
Aliased tables might also be used to increase the readability of generated SQL code, for instance if you have very long table names.

### Aliased Sub-Select
A select can be used as a pseudo table in another select. You just have to give it a name.
```C++
SQLPP_ALIAS_PROVIDER(sub);
auto sub_select = select(all_of(foo)).from(foo).where(foo.id == 42).as(sub);
```
The variable `sub_select` can be used as a table now.

### Joins
You can join two tables like this:
```C++
foo.join(bar).on(foo.id == bar.foo);
```
If you want to join more tables, you can chain joins.
```C++
foo.join(bar).on(foo.id == bar.foo).left_outer_join(baz).on(bar.id == baz.ref);
```
_Hint_: Omitting the call to `on` will result in mildly horrible error messages. But if you really want to join without a condition, then you can call `.unconditionally()` to express the intent.

## Where
The where condition can be set via the `where` method, which takes a boolean expression argument, for instance:
```C++
select(all_of(foo)).from(foo).where(foo.id != 17 and foo.name.like("%cake"));
```

In order to prevent users from accidentally forgetting the `.where()` clause, the library requires a call to `.where()` or `.unconditionally()`. The latter expresses the intent to select all rows unconditionally.

## Group By
The method `group_by` takes one or more expression arguments, for instance:
```C++
select(all_of(foo)).from(foo).group_by(foo.name);
```

## Having
The having condition can be set via the `having` method, just like the `where` method.

## Order By
The `order_by` method takes one of more order expression, which are normal expression adorned with `.asc()` or `.desc()`, e.g.
```C++
select(all_of(foo)).from(foo).order_by(foo.name.asc());
```

## Limit And Offset
The methods `limit` and `offset` take a size_t argument, for instance:
```C++
select(all_of(foo)).from(foo).unconditionally().limit(10u).offset(20u);
```
## For Update
The `for_update` method modifies the query with a simplified "FOR UPDATE" clause without columns.
```C++
select(all_of(foo)).from(foo).where(foo.id != 17).for_update();
```

# Running The Statement
OK, so now we know how to create a select statement. But the statement does not really do anything unless we hand it over to the database:
```C++
db(select(all_of(foo)).from(foo).unconditionally());
```
This call returns a result object of a pretty complex type. Thus, you would normally want to use `auto`:

```C++
auto result = db(select(all_of(foo)).from(foo).unconditionally());
```

# Accessing The Results
The `result` object created by executing a `select` query is a container of result rows.

## Range-based For Loops
Not surprisingly, you can iterate over the rows using a range-based for-loop like this:
```C++
for (const auto& row : db(select(all_of(foo)).from(foo)).unconditionally())
{
   std::cerr << row.id << std::endl;
   std::cerr << row.name << std::endl;
}
```
Lovely, isn't it? The row objects have types specifically tailored for the select query you wrote. You can access their member by name, and these members have the expected type.

## Function-based Access
If for some reason, you don't want to use range-based for-loops, you can use `front()` and `pop_front()` on the result, like this:
```C++
while(!result.empty())
{
   const auto& row = result.front();
   std::cerr << row.id << std::endl;
   std::cerr << row.name << std::endl;
   result.pop_front();
}
