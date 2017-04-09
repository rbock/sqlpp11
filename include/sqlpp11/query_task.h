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

#ifndef SQLPP_QUERY_TASK_H
#define SQLPP_QUERY_TASK_H

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <utility>
#include <type_traits>
#include <system_error>

#include <sqlpp11/error.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/connection_pool.h>

namespace sqlpp
{

	template<typename Connection_pool, typename Query, typename Lambda>
	class query_task
	{
	private:
		using Pool_connection = decltype((*((Connection_pool*)0)).get_connection());
		using Result = decltype((*((Pool_connection*)0))(Query()));

		template<class F, class...Args>
		struct is_callable
		{
			template<class U> static auto test(U* p) -> decltype((*p)(std::declval<Args>()...), void(), std::true_type());
			template<class U> static auto test(...) -> decltype(std::false_type());

			typedef decltype(test<F>(0)) type;
			static constexpr bool value = decltype(test<F>(0))::value;
		};

	public:
		query_task(Connection_pool& p, Query& q, Lambda& cb) : pool(p), query(q), callback(cb) {}

		void operator()()
		{
			Pool_connection connection;
			try
			{
				connection = std::move(pool.get_connection());
			}
			catch (const sqlpp::exception& e)
			{
				impl(sqlpp::error(sqlpp::error::connection_error, e.what()), std::move(connection), Result(), callback);
			}

			try
			{
				impl(sqlpp::error(sqlpp::error::ok), std::move(connection), std::move(connection(query)), callback);
			}
			catch (const std::exception& e)
			{
				impl(sqlpp::error(sqlpp::error::query_error, e.what()), std::move(connection), Result(), callback);
			}
		}

		template<typename Lambda, typename std::enable_if<!is_callable<Lambda>::value &&
			!is_callable<Lambda, sqlpp::error>::value &&
			!is_callable<Lambda, sqlpp::error, Result>::value &&
			!is_callable<Lambda, sqlpp::error, Result, Pool_connection>::value, int>::type = 0>
			void impl(sqlpp::error error, Pool_connection connection, Result result, Lambda& callback)
		{
			static_assert(false, "Callback signature is incompatible. Refer to the wiki for further instructions.");
		}

		template<typename Lambda, typename std::enable_if<is_callable<Lambda>::value, int>::type = 0>
		void impl(sqlpp::error error, Pool_connection connection, Result result, Lambda& callback)
		{
			callback();
		}

		template<typename Lambda, typename std::enable_if<is_callable<Lambda, sqlpp::error>::value, int>::type = 0>
		void impl(sqlpp::error error, Pool_connection connection, Result result, Lambda& callback)
		{
			callback(std::move(error));
		}

		template<typename Lambda, typename std::enable_if<is_callable<Lambda, sqlpp::error, Result>::value, int>::type = 0>
		void impl(sqlpp::error error, Pool_connection connection, Result result, Lambda& callback)
		{
			callback(std::move(error), std::move(result));
		}

		template<typename Lambda, typename std::enable_if<is_callable<Lambda, sqlpp::error, Result, Pool_connection>::value, int>::type = 0>
		void impl(sqlpp::error error, Pool_connection connection, Result result, Lambda& callback)
		{
			callback(std::move(error), std::move(result), std::move(connection));
		}

		Connection_pool& pool;
		Query query;
		Lambda callback;
	};

	template<typename Connection_pool, typename Query, typename Lambda>
	query_task<Connection_pool, Query, Lambda> make_query_task(Connection_pool& pool, Query& query, Lambda& lambda)
	{
		return query_task<Connection_pool, Query, Lambda>(pool, query, lambda);
	}

	template<typename Connection_pool, typename Query, typename Lambda>
	void async(Connection_pool& pool, Query& query, Lambda& callback)
	{
		std::async(std::launch::async, std::ref(pool), query, std::ref(callback));
	}
}

#endif
