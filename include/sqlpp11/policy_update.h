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

#ifndef SQLPP_POLICY_UPDATE_H
#define SQLPP_POLICY_UPDATE_H

#include <sqlpp11/wrong.h>

namespace sqlpp
{
  template <typename Needle, typename Replacement>
  struct policy_update_impl
  {
    template <typename T>
    using _policy_t = typename std::conditional<std::is_same<Needle, T>::value, Replacement, T>::type;
  };

  template <typename T, typename Needle, typename Replacement>
  using policy_update_t = typename policy_update_impl<Needle, Replacement>::template _policy_t<T>;

  template <typename Original, typename Needle, typename Replacement>
  struct update_policies_impl
  {
    using type = typename Original::template _policy_update_t<Needle, Replacement>;
  };

  template <typename Original, typename Needle, typename Replacement>
  using update_policies_t = typename update_policies_impl<Original, Needle, Replacement>::type;

  template <typename Policies, typename Needle, typename Replacement>
  using new_statement = typename Policies::template _new_statement_t<Needle, Replacement>;

  template <typename Check, typename Policies, typename Needle, typename Replacement>
  struct new_statement_impl
  {
    using type = Check;
  };

  template <typename Policies, typename Needle, typename Replacement>
  struct new_statement_impl<consistent_t, Policies, Needle, Replacement>
  {
    using type = typename Policies::template _new_statement_t<Needle, Replacement>;
  };

  template <typename Check, typename Policies, typename Needle, typename Replacement>
  using new_statement_t = typename new_statement_impl<Check, Policies, Needle, Replacement>::type;
}

#endif
