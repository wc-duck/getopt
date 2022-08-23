/* a getopt.
   version 0.1, march, 2012

   Copyright (C) 2012- Fredrik Kihlander

   https://github.com/wc-duck/getopt

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

#include "greatest.h"
#include <getopt/getopt.h>

#define ARRAY_LENGTH( arr ) ( sizeof( arr ) / sizeof( arr[0] ) )

int g_flag = -1;

static const getopt_option_t option_list[] = 
{
	{ "aaaa", 'a', GETOPT_OPTION_TYPE_NO_ARG,          0x0,      'a', "help a", 0 },
	{ "bbbb", 'b', GETOPT_OPTION_TYPE_NO_ARG,          0x0,      'b', "help b", 0 },
	{ "cccc", 'c', GETOPT_OPTION_TYPE_REQUIRED,        0x0,      'c', "help c", 0 },
	{ "dddd", 'd', GETOPT_OPTION_TYPE_OPTIONAL,        0x0,      'd', "help d", 0 },
	{ "eeee", 'e', GETOPT_OPTION_TYPE_FLAG_SET,        &g_flag, 1337, "help e", 0 },
	{ "ffff", 'f', GETOPT_OPTION_TYPE_FLAG_AND,        &g_flag,    1, "help f", 0 },
	{ "gggg", 'g', GETOPT_OPTION_TYPE_FLAG_OR ,        &g_flag,    1, "help g", 0 },
	{ "hhhh", 'h', GETOPT_OPTION_TYPE_NO_ARG,          0x0,        0, "help h", 0 },
	GETOPT_OPTIONS_END
};

static const getopt_option_t parse_option_list[] = 
{
	{ "ri32", 'a', GETOPT_OPTION_TYPE_REQUIRED_INT32,  0x0,      'a', "help a", 0 },
	{ "rf32", 'b', GETOPT_OPTION_TYPE_REQUIRED_FP32,   0x0,      'b', "help b", 0 },
	{ "oi32", 'c', GETOPT_OPTION_TYPE_OPTIONAL_INT32,  0x0,      'c', "help c", 0 },
	{ "of32", 'd', GETOPT_OPTION_TYPE_OPTIONAL_FP32,   0x0,      'd', "help d", 0 },
	GETOPT_OPTIONS_END
};

int test_get_opt_simple( int argc, const char** argv )
{
	bool got_a = false;
	bool got_b = false;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	ASSERT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'a': ASSERT( !got_a ); got_a = true; break; // TODO: TRY!!!, should be false
			case 'b': ASSERT( !got_b ); got_b = true; break;
			default:
				FAILm( "got an unexpected opt!" );
				break;
		}
	}

	ASSERT( got_a );
	ASSERT( got_b );
	return 0;
}

TEST short_opt()
{
	const char* argv1[] = { "dummy_prog", "-a", "-b" };
	const char* argv2[] = { "dummy_prog", "--aaaa", "--bbbb" };
	if( test_get_opt_simple( (int)ARRAY_LENGTH( argv1 ), argv1 ) != 0 )
		return -1;
	if( test_get_opt_simple( (int)ARRAY_LENGTH( argv2 ), argv2 ) != 0 )
		return -1;
	return 0;
}

TEST unknown_flags()
{
	const char* argv[] = { "dummy_prog", "-p", "--pppp", "--ccccc" };
	int argc           = (int)ARRAY_LENGTH( argv );

	int unknow_count = 0;
	const char* unknown_flags[3] = { 0x0 };

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	ASSERT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case '?':  unknown_flags[unknow_count++] = ctx.current_opt_arg; break;
			default:
				FAILm( "got an unexpected opt!" );
				break;
		}
	}

	ASSERT_EQ(3, unknow_count);
	ASSERT_STR_EQ("-p", unknown_flags[0]);
	ASSERT_STR_EQ("--pppp", unknown_flags[1]);
	ASSERT_STR_EQ("--ccccc", unknown_flags[2]);

	return 0;
}

int test_with_arg( int argc, const char** argv )
{
	int value_count = 0;
	const char* values[2] = { 0x0, 0x0 };

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	ASSERT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'c':  values[value_count++] = ctx.current_opt_arg; break;
			default:
				FAILm( "got an unexpected opt!" );
				break;
		}
	}

	ASSERT_EQ( 2, value_count );
	ASSERT_STR_EQ( "c_value_1", values[0] );
	ASSERT_STR_EQ( "c_value_2", values[1] );
	return 0;
}

int test_missing_arg( int argc, const char** argv )
{
	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	ASSERT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'c': FAILm( "we should not get a call to c since it requires an argument" ); break;
			case 'b': break;
			case '!': ASSERT_STR_EQ("cccc", ctx.current_opt_arg ); break; // flag should be stored in current_opt_arg
			default:
				FAILm( "got an unexpected opt!" );
				break;
		}
	}

	return 0;
}

int test_zero_args( int argc, const char** argv )
{
	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	ASSERT_EQ( 0, err );

	int opt;
	if( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		FAILm("No arguments should have been parsed since argc was 0!");
	}

	return 0;
}

TEST with_zero_args()
{
	if (test_zero_args( 0, NULL ) != 0 ) return -1;
	if (test_zero_args( 1, NULL ) != 0 ) return -1;
	return 0;
}



TEST with_args_short()
{
	const char* argv[] = { "dummy_prog", "-c", "c_value_1", "-c", "c_value_2" };
	return test_with_arg( (int)ARRAY_LENGTH( argv ), argv );
}

TEST with_args_long()
{
	const char* argv1[] = { "dummy_prog", "--cccc=c_value_1",         "--cccc=", "c_value_2" };
	const char* argv2[] = { "dummy_prog", "--cccc", "=c_value_1",     "--cccc", "=", "c_value_2" };
	const char* argv3[] = { "dummy_prog", "--cccc", "c_value_1",      "--cccc", "c_value_2" };
	if( test_with_arg( (int)ARRAY_LENGTH( argv1 ), argv1 ) != 0 ) return -1;
	if( test_with_arg( (int)ARRAY_LENGTH( argv2 ), argv2 ) != 0 ) return -1;
	if( test_with_arg( (int)ARRAY_LENGTH( argv3 ), argv3 ) != 0 ) return -1;
	return 0;
}

TEST with_args_short_after_long()
{
	const char* argv1[] = { "dummy_prog", "--cccc=c_value_1",         "-c", "c_value_2" };
	const char* argv2[] = { "dummy_prog", "--cccc", "=c_value_1",     "-c", "c_value_2" };
	const char* argv3[] = { "dummy_prog", "--cccc", "=", "c_value_1", "-c", "c_value_2" };

	if( test_with_arg( (int)ARRAY_LENGTH( argv1 ), argv1 ) != 0 ) return -1;
	if( test_with_arg( (int)ARRAY_LENGTH( argv2 ), argv2 ) != 0 ) return -1;
	if( test_with_arg( (int)ARRAY_LENGTH( argv3 ), argv3 ) != 0 ) return -1;

	return 0;
}

TEST missing_arg_short()
{
	const char* argv1[] = { "dummy_prog", "-c",     "-b" };
	const char* argv2[] = { "dummy_prog", "--cccc", "-b" };
	if( test_missing_arg( (int)ARRAY_LENGTH( argv1 ), argv1 ) != 0 ) return -1;
	if( test_missing_arg( (int)ARRAY_LENGTH( argv2 ), argv2 ) != 0 ) return -1;
	return 0;
}

int test_optional_arg( int argc, const char** argv )
{
	const char* arguments[2] = { "put_arg1_here", "put_arg2_here" };
	int d_count = 0;
	int b_count  = 0;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	ASSERT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'd': arguments[d_count++] = ctx.current_opt_arg; break;
			case 'b': ++b_count; break;
			default:
				FAILm( "got an unexpected opt!" );
				break;
		}
	}

	ASSERT_EQ(2, d_count);
	ASSERT_EQ(2, b_count);
	ASSERT_EQ((const char*)0x0, arguments[0] );
	ASSERT_STR_EQ("arg", arguments[1] );

	return 0;
}

TEST optional_arg()
{
	const char* argv1[] = { "dummy_prog", "-d", "-b", "-d", "arg", "-b" };
	const char* argv2[] = { "dummy_prog", "--dddd", "-b", "--dddd=", "arg", "-b" };
	if( test_optional_arg( (int)ARRAY_LENGTH( argv1 ), argv1 ) != 0 ) return -1;
	if( test_optional_arg( (int)ARRAY_LENGTH( argv2 ), argv2 ) != 0 ) return -1;
	return 0;
}

static int test_parse_int(int argc, const char** argv, int expect)
{
	int default_value = -2;
	int found_int = -1;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, parse_option_list );
	ASSERT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'a': // required int
			case 'c': // optional int
				ASSERT_EQ(-1, found_int);

				found_int = ctx.current_opt_arg
								? ctx.current_value.i32 
								: default_value;

				if(opt == 'a') // required
					ASSERT_FALSE(ctx.current_opt_arg == 0x0);

				break;
			case '!':
				break;
			default:
				FAILm( "got an unexpected opt!" );
				break;
		}
	}

	ASSERT_EQ(found_int, expect);

	return 0;
}

TEST int_arg()
{
	#define EXPECT_SUCCESS(value, ...)                                      \
		{                                                                   \
			const char* argv[] = { __VA_ARGS__ };                           \
			int res = test_parse_int((int)ARRAY_LENGTH(argv), argv, value); \
			if(res < 0)                                                     \
				return res;                                                 \
		}

	#define EXPECT_FAIL(...) \
		EXPECT_SUCCESS(-1, __VA_ARGS__)

	// ... required int short ...
	EXPECT_SUCCESS(  1337, "dummy_prog",     "-a",   "1337"); // ... dec ...
	EXPECT_SUCCESS(0x1337, "dummy_prog",     "-a", "0x1337"); // ... hex ...
	EXPECT_SUCCESS( 01337, "dummy_prog",     "-a",  "01337"); // ... oct ...

	// ... optional int short ...
	EXPECT_SUCCESS(  1337, "dummy_prog",     "-c",   "1337"); // ... dec ...
	EXPECT_SUCCESS(0x1337, "dummy_prog",     "-c", "0x1337"); // ... hex ...
	EXPECT_SUCCESS( 01337, "dummy_prog",     "-c",  "01337"); // ... oct ...
	EXPECT_SUCCESS(    -2, "dummy_prog",     "-c");           // ... no value ...

	// ... required int long ...
	EXPECT_SUCCESS(  1337, "dummy_prog", "--ri32",   "1337"); // ... dec ...
	EXPECT_SUCCESS(0x1337, "dummy_prog", "--ri32", "0x1337"); // ... hex ...
	EXPECT_SUCCESS( 01337, "dummy_prog", "--ri32",  "01337"); // ... oct ...

	// ... optional int long ...
	EXPECT_SUCCESS(  1337, "dummy_prog", "--oi32",   "1337"); // ... dec ...
	EXPECT_SUCCESS(0x1337, "dummy_prog", "--oi32", "0x1337"); // ... hex ...
	EXPECT_SUCCESS( 01337, "dummy_prog", "--oi32",  "01337"); // ... oct ...
	EXPECT_SUCCESS(    -2, "dummy_prog", "--oi32");           // ... long, optional int ...

	EXPECT_FAIL( "dummy_prog", "-a" );                 // ... short, required int, missing arg ...
	EXPECT_FAIL( "dummy_prog", "-a", "poop" );         // ... short, required int, invalid arg ...
	EXPECT_FAIL( "dummy_prog", "-a", "1337poop" );     // ... short, required int, invalid arg that start of as valid ...
	EXPECT_FAIL( "dummy_prog", "--ri32" );             // ... long, required int, missing arg ...
	EXPECT_FAIL( "dummy_prog", "--ri32", "poop" );     // ... long, required int, invalid arg ...
	EXPECT_FAIL( "dummy_prog", "--ri32", "1337poop" ); // ... long, required int, invalid arg that start of as valid ...

	#undef EXPECT_FAIL
	#undef EXPECT_SUCCESS

	return 0;
}

static int test_parse_fp32(int argc, const char** argv, float expect)
{
	float default_value = -2.0f;
	float found_fp32 = -1.0f;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, parse_option_list );
	ASSERT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'b': // required fp
			case 'd': // optional fp
			 	ASSERT_EQ(-1.0f, found_fp32);

				found_fp32 = ctx.current_opt_arg 
								? ctx.current_value.fp32
								: default_value;

				if(opt == 'b') // required
					ASSERT_FALSE(ctx.current_opt_arg == 0x0);

				break;
			case '!':
				break;
			default:
				FAILm( "got an unexpected opt!" );
				break;
		}
	}

	ASSERT_EQ(found_fp32, expect);

	return 0;
}

TEST fp32_arg()
{
	#define EXPECT_SUCCESS(value, ...)                                       \
		{                                                                    \
			const char* argv[] = { __VA_ARGS__ };                            \
			int res = test_parse_fp32((int)ARRAY_LENGTH(argv), argv, value); \
			if(res < 0)                                                      \
				return res;                                                  \
		}

	#define EXPECT_FAIL(...) \
		EXPECT_SUCCESS(-1, __VA_ARGS__)

	// ... required fp32 short ...
	EXPECT_SUCCESS(1337.0f, "dummy_prog",     "-b",  "1337"); // ... dec ...
	EXPECT_SUCCESS( 13.37f, "dummy_prog",     "-b", "13.37"); // ... fp ...

	// ... optional fp32 short ...
	EXPECT_SUCCESS(1337.0f, "dummy_prog",     "-d",  "1337"); // ... dec ...
	EXPECT_SUCCESS( 13.37f, "dummy_prog",     "-d", "13.37"); // ... hex ...
	EXPECT_SUCCESS(  -2.0f, "dummy_prog",     "-d");          // ... no value ...

	// ... required fp32 long ...
	EXPECT_SUCCESS(1337.0f, "dummy_prog", "--rf32",  "1337"); // ... dec ...
	EXPECT_SUCCESS( 13.37f, "dummy_prog", "--rf32", "13.37"); // ... hex ...

	// ... optional fp32 long ...
	EXPECT_SUCCESS(1337.0f, "dummy_prog", "--of32",  "1337"); // ... dec ...
	EXPECT_SUCCESS( 13.37f, "dummy_prog", "--of32", "13.37"); // ... hex ...
	EXPECT_SUCCESS(  -2.0f, "dummy_prog", "--of32");          // ... long, optional int ...

	EXPECT_FAIL( "dummy_prog", "-b" );                 // ... short, required int, missing arg ...
	EXPECT_FAIL( "dummy_prog", "-b", "poop" );         // ... short, required int, invalid arg ...
	EXPECT_FAIL( "dummy_prog", "-b", "1337poop" );     // ... short, required int, invalid arg that start of as valid ...
	EXPECT_FAIL( "dummy_prog", "--rf32" );             // ... long, required int, missing arg ...
	EXPECT_FAIL( "dummy_prog", "--rf32", "poop" );     // ... long, required int, invalid arg ...
	EXPECT_FAIL( "dummy_prog", "--rf32", "1337poop" ); // ... long, required int, invalid arg that start of as valid ...

	#undef EXPECT_FAIL
	#undef EXPECT_SUCCESS

	return 0;
};

TEST non_arguments()
{
	const char* argv[] = { "dummy_prog", "-c", "arg1", "non_arg1", "--cccc=arg2", "non_arg2", "non_arg3" };
	int argc = (int)ARRAY_LENGTH( argv );

	const char* args[2]      = { "fill1", "fill2" };
	const char* non_args[3]  = { "fill3", "fill4", "fill5" };
	int args_count     = 0;
	int non_args_count = 0;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	ASSERT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'c': args[args_count++]         = ctx.current_opt_arg; break;
			case '+': non_args[non_args_count++] = ctx.current_opt_arg; break;
			default:
				FAILm( "got an unexpected opt!" );
				break;
		}
	}

	ASSERT_EQ(2, args_count);
	ASSERT_EQ(3, non_args_count);
	ASSERT_STR_EQ("arg1", args[0]);
	ASSERT_STR_EQ("arg2", args[1]);
	ASSERT_STR_EQ("non_arg1", non_args[0]);
	ASSERT_STR_EQ("non_arg2", non_args[1]);
	ASSERT_STR_EQ("non_arg3", non_args[2]);

	return 0;
}

TEST non_arguments_for_no_args_flags()
{
	// test that flag that should have no args pass 'non_args' correctly ...
	const char* argv[] = { "dummy_prog", "-a", "non_arg1", "--bbbb", "non_arg2" };
	int argc = (int)ARRAY_LENGTH( argv );

	const char* args[2]      = { "fill1", "fill2" };
	const char* non_args[2]  = { "fill3", "fill4" };
	int args_count     = 0;
	int non_args_count = 0;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	ASSERT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'a':
				args[args_count++] = "a";
				ASSERT_EQ(nullptr, ctx.current_opt_arg);
				break;
			case 'b':
				args[args_count++] = "b";
				ASSERT_EQ(nullptr, ctx.current_opt_arg);
				break;
			case '+':
				non_args[non_args_count++] = ctx.current_opt_arg;
				break;
			default:
				printf("opt == '%c'\n", opt);
				FAILm("got an unexpected opt!");
				break;
		}
	}

	ASSERT_EQ(2, args_count);
	ASSERT_EQ(2, non_args_count);
	ASSERT_STR_EQ("a", args[0]);
	ASSERT_STR_EQ("b", args[1]);
	ASSERT_STR_EQ("non_arg1", non_args[0]);
	ASSERT_STR_EQ("non_arg2", non_args[1]);

	return 0;
}

TEST set_flag()
{
	g_flag = -1;

	const char* argv[] = { "dummy_prog", "-g", "-f", "-g", "-f", "-e" }; // ( ( ( 0xFFFFFFFF | 1 ) & 1 ) | 1 ) & 1
	int argc = (int)ARRAY_LENGTH( argv );

	int flag_values[5] = { 0 };
	int num_flags = 0;

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, option_list );
	ASSERT_EQ( 0, err );

	int opt;
	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case '\0': flag_values[num_flags++] = g_flag; break;
			default:
				FAILm( "got an unexpected opt!" );
				break;
		}
	}

	ASSERT_EQ(5, num_flags);
	int expect0 = -1;
	int expect1 = expect0 | 1;
	int expect2 = expect1 & 1;
	int expect3 = expect2 | 1;
	int expect4 = expect3 & 1;
	ASSERT_EQ(expect1, flag_values[0] );
	ASSERT_EQ(expect2, flag_values[1] );
	ASSERT_EQ(expect3, flag_values[2] );
	ASSERT_EQ(expect4, flag_values[3] );
	ASSERT_EQ(1337,    flag_values[4] );
	return 0;
}

TEST same_prefix_long_opt()
{
	static const getopt_option_t bug_option_list[] = 
	{
		{ "input", 		   0x0, GETOPT_OPTION_TYPE_REQUIRED, 0x0, 'i', "help input", "input value" },
		{ "input-variant", 0x0, GETOPT_OPTION_TYPE_REQUIRED, 0x0, 'I', "help input-variant", "input variant" },
		GETOPT_OPTIONS_END
	};

	const char* argv[] = { "dummy_prog", "--input", "Input Value", "--input-variant", "Input Variant" };
	int argc = (int)ARRAY_LENGTH( argv );

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, bug_option_list );
	ASSERT_EQ( 0, err );

	int opt;

	int found_input = 0;
	int found_input_variant = 0;

	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'i': { found_input = 1;         ASSERT_STR_EQ("Input Value",   ctx.current_opt_arg); break; }
			case 'I': { found_input_variant = 1; ASSERT_STR_EQ("Input Variant", ctx.current_opt_arg); break; }
			default:
			    FAILm( "got an unexpected opt!" );
				break;
		}
	}

	ASSERT_EQ(1, found_input);
	ASSERT_EQ(1, found_input_variant);

	return 0;
}

TEST no_longopt_with_longopt()
{
	static const getopt_option_t bug_option_list[] = 
	{
		{ 0x0, 'a', GETOPT_OPTION_TYPE_NO_ARG, 0x0, 'a', "help input", "input value" },
		GETOPT_OPTIONS_END
	};

	const char* argv[] = { "dummy_prog", "--input" };
	int argc = (int)ARRAY_LENGTH( argv );

	getopt_context_t ctx;
	int err = getopt_create_context( &ctx, argc, argv, bug_option_list );
	ASSERT_EQ( 0, err );

	int opt;

	int found_input = 0;

	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case 'a': ++found_input; break;
			case '?': break;
			default:
				FAILm( "got an unexpected opt!" );
				break;
		}
	}

	ASSERT_EQ(0, found_input);

	return 0;
}

TEST capture_bad_longopt()
{
	const char* argv[] = { "dummy_prog", "--input" };
	int argc = (int)ARRAY_LENGTH( argv );

	{
		static const getopt_option_t bug_option_list[] = 
		{
			// args are not allowed to begin with one '-'
			{ "-bad", 0, GETOPT_OPTION_TYPE_NO_ARG, 0x0, 'a', "help input", "input value" },
			GETOPT_OPTIONS_END
		};
		
		getopt_context_t ctx;
		int err = getopt_create_context( &ctx, argc, argv, bug_option_list );
		ASSERT_EQ( -1, err );
	}

	{
		static const getopt_option_t bug_option_list[] = 
		{
			// args are not allowed to begin with two '-'
			{ "--bad", 0, GETOPT_OPTION_TYPE_NO_ARG, 0x0, 'a', "help input", "input value" },
			GETOPT_OPTIONS_END
		};
		
		getopt_context_t ctx;
		int err = getopt_create_context( &ctx, argc, argv, bug_option_list );
		ASSERT_EQ( -1, err );
	}

	// test for bad args in different positions.
	{
		static const getopt_option_t bug_option_list[] = 
		{
			{ "--bad", 0, GETOPT_OPTION_TYPE_NO_ARG, 0x0, 'a', "help input", "input value" },
			{ "i",     0, GETOPT_OPTION_TYPE_NO_ARG, 0x0, 'b', "help input", "input value" },
			GETOPT_OPTIONS_END
		};
		
		getopt_context_t ctx;
		int err = getopt_create_context( &ctx, argc, argv, bug_option_list );
		ASSERT_EQ( -1, err );
	}

	{
		static const getopt_option_t bug_option_list[] = 
		{
			{ "i",     0, GETOPT_OPTION_TYPE_NO_ARG, 0x0, 'b', "help input", "input value" },
			{ "--bad", 0, GETOPT_OPTION_TYPE_NO_ARG, 0x0, 'a', "help input", "input value" },
			GETOPT_OPTIONS_END
		};
		
		getopt_context_t ctx;
		int err = getopt_create_context( &ctx, argc, argv, bug_option_list );
		ASSERT_EQ( -1, err );
	}

	{
		static const getopt_option_t bug_option_list[] = 
		{
			{ "j",     0, GETOPT_OPTION_TYPE_NO_ARG, 0x0, 'c', "help input", "input value" },
			{ "--bad", 0, GETOPT_OPTION_TYPE_NO_ARG, 0x0, 'a', "help input", "input value" },
			{ "i",     0, GETOPT_OPTION_TYPE_NO_ARG, 0x0, 'b', "help input", "input value" },
			GETOPT_OPTIONS_END
		};
		
		getopt_context_t ctx;
		int err = getopt_create_context( &ctx, argc, argv, bug_option_list );
		ASSERT_EQ( -1, err );
	}

	return 0;
}

GREATEST_SUITE( getopt )
{
	RUN_TEST( short_opt );
	RUN_TEST( unknown_flags );
	RUN_TEST( with_args_short );
	RUN_TEST( with_args_long );
	RUN_TEST( with_args_short_after_long );
	RUN_TEST( missing_arg_short );
	RUN_TEST( optional_arg );
	RUN_TEST( int_arg );
	RUN_TEST( fp32_arg );
	RUN_TEST( non_arguments );
	RUN_TEST( non_arguments_for_no_args_flags );
	RUN_TEST( set_flag );
	RUN_TEST( with_zero_args );
	RUN_TEST( same_prefix_long_opt );
	RUN_TEST( no_longopt_with_longopt );
	RUN_TEST( capture_bad_longopt );
}

GREATEST_MAIN_DEFS();

int main( int argc, char **argv )
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE( getopt );
    GREATEST_MAIN_END();
}

