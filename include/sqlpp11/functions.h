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

#ifndef SQLPP_FUNCTIONS_H
#define SQLPP_FUNCTIONS_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/column_types.h>
#include <sstream>

namespace sqlpp
{
#define SQLPP_MAKE_UNARY_TYPED_FUNCTION(NAME, SQL, CONSTRAINT, VALUE_TYPE) \
	namespace detail\
	{\
		template<typename T>\
			struct NAME##_t\
			{\
				static_assert(CONSTRAINT<typename std::decay<T>::type>::value, #NAME "() argument has constraint: " #CONSTRAINT);\
				static_assert(is_expression_t<typename std::decay<T>::type>::value, #NAME "() argument has contraint: is_expression_t");\
				using _operand = typename operand_t<T, CONSTRAINT>::type;\
				\
				struct _op\
				{\
					struct _value_type: public VALUE_TYPE\
					{\
						using _is_named_expression = tag_yes;\
					};\
					struct _name_t\
					{\
						static constexpr const char* _get_name() { return #SQL; }\
					};\
					template<typename M>\
					struct _member_t\
					{\
						M NAME;\
					};\
				};\
				\
				using type = named_nary_function_t<_op, _operand>;\
			};\
	}\
	template<typename T>\
	auto NAME(T&& t) -> typename detail::NAME##_t<T>::type\
	{\
		return { std::forward<T>(t) };\
	}

#define SQLPP_MAKE_UNARY_FUNCTION(NAME, SQL, CONSTRAINT) SQLPP_MAKE_UNARY_TYPED_FUNCTION(NAME, SQL, CONSTRAINT, _operand::_value_type)

	SQLPP_MAKE_UNARY_FUNCTION(avg, AVG, is_value_t);
	SQLPP_MAKE_UNARY_FUNCTION(min, MIN, is_value_t);
	SQLPP_MAKE_UNARY_FUNCTION(max, MAX, is_value_t);
	SQLPP_MAKE_UNARY_FUNCTION(sum, SUM, is_value_t);
	SQLPP_MAKE_UNARY_FUNCTION(any, ANY, is_select_t);
	SQLPP_MAKE_UNARY_FUNCTION(some, SOME, is_select_t);
	SQLPP_MAKE_UNARY_TYPED_FUNCTION(count, COUNT, is_expression_t, bigint);
	SQLPP_MAKE_UNARY_TYPED_FUNCTION(exists, EXISTS, is_select_t, boolean);
	SQLPP_MAKE_UNARY_TYPED_FUNCTION(not_exists, NOT EXISTS, is_select_t, boolean);

	template<typename T>
		auto value(T&& t) -> typename operand_t<T, is_value_t>::type
		{
			static_assert(not is_value_t<typename std::decay<T>::type>::value, "value() is to be called with non-sqlpp-type like int, or string");
			return { std::forward<T>(t) };
		}

	template<typename ValueType>
	struct verbatim_t: public ValueType::template operators<verbatim_t<ValueType>>
	{
		using _value_type = ValueType;

		template<typename Db>
		void serialize(std::ostream& os, const Db& db) const
		{
			os << _verbatim;
		}

		verbatim_t(const std::string& verbatim): _verbatim(verbatim) {}
		verbatim_t(std::string&& verbatim): _verbatim(std::forward<std::string>(verbatim)) {}
		verbatim_t(const verbatim_t&) = default;
		verbatim_t(verbatim_t&&) = default;
		verbatim_t& operator=(const verbatim_t&) = default;
		verbatim_t& operator=(verbatim_t&&) = default;
		~verbatim_t() = default;

		std::string _verbatim;
	};

	template<typename ValueType, typename StringType>
		auto verbatim(StringType&& s) -> verbatim_t<ValueType>
		{
			return { std::forward<StringType>(s) };
		}

	template<typename Expression, typename Db>
		auto flatten(const Expression& exp, const Db& db) -> verbatim_t<typename std::decay<Expression>::type::_value_type::_base_value_type>
		{
			std::ostringstream os;
			exp.serialize(os, db);
			return { os.str() };
		}
	
	template<typename Container>
		struct value_list_t // to be used in .in() method
		{
			using _container_t = Container;
			using _value_type = typename operand_t<typename _container_t::value_type, is_value_t>::type::_value_type;
			using _iterator = decltype(std::begin(std::declval<_container_t>()));

			template<typename Db>
				void serialize(std::ostream& os, const Db& db) const
				{
					bool first = true;
					for (const auto& entry: _container)
					{
						if (first)
							first = false;
						else
							os << ',';

						value(entry).serialize(os, db);
					}
				}

			_container_t _container;
		};

	template<typename Container>
	auto value_list(Container&& c) -> value_list_t<typename std::decay<Container>::type>
	{
		static_assert(not is_value_t<typename std::decay<Container>::type::value_type>::value, "value_list() is to be called with a container of non-sqlpp-type like std::vector<int>, or std::list(string)");
		return { std::forward<Container>(c) };
	}

	template<typename T>
		constexpr const char* get_sql_name(const T&) 
		{
			return std::decay<T>::type::_name_t::_get_name();
		}


}

#endif
