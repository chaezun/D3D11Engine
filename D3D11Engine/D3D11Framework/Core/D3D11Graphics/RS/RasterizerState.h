#pragma once

//RS단계에서의 동작 -> ID3D11RasterizerState에서 동작
//1. 보이지 않는 프리미티브 제거(은면제거, 컬링 처리)
//2. 좌표의 뷰포트 변환
//3. 시저 테스트
//4. 깊이 바이어스 계산
//5. 프리미티브들을 렌더 타겟 상의 텍셀 단위로 변경
//6. 멀티 샘플링이나 필 모드의 설정에 따른 처리

class RasterizerState final : public D3D11_Object
{
public:
	RasterizerState(class Context* context);
	~RasterizerState();

	auto GetResource() const ->ID3D11RasterizerState* { return state; }

	void Create(const D3D11_CULL_MODE& cull_mode, const D3D11_FILL_MODE& fill_mode);
	void Clear();

private:
   ID3D11Device* device			= nullptr;
   ID3D11RasterizerState* state = nullptr;

};