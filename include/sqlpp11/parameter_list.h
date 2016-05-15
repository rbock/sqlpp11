/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include <tuple>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/wrong.h>
#include <sqlpp11/detail/index_sequence.h>

namespace sqlpp
{
  template <typename T>
  struct parameter_list_t
  {
    static_assert(wrong_t<parameter_list_t>::value, "Template parameter for parameter_list_t has to be a type_vector");
  };

  template <typename... Parameter>
  struct parameter_list_t<detail::type_vector<Parameter...>> : public Parameter::_instance_t...
  {
    using _member_tuple_t = std::tuple<typename Parameter::_instance_t...>;
    using size = std::integral_constant<std::size_t, sizeof...(Parameter)>;

    parameter_list_t()
    {
    }

    template <typename Target>
    void _bind(Target& target) const
    {
      _bind_impl(target, detail::make_index_sequence<size::value>{});
    }

  private:
    template <typename Target, size_t... Is>
    void _bind_impl(Target& target, const detail::index_sequence<Is...>&) const
    {
      using swallow = int[];  // see interpret_tuple.h
      (void)swallow{0, (std::tuple_element<Is, _member_tuple_t>::type::operator()()._bind(target, Is), 0)...};
    }
  };

  template <typename Exp>
  using make_parameter_list_t = parameter_list_t<parameters_of<Exp>>;
}

#endif
