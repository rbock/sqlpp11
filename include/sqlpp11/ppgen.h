/*
 * Copyright (c) 2014-2015, niXman (i dot nixman dog gmail dot com)
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

#ifndef _sqlpp__ppgen__ppgen_h
#define _sqlpp__ppgen__ppgen_h

#include <sqlpp11/ppgen/blob.h>
#include <sqlpp11/ppgen/bool.h>
#include <sqlpp11/ppgen/character_set.h>
#include <sqlpp11/ppgen/datetime.h>
#include <sqlpp11/ppgen/engine.h>
#include <sqlpp11/ppgen/floating_point.h>
#include <sqlpp11/ppgen/integer.h>
#include <sqlpp11/ppgen/not_null.h>
#include <sqlpp11/ppgen/null.h>
#include <sqlpp11/ppgen/text.h>
#include <sqlpp11/ppgen/timestamp.h>
#include <sqlpp11/ppgen/varchar.h>
#include <sqlpp11/ppgen/wrap_seq.h>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/pop_front.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>

/***************************************************************************/

#define SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table) \
	BOOST_PP_TUPLE_ELEM(0, BOOST_PP_EXPAND table)

#define SQLPP_DECLARE_TABLE_GET_TABLE_PROPS(table) \
	BOOST_PP_TUPLE_POP_FRONT(BOOST_PP_EXPAND table)

/***************************************************************************/

#define SQLPP_DECLARE_TABLE_GET_COL_NAME(col) \
	BOOST_PP_TUPLE_ELEM(0, col)

#define SQLPP_DECLARE_TABLE_GET_COL_TYPE(col) \
	BOOST_PP_TUPLE_ELEM(1, col)

#define SQLPP_DECLARE_TABLE_GET_COL_PROPS(col) \
	BOOST_PP_TUPLE_POP_FRONT(BOOST_PP_TUPLE_POP_FRONT(col))

/***************************************************************************/

#define SQLPP_DECLARE_TABLE_PROCESS_PROPS(props) [props]

/***************************************************************************/

#define SQLPP_DECLARE_TABLE_ENUM_COLS_TYPES_IMPL(unused, data, idx, elem) \
	,BOOST_PP_CAT(data, _)::SQLPP_DECLARE_TABLE_GET_COL_NAME(elem)

#define SQLPP_DECLARE_TABLE_ENUM_COLS_TYPES(table, cols) \
	BOOST_PP_SEQ_FOR_EACH_I( \
		 SQLPP_DECLARE_TABLE_ENUM_COLS_TYPES_IMPL \
		,table \
		,cols \
	)

/***************************************************************************/

#define SQLPP_DECLARE_TABLE_IMPL(unused, data, idx, elem) \
	struct SQLPP_DECLARE_TABLE_GET_COL_NAME(elem) { \
		struct _name_t { \
			static constexpr const char* _get_name() \
			{ return BOOST_PP_STRINGIZE(SQLPP_DECLARE_TABLE_GET_COL_NAME(elem)); } \
			\
			template<typename T> \
			struct _member_t { \
				T SQLPP_DECLARE_TABLE_GET_COL_NAME(elem); \
				\
				T& operator()() { return SQLPP_DECLARE_TABLE_GET_COL_NAME(elem); } \
				const T& operator()() const { return SQLPP_DECLARE_TABLE_GET_COL_NAME(elem); } \
			}; /* struct _member_t */ \
			\
			using _traits = sqlpp::make_traits< \
				SQLPP_DECLARE_TABLE_PROCESS_PROPS(SQLPP_DECLARE_TABLE_GET_COL_PROPS(elem)) \
			>; \
		}; /* struct _name_t */ \
		\
	}; /* struct SQLPP_DECLARE_TABLE_GET_COL_NAME(elem) */

#define SQLPP_DECLARE_TABLE_AUX(table, cols) \
	namespace SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table) { \
		namespace BOOST_PP_CAT(SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table), _) { \
			BOOST_PP_SEQ_FOR_EACH_I( \
				SQLPP_DECLARE_TABLE_IMPL \
				,~ \
				,cols \
			) \
		} /* namespace BOOST_PP_CAT(SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table), _) */ \
		\
		struct SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table) \
			: sqlpp::table_t< \
				SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table) \
				SQLPP_DECLARE_TABLE_ENUM_COLS_TYPES( \
					SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table) \
					,cols \
				) \
			> \
		{ \
			struct _name_t { \
				static constexpr const char* _get_name() \
				{ return BOOST_PP_STRINGIZE(SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table)); } \
				\
				template<typename T> \
				struct _member_t { \
					T SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table); \
					\
					T& operator()() { return SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table); } \
					const T& operator()() const { return SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table); } \
					\
				}; /* struct _member_t */ \
				\
			}; /* struct _name_t */ \
			\
		}; /* struct SQLPP_DECLARE_TABLE_GET_TABLE_NAME(table) */ \
	\
	}

/***************************************************************************/

#define SQLPP_DECLARE_TABLE(table, cols) \
	SQLPP_DECLARE_TABLE_AUX( \
		 BOOST_PP_CAT(SQLPP_WRAP_SEQUENCE_X table, 0) \
		,BOOST_PP_CAT(SQLPP_WRAP_SEQUENCE_X cols, 0) \
	)

/***************************************************************************/

#endif // _sqlpp__ppgen__ppgen_h
