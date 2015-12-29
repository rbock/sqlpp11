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

#ifndef SQLPP_INTERPRET_TUPLE_H
#define SQLPP_INTERPRET_TUPLE_H

#include <tuple>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/detail/index_sequence.h>

namespace sqlpp
{
  template <typename Element, typename Separator, typename Context, typename UseBraces>
  static void interpret_tuple_element(
      const Element& element, const Separator& separator, Context& context, const UseBraces&, size_t index)
  {
    if (index)
      context << separator;
    if (UseBraces::value)
      serialize_operand(element, context);
    else
      serialize(element, context);
  }

  template <typename Tuple, typename Separator, typename Context, typename UseBraces, size_t... Is>
  auto interpret_tuple_impl(const Tuple& t,
                            const Separator& separator,
                            Context& context,
                            const UseBraces& useBraces,
                            const detail::index_sequence<Is...>&) -> Context &
  {
    // Note: A braced-init-list does guarantee the order of evaluation according to 12.6.1 [class.explicit.init]
    // paragraph 2 and 8.5.4 [dcl.init.list] paragraph 4.
    // See for example: "http://en.cppreference.com/w/cpp/utility/integer_sequence"
    // See also: "http://stackoverflow.com/questions/6245735/pretty-print-stdtuple/6245777#6245777"
    // Beware of gcc-bug: "http://gcc.gnu.org/bugzilla/show_bug.cgi?id=51253", otherwise an empty swallow struct could
    // be used
    using swallow = int[];
    (void)swallow{0,  // workaround against -Wpedantic GCC warning "zero-size array 'int [0]'"
                  (interpret_tuple_element(std::get<Is>(t), separator, context, useBraces, Is), 0)...};
    return context;
  }

  template <typename Tuple, typename Separator, typename Context>
  auto interpret_tuple(const Tuple& t, const Separator& separator, Context& context) -> Context &
  {
    return interpret_tuple_impl(t, separator, context, std::true_type{},
                                detail::make_index_sequence<std::tuple_size<Tuple>::value>{});
  }

  template <typename Tuple, typename Separator, typename Context>
  auto interpret_tuple_without_braces(const Tuple& t, const Separator& separator, Context& context) -> Context &
  {
    return interpret_tuple_impl(t, separator, context, std::false_type{},
                                detail::make_index_sequence<std::tuple_size<Tuple>::value>{});
  }
}

#endif
