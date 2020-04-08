#pragma once

class SamplerState final : public D3D11_Object
{
public:
	SamplerState(class Context* context);
	~SamplerState();

	auto GetResource() const -> ID3D11SamplerState* {return state;}

	void Create
	(
	   const D3D11_FILTER& filter,
	   const D3D11_TEXTURE_ADDRESS_MODE& address_mode,
	   const D3D11_COMPARISON_FUNC& comparison_func
	);
	void Clear();

private:
    ID3D11Device* device =nullptr;
	ID3D11SamplerState* state =nullptr;
  
};