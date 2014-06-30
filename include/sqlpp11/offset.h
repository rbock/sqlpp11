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

#ifndef SQLPP_OFFSET_H
#define SQLPP_OFFSET_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
	// OFFSET DATA
	template<typename Offset>
		struct offset_data_t
		{
			offset_data_t(Offset value):
				_value(value)
			{}

			offset_data_t(const offset_data_t&) = default;
			offset_data_t(offset_data_t&&) = default;
			offset_data_t& operator=(const offset_data_t&) = default;
			offset_data_t& operator=(offset_data_t&&) = default;
			~offset_data_t() = default;

			Offset _value;
		};

	// OFFSET
	template<typename Offset>
		struct offset_t
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::offset>;
			using _recursive_traits = make_recursive_traits<Offset>;

			static_assert(is_integral_t<Offset>::value, "offset requires an integral value or integral parameter");

			// Data
			using _data_t = offset_data_t<Offset>;

			// Member implementation with data and methods
			template <typename Policies>
				struct _impl_t
				{
					_data_t _data;
				};

			// Member template for adding the named member to a statement
			template<typename Policies>
				struct _member_t
				{
					using _data_t = offset_data_t<Offset>;

					_impl_t<Policies> offset;
					_impl_t<Policies>& operator()() { return offset; }
					const _impl_t<Policies>& operator()() const { return offset; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.offset)
						{
							return t.offset;
						}
				};

			template<typename Policies>
				struct _methods_t
				{
					static void _check_consistency() {}
				};
		};

	// DYNAMIC OFFSET DATA
	template<typename Database>
		struct dynamic_offset_data_t
		{
			dynamic_offset_data_t():
				_value(noop())
			{
			}

			template<typename Offset>
				dynamic_offset_data_t(Offset value):
					_initialized(true),
					_value(typename wrap_operand<Offset>::type(value))
			{
			}

			dynamic_offset_data_t(const dynamic_offset_data_t&) = default;
			dynamic_offset_data_t(dynamic_offset_data_t&&) = default;
			dynamic_offset_data_t& operator=(const dynamic_offset_data_t&) = default;
			dynamic_offset_data_t& operator=(dynamic_offset_data_t&&) = default;
			~dynamic_offset_data_t() = default;

			bool _initialized = false;
			interpretable_t<Database> _value;
		};

	// DYNAMIC OFFSET
	template<typename Database>
		struct dynamic_offset_t
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::offset>;
			using _recursive_traits = make_recursive_traits<>;

			// Data
			using _data_t = dynamic_offset_data_t<Database>;

			// Member implementation with data and methods
			template <typename Policies>
				struct _impl_t
				{
					template<typename Offset>
						void set(Offset value)
						{
							// FIXME: Make sure that Offset does not require external tables? Need to read up on SQL
							using arg_t = typename wrap_operand<Offset>::type;
							_data._value = arg_t{value};
							_data._initialized = true;
						}
				public:
					_data_t _data;
				};

			// Member template for adding the named member to a statement
			template<typename Policies>
				struct _member_t
				{
					using _data_t = dynamic_offset_data_t<Database>;

					_impl_t<Policies> offset;
					_impl_t<Policies>& operator()() { return offset; }
					const _impl_t<Policies>& operator()() const { return offset; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.offset)
						{
							return t.offset;
						}
				};

			template<typename Policies>
				struct _methods_t
				{
					static void _check_consistency() {}

					template<typename Offset>
						void set_offset(Offset value)
						{
							// FIXME: Make sure that Offset does not require external tables? Need to read up on SQL
							using arg_t = typename wrap_operand<Offset>::type;
							static_cast<typename Policies::_statement_t*>(this)->_offset()._value = arg_t{value};
							static_cast<typename Policies::_statement_t*>(this)->_offset()._initialized = true;
						}
				};

			bool _initialized = false;
			interpretable_t<Database> _value;
		};

	struct no_offset_t
	{
		using _traits = make_traits<no_value_t, ::sqlpp::tag::noop>;
		using _recursive_traits = make_recursive_traits<>;

		// Data
		using _data_t = no_data_t;

		// Member implementation with data and methods
		template<typename Policies>
			struct _impl_t
			{
				_data_t _data;
			};

		// Member template for adding the named member to a statement
		template<typename Policies>
			struct _member_t
			{
				using _data_t = no_data_t;

				_impl_t<Policies> no_offset;
				_impl_t<Policies>& operator()() { return no_offset; }
				const _impl_t<Policies>& operator()() const { return no_offset; }

				template<typename T>
					static auto _get_member(T t) -> decltype(t.no_offset)
					{
						return t.no_offset;
					}
			};

		template<typename Policies>
			struct _methods_t
			{
				using _database_t = typename Policies::_database_t;
				template<typename T>
					using _new_statement_t = typename Policies::template _new_statement_t<no_offset_t, T>;

				static void _check_consistency() {}

				template<typename Arg>
					auto offset(Arg arg)
					-> _new_statement_t<offset_t<typename wrap_operand<Arg>::type>>
					{
						return { *static_cast<typename Policies::_statement_t*>(this), offset_data_t<typename wrap_operand<Arg>::type>{{arg}} };
					}

				auto dynamic_offset()
					-> _new_statement_t<dynamic_offset_t<_database_t>>
					{
						static_assert(not std::is_same<_database_t, void>::value, "dynamic_offset must not be called in a static statement");
						return { *static_cast<typename Policies::_statement_t*>(this), dynamic_offset_data_t<_database_t>{} };
					}
			};
	};

	// Interpreters
	template<typename Context, typename Offset>
		struct serializer_t<Context, offset_data_t<Offset>>
		{
			using T = offset_data_t<Offset>;

			static Context& _(const T& t, Context& context)
			{
				context << " OFFSET ";
				serialize(t._value, context);
				return context;
			}
		};

	template<typename Context, typename Database>
		struct serializer_t<Context, dynamic_offset_data_t<Database>>
		{
			using T = dynamic_offset_data_t<Database>;

			static Context& _(const T& t, Context& context)
			{
				if (t._initialized)
				{
					context << " OFFSET ";
					serialize(t._value, context);
				}
				return context;
			}
		};
}

#endif
