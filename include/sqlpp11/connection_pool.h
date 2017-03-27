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

#ifndef SQLPP_CONNECTION_POOL_H
#define SQLPP_CONNECTION_POOL_H

#include <mutex>
#include <stack>
#include <memory>
#include <iostream>
#include <sqlpp11/exception.h>

namespace sqlpp
{
	template <typename Connection, typename Connection_config>
	class connection_pool
	{
	private:
		std::mutex connection_pool_mutex;
		const std::shared_ptr<Connection_config> config;
		unsigned int maximum_pool_size = 0;
		std::stack<std::unique_ptr<Connection>> free_connections;

		struct async_connection : public Connection
		{
			friend class connection_pool;

		private:
			connection_pool* origin;

		public:
			async_connection(std::unique_ptr<Connection>& connection, connection_pool* origin) :
				Connection(std::move(*connection.get())), origin(origin) {}

			~async_connection()
			{
				// is_valid() checks if the Connection object has a valid SQL handle
				// to sure we are not moving an empty connection back to the pool
				if (this->is_valid())
				{
					origin->free_connection(std::make_unique<async_connection>(std::move(*this)));
				}
			}

			async_connection(const async_connection&) = delete;
			async_connection(async_connection&& other)
				: Connection(std::move(other)), origin(other.origin) {}
			async_connection& operator=(const async_connection&) = delete;
			async_connection& operator=(async_connection&&) = delete;
		};

		bool is_connection_compatible(const std::unique_ptr<async_connection>& connection)
		{
			return connection.get()->get_config().get() == this->config.get();
		}
	public:
		connection_pool(const std::shared_ptr<Connection_config>& config, unsigned int pool_size)
			: config(config), maximum_pool_size(pool_size)
		{}
		~connection_pool() = default;
		connection_pool(const connection_pool&) = delete;
		connection_pool(connection_pool&&) = delete;
		connection_pool& operator=(const connection_pool&) = delete;
		connection_pool& operator=(connection_pool&&) = delete;

		std::unique_ptr<async_connection> get_connection()
		{
			std::lock_guard<std::mutex> lock(connection_pool_mutex);
			if (!free_connections.empty())
			{
				auto connection = std::move(free_connections.top());
				free_connections.pop();
				return std::make_unique<async_connection>(connection, this);
			}

			try
			{
				return std::make_unique<async_connection>(std::move(std::make_unique<Connection>(config)), this);
			}
			catch (const sqlpp::exception& e)
			{
				std::cerr << "Failed to spawn a new connection." << std::endl;
				std::cerr << e.what() << std::endl;
				throw;
			}
		}

		// Caller is responsible for making sure the connection being
		// returned has the same configuration as the connection pool.
		void free_connection(std::unique_ptr<async_connection> connection)
		{
			std::lock_guard<std::mutex> lock(connection_pool_mutex);
			if (free_connections.size() >= maximum_pool_size)
			{
				// Exceeds default size, empty the async_connection object
				// so that it doesn't trigger RAII
				Connection destructor = std::move(*connection.get());
			}
			else
			{
				if (connection.get())
				{
					if (is_connection_compatible(connection))
					{
						free_connections.push(std::move(connection));
					}
					else
					{
						throw sqlpp::exception("Trying to free a connection with incompatible config.");
					}
				}
				else
				{
					throw sqlpp::exception("Trying to free an empty connection.");
				}
			}
		}
	};
}

#endif
