#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
 * Copyright (c) 2023, Vesselin Atanasov
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

#include <sqlpp11/compat/make_unique.h>

#include <memory>
#include <utility>

namespace sqlpp
{
  struct connection
  {
  };

  template <typename ConnectionBase>
  class common_connection : public ConnectionBase
  {
  public:
    bool is_connected() const
    {
      return ConnectionBase::_handle ? ConnectionBase::_handle->is_connected() : false;
    }

    bool ping_server() const
    {
      return ConnectionBase::_handle ? ConnectionBase::_handle->ping_server() : false;
    }

  protected:
    using ConnectionBase::ConnectionBase;
  };

  // Normal (non-pooled) connection
  template <typename ConnectionBase>
  class normal_connection : public common_connection<ConnectionBase>
  {
  public:
    using _config_t = typename ConnectionBase::_config_t;
    using _config_ptr_t = typename ConnectionBase::_config_ptr_t;

    // Constructors
    normal_connection() = default;

    normal_connection(const _config_t& config) : normal_connection{std::make_shared<_config_t>(config)}
    {
    }

    normal_connection(const _config_ptr_t& config)
        : common_connection<ConnectionBase>(compat::make_unique<_handle_t>(config))
    {
    }

    normal_connection(const normal_connection&) = delete;
    normal_connection(normal_connection&&) = default;

    // Assigment operators
    normal_connection& operator=(const normal_connection&) = delete;
    normal_connection& operator=(normal_connection&&) = default;

    // creates a connection handle and connects to database
    void connectUsing(const _config_ptr_t& config) noexcept(false)
    {
      ConnectionBase::_handle = compat::make_unique<_handle_t>(config);
    }

  private:
    using _handle_t = typename ConnectionBase::_handle_t;
  };

  // Forward declaration
  template <typename ConnectionBase>
  class connection_pool;

  // Pooled connection
  template <typename ConnectionBase>
  class pooled_connection : public common_connection<ConnectionBase>
  {
    friend class connection_pool<ConnectionBase>::pool_core;

  public:
    using _config_ptr_t = typename ConnectionBase::_config_ptr_t;
    using _handle_t = typename ConnectionBase::_handle_t;
    using _handle_ptr_t = typename ConnectionBase::_handle_ptr_t;
    using _pool_core_ptr_t = std::shared_ptr<typename connection_pool<ConnectionBase>::pool_core>;

    // Copy/Move constructors
    pooled_connection(const pooled_connection&) = delete;
    pooled_connection(pooled_connection&& other) = default;

    ~pooled_connection()
    {
      conn_release();
    }

    // Assigment operators
    pooled_connection& operator=(const pooled_connection&) = delete;

    pooled_connection& operator=(pooled_connection&& other)
    {
      if (this != &other)
      {
        conn_release();
        static_cast<ConnectionBase&>(*this) = std::move(static_cast<ConnectionBase&>(other));
        _pool_core = std::move(other._pool_core);
      }
      return *this;
    }

  private:
    _pool_core_ptr_t _pool_core;

    // Constructors used by the connection pool
    pooled_connection(_handle_ptr_t&& handle, _pool_core_ptr_t pool_core)
        : common_connection<ConnectionBase>(std::move(handle)), _pool_core(pool_core)
    {
    }

    pooled_connection(const _config_ptr_t& config, _pool_core_ptr_t pool_core)
        : common_connection<ConnectionBase>(compat::make_unique<_handle_t>(config)), _pool_core(pool_core)
    {
    }

    void conn_release()
    {
      if (_pool_core)
      {
        _pool_core->put(ConnectionBase::_handle);
        _pool_core = nullptr;
      }
    }
  };
}  // namespace sqlpp
