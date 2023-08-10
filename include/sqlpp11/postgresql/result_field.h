#pragma once

/*
 * Copyright (c) 2021-2021, Roland Bock
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
#include <sqlpp11/exception.h>
#include <sqlpp11/result_field.h>
#include <sqlpp11/result_field_base.h>
#include <sqlpp11/data_types/blob/data_type.h>
#include <sqlpp11/field_spec.h>
#include <ostream>
#include <string>

namespace sqlpp
{
  namespace postgresql
  {
    // Forward declaration
    class connection_base;
  }

  namespace detail
  {
    inline unsigned char unhex(unsigned char c)
    {
      switch (c)
      {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          return c - '0';
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
          return c + 10 - 'a';
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
          return c + 10 - 'A';
      }
      throw sqlpp::exception{std::string{"Unexpected hex char: "} + static_cast<char>(c)};
    }

    inline void hex_assign(std::vector<unsigned char>& value, const uint8_t* blob, size_t len)
    {
      value.resize(len / 2 - 1); // unhex - leading chars
      size_t val_index = 0;
      size_t blob_index = 2;
      while (blob_index < len)
      {
        value[val_index] = static_cast<unsigned char>(unhex(blob[blob_index]) << 4) + unhex(blob[blob_index + 1]);
        ++val_index;
        blob_index += 2;
      }
    }
  }  // namespace detail

  template <typename NameType, bool CanBeNull>
  struct result_field_t<postgresql::connection_base, field_spec_t<NameType, blob, CanBeNull>>
      : public result_field_base<postgresql::connection_base, field_spec_t<NameType, blob, CanBeNull>>
  {
  private:
    const uint8_t* _blob{nullptr};  // Non-owning

  public:
    size_t len{};

    template <typename Target>
    void _bind(Target& target, size_t index)
    {
      target._bind_blob_result(index, &_blob, &len);
      if (_blob)
      {
        detail::hex_assign(this->_value, _blob, len);
        len = this->_value.size();
      }
      else
        this->_value.clear();
      this->_is_null = (_blob == nullptr);
    }

  };
}  // namespace sqlpp
