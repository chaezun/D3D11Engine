#include "Framework.h"
#include "FileStream.h"

FileStream::FileStream(const std::string & path, const uint & flags)
{
	this->is_open = false;
	this->flags = flags;

	int ios_flags = std::ios::binary;
	ios_flags |= (this->flags& FILE_STREAM_WRITE) ? std::ios::out : 0U;
	ios_flags |= (this->flags& FILE_STREAM_READ) ? std::ios::in : 0U;
	ios_flags |= (this->flags& FILE_STREAM_APPEND) ? std::ios::app : 0U;

	if (this->flags& FILE_STREAM_WRITE)
	{
	     f_out.open(path,ios_flags);
		 if (f_out.fail())
		 {
		    LOG_ERROR_F("Failed to open \"%s\" for writing", path.c_str());
			return ;
		 }
	}
	else if (this->flags& FILE_STREAM_READ)
	{
		f_in.open(path, ios_flags);
		if (f_in.fail())
		{
			LOG_ERROR_F("Failed to open \"%s\" for reading", path.c_str());
			return;
		}
	}

	is_open=true;
}

FileStream::~FileStream()
{
   Close();
}

void FileStream::Close()
{
	if (this->flags& FILE_STREAM_WRITE)
	{
		f_out.flush();
		f_out.close();
	}
	else if (this->flags& FILE_STREAM_READ)
	{
		f_in.clear();
		f_in.close();
	}
}

void FileStream::Skip(const uint & n)
{
   if(this->flags& FILE_STREAM_WRITE)
      //특정 위치를 찾음(현재 위치에서 n번째 까지)
      f_out.seekp(n,std::ios::cur);

   else if (this->flags& FILE_STREAM_READ)
	  f_in.ignore(n, std::ios::cur);
}

void FileStream::Write(const void * value, const uint & size)
{
   f_out.write(reinterpret_cast<const char*>(value),size);
}

void FileStream::Write(const std::string & value)
{
   const auto length=static_cast<uint>(value.length());

   Write(length);

   f_out.write(value.c_str(),length);
}

void FileStream::Write(const std::vector<std::byte>& value)
{
	const auto length = static_cast<uint>(value.size());

	Write(length);

	f_out.write(reinterpret_cast<const char*>(value.data()), length);
}

void FileStream::Read(std::string& value)
{
   uint length=Read<uint>();

   //배열 초기화
   value.clear();
   //capacity초기화
   value.shrink_to_fit();

   value.reserve(length);
   value.resize(length);
   
   f_in.read(value.data(), length);
}

void FileStream::Read(std::vector<std::byte>& value)
{
	uint length = Read<uint>();

	//배열 초기화
	value.clear();
	//capacity초기화
	value.shrink_to_fit();

	value.reserve(length);
	value.resize(length);

	f_in.read(reinterpret_cast<char*>(value.data()), length);
}
