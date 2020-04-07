#include "D3D11Framework.h"
#include "Script_Interface.h"
#include <angelscript.h>
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/Transform.h"
//#include "Scene/Component/RigidBody.h"

void Script_Interface::Register(Context * context, asIScriptEngine * script_engine)
{
	this->context = context;
	this->script_engine = script_engine;

	RegisterEnumerations();
	RegisterTypes();
	RegisterVector2();
	RegisterVector3();
	RegisterTimer();
	RegisterInput();
	RegisterTransform();
	RegisterRigidBody();
	RegisterActor();
	RegisterQuaternion();
}

void Script_Interface::RegisterEnumerations()
{
	// Log
	script_engine->RegisterEnum("LogType");
	script_engine->RegisterEnumValue("LogType", "Info", static_cast<uint>(LogType::Info));
	script_engine->RegisterEnumValue("LogType", "Warning", static_cast<uint>(LogType::Warning));
	script_engine->RegisterEnumValue("LogType", "Error", static_cast<uint>(LogType::Error));

	// Component types
	script_engine->RegisterEnum("ComponentType");
	script_engine->RegisterEnumValue("ComponentType", "Camera", static_cast<uint>(ComponentType::Camera));
	script_engine->RegisterEnumValue("ComponentType", "Renderable", static_cast<uint>(ComponentType::Renderable));
	script_engine->RegisterEnumValue("ComponentType", "Script", static_cast<uint>(ComponentType::Script));
	script_engine->RegisterEnumValue("ComponentType", "Transform", static_cast<uint>(ComponentType::Transform));
	script_engine->RegisterEnumValue("ComponentType", "RigidBody", static_cast<uint>(ComponentType::RigidBody));

	// KeyCode
	script_engine->RegisterEnum("KeyCode");
	script_engine->RegisterEnumValue("KeyCode", "CLICK_LEFT", static_cast<unsigned long>(KeyCode::CLICK_LEFT));
	script_engine->RegisterEnumValue("KeyCode", "CLICK_RIGHT", static_cast<unsigned long>(KeyCode::CLICK_RIGHT));
	script_engine->RegisterEnumValue("KeyCode", "KEY_0", static_cast<unsigned long>(KeyCode::KEY_0));
	script_engine->RegisterEnumValue("KeyCode", "KEY_1", static_cast<unsigned long>(KeyCode::KEY_1));
	script_engine->RegisterEnumValue("KeyCode", "KEY_2", static_cast<unsigned long>(KeyCode::KEY_2));
	script_engine->RegisterEnumValue("KeyCode", "KEY_3", static_cast<unsigned long>(KeyCode::KEY_3));
	script_engine->RegisterEnumValue("KeyCode", "KEY_4", static_cast<unsigned long>(KeyCode::KEY_4));
	script_engine->RegisterEnumValue("KeyCode", "KEY_5", static_cast<unsigned long>(KeyCode::KEY_5));
	script_engine->RegisterEnumValue("KeyCode", "KEY_6", static_cast<unsigned long>(KeyCode::KEY_6));
	script_engine->RegisterEnumValue("KeyCode", "KEY_7", static_cast<unsigned long>(KeyCode::KEY_7));
	script_engine->RegisterEnumValue("KeyCode", "KEY_8", static_cast<unsigned long>(KeyCode::KEY_8));
	script_engine->RegisterEnumValue("KeyCode", "KEY_9", static_cast<unsigned long>(KeyCode::KEY_9));
	script_engine->RegisterEnumValue("KeyCode", "KEY_A", static_cast<unsigned long>(KeyCode::KEY_A));
	script_engine->RegisterEnumValue("KeyCode", "KEY_B", static_cast<unsigned long>(KeyCode::KEY_B));
	script_engine->RegisterEnumValue("KeyCode", "KEY_C", static_cast<unsigned long>(KeyCode::KEY_C));
	script_engine->RegisterEnumValue("KeyCode", "KEY_D", static_cast<unsigned long>(KeyCode::KEY_D));
	script_engine->RegisterEnumValue("KeyCode", "KEY_E", static_cast<unsigned long>(KeyCode::KEY_E));
	script_engine->RegisterEnumValue("KeyCode", "KEY_F", static_cast<unsigned long>(KeyCode::KEY_F));
	script_engine->RegisterEnumValue("KeyCode", "KEY_G", static_cast<unsigned long>(KeyCode::KEY_G));
	script_engine->RegisterEnumValue("KeyCode", "KEY_H", static_cast<unsigned long>(KeyCode::KEY_H));
	script_engine->RegisterEnumValue("KeyCode", "KEY_I", static_cast<unsigned long>(KeyCode::KEY_I));
	script_engine->RegisterEnumValue("KeyCode", "KEY_J", static_cast<unsigned long>(KeyCode::KEY_J));
	script_engine->RegisterEnumValue("KeyCode", "KEY_K", static_cast<unsigned long>(KeyCode::KEY_K));
	script_engine->RegisterEnumValue("KeyCode", "KEY_L", static_cast<unsigned long>(KeyCode::KEY_L));
	script_engine->RegisterEnumValue("KeyCode", "KEY_M", static_cast<unsigned long>(KeyCode::KEY_M));
	script_engine->RegisterEnumValue("KeyCode", "KEY_N", static_cast<unsigned long>(KeyCode::KEY_N));
	script_engine->RegisterEnumValue("KeyCode", "KEY_O", static_cast<unsigned long>(KeyCode::KEY_O));
	script_engine->RegisterEnumValue("KeyCode", "KEY_P", static_cast<unsigned long>(KeyCode::KEY_P));
	script_engine->RegisterEnumValue("KeyCode", "KEY_Q", static_cast<unsigned long>(KeyCode::KEY_Q));
	script_engine->RegisterEnumValue("KeyCode", "KEY_R", static_cast<unsigned long>(KeyCode::KEY_R));
	script_engine->RegisterEnumValue("KeyCode", "KEY_S", static_cast<unsigned long>(KeyCode::KEY_S));
	script_engine->RegisterEnumValue("KeyCode", "KEY_T", static_cast<unsigned long>(KeyCode::KEY_T));
	script_engine->RegisterEnumValue("KeyCode", "KEY_U", static_cast<unsigned long>(KeyCode::KEY_U));
	script_engine->RegisterEnumValue("KeyCode", "KEY_V", static_cast<unsigned long>(KeyCode::KEY_V));
	script_engine->RegisterEnumValue("KeyCode", "KEY_W", static_cast<unsigned long>(KeyCode::KEY_W));
	script_engine->RegisterEnumValue("KeyCode", "KEY_X", static_cast<unsigned long>(KeyCode::KEY_X));
	script_engine->RegisterEnumValue("KeyCode", "KEY_Y", static_cast<unsigned long>(KeyCode::KEY_Y));
	script_engine->RegisterEnumValue("KeyCode", "KEY_Z", static_cast<unsigned long>(KeyCode::KEY_Z));
	script_engine->RegisterEnumValue("KeyCode", "KEY_SHIFT", static_cast<unsigned long>(KeyCode::KEY_SHIFT));
	script_engine->RegisterEnumValue("KeyCode", "KEY_CONTROL", static_cast<unsigned long>(KeyCode::KEY_CONTROL));
	script_engine->RegisterEnumValue("KeyCode", "KEY_SPACE", static_cast<unsigned long>(KeyCode::KEY_SPACE));

	// RigidBody Force Type
	/*script_engine->RegisterEnum("ForceType");
	script_engine->RegisterEnumValue("ForceType", "Force", static_cast<uint>(ForceType::Force));
	script_engine->RegisterEnumValue("ForceType", "Impulse", static_cast<uint>(ForceType::Impulse));*/
}

