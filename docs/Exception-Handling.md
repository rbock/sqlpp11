## When to expect an exception

sqlpp11 connectors throw the majority of `sqlpp::exception`s so check your connector's documentation. Generally, you should expect an exception when:

-  Connecting to a database
-  Preparing a statement
-  Executing a statement
-  Retrieving and iterating through result rows

Additionally, the date library used by sqlpp11 may throw `std::runtime_error`. As of 2017-04-08 this only happens when formatting a date using a format string.