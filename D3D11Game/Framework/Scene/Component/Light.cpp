#include "Framework.h"
#include "Light.h"
#include "Transform.h"
#include "Camera.h"

Light::Light(Context * context, Actor * actor, Transform * transform)
  :IComponent(context, actor, transform)
{
   ZeroMemory(&cpu_buffer, sizeof(LIGHT_DATA));
}

void Light::OnStart()
{
}

void Light::OnUpdate()
{
}

void Light::OnStop()
{
}

auto Light::GetDirection() const -> Vector3
{
	return transform->GetForward();
}

void Light::SetLightType(const LightType & type)
{
   light_type=type;
   is_update=true;
   //Directional일 경우 무조건 그림자를 그림
   is_cast_shadow = light_type == LightType::Directional;
}

void Light::SetRange(const float & range)
{
}

void Light::SetAngle(const float & angle)
{
}

void Light::SetCastShadow(const bool & is_cast_shadow)
{
}

void Light::UpdateConstantBuffer()
{
  if(!gpu_buffer)
  {
     gpu_buffer=std::make_shared<ConstantBuffer>(context);
	 gpu_buffer->Create<LIGHT_DATA>();
  }

  auto is_update=false;
  is_update |=cpu_buffer.color         != color;
  is_update |= cpu_buffer.intensity    != intensity;
  is_update |= cpu_buffer.position     != transform->GetTranslation();
  is_update |= cpu_buffer.range        != range;
  is_update |= cpu_buffer.direction    != GetDirection();
  is_update |= cpu_buffer.angle_radian != angle_radian;
  is_update |= cpu_buffer.bias         != bias;
  is_update |= cpu_buffer.normal_bias  != normal_bias;

  if(!is_update)
    return;

   auto gpu_data= gpu_buffer->Map<LIGHT_DATA>();

   if (!gpu_data)
   {
	   LOG_ERROR("Failed to map buffer");
	   return;
   }

   gpu_data->color        = cpu_buffer.color = color;
   gpu_data->intensity    = cpu_buffer.intensity = intensity;
   gpu_data->position     = cpu_buffer.position = transform->GetTranslation();
   gpu_data->range        = cpu_buffer.range = range;
   gpu_data->direction    = cpu_buffer.direction = GetDirection();
   gpu_data->angle_radian = cpu_buffer.angle_radian = angle_radian;
   gpu_data->bias         = cpu_buffer.bias = bias;
   gpu_data->normal_bias  = cpu_buffer.normal_bias = normal_bias;


   gpu_buffer->Unmap();
}