void Script_Interface::RegisterTypes()
{
	script_engine->RegisterInterface("ScriptBehavior");

	script_engine->RegisterObjectType("Settings", 0, asOBJ_REF | asOBJ_NOCOUNT);
	script_engine->RegisterObjectType("Input", 0, asOBJ_REF | asOBJ_NOCOUNT);
	script_engine->RegisterObjectType("Timer", 0, asOBJ_REF | asOBJ_NOCOUNT);
	script_engine->RegisterObjectType("Actor", 0, asOBJ_REF | asOBJ_NOCOUNT);
	script_engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOCOUNT);
	script_engine->RegisterObjectType("RigidBody", 0, asOBJ_REF | asOBJ_NOCOUNT);
	script_engine->RegisterObjectType("Renderable", 0, asOBJ_REF | asOBJ_NOCOUNT);
	script_engine->RegisterObjectType("Material", 0, asOBJ_REF | asOBJ_NOCOUNT);
	script_engine->RegisterObjectType("Camera", 0, asOBJ_REF | asOBJ_NOCOUNT);
	script_engine->RegisterObjectType("Vector2", sizeof(Vector2), asOBJ_VALUE | asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR);
	script_engine->RegisterObjectType("Vector3", sizeof(Vector3), asOBJ_VALUE | asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR);
	script_engine->RegisterObjectType("Quaternion", sizeof(Quaternion), asOBJ_VALUE | asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR);
}

