#pragma once

//파일을 관리하기 위한 클래스(생성 및 삭제)
//이 클래스의 함수명의 끝이 Directory인 경우 인자에 폴더의 위치만 넣어야함
//이 클래스의 함수명의 끝이 File인 경우 인자에 파일의 확장자까지 넣어야함
class FileSystem final
{
public:
    static void OpenDirectoryWindow(const std::string& directory);
	static auto Create_Directory(const std::string& path)->const bool; //특정 경로에 폴더를 생성
	static auto Delete_Directory(const std::string& path)->const bool; //특정 경로의 폴더 또는 파일을 삭제(폴더 내의 데이터를 모두 삭제)
	static auto Delete_File(const std::string& path) -> const bool; //특정 경로의 파일을 삭제
	static auto Copy_File(const std::string& src, const std::string& dst)->const bool; //파일을 복사 src(source)의 내용을 dst(destination)에 복사
	static auto IsDirectory(const std::string& path) -> const bool; //특정 경로가 폴더인지 확인
	static auto IsExistDirectory(const std::string& path) -> const bool; //특정 경로에 폴더가 존재하는지 확인
	static auto IsExistFile(const std::string& path) -> const bool; //특정 폴더에 파일이 존재하는지 확인

	static auto GetFileNameFromPath(const std::string& path)->const std::string; //특정 경로에 있는 파일의 이름을 반환(확장자 포함)
	static auto GetIntactFileNameFromPath(const std::string& path)->const std::string; //특정 경로에 있는 파일의 이름만을 반환(확장자 미포함)
	static auto GetDirectoryFromPath(const std::string& path)-> const std::string; //특정 경로에 있는 폴더의 이름을 반환
	static auto GetExtensionFromPath(const std::string& path)-> const std::string; //특정 경로에 있는 파일의 확장자를 반환
	static auto GetPathWithoutExtension(const std::string& path)-> const std::string; //특정 경로에서 특정 파일의 확장자만 제외한 나머지 경로를 반환
	static auto GetRelativeFromPath(const std::string& absolute_path)-> const std::string; //경로->상대경로로 변환
	static auto GetParentDirectory(const std::string& path)-> const std::string; //특정 경로의 부모폴더(가장 최상단에 있는 폴더)의 경로를 구함
	static auto GetWorkingDirectory()-> const std::string; //현재 실행중인 경로를 반환
	static auto GetFileDataNameFromPath(const std::string& path) -> const std::string; // 특정 경로에 있는 파일의 이름에서 특징만을 추출해서 반환(애니메이션을 구분하기 위해 사용)

	static auto GetDirectoriesInDirectory(const std::string& directory) -> const std::vector<std::string>;
	static auto GetFilesInDirectory(const std::string& directory) -> const std::vector<std::string>;

	static auto GetSupportedTextureFormats() -> const std::vector<std::string>& { return supported_texture_formats; }
	static auto GetSupportedModelFormats() -> const std::vector<std::string>& { return supported_model_formats; }
	static auto GetSupportedScriptFormats() -> const std::vector<std::string>& { return supported_script_formats; }
	static auto GetSupportedAudioFormats() -> const std::vector<std::string>& { return supported_audio_formats; }

	//미리 정의했던 지원되는 확장자를 현재 경로의 파일의 확장자와 비교하여 지원되는지 여부를 확인
	//Texture, Model, Animation, Script, Audio
	static auto IsSupportedTextureFile(const std::string& path) -> const bool;   //Texture
	static auto IsSupportedModelFile(const std::string& path) -> const bool;     //Model
	static auto IsSupportedScriptFile(const std::string& path) -> const bool;    //Script
	static auto IsSupportedAudioFile(const std::string& path) -> const bool;     //Audio

	static auto ToUppercase(const std::string& lower) -> const std::string; //소문자 -> 대문자
	static auto ToLowercase(const std::string& upper) -> const std::string; //대문자 -> 소문자
	static auto ToString(const std::wstring& wstr) -> const std::string; //유니코드 -> 멀티바이트
	static auto ToWString(const std::string& str) -> const std::wstring; //멀티바이트 -> 유니코드
	static const char * ConvertStringToChar(const std::string& message); //String -> Char 변환
	static const std::string ConvertCharToString(const char* message); //Char -> String 변환

	static void ReplaceAll(std::string& in_out_str, const std::string& from, const std::string& to);
	static void ChangeFileName(const std::string& from_path, const std::string& to_path);

private:
    static std::vector<std::string> supported_texture_formats;		//Texture에 지원되는 확장자들
	static std::vector<std::string> supported_model_formats;		//Model에 지원되는 확장자들
	static std::vector<std::string> supported_script_formats;		//Script에 지원되는 확장자들
	static std::vector<std::string> supported_audio_formats;		//Audio에 지원되는 확장자들
};