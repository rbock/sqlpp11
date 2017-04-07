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

#pragma once

#ifndef SQLPP_ASYNC_QUERY_SERVICE_H
#define SQLPP_ASYNC_QUERY_SERVICE_H

#define SQLPP_ASIO_STANDALONE
#ifdef SQLPP_ASIO_STANDALONE
#define SQLPP_ASIO asio
#include <asio.hpp>
#else
#include <boost/asio.hpp>
#define SQLPP_ASIO boost::asio
#endif

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <utility>
#include <type_traits>

#include <sqlpp11/exception.h>
#include <sqlpp11/connection_pool.h>

namespace sqlpp
{
	namespace async
	{
		enum
		{
			ok = 0,
			error = 1
		};
	}
	template<typename Connection_pool, typename Query, typename Lambda>
	class query_poster
	{
	private:
		using Async_connection = decltype((*((Connection_pool*)0)).get_connection());
		using Result = decltype((*((Async_connection*)0))(Query()));

		template<class F, class...Args>
		struct is_callable
		{
			template<class U> static auto test(U* p) -> decltype((*p)(std::declval<Args>()...), void(), std::true_type());
			template<class U> static auto test(...) -> decltype(std::false_type());

			typedef decltype(test<F>(0)) type;
			static constexpr bool value = decltype(test<F>(0))::value;
		};

	public:
		query_poster(Connection_pool& p, Query& q, Lambda& cb) : pool(p), query(q), callback(cb) {}

		void operator()()
		{
			auto connection = pool.get_connection();
			auto error_code = std::error_code(sqlpp::async::ok, std::generic_category());
			Result result;
			try
			{
				result = std::move(connection(query));
			}
			catch (const sqlpp::exception&)
			{
				error_code.assign(sqlpp::async::error, std::generic_category());
			}

			impl(error_code, std::move(connection), std::move(result), callback);
		}

		template<typename Lambda, typename std::enable_if<!is_callable<Lambda>::value &&
			!is_callable<Lambda, std::error_code>::value &&
			!is_callable<Lambda, std::error_code, Result>::value &&
			!is_callable<Lambda, std::error_code, Result, Async_connection>::value, int>::type = 0>
		void impl(std::error_code error, Async_connection connection, Result result, Lambda callback)
		{
			static_assert(false, "Callback signature is incompatible. Refer to the wiki for further instructions.");
		}

		template<typename Lambda, typename std::enable_if<is_callable<Lambda>::value, int>::type = 0>
		void impl(std::error_code error_code, Async_connection connection, Result result, Lambda callback)
		{
			callback();
		}

		template<typename Lambda, typename std::enable_if<is_callable<Lambda, bool>::value, int>::type = 0>
		void impl(std::error_code error_code, Async_connection connection, Result result, Lambda callback)
		{
			callback(error_code);
		}

		template<typename Lambda, typename std::enable_if<is_callable<Lambda, bool, Result>::value, int>::type = 0>
		void impl(std::error_code error_code, Async_connection connection, Result result, Lambda callback)
		{
			callback(error_code, std::move(result));
		}

		template<typename Lambda, typename std::enable_if<is_callable<Lambda, bool, Result, Async_connection>::value, int>::type = 0>
		void impl(std::error_code error_code, Async_connection connection, Result result, Lambda callback)
		{
			callback(error_code, std::move(result), std::move(connection));
		}

		Connection_pool& pool;
		Query query;
		Lambda& callback;
	};

	using namespace std::chrono_literals;
	struct asio_query_service
	{
		std::vector<std::thread> io_threads;
		std::unique_ptr<SQLPP_ASIO::steady_timer> timer;
		SQLPP_ASIO::io_service& _impl;

		void timer_loop()
		{
			timer->expires_from_now(std::chrono::seconds(10));
			timer->async_wait(std::bind(&asio_query_service::timer_loop, this));
		}

		asio_query_service(SQLPP_ASIO::io_service& io_service, unsigned int thread_count)
			: _impl(io_service)
		{
			timer = std::make_unique<SQLPP_ASIO::steady_timer>(_impl);
			timer_loop(); // Keeps the io_service threads from returning

			for (unsigned i = 0; i < thread_count; i++)
			{
				try
				{
					io_threads.push_back(std::move(std::thread([&] {io_service.run(); })));
				}
				catch (std::exception e)
				{
					throw sqlpp::exception("Async io service failed to create io threads.");
				}
			}
		}

		~asio_query_service()
		{
			_impl.stop();
			for (auto& thread : io_threads)
			{
				thread.join();
			}
		}

		template<typename Connection_pool, typename Query, typename Lambda>
		void async_query(Connection_pool& pool, Query& query, Lambda& callback)
		{
			_impl.post(query_poster<Connection_pool, Query, Lambda>(pool, query, callback));
		}

	};

	template <typename Io_service>
	struct async_query_service
	{
	private:
		// todo: support for other I/O libraries
		asio_query_service _io_service;

	public:
		async_query_service(Io_service& io_service, unsigned int thread_count)
			: _io_service(io_service, thread_count) {}

		template<typename Connection, typename Connection_config, typename Connection_validator, typename Query, typename Lambda>
		void async_query(connection_pool<Connection_config, Connection_validator, Connection>& pool, Query& query, Lambda& callback)
		{
			_io_service.async_query(pool, query, callback);
		}
	};
}

#endif
