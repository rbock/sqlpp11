#pragma once

/*
Copyright (c) 2017 - 2018, Roland Bock
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

#include <sqlpp11/compat/make_unique.h>

#include <random>
#include <set>
#include <thread>
#include <unordered_set>

#include "TabDepartment.h"

namespace sqlpp
{
  namespace test
  {
    namespace
    {
      template<typename Pool>
      using native_type = typename std::decay<decltype(std::declval<Pool>().get().native_handle())>::type;

      template<typename Pool>
      using native_set = std::unordered_set<native_type<Pool>>;

      template<typename Pool>
      using pool_conn_type = typename std::decay<decltype(std::declval<Pool>().get())>::type;

      template<typename Pool>
      native_set<Pool> get_native_handles(Pool& pool)
      {
        native_set<Pool> ns;
        if (pool.available() == 0) {
          return ns;
        }
        for (;;) {
          auto handle = pool.get().native_handle();
          auto insert_res = ns.insert(handle);
          if (insert_res.second == false) {
            return ns;
          }
        }
      }

      template <typename Pool>
      void test_conn_move(Pool& pool)
      {
        std::clog << __func__ << '\n';
        auto nh_all = get_native_handles(pool);
        {
          // Get one connection from the pool
          auto conn_1 = pool.get();
          // If the native handles list was empty before getting a connection, then a new connection
          // was created, so we need to update the set of all native handles
          if (nh_all.empty()) {
            nh_all.insert(conn_1.native_handle());
          }
          auto nh_removed = nh_all;
          if (nh_removed.erase(conn_1.native_handle()) != 1) {
            throw std::logic_error{"Got an unknown native connection handle"};
          }
          if (get_native_handles(pool) != nh_removed) {
            throw std::logic_error{"Could not get correctly a connection from the pool"};
          }
          {
            // Move the pooled connection once
            auto conn_2 = std::move(conn_1);
            if (get_native_handles(pool) != nh_removed) {
              throw std::logic_error{"Moving a connection changes the pool"};
            }

            // Move the pooled connection again
            conn_1 = std::move(conn_2);
            if (get_native_handles(pool) != nh_removed) {
              throw std::logic_error{"Moving a connection changes the pool"};
            }

            // The empty connection conn_2 goes out of scope and gets destroyed
          }

          // Check if destroying an empty connection changed the pool
          if (get_native_handles(pool) != nh_removed) {
            throw std::logic_error{"Destroying an empty connection changes the pool"};
          }

          // The valid connection conn_1 goes out of scope and gets destroyed
        }

        // Check if destroying a valid connection from the pool returned the handle to the pool
        if (get_native_handles(pool) != nh_all) {
          throw std::logic_error{"Destroying a valid connection does not return its handle to the pool"};
        }
      }

      template <typename Pool>
      void test_conn_check(Pool& pool)
      {
        std::clog << __func__ << '\n';
        auto check_db = [] (typename Pool::_pooled_connection_t db) {
          if (db.is_connected() == false)
          {
            throw std::runtime_error{"is_connected() returned false"};
          }
          if (db.ping_server() == false)
          {
            throw std::runtime_error{"ping_server() returned false"};
          }
        };
        check_db(pool.get(connection_check::none));
        check_db(pool.get(connection_check::passive));
        check_db(pool.get(connection_check::ping));
      }

      template <typename Pool>
      void test_basic(Pool& pool, const std::string& create_table)
      {
        std::clog << __func__ << '\n';
        try
        {
          auto db = pool.get();
          db.execute("DROP TABLE IF EXISTS tab_department");
          db.execute(create_table);
          model::TabDepartment tabDept = {};
          db(insert_into(tabDept).default_values());
        }
        catch (const std::exception& e)
        {
          std::cerr << "Exception in " << __func__ << "\n";
          throw;
        }
      }

      template <typename Pool>
      void test_single_connection(Pool& pool)
      {
        std::clog << __func__ << '\n';
        try
        {
          auto* handle = [&pool]() {
            auto db = pool.get();
            return db.native_handle();
          }();

          for (auto i = 0; i < 100; ++i)
          {
            auto db = pool.get();
            if (handle != db.native_handle())
            {
              std::cerr << "original connection: " << handle << std::endl;
              std::cerr << "received connection: " << db.native_handle() << std::endl;
              throw std::logic_error{"Pool acquired more than one connection"};
            }
          }
        }
        catch (const std::exception& e)
        {
          std::cerr << "Exception in " << __func__ << "\n";
          throw;
        }
      }

      template <typename Pool>
      void test_multiple_connections(Pool& pool)
      {
        std::clog << __func__ << '\n';
        try
        {
          model::TabDepartment tabDept = {};
          auto connections = std::vector<typename std::decay<decltype(pool.get())>::type>{};
          auto pointers = std::set<void*>{};
          for (auto i = 0; i < 50; ++i)
          {
            connections.push_back(pool.get());
            if (pointers.count(connections.back().native_handle()))
            {
              throw std::logic_error{"Pool yielded connection twice (without getting it back in between)"};
            }
            pointers.insert(connections.back().native_handle());
            connections.back()(insert_into(tabDept).default_values());
          }
        }
        catch (const std::exception& e)
        {
          std::cerr << "Exception in " << __func__ << "\n";
          throw;
        }
      }

      template <typename Pool>
      void test_multithreaded(Pool& pool)
      {
        std::clog << __func__ << '\n';
        std::random_device r;
        std::default_random_engine random_engine(r());
        std::uniform_int_distribution<int> uniform_dist(1, 20);

        std::clog << "Run a random number [1,20] of threads\n";
        std::clog << "Each with a random number [1,20] of {pool.get() & insert}\n";

        try
        {
          auto threads = std::vector<std::thread>{};
          const auto thread_count = uniform_dist(random_engine);

          for (auto i = 0; i < thread_count; ++i)
          {
            auto func = __func__;
            auto call_count = uniform_dist(random_engine);
            threads.push_back(std::thread([call_count, &func, &pool]() {
              constexpr model::TabDepartment tabDept = {};
              try
              {
                for (auto k = 0; k < call_count; ++k)
                {
                  auto connection = pool.get();
                  connection(insert_into(tabDept).default_values());
                }
              }
              catch (const std::exception& e)
              {
                std::cerr << std::string(func) + ": In-thread exception: " + e.what() + "\n";
                std::abort();
              }
            }));
          }
          for (auto&& t : threads)
          {
            t.join();
          }
        }
        catch (const std::exception& e)
        {
          std::cerr << "Exception in " << __func__ << "\n";
          throw;
        }
      }

      template <typename Pool>
      void test_destruction_order(typename Pool::_config_ptr_t config)
      {
        std::clog << __func__ << '\n';
        // Create a pool, get a connection from it and then destroy the pool before the connection
        auto pool = sqlpp::compat::make_unique<Pool>(config, 5);
        auto conn = pool->get();
        pool = nullptr;
      }
    }

    template <typename Pool>
    void test_connection_pool (typename Pool::_config_ptr_t config, const std::string& create_table, bool test_mt)
    {
      auto pool = Pool {config, 5};
      test_conn_move(pool);
      test_basic(pool, create_table);
      test_conn_check(pool);
      test_single_connection(pool);
      test_multiple_connections(pool);
      if (test_mt)
      {
        test_multithreaded(pool);
      }
      test_destruction_order<Pool>(config);
    }
  }  // namespace test
}  // namespace sqlpp
