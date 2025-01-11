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

#include <type_traits>
#include <stddef.h>
#include <sqlpp11/core/wrong.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/name/char_sequence.h>
#include <sqlpp11/core/name/name_tag.h>

namespace sqlpp
{
  namespace detail
  {
    // some forward declarations and helpers
    template <typename... T>
    struct make_type_set;

    template <typename... T>
    using make_type_set_t = typename make_type_set<T...>::type;

    template <typename E, typename SET>
    struct is_element_of;

    template <typename T>
    struct _base
    {
    };

    // A type set
    template <typename... Elements>
    struct type_set
    {
    private:
      struct _impl : _base<Elements>...
      {
      };

    public:
      static constexpr size_t size()
      {
        return sizeof...(Elements);
      }

      static constexpr bool empty()
      {
        return size() == 0;
      }

      template <typename T>
      static constexpr bool count()
      {
        return std::is_base_of<_base<T>, _impl>::value;
      }

      template <typename T>
      static constexpr bool contains()
      {
        return count<T>();
      }

      template <typename... X>
      static constexpr bool contains_any(type_set<X...>)
      {
        return ::sqlpp::logic::any<contains<X>()...>::value;
      }

      template <typename... X>
      static constexpr bool contains_all(type_set<X...>)
      {
        return ::sqlpp::logic::all<contains<X>()...>::value;
      }

      template <typename... X>
      static constexpr bool contains_none(type_set<X...>)
      {
        return ::sqlpp::logic::none<contains<X>()...>::value;
      }

      template <typename T>
      struct insert
      {
        using type = typename std::conditional<count<T>(), type_set, type_set<T, Elements...>>::type;
      };

      template <template <typename A> class Predicate, typename T>
      struct insert_if
      {
        using type =
            typename std::conditional<Predicate<T>::value and not count<T>(), type_set<Elements..., T>, type_set>::type;
      };
    };

    template <typename E, typename SET>
    struct is_element_of
    {
      static_assert(wrong_t<is_element_of>::value, "SET has to be a type set");
    };

    template <typename E, typename... Elements>
    struct is_element_of<E, type_set<Elements...>>
    {
      static constexpr bool value = type_set<Elements...>::template count<E>();
    };

    template <>
    struct make_type_set<>
    {
      using type = type_set<>;
    };

    template <typename T, typename... Rest>
    struct make_type_set<T, Rest...>
    {
      using type = typename make_type_set<Rest...>::type::template insert<T>::type;
    };

    template <template <typename> class Predicate, typename... T>
    struct make_type_set_if;

    template <template <typename> class Predicate>
    struct make_type_set_if<Predicate>
    {
      using type = type_set<>;
    };

    template <template <typename> class Predicate, typename T, typename... Rest>
    struct make_type_set_if<Predicate, T, Rest...>
    {
      using type = typename make_type_set_if<Predicate, Rest...>::type::template insert_if<Predicate, T>::type;
    };

    template <template <typename> class Predicate, typename... T>
    using make_type_set_if_t = typename make_type_set_if<Predicate, T...>::type;

    template <template <typename> class Predicate, typename... T>
    struct make_type_set_if_not
    {
      template <typename X>
      using InversePredicate = std::integral_constant<bool, not Predicate<X>::value>;
      using type = typename make_type_set_if<InversePredicate, T...>::type;
    };

    template <template <typename> class Predicate, typename... T>
    using make_type_set_if_not_t = typename make_type_set_if_not<Predicate, T...>::type;

    template <typename... T>
    using has_duplicates = std::integral_constant<bool, make_type_set_t<T...>::size() != sizeof...(T)>;

    template <typename... T>
    struct make_joined_set
    {
      static_assert(wrong_t<make_joined_set>::value, "invalid argument for joined set");
    };

    template <>
    struct make_joined_set<>
    {
      using type = type_set<>;
    };

    template <typename... Es>
    struct make_joined_set<type_set<Es...>>
    {
      using type = type_set<Es...>;
    };

    template <typename... Ls, typename... Rs>
    struct make_joined_set<type_set<Ls...>, type_set<Rs...>>
    {
      using type = make_type_set_t<Ls..., Rs...>;
    };

    template <typename... E, typename... T>
    struct make_joined_set<type_set<E...>, T...>
    {
      using _rest = typename make_joined_set<T...>::type;

      using type = typename make_joined_set<type_set<E...>, _rest>::type;
    };

    template <typename... Sets>
    using make_joined_set_t = typename make_joined_set<Sets...>::type;

    template <typename Minuend, typename Subtrahend>
    struct make_difference_set
    {
      static_assert(wrong_t<make_difference_set>::value, "invalid argument for difference set");
    };

    template <typename... Minuends, typename... Subtrahends>
    struct make_difference_set<type_set<Minuends...>, type_set<Subtrahends...>>
    {
      template <typename E>
      using is_subtrahend = is_element_of<E, type_set<Subtrahends...>>;
      using type = make_type_set_if_not_t<is_subtrahend, Minuends...>;
    };

    template <typename Minuend, typename Subtrahend>
    using make_difference_set_t = typename make_difference_set<Minuend, Subtrahend>::type;

    template <typename Lhs, typename Rhs>
    struct make_intersect_set
    {
      static_assert(wrong_t<make_intersect_set>::value, "invalid argument for intersect set");
    };

    template <typename... LhsElements, typename... RhsElements>
    struct make_intersect_set<type_set<LhsElements...>, type_set<RhsElements...>>
    {
      template <typename E>
      using is_in_both = ::sqlpp::logic::all<type_set<LhsElements...>::template count<E>(),
                                               type_set<RhsElements...>::template count<E>()>;
      using type = make_type_set_if_t<is_in_both, LhsElements...>;
    };

    template <typename Lhs, typename Rhs>
    using make_intersect_set_t = typename make_intersect_set<Lhs, Rhs>::type;

    template <typename T, template <typename> class Transformation>
    struct transform_set
    {
      static_assert(wrong_t<transform_set>::value, "invalid arguments for transform_set");
    };

    template <typename... E, template <typename> class Transformation>
    struct transform_set<type_set<E...>, Transformation>
    {
      using type = make_type_set_t<Transformation<E>...>;
    };

    template <typename T, template <typename> class Transformation>
    using transform_set_t = typename transform_set<T, Transformation>::type;

    template<typename ...T>
      struct are_unique : public std::integral_constant<bool, make_type_set_t<T...>::size() == sizeof...(T)>
      {
      };

    template<typename ...T>
      struct are_same : public std::integral_constant<bool, make_type_set_t<T...>::size() <= 1>
      {
      };

      template <typename... T>
      struct are_disjoint;

      template <>
      struct are_disjoint<> : public std::true_type
      {
      };

      template <typename... T>
      struct are_disjoint<type_set<T...>> : public std::true_type
      {
      };

      template <typename... L, typename... R, typename... Rest>
      struct are_disjoint<type_set<L...>, type_set<R...>, Rest...>
      {
        static constexpr bool value =
            are_unique<L..., R...>::value and are_disjoint<make_type_set_t<L..., R...>, Rest...>::value;
      };

  }  // namespace detail
}  // namespace sqlpp
