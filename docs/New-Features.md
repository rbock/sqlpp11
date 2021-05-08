# New Features

There are a bunch of new features, that are not fully documented yet. If you would like to contribute documentation, please let me know.

## Preprocessor generator for columns/tables
You'll need boost 1.50 or greater to use this feature by niXman:

```C++
#include <sqlpp11/ppgen.h>
SQLPP_DECLARE_TABLE(
  (tab_person)
  ,
  (id     , int         , SQLPP_AUTO_INCREMENT)
  (name   , varchar(255), SQLPP_NOT_NULL      )
  (feature, int         , SQLPP_NOT_NULL      )
)
```

See `examples/ppgen.hpp`.

## Union
Unions are now supported. The arguments need to have the same names and types in their columns.

```C++
db(select(t.alpha).from(t).where(true)
    .union_distinct(select(f.epsilon.as(t.alpha)).from(f).where(true)));
db(select(t.alpha).from(t).where(true)
    .union_all(select(f.epsilon.as(t.alpha)).from(f).where(true)));
```

## With
sqlpp11 supports common table expressions:

```C++
auto x = sqlpp::cte(sqlpp::alias::x).as(select(all_of(t)).from(t));

db(with(x)(select(x.alpha).from(x).where(true)));
```
## Custom Queries
This allows you to combine building blocks of sqlpp11 into custom queries, for instance a SELECT..INTO which is not supported yet.

```C++
// A custom (select ... into) with adjusted return type
// The first argument with a return type is the select, 
// but the custom query is really an insert. Thus, we tell it so.
db(custom_query(select(all_of(t)).from(t), into(f))
       .with_result_type_of(insert_into(f)));
```

## Schema-qualified tables
sqlpp11 assumes that you're connection addresses one database, normally. But you can tell it about other databases using the `sqlpp::schema_t` and instantiating schema-qualified tables with it:

```C++
auto schema = db.attach("lorem_ipsum");
auto s = schema_qualified_table(schema, TabSample{}).as(sqlpp::alias::x)
// s now represents "lorem_ipsum.tab_sample as x"
// s can be used like any other table in the code
```
