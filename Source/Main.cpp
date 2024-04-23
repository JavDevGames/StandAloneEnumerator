#include <stdio.h>
#include "DirectoryEnumeration.h"
#include "Args.h"

void ParseArgs(int argc, char** argv, Args &config)
{
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "--path") == 0)
		{
			config.path = argv[i + 1];
		}
		else if (strcmp(argv[i], "--strategy") == 0)
		{
			std::string strategy = argv[i + 1];
			if (strategy == "enumerate_wstring")
				config.strategy = kEnumerate_WString;
			else if (strategy == "enumerate_string")
				config.strategy = kEnumerate_String;
			if (strategy == "enumerate_parallel_wstring")
				config.strategy = kEnumerate_Parallel_WString;
			if (strategy == "enumerate_parallel_string")
				config.strategy = kEnumerate_Parallel_String;
		}
		else if (strcmp(argv[i], "--threads") == 0)
		{
			std::string threadCount = argv[i + 1];
			config.threadCount = std::stoi(threadCount);
		}
	}
}

int main(int argc, char** argv)
{
	DirectoryEnumeration enumerator;

	Args config;
	ParseArgs(argc, argv, config);

	std::vector<std::wstring> outListWString;
	std::vector<std::string> outList;

	switch (config.strategy)
	{
		case kEnumerate_WString:
		{
			std::wstring tempPath = std::wstring(config.path.begin(), config.path.end());
			enumerator.EnumerateDirectory(tempPath, outListWString);
			printf("Number of files: %zd\n", outListWString.size());
			break;
		}
		case kEnumerate_String:
		{
			enumerator.EnumerateDirectoryA(config.path, outList);
			printf("Number of files: %zd\n", outList.size());
			break;
		}
		case kEnumerate_Parallel_WString:
		{
			std::wstring tempPath = std::wstring(config.path.begin(), config.path.end());
			enumerator.EnumerateDirectoryParallel(config, tempPath, outListWString);
			printf("Number of files: %zd\n", outListWString.size());
			break;
		}
		case kEnumerate_Parallel_String:
		{
			enumerator.EnumerateDirectoryParallelA(config, config.path, outList);
			printf("Number of files: %zd\n", outList.size());
			break;
		}
	}

	
}