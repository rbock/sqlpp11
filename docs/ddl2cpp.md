__Create DDL files__:
``` 
mysql: 'show create table MyDatabase.MyTable' #or
mysqldump --no-data MyDatabase > MyDatabase.sql

```
Create headers for them with provided Python script:
```
%sqlpp23_dir%/scripts/ddl2cpp ~/temp/MyTable.ddl  ~/temp/MyTable %DatabaseNamespaceForExample%
```

In case you’re getting notes about unsupported column type consider:
  - Take a look at the other datatypes in sqlpp23/data_types. They are not hard to implement.
  - Use the `--datatype-file` command line argument as described below.

Include generated header (MyTable.h), that’s all.

If you prefer Ruby over Python, you might want to take a look at https://github.com/douyw/sqlpp23gen


Unsupported column types:
-------------
__Map unsupported column types to supported column types with a csv file__:

One can use the `--datatype-file` command line argument for the ddl2cpp script to map unsupported column types to supported column types.

The format of the csv file is:
```
<dataType>, <col_type1>, <col_type2>
<dataType>, <col_type3>
```

Where `<dataType>` is one or more of the following internal types:

  - `Boolean`
  - `Integer`
  - `Serial`
  - `FloatingPoint`
  - `Text`
  - `Blob`
  - `Date`
  - `DateTime`
  - `Time`

Example:

```
Boolean, one_or_zero
Text, url, uuid
```


