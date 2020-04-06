#pragma once
//4506 에러를 그냥 무시
#pragma warning(disable : 4506)

#define NOMINMAX

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

//Window
#include <Windows.h>
#include <assert.h>

//STL
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <string>
#include <deque>
#include <queue>
#include <vector>
#include <map>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <variant>

//DirectX
#include <d3dcompiler.h>
#include <d3d11_4.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

typedef unsigned int uint;
typedef unsigned long ulong;

//Macro function
#define SAFE_RELEASE(p)         { if (p) { (p)->Release(); (p) = nullptr; } }
#define SAFE_DELETE(p)          { if (p) { delete (p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p)    { if (p) { delete[] (p); (p) = nullptr; } }

//Math
#include "Math/Math.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Color4.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include "Math/BoundBox.h"
#include "Math/Ray.h"

//Utility
#include "Utility/GUID_Generator.h"
#include "Utility/Geometry_Generator.h"
#include "Utility/FileSystem.h"
#include "Utility/FileStream.h"

//Log
#include "Log/Log.h"

//Main System
#include "Core/Settings.h"
#include "Core/Context.h"
#include "Core/Engine.h"

//Event System
#include "Core/EventSystem/EventSystem.h"

//Subsystem
#include "Core/Subsystem/Timer.h"
#include "Core/Subsystem/Input.h"
#include "Core/Subsystem/Graphics.h"
#include "Core/Subsystem/Thread.h"
#include "Core/Subsystem/Scripting.h"
#include "Core/Subsystem/ResourceManager.h"
#include "Core/Subsystem/SceneManager.h"
#include "Core/Subsystem/Renderer.h"

//Basic Data
#include "Core/D3D11/BasicData/D3D11_Config.h"
#include "Core/D3D11/BasicData/Vertex.h"
#include "Core/D3D11/BasicData/Geometry.h"

//D3D11 Wrapper class
#include "Core/D3D11/D3D11_Object.h"
#include "Core/D3D11/IA/VertexBuffer.h"
#include "Core/D3D11/IA/IndexBuffer.h"
#include "Core/D3D11/IA/InputLayout.h"
#include "Core/D3D11/Shader/VertexShader.h"
#include "Core/D3D11/Shader/PixelShader.h"
#include "Core/D3D11/Shader/ConstantBuffer.h"
#include "Core/D3D11/Shader/SamplerState.h"
#include "Core/D3D11/RS/RasterizerState.h"
#include "Core/D3D11/OM/DepthStencilState.h"

//Shader
#include "Shader/Shader.h"

//Resource
#include "Resource/Texture2D.h"
#include "Resource/Mesh.h"
#include "Resource/Material.h"
#include "Resource/Model.h"