void Script_Interface::RegisterTimer()
{
	script_engine->RegisterGlobalProperty("Timer timer", context->GetSubsystem<Timer>());
	script_engine->RegisterObjectMethod("Timer", "const float& GetDeltaTime()", asMETHOD(Timer, GetDeltaTimeSec), asCALL_THISCALL);
}

void Script_Interface::RegisterInput()
{
	script_engine->RegisterGlobalProperty("Input input", context->GetSubsystem<Input>());
	script_engine->RegisterObjectMethod("Input", "const Vector2& GetMousePosition()", asMETHOD(Input, GetMousePosition), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Input", "const Vector2& GetMouseMoveValue()", asMETHOD(Input, GetMouseMoveValue), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Input", "const bool KeyPress(const KeyCode& in)", asMETHOD(Input, KeyPress), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Input", "const bool KeyDown(const KeyCode& in)", asMETHOD(Input, KeyDown), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Input", "const bool KeyUp(const KeyCode& in)", asMETHOD(Input, KeyUp), asCALL_THISCALL);
}

void Script_Interface::RegisterTransform()
{
	script_engine->RegisterObjectMethod("Transform", "Transform &opAssign(const Transform &in)", asMETHODPR(Transform, operator =, (const Transform&), Transform&), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "const Vector3& GetLocalScale()", asMETHOD(Transform, GetLocalScale), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "void SetLocalScale(const Vector3& in)", asMETHOD(Transform, SetLocalScale), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "const Quaternion& GetLocalRotation()", asMETHOD(Transform, GetLocalRotation), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "void SetLocalRotation(const Quaternion& in)", asMETHOD(Transform, SetLocalRotation), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "const Vector3& GetLocalTranslation()", asMETHOD(Transform, GetLocalTranslation), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "void SetLocalTranslation(const Vector3& in)", asMETHOD(Transform, SetLocalTranslation), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "const Vector3 GetScale()", asMETHOD(Transform, GetScale), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "void SetScale(const Vector3& in)", asMETHOD(Transform, SetScale), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "const Quaternion GetRotation()", asMETHOD(Transform, GetRotation), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "void SetRotation(const Quaternion& in)", asMETHOD(Transform, SetRotation), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "const Vector3 GetTranslation()", asMETHOD(Transform, GetTranslation), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "void SetTranslation(const Vector3& in)", asMETHOD(Transform, SetTranslation), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "Transform& GetRoot()", asMETHOD(Transform, GetRoot), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "Transform& GetParent()", asMETHOD(Transform, GetParent), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "const Vector3 GetForward()", asMETHOD(Transform, GetForward), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "const Vector3 GetRight()", asMETHOD(Transform, GetRight), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "const Vector3 GetUp()", asMETHOD(Transform, GetUp), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Transform", "Actor& GetActor()", asMETHOD(Transform, GetActor), asCALL_THISCALL);
}

void Script_Interface::RegisterRigidBody()
{
	/*script_engine->RegisterObjectMethod("RigidBody", "RigidBody &opAssign(const RigidBody &in)", asMETHODPR(RigidBody, operator =, (const RigidBody&), RigidBody&), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const float& GetMass() const", asMETHOD(RigidBody, GetMass), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetMass(const float& in)", asMETHOD(RigidBody, SetMass), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const float& GetFriction() const", asMETHOD(RigidBody, GetFriction), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetFriction(const float& in)", asMETHOD(RigidBody, SetFriction), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const float& GetFrictionRolling() const", asMETHOD(RigidBody, GetFrictionRolling), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetFrictionRolling(const float& in)", asMETHOD(RigidBody, SetFrictionRolling), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const float& GetRestitution() const", asMETHOD(RigidBody, GetRestitution), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetRestitution(const float& in)", asMETHOD(RigidBody, SetRestitution), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const bool& IsUseGravity() const", asMETHOD(RigidBody, IsUseGravity), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetUseGravity(const bool& in)", asMETHOD(RigidBody, SetUseGravity), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const bool& IsKinematic() const", asMETHOD(RigidBody, IsKinematic), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetKinematic(const bool& in)", asMETHOD(RigidBody, SetKinematic), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const bool& IsHasSimulated() const", asMETHOD(RigidBody, IsHasSimulated), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetHasSimulated(const bool& in)", asMETHOD(RigidBody, SetHasSimulated), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const Vector3& GetGravity() const", asMETHOD(RigidBody, GetGravity), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetGravity(const Vector3& in)", asMETHOD(RigidBody, SetGravity), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const Vector3& GetCenterOfMass() const", asMETHOD(RigidBody, GetCenterOfMass), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetCenterOfMass(const Vector3& in)", asMETHOD(RigidBody, SetCenterOfMass), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const Vector3& GetPosition() const", asMETHOD(RigidBody, GetPosition), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetPosition(const Vector3& in)", asMETHOD(RigidBody, SetPosition), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const Quaternion GetRotation() const", asMETHOD(RigidBody, GetRotation), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void SetRotation(const Quaternion& in)", asMETHOD(RigidBody, SetRotation), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const bool& IsInWorld() const", asMETHOD(RigidBody, IsInWorld), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const Vector3& SetLinearVelocity() const", asMETHOD(RigidBody, SetLinearVelocity), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "const Vector3& SetAngularVelocity() const", asMETHOD(RigidBody, SetAngularVelocity), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void ApplyForce(const Vector3& in, const ForceType& in)", asMETHOD(RigidBody, ApplyForce), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void ApplyForceAtPosition(const Vector3& in, const Vector3& in, const ForceType& in)", asMETHOD(RigidBody, ApplyForceAtPosition), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void ApplyTorque(const Vector3& in, const ForceType& in)", asMETHOD(RigidBody, ApplyTorque), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void Activate() const", asMETHOD(RigidBody, Activate), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void Deactivate() const", asMETHOD(RigidBody, Deactivate), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("RigidBody", "void ClearForces() const", asMETHOD(RigidBody, ClearForces), asCALL_THISCALL);*/
}

void Script_Interface::RegisterActor()
{
	script_engine->RegisterObjectMethod("Actor", "Actor &opAssign(const Actor &in)", asMETHODPR(Actor, operator =, (const Actor&), Actor&), asCALL_THISCALL);
	//script_engine->RegisterObjectMethod("Actor", "const int& GetID()", asMETHOD(Actor, GetID), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Actor", "const string& GetName()", asMETHOD(Actor, GetName), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Actor", "void SetName(const string& in)", asMETHOD(Actor, SetName), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Actor", "const bool& IsActive()", asMETHOD(Actor, IsActive), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Actor", "void SetActive(const bool& in)", asMETHOD(Actor, SetActive), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Actor", "Transform &GetTransform()", asMETHOD(Actor, GetTransform_Raw), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Actor", "Camera &GetCamera()", asMETHOD(Actor, GetComponent_Raw<Camera>), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Actor", "Renderable &GetRenderable()", asMETHOD(Actor, GetComponent_Raw<Renderable>), asCALL_THISCALL);
	//script_engine->RegisterObjectMethod("Actor", "RigidBody &GetRigidBody()", asMETHOD(Actor, GetComponent_Raw<RigidBody>), asCALL_THISCALL);
}

void ConstructorVector2(Vector2* other)
{
	new(other) Vector2(0.0f, 0.0f);
}

void CopyConstructorVector2(const Vector2& in, Vector2* other)
{
	new(other) Vector2(in.x, in.y);
}

void ConstructorVector2Floats(float x, float y, Vector2* other)
{
	new(other) Vector2(x, y);
}

void DestructVector2(Vector2* other)
{
	other->~Vector2();
}

static Vector2& Vector2AddAssignVector2(const Vector2& other, Vector2* self)
{
	return *self = *self + other;
}

void Script_Interface::RegisterVector2()
{
	script_engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructorVector2), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT, "void f(const Vector2 &in)", asFUNCTION(CopyConstructorVector2), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(ConstructorVector2Floats), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectBehaviour("Vector2", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructVector2), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector2", "Vector2 &opAddAssign(const Vector2 &in)", asFUNCTION(Vector2AddAssignVector2), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector2", "Vector2 &opAssign(const Vector2 &in)", asMETHODPR(Vector2, operator=, (const Vector2&), Vector2&), asCALL_THISCALL);
	script_engine->RegisterObjectProperty("Vector2", "float x", asOFFSET(Vector2, x));
	script_engine->RegisterObjectProperty("Vector2", "float y", asOFFSET(Vector2, y));
}

