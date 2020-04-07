#include "D3D11Framework.h"
#include "FileSystem.h"
#include <filesystem>
#include <shellapi.h>

//find()                : 주어진 문자열이 존재하는 위치-> 정방향
//rfind()               : 주어진 문자열이 존재하는 위치-> 역방향
//find_first_of()       : 주어진 문자 중에 하나라도 걸리는 첫번째 위치
//find_last_of()        : 주어진 문자 중에 하나라도 걸리는 마지막 위치
//find_first_not_of()   : 주어진 문자가 아닌 문자가 걸리는 첫번째 위치
//find_last_not_of()    : 주어진 문자가 아닌 문자가 걸리는 마지막 위치

using namespace std::filesystem;

std::vector<std::string> FileSystem::supported_texture_formats
{
	".jpg",
	".png",
	".bmp",
	".tga",
	".dds",
	".exr",
	".raw",
	".gif",
	".hdr",
	".ico",
	".iff",
	".jng",
	".jpeg",
	".koala",
	".kodak",
	".mng",
	".pcx",
	".pbm",
	".pgm",
	".ppm",
	".pfm",
	".pict",
	".psd",
	".raw",
	".sgi",
	".targa",
	".tiff",
	".tif",
	".wbmp",
	".webp",
	".xbm",
	".xpm"
};

std::vector<std::string> FileSystem::supported_model_formats
{
	".3ds",
	".obj",
	".fbx",
	".blend",
	".dae",
	".lwo",
	".c4d",
	".ase",
	".dxf",
	".hmp",
	".md2",
	".md3",
	".md5",
	".mdc",
	".mdl",
	".nff",
	".ply",
	".stl",
	".x",
	".smd",
	".lxo",
	".lws",
	".ter",
	".ac3d",
	".ms3d",
	".cob",
	".q3bsp",
	".xgl",
	".csm",
	".bvh",
	".b3d",
	".ndo"
};

std::vector<std::string> FileSystem::supported_script_formats
{
	".as",
};

void FileSystem::OpenDirectoryWindow(const std::string & directory)
{
	ShellExecuteA(nullptr, nullptr, nullptr, nullptr, directory.c_str(), SW_SHOW);
}

