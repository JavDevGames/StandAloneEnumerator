#pragma once
#include <vector>
#include <string>
#include "Args.h"

class DirectoryEnumeration
{
public:
	DirectoryEnumeration() : m_DotW(L"."), m_DotDotW(L"..") {};

	void EnumerateDirectoryA(const std::string &root, std::vector<std::string>& outList);
	void EnumerateDirectory(const std::wstring &root, std::vector<std::wstring>& outList);
	void EnumerateDirectoryParallel(const Args &config, const std::wstring& root, std::vector<std::wstring>& outList);
	void EnumerateDirectoryParallelA(const Args& config, const std::string& root, std::vector<std::string>& outList);

private:
	const std::wstring m_DotW;
	const std::wstring m_DotDotW;
};

