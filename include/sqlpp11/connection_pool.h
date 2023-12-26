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

#include <sqlpp11/connection.h>
#include <sqlpp11/detail/circular_buffer.h>

#include <memory>
#include <mutex>
#include <stdexcept>

namespace sqlpp
{
  enum class connection_check
  {
    none,
    passive,
    ping
  };

  template <typename ConnectionBase>
  class connection_pool
  {
  public:
    using _config_ptr_t = typename ConnectionBase::_config_ptr_t;
    using _handle_ptr_t = typename ConnectionBase::_handle_ptr_t;
    using _pooled_connection_t = sqlpp::pooled_connection<ConnectionBase>;

    class pool_core : public std::enable_shared_from_this<pool_core>
    {
    public:
      pool_core(const _config_ptr_t& connection_config, std::size_t capacity)
          : _connection_config{connection_config}, _handles{capacity}
      {
      }

      pool_core() = delete;
      pool_core(const pool_core&) = delete;
      pool_core(pool_core&&) = delete;

      pool_core& operator=(const pool_core&) = delete;
      pool_core& operator=(pool_core&&) = delete;

      _pooled_connection_t get(connection_check check)
      {
        std::unique_lock<std::mutex> lock{_mutex};
        if (_handles.empty())
        {
          lock.unlock();
          return _pooled_connection_t{_connection_config, this->shared_from_this()};
        }
        auto handle = std::move(_handles.front());
        _handles.pop_front();
        lock.unlock();
        // If the fetched connection is dead, drop it and create a new one on the fly
        return
          check_connection(handle, check) ?
          _pooled_connection_t{std::move(handle), this->shared_from_this()} :
          _pooled_connection_t{_connection_config, this->shared_from_this()};
      }

      void put(_handle_ptr_t& handle)
      {
        std::unique_lock<std::mutex> lock{_mutex};
        if (_handles.full())
        {
          _handles.set_capacity(_handles.capacity() + 5);
        }
        _handles.push_back(std::move(handle));
      }

      // Returns number of connections available in the pool. Only used in tests.
      std::size_t available()
      {
        std::unique_lock<std::mutex> lock{_mutex};
        return _handles.size();
      }

    private:
      inline bool check_connection(_handle_ptr_t& handle, connection_check check)
      {
        switch (check)
        {
          case connection_check::none:
            return true;
          case connection_check::passive:
            return handle->is_connected();
          case connection_check::ping:
            return handle->ping_server();
          default:
            throw std::invalid_argument{"Invalid connection check value"};
        }
      }

      _config_ptr_t _connection_config;
      sqlpp::detail::circular_buffer<_handle_ptr_t> _handles;
      std::mutex _mutex;
    };

    connection_pool() = default;

    connection_pool(const _config_ptr_t& connection_config, std::size_t capacity)
        : _core{std::make_shared<pool_core>(connection_config, capacity)}
    {
    }

    connection_pool(const connection_pool&) = delete;
    connection_pool(connection_pool&&) = default;

    connection_pool& operator=(const connection_pool&) = delete;
    connection_pool& operator=(connection_pool&&) = default;

    void initialize(const _config_ptr_t& connection_config, std::size_t capacity)
    {
      if (_core)
      {
        throw std::runtime_error{"Connection pool already initialized"};
      }
      _core = std::make_shared<pool_core>(connection_config, capacity);
    }

    _pooled_connection_t get(connection_check check = connection_check::passive)
    {
      return _core->get(check);
    }

    // Returns number of connections available in the pool. Only used in tests.
    std::size_t available()
    {
      return _core->available();
    }

  private:
    std::shared_ptr<pool_core> _core;
  };
}  // namespace sqlpp
