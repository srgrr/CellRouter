#pragma once
#include <random>
#include <unordered_set>
#include <cassert>
#include <iostream>

/*
	Return a reference to a fixed mersenne twister.
	Will be used as rng for all distributions
*/
std::mt19937& mtgen();

/*
  Return a random integer in [l, h]
*/
int random_integer(int l, int h);

/*
  Return a random permutation of order N
*/
std::vector< int > random_permutation(int n);

/*
  Returns the sign of a number
*/
int sgn(int x);
