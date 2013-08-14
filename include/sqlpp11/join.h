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

#ifndef SQLPP_JOIN_H
#define SQLPP_JOIN_H

#include <sqlpp11/type_traits.h>

namespace sqlpp
{
	struct inner_join_t
	{
		static constexpr const char* _name = "INNER";
	};
	struct outer_join_t
	{
		static constexpr const char* _name = "OUTER";
	};
	struct left_join_t
	{
		static constexpr const char* _name = "LEFT OUTER";
	};
	struct right_join_t
	{
		static constexpr const char* _name = "RIGHT OUTER";
	};

	template<typename JoinType, typename Lhs, typename Rhs, typename On = noop>
	struct join_t
	{
		static_assert(is_table_t<Lhs>::value, "invalid lhs argument for join()");
		static_assert(is_table_t<Rhs>::value, "invalid rhs argument for join()");
		static_assert(is_noop<On>::value or is_expression_t<On>::value, "invalid on expression in join().on()");

		static_assert(Lhs::_table_set::template is_disjunct_from<typename Rhs::_table_set>::value, "joined tables must not be identical");

		using _is_table = typename std::conditional<is_noop<On>::value, 
					tag_no, 
					tag_yes>::type;
		using _table_set = typename std::conditional<is_noop<On>::value, 
					void, 
					typename Lhs::_table_set::template join<typename Rhs::_table_set>::type>::type;

		template<typename Expr> 
			using add_on_t = join_t<JoinType, Lhs, Rhs, typename std::decay<Expr>::type>;

		template<typename Expr>
			add_on_t<Expr> on(Expr&& expr)
			{
				return { _lhs, _rhs, std::forward<Expr>(expr) };
			}

		template<typename T>
			join_t<inner_join_t, join_t, typename std::decay<T>::type> join(T&& t)
			{
				return { *this, std::forward<T>(t) };
			}

		template<typename Db>
			void serialize(std::ostream& os, Db& db) const
			{
				os << " (";
				_lhs.serialize(os, db);
				os << ") " << JoinType::_name << " JOIN (";
				_rhs.serialize(os, db);
				os << ") ON ("; 
				_on.serialize(os, db);
				os << ")";
			}

		Lhs _lhs;
		Rhs _rhs;
		On _on;
	};
}

#endif
