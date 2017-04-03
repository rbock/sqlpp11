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
#include <unordered_map>
#include <memory>
#include <iostream>
#include <chrono>
#include <type_traits>
#include <sqlpp11/exception.h>
#include <sqlpp11/pool_connection.h>

namespace sqlpp
{
	namespace connection_validator
	{
		struct automatic
		{
			template<typename Connection>
			bool validate(Connection* connection)
			{
				if (!connection->is_valid())
				{
					try
					{
						connection->reconnect();
					}
					catch (const sqlpp::exception& e)
					{
						throw sqlpp::exception("Failed to reconnect to database.");
					}
				}
			}

			template<typename Connection>
			void deregister(Connection* connection) {}
		};

		using namespace std::chrono_literals;
		class periodic
		{
		private:
			std::chrono::seconds revalidate_interval;
			std::unordered_map<void*,std::chrono::time_point<std::chrono::system_clock>> last_checked;

		public:
			periodic(const std::chrono::seconds r = 28800s) //default wait_timeout in MySQL
				: revalidate_interval(r), last_checked() {}

			template<typename Connection>
			void validate(Connection* connection)
			{
				auto last = last_checked.find(connection);
				auto now = std::chrono::system_clock::now();
				if (last == last_checked.end())
				{
					last_checked.emplace_hint(last, connection, now);
				}

				if (now - last->second < revalidate_interval)
				{
					return;
				}
				
				if (!connection->is_valid())
				{
					try
					{
						connection->reconnect();
					}
					catch (const sqlpp::exception& e)
					{
						throw sqlpp::exception("Failed to reconnect to database.");
					}
				}

				last = now;
			}

			template<typename Connection>
			void deregister(Connection* con)
			{
				auto itr = last_checked.find(con);
				if(itr != last_checked.end())
				{
					last_checked.erase(itr);
				}
			}
		};

		struct none
		{
			template<typename Connection>
			void validate(Connection*) {}

			template<typename Connection>
			void deregister(Connection*) {}
		};
	}

	template <typename Connection_config,
		typename Connection_validator = connection_validator::automatic,
		typename Connection = typename std::enable_if<std::is_class<Connection_config::connection>::value, Connection_config::connection>::type>
	class connection_pool
	{
		friend pool_connection<Connection_config, Connection_validator, Connection>;

	private:
		std::mutex connection_pool_mutex;
		const std::shared_ptr<Connection_config> config;
		size_t maximum_pool_size = 0;
		std::stack<std::unique_ptr<Connection>> free_connections;
		Connection_validator connection_validator;

		void free_connection(std::unique_ptr<Connection>& connection)
		{
			std::lock_guard<std::mutex> lock(connection_pool_mutex);
			if (free_connections.size() >= maximum_pool_size)
			{
				// Exceeds default size, deregister left over info in the connection_validator and let connection self destroy.
				connection_validator.deregister(connection.get());
			}
			else
			{
				if (connection.get())
				{
					free_connections.push(std::move(connection));
				}
				else
				{
					throw sqlpp::exception("Trying to free an empty connection.");
				}
			}
		}

	public:
		connection_pool(const std::shared_ptr<Connection_config>& config, size_t pool_size)
			: config(config), maximum_pool_size(pool_size), connection_validator(Connection_validator()) {}
		~connection_pool() = default;
		connection_pool(const connection_pool&) = delete;
		connection_pool(connection_pool&& other)
			: config(std::move(other.config)), maximum_pool_size(std::move(other.maximum_pool_size)),
			connection_validator(std::move(other.connection_validator)) {}
		connection_pool& operator=(const connection_pool&) = delete;
		connection_pool& operator=(connection_pool&&) = delete;

		pool_connection<Connection_config, Connection_validator, Connection> get_connection()
		{
			std::lock_guard<std::mutex> lock(connection_pool_mutex);
			while (true)
			{
				try
				{
					if (!free_connections.empty())
					{
						auto connection = std::move(free_connections.top());
						free_connections.pop();
						connection_validator.validate(connection.get());

						return pool_connection<Connection_config, Connection_validator, Connection>(std::move(connection), this);
					}
					else
					{
						break;
					}
				}
				catch (const sqlpp::exception& e)
				{
					throw sqlpp::exception("Failed to retrieve a valid connection.");
				}
			}

			try
			{
				return pool_connection<Connection_config, Connection_validator, Connection>(std::move(std::make_unique<Connection>(config)), this);
			}
			catch (const sqlpp::exception& e)
			{
				throw sqlpp::exception("Failed to spawn a new connection.");
			}
		}
	};

	template<typename Connection_config,
		typename Connection_validator = connection_validator::automatic,
		typename Connection = typename std::enable_if<std::is_class<Connection_config::connection>::value,Connection_config::connection>::type>
	connection_pool<Connection_config, Connection_validator, Connection> make_connection_pool(
		const std::shared_ptr<Connection_config>& config,
		size_t max_pool_size)
	{
		return connection_pool<Connection_config, Connection_validator, Connection>(config, max_pool_size);
	}
}

#endif
