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

#ifndef SQLPP_TEXT_RESULT_FIELD_H
#define SQLPP_TEXT_RESULT_FIELD_H

#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/result_field.h>
#include <sqlpp11/result_field_base.h>
#include <sqlpp11/data_types/text/data_type.h>
#include <sqlpp11/field_spec.h>
#include <ostream>

namespace sqlpp
{
  template <typename Db, typename NameType, bool CanBeNull, bool NullIsTrivialValue>
  struct result_field_t<Db, field_spec_t<NameType, text, CanBeNull, NullIsTrivialValue>>
      : public result_field_base<Db, field_spec_t<NameType, text, CanBeNull, NullIsTrivialValue>>
  {
    template <typename Target>
    void _bind(Target& target, size_t index)
    {
      const char* text{nullptr};
      size_t len{};
      target._bind_text_result(index, &text, &len);
      this->_value = {text, len};
      this->_is_null = (len == 0);
    }

    template <typename Target>
    void _post_bind(Target& target, size_t index)
    {
      const char* text{nullptr};
      size_t len{};
      target._post_bind_text_result(index, &text, &len);
      this->_value = {text, len};
      this->_is_null = (len == 0);
    }
  };

  template <typename Db, typename NameType, bool CanBeNull, bool NullIsTrivialValue>
  inline std::ostream& operator<<(
      std::ostream& os, const result_field_t<Db, field_spec_t<NameType, text, CanBeNull, NullIsTrivialValue>>& e)
  {
    if (e.is_null() and not NullIsTrivialValue)
    {
      return os << "NULL";
    }
    else
    {
      return os << e.value();
    }
  }
}
#endif
