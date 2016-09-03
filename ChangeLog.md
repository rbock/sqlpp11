Important changes in sqlpp11
============================

Breaking changes in 0.36:
-------------------------
__Abstract__:

One of the main motivations of sqlpp11 is to prevent SQL programming mistakes at compile time. The following changes prevent reported mishaps.
  * `from(a,b)` not allowed anymore, please use explicit joins
  * `where(true)` not allowed anymore, please use `.unconditionally()` or sqlpp::value(true)
  * `some_sql_expression and true` not allowed anymore, please use `tab.col == sqlpp::value(true)` if you really want to express this.
  * `having(expression)` requires `expression` to be made of aggregates, e.g. columns named in `group_by()` or aggregate functions like `count()` or constant values.
  * `where()` and `having` accept only one parameter

__Explicit joins required__:

Up until sqlpp11-0.35 you could write something like

```
auto result = db(select(all_of(a), all_of(b))
                 .from(a,b)
                 .where(someCondition));
```

Using this syntax in `from()`, it was expected to have the join condition implicitly in the `where()` call.
But there is no reliable way to tell whether or not that condition is there. Or, if there definitely is none, whether
it was omitted on purpose or by accident.
In one case, an accidentally omitted join condition in the `where()` brought a production system to a screeching halt.

In order to prevent this in the future, sqlpp11 now requires you to join table explicitly, including an explicit join condition, e.g.

```
auto result = db(select(all_of(a), all_of(b))
                 .from(a.join(b).on(a.b == b.id))
                 .where(someCondition));
```

Most joins, (`join`/`inner_join`, `left_outer_join`, `right_outer_join` and `outer_join`) require a join condition to given via `on()`.
The join condition has to be some sqlpp11 boolean expression.

In those rare cases, when you really need a cross join, you can also use `cross_join()` which has no join condition, of course.

```
auto result = db(select(all_of(a), all_of(b))
                 .from(a.cross_join(b))
                 .where(someCondition));
```

__Use `.unconditionally()`__

If you want to select/update/remove all rows, earlier versions of sqlpp11 required the use of `where(true)`. Since version 0.36, use `unconditionally()`, for instance:
```
auto result = db(select(all_of(t)).from(t).unconditionally());
```

__Use `sqlpp::value()` to wrap bool values in boolean expressions__

Lets say you had

```
struct X
{
    int a;
    int b;
};
auto x = X{};
```

Then earlier versions of sqlpp11 would compile the following expression:

```
select(all_of(t)).from(t).where(x.a == x.a or t.b == t.b);
```

What you probably meant was:

```
select(all_of(t)).from(t).where(t.a == x.a and t.b == x.b);
```

In order to prevent this kind of mistake, boolean operators in sql expressions require sqlpp boolean expressions as operators.
The library also requires the types of the left/right hand side operands of a comparison to be different, so that `t.a < t.a` does not compile any more.

In the rare case you really have a bool value that you want to use a boolean sql expression, you have to wrap it in sqlpp::value(x), e.g.

```
select(all_of(t)).from(t).where(sqlpp::value(x.a == 17) and t.b == x.b);
```

__`having()` requires aggregate expressions__

In older versions, the following code was allowed:

```
select(all_of(t)).from(t).where(t.a > 7).having(t.b != "");
```

As of sqlpp11-0.36, the having argument must be made of aggregate columns or functions, e.g.

```
select(all_of(t)).from(t).unconditionally().group_by(t.b).having(t.b != "", avg(t.c) < 42);
```

__`where()` and `having` accept only one expression__

In older versions, `where()` and `having()` would accept more than one argument and combine those arguments with `and`.
I am not sure this was ever used. So it just made life harder for the compiler.

As of version 0.36, `where()` and `having()` accept only one parameter.
