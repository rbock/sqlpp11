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
			struct make_set;

		// A type set
		template<typename... Element>
			struct type_set
		{
			using _elements_t = std::tuple<Element...>;
			using size = std::tuple_size<_elements_t>;
			using _is_type_set = std::true_type;

			template<typename T>
				struct count
			{
				template<typename E>
					using same = std::is_same<T, E>;
				static constexpr bool value = or_t<same, Element...>::value;
			};

			template<typename T>
				struct is_superset_of
				{
					static_assert(::sqlpp::vendor::wrong_t<T>::value, "invalid argument for is_superset_of");
				};

			template<typename... T>
				struct is_superset_of<type_set<T...>>
				{
					static constexpr bool value = and_t<count, T...>::value;
				};

			template<typename T>
				struct join
				{
					static_assert(::sqlpp::vendor::wrong_t<T>::value, "invalid argument for type_set::join");
				};

			template<typename... T>
				struct join<type_set<T...>>
				{
					using type = typename make_set<Element..., T...>::type;
				};

			template<typename T>
				struct is_subset_of
				{
					static_assert(::sqlpp::vendor::wrong_t<T>::value, "invalid argument for is_subset_of");
				};

			template<typename... T>
				struct is_subset_of<type_set<T...>>
				{
					static constexpr bool value = type_set<T...>::template is_superset_of<type_set>::value;
				};

			template<typename T>
				struct is_disjunct_from
				{
					static_assert(::sqlpp::vendor::wrong_t<T>::value, "invalid argument for is_disjunct_from");
				};

			template<typename... T>
				struct is_disjunct_from<type_set<T...>>
				{
					static constexpr bool value = not(or_t<type_set::count, T...>::value or or_t<type_set<T...>::template count, Element...>::value);
				};

			template<typename T>
				struct insert
				{
					using type = typename std::conditional<count<T>::value,
								type_set,
								type_set<Element..., T>>::type;
				};

			template<template<typename A> class Predicate, typename T>
				struct insert_if
				{
					using type = typename std::conditional<Predicate<T>::value,
								type_set<Element..., T>,
								type_set>::type;
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
				template<typename X>
				using InversePredicate = std::integral_constant<bool, not Predicate<X>::value>;
				using type = typename make_set_if<InversePredicate, T...>::type;
			};

		template<typename... T>
			using has_duplicates = std::integral_constant<bool, make_set<T...>::type::size::value != sizeof...(T)>;

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

		/*
		template<typename... E>
			struct make_joined_set<type_set<E...>>
			{
				using type = type_set<E...>;
			};
			*/

		template<typename... E, typename... T>
			struct make_joined_set<type_set<E...>, T...>
			{
				using _rest = typename make_joined_set<T...>::type;
				
				using type = typename type_set<E...>::template join<_rest>::type;
			};

	}
}


#endif
