#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include <sqlpp11/mysql/connection.h>
#include <iostream>

namespace
{
  template <typename Result, typename Expected>
  void assert_equal(int lineNo, const Result& result, const Expected& expected)
  {
    if (result != expected)
    {
      std::cerr << __FILE__ << " " << lineNo << '\n'
                << "Expected: -->|" << expected << "|<--\n"
                << "Received: -->|" << result << "|<--\n";
      throw std::runtime_error("unexpected result");
    }
  }

  template <typename Expression>
  void compare(int lineNo, const Expression& expr, const std::string& expected)
  {
    auto config{std::make_shared<sqlpp::mysql::connection_config>()};
    config->user = "root";
    config->database = "sqlpp_mysql";
    config->debug = true;
    try
    {
      sqlpp::mysql::connection db(config);
    }
    catch (const sqlpp::exception& e)
    {
      std::cerr << e.what() << std::endl;
      throw std::logic_error("For testing, you'll need to create a database sqlpp_mysql for user root (no password)");
    }

    sqlpp::mysql::connection connection{config};
    sqlpp::mysql::context_t printer{connection};

    const auto result = serialize(expr, printer).str();

    assert_equal(lineNo, result, expected);
  }
}  // namespace
