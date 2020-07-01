#include "D3D11Framework.h"
#include "InputLayout.h"

InputLayout::InputLayout(Context * context)
{
	device = context->GetSubsystem<Graphics>()->GetDevice();
}

InputLayout::~InputLayout()
{
	Clear();
}

//ID3DBlob는 컴파일된 HLSL의 바이트코드를 담아두는 역할을 하며 정보전달을 위해서 사용
void InputLayout::Create(D3D11_INPUT_ELEMENT_DESC * descs, const uint & count, ID3DBlob * blob)
{
	if (!descs || count == 0 || !blob)
		assert(false);

	auto hr = device->CreateInputLayout
	(
		descs,
		count,
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&input_layout
	);
	assert(SUCCEEDED(hr));
}

void InputLayout::Create(ID3DBlob * blob)
{
	if (!blob)
		assert(false);

	ID3D11ShaderReflection* reflector = nullptr; //쉐이더 정보를 가지고 있음
   //쉐이더의 정보에 접근하기 위한 인터페이스 reflection 생성
	auto hr = D3DReflect
	(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		IID_ID3D11ShaderReflection, //= __uuidof(ID3D11ShaderReflection)
		reinterpret_cast<void**>(&reflector)
	);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_DESC shader_desc;
	//shader_desc에 reflector에 저장된 정보를 넘김
	reflector->GetDesc(&shader_desc);

	//쉐이더의 여러 vertex속성을 설명하는 구조체를 담을 배열 생성
	std::vector<D3D11_INPUT_ELEMENT_DESC> input_element_descs;

	//쉐이더의 D3D11_INPUT_ELEMENT_DESC의 요소 개수만큼 실행
	for (uint i = 0; i < shader_desc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC param_desc;
		reflector->GetInputParameterDesc(i, &param_desc);

		//D3D11_INPUT_ELEMENT_DESC : 각각의 Vertex의 속성을 설명하는 구조체
		D3D11_INPUT_ELEMENT_DESC element_desc;
		element_desc.SemanticName = param_desc.SemanticName;
		element_desc.SemanticIndex = param_desc.SemanticIndex;
		element_desc.InputSlot = 0;
		element_desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		element_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		element_desc.InstanceDataStepRate = 0;

		//위치정보는 항상 x, y, z 총 3가지로 구성되어 있기 때문에 고정
		std::string semantic_name = element_desc.SemanticName;
		if (semantic_name == "POSITION")
			element_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		//위치정보 외의 정보 (color 나 uv정보)
		//요소가 float 1개
		else if (param_desc.Mask == 1)
		{
			if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				element_desc.Format = DXGI_FORMAT_R32_UINT;
			else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				element_desc.Format = DXGI_FORMAT_R32_SINT;
			else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				element_desc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		//요소가 float 2개
		else if (param_desc.Mask <= 3)
		{
			if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				element_desc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				element_desc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				element_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		//요소가 float 3개
		else if (param_desc.Mask <= 7)
		{
			if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				element_desc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				element_desc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				element_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		//요소가 float 4개
		else if (param_desc.Mask <= 15)
		{
			if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				element_desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				element_desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				element_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}


		input_element_descs.emplace_back(element_desc);
	}

	Create
	(
		input_element_descs.data(),
		static_cast<uint>(input_element_descs.size()),
		blob
	);
}

void InputLayout::Clear()
{
	SAFE_RELEASE(input_layout);
}