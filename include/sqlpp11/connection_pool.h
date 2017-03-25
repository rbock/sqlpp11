#pragma once
#include <mutex>
#include <vector>

namespace sqlpp
{
	namespace mysql
	{
		template <typename Connection, typename Connection_config>
		class connection_pool
		{
		private:
			std::mutex connection_pool_mutex;
			const std::shared_ptr<Connection_config> config;

			struct async_connection : Connection
			{
				async_connection(std::shared_ptr<Connection_config> config)
					: Connection(config)
				{}
				bool is_free = true;
			};

			std::vector<async_connection> connections;

			unsigned int default_pool_size = 0;
			unsigned int current_pool_size = 0;
			unsigned int max_pool_size = 0;

			void resize(unsigned int target_pool_size)
			{
				int lock = try_lock(connection_pool_mutex);
				if (lock == -1)
				{
					throw sqlpp::exception("Connection_pool resize should not be called before locking.");
					connection_pool_mutex.unlock();
				}
				else
				{
					int diff = current_pool_size - target_pool_size;
					if (!diff)
					{
						return;
					}

					// does not guarantee to resize to target_pool_size
					if (diff > 0)
					{
						for (auto it = connections.begin(); it != connections.end();)
						{
							if (diff && (*it).is_free)
							{
								it = connections.erase(it);
								diff--;
							}
							else
							{
								++it;
							}
						}
					}

					if (diff < 0)
					{
						connections.reserve(target_pool_size);
						for (int i = 0, count = -diff; i < count; i++)
						{
							try
							{
								connections.push_back(async_connection(config));
								diff++;
							}
							catch (const sqlpp::exception& e)
							{
								std::cerr << "Failed to spawn new connection." << std::endl;
								std::cerr << e.what() << std::endl;
							}
						}
					}
					current_pool_size = target_pool_size + diff;
				}
			}

		public:
			connection_pool(const std::shared_ptr<Connection_config>& config, unsigned int pool_size)
				: config(config), default_pool_size(pool_size)
			{
				std::lock_guard<std::mutex> lock(connection_pool_mutex);
				try
				{
					resize(pool_size);
				}
				catch (const sqlpp::exception& e)
				{
					std::cerr << "Failed to resize connection pool." << std::endl;
					std::cerr << e.what() << std::endl;
				}
			}
			~connection_pool() = default;
			connection_pool(const connection_pool&) = delete;
			connection_pool(connection_pool&&) = delete;
			connection_pool& operator=(const connection_pool&) = delete;
			connection_pool& operator=(connection_pool&&) = delete;

			std::shared_ptr<Connection> get_connection()
			{
				std::lock_guard<std::mutex> lock(connection_pool_mutex);
				for (auto& connection : connections)
				{
					if (connection.is_free)
					{
						connection.is_free = false;
						return std::shared_ptr<Connection>(static_cast<Connection*>(&connection));
					}
				}

				try
				{
					resize(current_pool_size + 1);
				}
				catch (const sqlpp::exception& e)
				{
					std::cerr << "Failed to resize connection pool." << std::endl;
					std::cerr << e.what() << std::endl;
				}

				return std::shared_ptr<Connection>(static_cast<Connection*>(&connections.back()));
			}

			void free_connection(const std::shared_ptr<Connection>& connection)
			{
				std::lock_guard<std::mutex> lock(connection_pool_mutex);
				static_cast<async_connection*>(connection.get())->is_free = true;
			}
		};
	}
}
