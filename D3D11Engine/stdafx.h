//미리 컴파일된 헤더(D3D11Engine전용)
#pragma once

#include "D3D11Framework.h"
//_Libraries 디렉토리 -> Lib_x64 디렉토리에
//D3D11Framework의 정적 라이브러리가 빌드될 때마다
//갱신되고 그것을 D3D11Engine에서 사용함(라이브러리 링크)
#pragma comment(lib, "D3D11Framework.lib")

#include "ImGui/Source/imgui.h" //ImGui의 헤더들은 반드시 imgui.h보다 뒤에 포함되어야 하므로 가장 앞에 포함
#include "ImGui/Source/imgui_internal.h"
#include "ImGui/Source/imgui_stdlib.h"

//Helper
#include "Helper/Editor_Helper.h"
#include "Helper/Icon_Provider.h"
#include "Helper/DragDropEvent.h"
#include "Helper/ColorPicker.h"
#include "Helper/ScriptEditor.h"
