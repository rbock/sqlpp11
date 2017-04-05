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
#include <sqlpp11/exception.h>
#include <sqlpp11/connection_pool.h>

namespace sqlpp
{
	using namespace std::chrono_literals;
	struct async_query_service
	{
	private:
		struct async_io_service
		{
			std::vector<std::thread> io_threads;
			std::unique_ptr<SQLPP_ASIO::steady_timer> timer;
			SQLPP_ASIO::io_service& _impl;

			void timer_loop()
			{
				timer->expires_from_now(std::chrono::seconds(10));
				timer->async_wait(std::bind(&async_io_service::timer_loop, this));
			}

			async_io_service(SQLPP_ASIO::io_service& io_service, unsigned int thread_count)
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

			~async_io_service()
			{
				_impl.stop();
				for (auto& thread : io_threads)
				{
					thread.join();
				}
			}
		};
		async_io_service _io_service;

	public:
		async_query_service(SQLPP_ASIO::io_service& io_service, unsigned int thread_count)
			: _io_service(io_service, thread_count) {}

		template<typename Connection, typename Connection_config, typename Connection_validator, typename Query, typename Lambda>
		void async_query(connection_pool<Connection, Connection_config, Connection_validator>& connection_pool, Query query, Lambda callback)
		{
			_io_service._impl.post(
				[&]()
			{
				auto async_connection = connection_pool.get_connection();
				callback(std::move(async_connection(query)));
			}
			);
		}
	};
}

#endif
