#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <type_traits>

namespace sqlpp {
namespace detail {
template <typename Clause, typename OldStatement, typename NewClauseData>
const typename Clause::_data_t& pick_arg_impl(
    const OldStatement& /* old_statement */,
    const NewClauseData& new_data,
    const std::true_type& /*unused*/) {
  return new_data;
}

template <typename Clause, typename OldStatement, typename NewClauseData>
const typename Clause::_data_t& pick_arg_impl(
    const OldStatement& old_statement,
    const NewClauseData& /* new_data */,
    const std::false_type& /*unused*/) {
  using old_base_t = typename Clause::template _base_t<OldStatement>;
  return static_cast<const old_base_t&>(old_statement)._data;
}

// Returns a old_statement's new_data either by picking the new_data from the
// old_statement or using the new new_data
template <typename Clause, typename OldStatement, typename NewClauseData>
const typename Clause::_data_t& pick_arg(const OldStatement& old_statement,
                                         const NewClauseData& new_data) {
  return pick_arg_impl<Clause>(
      old_statement, new_data,
      std::is_same<typename Clause::_data_t,
                   typename std::decay<NewClauseData>::type>());
}
}  // namespace detail
}  // namespace sqlpp