void ConstructorVector3(Vector3* self)
{
	new(self) Vector3(0, 0, 0);
}

void CopyConstructorVector3(const Vector3& other, Vector3* self)
{
	new(self) Vector3(other.x, other.y, other.z);
}

void ConstructorVector3Floats(float x, float y, float z, Vector3* self)
{
	new(self) Vector3(x, y, z);
}

void DestructVector3(Vector3* self)
{
	self->~Vector3();
}

static Vector3& Vector3Assignment(const Vector3& other, Vector3* self)
{
	return *self = other;
}

static Vector3 Vector3AddVector3(const Vector3& other, Vector3* self)
{
	return *self + other;
}

static Vector3& Vector3AddAssignVector3(const Vector3& other, Vector3* self)
{
	return *self = *self + other;
}

static Vector3& Vector3SubAssignVector3(const Vector3& other, Vector3* self)
{
	return *self = *self - other;
}

static Vector3& Vector3MulAssignVector3(const Vector3& other, Vector3* self)
{
	return *self = *self * other;
}

static Vector3& Vector3MulAssignFloat(float value, Vector3* self)
{
	return *self = *self * value;
}

static Vector3 Vector3MulVector3(const Vector3& other, Vector3* self)
{
	return *self * other;
}

static Vector3 Vector3MulFloat(float value, Vector3* self)
{
	return *self * value;
}

