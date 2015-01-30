/* **************************************************************************
 *                                                                          *
 *     (C) Copyright Edward Diener 2013.
 *     Distributed under the Boost Software License, Version 1.0. (See
 *     accompanying file LICENSE_1_0.txt or copy at
 *     http://www.boost.org/LICENSE_1_0.txt)
 *                                                                          *
 ************************************************************************** */

/* See http://www.boost.org for most recent version. */

# ifndef SQLPP_BOOST_PREPROCESSOR_TUPLE_POP_FRONT_HPP
# define SQLPP_BOOST_PREPROCESSOR_TUPLE_POP_FRONT_HPP

# include <boost/preprocessor/config/config.hpp>

# if BOOST_PP_VARIADICS

# include <boost/preprocessor/array/pop_front.hpp>
# include <boost/preprocessor/array/to_tuple.hpp>
# include <boost/preprocessor/comparison/greater.hpp>
# include <boost/preprocessor/control/iif.hpp>
# include <boost/preprocessor/tuple/size.hpp>
# include <boost/preprocessor/tuple/to_array.hpp>


/* BOOST_PP_TUPLE_POP_FRONT */
# define SQLPP_BOOST_PP_TUPLE_POP_FRONT(tuple) \
	BOOST_PP_IIF \
		( \
		BOOST_PP_GREATER(BOOST_PP_TUPLE_SIZE(tuple),1), \
		SQLPP_BOOST_PP_TUPLE_POP_FRONT_EXEC, \
		SQLPP_BOOST_PP_TUPLE_POP_FRONT_RETURN \
		) \
	(tuple)

# define SQLPP_BOOST_PP_TUPLE_POP_FRONT_EXEC(tuple) \
	BOOST_PP_ARRAY_TO_TUPLE(BOOST_PP_ARRAY_POP_FRONT(BOOST_PP_TUPLE_TO_ARRAY(tuple)))

# define SQLPP_BOOST_PP_TUPLE_POP_FRONT_RETURN(tuple) tuple

# endif // BOOST_PP_VARIADICS

# endif // SQLPP_BOOST_PREPROCESSOR_TUPLE_POP_FRONT_HPP
