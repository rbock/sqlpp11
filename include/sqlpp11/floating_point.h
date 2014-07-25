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

namespace sqlpp
{
	namespace detail
	{

		// floating_point value type
		struct floating_point
		{
			using _tag = ::sqlpp::tag::floating_point;
			using _cpp_value_type = double;

			struct _parameter_t
			{
				using _value_type = floating_point;

				_parameter_t():
					_value(0),
					_is_null(true)
				{}

				_parameter_t(const _cpp_value_type& value):
					_value(value),
					_is_null(false)
				{}

				_parameter_t& operator=(const _cpp_value_type& value)
				{
					_value = value;
					_is_null = false;
					return *this;
				}

				_parameter_t& operator=(const std::nullptr_t&)
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

			template<typename Db, typename FieldSpec>
				struct _result_field_t;

			// I am SO waiting for concepts lite!
			template<typename Field, typename Enable = void>
				struct field_methods_t
				{
					operator _cpp_value_type() const { return static_cast<const Field&>(*this).value(); }
				};

			template<typename Db, typename FieldSpec>
				struct field_methods_t<
						_result_field_t<Db, FieldSpec>, 
				    typename std::enable_if<connector_enforce_result_validity_t<Db>::value 
							and column_spec_can_be_null_t<FieldSpec>::value
							and not null_is_trivial_value_t<FieldSpec>::value>::type>
				{
				};

			template<typename Db, typename FieldSpec>
				struct _result_field_t: public field_methods_t<_result_field_t<Db, FieldSpec>>
				{
					using _value_type = integral;

					_result_field_t():
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

					_cpp_value_type value() const
					{
						if (not _is_valid)
							throw exception("accessing value in non-existing row");

						if (_is_null)
						{
							if (connector_enforce_result_validity_t<Db>::value and not null_is_trivial_value_t<FieldSpec>::value)
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

			template<typename T>
				struct _is_valid_operand
				{
					static constexpr bool value = 
						is_expression_t<T>::value // expressions are OK
						and is_numeric_t<T>::value // the correct value type is required, of course
						;
				};

			template<typename Base>
				struct expression_operators: public basic_expression_operators<Base, is_numeric_t>
			{
				template<typename T>
					plus_t<Base, floating_point, wrap_operand_t<T>> operator +(T t) const
					{
						using rhs = wrap_operand_t<T>;
						static_assert(_is_valid_operand<rhs>::value, "invalid rhs operand");

						return { *static_cast<const Base*>(this), rhs{t} };
					}

				template<typename T>
					minus_t<Base, floating_point, wrap_operand_t<T>> operator -(T t) const
					{
						using rhs = wrap_operand_t<T>;
						static_assert(_is_valid_operand<rhs>::value, "invalid rhs operand");

						return { *static_cast<const Base*>(this), rhs{t} };
					}

				template<typename T>
					multiplies_t<Base, floating_point, wrap_operand_t<T>> operator *(T t) const
					{
						using rhs = wrap_operand_t<T>;

						return { *static_cast<const Base*>(this), rhs{t} };
					}

				template<typename T>
					divides_t<Base, wrap_operand_t<T>> operator /(T t) const
					{
						using rhs = wrap_operand_t<T>;

						return { *static_cast<const Base*>(this), rhs{t} };
					}

				unary_plus_t<floating_point, Base> operator +() const
				{
					return { *static_cast<const Base*>(this) };
				}

				unary_minus_t<floating_point, Base> operator -() const
				{
					return { *static_cast<const Base*>(this) };
				}
			};

			template<typename Base>
				struct column_operators
				{
					template<typename T>
						auto operator +=(T t) const -> assignment_t<Base, plus_t<Base, floating_point, wrap_operand_t<T>>>
						{
							using rhs = wrap_operand_t<T>;
							static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

							return { *static_cast<const Base*>(this), { *static_cast<const Base*>(this), rhs{t} } };
						}

					template<typename T>
						auto operator -=(T t) const -> assignment_t<Base, minus_t<Base, floating_point, wrap_operand_t<T>>>
						{
							using rhs = wrap_operand_t<T>;
							static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

							return { *static_cast<const Base*>(this), { *static_cast<const Base*>(this), rhs{t} } };
						}

					template<typename T>
						auto operator /=(T t) const -> assignment_t<Base, divides_t<Base, wrap_operand_t<T>>>
						{
							using rhs = wrap_operand_t<T>;
							static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

							return { *static_cast<const Base*>(this), { *static_cast<const Base*>(this), rhs{t} } };
						}

					template<typename T>
						auto operator *=(T t) const -> assignment_t<Base, multiplies_t<Base, floating_point, wrap_operand_t<T>>>
						{
							using rhs = wrap_operand_t<T>;
							static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

							return { *static_cast<const Base*>(this), { *static_cast<const Base*>(this), rhs{t} } };
						}
				};
		};

		template<typename Db, typename FieldSpec>
			inline std::ostream& operator<<(std::ostream& os, const floating_point::_result_field_t<Db, FieldSpec>& e)
			{
				return os << e.value();
			}
	}

	using floating_point = detail::floating_point;

}
#endif
