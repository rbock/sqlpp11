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

#include <memory>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/char_sequence.h>

namespace sqlpp
{
  template <typename Db>
  struct named_interpretable_t
  {
    using _serializer_context_t = typename Db::_serializer_context_t;

    template <typename T>
    named_interpretable_t(T t) : _requires_parens(requires_parens_t<T>::value), _impl(std::make_shared<_impl_t<T>>(t))
    {
    }

    named_interpretable_t(const named_interpretable_t&) = default;
    named_interpretable_t(named_interpretable_t&&) = default;
    named_interpretable_t& operator=(const named_interpretable_t&) = default;
    named_interpretable_t& operator=(named_interpretable_t&&) = default;
    ~named_interpretable_t() = default;

    _serializer_context_t& interpret(_serializer_context_t& context) const
    {
      return _impl->interpret(context);
    }

    std::string _get_name() const
    {
      return _impl->_get_name();
    }

    bool _requires_parens;

  private:
    struct _impl_base
    {
      virtual ~_impl_base() = default;
      virtual _serializer_context_t& interpret(_serializer_context_t& context) const = 0;
      virtual std::string _get_name() const = 0;
    };

    template <typename T>
    struct _impl_t : public _impl_base
    {
      static_assert(not make_parameter_list_t<T>::size::value, "parameters not supported in dynamic statement parts");
      _impl_t(T t) : _t(t)
      {
      }

      _serializer_context_t& interpret(_serializer_context_t& context) const
      {
        serialize(_t, context);
        return context;
      }

      std::string _get_name() const
      {
        return name_of<T>::template char_ptr<_serializer_context_t>();
      }

      T _t;
    };

    std::shared_ptr<const _impl_base> _impl;
  };

  template <typename Context, typename Database>
  Context& serialize(const named_interpretable_t<Database>& t, Context& context)
  {
    if (t._requires_parens)
    {
      context << '(';
      t.interpret(context);
      context << ')';
    }
    else
    {
      t.interpret(context);
    }

    return context;
  }
}  // namespace sqlpp
