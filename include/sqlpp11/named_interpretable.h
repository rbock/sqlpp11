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

#ifndef SQLPP_NAMED_SERIALIZABLE_H
#define SQLPP_NAMED_SERIALIZABLE_H

#include <memory>
#include <sqlpp11/serializer_context.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/char_sequence.h>

namespace sqlpp
{
  template <typename Db>
  struct named_interpretable_t
  {
    using _serializer_context_t = typename Db::_serializer_context_t;
    using _interpreter_context_t = typename Db::_interpreter_context_t;

    template <typename T>
    named_interpretable_t(T t)
        : _requires_braces(requires_braces_t<T>::value), _impl(std::make_shared<_impl_t<T>>(t))
    {
    }

    named_interpretable_t(const named_interpretable_t&) = default;
    named_interpretable_t(named_interpretable_t&&) = default;
    named_interpretable_t& operator=(const named_interpretable_t&) = default;
    named_interpretable_t& operator=(named_interpretable_t&&) = default;
    ~named_interpretable_t() = default;

    serializer_context_t& serialize(serializer_context_t& context) const
    {
      return _impl->serialize(context);
    }

    // This method only exists if Db::_serializer_context_t and serializer_context_t are not the same
    template <typename Context>
    auto serialize(Context& context) const ->
        typename std::enable_if<std::is_same<Context, _serializer_context_t>::value and
                                    not std::is_same<Context, serializer_context_t>::value,
                                Context&>::type
    {
      return _impl->db_serialize(context);
    }

    _interpreter_context_t& interpret(_interpreter_context_t& context) const
    {
      return _impl->interpret(context);
    }

    std::string _get_name() const
    {
      return _impl->_get_name();
    }

    bool _requires_braces;

  private:
    struct _impl_base
    {
      virtual serializer_context_t& serialize(serializer_context_t& context) const = 0;
      virtual _serializer_context_t& db_serialize(_serializer_context_t& context) const = 0;
      virtual _interpreter_context_t& interpret(_interpreter_context_t& context) const = 0;
      virtual std::string _get_name() const = 0;
    };

    template <typename T>
    struct _impl_t : public _impl_base
    {
      static_assert(not make_parameter_list_t<T>::size::value, "parameters not supported in dynamic statement parts");
      _impl_t(T t) : _t(t)
      {
      }

      serializer_context_t& serialize(serializer_context_t& context) const
      {
        ::sqlpp::serialize(_t, context);
        return context;
      }

      _serializer_context_t& db_serialize(_serializer_context_t& context) const
      {
        Db::_serialize_interpretable(_t, context);
        return context;
      }

      _interpreter_context_t& interpret(_interpreter_context_t& context) const
      {
        Db::_interpret_interpretable(_t, context);
        return context;
      }

      std::string _get_name() const
      {
        return name_of<T>::char_ptr();
      }

      T _t;
    };

    std::shared_ptr<const _impl_base> _impl;
  };

  template <typename Context, typename Database>
  struct serializer_t<Context, named_interpretable_t<Database>>
  {
    using _serialize_check = consistent_t;
    using T = named_interpretable_t<Database>;

    static Context& _(const T& t, Context& context)
    {
      if (t._requires_braces)
      {
        context << '(';
        t.serialize(context);
        context << ')';
      }
      else
        t.serialize(context);

      return context;
    }
  };
}

#endif
