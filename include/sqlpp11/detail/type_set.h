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

#ifndef SQLPP_DETAIL_TYPE_SET_H
#define SQLPP_DETAIL_TYPE_SET_H

#include <type_traits>
#include <sqlpp11/wrong.h>
#include <sqlpp11/logic.h>

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

    // A type set
    template <typename... Elements>
    struct type_set
    {
      using size = std::integral_constant<size_t, sizeof...(Elements)>;
      using _is_type_set = std::true_type;

      static_assert(std::is_same<type_set, make_type_set_t<Elements...>>::value,
                    "use make_type_set to construct a typeset");

      template <typename T>
      struct insert
      {
        using type =
            typename std::conditional<not is_element_of<T, type_set>::value, type_set<T, Elements...>, type_set>::type;
      };

      template <template <typename A> class Predicate, typename T>
      struct insert_if
      {
        using type = typename std::conditional<Predicate<T>::value and not is_element_of<T, type_set>::value,
                                               type_set<Elements..., T>,
                                               type_set>::type;
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
      static constexpr bool value = ::sqlpp::logic::any_t<std::is_same<E, Elements>::value...>::value;
    };

    template <typename L, typename R>
    struct joined_set
    {
      static_assert(wrong_t<joined_set>::value, "L and R have to be type sets");
    };

    template <typename... LElements, typename... RElements>
    struct joined_set<type_set<LElements...>, type_set<RElements...>>
    {
      using type = make_type_set_t<LElements..., RElements...>;
    };

    template <typename L, typename R>
    using joined_set_t = typename joined_set<L, R>::type;

    template <typename L, typename R>
    struct is_superset_of
    {
      static_assert(wrong_t<is_superset_of>::value, "L and R have to be type sets");
    };

    template <typename... LElements, typename... RElements>
    struct is_superset_of<type_set<LElements...>, type_set<RElements...>>
    {
      static constexpr bool value =
          joined_set_t<type_set<LElements...>, type_set<RElements...>>::size::value == sizeof...(LElements);
    };

    template <typename L, typename R>
    struct is_subset_of
    {
      static constexpr bool value = is_superset_of<R, L>::value;
    };

    template <typename L, typename R>
    struct is_disjunct_from
    {
      static_assert(wrong_t<is_disjunct_from>::value, "invalid argument for is_disjunct_from");
    };

    template <typename... LElements, typename... RElements>
    struct is_disjunct_from<type_set<LElements...>, type_set<RElements...>>
    {
      static constexpr bool value = joined_set_t<type_set<LElements...>, type_set<RElements...>>::size::value ==
                                    sizeof...(LElements) + sizeof...(RElements);
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
    using has_duplicates = std::integral_constant<bool, make_type_set_t<T...>::size::value != sizeof...(T)>;

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

    template <typename... E, typename... T>
    struct make_joined_set<type_set<E...>, T...>
    {
      using _rest = typename make_joined_set<T...>::type;

      using type = joined_set_t<type_set<E...>, _rest>;
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
      using is_in_both = ::sqlpp::logic::all_t<is_element_of<E, type_set<LhsElements...>>::value,
                                               is_element_of<E, type_set<RhsElements...>>::value>;
      using type = make_type_set_if_t<is_in_both, LhsElements...>;
    };

    template <typename Lhs, typename Rhs>
    using make_intersect_set_t = typename make_intersect_set<Lhs, Rhs>::type;

    template <template <typename> class Transformation, typename T>
    struct transform_set
    {
      static_assert(wrong_t<transform_set>::value, "invalid argument for transform_set");
    };

    template <template <typename> class Transformation, typename... E>
    struct transform_set<Transformation, type_set<E...>>
    {
      using type = make_type_set_t<Transformation<E>...>;
    };

    template <template <typename> class Transformation, typename T>
    using transform_set_t = typename transform_set<Transformation, T>::type;

    // workaround for msvc bug https://connect.microsoft.com/VisualStudio/feedback/details/2173198
    template <typename T>
    struct make_name_of_set
    {
      static_assert(wrong_t<make_name_of_set>::value, "invalid argument for transform_set");
    };

    template <typename... E>
    struct make_name_of_set<type_set<E...>>
    {
      using type = make_type_set_t<typename E::_alias_t::_name_t...>;
    };

    template <typename T>
    using make_name_of_set_t = typename make_name_of_set<T>::type;
  }
}

#endif
