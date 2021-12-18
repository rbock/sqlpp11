This page will tell you about how to define tables for sqlpp11, once I get around to writing it. Until then, here is an example, taken from [here](https://github.com/rbock/sqlpp11/blob/master/tests/Sample.h):

```C++
namespace TabSample_
{
	struct Alpha
	{
		struct _name_t
		{
			static constexpr const char* _get_name() { return "alpha"; }
			template<typename T>
				struct _member_t
				{
					T alpha;
				};
		};
		using _value_type = sqlpp::bigint;
		struct _column_type
		{
			using _must_not_insert = std::true_type;
			using _must_not_update = std::true_type;
			using _can_be_null = std::true_type;
			using _trivial_value_is_null = std::true_type;
			using _foreign_key = decltype(TabFoo::omega);
		};
	};

	struct Beta
	{
		struct _name_t
		{
			static constexpr const char* _get_name() { return "beta"; }
			template<typename T>
				struct _member_t
				{
					T beta;
				};
		};
		using _value_type = sqlpp::varchar;
		struct _column_type
		{
			using _can_be_null = std::true_type;
			using _trivial_value_is_null = std::true_type;
			using _must_not_update = std::true_type;
		};
	};

	struct Gamma
	{
		struct _name_t
		{
			static constexpr const char* _get_name() { return "gamma"; }
			template<typename T>
				struct _member_t
				{
					T gamma;
				};
		};
		using _value_type = sqlpp::boolean;
		struct _column_type
		{
			using _require_insert = std::true_type;
		};
	};
}

struct TabSample: sqlpp::table_base_t<
									TabSample, 
									TabSample_::Alpha, 
									TabSample_::Beta,
									TabSample_::Gamma
													 >
{
	using _value_type = sqlpp::no_value_t;
	struct _name_t
	{
		static constexpr const char* _get_name() { return "tab_sample"; }
		template<typename T>
			struct _member_t
			{
				T tabSample;
			};
	};
	template<typename Db>
		void serialize_impl(std::ostream& os, Db& db) const
		{
			os << _name_t::_get_name();
		}
};
```
Not too complex, I hope?

Not lean enough to be fun writing though, right? A sample code generator (DDL -> C++) written in python can be found [here](https://github.com/rbock/sqlpp11/blob/master/scripts/ddl2cpp).

## Names with reserved SQL keywords
In case the SQL entity uses a reserved keyword it is required to escape the name using the target's RDBMS rule.  For example:

```SQL
SELECT "order" FROM orders;
```
Here the column order is the same as the reserved keyword ORDER from the "ORDER BY" SQL clause.  As such in for example PostgreSQL you need to put the name in quotes.  Pay attention that the rules are RDBMS specific and that some RDBMS then consider the name case sensitive and that the select may fail due to incorrect case of the entity name.  To achieve this you need to amend the column literal. For example:

```C++
namespace TabSample_
{
        struct Order
        {
                struct _name_t
                {
                        static constexpr const char* _get_name() { return "\"order\""; }
                        template<typename T>
                                struct _member_t
                                {
                                        T order;
                                };
                };
                using _value_type = sqlpp::bigint;
                struct _column_type
                {
                        using _must_not_insert = std::true_type;
                        using _must_not_update = std::true_type;
                        using _can_be_null = std::true_type;
                        using _trivial_value_is_null = std::true_type;
                        using _foreign_key = decltype(TabFoo::omega);
                };
        };
}
```
