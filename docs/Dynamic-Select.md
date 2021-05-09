# Introduction
_This page explains dynamic select statements. Before studying this page, you should read about [static select statements](Select.md)._

If you know the exact structure of your queries at compile time, statically constructed select statements are perfect. But if the structure depends on runtime information like user input, you will need dynamic select statements. Depending on your needs, you can choose the required dosage.

# A Basic Example
So let us construct select query with a dynamic part
```C++
auto s = dynamic_select(db, all_of(foo)).from(foo).dynamic_where();
if (runtimeQuery.id)
   s.where.add(foo.id == runtimeQuery.id);
if (!runtimeQuery.name.empty())
   s.where.add(foo.name == runtimeQuery.name);
```
Admittedly, a rather lame example (please suggest nicer ones), but anyway, this is what's happening:
```C++
dynamic_select(db, ...)
```
This initializes a dynamic select. One major difference between `dynamic_select` and `select` is the first argument of `dynamic_select`: a database connection. This is used to evaluate the dynamic parts of the query as they are added.

```C++
.dynamic_where();
...
s.where.add(foo.name == runtimeQuery.name);
s.where.add(foo.id == runtimeQuery.id);
```
The first part creates a new select object that accepts a dynamically constructed where expression. In this case the user can determine whether to search for a certain name or a certain id, or neither or both.

# Dynamic Select
## Dynamic Columns
If the (some) selected columns are not known at compile time, you can do something like this:
```C++
auto s = dynamic_select(db).dynamic_columns(foo.id).from(foo).unconditionally();
if (someCondition)
  s.selected_columns.add(foo.name);
if (someOtherCondition)
  s.selected_columns.add(foo.hasFun);
```
In this example, the column id is always selected. The other two columns may or may not be selected. This is determined at runtime. This impacts the way the results are accessed because the type of the result row is not known at the same level of detail as in the static case. The dynamic fields can be accessed via name lookup like in a map:
```C++
for (const auto& row : db(s))
{
   long id = row.id;
   if (someCondition)
     std::string name = row.at("name");
   if (someOtherCondition)
     std::string hasFun = row.at("hasFun");
}
```
This also shows another difference. Dynamic fields are of text type. It would be possible to add conversion methods for other types as well, but this has not been coded yet. Please let me know you wishes.

## Dynamic From
In a static query the compiler can verify whether the `from()` clause is sufficient to support all other aspects of the query.

With a dynamic from, the compile cannot know tables that going into the `from()`. Such checks would therefore be impossible. But it allows you to add joins at runtime!

```C++
auto s = dynamic_select(db, all_of(foo)).dynamic_from(foo).dynamic_where();
if (someOtherCondition)
  s.from.add(dynamic_join(bar).on(foo.barId == bar.id));
```

In this example, the user may want to include `bar` into the query.

## Dynamic Where
As shown in other examples, the where condition can be constructed dynamically using the methods `dynamic_where()` and `where.add()`. The former prepares the select to accept dynamic where conditions, the latter adds a condition. Several calls to add_where will be combined into one condition by `and`.

## Dynamic Having, Group By, OrderBy, Limit and Offset
Similar to the dynamic where, you can use `dynamic_having`, `dynamic_group_by`, `dynamic_order_by`, `dynamic_limit` and `dynamic_offset` to prepare the select statement to accept certain dynamic aspects. Using `having.add`, `group_by.add`, `order_by.add`, `limit.set` and `offset.set` you can then adjust those query parts according to the runtime information.
