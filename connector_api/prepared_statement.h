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

#ifndef SQLPP_DATABASE_PREPARED_STATEMENT_H
#define SQLPP_DATABASE_PREPARED_STATEMENT_H

#include <memory>
#include <string>

namespace sqlpp
{
  namespace database
  {
    class prepared_statement_t
    {
    public:
      prepared_statement_t() = delete;
      prepared_statement_t(...);
      prepared_statement_t(const prepared_statement_t&) = delete;
      prepared_statement_t(prepared_statement_t&& rhs);
      prepared_statement_t& operator=(const prepared_statement_t&) = delete;
      prepared_statement_t& operator=(prepared_statement_t&&);
      ~prepared_statement_t();

      bool operator==(const prepared_statement_t& rhs) const;

      // These are called by the sqlpp11::prepared_*_t to bind the individual parameters
      // More will be added over time
      void _bind_boolean_parameter(size_t index, const signed char* value, bool is_null);
      void _bind_floating_point_parameter(size_t index, const double* value, bool is_null);
      void _bind_integral_parameter(size_t index, const int64_t* value, bool is_null);
      void _bind_text_parameter(size_t index, const std::string* value, bool is_null);
    };
  }
}
#endif
