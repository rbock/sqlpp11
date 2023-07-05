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

#ifndef SQLPP11_CONNECTION_H
#define SQLPP11_CONNECTION_H

#include <functional>
#include <memory>

namespace sqlpp
{
  struct connection
  {
  };

  // Normal (non-pooled) connection
  template<typename ConnBase>
  class conn_normal : public ConnBase
  {
  public:
    using _config_t = typename ConnBase::_config_t;
    using _config_ptr_t = typename ConnBase::_config_ptr_t;

    // Constructors
    conn_normal() = default;
    conn_normal(const _config_t& config);
    conn_normal(const _config_ptr_t& config);
    conn_normal(const conn_normal&) = delete;
    conn_normal(conn_normal&&) = default;

    // Assigment operators
    conn_normal& operator=(const conn_normal&) = delete;
    conn_normal& operator=(conn_normal&&) = default;

    // creates a connection handle and connects to database
    void connectUsing(const _config_ptr_t& config) noexcept(false);

  private:
    using _handle_t = typename ConnBase::_handle_t;
  };

  template<typename ConnBase>
  conn_normal<ConnBase>::conn_normal(const _config_t& config) :
    conn_normal{std::make_shared<_config_t>(config)}
  {
  }

  template<typename ConnBase>
  conn_normal<ConnBase>::conn_normal(const _config_ptr_t& config) :
    ConnBase{std::make_unique<_handle_t>(config)}
  {
  }

  template<typename ConnBase>
  void conn_normal<ConnBase>::connectUsing(const _config_ptr_t& config) noexcept(false)
  {
    ConnBase::_handle = std::make_unique<_handle_t>(config);
  }

  // Forward declaration
  template<typename ConnBase>
  class connection_pool;

  // Pooled connection
  template<typename ConnBase>
  class conn_pooled : public ConnBase
  {
    friend class connection_pool<ConnBase>::pool_core;

  public:
    using _config_ptr_t = typename ConnBase::_config_ptr_t;
    using _handle_t = typename ConnBase::_handle_t;
    using _handle_ptr_t = typename ConnBase::_handle_ptr_t;
    using _pool_core_ptr_t = std::shared_ptr<typename connection_pool<ConnBase>::pool_core>;

    // Copy/Move constructors
    conn_pooled(const conn_pooled&) = delete;
    conn_pooled(conn_pooled&& other) = default;
    ~conn_pooled();

    // Assigment operators
    conn_pooled& operator=(const conn_pooled&) = delete;
    conn_pooled& operator=(conn_pooled&& other);

  private:
    _pool_core_ptr_t _pool_core;

    // Constructors used by the connection pool
    conn_pooled(_handle_ptr_t&& handle, _pool_core_ptr_t pool_core);
    conn_pooled(const _config_ptr_t& config, _pool_core_ptr_t pool_core);

    void conn_release();
  };

  template<typename ConnBase>
  conn_pooled<ConnBase>::~conn_pooled()
  {
    conn_release();
  }

  template<typename ConnBase>
  conn_pooled<ConnBase>& conn_pooled<ConnBase>::operator=(conn_pooled&& other)
  {
    if (this != &other) {
      conn_release();
      static_cast<ConnBase&>(*this) = std::move(static_cast<ConnBase&>(other));
      _pool_core = std::move(other._pool_core);
    }
    return *this;
  }

  template<typename ConnBase>
  conn_pooled<ConnBase>::conn_pooled(_handle_ptr_t&& handle, _pool_core_ptr_t pool_core) :
    ConnBase{std::move(handle)},
    _pool_core{pool_core}
  {
  }

  template<typename ConnBase>
  conn_pooled<ConnBase>::conn_pooled(const _config_ptr_t& config, _pool_core_ptr_t pool_core) :
    ConnBase{std::make_unique<_handle_t>(config)},
    _pool_core{pool_core}
  {
  }

  template<typename ConnBase>
  void conn_pooled<ConnBase>::conn_release()
  {
    if (_pool_core) {
      _pool_core->put(ConnBase::_handle);
      _pool_core = nullptr;
    }
  }
}  // namespace sqlpp

#endif
