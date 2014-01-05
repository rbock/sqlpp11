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
	namespace detail
	{
		template<typename... T>
			struct or_t;

		template<typename T, typename... Rest>
			struct or_t<T, Rest...>
			{
				static constexpr bool value = T::value or or_t<Rest...>::value;
			};

		template<>
			struct or_t<>
			{
				static constexpr bool value = false;
			};

	}

	template<typename T>
		struct parameter_list_t
		{
			static_assert(detail::wrong<T>::value, "Template parameter for parameter_list_t has to be a tuple");
		};

	template<typename... Parameter>
		struct parameter_list_t<std::tuple<Parameter...>>: public Parameter::_instance_t...
		{
			using _member_tuple_t = std::tuple<typename Parameter::_instance_t...>;
			using size = std::integral_constant<std::size_t, sizeof...(Parameter)>;
			using _contains_trivial_value_is_null_t = detail::or_t<typename Parameter::_trivial_value_is_null_t...>;

			parameter_list_t():
				Parameter::_instance_t({typename Parameter::_trivial_value_is_null_t()})...
			{}

			template<typename Target>
				void _bind(Target& target) const
				{
					_bind_impl(target, index_t<0>());
				}

		private:
			template<size_t> struct index_t {}; // this is just for overloading

			template<typename Target, size_t index>
				void _bind_impl(Target& target, const index_t<index>&) const
				{
					const auto& parameter = static_cast<typename std::tuple_element<index, const _member_tuple_t>::type&>(*this)();
					parameter.bind(target, index);
					_bind_impl(target, index_t<index + 1>());
				}

			template<typename Target>
				void _bind_impl(Target& target, const index_t<size::value>&) const
				{
				}
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
			struct get_parameter_tuple<Exp, typename std::enable_if<not std::is_same<typename Exp::_parameter_tuple_t, void>::value, void>::type>
			{
				using type = typename get_parameter_tuple<typename Exp::_parameter_tuple_t>::type;
			};

	}

	template<typename Exp>
		struct make_parameter_list_t
		{
			using type = parameter_list_t<typename detail::get_parameter_tuple<typename std::decay<Exp>::type>::type>;
		};

	template<typename T>
	size_t set_parameter_index(T& t, size_t index);

	namespace detail
	{
		template<typename Exp, typename Enable = void>
			struct set_parameter_index_t
			{
				size_t operator()(Exp& e, size_t index)
				{
					return index;
				}
			};

		template<typename Exp>
			struct set_parameter_index_t<Exp, typename std::enable_if<is_parameter_t<Exp>::value, void>::type>
			{
				size_t operator()(Exp& e, size_t index)
				{
					return e._set_parameter_index(index);
				}
			};

		template<typename... Param>
			struct set_parameter_index_t<std::tuple<Param...>, void>
			{
				template<size_t> struct type{};

				size_t operator()(std::tuple<Param...>& t, size_t index)
				{
					return impl(t, index, type<0>());
				}
			private:
				template<size_t pos>
				size_t impl(std::tuple<Param...>& t, size_t index, const type<pos>&)
				{
					index = sqlpp::set_parameter_index(std::get<pos>(t), index);
					return impl(t, index, type<pos + 1>());
				}

				size_t impl(std::tuple<Param...>& t, size_t index, const type<sizeof...(Param)>&)
				{
					return index;
				}
			};

		template<typename Exp>
			struct set_parameter_index_t<Exp, typename std::enable_if<not std::is_same<typename Exp::_parameter_tuple_t, void>::value, void>::type>
			{
				size_t operator()(Exp& e, size_t index)
				{
					return e._set_parameter_index(index);
				}
			};

	}


	template<typename T>
	size_t set_parameter_index(T& t, size_t index)
	{
		return detail::set_parameter_index_t<T>()(t, index);
	}

}

#endif
