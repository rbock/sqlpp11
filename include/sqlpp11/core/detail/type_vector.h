#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include <sqlpp11/core/wrong.h>
#include <sqlpp11/core/logic.h>

namespace sqlpp
{
  namespace detail
  {
    template <typename... T>
    struct are_same;

    template <>
    struct are_same<> : public std::true_type
    {
    };

    template <typename First, typename... T>
    struct are_same<First, T...> : public std::is_same<are_same<First, T...>, are_same<T..., First>>
    {
    };

    template <typename... T>
    struct type_vector
    {
      static constexpr bool are_same()
      {
        return ::sqlpp::detail::are_same<T...>::value;
      }

      template <typename X>
      struct contains : public ::sqlpp::logic::any<std::is_same<T, X>::value...> {
      };

      template <typename X>
      struct contains_not : public ::sqlpp::logic::none<std::is_same<T, X>::value...> {
      };

      template <typename TypeVector>
      struct contains_all;

      template <typename... X>
      struct contains_all<type_vector<X...>> : public ::sqlpp::logic::all<contains<X>::value...>
      {
      };

      template <typename... X>
      static constexpr bool contains_any(type_vector<X...>)
      {
        return ::sqlpp::logic::any<contains<X>::value...>::value;
      }

      template <typename... X>
      static constexpr bool contains_none(type_vector<X...>)
      {
        return ::sqlpp::logic::none<contains<X>::value...>::value;
      }

      static constexpr size_t size()
      {
        return sizeof...(T);
      }

      static constexpr bool empty()
      {
        return size() == 0;
      }
    };

    template <typename... T>
    struct type_vector_cat_impl
    {
      static_assert(wrong_t<type_vector_cat_impl>::value, "type_vector_cat must be called with type_vector arguments");
    };

    template <>
    struct type_vector_cat_impl<>
    {
      using type = type_vector<>;
    };

    template <typename... T>
    struct type_vector_cat_impl<type_vector<T...>>
    {
      using type = type_vector<T...>;
    };

    template <typename... L, typename... R>
    struct type_vector_cat_impl<type_vector<L...>, type_vector<R...>>
    {
      using type = type_vector<L..., R...>;
    };

    template <typename... L, typename... Rest>
    struct type_vector_cat_impl<type_vector<L...>, Rest...>
    {
      using type = typename type_vector_cat_impl<type_vector<L...>, typename type_vector_cat_impl<Rest...>::type>::type;
    };

    template <typename... T>
    using type_vector_cat_t = typename type_vector_cat_impl<T...>::type;

    template <typename TypeVector, template <typename> class Transform>
    struct transform;

    template <template <typename> class Transform, typename... T>
    struct transform<type_vector<T...>, Transform>
    {
      using type = type_vector<typename Transform<T>::type...>;
    };

    template <typename TypeVector, template <typename> class Transform>
    using transform_t = typename transform<TypeVector, Transform>::type;

    template <typename TypeVector, template <typename> class Predicate>
    struct copy_if;

    template <template <typename> class Predicate, typename... T>
    struct copy_if<type_vector<T...>, Predicate>
    {
      using type =
          type_vector_cat_t<typename std::conditional<Predicate<T>::value, type_vector<T>, type_vector<>>::type...>;
    };

    template <typename TypeVector, template <typename> class Predicate>
    using copy_if_t = typename copy_if<TypeVector, Predicate>::type;

  }  // namespace detail
}  // namespace sqlpp
