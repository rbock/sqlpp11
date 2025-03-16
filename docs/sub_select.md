# Sub-Select

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

### Aliased Sub-Select
A select can be used as a pseudo table in another select. You just have to give it a name.
```C++
SQLPP_ALIAS_PROVIDER(sub);
auto sub_select = select(all_of(foo)).from(foo).where(foo.id == 42).as(sub);
```
The variable `sub_select` can be used as a table now.


