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

#ifndef SQLPP_EXPRESSION_H
#define SQLPP_EXPRESSION_H

#include <sqlpp11/detail/wrap_operand.h>
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/alias.h>
#include <sqlpp11/noop.h>

namespace sqlpp
{
	template<typename Lhs, typename Rhs>
		struct assignment_t
		{
			using _is_assignment = std::true_type;
			using column_type = Lhs;
			using value_type = Rhs;
			using _parameters = std::tuple<Lhs, Rhs>;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					_lhs.serialize_name(os, db);
					if (trivial_value_is_null_t<Lhs>::value and _rhs._is_trivial())
					{
						os << "=NULL";
					}
					else
					{
						os << "=";
						_rhs.serialize(os, db);
					}
				}

			Lhs _lhs;
			Rhs _rhs;
		};

	template<typename Lhs, typename Rhs, typename ValueType = detail::boolean>
		struct equal_t: public ValueType::template operators<equal_t<Lhs, Rhs>>
		{
			using _value_type = ValueType;
			using _parameters = std::tuple<Lhs, Rhs>;

			template<typename L, typename R>
			equal_t(L&& l, R&& r):
				_lhs(std::forward<L>(l)), 
				_rhs(std::forward<R>(r))
			{}

			equal_t(const equal_t&) = default;
			equal_t(equal_t&&) = default;
			equal_t& operator=(const equal_t&) = default;
			equal_t& operator=(equal_t&&) = default;
			~equal_t() = default;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << "(";
					_lhs.serialize(os, db);
					if (trivial_value_is_null_t<Lhs>::value and _rhs._is_trivial())
					{
						os << " IS NULL";
					}
					else
					{
						os << "=";
						_rhs.serialize(os, db);
					}
					os << ")";
				}

		private:
			Lhs _lhs;
			Rhs _rhs;
		};

	template<typename Lhs, typename Rhs, typename ValueType = detail::boolean>
		struct not_equal_t: public ValueType::template operators<not_equal_t<Lhs, Rhs>>
		{
			using _value_type = ValueType;
			using _parameters = std::tuple<Lhs, Rhs>;

			template<typename L, typename R>
			not_equal_t(L&& l, R&& r):
				_lhs(std::forward<L>(l)), 
				_rhs(std::forward<R>(r))
			{}

			not_equal_t(const not_equal_t&) = default;
			not_equal_t(not_equal_t&&) = default;
			not_equal_t& operator=(const not_equal_t&) = default;
			not_equal_t& operator=(not_equal_t&&) = default;
			~not_equal_t() = default;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << "(";
					_lhs.serialize(os, db);
					if (trivial_value_is_null_t<Lhs>::value and _rhs._is_trivial())
					{
						os << " IS NOT NULL";
					}
					else
					{
						os << "!=";
						_rhs.serialize(os, db);
					}
					os << ")";
				}

		private:
			Lhs _lhs;
			Rhs _rhs;
		};

	template<typename Lhs, typename ValueType = detail::boolean>
		struct not_t: public ValueType::template operators<not_t<Lhs>>
		{
			using _value_type = ValueType;
			using _parameters = std::tuple<Lhs>;

			not_t(Lhs l):
				_lhs(l)
			{}

			not_t(const not_t&) = default;
			not_t(not_t&&) = default;
			not_t& operator=(const not_t&) = default;
			not_t& operator=(not_t&&) = default;
			~not_t() = default;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << "(";
					if (trivial_value_is_null_t<Lhs>::value and _lhs._is_trivial())
					{
						_lhs.serialize(os, db);
						os << " IS NULL";
					}
					else
					{
						os << "NOT";
						_lhs.serialize(os, db);
					}
					os << ")";
				}

		private:
			Lhs _lhs;
		};

	template<typename Lhs, typename O, typename Rhs>
		struct binary_expression_t: public O::_value_type::template operators<binary_expression_t<Lhs, O, Rhs>>
		{
			using _value_type = typename O::_value_type;
			using _parameters = std::tuple<Lhs, Rhs>;

			binary_expression_t(Lhs&& l, Rhs&& r):
				_lhs(std::move(l)), 
				_rhs(std::move(r))
			{}

			binary_expression_t(const Lhs& l, const Rhs& r):
				_lhs(l), 
				_rhs(r)
			{}

			binary_expression_t(const binary_expression_t&) = default;
			binary_expression_t(binary_expression_t&&) = default;
			binary_expression_t& operator=(const binary_expression_t&) = default;
			binary_expression_t& operator=(binary_expression_t&&) = default;
			~binary_expression_t() = default;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << "(";
					_lhs.serialize(os, db);
					os << O::_name;
					_rhs.serialize(os, db);
					os << ")";
				}

		private:
			Lhs _lhs;
			Rhs _rhs;
		};

}

#endif
