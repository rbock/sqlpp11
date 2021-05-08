# Planned Features
There is lots to do, of course. Here are things I plan to implement:

## Conditional columns for select statements
There is an issue with dynamic columns: They are not fixed in name and type at compile time. Thus, they have to be accessed via a string argument and yield a string value.

Offering a conditional column would therefore be nice. If the condition is true, it yields a value. Otherwise, it yields NULL.

I am not sure about the exact syntax yet. It might look similar to SQL's case statement.

## Dynamic or conditional joins
As of now, you can use `dynamic_from` and then add tables to it, i.e. using an inner join with the `on` placed in the `where` clause. There is no support for outer joins though.

It would be nice to be able to do something like this:

```C++
if (x)
  s.from.right_outer_join(bar).on(some_condition);
```

Or, similar to the conditional columns:

```C++
select(...).from(foo.conditional_right_outer_join(x, bar).on(some_condition));
```

## Date/Time types
Yes, I know, long overdue. Still not sure what best to use on the C++ side (personally, I am using unix timestamps). I am not a big fan of `std::tm` aka `struct tm`. Currently I am favouring `std::chrono::time_point`.

