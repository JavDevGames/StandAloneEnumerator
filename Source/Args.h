#pragma once
#include <stdio.h>
#include <string>

enum EnumerationStrategy
{
	kEnumerate_None = 0,
	kEnumerate_WString = 1,
	kEnumerate_String = 2,
	kEnumerate_Parallel_WString = 3,
	kEnumerate_Parallel_String = 4,
};

struct Args
{
	std::string path;
	EnumerationStrategy strategy;
	int threadCount;
};
