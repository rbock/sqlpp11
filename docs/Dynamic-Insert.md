# Introduction
_This page explains dynamic insert statements. Before studying this page, you should read about [static insert statements](Insert.md)._

If you know the exact structure of your inserts at compile time, statically constructed insert statements are perfect. But if the structure depends on runtime information like user input, you will need dynamic insert statements. For those only provide part fields of table schema, leave others to default value or NULL, dynamic insert statement is a must. Depending on your needs, you can choose the required dosage.

# A Basic Example
So let us construct insert statement with a dynamic part
```C++
auto s = dynamic_insert_into(db, foo).dynamic_set(
    foo.name = name,
);
s.insert_list.add(foo.id = runtimeStatement.id);
s.insert_list.add(foo.hasFun = runtimeStatement.hasFun);
const int64_t result = db(s);
```
Admittedly, a rather simplistic example (please suggest nicer ones), but anyway, this is what's happening:
```C++
dynamic_insert_into(db, table) 
```
This initializes a dynamic insert. One major difference between `dynamic_insert_into` and `insert_into` is the first argument of `dynamic_insert_into`: a database connection. This is used to evaluate the dynamic parts of the statement as they are added.

```C++
.dynamic_set(...);
...
s.insert_list.add(foo.id = runtimeStatement.id);
s.insert_list.add(foo.hasFun = runtimeStatement.hasFun);
```
The first part creates a new insert object that accepts a dynamically constructed set expression. In this case the user can determine whether to search for a certain id or a certain hasFun, or neither or both.

```C++
const int64_t result = db(s);
```
Finally use `db(s)` to execute the insert statement, and returns a `int64_t` value. If `foo.id` column is a `AUTO INCREMENT` (auto generated) id field, this will returns the auto generated id, for others this will returns 0.
