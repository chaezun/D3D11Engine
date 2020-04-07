#include "D3D11Framework.h"
#include "Transform.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"

Transform::Transform(Context * context, Actor * actor, Transform * transform)
	:IComponent(context, actor, transform)
{
   ZeroMemory(&cpu_buffer, sizeof(TRANSFORM_DATA));
}

Transform::~Transform()
{
	childs.clear();
	childs.shrink_to_fit();
}

void Transform::OnStart()
{
}

void Transform::OnUpdate()
{
}

void Transform::OnStop()
{
}

void Transform::SetLocalScale(const Vector3 & scale)
{
	if (local_scale == scale)
		return;

	local_scale.x = (scale.x == 0.0f) ? std::numeric_limits<float>::epsilon() : scale.x;
	local_scale.y = (scale.y == 0.0f) ? std::numeric_limits<float>::epsilon() : scale.y;
	local_scale.z = (scale.z == 0.0f) ? std::numeric_limits<float>::epsilon() : scale.z;

	UpdateTransform();
}

void Transform::SetLocalTranslation(const Vector3 & translation)
{
	if (local_translation == translation)
		return;

	local_translation = translation;
	UpdateTransform();
}

void Transform::SetLocalRotation(const Quaternion & rotation)
{
	if (local_rotation == rotation)
		return;

	local_rotation = rotation;
	UpdateTransform();
}

void Transform::SetScale(const Vector3 & scale)
{
	if (GetScale() == scale)
		return;

	SetLocalScale(HasParent() ? scale / parent->GetScale() : scale);
}

void Transform::SetTranslation(const Vector3 & translation)
{
	if (GetTranslation() == translation)
		return;

	SetLocalTranslation(HasParent() ? translation * parent->GetWorldMatrix().Inverse() : translation);
}

void Transform::SetRotation(const Quaternion & rotation)
{
	if (GetRotation() == rotation)
		return;

	SetLocalRotation(HasParent() ? rotation * parent->GetRotation().Inverse() : rotation);
}

auto Transform::GetRight() const -> const Vector3
{
	return Vector3::Right*local_rotation;
}

auto Transform::GetUp() const -> const Vector3
{
	return Vector3::Up*local_rotation;
}

auto Transform::GetForward() const -> const Vector3
{
	return Vector3::Forward*local_rotation;
}

auto Transform::GetWorldRotationMatrix() -> const Matrix
{
	return Matrix::RotationQuaternion(GetRotation());
}

void Transform::SetParent(Transform * new_parent)
{
    //부모가 nullptr인 경우
	if (!new_parent)
	{
		DetatchChild();
		return;
	}

	//인자로 들어온 부모가 내 자신인 경우
	if (new_parent->GetID() == GetID())
		return; 

    //자신이 부모가 있는 경우
	if (HasParent())
	{
	    //내 부모와 새 부모가 같은 경우
		if(new_parent->GetID()==parent->GetID())
		return;
	}

	if (new_parent->IsDescendent(this))
	{
		if (HasParent())
		{
			for(const auto& child: childs)
			   child->SetParent(parent);
	    }

		else
		{
			for(const auto& child: childs)
			   child->DetatchChild();
		}
	}

	auto old_parent = parent;
	parent=new_parent;

	if(old_parent)
	  old_parent->AcquireChild();

    if(parent)
	    parent->AcquireChild();

    UpdateTransform();
}

auto Transform::GetChildFromIndex(const uint & index) -> Transform *
{
	if (!HasChilds())
	{
		LOG_WARNING(GetActorName() + " has no children");
		return nullptr;
	}

	if (index >= GetChildCount())
	{
		LOG_WARNING("There is no child with an index of\"" + std::to_string(index) + "\"");
		return nullptr;
	}

	return childs[index];
}

auto Transform::GetChildFromName(const std::string & name) -> Transform *
{
	for (const auto& child : childs)
	{
		if(child->GetActorName()==name)
		  return child;
	}

	return nullptr;
}

void Transform::AddChild(Transform * child)
{
   if(!child)
     return;

   if(child->GetID()==GetID())
     return;

    child->SetParent(this);
}

void Transform::DetatchChild()
{
   if(!HasParent())
      return;
   
   auto old_parent=parent;
   parent=nullptr;

   UpdateTransform();

   if(old_parent)
      old_parent->AcquireChild();
}

void Transform::AcquireChild()
{
    childs.clear();
	childs.shrink_to_fit();

	auto scene=context->GetSubsystem<SceneManager>()->GetCurrentScene();

	if (scene)
	{
		auto actors=scene->GetAllActors();
		for (const auto& actor : actors)
		{
			if(!actor)
			  continue;

            auto child=actor->GetTransform();

			if(!child->HasParent())
			  continue;
            
			if (child->GetParent()->GetID() == GetID())
			{
				childs.emplace_back(child.get());
				child->AcquireChild();
			}
		}
	}
}

auto Transform::IsDescendent(Transform * transform) const -> const bool
{
    auto descendents =transform->GetChilds();

	for (const auto& descendent : descendents)
	{
		if (descendent->GetID() == GetID())
		    return true;   
	}

	return false;
}

void Transform::UpdateConstantBuffer(const Matrix & view_proj)
{
	if (!gpu_buffer)
	{
	   gpu_buffer=std::make_shared<ConstantBuffer>(context);
	   gpu_buffer->Create<TRANSFORM_DATA>();
    }

	if(cpu_buffer.world==world)
	   return;

	auto gpu_data= gpu_buffer->Map<TRANSFORM_DATA>();
	if (!gpu_data)
	{
		LOG_ERROR("Failed to map buffer");
		return;
	}

	gpu_data->world=cpu_buffer.world=world;

	gpu_buffer->Unmap();
}

void Transform::Translate(const Vector3 & delta)
{
	if (HasParent())
		SetLocalTranslation(local_translation+delta*parent->GetWorldMatrix().Inverse());

	else
	    SetLocalTranslation(local_translation+delta);
}

void Transform::UpdateTransform()
{
	auto S = Matrix::Scaling(local_scale);
	auto R = Matrix::RotationQuaternion(local_rotation);
	auto T = Matrix::Translation(local_translation);

	local = S * R*T;

	if (HasParent())
		world = local * parent->GetWorldMatrix();
	else
		world = local;

	for (const auto& child : childs)
		child->UpdateTransform();
}
