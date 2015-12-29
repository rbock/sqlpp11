/*
 * Copyright (c) 2013-2015, Roland Bock
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

#ifndef SQLPP_TAB_SAMPLE_H
#define SQLPP_TAB_SAMPLE_H

#include <sqlpp11/table_base.h>
#include <sqlpp11/column_types.h>

// clang-format off
namespace TabFoo_
{
	struct Omega
	{
		struct _name_t
		{
			static constexpr const char* _get_name() { return "omega"; }
		};
		template<typename T>
		struct _member_t
		{
			/*
			template<typename... TT>
				_name_t(TT&&... t): omega(std::forward<TT>(t)...) {}

			template<typename TT>
				_name_t& operator=(TT&& t) { omega = std::forward<TT>(t); return *this; }
				*/

			T omega;
		};
		using _value_type = sqlpp::bigint;
		struct _column_type
		{
		};
	};
}

struct TabFoo: sqlpp::table_base_t<
									TabFoo,
									TabFoo_::Omega
													 >
{
	using _value_type = sqlpp::no_value_t;
	struct _name_t
	{
		static constexpr const char* _get_name() { return "tab_foo"; }
	};
	template<typename T>
		struct _member_t
		{
			T tabFoo;
		};
	template<typename Db>
		void serialize(std::ostream& os, Db& db) const
		{
			os << _name_t::_get_name();
		}
};

namespace TabSample_
{
	struct Alpha
	{
		struct _name_t
		{
			static constexpr const char* _get_name() { return "alpha"; }
		};
		template<typename T>
		struct _member_t
		{
			/*
			template<typename... TT>
				_name_t(TT&&... t): alpha(std::forward<TT>(t)...) {}

			template<typename TT>
				_name_t& operator=(TT&& t) { alpha = std::forward<TT>(t); return *this; }
				*/

			T alpha;
		};
		using _value_type = sqlpp::bigint;
		struct _column_type
		{
			using _must_not_insert = sqlpp::tag_yes;
			using _must_not_update = sqlpp::tag_yes;
			using _can_be_null = sqlpp::tag_yes;
			using _foreign_key = decltype(TabFoo::omega);
		};
	};

	struct Beta
	{
		struct _name_t
		{
			static constexpr const char* _get_name() { return "beta"; }
		};
		template<typename T>
		struct _member_t
		{
			T beta;
		};
		using _value_type = sqlpp::varchar;
		struct _column_type
		{
			using _can_be_null = sqlpp::tag_yes;
			using _must_not_update = sqlpp::tag_yes;
		};
	};

	struct Gamma
	{
		struct _name_t
		{
			static constexpr const char* _get_name() { return "gamma"; }
		};
		template<typename T>
		struct _member_t
		{
			T gamma;
		};
		using _value_type = sqlpp::boolean;
		struct _column_type
		{
			using _require_insert = sqlpp::tag_yes;
		};
	};
}

struct TabSample: sqlpp::table_base_t<
									TabSample,
									TabSample_::Alpha,
									TabSample_::Beta,
									TabSample_::Gamma
													 >
{
	using _value_type = sqlpp::no_value_t;
	struct _name_t
	{
		static constexpr const char* _get_name() { return "tab_sample"; }
	};
	template<typename T>
		struct _member_t
		{
			T tabSample;
		};
	template<typename Db>
		void serialize(std::ostream& os, Db& db) const
		{
			os << _name_t::_get_name();
		}
};

#endif
