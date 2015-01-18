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

#ifndef SQLPP_FLOATING_POINT_H
#define SQLPP_FLOATING_POINT_H

#include <cstdlib>
#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/result_field.h>

namespace sqlpp
{
	// floating_point value type
	struct floating_point
	{
		using _traits = make_traits<floating_point, tag::is_value_type>;
		using _tag = tag::is_floating_point;
		using _cpp_value_type = double;

		template<typename T>
			using _is_valid_operand = is_numeric_t<T>;
	};

	// floating_point parameter type
	template<>
		struct parameter_value_t<floating_point>
		{
			using _value_type = floating_point;
			using _cpp_value_type = typename _value_type::_cpp_value_type;

			parameter_value_t():
				_value(0),
				_is_null(true)
			{}

			parameter_value_t(const _cpp_value_type& value):
				_value(value),
				_is_null(false)
			{}

			parameter_value_t& operator=(const _cpp_value_type& value)
			{
				_value = value;
				_is_null = false;
				return *this;
			}

			parameter_value_t& operator=(const tvin_t<wrap_operand_t<_cpp_value_type>>& t)
			{
				if (t._is_trivial())
				{
					_value = 0;
					_is_null = true;
				}
				else
				{
					_value = t._value._t;
					_is_null = false;
				}
				return *this;
			}

			parameter_value_t& operator=(const std::nullptr_t&)
			{
				_value = 0;
				_is_null = true;
				return *this;
			}

			bool is_null() const
			{ 
				return _is_null; 
			}

			const _cpp_value_type& value() const
			{
				return _value;
			}

			operator _cpp_value_type() const { return _value; }

			template<typename Target>
				void _bind(Target& target, size_t index) const
				{
					target._bind_floating_point_parameter(index, &_value, _is_null);
				}

		private:
			_cpp_value_type _value;
			bool _is_null;
		};

	// floating_point expression operators
	template<typename Expr>
		struct expression_operators<Expr, floating_point>:
			public basic_expression_operators<Expr, floating_point>
		{
			template<typename T>
				using _is_valid_operand = is_valid_operand<floating_point, T>;

			template<typename T>
				plus_t<Expr, floating_point, wrap_operand_t<T>> operator +(T t) const
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_operand<rhs>::value, "invalid rhs operand");

					return { *static_cast<const Expr*>(this), rhs{t} };
				}

			template<typename T>
				minus_t<Expr, floating_point, wrap_operand_t<T>> operator -(T t) const
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_operand<rhs>::value, "invalid rhs operand");

					return { *static_cast<const Expr*>(this), rhs{t} };
				}

			template<typename T>
				multiplies_t<Expr, floating_point, wrap_operand_t<T>> operator *(T t) const
				{
					using rhs = wrap_operand_t<T>;

					return { *static_cast<const Expr*>(this), rhs{t} };
				}

			template<typename T>
				divides_t<Expr, wrap_operand_t<T>> operator /(T t) const
				{
					using rhs = wrap_operand_t<T>;

					return { *static_cast<const Expr*>(this), rhs{t} };
				}

			unary_plus_t<floating_point, Expr> operator +() const
			{
				return { *static_cast<const Expr*>(this) };
			}

			unary_minus_t<floating_point, Expr> operator -() const
			{
				return { *static_cast<const Expr*>(this) };
			}
		};

	// floating_point column operators
	template<typename Column>
		struct column_operators<Column, floating_point>
		{
			template<typename T>
				using _is_valid_operand = is_valid_operand<floating_point, T>;

			template<typename T>
				auto operator +=(T t) const -> assignment_t<Column, plus_t<Column, floating_point, wrap_operand_t<T>>>
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

					return { *static_cast<const Column*>(this), { *static_cast<const Column*>(this), rhs{t} } };
				}

			template<typename T>
				auto operator -=(T t) const -> assignment_t<Column, minus_t<Column, floating_point, wrap_operand_t<T>>>
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

					return { *static_cast<const Column*>(this), { *static_cast<const Column*>(this), rhs{t} } };
				}

			template<typename T>
				auto operator /=(T t) const -> assignment_t<Column, divides_t<Column, wrap_operand_t<T>>>
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

					return { *static_cast<const Column*>(this), { *static_cast<const Column*>(this), rhs{t} } };
				}

			template<typename T>
				auto operator *=(T t) const -> assignment_t<Column, multiplies_t<Column, floating_point, wrap_operand_t<T>>>
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

					return { *static_cast<const Column*>(this), { *static_cast<const Column*>(this), rhs{t} } };
				}
		};
	// floating_point result field
	template<typename Db, typename FieldSpec>
		struct result_field_t<floating_point, Db, FieldSpec>: public result_field_methods_t<result_field_t<floating_point, Db, FieldSpec>>
		{
			static_assert(std::is_same<value_type_of<FieldSpec>, floating_point>::value, "field type mismatch");
			using _cpp_value_type = typename floating_point::_cpp_value_type;

			result_field_t():
				_is_valid(false),
				_is_null(true),
				_value(0)
			{}

			void _validate()
			{
				_is_valid = true;
			}

			void _invalidate()
			{
				_is_valid = false;
				_is_null = true;
				_value = 0;
			}

			bool is_null() const
			{ 
				if (not _is_valid)
					throw exception("accessing is_null in non-existing row");
			return _is_null; 
		}

		bool _is_trivial() const
		{
			if (not _is_valid)
				throw exception("accessing is_null in non-existing row");

			return value() == 0;
		}

		_cpp_value_type value() const
		{
			if (not _is_valid)
				throw exception("accessing value in non-existing row");

			if (_is_null)
			{
				if (enforce_null_result_treatment_t<Db>::value and not null_is_trivial_value_t<FieldSpec>::value)
				{
					throw exception("accessing value of NULL field");
				}
				else
				{
					return 0;
				}
			}
			return _value;
		}

		template<typename Target>
			void _bind(Target& target, size_t i)
			{
				target._bind_floating_point_result(i, &_value, &_is_null);
			}

	private:
		bool _is_valid;
		bool _is_null;
		_cpp_value_type _value;
	};

	template<typename Db, typename FieldSpec>
		inline std::ostream& operator<<(std::ostream& os, const result_field_t<floating_point, Db, FieldSpec>& e)
		{
			return serialize(e, os);
		}

}
#endif
