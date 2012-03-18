/* a getopt.
   version 0.1, march, 2012

   Copyright (C) 2012- Fredrik Kihlander

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.

   Fredrik Kihlander
*/

#include <gtest/gtest.h>
#include <getopt/getopt.h>

#define ARRAY_LENGTH( arr ) ( sizeof( arr ) / sizeof( arr[0] ) )

int g_flag = 0xFFFFFFFF;

static const getopt_option_t option_list[] = 
{
	{ "aaaa", 'a', GETOPT_OPTION_TYPE_NO_ARG,   0x0,      'a', "help a" },
	{ "bbbb", 'b', GETOPT_OPTION_TYPE_NO_ARG,   0x0,      'b', "help b" },
	{ "cccc", 'c', GETOPT_OPTION_TYPE_REQUIRED, 0x0,      'c', "help c" },
	{ "dddd", 'd', GETOPT_OPTION_TYPE_OPTIONAL, 0x0,      'd', "help d" },
	{ "eeee", 'e', GETOPT_OPTION_TYPE_FLAG_SET, &g_flag, 1337, "help e" },
	{ "ffff", 'f', GETOPT_OPTION_TYPE_FLAG_AND, &g_flag,    1, "help f" },
	{ "gggg", 'g', GETOPT_OPTION_TYPE_FLAG_OR , &g_flag,    1, "help g" },
	{ 0 } // end option_list
};

void test_get_opt_simple( int argc, const char** argv )
{
	bool got_a = false;
	bool got_b = false;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	EXPECT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'a': EXPECT_FALSE( got_a ); got_a = true; break;
			case 'b': EXPECT_FALSE( got_b ); got_b = true; break;
			default:
				EXPECT_TRUE( false ) << "got opt '" << (char)opt << "'";
				break;
		}
	}

	EXPECT_TRUE( got_a );
	EXPECT_TRUE( got_b );	
}

TEST( getopt, short_opt )
{
	const char* argv1[] = { "dummy_prog", "-a", "-b" };
	const char* argv2[] = { "dummy_prog", "--aaaa", "--bbbb" };
	test_get_opt_simple( ARRAY_LENGTH( argv1 ), argv1 );
	test_get_opt_simple( ARRAY_LENGTH( argv2 ), argv2 );
}

TEST( getopt, unknown_flags )
{
	const char* argv[] = { "dummy_prog", "-p", "--pppp", "--ccccc" };
	int argc           = ARRAY_LENGTH( argv );

	int unknow_count = 0;
	const char* unknown_flags[3] = { 0x0 };

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	EXPECT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case '?':  unknown_flags[unknow_count++] = ctx.current_opt_arg; break;
			default:
				EXPECT_TRUE( false ) << "got opt '" << (char)opt << "'";
				break;
		}
	}

	EXPECT_EQ(3, unknow_count);
	EXPECT_STREQ("-p", unknown_flags[0]);
	EXPECT_STREQ("--pppp", unknown_flags[1]);
	EXPECT_STREQ("--ccccc", unknown_flags[2]);
}

void test_with_arg( int argc, const char** argv )
{
	int value_count = 0;
	const char* values[2] = { 0x0, 0x0 };

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	EXPECT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'c':  values[value_count++] = ctx.current_opt_arg; break;
			default:
				EXPECT_TRUE( false ) << "got opt '" << (char)opt << "'";
				break;
		}
	}

	EXPECT_EQ( 2, value_count );
	EXPECT_STREQ( "c_value_1", values[0] );
	EXPECT_STREQ( "c_value_2", values[1] );	
}

void test_missing_arg( int argc, const char** argv )
{
	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	EXPECT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'c': EXPECT_TRUE( false ); break; // we should not get a call to c since it requires an argument
			case 'b': break;
			case '!': EXPECT_STREQ("cccc", ctx.current_opt_arg ); break; // flag should be stored in current_opt_arg
			default:
				EXPECT_TRUE( false ) << "got opt '" << (char)opt << "'";
				break;
		}
	}
}

TEST( getopt, with_args_short )
{
	const char* argv[] = { "dummy_prog", "-c", "c_value_1", "-c", "c_value_2" }; 
	test_with_arg( ARRAY_LENGTH( argv ), argv );
}

TEST( getopt, with_args_long )
{
	const char* argv1[] = { "dummy_prog", "--cccc=c_value_1",         "--cccc=", "c_value_2" };
	const char* argv2[] = { "dummy_prog", "--cccc", "=c_value_1",     "--cccc", "=", "c_value_2" };
	test_with_arg( ARRAY_LENGTH( argv1 ), argv1 );
	test_with_arg( ARRAY_LENGTH( argv2 ), argv2 );
}

