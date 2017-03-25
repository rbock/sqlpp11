#pragma once
#include <mutex>
#include <stack>
#include <memory>

namespace sqlpp
{
	template <typename Connection, typename Connection_config>
	class connection_pool
	{
	private:
		std::mutex connection_pool_mutex;
		const std::shared_ptr<Connection_config> config;
		unsigned int default_pool_size = 0;
		std::stack<std::unique_ptr<Connection>> free_connections;

	public:
		connection_pool(const std::shared_ptr<Connection_config>& config, unsigned int pool_size)
			: config(config), default_pool_size(pool_size)
		{
			std::lock_guard<std::mutex> lock(connection_pool_mutex);
			try
			{
				for (int i = 0; i < pool_size; i++)
				{
					free_connections.push(std::make_unique<Connection>(config));
				}
			}
			catch (const sqlpp::exception& e)
			{
				std::cerr << "Failed to spawn new connection." << std::endl;
				std::cerr << e.what() << std::endl;
			}
		}
		~connection_pool() = default;
		connection_pool(const connection_pool&) = delete;
		connection_pool(connection_pool&&) = delete;
		connection_pool& operator=(const connection_pool&) = delete;
		connection_pool& operator=(connection_pool&&) = delete;

		std::unique_ptr<Connection> get_connection()
		{
			std::lock_guard<std::mutex> lock(connection_pool_mutex);
			if (!free_connections.empty())
			{
				auto connection = std::move(free_connections.top());
				free_connections.pop();
				return connection;
			}

			try
			{
				auto connection = std::make_unique<Connection>(config);
				return connection;
			}
			catch (const sqlpp::exception& e)
			{
				std::cerr << "Failed to spawn new connection." << std::endl;
				std::cerr << e.what() << std::endl;
				return std::unique_ptr<Connection>();
			}
		}

		void free_connection(std::unique_ptr<Connection> connection)
		{
			std::lock_guard<std::mutex> lock(connection_pool_mutex);
			if (free_connections.size() >= default_pool_size)
			{
				// Exceeds default size, do nothing and let unique_ptr self destroy.
			}
			else
			{
				// TODO: we don't know if the connection is originally from this connection pool.
				// There's no way to check because we don't have access to config info in the Connection class:
				// connection.get()->_handle->config  --- _handle is private
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
	};
}
