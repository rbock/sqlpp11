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

#ifndef SQLPP_COLUMN_H
#define SQLPP_COLUMN_H

#include <sqlpp11/alias.h>
#include <sqlpp11/column_fwd.h>
#include <sqlpp11/default_value.h>
#include <sqlpp11/null.h>
#include <sqlpp11/sort_order.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/vendor/assignment.h>
#include <sqlpp11/vendor/expression.h>
#include <sqlpp11/vendor/interpreter.h>
#include <sqlpp11/vendor/wrong.h>

namespace sqlpp
{
	template<typename Table, typename ColumnSpec>
	struct column_t: public ColumnSpec::_value_type::template operators<column_t<Table, ColumnSpec>>
	{ 
		using _is_column = std::true_type;
		using _spec_t = ColumnSpec;
		using _table = Table;
		using _column_type = typename ColumnSpec::_column_type;
		struct _value_type: ColumnSpec::_value_type
		{
			using _is_expression = std::true_type;
			using _is_named_expression = std::true_type;
			using _is_alias = std::false_type;
		};

		using _name_t = typename ColumnSpec::_name_t;

		column_t() = default;
		column_t(const column_t&) = default;
		column_t(column_t&&) = default;
		column_t& operator=(const column_t&) = default;
		column_t& operator=(column_t&&) = default;
		~column_t() = default;

		static constexpr const char* _get_name()
		{
			return _name_t::_get_name();
		}

		template<typename alias_provider>
			expression_alias_t<column_t, alias_provider> as(const alias_provider&) const
			{
				return { *this };
			}

		template<typename T>
			auto operator =(T t) const
			-> typename std::enable_if<_value_type::template _constraint<typename vendor::wrap_operand<T>::type>::value and not std::is_same<column_t, T>::value, 
			     vendor::assignment_t<column_t, typename vendor::wrap_operand<T>::type>>::type
			{
				return { *this, {t} };
			}

		auto operator =(sqlpp::null_t) const
			->vendor::assignment_t<column_t, sqlpp::null_t>
			{
				static_assert(can_be_null_t<column_t>::value, "column cannot be null");
				return { *this, {} };
			}

		auto operator =(sqlpp::default_value_t) const
			->vendor::assignment_t<column_t, sqlpp::default_value_t>
			{
				return { *this, {} };
			}
	};

	namespace vendor
	{
		template<typename Context, typename... Args>
			struct interpreter_t<Context, column_t<Args...>>
			{
				using T = column_t<Args...>;

				static Context& _(const T& t, Context& context)
				{
					context << T::_table::_name_t::_get_name() << '.' << T::_name_t::_get_name();
					return context;
				}
			};

	}
}

#endif