auto FileSystem::Create_Directory(const std::string & path) -> const bool
{
	try
	{
		return create_directories(path);
	}

	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s,%s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::Delete_Directory(const std::string & path) -> const bool
{
	try
	{
		return remove_all(path) > 0;
	}

	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s,%s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::Delete_File(const std::string & path) -> const bool
{
	if (is_directory(path))
		return false;

	try
	{
		return remove(path);
	}
	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::Copy_File(const std::string & src, const std::string & dst) -> const bool
{
	if (src == dst)
		return false;

	//폴더가 없는 경우 생성
	if (!IsExistDirectory(GetDirectoryFromPath(dst)))
		Create_Directory(GetDirectoryFromPath(dst));

	try
	{
		return copy_file(src, dst, copy_options::overwrite_existing);
	}

	catch (filesystem_error& error)
	{
		LOG_ERROR("Could not copy\"" + src + "\", " + std::string(error.what()));
		return false;
	}
}

auto FileSystem::IsDirectory(const std::string & path) -> const bool
{
	try
	{
		return is_directory(path);
	}

	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s,%s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::IsExistDirectory(const std::string & path) -> const bool
{
	try
	{
		return exists(path);
	}

	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s,%s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::IsExistFile(const std::string & path) -> const bool
{
	try
	{
		return exists(path);
	}

	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s,%s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::GetFileNameFromPath(const std::string & path) -> const std::string
{
	auto last_index = path.find_last_of("\\/");
	auto file_name = path.substr(last_index + 1, path.length());

	return file_name;
}

//확장자를 제외한 파일 이름을 구하는 함수
auto FileSystem::GetIntactFileNameFromPath(const std::string & path) -> const std::string
{
	auto file_name = GetFileNameFromPath(path);
	auto last_index = file_name.find_last_of('.');
	auto intact_file_name = file_name.substr(0, last_index);

	return intact_file_name;
}

auto FileSystem::GetDirectoryFromPath(const std::string & path) -> const std::string
{
	auto last_index = path.find_last_of("\\/");
	auto directory = path.substr(0, last_index + 1);

	return directory;
}

auto FileSystem::GetExtensionFromPath(const std::string & path) -> const std::string
{
	if (path.empty() || path == NOT_ASSIGNED_STR)
		return NOT_ASSIGNED_STR;

	auto last_index = path.find_last_of('.');

	if (last_index != std::string::npos)
		return path.substr(last_index, path.length());

	return NOT_ASSIGNED_STR;
}

auto FileSystem::GetPathWithoutExtension(const std::string & path) -> const std::string
{
	auto directory = GetDirectoryFromPath(path);
	auto intact_file_name = GetIntactFileNameFromPath(path);

	return directory + intact_file_name;
}

//경로->상대경로로 변환
auto FileSystem::GetRelativeFromPath(const std::string & absolute_path) -> const std::string
{
	//절대 경로를 만듬
	//path클래스는 파일 경로를 저장하기 위한 클래스임.
	//absolute함수는 절대 경로로 이름을 반환하는 역할을 함
	//ex) C:\users\document~ 이런 경로로 이름을 반환
	//함수 인자로 받은 경로를 절대 경로로 만들어 p에 저장
	path p = absolute(absolute_path);
	//현재 실행중인 경로를 절대 경로로 만들어 r에 저장
	path r = absolute(GetWorkingDirectory());

	//루트 경로가 다를경우 절대경로 반환
	//현재 실행 중인 절대경로와 인자로 받은 절대경로의 루트 경로가 다른 경우
	//인자로 받은 절대경로를 string으로 변환하여 반환
	//현재 실행 중인 절대경로와 변환할 경로가 애초에 뿌리부터 다르면 상대경로로 변환이 불가능하기 때문에 먼저 검사
	if (p.root_path() != r.root_path())
		return p.generic_string();

	//루트 경로가 같은 경우
	//결과를 담을 객체
	path result;

	//두 경로가 갈라지는 지점을 체크
	path::const_iterator iter_path = p.begin();
	path::const_iterator iter_relative = r.begin();

	//p와 r의 디렉토리 글자가 같고 각 p와 r의 end()가 아닐 때 까지 iterator 순환
	//여기서 const_iterator는 글자 하나하나가 아닌 하나의 디렉토리 글자 전체를 의미함.
	//즉, /를 기준으로 /뒤에 오는 디렉토리의 문자 혹은 문자열의 주소를 가지고 있음.
	//ex) C:\Users\User\Desktop\D3D_D3D11Framework\Day16에서 
	//const_iterator를 +하면 C, 두번 +하면 Users, 세번 +하면 User를 가지고 있음.
	while (
		*iter_path == *iter_relative &&
		iter_path != p.end() &&
		iter_relative != r.end())
	{
		iter_path++;
		iter_relative++;
	}

	//relative에 남은 각 토큰에대해 ..을 추가
	//여기서 상대 경로를 
	if (iter_relative != r.end())
	{
		iter_relative++;
		while (iter_relative != r.end())
		{
			//path 클래스에서 /연산자는 디렉토리 구분자로 두 경로를 연결하는 것을 의미.
			//즉 경로뒤에 /..을 붙인다는 의미
			result /= "..";
			iter_relative++;
		}
	}

	//남은 경로 추가
	while (iter_path != p.end())
	{
		//인자로 받은 절대 경로의 뒷부분을 result에 연결함.
		//여기서 뒷부분은 현재 실행중인 절대경로와 겹치지 않는 부분임
		result /= *iter_path;
		iter_path++;
	}

	//path->string형식으로 자료형을 변환하여 반환
	return result.generic_string();
}

//인자로 받은 경로로 부모디렉토리의 경로를 구하는 함수
auto FileSystem::GetParentDirectory(const std::string & path) -> const std::string
{
	auto found = path.find_last_of("\\/");
	auto result = path;

	//탈출조건
	//found에서 '\\' or '/'이 없는 경우 값을 그냥 반환
	if (found == std::string::npos)
		return path;

	//만약 found에서 '\\' or '/'를 찾았는데 이것이 인자로 받은 path의 길이-1번째 인덱스와 같은 경우
	//순환호출을 이용하여 하위 디렉토리를 잘라서 자기자신의 함수를 호출함.
	//ex) C:\Users\User\Desktop\D3D_D3D11Framework\ -> 인자로 받은 path의 마지막 문자가 \일 경우 
	if (found == path.length() - 1)
	{
		//result에 마지막 /을 제외한 나머지 문자를 저장
		result = result.substr(0, found - 1);
		return GetParentDirectory(result);
	}

	//위의 두 조건에 충족하지 않은 경우
	//ex) C:\Users\User\Desktop\D3D_D3D11Framework -> 인자로 받은 path의 마지막 문자가 \가 아닌 경우
	//ex) C:\Users\User\Desktop\을 반환
	return result.substr(0, found) + "/";
}

auto FileSystem::GetWorkingDirectory() -> const std::string
{
	return current_path().generic_string() + "/"; //path->string 으로 자료형 변환
}

auto FileSystem::GetDirectoriesInDirectory(const std::string & directory) -> const std::vector<std::string>
{
	std::vector<std::string> directories;

	directory_iterator end_iter;
	for (directory_iterator iter(directory); iter != end_iter; iter++)
	{
		if (!is_directory(iter->status()))
			continue;

		directories.emplace_back(iter->path().generic_string());
	}

	return directories;
}

auto FileSystem::GetFilesInDirectory(const std::string & directory) -> const std::vector<std::string>
{
	std::vector<std::string> files;

	directory_iterator end_iter;
	for (directory_iterator iter(directory); iter != end_iter; iter++)
	{
		if (!is_regular_file(iter->status()))
			continue;

		files.emplace_back(iter->path().generic_string());
	}

	return files;
}

auto FileSystem::IsSupportedTextureFile(const std::string & path) -> const bool
{

	auto file_extension = GetExtensionFromPath(path);
	auto supported_formats = GetSupportedTextureFormats();

	for (const auto& format : supported_formats)
	{
		//대소문자 비교포함
		if (file_extension == format || file_extension == ToUppercase(format))
			return true;
	}

	return false;
}

auto FileSystem::IsSupportedModelFile(const std::string & path) -> const bool
{
	auto file_extension = GetExtensionFromPath(path);
	auto supported_formats = GetSupportedModelFormats();

	for (const auto& format : supported_formats)
	{
		if (file_extension == format || file_extension == ToUppercase(format))
			return true;
	}

	return false;
}

auto FileSystem::IsSupportedScriptFile(const std::string & path) -> const bool
{
	auto file_extension = GetExtensionFromPath(path);
	auto supported_formats = GetSupportedScriptFormats();

	for (const auto& format : supported_formats)
	{
		if (file_extension == format || file_extension == ToUppercase(format))
			return true;
	}

	return false;
}

auto FileSystem::ToUppercase(const std::string & lower) -> const std::string
{
	std::string upper;

	for (const auto& character : lower)
		upper += toupper(character);

	return upper;
}

auto FileSystem::ToLowercase(const std::string & upper) -> const std::string
{
	std::string lower;

	for (const auto& character : upper)
		lower += tolower(character);

	return lower;
}

auto FileSystem::ToString(const std::wstring & wstr) -> const std::string
{
	int len;
	//WideCharToMultiByte은 유니코드를 멀티바이트 코드로 바꿔줌
	//동시에 문자열의 길이 값을 반환
	//함수의 중간인자에 -1은 처음부터 끝까지 복사한다는 것을 의미
	len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

	//문자열의 길이만큼 char형으로 동적할당
	char* buf = new char[len];

	//유니코드에서 멀티바이트 코드로 변환한 문자열을 buf에 len값 만큼 복사
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, buf, len, NULL, NULL);

	//복사한 문자열을 가지고 있는 buf의 데이터로 string객체 생성
	std::string result(buf);

	//필요가 없어진 버퍼 해제
	delete[] buf;

	//string 객체 반환
	return result;
}

auto FileSystem::ToWString(const std::string & str) -> const std::wstring
{
	std::wstring result;
	result.assign(str.begin(), str.end());

	return result;
}

void FileSystem::ReplaceAll(std::string & in_out_str, const std::string & from, const std::string & to)
{
	size_t start_pos = 0;

	while ((start_pos = in_out_str.find(from, start_pos)) != std::string::npos)
	{
		in_out_str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

void FileSystem::ChangeFileName(const std::string & from_path, const std::string & to_path)
{
	if (!IsExistDirectory(from_path))
		return;

	rename(from_path.c_str(), to_path.c_str());
}
