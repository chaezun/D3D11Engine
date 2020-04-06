#pragma once
#include "IComponent.h"

class Transform final : public IComponent
{
public: 
	Transform
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Transform();

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	//=================================================================================================
	// [Local]
	//=================================================================================================
	auto GetLocalScale() const ->const Vector3& {return local_scale;}
	void SetLocalScale(const Vector3& scale);

	auto GetLocalTranslation() const ->const Vector3& { return local_translation; }
	void SetLocalTranslation(const Vector3& translation);

	auto GetLocalRotation() const ->const Quaternion& { return local_rotation; }
	void SetLocalRotation(const Quaternion& rotation);

	auto GetLocalMatrix() const -> const Matrix& {return local;}

	//=================================================================================================
	// [World]
	//=================================================================================================
	auto GetScale() -> const Vector3 { return world.GetScale();}
	void SetScale(const Vector3& scale);

	auto GetTranslation() -> const Vector3 { return world.GetTranslation(); }
	void SetTranslation(const Vector3& translation);

	auto GetRotation() -> const Quaternion { return world.GetRotation(); }
	void SetRotation(const Quaternion& rotation);

	auto GetRight() const -> const Vector3;
	auto GetUp() const -> const Vector3;
	auto GetForward() const -> const Vector3;

	auto GetWorldMatrix() const -> const Matrix& {return world;}
	auto GetWorldRotationMatrix() -> const Matrix;

	//=================================================================================================
	// [Hierarchy]
	//=================================================================================================
	auto GetRoot() -> Transform* { return HasParent()? parent->GetRoot():this; }

	auto GetParent() -> Transform* {return parent;}
	void SetParent(Transform* new_parent);

	auto GetChilds() const -> const std::vector<Transform*>& {return childs;}
	auto GetChildFromIndex(const uint& index) -> Transform*;
	auto GetChildFromName(const std::string& name) -> Transform*;
	auto GetChildCount() const -> const uint { return static_cast<uint>(childs.size()); }

	void AddChild(Transform* child);
	void DetatchChild();
	void AcquireChild();

	auto IsRoot() const -> const bool { return !HasParent();}
	auto IsDescendent(Transform* transform) const -> const bool;
	auto HasParent() const -> const bool {return parent ? true : false;}
	auto HasChilds() const -> const bool {return !childs.empty();}

	//=================================================================================================
	// [Constant Buffer]
	//=================================================================================================
	void UpdateConstantBuffer(const Matrix& view_proj);
	auto GetConstantBuffer() const -> const std::shared_ptr<class ConstantBuffer>& { return gpu_buffer; }

	//=================================================================================================
	// [MICS]
	//=================================================================================================
	void Translate(const Vector3& delta);
	void UpdateTransform();

private:
   Vector3 local_scale=1.0f;
   Vector3 local_translation=0.0f;
   Quaternion local_rotation=Quaternion::Identity;

   Matrix local = Matrix::Identity;
   Matrix world = Matrix::Identity;

   Transform* parent=nullptr;
   std::vector<Transform*> childs;

    //=================================================================================================
	// [Constant Buffer]
	//=================================================================================================
	TRANSFORM_DATA cpu_buffer;
	std::shared_ptr<class ConstantBuffer> gpu_buffer;

};