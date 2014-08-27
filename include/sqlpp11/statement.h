/*
 * Copyright (c) 2013-2014, Roland Bock
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLPP_STATEMENT_H
#define SQLPP_STATEMENT_H

#include <sqlpp11/result.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_select.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/noop.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/serializer.h>

#include <sqlpp11/detail/get_last.h>
#include <sqlpp11/detail/pick_arg.h>

namespace sqlpp
{
	template<typename Db, typename... Policies>
		struct statement_t;

	namespace detail
	{
		template<typename Db = void, typename... Policies>
			struct statement_policies_t
			{
				using _database_t = Db;
				using _statement_t = statement_t<Db, Policies...>;

				template<typename Needle, typename Replacement>
					struct _policies_update_t
					{
						static_assert(detail::is_element_of<Needle, make_type_set_t<Policies...>>::value, "policies update for non-policy class detected");
						using type =  statement_t<Db, policy_update_t<Policies, Needle, Replacement>...>;
					};

				template<typename Needle, typename Replacement>
					using _new_statement_t = typename _policies_update_t<Needle, Replacement>::type;

				using _all_required_tables = detail::make_joined_set_t<required_tables_of<Policies>...>;
				using _all_provided_tables = detail::make_joined_set_t<provided_tables_of<Policies>...>;
				using _all_provided_outer_tables = detail::make_joined_set_t<provided_outer_tables_of<Policies>...>;
				using _all_extra_tables = detail::make_joined_set_t<extra_tables_of<Policies>...>;

				using _known_tables = detail::make_joined_set_t<_all_provided_tables, _all_extra_tables>;

				template<typename Expression>
					using _no_unknown_tables = detail::is_subset_of<required_tables_of<Expression>, _known_tables>;

				// The tables not covered by the from.
				using _required_tables = detail::make_difference_set_t<
					_all_required_tables,
					_all_provided_tables // Hint: extra_tables are not used here because they are just a helper for dynamic .add_*()
						>;

				using _result_type_provider = detail::get_last_if<is_return_value_t, noop, Policies...>;

				struct _result_methods_t: public _result_type_provider::template _result_methods_t<statement_policies_t>
				{};


				// A select can be used as a pseudo table if
				//   - at least one column is selected
				//   - the select is complete (leaks no tables)
				using _can_be_used_as_table = typename std::conditional<
					is_select_column_list_t<_result_type_provider>::value and _required_tables::size::value == 0,
					std::true_type,
					std::false_type
						>::type;

				using _value_type = typename std::conditional<
					detail::none_t<is_missing_t<Policies>::value...>::value,
					value_type_of<_result_type_provider>,
					no_value_t // if a required statement part is missing (e.g. columns in a select), then the statement cannot be used as a value
						>::type;

				using _traits = make_traits<_value_type, tag_if<tag::is_expression, not std::is_same<_value_type, no_value_t>::value>>;

				struct _recursive_traits
				{
					using _required_tables = statement_policies_t::_required_tables;
					using _provided_tables = detail::type_set<>;
					using _provided_outer_tables = detail::type_set<>;
					using _extra_tables = detail::type_set<>;
					using _parameters = detail::make_parameter_tuple_t<parameters_of<Policies>...>;
					using _can_be_null = detail::any_t<
									can_be_null_t<_result_type_provider>::value, 
									detail::make_intersect_set_t<
									  required_tables_of<_result_type_provider>, 
								    provided_outer_tables_of<statement_policies_t>
								  >::size::value != 0>;
				};
			};
	}

	template<typename Db,
		typename... Policies
			>
			struct statement_t:
				public Policies::template _member_t<detail::statement_policies_t<Db, Policies...>>...,
				public expression_operators<statement_t<Db, Policies...>, value_type_of<detail::statement_policies_t<Db, Policies...>>>,
				public detail::statement_policies_t<Db, Policies...>::_result_methods_t,
				public Policies::template _methods_t<detail::statement_policies_t<Db, Policies...>>...
	{
		using _policies_t = typename detail::statement_policies_t<Db, Policies...>;

		using _traits = make_traits<value_type_of<_policies_t>,
					tag::is_select, 
					tag_if<tag::is_expression, is_expression_t<_policies_t>::value>, 
					tag_if<tag::is_named_expression, is_expression_t<_policies_t>::value>,
					tag::requires_braces>;
		using _recursive_traits = typename _policies_t::_recursive_traits;
		using _used_outer_tables = typename _policies_t::_all_provided_outer_tables;

		using _result_type_provider = typename _policies_t::_result_type_provider;

		using _name_t = typename _result_type_provider::_name_t;

		// Constructors
		statement_t()
		{}

		template<typename Statement, typename Term>
			statement_t(Statement statement, Term term):
				Policies::template _member_t<_policies_t>{
					typename Policies::template _impl_t<_policies_t>{
						detail::pick_arg<typename Policies::template _member_t<_policies_t>>(statement, term)
					}}...
		//Policies::template _member_t<_policies_t>{{detail::pick_arg<typename Policies::template _member_t<_policies_t>>(statement, term)}}...
		{}

		statement_t(const statement_t& r) = default;
		statement_t(statement_t&& r) = default;
		statement_t& operator=(const statement_t& r) = default;
		statement_t& operator=(statement_t&& r) = default;
		~statement_t() = default;

		static constexpr size_t _get_static_no_of_parameters()
		{
			return std::tuple_size<parameters_of<statement_t>>::value;
		}

		size_t _get_no_of_parameters() const
		{
			return _get_static_no_of_parameters();
		}

		static void _check_consistency()
		{
			static_assert(not required_tables_of<_policies_t>::size::value, "one sub expression requires tables which are otherwise not known in the statement");

			using swallow = int[]; 
			(void) swallow{(Policies::template _methods_t<detail::statement_policies_t<Db, Policies...>>::_check_consistency(), 0)...};
		}


	};

	template<typename Context, typename Database, typename... Policies>
		struct serializer_t<Context, statement_t<Database, Policies...>>
		{
			using T = statement_t<Database, Policies...>;
			using P = detail::statement_policies_t<Database, Policies...>;

			static Context& _(const T& t, Context& context)
			{
				using swallow = int[]; 
				(void) swallow{(serialize(static_cast<const typename Policies::template _member_t<P>&>(t)()._data, context), 0)...};

				return context;
			}
		};

	template<typename NameData>
		struct statement_name_t
		{
			using _traits = make_traits<no_value_t, tag::is_noop>;
			using _recursive_traits = make_recursive_traits<>;

			// Data
			using _data_t = NameData;

			// Member implementation with data and methods
			template<typename Policies>
				struct _impl_t
				{
					_data_t _data;
				};

			// Member template for adding the named member to a statement
			template<typename Policies>
				struct _member_t
				{
					using _data_t = NameData;

					_impl_t<Policies> statement_name;
					_impl_t<Policies>& operator()() { return statement_name; }
					const _impl_t<Policies>& operator()() const { return statement_name; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.statement_name)
						{
							return t.statement_name;
						}
				};

			// Additional methods for the statement
			template<typename Policies>
				struct _methods_t
				{
					static void _check_consistency() {}
				};
		};

}

#endif


