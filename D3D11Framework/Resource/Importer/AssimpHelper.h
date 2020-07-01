#pragma once

//속성 -> c/c++ -> 경고수준 
//경고수준을 잠깐 0으로 설정
#pragma warning(push,0)
//Assimp(Open Asset Import Library) Library
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ProgressHandler.hpp>
//경고수준을 해제
#pragma warning (pop)

#include "Resource/ProgressReport.h"

class AssimpHelper final
{
public:
   static auto ToVector2(const aiVector2D& value) -> const Vector2;
   static auto ToVector2(const aiVector3D& value) -> const Vector2;
   static auto ToVector3(const aiVector3D& value) -> const Vector3;
   static auto ToColor(const aiColor4D& value) -> const Color4;
   static auto ToQuaternion(const aiQuaternion& value) -> const Quaternion;
   static auto ToMatrix(const aiMatrix4x4& value) -> const Matrix;

   static void ComputeNodeCount(const aiNode* node, int* count);
   static void ComputeActorTransform(const aiNode* node, class Actor* actor);

   static auto Try_Multi_Extension(const std::string& path) -> const std::string;
   static auto ValidatePath(const std::string& original_path, const std::string& model_path) -> const std::string;
};

class AssimpLogger final : public Assimp::Logger
{
public:
	 bool attachStream(Assimp::LogStream * pStream, unsigned int severity = Debugging | Err | Warn | Info) override {return true;}
	 bool detatchStream(Assimp::LogStream * pStream, unsigned int severity = Debugging | Err | Warn | Info) override { return true;}

private:
	 void OnDebug(const char * message) override
	 {
#ifdef _DEBUG
      Log::caller_name="ModelImporter::";
	  Log::Write(message,LogType::Info);
#endif
	 }

	 void OnInfo(const char * message) override
	 {
		 Log::caller_name = "ModelImporter::";
		 Log::Write(message, LogType::Info);
	 }

	 void OnWarn(const char * message) override
	 {
		 Log::caller_name = "ModelImporter::";
		 Log::Write(message, LogType::Warning);
	 }

	 void OnError(const char * message) override
	 {
		 Log::caller_name = "ModelImporter::";
		 Log::Write(message, LogType::Error);
	 }
};

class AssimpProgress final : public Assimp::ProgressHandler
{
public:
	AssimpProgress(const std::string& path)
	   :path(path),
	    name(FileSystem::GetIntactFileNameFromPath(path))
	{
	   auto& progress = ProgressReport::Get();
	   progress.Reset(ProgressType::Model);
	   progress.SetLoading(ProgressType::Model, true);
	}

	~AssimpProgress()
	{
		ProgressReport::Get().SetLoading(ProgressType::Model, false);
	}

	bool Update(float percentage = (-1.0F)) override { return true; }

	//파일을 읽어오는 작업
	void UpdateFileRead(int current_step, int number_of_step) override
	{
	   auto& progress= ProgressReport::Get();
	   progress.SetStatus(ProgressType::Model, "Loading...\"" + name + "\"");
	   progress.SetJobsDone(ProgressType::Model,current_step);
	   progress.SetJobCount(ProgressType::Model, number_of_step);
	}

	//파일을 읽어와서 처리하는 작업
	void UpdatePostProcess(int current_step, int number_of_step) override
	{
		auto& progress = ProgressReport::Get();
		progress.SetStatus(ProgressType::Model, "Post Processing...\"" + name + "\"");
		progress.SetJobsDone(ProgressType::Model, current_step);
		progress.SetJobCount(ProgressType::Model, number_of_step);
	}

private:
    std::string path;
	std::string name;

};