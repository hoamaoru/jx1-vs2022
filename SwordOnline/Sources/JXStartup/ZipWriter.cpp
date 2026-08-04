#include "StdAfx.h"
#include "ZipWriter.h"

namespace
{
	DWORD Crc32(const unsigned char* data, size_t len)
	{
		DWORD crc = 0xFFFFFFFFu;
		for (size_t i = 0; i < len; ++i)
		{
			crc ^= data[i];
			for (int bit = 0; bit < 8; ++bit)
				crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
		}
		return ~crc;
	}

	void GetDosDateTime(WORD& dosDate, WORD& dosTime)
	{
		SYSTEMTIME st;
		::GetLocalTime(&st);
		dosTime = (WORD)((st.wHour << 11) | (st.wMinute << 5) | (st.wSecond / 2));
		dosDate = (WORD)(((st.wYear - 1980) << 9) | (st.wMonth << 5) | st.wDay);
	}

	void AppendU16(std::string& buf, WORD v) { buf.append((const char*)&v, 2); }
	void AppendU32(std::string& buf, DWORD v) { buf.append((const char*)&v, 4); }
}

void CZipWriter::AddEntry(const std::string& name, const std::string& data)
{
	Entry e;
	e.name = name;
	e.data = data;
	e.crc = Crc32((const unsigned char*)data.data(), data.size());
	m_entries.push_back(std::move(e));
}

bool CZipWriter::Save(const std::string& path)
{
	HANDLE hFile = ::CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	WORD dosDate = 0, dosTime = 0;
	GetDosDateTime(dosDate, dosTime);

	DWORD offset = 0;
	DWORD written = 0;

	for (auto& e : m_entries)
	{
		e.localHeaderOffset = offset;

		std::string header;
		AppendU32(header, 0x04034b50);
		AppendU16(header, 20);
		AppendU16(header, 0);
		AppendU16(header, 0); // method 0 = stored
		AppendU16(header, dosTime);
		AppendU16(header, dosDate);
		AppendU32(header, e.crc);
		AppendU32(header, (DWORD)e.data.size());
		AppendU32(header, (DWORD)e.data.size());
		AppendU16(header, (WORD)e.name.size());
		AppendU16(header, 0);
		header += e.name;

		::WriteFile(hFile, header.data(), (DWORD)header.size(), &written, nullptr);
		if (!e.data.empty())
			::WriteFile(hFile, e.data.data(), (DWORD)e.data.size(), &written, nullptr);

		offset += (DWORD)header.size() + (DWORD)e.data.size();
	}

	DWORD centralDirStart = offset;

	for (auto& e : m_entries)
	{
		std::string cd;
		AppendU32(cd, 0x02014b50);
		AppendU16(cd, 20);
		AppendU16(cd, 20);
		AppendU16(cd, 0);
		AppendU16(cd, 0);
		AppendU16(cd, dosTime);
		AppendU16(cd, dosDate);
		AppendU32(cd, e.crc);
		AppendU32(cd, (DWORD)e.data.size());
		AppendU32(cd, (DWORD)e.data.size());
		AppendU16(cd, (WORD)e.name.size());
		AppendU16(cd, 0);
		AppendU16(cd, 0);
		AppendU16(cd, 0);
		AppendU16(cd, 0);
		AppendU32(cd, 0);
		AppendU32(cd, e.localHeaderOffset);
		cd += e.name;

		::WriteFile(hFile, cd.data(), (DWORD)cd.size(), &written, nullptr);
		offset += (DWORD)cd.size();
	}

	DWORD centralDirSize = offset - centralDirStart;

	std::string eocd;
	AppendU32(eocd, 0x06054b50);
	AppendU16(eocd, 0);
	AppendU16(eocd, 0);
	AppendU16(eocd, (WORD)m_entries.size());
	AppendU16(eocd, (WORD)m_entries.size());
	AppendU32(eocd, centralDirSize);
	AppendU32(eocd, centralDirStart);
	AppendU16(eocd, 0);

	::WriteFile(hFile, eocd.data(), (DWORD)eocd.size(), &written, nullptr);

	::CloseHandle(hFile);
	return true;
}
