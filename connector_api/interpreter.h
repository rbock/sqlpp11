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

#ifndef SQLPP_DATABASE_INTERPRETER_H
#define SQLPP_DATABASE_INTERPRETER_H

#include <sqlpp11/vendor/concat.h>
#include <sqlpp11/vendor/insert_list.h>

/*
 * sqlpp11 offers an interpreter that can be used to serialize the expression tree
 * into a standard SQL string.
 *
 * The connector library can specialize the interpreter template to partially or
 * completely change the way the expression tree is interpreted.
 *
 * For example, this specialization will produce indexed parameters instead of just '?'
 */
namespace sqlpp
{
  namespace vendor
  {
    template <typename ValueType, typename NameType>
    struct interpreter_t<database::context_t, parameter_t<ValueType, NameType>>
    {
      using T = parameter_t<ValueType, NameType>;

      static database::context_t& _(const T& t, database::context_t& context)
      {
        context << "?" << context.count();
        context.pop_count();
        return context;
      }
    };
  }
}

#endif
