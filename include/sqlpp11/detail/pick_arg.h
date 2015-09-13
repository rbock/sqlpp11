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

#ifndef SQLPP_DETAIL_PICK_ARG_H
#define SQLPP_DETAIL_PICK_ARG_H

#include <type_traits>

namespace sqlpp
{
  namespace detail
  {
    template <typename Target, typename Statement, typename Term>
    typename Target::_data_t pick_arg_impl(Statement /* statement */, Term term, const std::true_type&)
    {
      return term;
    }

    template <typename Target, typename Statement, typename Term>
    typename Target::_data_t pick_arg_impl(Statement statement, Term /* term */, const std::false_type&)
    {
      return Target::_get_member(statement)._data;
    }

    // Returns a statement's term either by picking the term from the statement or using the new term
    template <typename Target, typename Statement, typename Term>
    typename Target::_data_t pick_arg(Statement statement, Term term)
    {
      return pick_arg_impl<Target>(statement, term, std::is_same<typename Target::_data_t, Term>());
    }
  }
}

#endif
