#pragma once

/*
 * Copyright (c) 2013-2017, Roland Bock
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

#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/result_field.h>
#include <sqlpp11/result_field_base.h>
#include <sqlpp11/data_types/blob/data_type.h>
#include <sqlpp11/field_spec.h>
#include <ostream>
#include <string>

namespace sqlpp
{
  template <typename Db, typename NameType, bool CanBeNull>
  struct result_field_t<Db, field_spec_t<NameType, blob, CanBeNull>>
      : public result_field_base<Db, field_spec_t<NameType, blob, CanBeNull>>
  {
    const uint8_t* blob{nullptr};  // Non-owning
    size_t len{};

    template <typename Target>
    void _bind(Target& target, size_t index)
    {
      target._bind_blob_result(index, &blob, &len);
      if (blob)
        this->_value.assign(blob, blob + len);
      else
        this->_value.clear();
      this->_is_null = (blob == nullptr);
    }

    template <typename Target>
    void _post_bind(Target& target, size_t index)
    {
      target._post_bind_blob_result(index, &blob, &len);
      if (blob)
        this->_value.assign(blob, blob + len);
      else
        this->_value.clear();
      this->_is_null = (blob == nullptr);
    }
  };

  template <typename Db, typename NameType, bool CanBeNull>
  inline std::ostream& operator<<(
      std::ostream& os, const result_field_t<Db, field_spec_t<NameType, blob, CanBeNull>>& e)
  {
    if (e.is_null())
    {
      return os << "NULL";
    }
    else
    {
      std::vector<uint8_t> value = e.value();
      std::string value_str(value.begin(), value.end());
      return os << value_str;
    }
  }
}  // namespace sqlpp