void Script_Interface::RegisterVector3()
{
	// operator overloads http://www.angelcode.com/angelscript/sdk/docs/manual/doc_script_class_ops.html

	script_engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructorVector3), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT, "void f(const Vector3 &in)", asFUNCTION(CopyConstructorVector3), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(ConstructorVector3Floats), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectBehaviour("Vector3", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructVector3), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector3", "Vector3 &opAssign(const Vector3 &in)", asFUNCTION(Vector3Assignment), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector3", "Vector3 opAdd(const Vector3 &in)", asFUNCTION(Vector3AddVector3), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector3", "Vector3 &opAddAssign(const Vector3 &in)", asFUNCTION(Vector3AddAssignVector3), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector3", "Vector3 &opSubAssign(const Vector3 &in)", asFUNCTION(Vector3SubAssignVector3), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector3", "Vector3 &opMulAssign(const Vector3 &in)", asFUNCTION(Vector3MulAssignVector3), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector3", "Vector3 &opMulAssign(float)", asFUNCTION(Vector3MulAssignFloat), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector3", "Vector3 opMul(const Vector3 &in)", asFUNCTION(Vector3MulVector3), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector3", "Vector3 opMul(float)", asFUNCTION(Vector3MulFloat), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Vector3", "Vector3 opMul_r(float)", asFUNCTION(Vector3MulFloat), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectProperty("Vector3", "float x", asOFFSET(Vector3, x));
	script_engine->RegisterObjectProperty("Vector3", "float y", asOFFSET(Vector3, y));
	script_engine->RegisterObjectProperty("Vector3", "float z", asOFFSET(Vector3, z));
}

void ConstructorQuaternion(Quaternion* self)
{
	new(self) Quaternion(0, 0, 0, 0);
}

void CopyConstructorQuaternion(const Quaternion& other, Quaternion* self)
{
	new(self) Quaternion(other.x, other.y, other.z, other.w);
}

void ConstructorQuaternionFloats(float x, float y, float z, float w, Quaternion* self)
{
	new(self) Quaternion(x, y, z, w);
}

void DestructQuaternion(Quaternion* self)
{
	self->~Quaternion();
}

static Quaternion& QuaternionAssignment(const Quaternion& other, Quaternion* self)
{
	return *self = other;
}

static const Quaternion MyQuaternionFromEulerAngle(const Vector3& other)
{
	return Quaternion::QuaternionFromEulerAngle(other);
}

void Script_Interface::RegisterQuaternion()
{
	script_engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructorQuaternion), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_CONSTRUCT, "void f(const Quaternion &in)", asFUNCTION(CopyConstructorQuaternion), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(ConstructorQuaternionFloats), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructQuaternion), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Quaternion", "Quaternion &opAssign(const Quaternion &in)", asFUNCTION(QuaternionAssignment), asCALL_CDECL_OBJLAST);
	script_engine->RegisterObjectMethod("Quaternion", "const Vector3 ToEulerAngle() const", asMETHOD(Quaternion, ToEulerAngle), asCALL_THISCALL);
	script_engine->RegisterGlobalFunction("const Quaternion MyQuaternionFromEulerAngle(const Vector3& in)", asFUNCTION(MyQuaternionFromEulerAngle), asCALL_CDECL);
	script_engine->RegisterObjectProperty("Quaternion", "float x", asOFFSET(Quaternion, x));
	script_engine->RegisterObjectProperty("Quaternion", "float y", asOFFSET(Quaternion, y));
	script_engine->RegisterObjectProperty("Quaternion", "float z", asOFFSET(Quaternion, z));
	script_engine->RegisterObjectProperty("Quaternion", "float w", asOFFSET(Quaternion, w));
}