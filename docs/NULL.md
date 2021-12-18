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

# Obtaining potential NULL values from a select
sqlpp11 can determine whether a result field can be null, based on the columns involved and the structure of your query. If in doubt (for instance due to dynamic parts), it will assume that a field can be NULL.

You can check if a field is NULL by calling the `is_null()` method. That's easy.

When it comes to accessing the value, there are two options, though. These can be controlled by the connection class and the columns of your tables.

## Option 1: No conversion operator
```C++
class connection: public sqlpp::connection
{
  public:
    using _traits = ::sqlpp::make_traits<::sqlpp::no_value_t, 
             ::sqlpp::tag::enforce_null_result_treatment
           >;
```
If this tag is used in the connection's traits and the respective column does not override it, then there is no conversion operator for fields that can be NULL. You have to access the value through the `value()` method.
If the field is NULL, this method will throw an `sqlpp::exception`.

## Option 2: Conversion operator, converting NULL to trivial value
If the `tag::enforce_null_result_treatment` is not used in the connection class or the respective column uses `tag::enforce_null_result_treatment`, then there is a conversion operator. Both the conversion operator and the `value()` method will not throw in case of a NULL value. Instead, the will return the trivial value for the field's type, e.g. 0 for numbers or "" for texts.

## Alternatives:
One often discussed alternative would be boost::optional or (in the future) std::optional. There is one drawbacks (correct me if I am wrong, please):

`optional` cannot be used for binding result values because it is unclear whether there already is a value to bind to.

# Handling NULL in statements
When adding potential NULL values to a statement, you have two options:

## Manually
```C++
auto s = dynamic_select(db, all_of(tab)).from(tab).dynamic_where();
if (i_do_have_a_decent_value_of_alpha)
  s.add_where(tab.alpha == alpha);
else
  s.add_where(tab.alpha.is_null());
```

## tvin()
`tvin()` is a free function that can be used with `std::string` and build-in types like `int` or `float` or `bool`. `tvin` stands for Trivial Value Is NULL. It is used in combination with `operator==()`, `operator!=()` and `operator=()`. These operators will behave the way they should, e.g.

```C++
select(all_of(tab)).from(tab).where(tab.alpha == sqlpp::tvin(a));
```

This will evaluate to 

```SQL
SELECT tab.* FROM tab WHERE alpha = 7;
-- OR
SELECT tab.* FROM tab WHERE alpha IS NULL;
```

Similar with insert:

```C++
insert_into(tab).set(tab.alpha = sqlpp::tvin(a));
```

This will evaluate to 

```SQL
INSERT INTO tab (alpha) VALUES(7);
-- OR
INSERT INTO tab (alpha) VALUES(NULL);
```

## Using Column Type
Like to accessing values in select results, setting values can be controlled via the column type:

```C++
struct Alpha
{
  struct _name_t;
  using _traits = sqlpp::make_traits<sqlpp::bigint, 
     // ...
     sqlpp::tag::trivial_value_is_null>;
};
```
With this tag, you do not need to use `tvin()` for operators `=`, `==`, `!=`. It is used automatically. It translates operator `!` into `IS NULL`.

**Hint**: Please be aware that trivial_value_is_null will not work with parameters in prepared statements.