#include "DirectoryEnumeration.h"
#include <windows.h>
#include "concurrentqueue.h"

void DirectoryEnumeration::EnumerateDirectory(const std::wstring &root, std::vector<std::wstring> &outList)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	std::wstring rootWildCard = root + L"\\*";
	LPCWSTR sw = rootWildCard.c_str();

	hFind = FindFirstFileW(sw, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFile failed: %d\n", GetLastError());
		return;
	}

	do
	{
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (std::wcscmp(findFileData.cFileName, m_DotW.c_str()) == 0 || std::wcscmp(findFileData.cFileName, m_DotDotW.c_str()) == 0)
			{
				continue;
			}

			std::wstring combined = root + L"\\" + findFileData.cFileName;
			outList.push_back(combined);
			EnumerateDirectory(combined, outList);
		}
		else
		{
			std::wstring combined = root + L"\\" + findFileData.cFileName;
			outList.push_back(combined);
		}

	} while (FindNextFile(hFind, &findFileData) != 0);


	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("Error with FindFiles: %d\n", dwError);
	}

	FindClose(hFind);
}

void DirectoryEnumeration::EnumerateDirectoryA(const std::string& root, std::vector<std::string>& outList)
{
	const char* dot = ".";
	const char* dotdot = "..";
	WIN32_FIND_DATAA findFileData;
	HANDLE hFind;

	std::string rootWildCard = root + "\\*";
	LPCSTR sw = rootWildCard.c_str();

	hFind = FindFirstFileA(sw, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFile failed: %d\n", GetLastError());
		return;
	}

	do
	{
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!std::strcmp(findFileData.cFileName, dot) || !std::strcmp(findFileData.cFileName, dotdot))
				continue;

			std::string combined = root + "\\" + findFileData.cFileName;
			outList.emplace_back(combined);
			EnumerateDirectoryA(combined, outList);
		}
		else
			outList.emplace_back(root + "\\" + findFileData.cFileName);

	} 
	while (FindNextFileA(hFind, &findFileData));

	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("Error with FindFiles: %d\n", dwError);
	}

	FindClose(hFind);
}

void DirectoryEnumeration::EnumerateDirectoryParallel(const Args& config, const std::wstring& root, std::vector<std::wstring>& outList)
{
	moodycamel::ConcurrentQueue<std::wstring> queue;
	const int threadCount = config.threadCount;
	std::vector<std::thread> threads(threadCount);
	std::atomic<int> runningThreads = 0;
	moodycamel::ConcurrentQueue<std::wstring> outListQueue;
	
	queue.enqueue(root);
	outListQueue.enqueue(root);

	for (int i = 0; i != threadCount; ++i) 
	{
		threads[i] = std::thread([&]() 
		{
			do
			{
				std::wstring curPath;
				if (queue.try_dequeue(curPath))
				{
					runningThreads++;
					WIN32_FIND_DATA findFileData;
					HANDLE hFind;

					std::wstring rootWildCard = curPath + L"\\*";
					LPCWSTR sw = rootWildCard.c_str();

					hFind = FindFirstFileW(sw, &findFileData);

					if (hFind == INVALID_HANDLE_VALUE)
					{
						printf("FindFirstFile failed: %d\n", GetLastError());
						runningThreads--;
						break;
					}

					do
					{
						if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							if (std::wcscmp(findFileData.cFileName, m_DotW.c_str()) == 0 || std::wcscmp(findFileData.cFileName, m_DotDotW.c_str()) == 0)
							{
								continue;
							}

							std::wstring combined = curPath + L"\\" + findFileData.cFileName;
							outListQueue.enqueue(combined);
							queue.enqueue(combined);
						}
						else
						{
							std::wstring combined = curPath + L"\\" + findFileData.cFileName;
							outListQueue.enqueue(combined);

						}

					} while (FindNextFile(hFind, &findFileData));

					DWORD dwError = GetLastError();
					if (dwError != ERROR_NO_MORE_FILES)
					{
						printf("Error with FindFiles: %d\n", dwError);
					}

					FindClose(hFind);
					runningThreads--;
				}
			} while (runningThreads > 0 || queue.size_approx() > 0);
		});
	}

	// Wait for all threads
	for (int i = 0; i != threadCount; ++i) 
		threads[i].join();

	size_t size = outListQueue.size_approx();
	outList.reserve(size);

	for (int i = 0; i < size; ++i)
	{
		std::wstring entry;
		if(outListQueue.try_dequeue(entry))
			outList.push_back(std::move(entry));
	}
}

void DirectoryEnumeration::EnumerateDirectoryParallelA(const Args& config, const std::string& root, std::vector<std::string>& outList)
{
	const char* dot = ".";
	const char* dotdot = "..";

	moodycamel::ConcurrentQueue<std::string> queue;
	const int threadCount = config.threadCount;
	std::vector<std::thread> threads(threadCount);
	std::atomic<int> runningThreads = 0;
	moodycamel::ConcurrentQueue<std::string> outListQueue;

	queue.enqueue(root);
	outListQueue.enqueue(root);

	for (int i = 0; i != threadCount; ++i)
	{
		threads[i] = std::thread([&]()
			{
				do
				{
					std::string curPath;
					if (queue.try_dequeue(curPath))
					{
						runningThreads++;
						WIN32_FIND_DATAA findFileData;
						HANDLE hFind;

						std::string rootWildCard = curPath + "\\*";
						LPCSTR sw = rootWildCard.c_str();

						hFind = FindFirstFileA(sw, &findFileData);

						do
						{
							if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
							{
								if (!std::strcmp(findFileData.cFileName, dot) || !std::strcmp(findFileData.cFileName, dotdot))
									continue;

								std::string combined = curPath + "\\" + findFileData.cFileName;
								outListQueue.enqueue(combined);
								queue.enqueue(std::move(combined));
							}
							else
							{
								std::string combined = curPath + "\\" + findFileData.cFileName;
								outListQueue.enqueue(std::move(combined));
							}
						} while (FindNextFileA(hFind, &findFileData));

						DWORD dwError = GetLastError();
						if (dwError != ERROR_NO_MORE_FILES)
						{
							printf("Error with FindFiles: %d\n", dwError);
						}

						FindClose(hFind);
						runningThreads--;
					}
				} while (runningThreads > 0 || queue.size_approx() > 0);
			});
	}

	// Wait for all threads
	for (int i = 0; i != threadCount; ++i)
		threads[i].join();

	size_t size = outListQueue.size_approx();
	outList.reserve(size);

	for (int i = 0; i < size; ++i)
	{
		std::string entry;
		if (outListQueue.try_dequeue(entry))
			outList.push_back(std::move(entry));
	}
}