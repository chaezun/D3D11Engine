#include "Framework.h"
#include "Camera.h"
#include "Transform.h"
#include "Renderable.h"
#include "Scene/Actor.h"

Camera::Camera(Context * context, Actor * actor, Transform * transform)
   :IComponent(context,actor,transform)
{
   renderer=context->GetSubsystem<Renderer>();
   input = context->GetSubsystem<Input>();
   timer = context->GetSubsystem<Timer>();
}

void Camera::OnStart()
{
}

void Camera::OnUpdate()
{
   auto rotation = transform->GetRotation().ToEulerAngle();
   rotation.z = 0.0f;

   auto right=transform->GetRight();
   auto up = transform->GetUp();
   auto forward = transform->GetForward();

   if (input->BtnPress(KeyCode::CLICK_RIGHT))
   {
      if(input->KeyPress(KeyCode::KEY_W))
	    movement_speed+=forward*accelation*timer->GetDeltaTimeSec();
      
	  else if (input->KeyPress(KeyCode::KEY_S))
		movement_speed -= forward * accelation*timer->GetDeltaTimeSec();

	  if (input->KeyPress(KeyCode::KEY_D))
		  movement_speed += right * accelation*timer->GetDeltaTimeSec();

	  else if (input->KeyPress(KeyCode::KEY_A))
		  movement_speed -= right * accelation*timer->GetDeltaTimeSec();

	  if (input->KeyPress(KeyCode::KEY_E))
		  movement_speed += up * accelation*timer->GetDeltaTimeSec();

	  else if (input->KeyPress(KeyCode::KEY_Q))
		  movement_speed -= up * accelation*timer->GetDeltaTimeSec();

	  transform->Translate(movement_speed);
	  movement_speed*=drag;

	  auto delta=input->GetMouseMoveValue();

	  //마우스 휠의 움직임이라 서로 반대 좌표가 들어가야 함.
	  rotation.x+=delta.y*0.1f;
	  rotation.y+=delta.x*0.1f;

	  transform->SetRotation(Quaternion::QuaternionFromEulerAngle(rotation));
   }

   UpdateViewMatirx();
   UpdateProjectionMatrix();
}

void Camera::OnStop()
{
}

auto Camera::GetCameraRay() -> const Ray
{
    const auto& mouse_position			= context->GetSubsystem<Input>()->GetMousePosition();
	const auto& resolution				= renderer->GetResolution();
	const auto& offset				    = renderer->GetEditorOffset();
	const auto relative_mouse_position  = mouse_position - offset;

	const auto is_x_outside = (mouse_position.x < offset.x || mouse_position.x >offset.x + resolution.x);
	const auto is_y_outside = (mouse_position.y < offset.y || mouse_position.y >offset.y + resolution.y);

	if (is_x_outside || is_y_outside)
		return Ray();

	return ray = Ray(transform->GetTranslation(), ScreenToWorldPoint(relative_mouse_position));
}

auto Camera::Pick(const Vector2 & mouse_position, std::shared_ptr<Actor>& actor) -> const bool
{
    const auto& resolution				= renderer->GetResolution();
	const auto& offset					= renderer->GetEditorOffset();
	const auto relative_mouse_position  = mouse_position - offset;

	const auto is_x_outside = (mouse_position.x < offset.x || mouse_position.x >offset.x + resolution.x);
	const auto is_y_outside = (mouse_position.y < offset.y || mouse_position.y >offset.y + resolution.y);

	if(is_x_outside || is_y_outside)
	   return false;

    ray			= Ray(transform->GetTranslation(), ScreenToWorldPoint(relative_mouse_position));
	auto hits	= ray.Trace(context);

	struct Actor_Score final
	{
		Actor_Score(const std::shared_ptr<Actor>& actor, const float& ray_distance, const float& center_distance)
		  : actor(actor),
		    score(ray_distance* 0.1f + center_distance * 0.9f)
		{}

		std::shared_ptr<Actor> actor;
		float score;
	};

	std::vector<Actor_Score> scores;
	scores.reserve(hits.size());
	for (const auto& hit : hits)
	{
		if(hit.is_inside)
		   continue;

		auto& aabb = hit.actor->GetComponent<Renderable>()->GetBoundBox();
		auto center_distance = Vector3::DistanceSq(hit.position, aabb.GetCenter());

		scores.emplace_back
		(
			hit.actor,
			1.0f - hit.distance / ray.GetLength(),
			1.0f - center_distance / aabb.GetExtents().Length()
		);
	}

	actor = nullptr;

	if (!scores.empty())
	{
	   std::sort(scores.begin(), scores.end(), [](const Actor_Score& lhs, const Actor_Score& rhs)
	   {
	      return lhs.score > rhs.score;
	   });

	   actor = scores.front().actor;
	   return true;
	}

	if (!actor && !hits.empty())
       actor = hits.front().actor;

	return true;
}

//과제
auto Camera::WorldToScreenPoint(const Vector3 & world_position) -> const Vector2
{
	return Vector2();
}


auto Camera::ScreenToWorldPoint(const Vector2 & screen_position) -> const Vector3
{
    const auto& resolution=renderer->GetResolution();

	Vector3 clip_position;
	//특정 값* * 2.0f - 1.0f ->결과 값을 -1 ~ 1로 범위를 정하는 공식
	clip_position.x = (screen_position.x /resolution.x) * 2.0f - 1.0f;
	//-2.0f을 곱하는 이유는 윈도우 좌표계와 y축 방향이 반대이기 때문
	clip_position.y = (screen_position.y / resolution.y) * -2.0f + 1.0f;
	clip_position.z = 1.0f;

	const auto view_projection_inverse = (view * proj).Inverse();
	auto world_position = clip_position * view_projection_inverse;

	return world_position;
}

void Camera::UpdateViewMatirx()
{
   auto position =transform->GetTranslation();
   auto up=transform->GetUp();
   auto forward=transform->GetForward();

   view=Matrix::LookAtLH(position,(position+forward),up);
}

void Camera::UpdateProjectionMatrix()
{
   auto resolution=renderer->GetResolution();

   switch (projection_type)
   {
   case ProjectionType::Perspective:
       proj=Matrix::PerspectiveFovLH(fov, resolution.x/ resolution.y, near_plane, far_plane);
	   break;

   case ProjectionType::Orthographic:
       proj=Matrix::OrthoLH(resolution.x, resolution.y, near_plane, far_plane);
	   break;
   }
}
