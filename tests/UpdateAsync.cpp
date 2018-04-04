/*
* Copyright (c) 2013-2016, Roland Bock
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

#include "MockDb.h"
#include "MockConfig.h"
#include "Sample.h"
#include <sqlpp11/connection_pool.h>
#include <sqlpp11/future.h>
#include <sqlpp11/update.h>
#include <sqlpp11/thread_pool.h>

using namespace std::chrono_literals;

int UpdateAsync(int, char*[])
{
  auto config = std::make_shared<MockConfig>();
  auto pool = sqlpp::make_connection_pool(config, 1);

  auto f = test::TabFoo{};

  {
    auto sql = sqlpp::update(f).set(f.omega = 10.56f).where(f.psi != 3);
    sqlpp::async(pool, sql);
  }

  {
    auto sql = sqlpp::update(f).set(f.delta = "updated").unconditionally();
    sqlpp::async(pool, sql);
  }

  std::this_thread::sleep_for(100ms);

  return 0;
}