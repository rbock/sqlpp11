#pragma once

/*
 * Copyright (c) 2024, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define SQLPP_TEST_STATIC_ASSERT

#include <iostream>
#include <sqlpp11/sqlpp11.h>

#define SQLPP_CHECK_STATIC_ASSERT(expression, message)                             \
  {                                                                                \
    try                                                                            \
    {                                                                              \
      expression;                                                                  \
      std::cerr << __FILE__ << " " << __LINE__ << '\n' << "Expected exception!\n"; \
      return -1;                                                                   \
    }                                                                              \
    catch (const sqlpp::unit_test_exception& e)                                    \
    {                                                                              \
      if (e.what() != sqlpp::string_view(message))                                 \
      {                                                                            \
        std::cerr << __FILE__ << " " << __LINE__ << '\n'                           \
                  << "Expected: -->|" << message << "|<--\n"                       \
                  << "Received: -->|" << e.what() << "|<--\n";                     \
        return -1;                                                                 \
      }                                                                            \
    }                                                                              \
  }

