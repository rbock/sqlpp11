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

#ifndef SQLPP_PARAMETER_LIST_H
#define SQLPP_PARAMETER_LIST_H

#include <sqlpp11/detail/wrong.h>
#include <tuple>

namespace sqlpp
{
	template<typename T>
		struct parameter_list_t
		{
			static_assert(detail::wrong<T>::value, "Template parameter for parameter_list_t has to be a tuple");
		};

	template<typename... Parameter>
		struct parameter_list_t<std::tuple<Parameter...>>: public Parameter::_member_t...
	{
		parameter_list_t():
			Parameter::_member_t()...,
			_parameter_tuple(static_cast<typename Parameter::_member_t&>(*this)()...)
		{}

		parameter_list_t(const parameter_list_t& rhs)
			noexcept(std::is_nothrow_copy_constructible<std::tuple<typename Parameter::_member_t...>>::value):
			Parameter::_member_t(static_cast<const typename Parameter::_member_t&>(rhs))...,
			_parameter_tuple(static_cast<typename Parameter::_member_t&>(*this)()...)
		{}

		parameter_list_t(parameter_list_t&& rhs)
			noexcept(std::is_nothrow_move_constructible<std::tuple<typename Parameter::_member_t...>>::value):
			Parameter::_member_t(std::move(static_cast<typename Parameter::_member_t&>(rhs)))...,
			_parameter_tuple(static_cast<typename Parameter::_member_t&>(*this)()...)
		{}

		parameter_list_t& operator=(const parameter_list_t& rhs)
			noexcept(std::is_nothrow_copy_assignable<std::tuple<typename Parameter::_member_t&...>>::value)
		{
			_parameter_tuple = rhs._parameter_tuple;
			return *this;
		}

		parameter_list_t& operator=(parameter_list_t&& rhs)
			noexcept(std::is_nothrow_move_assignable<std::tuple<typename Parameter::_member_t&...>>::value)
		{
			_parameter_tuple = std::move(rhs._parameter_tuple);
			return *this;
		}

		~parameter_list_t() = default;

		using parameter_tuple_t = std::tuple<typename Parameter::_value_type::_cpp_value_type&...>;
		using size = std::tuple_size<parameter_tuple_t>;
	 	parameter_tuple_t _parameter_tuple;
	};

	namespace detail
	{
		template<typename Exp, typename Enable = void>
			struct get_parameter_tuple
			{
				using type = std::tuple<>;
			};

		template<typename Exp>
			struct get_parameter_tuple<Exp, typename std::enable_if<is_parameter_t<Exp>::value, void>::type>
			{
				using type = std::tuple<Exp>;
			};

		template<typename... Param>
			struct get_parameter_tuple<std::tuple<Param...>, void>
			{
				// cat together parameter tuples
				using type = decltype(std::tuple_cat(std::declval<typename get_parameter_tuple<Param>::type>()...));
			};

		template<typename Exp>
			struct get_parameter_tuple<Exp, typename std::enable_if<not std::is_same<typename Exp::_parameter_t, void>::value, void>::type>
			{
				using type = typename get_parameter_tuple<typename Exp::_parameter_t>::type;
			};

	}

	template<typename Exp>
		struct make_parameter_list_t
		{
			using type = parameter_list_t<typename detail::get_parameter_tuple<typename std::decay<Exp>::type>::type>;
		};

}

#endif
