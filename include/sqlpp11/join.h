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

#ifndef SQLPP_JOIN_H
#define SQLPP_JOIN_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/on.h>
#include <sqlpp11/noop.h>

namespace sqlpp
{
	struct inner_join_t
	{
		template<typename Lhs, typename Rhs>
			using _provided_outer_tables = detail::make_joined_set_t<provided_outer_tables_of<Lhs>, provided_outer_tables_of<Rhs>>;

		static constexpr const char* _name = " INNER ";
	};
	struct outer_join_t
	{
		template<typename Lhs, typename Rhs>
			using _provided_outer_tables = detail::make_joined_set_t<provided_tables_of<Lhs>, provided_tables_of<Rhs>>;

		static constexpr const char* _name = " OUTER ";
	};
	struct left_outer_join_t
	{
		template<typename Lhs, typename Rhs>
			using _provided_outer_tables = detail::make_joined_set_t<provided_tables_of<Lhs>, provided_outer_tables_of<Rhs>>;

		static constexpr const char* _name = " LEFT OUTER ";
	};
	struct right_outer_join_t
	{
		template<typename Lhs, typename Rhs>
			using _provided_outer_tables = detail::make_joined_set_t<provided_outer_tables_of<Lhs>, provided_tables_of<Rhs>>;

		static constexpr const char* _name = " RIGHT OUTER ";
	};

	template<typename JoinType, typename Lhs, typename Rhs, typename On = noop>
		struct join_t
		{
			using _traits = make_traits<no_value_t, tag::is_table, tag::is_join>;
			struct _recursive_traits
			{
				using _required_ctes = detail::make_joined_set_t<required_ctes_of<Lhs>, required_ctes_of<Rhs>>;
				using _provided_ctes = detail::type_set<>;
				using _required_tables = detail::make_joined_set_t<required_tables_of<Lhs>, required_tables_of<Rhs>>;
				using _provided_tables = detail::make_joined_set_t<provided_tables_of<Lhs>, provided_tables_of<Rhs>>;
				using _provided_outer_tables = typename JoinType::template _provided_outer_tables<Lhs, Rhs>;
				using _extra_tables = detail::make_joined_set_t<extra_tables_of<Lhs>, extra_tables_of<Rhs>>;
				using _parameters = detail::make_parameter_tuple_t<parameters_of<Lhs>, parameters_of<Rhs>>;
				using _tags = detail::type_set<>;
			};


			static_assert(is_table_t<Lhs>::value, "lhs argument for join() has to be a table or join");
			static_assert(is_table_t<Rhs>::value, "rhs argument for join() has to be a table");
			static_assert(not is_join_t<Rhs>::value, "rhs argument for join must not be a join");
			static_assert(is_noop<On>::value or is_on_t<On>::value, "invalid on expression in join().on()");

			static_assert(detail::is_disjunct_from<provided_tables_of<Lhs>, provided_tables_of<Rhs>>::value, "joined tables must not be identical");

			static_assert(_recursive_traits::_required_tables::size::value == 0, "joined tables must not depend on other tables");

			template<typename OnT> 
				using set_on_t = join_t<JoinType, Lhs, Rhs, OnT>;

			template<typename... Expr>
				auto on(Expr... expr)
				-> set_on_t<on_t<void, Expr...>>
				{
					static_assert(is_noop<On>::value, "cannot call on() twice for a single join()");
					static_assert(logic::all_t<is_expression_t<Expr>::value...>::value, "at least one argument is not an expression in on()");

					return { _lhs, 
						_rhs, 
						{std::tuple<Expr...>{expr...}}
					};
				}

			template<typename T>
				join_t<inner_join_t, join_t, T> join(T t)
				{
					static_assert(not is_noop<On>::value, "join type requires on()");
					return { *this, t };
				}

			template<typename T>
				join_t<inner_join_t, join_t, T> inner_join(T t)
				{
					static_assert(not is_noop<On>::value, "join type requires on()");
					return { *this, t };
				}

			template<typename T>
				join_t<outer_join_t, join_t, T> outer_join(T t)
				{
					static_assert(not is_noop<On>::value, "join type requires on()");
					return { *this, t };
				}

			template<typename T>
				join_t<left_outer_join_t, join_t, T> left_outer_join(T t)
				{
					static_assert(not is_noop<On>::value, "join type requires on()");
					return { *this, t };
				}

			template<typename T>
				join_t<right_outer_join_t, join_t, T> right_outer_join(T t)
				{
					static_assert(not is_noop<On>::value, "join type requires on()");
					return { *this, t };
				}

			Lhs _lhs;
			Rhs _rhs;
			On _on;
		};

	template<typename Context, typename JoinType, typename Lhs, typename Rhs, typename On>
		struct serializer_t<Context, join_t<JoinType, Lhs, Rhs, On>>
		{
			using _serialize_check = serialize_check_of<Context, Lhs, Rhs, On>;
			using T = join_t<JoinType, Lhs, Rhs, On>;

			static Context& _(const T& t, Context& context)
			{
				static_assert(not is_noop<On>::value, "joined tables require on()");
				serialize(t._lhs, context);
				context << JoinType::_name;
				context << " JOIN ";
				serialize(t._rhs, context);
				serialize(t._on, context);
				return context;
			}
		};

}

#endif
