/*
 * Copyright (c) 2013 - 2017, Roland Bock, Frank Park
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

#ifndef SQLPP_POOL_CONNECTION_H
#define SQLPP_POOL_CONNECTION_H

#include <memory>

namespace sqlpp
{
  template <typename Connection_config, typename Reconnect_policy, typename Connection>
  class connection_pool;

  template <typename Connection_config,
            typename Reconnect_policy,
            typename Connection,
            typename Connection_pool = connection_pool<Connection_config, Reconnect_policy, Connection>>
  struct pool_connection
  {
  private:
    std::unique_ptr<Connection> _impl;
    Connection_pool* origin;

  public:
    pool_connection(std::unique_ptr<Connection>& connection, Connection_pool* origin)
        : _impl(std::move(connection)), origin(origin)
    {
    }

    ~pool_connection()
    {
      origin->free_connection(_impl);
    }

    template <typename... Args>
    auto operator()(Args&&... args) -> decltype((*_impl)(std::forward<Args>(args)...))
    {
      return (*_impl)(std::forward<Args>(args)...);
    }

    template <typename T>
    auto operator()(const T& t) -> decltype((*_impl)(t))
    {
      return (*_impl)(t);
    }

    Connection* operator->()
    {
      return _impl.get();
    }

    pool_connection(const pool_connection&) = delete;
    pool_connection(pool_connection&& other) : _impl(std::move(other._impl)), origin(other.origin)
    {
    }
    pool_connection& operator=(const pool_connection&) = delete;
    pool_connection& operator=(pool_connection&&) = delete;
  };
}

#endif
