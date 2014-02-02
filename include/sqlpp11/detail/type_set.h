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

#ifndef SQLPP_DETAIL_TYPE_SET_H
#define SQLPP_DETAIL_TYPE_SET_H

#include <type_traits>
#include <sqlpp11/vendor/wrong.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename... T>
			struct make_set;

		template<typename T>
			class type_set_element {};

		template<typename SET, typename... T>
			struct is_superset_of_impl
			: std::false_type {};

		template<typename SET>
			struct is_superset_of_impl<SET>
			: std::true_type {};

		template<typename SET, typename T, typename... Rest>
			struct is_superset_of_impl<SET, T, Rest...>
			: std::integral_constant<bool, SET::template contains<T>::value and is_superset_of_impl<SET, Rest...>::value> {};

		template<typename SET, typename... T>
			struct is_disjunct_from_impl
			: std::false_type {};

		template<typename SET>
			struct is_disjunct_from_impl<SET>
			: std::true_type {};

		template<typename SET, typename T, typename... Rest>
			struct is_disjunct_from_impl<SET, T, Rest...>
			: std::integral_constant<bool, not SET::template contains<T>::value and is_disjunct_from_impl<SET, Rest...>::value> {};

		template<typename... Element>
			struct type_set: type_set_element<Element>...
		{
			struct size: std::integral_constant<size_t, sizeof...(Element)> {};

			template<typename T>
				struct contains
				: std::integral_constant<bool, std::is_base_of<type_set_element<T>, type_set>::value> {};

			template<typename T>
				struct is_superset_of
				{
					static_assert(::sqlpp::vendor::wrong_t<T>::value, "invalid argument for is_superset_of");
				};

			template<typename... T>
				struct is_superset_of<type_set<T...>>
				: is_superset_of_impl<type_set, T...>{};

			template<typename T>
				struct join
				{
					static_assert(::sqlpp::vendor::wrong_t<T>::value, "invalid argument for type_set::join");
				};

			template<typename... T>
				struct join<type_set<T...>>
				: make_set<Element..., T...> {};

			template<typename T>
				struct is_disjunct_from
				{
					static_assert(::sqlpp::vendor::wrong_t<T>::value, "invalid argument for is_disjunct_from");
				};

			template<typename... T>
				struct is_disjunct_from<type_set<T...>>
				: is_disjunct_from_impl<type_set, T...>{};

			template<typename T>
				struct is_subset_of
				{
					static_assert(::sqlpp::vendor::wrong_t<T>::value, "invalid argument for is_subset_of");
				};

			template<typename... T>
				struct is_subset_of<type_set<T...>>
				: is_superset_of_impl<type_set<T...>, Element...>{};

			template<typename T>
				struct equals
				{
					static_assert(::sqlpp::vendor::wrong_t<T>::value, "invalid argument for equals");
				};

			template<typename... T>
				struct equals<type_set<T...>>
				: std::integral_constant<bool, 
				is_superset_of_impl<type_set<T...>, Element...>::value 
					and
					is_superset_of_impl<type_set<Element...>, T...>::value> {};

			template<typename T, typename Enable = void>
				struct insert
				{
					using type = type_set;
				};

			template<typename T>
				struct insert<T, typename std::enable_if<not type_set::template contains<T>::value>::type>
				{
					using type = type_set<Element..., T>;
				};

			template<template<typename A> class Predicate, typename T, typename Enable = void>
				struct insert_if
				{
					using type = type_set;
				};

			template<template<typename A> class Predicate, typename T>
				struct insert_if<Predicate, T, typename std::enable_if<not type_set::template contains<T>::value and Predicate<T>::value>::type>
				{
					using type = type_set<Element..., T>;
				};

		};

		template<>
			struct make_set<>
			{
				using type = type_set<>;
			};

		template<typename T, typename... Rest>
			struct make_set<T, Rest...>
			{
				using type = typename make_set<Rest...>::type::template insert<T>::type;
			};

		template<template<typename> class Predicate, typename... T>
			struct make_set_if;

		template<template<typename> class Predicate>
			struct make_set_if<Predicate>
			{
				using type = type_set<>;
			};

		template<template<typename> class Predicate, typename T, typename... Rest>
			struct make_set_if<Predicate, T, Rest...>
			{
				using type = typename make_set_if<Predicate, Rest...>::type::template insert_if<Predicate, T>::type;
			};

		template<template<typename> class Predicate, typename... T>
			struct make_set_if_not
			{
				template<typename A>
					struct InversePredicate
					{
						static constexpr bool value = not Predicate<A>::value;
					};
				using type = typename make_set_if<InversePredicate, T...>::type;
			};

		template<typename... T>
			struct has_duplicates
			: std::integral_constant<bool, make_set<T...>::type::size::value != sizeof...(T)> {};

	}
}


#endif
