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

#ifndef SQLPP_VENDOR_SELECT_FLAG_LIST_H
#define SQLPP_VENDOR_SELECT_FLAG_LIST_H

#include <tuple>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/select_flags.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/vendor/policy_update.h>

namespace sqlpp
{
	namespace vendor
	{
		// SELECT FLAGS
		template<typename Database, typename... Flags>
			struct select_flag_list_t
			{
				using _is_select_flag_list = std::true_type; 
				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
				using _parameter_tuple_t = std::tuple<Flags...>;
				using size = std::tuple_size<_parameter_tuple_t>;

				static_assert(not ::sqlpp::detail::has_duplicates<Flags...>::value, "at least one duplicate argument detected in select flag list");

				static_assert(::sqlpp::detail::all_t<is_select_flag_t, Flags...>::value, "at least one argument is not a select flag in select flag list");

				select_flag_list_t(Flags... flags):
					_flags(flags...)
				{}

				select_flag_list_t(const select_flag_list_t&) = default;
				select_flag_list_t(select_flag_list_t&&) = default;
				select_flag_list_t& operator=(const select_flag_list_t&) = default;
				select_flag_list_t& operator=(select_flag_list_t&&) = default;
				~select_flag_list_t() = default;

				template<typename Policies>
					struct _methods_t
					{
						template<typename Flag>
							void add_flag(Flag flag)
							{
								static_assert(_is_dynamic::value, "add_flag must not be called for static select flags");
								static_assert(is_select_flag_t<Flag>::value, "invalid select flag argument in add_flag()");
#warning: Need to dispatch to actual add method to prevent error messages from being generated
								return static_cast<typename Policies::_select_t*>(this)->_flag_list._dynamic_flags.emplace_back(flag);
							}
					};

				const select_flag_list_t& _flag_list() const { return *this; }
				_parameter_tuple_t _flags;
				vendor::interpretable_list_t<Database> _dynamic_flags;
			};

		struct no_select_flag_list_t
		{
			using _is_noop = std::true_type;

			template<typename Policies>
				struct _methods_t
				{
					using _database_t = typename Policies::_database_t;
					template<typename T>
					using _new_select_t = typename Policies::template _policies_update_t<no_select_flag_list_t, T>;

					template<typename... Args>
						auto flags(Args... args)
						-> _new_select_t<select_flag_list_t<void, Args...>>
						{
							return { *static_cast<typename Policies::_select_t*>(this), select_flag_list_t<void, Args...>{args...} };
						}

					template<typename... Args>
						auto dynamic_flags(Args... args)
						-> _new_select_t<select_flag_list_t<_database_t, Args...>>
						{
							static_assert(not std::is_same<_database_t, void>::value, "dynamic_flags must not be called in a static statement");
							return { *static_cast<typename Policies::_select_t*>(this), vendor::select_flag_list_t<_database_t, Args...>{args...} };
						}
				};
		};


		// Interpreters
		template<typename Context, typename Database, typename... Flags>
			struct serializer_t<Context, select_flag_list_t<Database, Flags...>>
			{
				using T = select_flag_list_t<Database, Flags...>;

				static Context& _(const T& t, Context& context)
				{
					interpret_tuple(t._flags, ' ', context);
					if (sizeof...(Flags))
						context << ' ';
					interpret_list(t._dynamic_flags, ',', context);
					if (not t._dynamic_flags.empty())
						context << ' ';
					return context;
				}
			};

		template<typename Context>
			struct serializer_t<Context, no_select_flag_list_t>
			{
				using T = no_select_flag_list_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};
	}

}

#endif