TEST( getopt, with_args_short_after_long )
{
	const char* argv1[] = { "dummy_prog", "--cccc=c_value_1",         "-c", "c_value_2" };
	const char* argv2[] = { "dummy_prog", "--cccc", "=c_value_1",     "-c", "c_value_2" };
	const char* argv3[] = { "dummy_prog", "--cccc", "=", "c_value_1", "-c", "c_value_2" };

	test_with_arg( ARRAY_LENGTH( argv1 ), argv1 );
	test_with_arg( ARRAY_LENGTH( argv2 ), argv2 );
	test_with_arg( ARRAY_LENGTH( argv3 ), argv3 );
}

TEST( getopt, missing_arg_short )
{
	const char* argv1[] = { "dummy_prog", "-c",     "-b" }; 
	const char* argv2[] = { "dummy_prog", "--cccc", "-b" };
	test_missing_arg( ARRAY_LENGTH( argv1 ), argv1 );
	test_missing_arg( ARRAY_LENGTH( argv2 ), argv2 );
}

void test_optional_arg( int argc, const char** argv )
{
	const char* arguments[2] = { "put_arg1_here", "put_arg2_here" };
	int d_count = 0;
	int b_count  = 0;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	EXPECT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'd': arguments[d_count++] = ctx.current_opt_arg; break;
			case 'b': ++b_count; break;
			default:
				EXPECT_TRUE( false ) << "got opt '" << (char)opt << "' " << "value '" << ctx.current_opt_arg << "'";
				break;
		}
	}

	EXPECT_EQ(2, d_count);
	EXPECT_EQ(2, b_count);
	EXPECT_EQ((const char*)0x0, arguments[0] );
	EXPECT_STREQ("arg", arguments[1] );
}

TEST( getopt, optional_arg )
{
	const char* argv1[] = { "dummy_prog", "-d", "-b", "-d", "arg", "-b" };
	const char* argv2[] = { "dummy_prog", "--dddd", "-b", "--dddd=", "arg", "-b" };
	test_optional_arg( ARRAY_LENGTH( argv1 ), argv1 );
	test_optional_arg( ARRAY_LENGTH( argv2 ), argv2 );
}

TEST( getopt, non_arguments )
{
	const char* argv[] = { "dummy_prog", "-c", "arg1", "non_arg1", "--cccc=arg2", "non_arg2", "non_arg3" };
	int argc = ARRAY_LENGTH( argv );

	const char* args[2]      = { "fill1", "fill2" };
	const char* non_args[3]  = { "fill3", "fill4", "fill5" };
	int args_count     = 0;
	int non_args_count = 0;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	EXPECT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'c': args[args_count++]         = ctx.current_opt_arg; break;
			case '+': non_args[non_args_count++] = ctx.current_opt_arg; break;
			default:
				EXPECT_TRUE( false ) << "got opt '" << (char)opt << "' " << "value '" << ctx.current_opt_arg << "'";
				break;
		}
	}

	EXPECT_EQ(2, args_count);
	EXPECT_EQ(3, non_args_count);
	EXPECT_STREQ("arg1", args[0]);
	EXPECT_STREQ("arg2", args[1]);
	EXPECT_STREQ("non_arg1", non_args[0]);
	EXPECT_STREQ("non_arg2", non_args[1]);
	EXPECT_STREQ("non_arg3", non_args[2]);
}

TEST( getopt, set_flag )
{
	g_flag = 0xFFFFFFFF;

	const char* argv[] = { "dummy_prog", "-g", "-f", "-g", "-f", "-e" }; // ( ( ( 0xFFFFFFFF | 1 ) & 1 ) | 1 ) & 1
	int argc = ARRAY_LENGTH( argv );

	int flag_values[5] = { 0 };
	int num_flags = 0;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	EXPECT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case '\0': flag_values[num_flags++] = g_flag; break;
			default:
				EXPECT_TRUE( false ) << "got opt '" << (char)opt << "' " << "value '" << ctx.current_opt_arg << "'";
				break;
		}
	}

	EXPECT_EQ(5, num_flags);
	int expect0 = 0xFFFFFFFF;
	int expect1 = expect0 | 1;
	int expect2 = expect1 & 1;
	int expect3 = expect2 | 1;
	int expect4 = expect3 & 1;
	EXPECT_EQ(expect1, flag_values[0] );
	EXPECT_EQ(expect2, flag_values[1] );
	EXPECT_EQ(expect3, flag_values[2] );
	EXPECT_EQ(expect4, flag_values[3] );
	EXPECT_EQ(1337,    flag_values[4] );
}

int main( int argc, char** argv )
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
