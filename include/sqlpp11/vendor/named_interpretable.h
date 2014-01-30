/*
 * Copyright (c) 2013, Roland Bock
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

#ifndef SQLPP_NAMED_SERIALIZABLE_H
#define SQLPP_NAMED_SERIALIZABLE_H

#include <memory>
#include <sqlpp11/serializer.h>
#include <sqlpp11/parameter_list.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename Db>
			struct named_interpretable_t
			{
				using _context_t = typename Db::_context_t;

				template<typename T>
					named_interpretable_t(T t):
						_impl(std::make_shared<_impl_t<T>>(t))
				{}

				named_interpretable_t(const named_interpretable_t&) = default;
				named_interpretable_t(named_interpretable_t&&) = default;
				named_interpretable_t& operator=(const named_interpretable_t&) = default;
				named_interpretable_t& operator=(named_interpretable_t&&) = default;
				~named_interpretable_t() = default;

				sqlpp::serializer_t& interpret(sqlpp::serializer_t& context) const
				{
					return _impl->interpret(context);
				}

				_context_t& interpret(_context_t& context) const
				{
					return _impl->interpret(context);
				}

				std::string _get_name() const
				{
					_impl->_get_name();
				}

			private:
				struct _impl_base
				{
					virtual sqlpp::serializer_t& interpret(sqlpp::serializer_t& context) const = 0;
					virtual _context_t& interpret(_context_t& context) const = 0;
					virtual std::string _get_name() const = 0;
				};

				template<typename T>
					struct _impl_t: public _impl_base
				{
					static_assert(not make_parameter_list_t<T>::type::size::value, "parameters not supported in dynamic statement parts");
					_impl_t(T t):
						_t(t)
					{}

					sqlpp::serializer_t& interpret(sqlpp::serializer_t& context) const
					{
						sqlpp::interpret(_t, context);
						return context;
					}

					_context_t& interpret(_context_t& context) const
					{
						sqlpp::interpret(_t, context);
						return context;
					}

					std::string _get_name() const
					{
						return T::_name_t::_get_name();
					}

					T _t;
				};

				std::shared_ptr<const _impl_base> _impl;
			};

		template<typename Context, typename Database>
			struct interpreter_t<Context, named_interpretable_t<Database>>
			{
				using T = named_interpretable_t<Database>;

				static Context& _(const T& t, Context& context)
				{
					t.interpret(context);
					return context;
				}
			};
	}

}

#endif
