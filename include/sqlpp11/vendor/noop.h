/*
 * Copyright (c) 2013-2014, Roland Bock
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

#ifndef SQLPP_NOOP_H
#define SQLPP_NOOP_H

#include <type_traits>
#include <sqlpp11/no_value.h>
#include <sqlpp11/vendor/serializer.h>

namespace sqlpp
{
	namespace vendor
	{
#warning: Need extra include file for no_data
		struct no_data_t {};

		template<typename Context>
			struct serializer_t<Context, no_data_t>
			{
				using T = no_data_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};

		struct noop 
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::noop>;
			using _recursive_traits = make_recursive_traits<>;

			struct _name_t {};

			template<typename Policies>
				struct _result_methods_t
				{};
		};

		template<typename Context>
			struct serializer_t<Context, noop>
			{
				using T = noop;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};

		template<typename T>
			struct is_noop: std::is_same<T, noop> {};
	}

}
#endif
