// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2017 SUSE LINUX GmbH
 * Copyright (C) 2019 Jesse F. Williamson
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
*/


#include <sstream>

#include "local_random.hpp"

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// Helper to see if calls compile with various types:
template <typename T>
T type_check_ok(const T min, const T max)
{
 return local_random::generate_random_number(min, max);
}

/* Help wrangle "unused variable" warnings: */
template <typename ...XS>
void swallow_values(const XS& ...xs)
{
 (static_cast<void>(xs), ...);
}

// Mini-examples showing canonical usage:
TEST_CASE("canonical usage")
{
 // Seed random number generation:
 local_random::randomize_rng();

 // Get a random int between 0 and max int:
 auto a = local_random::generate_random_number();

 // Get a random int between 0 and 20:
 auto b = local_random::generate_random_number(20);

 // Get a random int between 1 and 20:
 auto c = local_random::generate_random_number(1, 20);

 // Get a random float between 0.0 and 20.0:
 auto d = local_random::generate_random_number(20.0);

 // Get a random float between 0.001 and 0.991:
 auto e = local_random::generate_random_number(0.001, 0.991);

 // Make a function object RNG suitable for putting on its own thread:
 auto gen_fn = local_random::random_number_generator<int>();
 auto z = gen_fn();
 gen_fn.seed(42);   // re-seed

 // Placate the compiler: 
 swallow_values(a, b, c, d, e, z);
}

TEST_CASE("check library")
{
 /* The intent of this test is not to formally test random number generation, 
 but rather to casually check that "it works" and catch regressions: */

 // The default overload should compile:
 local_random::randomize_rng();

 {
 int a = local_random::generate_random_number();
 int b = local_random::generate_random_number();

 /* Technically, this can still collide and cause a false negative, but let's 
 be optimistic: */
 if(std::numeric_limits<int>::max() > 32767) 
  {
	REQUIRE(a != b);
  }
 }

 SECTION("nullary version accepts different numeric types")
 {
 long def = local_random::generate_random_number();
 long l = local_random::generate_random_number<long>();
 int64_t i = local_random::generate_random_number<int64_t>();
 double d = local_random::generate_random_number<double>();

 swallow_values(def, l, i, d);
 }

 // (optimistically) 
 SECTION("nullary and unary versions never return < 0")
 {
 for(long i = 0; 1000000 != i; i++) 
  {
	REQUIRE(0 <= local_random::generate_random_number());
	REQUIRE(0 <= local_random::generate_random_number(1));
	REQUIRE(0 <= local_random::generate_random_number<float>(1.0));
  }
 }

 SECTION("nullary versions never return > numeric_limits<>::max()")
 {
 auto a = local_random::generate_random_number(1, std::numeric_limits<int>::max());
 auto b = local_random::generate_random_number(1, std::numeric_limits<int>::max());

 if(std::numeric_limits<int>::max() > 32767) 
  {
	REQUIRE(a > 0);
	REQUIRE(b > 0);

	// There is a chance of collision causing test failure here; it's unclear to me how
	// to actually test around that, without greatly complicating the test (perhaps there's
	// a minimum number of collisions acceptable over a certain statistical limit that can
	// be determined-- but I think it's kinda overkill, we really want to test our implementation,
	// not the actual random number library; this should not happen often at all):
	REQUIRE(a != b);
  }
 }

 SECTION("successive calls do not break")
 {
	 for(auto n = 100000; n; --n) 
	  {
		int a = local_random::generate_random_number(0, 6);
		REQUIRE(a > -1);
		REQUIRE(a < 7);
	  }
 }

 // Check bounding on zero (checking appropriate value for zero compiles and works):
 SECTION("check bounding on zero")
 {
	 for(auto n = 10; n; --n) 
	  {
		REQUIRE(0 == local_random::generate_random_number<int>(0, 0));
		REQUIRE(0 == local_random::generate_random_number<float>(0.0, 0.0));
	  }
 }

 SECTION("multiple integral types (int)")
 {
 int min = 0, max = 1;
 type_check_ok(min, max);
 }

 SECTION("multiple types (long)")
 {
 long min = 0, max = 1l;
 type_check_ok(min, max);
 }

 SECTION("multiple types (floating point, double)")
 {
 double min = 0.0, max = 1.0;
 type_check_ok(min, max);
 }

 SECTION("multiple types (floating point, float)")
 {
 float min = 0.0, max = 1.0;
 type_check_ok(min, max);
 }

 SECTION("min > max should not explode")
 {
 float min = 1.0, max = 0.0;
 type_check_ok(min, max);
 }

 SECTION("when combining types, everything should convert to the largest type")
 {
   SECTION("check with integral types")
   {
   int x = 0;
   long long y = 1;

   auto z = local_random::generate_random_number(x, y);

   bool result = std::is_same_v<decltype(z), decltype(y)>;

   REQUIRE(result);
   }

   SECTION("check with floating-point types")
   {
   float x = 0.0;
   long double y = 1.0;

   auto z = local_random::generate_random_number(x, y);

   bool result = std::is_same_v<decltype(z), decltype(y)>;

   REQUIRE(result);
   }

   // It would be nice to have a test to check that mixing integral and floating point
   // numbers should not compile, however we currently have no good way I know of
   // to do such negative tests.
 }
}

TEST_CASE("class interface")
{
 local_random::random_number_generator<int> rng_i;
 local_random::random_number_generator<float> rng_f;

 SECTION("other ctors")
 {
  local_random::random_number_generator<int> rng(1234);   // seed
 }

 SECTION("deduction guide")
 {
  local_random::random_number_generator rng0(10);		// deduce int
  local_random::random_number_generator rng1(10.0); 	// deduce float
 }

 SECTION("default call")
 {
 int a = rng_i();
 int b = rng_i();

 // Technically can fail, but should "almost never" happen:
 REQUIRE(a != b);
 }

 SECTION("call with int parameter")
 {
 int a = rng_i(10);
 REQUIRE(a <= 10);
 REQUIRE(a >= 0);
 }

 SECTION("call with float parameter")
 {
 float a = rng_f(10.0);
 REQUIRE(a <= 10.0);
 REQUIRE(a >= 0.0);
 }

 SECTION("call with min/max int parameters")
 {
 int a = rng_i(10, 20);
 REQUIRE(a <= 20);
 REQUIRE(a >= 10);
 }

 SECTION("call with min/max float parameters")
 {
 float a = rng_f(10.0, 20.0);
 REQUIRE(a <= 20.0);
 REQUIRE(a >= 10.0);
 }
}

