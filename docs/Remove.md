Since `delete` is keyword in C++ that has a different meaning than `delete` in SQL, sqlpp11 calls the method remove. There is no detailed documentation available yet, but here is an example that might help if you have read about [select statements](Select.md) already:

```C++
db(remove_from(tab).where(tab.alpha == tab.alpha + 3));
```

## Removing using multiple tables as condition:

```C++
test_sqlpp::Users usr;
test_sqlpp::UsersForms usr_forms;
test_sqlpp::Forms form_;

db(remove_from(usr_forms).using_(usr, form_, usr_forms).where( 
    usr_forms.iduser == usr.id 
    and usr.username == username 
    and usr_forms.idform == form_.id
    and form_.name == form_name
    ));
```
