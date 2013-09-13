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

#ifndef SQLPP_SELECT_EXPRESSION_LIST_H
#define SQLPP_SELECT_EXPRESSION_LIST_H

#include <tuple>
#include <ostream>
#include <sqlpp11/select_fwd.h>
#include <sqlpp11/expression_fwd.h>
#include <sqlpp11/no_value.h>
#include <sqlpp11/table_base.h>
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/detail/set.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename Db, typename Select>
		void serialize_select(std::ostream& os, Db& db, const Select& select)
		{
			os << "SELECT ";

			select._flags.serialize(os, db);
			select._expression_list.serialize(os, db);
			select._from.serialize(os, db);
			select._where.serialize(os, db);
			select._group_by.serialize(os, db);
			select._having.serialize(os, db);
			select._order_by.serialize(os, db);
			select._limit.serialize(os, db);
			select._offset.serialize(os, db);
		};
		
		template<typename T, typename... Rest>
			struct get_first_argument
			{
				using type = T;
			};
	}
	template<typename... NamedExpr>
		struct select_expression_list_t<std::tuple<NamedExpr...>>
		{
			// check for at least one select expression
			static_assert(sizeof...(NamedExpr), "at least one select expression required");

			// check for duplicate select expressions
			static_assert(not detail::has_duplicates<NamedExpr...>::value, "at least one duplicate argument detected");

			// check for invalid select expressions
			template<typename T>
				struct is_valid_expression_t: public std::integral_constant<bool, is_named_expression_t<T>::value or is_multi_column_t<T>::value> {};
			using _valid_expressions = typename detail::make_set_if<is_valid_expression_t, NamedExpr...>::type;
			static_assert(_valid_expressions::size::value == sizeof...(NamedExpr), "at least one argument is not a named expression");

			// check for duplicate select expression names
			static_assert(not detail::has_duplicates<typename NamedExpr::_name_t::template _member_t<int>...>::value, "at least one duplicate name detected");
			
			// declare this to be a select expression
			using _is_select_expression_list = tag_yes;

			// provide type information for sub-selects that are used as expressions
			struct _column_type {};
			struct _value_type: std::conditional<sizeof...(NamedExpr) == 1, typename detail::get_first_argument<NamedExpr...>::type::_value_type, no_value_t>::type
			{
				using _is_expression = typename std::conditional<sizeof...(NamedExpr) == 1, tag_yes, tag_no>::type;
				using _is_named_expression = typename std::conditional<sizeof...(NamedExpr) == 1, tag_yes, tag_no>::type;
				using _is_alias = tag_no;
			};
			struct _no_name_t {};
			using _name_t = typename std::conditional<sizeof...(NamedExpr) == 1, typename detail::get_first_argument<NamedExpr...>::type::_name_t, _no_name_t>::type;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					detail::serialize_tuple(os, db, _expressions, ',');
				}

			std::tuple<NamedExpr...> _expressions;
		};

}

#endif
