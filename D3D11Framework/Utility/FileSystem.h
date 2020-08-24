#pragma once

//������ �����ϱ� ���� Ŭ����(���� �� ����)
//�� Ŭ������ �Լ����� ���� Directory�� ��� ���ڿ� ������ ��ġ�� �־����
//�� Ŭ������ �Լ����� ���� File�� ��� ���ڿ� ������ Ȯ���ڱ��� �־����
class FileSystem final
{
public:
    static void OpenDirectoryWindow(const std::string& directory);
	static auto Create_Directory(const std::string& path)->const bool; //Ư�� ��ο� ������ ����
	static auto Delete_Directory(const std::string& path)->const bool; //Ư�� ����� ���� �Ǵ� ������ ����(���� ���� �����͸� ��� ����)
	static auto Delete_File(const std::string& path) -> const bool; //Ư�� ����� ������ ����
	static auto Copy_File(const std::string& src, const std::string& dst)->const bool; //������ ���� src(source)�� ������ dst(destination)�� ����
	static auto IsDirectory(const std::string& path) -> const bool; //Ư�� ��ΰ� �������� Ȯ��
	static auto IsExistDirectory(const std::string& path) -> const bool; //Ư�� ��ο� ������ �����ϴ��� Ȯ��
	static auto IsExistFile(const std::string& path) -> const bool; //Ư�� ������ ������ �����ϴ��� Ȯ��

	static auto GetFileNameFromPath(const std::string& path)->const std::string; //Ư�� ��ο� �ִ� ������ �̸��� ��ȯ(Ȯ���� ����)
	static auto GetIntactFileNameFromPath(const std::string& path)->const std::string; //Ư�� ��ο� �ִ� ������ �̸����� ��ȯ(Ȯ���� ������)
	static auto GetDirectoryFromPath(const std::string& path)-> const std::string; //Ư�� ��ο� �ִ� ������ �̸��� ��ȯ
	static auto GetExtensionFromPath(const std::string& path)-> const std::string; //Ư�� ��ο� �ִ� ������ Ȯ���ڸ� ��ȯ
	static auto GetPathWithoutExtension(const std::string& path)-> const std::string; //Ư�� ��ο��� Ư�� ������ Ȯ���ڸ� ������ ������ ��θ� ��ȯ
	static auto GetRelativeFromPath(const std::string& absolute_path)-> const std::string; //���->����η� ��ȯ
	static auto GetParentDirectory(const std::string& path)-> const std::string; //Ư�� ����� �θ�����(���� �ֻ�ܿ� �ִ� ����)�� ��θ� ����
	static auto GetWorkingDirectory()-> const std::string; //���� �������� ��θ� ��ȯ
	static auto GetFileDataNameFromPath(const std::string& path) -> const std::string; // Ư�� ��ο� �ִ� ������ �̸����� Ư¡���� �����ؼ� ��ȯ(�ִϸ��̼��� �����ϱ� ���� ���)

	static auto GetDirectoriesInDirectory(const std::string& directory) -> const std::vector<std::string>;
	static auto GetFilesInDirectory(const std::string& directory) -> const std::vector<std::string>;

	static auto GetSupportedTextureFormats() -> const std::vector<std::string>& { return supported_texture_formats; }
	static auto GetSupportedModelFormats() -> const std::vector<std::string>& { return supported_model_formats; }
	static auto GetSupportedScriptFormats() -> const std::vector<std::string>& { return supported_script_formats; }
	static auto GetSupportedAudioFormats() -> const std::vector<std::string>& { return supported_audio_formats; }

	//�̸� �����ߴ� �����Ǵ� Ȯ���ڸ� ���� ����� ������ Ȯ���ڿ� ���Ͽ� �����Ǵ��� ���θ� Ȯ��
	//Texture, Model, Animation, Script, Audio
	static auto IsSupportedTextureFile(const std::string& path) -> const bool;   //Texture
	static auto IsSupportedModelFile(const std::string& path) -> const bool;     //Model
	static auto IsSupportedScriptFile(const std::string& path) -> const bool;    //Script
	static auto IsSupportedAudioFile(const std::string& path) -> const bool;     //Audio

	static auto ToUppercase(const std::string& lower) -> const std::string; //�ҹ��� -> �빮��
	static auto ToLowercase(const std::string& upper) -> const std::string; //�빮�� -> �ҹ���
	static auto ToString(const std::wstring& wstr) -> const std::string; //�����ڵ� -> ��Ƽ����Ʈ
	static auto ToWString(const std::string& str) -> const std::wstring; //��Ƽ����Ʈ -> �����ڵ�
	static const char * ConvertStringToChar(const std::string& message); //String -> Char ��ȯ
	static const std::string ConvertCharToString(const char* message); //Char -> String ��ȯ

	static void ReplaceAll(std::string& in_out_str, const std::string& from, const std::string& to);
	static void ChangeFileName(const std::string& from_path, const std::string& to_path);

private:
    static std::vector<std::string> supported_texture_formats;		//Texture�� �����Ǵ� Ȯ���ڵ�
	static std::vector<std::string> supported_model_formats;		//Model�� �����Ǵ� Ȯ���ڵ�
	static std::vector<std::string> supported_script_formats;		//Script�� �����Ǵ� Ȯ���ڵ�
	static std::vector<std::string> supported_audio_formats;		//Audio�� �����Ǵ� Ȯ���ڵ�
};