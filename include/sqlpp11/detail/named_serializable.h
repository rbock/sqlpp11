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

#include <ostream>
#include <vector>
#include <memory>

namespace sqlpp
{
	namespace detail
	{
		template<typename Db>
			struct named_serializable_t
			{
				template<typename T, 
					typename std::enable_if<not std::is_same<typename std::decay<T>::type, named_serializable_t<Db>>::value, int>::type = 0 // prevent accidental overload for copy constructor
						>
					named_serializable_t(T&& t):
						_impl(std::make_shared<_impl_t<typename std::decay<T>::type>>(std::forward<T>(t)))
				{}

				named_serializable_t(const named_serializable_t&) = default;
				named_serializable_t(named_serializable_t&&) = default;
				named_serializable_t& operator=(const named_serializable_t&) = default;
				named_serializable_t& operator=(named_serializable_t&&) = default;
				~named_serializable_t() = default;

				void serialize(std::ostream& os, Db& db) const
				{
					_impl->serialize(os, db);
				}

				std::string _get_name() const
				{
					_impl->_get_name();
				}

			private:
				struct _impl_base
				{
					virtual void serialize(std::ostream& os, Db& db) const = 0;
					virtual std::string _get_name() const = 0;
				};

				template<typename T>
					struct _impl_t: public _impl_base
				{
					_impl_t(const T& t):
						_t(t)
					{}

					_impl_t(T&& t):
						_t(std::move(t))
					{}

					void serialize(std::ostream& os, Db& db) const
					{
						_t.serialize(os, db);
					}

					std::string _get_name() const
					{
						return T::_name_t::_get_name();
					}

					T _t;
				};

				std::shared_ptr<const _impl_base> _impl;
			};
	}
}

#endif
