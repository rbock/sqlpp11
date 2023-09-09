#pragma once

/*
Copyright (c) 2023, Vesselin Atanasov
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdexcept>

namespace sqlpp
{
  namespace test
  {
    namespace
    {
      template <typename Connection>
      void test_conn_empty()
      {
        Connection db;
        if (db.is_connected()) {
          throw std::runtime_error{"Unexpected is_connected() == true"};
        }
        if (db.ping_server()) {
          throw std::runtime_error{"Unexpected ping_server() == true"};
        }
      }

      template <typename Connection, typename ConfigPtr>
      void test_conn_connected(const ConfigPtr& connection_config)
      {
        Connection db{connection_config};
        if (db.is_connected() == false) {
          throw std::runtime_error{"Unexpected is_connected() == false"};
        }
        if (db.ping_server() == false) {
          throw std::runtime_error{"Unexpected ping_server() == false"};
        }
      }
    }

    template <typename Connection, typename ConfigPtr>
    void test_normal_connection(const ConfigPtr& connection_config)
    {
      test_conn_empty<Connection>();
      test_conn_connected<Connection>(connection_config);
    }
  }  // namespace test
}  // namespace sqlpp
