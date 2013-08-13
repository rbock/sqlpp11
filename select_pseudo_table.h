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

#ifndef SQLPP_SELECT_PSEUDO_TABLE_H
#define SQLPP_SELECT_PSEUDO_TABLE_H

#include <sqlpp11/no_value.h>

namespace sqlpp
{
	// provide type information for sub-selects that are used as named expressions or tables
	template<typename Expr>
		struct select_column_spec_t
		{
			using _name_t = typename Expr::_name_t;
			template<typename T>
				using _member_t = typename Expr::template _member_t<T>;
			using _value_type = typename Expr::_value_type;
			struct _column_type {};
		};

	template<
						 typename Flags,
						 typename ExpressionList,
						 typename From,
						 typename Where,
						 typename GroupBy,
						 typename Having,
						 typename OrderBy,
						 typename Limit,
						 typename Offset,
						 typename... NamedExpr
							 >
							 struct select_pseudo_table_t: public sqlpp::table_base_t<select_pseudo_table_t<
																						 Flags, 
																						 ExpressionList, 
																						 From, 
																						 Where, 
																						 GroupBy, 
																						 Having, 
																						 OrderBy, 
																						 Limit, 
																						 Offset,
																						 NamedExpr...>, select_column_spec_t<NamedExpr>...>
	{
		using _value_type = no_value_t;

		select_pseudo_table_t(const Flags& flags, const ExpressionList& expression_list, const From& from,
				const Where& where, const GroupBy& group_by, const Having& having,
				const OrderBy& order_by, const Limit& limit, const Offset& offset):
			_flags(flags),
			_expression_list(expression_list),
			_from(from),
			_where(where),
			_group_by(group_by),
			_having(having),
			_order_by(order_by),
			_limit(limit),
			_offset(offset)
		{
			std::cerr << "Copying arguments into pseudo_table" << std::endl;
			//std::cerr << _limit._limit << std::endl;
		}

		select_pseudo_table_t(const select_pseudo_table_t& rhs):
			_flags(rhs._flags),
			_expression_list(rhs._expression_list),
			_from(rhs._from),
			_where(rhs._where),
			_group_by(rhs._group_by),
			_having(rhs._having),
			_order_by(rhs._order_by),
			_limit(rhs._limit),
			_offset(rhs._offset)
		{
			std::cerr << "Copying pseudo_table" << std::endl;
			//std::cerr << _limit._limit << std::endl;
		}

		template<typename Db>
			void serialize(std::ostream& os, Db& db) const
			{
				//std::cerr << "pseudo_table::serialize: " << _limit._limit << std::endl;
				detail::serialize_select(os, db, *this);
			}

		Flags _flags;
		ExpressionList _expression_list;
		From _from;
		Where _where;
		GroupBy _group_by;
		Having _having;
		OrderBy _order_by;
		Limit _limit;
		Offset _offset;
	};

}

#endif
