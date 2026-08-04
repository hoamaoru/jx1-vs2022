#pragma once

#include "StdAfx.h"

// Minimal ZIP writer (STORED entries only, no compression). No third-party
// dependency: log files are small enough that skipping DEFLATE is fine, and
// the output is still a fully standard .zip openable by any tool.
class CZipWriter
{
public:
	void AddEntry(const std::string& name, const std::string& data);
	bool Save(const std::string& path);

private:
	struct Entry
	{
		std::string name;
		std::string data;
		DWORD crc = 0;
		DWORD localHeaderOffset = 0;
	};

	std::vector<Entry> m_entries;
};
