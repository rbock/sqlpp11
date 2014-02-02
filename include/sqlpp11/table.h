/*
 * Copyright (c) 2013, Roland Bock
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

#ifndef SQLPP_TABLE_H
#define SQLPP_TABLE_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/table_alias.h>
#include <sqlpp11/column.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/join.h>
#include <sqlpp11/no_value.h>

namespace sqlpp
{
	struct table_base_t {};

	template<typename Table, typename... ColumnSpec>
	struct table_t: public table_base_t, public ColumnSpec::_name_t::template _member_t<column_t<Table, ColumnSpec>>...
	{
		using _table_set = detail::type_set<Table>; // Hint need a type_set here to be similar to a join (which always represents more than one table)
		using _all_columns = typename detail::make_set<column_t<Table, ColumnSpec>...>::type;
		static_assert(_all_columns::size::value, "at least one column required per table");
		using _required_insert_columns = typename detail::make_set_if<require_insert_t, column_t<Table, ColumnSpec>...>::type;
		using _all_of_t = std::tuple<column_t<Table, ColumnSpec>...>;
		template<typename AliasProvider>
			using _alias_t = table_alias_t<AliasProvider, Table, ColumnSpec...>;

		using _is_table = std::true_type;

		template<typename T>
			join_t<inner_join_t, Table, T> join(T t)
			{
				return { *static_cast<const Table*>(this), t };
			}

		template<typename T>
			join_t<inner_join_t, Table, T> inner_join(T t)
			{
				return { *static_cast<const Table*>(this), t };
			}

		template<typename T>
			join_t<outer_join_t, Table, T> outer_join(T t)
			{
				return { *static_cast<const Table*>(this), t };
			}

		template<typename T>
			join_t<left_outer_join_t, Table, T> left_outer_join(T t)
			{
				return { *static_cast<const Table*>(this), t };
			}

		template<typename T>
			join_t<right_outer_join_t, Table, T> right_outer_join(T t)
			{
				return { *static_cast<const Table*>(this), t };
			}

		template<typename AliasProvider>
			_alias_t<AliasProvider> as(const AliasProvider&) const
			{
				return {*static_cast<const Table*>(this)};
			}

		const Table& ref() const
		{
			return *static_cast<const Table*>(this);
		}
	};

	template<typename Table>
	auto all_of(Table t) -> typename Table::_all_of_t
	{
		return {};
	}

	namespace vendor
	{
		template<typename Context, typename X>
			struct interpreter_t<Context, X, typename std::enable_if<std::is_base_of<table_base_t, X>::value and not is_pseudo_table_t<X>::value, void>::type>
			{
				using T = X;

				static Context& _(const T& t, Context& context)
				{
					context << T::_name_t::_get_name();
					return context;
				}
			};


	}
}

#endif

