#include "D3D11Framework.h"
#include "FileSystem.h"
#include <filesystem>
#include <shellapi.h>

//find()                : �־��� ���ڿ��� �����ϴ� ��ġ-> ������
//rfind()               : �־��� ���ڿ��� �����ϴ� ��ġ-> ������
//find_first_of()       : �־��� ���� �߿� �ϳ��� �ɸ��� ù��° ��ġ
//find_last_of()        : �־��� ���� �߿� �ϳ��� �ɸ��� ������ ��ġ
//find_first_not_of()   : �־��� ���ڰ� �ƴ� ���ڰ� �ɸ��� ù��° ��ġ
//find_last_not_of()    : �־��� ���ڰ� �ƴ� ���ڰ� �ɸ��� ������ ��ġ

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

	//������ ���� ��� ����
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

//Ȯ���ڸ� ������ ���� �̸��� ���ϴ� �Լ�
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

//���->����η� ��ȯ
auto FileSystem::GetRelativeFromPath(const std::string & absolute_path) -> const std::string
{
	//���� ��θ� ����
	//pathŬ������ ���� ��θ� �����ϱ� ���� Ŭ������.
	//absolute�Լ��� ���� ��η� �̸��� ��ȯ�ϴ� ������ ��
	//ex) C:\users\document~ �̷� ��η� �̸��� ��ȯ
	//�Լ� ���ڷ� ���� ��θ� ���� ��η� ����� p�� ����
	path p = absolute(absolute_path);
	//���� �������� ��θ� ���� ��η� ����� r�� ����
	path r = absolute(GetWorkingDirectory());

	//��Ʈ ��ΰ� �ٸ���� ������ ��ȯ
	//���� ���� ���� �����ο� ���ڷ� ���� �������� ��Ʈ ��ΰ� �ٸ� ���
	//���ڷ� ���� �����θ� string���� ��ȯ�Ͽ� ��ȯ
	//���� ���� ���� �����ο� ��ȯ�� ��ΰ� ���ʿ� �Ѹ����� �ٸ��� ����η� ��ȯ�� �Ұ����ϱ� ������ ���� �˻�
	if (p.root_path() != r.root_path())
		return p.generic_string();

	//��Ʈ ��ΰ� ���� ���
	//����� ���� ��ü
	path result;

	//�� ��ΰ� �������� ������ üũ
	path::const_iterator iter_path = p.begin();
	path::const_iterator iter_relative = r.begin();

	//p�� r�� ���丮 ���ڰ� ���� �� p�� r�� end()�� �ƴ� �� ���� iterator ��ȯ
	//���⼭ const_iterator�� ���� �ϳ��ϳ��� �ƴ� �ϳ��� ���丮 ���� ��ü�� �ǹ���.
	//��, /�� �������� /�ڿ� ���� ���丮�� ���� Ȥ�� ���ڿ��� �ּҸ� ������ ����.
	//ex) C:\Users\User\Desktop\D3D_D3D11Framework\Day16���� 
	//const_iterator�� +�ϸ� C, �ι� +�ϸ� Users, ���� +�ϸ� User�� ������ ����.
	while (
		*iter_path == *iter_relative &&
		iter_path != p.end() &&
		iter_relative != r.end())
	{
		iter_path++;
		iter_relative++;
	}

	//relative�� ���� �� ��ū������ ..�� �߰�
	//���⼭ ��� ��θ� 
	if (iter_relative != r.end())
	{
		iter_relative++;
		while (iter_relative != r.end())
		{
			//path Ŭ�������� /�����ڴ� ���丮 �����ڷ� �� ��θ� �����ϴ� ���� �ǹ�.
			//�� ��εڿ� /..�� ���δٴ� �ǹ�
			result /= "..";
			iter_relative++;
		}
	}

	//���� ��� �߰�
	while (iter_path != p.end())
	{
		//���ڷ� ���� ���� ����� �޺κ��� result�� ������.
		//���⼭ �޺κ��� ���� �������� �����ο� ��ġ�� �ʴ� �κ���
		result /= *iter_path;
		iter_path++;
	}

	//path->string�������� �ڷ����� ��ȯ�Ͽ� ��ȯ
	return result.generic_string();
}

//���ڷ� ���� ��η� �θ���丮�� ��θ� ���ϴ� �Լ�
auto FileSystem::GetParentDirectory(const std::string & path) -> const std::string
{
	auto found = path.find_last_of("\\/");
	auto result = path;

	//Ż������
	//found���� '\\' or '/'�� ���� ��� ���� �׳� ��ȯ
	if (found == std::string::npos)
		return path;

	//���� found���� '\\' or '/'�� ã�Ҵµ� �̰��� ���ڷ� ���� path�� ����-1��° �ε����� ���� ���
	//��ȯȣ���� �̿��Ͽ� ���� ���丮�� �߶� �ڱ��ڽ��� �Լ��� ȣ����.
	//ex) C:\Users\User\Desktop\D3D_D3D11Framework\ -> ���ڷ� ���� path�� ������ ���ڰ� \�� ��� 
	if (found == path.length() - 1)
	{
		//result�� ������ /�� ������ ������ ���ڸ� ����
		result = result.substr(0, found - 1);
		return GetParentDirectory(result);
	}

	//���� �� ���ǿ� �������� ���� ���
	//ex) C:\Users\User\Desktop\D3D_D3D11Framework -> ���ڷ� ���� path�� ������ ���ڰ� \�� �ƴ� ���
	//ex) C:\Users\User\Desktop\�� ��ȯ
	return result.substr(0, found) + "/";
}

auto FileSystem::GetWorkingDirectory() -> const std::string
{
	return current_path().generic_string() + "/"; //path->string ���� �ڷ��� ��ȯ
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
		//��ҹ��� ������
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
	//WideCharToMultiByte�� �����ڵ带 ��Ƽ����Ʈ �ڵ�� �ٲ���
	//���ÿ� ���ڿ��� ���� ���� ��ȯ
	//�Լ��� �߰����ڿ� -1�� ó������ ������ �����Ѵٴ� ���� �ǹ�
	len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

	//���ڿ��� ���̸�ŭ char������ �����Ҵ�
	char* buf = new char[len];

	//�����ڵ忡�� ��Ƽ����Ʈ �ڵ�� ��ȯ�� ���ڿ��� buf�� len�� ��ŭ ����
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, buf, len, NULL, NULL);

	//������ ���ڿ��� ������ �ִ� buf�� �����ͷ� string��ü ����
	std::string result(buf);

	//�ʿ䰡 ������ ���� ����
	delete[] buf;

	//string ��ü ��ȯ
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
