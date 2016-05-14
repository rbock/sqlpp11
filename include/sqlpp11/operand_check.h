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

#ifndef SQLPP_OPERAND_CHECK_H
#define SQLPP_OPERAND_CHECK_H

#include <sqlpp11/wrap_operand.h>
#include <sqlpp11/detail/enable_if.h>

namespace sqlpp
{
  template <typename T, template <typename> class Pred, typename Enable = void>
  struct unary_operand_check
  {
  };

  template <typename T, template <typename> class Pred>
  struct unary_operand_check<T, Pred, detail::enable_if_t<Pred<T>::value>>
  {
    using type = void;
  };

  template <typename T, template <typename> class Pred>
  using unary_operand_check_t = typename unary_operand_check<wrap_operand_t<T>, Pred>::type;

  template <typename T, template <typename> class Pred>
  using unwrapped_unary_operand_check_t = typename unary_operand_check<T, Pred>::type;

  template <typename L,
            template <typename> class LPred,
            typename R,
            template <typename> class RPred,
            typename Enable = void>
  struct binary_operand_check
  {
  };

  template <typename L, template <typename> class LPred, typename R, template <typename> class RPred>
  struct binary_operand_check<L, LPred, R, RPred, detail::enable_if_t<LPred<L>::value and RPred<R>::value>>
  {
    using type = void;
  };

  template <typename L, template <typename> class LPred, typename R, template <typename> class RPred>
  using binary_operand_check_t =
      typename binary_operand_check<wrap_operand_t<L>, LPred, wrap_operand_t<R>, RPred>::type;

  template <typename L, template <typename> class LPred, typename R, template <typename> class RPred>
  using unwrapped_binary_operand_check_t = typename binary_operand_check<L, LPred, R, RPred>::type;
}

#endif
