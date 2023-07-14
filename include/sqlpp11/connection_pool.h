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

#include <mutex>

#include <sqlpp11/detail/circular_buffer.h>

namespace sqlpp
{
  template<typename ConnectionBase>
  class connection_pool
  {
  public:
    using _config_ptr_t = typename ConnectionBase::_config_ptr_t;
    using _handle_ptr_t = typename ConnectionBase::_handle_ptr_t;
    using _pooled_connection_t = sqlpp::pooled_connection<ConnectionBase>;

    class pool_core : public std::enable_shared_from_this<pool_core>
    {
    public:
      pool_core(const _config_ptr_t& connection_config, std::size_t capacity);
      pool_core() = delete;
      pool_core(const pool_core &) = delete;
      pool_core(pool_core &&) = delete;

      pool_core& operator=(const pool_core&) = delete;
      pool_core& operator=(pool_core&&) = delete;

      _pooled_connection_t get();
      void put(_handle_ptr_t& handle);
      // Returns number of connections available in the pool. Only used in tests.
      std::size_t available();

    private:
      _config_ptr_t _connection_config;
      sqlpp::detail::circular_buffer<_handle_ptr_t> _handles;
      std::mutex _mutex;
    };

    connection_pool() = default;
    connection_pool(const _config_ptr_t& connection_config, std::size_t capacity);
    connection_pool(const connection_pool&) = delete;
    connection_pool(connection_pool&&) = default;

    connection_pool& operator=(const connection_pool&) = delete;
    connection_pool& operator=(connection_pool&&) = default;

    void initialize(const _config_ptr_t& connection_config, std::size_t capacity);
    _pooled_connection_t get();
    // Returns number of connections available in the pool. Only used in tests.
    std::size_t available();

  private:
    std::shared_ptr<pool_core> _core;
  };

  template<typename ConnectionBase>
  connection_pool<ConnectionBase>::pool_core::pool_core(const _config_ptr_t& connection_config, std::size_t capacity) :
    _connection_config{connection_config},
    _handles{capacity}
  {
  }

  template<typename ConnectionBase>
  typename connection_pool<ConnectionBase>::_pooled_connection_t connection_pool<ConnectionBase>::pool_core::get()
  {
    std::unique_lock lock{_mutex};
    if (_handles.empty()) {
      lock.unlock();
      return _pooled_connection_t{_connection_config, this->shared_from_this()};
    }
    auto handle = std::move(_handles.front());
    _handles.pop_front();
    lock.unlock();
    // If the fetched connection is dead, drop it and create a new one on the fly
    return
      handle->check_connection() ?
      _pooled_connection_t{std::move(handle), this->shared_from_this()} :
      _pooled_connection_t{_connection_config, this->shared_from_this()};
  }

  template<typename ConnectionBase>
  void connection_pool<ConnectionBase>::pool_core::put(_handle_ptr_t& handle)
  {
    std::unique_lock lock{_mutex};
    if (_handles.full ()) {
            _handles.set_capacity (_handles.capacity () + 5);
    }
    _handles.push_back(std::move(handle));
  }

  template<typename ConnectionBase>
  std::size_t connection_pool<ConnectionBase>::pool_core::available()
  {
    std::unique_lock lock{_mutex};
    return _handles.size();
  }

  template<typename ConnectionBase>
  connection_pool<ConnectionBase>::connection_pool(const _config_ptr_t& connection_config, std::size_t capacity) :
    _core{std::make_shared<pool_core>(connection_config, capacity)}
  {
  }

  template<typename ConnectionBase>
  void connection_pool<ConnectionBase>::initialize(const _config_ptr_t& connection_config, std::size_t capacity)
  {
    if (_core) {
      throw std::runtime_error{"Connection pool already initialized"};
    }
    _core = std::make_shared<pool_core>(connection_config, capacity);
  }

  template<typename ConnectionBase>
  typename connection_pool<ConnectionBase>::_pooled_connection_t connection_pool<ConnectionBase>::get()
  {
    return _core->get();
  }

  template<typename ConnectionBase>
  std::size_t connection_pool<ConnectionBase>::available()
  {
    return _core->available();
  }
} // namespace sqlpp
