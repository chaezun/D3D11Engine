#pragma once

class FileSystem final
{
public:
    static void OpenDirectoryWindow(const std::string& directory);
    static auto Create_Directory(const std::string& path)->const bool;
	static auto Delete_Directory(const std::string& path)->const bool;
    static auto Delete_File(const std::string& path) -> const bool;
	static auto Copy_File(const std::string& src, const std::string& dst)->const bool;
	static auto IsDirectory(const std::string& path) -> const bool;
	static auto IsExistDirectory(const std::string& path) -> const bool;
	static auto IsExistFile(const std::string& path) -> const bool;

	static auto GetFileNameFromPath(const std::string& path)->const std::string;
	static auto GetIntactFileNameFromPath(const std::string& path)->const std::string;
	static auto GetDirectoryFromPath(const std::string& path)-> const std::string;
	static auto GetExtensionFromPath(const std::string& path)-> const std::string;
	static auto GetPathWithoutExtension(const std::string& path)-> const std::string;
	static auto GetRelativeFromPath(const std::string& absolute_path)-> const std::string;
	static auto GetParentDirectory(const std::string& path)-> const std::string;
	static auto GetWorkingDirectory()-> const std::string;

	static auto GetDirectoriesInDirectory(const std::string& directory) -> const std::vector<std::string>;
	static auto GetFilesInDirectory(const std::string& directory) -> const std::vector<std::string>;

	static auto GetSupportedTextureFormats() -> const std::vector<std::string>& { return supported_texture_formats; }
	static auto GetSupportedModelFormats() -> const std::vector<std::string>& { return supported_model_formats; }
	static auto GetSupportedScriptFormats() -> const std::vector<std::string>& { return supported_script_formats; }

	static auto IsSupportedTextureFile(const std::string& path) -> const bool;
	static auto IsSupportedModelFile(const std::string& path) -> const bool;
	static auto IsSupportedScriptFile(const std::string& path) -> const bool;

    static auto ToUppercase(const std::string& lower) -> const std::string;
    static auto ToLowercase(const std::string& upper) -> const std::string;
    static auto ToString(const std::wstring& wstr) -> const std::string;
    static auto ToWString(const std::string& str) -> const std::wstring;

	static void ReplaceAll(std::string& in_out_str, const std::string& from, const std::string& to);
	//°úÁ¦
	static void ChangeFileName(const std::string& from_path, const std::string& to_path);

private:
    static std::vector<std::string> supported_texture_formats;
	static std::vector<std::string> supported_model_formats;
	static std::vector<std::string> supported_script_formats;
};