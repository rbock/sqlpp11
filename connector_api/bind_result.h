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

#ifndef SQLPP_DATABASE_BIND_RESULT_H
#define SQLPP_DATABASE_BIND_RESULT_H

#include <memory>

namespace sqlpp
{
  namespace database
  {
    /*
     * bind_result_t binds values of a sqlpp11 result row
     * to the results of a statement
     */
    class bind_result_t
    {
    public:
      bind_result_t();  // default constructor for a result that will not yield a valid row
      bind_result_t(...);
      bind_result_t(const bind_result_t&) = delete;
      bind_result_t(bind_result_t&& rhs);
      bind_result_t& operator=(const bind_result_t&) = delete;
      bind_result_t& operator=(bind_result_t&&);
      ~bind_result_t();

      bool operator==(const bind_result_t& rhs) const;

      template <typename ResultRow>
      void next(ResultRow& result_row);

      // something similar to this:
      /*
      {
        if (!_handle)
        {
          result_row.invalidate();
          return;
        }

        if (next_impl())
        {
          if (not result_row)
          {
            result_row.validate();
          }
          result_row._bind(*this); // bind result row values to results
        }
        else
        {
          if (result_row)
            result_row.invalidate();
        }
      };
      */

      // These are called by the result row to bind individual result values
      // More will be added over time
      void _bind_boolean_result(size_t index, signed char* value, bool* is_null);
      void _bind_floating_point_result(size_t index, double* value, bool* is_null);
      void _bind_integral_result(size_t index, int64_t* value, bool* is_null);
      void _bind_text_result(size_t index, const char** text, size_t* len);
      ...
    };
  }
}
#endif
