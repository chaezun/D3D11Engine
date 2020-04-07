#pragma once

enum FILE_STREAM_MODE :uint
{
	FILE_STREAM_READ=1U<<0,
	FILE_STREAM_WRITE = 1U << 1,
	FILE_STREAM_APPEND = 1U << 2,
};

//2진수로 바꿔주는 binary 클래스
class FileStream final
{
public:
	FileStream(const std::string& path,const uint& flags);
	~FileStream();

	auto IsOpen() const -> const bool {return is_open;}
	void Close();

	void Skip(const uint& n);

	//===============================================================
    // [Write]
    //===============================================================
	template <typename T, typename = typename std::enable_if<
		std::is_same<T, bool>::value ||
		std::is_same<T, char>::value ||
		std::is_same<T, byte>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, uint>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, unsigned long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value ||
		std::is_same<T, std::byte>::value ||
		std::is_same<T, Vector2>::value ||
		std::is_same<T, Vector3>::value ||
		std::is_same<T, Vector4>::value ||
		std::is_same<T, Color4>::value ||
		std::is_same<T, Quaternion>::value ||
		std::is_same<T, Matrix>::value>::type>
	void Write(const T& value)
	{
	   f_out.write(reinterpret_cast<const char*>(&value),sizeof(T));
	}

	void Write(const void* value, const uint& size);
	void Write(const std::string& value);
	void Write(const std::vector<std::byte>& value);

	//===============================================================
    // [Read]
    //===============================================================
	template <typename T, typename = typename std::enable_if<
		std::is_same<T, bool>::value ||
		std::is_same<T, char>::value ||
		std::is_same<T, byte>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, uint>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, unsigned long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value ||
		std::is_same<T, std::byte>::value ||
		std::is_same<T, Vector2>::value ||
		std::is_same<T, Vector3>::value ||
		std::is_same<T, Vector4>::value ||
		std::is_same<T, Color4>::value ||
		std::is_same<T, Quaternion>::value ||
		std::is_same<T, Matrix>::value>::type>
	void Read(T& value)
	{
		f_in.read(reinterpret_cast<char*>(&value), sizeof(T));
	}

	void Read(std::string& value);
	void Read(std::vector<std::byte>& value);

	template <typename T, typename = typename std::enable_if<
		std::is_same<T, bool>::value ||
		std::is_same<T, char>::value ||
		std::is_same<T, byte>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, uint>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, unsigned long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value ||
		std::is_same<T, std::byte>::value ||
		std::is_same<T, Vector2>::value ||
		std::is_same<T, Vector3>::value ||
		std::is_same<T, Vector4>::value ||
		std::is_same<T, Color4>::value ||
		std::is_same<T, Quaternion>::value ||
		std::is_same<T, Matrix>::value>::type>
	const T Read()
	{
	    T value;
		Read(value);

		return value;
	}

private:
    std::ofstream f_out;
	std::ifstream f_in;

	uint flags=0;
	bool is_open=false;
};