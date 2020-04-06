#pragma once

class DepthStencilState final : public D3D11_Object
{
public:
	DepthStencilState(class Context* context);
	~DepthStencilState();

	auto GetResource() const -> ID3D11DepthStencilState* {return state;}

	void Create
	(
	   const bool& is_depth_enable,
	   const D3D11_COMPARISON_FUNC& comparison_func = D3D11_COMPARISON_LESS_EQUAL
	);
	void Clear();

private:
   ID3D11Device* device=nullptr;
   ID3D11DepthStencilState* state=nullptr;

};