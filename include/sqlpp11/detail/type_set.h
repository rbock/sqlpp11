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

#ifndef SQLPP_DETAIL_TYPE_SET_H
#define SQLPP_DETAIL_TYPE_SET_H

#include <tuple>
#include <type_traits>
#include <sqlpp11/vendor/wrong.h>
#include <sqlpp11/detail/logic.h>

namespace sqlpp
{
	namespace detail
	{
		// some forward declarations and helpers
		template<typename... T>
			struct make_type_set;

		template<typename E, typename SET>
			struct is_element_of;

		// A type set
		template<typename... Elements>
			struct type_set
		{
			using size = std::integral_constant<size_t, sizeof...(Elements)>;
			using _is_type_set = std::true_type;

			static_assert(std::is_same<type_set, typename make_type_set<Elements...>::type>::value, "use make_type_set to construct a set");

			template<typename T>
				struct insert
				{
					using type = typename std::conditional<not is_element_of<T, type_set>::value,
								type_set<T, Elements...>,
								type_set>::type;
				};

			template<template<typename A> class Predicate, typename T>
				struct insert_if
				{
					using type = typename std::conditional<Predicate<T>::value and not is_element_of<T, type_set>::value,
								type_set<Elements..., T>,
								type_set>::type;
				};
		};


		template<typename E, typename SET>
			struct is_element_of
			{
				static_assert(::sqlpp::vendor::wrong_t<E, SET>::value, "SET has to be a type set");
			};

		template<typename E, typename... Elements>
			struct is_element_of<E, type_set<Elements...>>
			{
				template<typename X>
					using matchE = std::is_same<E, X>;
				static constexpr bool value = any_t<matchE, Elements...>::value;
			};

		template<typename L, typename R>
			struct is_superset_of
			{
				static_assert(::sqlpp::vendor::wrong_t<L, R>::value, "L and R have to be type sets");
			};

		template<typename... LElements, typename... RElements>
			struct is_superset_of<type_set<LElements...>, type_set<RElements...>>
			{
				template<typename X>
					using is_element_of_L = is_element_of<X, type_set<LElements...>>;
				static constexpr bool value = all_t<is_element_of_L, RElements...>::value;
			};

		template<typename L, typename R>
			struct is_subset_of
			{
				static constexpr bool value = is_superset_of<R, L>::value;
			};

		template<typename L, typename R>
			struct joined_set
			{
				static_assert(::sqlpp::vendor::wrong_t<L, R>::value, "L and R have to be type sets");
			};

		template<typename... LElements, typename... RElements>
			struct joined_set<type_set<LElements...>, type_set<RElements...>>
			{
				using type = typename make_type_set<LElements..., RElements...>::type;
			};

		template<typename L, typename R>
			struct is_disjunct_from
			{
				static_assert(::sqlpp::vendor::wrong_t<L, R>::value, "invalid argument for is_disjunct_from");
			};

		template<typename... LElements, typename... RElements>
			struct is_disjunct_from<type_set<LElements...>, type_set<RElements...>>
			{
				template<typename X>
					using is_element_of_L = is_element_of<X, type_set<LElements...>>;
				template<typename X>
					using is_element_of_R = is_element_of<X, type_set<RElements...>>;
				static constexpr bool value = 
					not(any_t<is_element_of_L, RElements...>::value or any_t<is_element_of_R, LElements...>::value);
			};

		template<>
			struct make_type_set<>
			{
				using type = type_set<>;
			};

		template<typename T, typename... Rest>
			struct make_type_set<T, Rest...>
			{
				using type = typename make_type_set<Rest...>::type::template insert<T>::type;
			};

		template<template<typename> class Predicate, typename... T>
			struct make_type_set_if;

		template<template<typename> class Predicate>
			struct make_type_set_if<Predicate>
			{
				using type = type_set<>;
			};

		template<template<typename> class Predicate, typename T, typename... Rest>
			struct make_type_set_if<Predicate, T, Rest...>
			{
				using type = typename make_type_set_if<Predicate, Rest...>::type::template insert_if<Predicate, T>::type;
			};

		template<template<typename> class Predicate, typename... T>
			struct make_type_set_if_not
			{
				template<typename X>
				using InversePredicate = std::integral_constant<bool, not Predicate<X>::value>;
				using type = typename make_type_set_if<InversePredicate, T...>::type;
			};

		template<typename... T>
			using has_duplicates = std::integral_constant<bool, make_type_set<T...>::type::size::value != sizeof...(T)>;

		template<typename... T>
			struct make_joined_set
			{
				static_assert(::sqlpp::vendor::wrong_t<T...>::value, "invalid argument for joined set");
			};

		template<>
			struct make_joined_set<>
			{
				using type = type_set<>;
			};

		template<typename... E, typename... T>
			struct make_joined_set<type_set<E...>, T...>
			{
				using _rest = typename make_joined_set<T...>::type;
				
				using type = typename joined_set<type_set<E...>, _rest>::type;
			};

	}
}


#endif
