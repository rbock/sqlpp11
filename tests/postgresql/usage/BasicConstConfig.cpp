/*
 * Copyright (c) 2017, Volker Aßmann
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

#include <iostream>
#include <memory>
#include <stdexcept>

#include <sqlpp11/postgresql/connection.h>
#include <sqlpp11/postgresql/exception.h>
#include <sqlpp11/sqlpp11.h>

namespace sql = sqlpp::postgresql;
int BasicConstConfig(int, char*[])
{
  const std::shared_ptr<const sql::connection_config> const_config = []()
  {
    auto config = std::make_shared<sql::connection_config>();
    config->host = "localhost";
    config->user = "unknown_user_must_fail";
    return config;
  }();

  try
  {
    sql::connection db(const_config);

    throw std::logic_error("should never reach this point");
  }
  catch (const sqlpp::postgresql::broken_connection& ex)
  {
    std::cout << "Got exception: '" << ex.what() << "'";
    return 0;
  }

  return 1;
}
