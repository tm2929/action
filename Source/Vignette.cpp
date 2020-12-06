#include "Vignette.h"
#include "ShaderEX.h"

Vignette::Vignette(ID3D11Device* device, unsigned int width, unsigned int height)
{
	fullscreenQuad = std::make_unique<FullscreenQuad>(device);
	framebuffer = std::make_unique<FrameBuffer>(device, width, height, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	constantBuffer = std::make_unique<ConstantBuffer<ShaderConstants>>(device);
	D3D11_INPUT_ELEMENT_DESC input_element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(input_element);
	create_vs_from_cso(device, "Data/shaders/cso/vignette_vs.cso", vertexShader.GetAddressOf(), &inputLayout, input_element, numElements);
	create_ps_from_cso(device, "Data/shaders/cso/vignette_ps.cso", pixelShader.GetAddressOf());

	samplerState= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
}

void Vignette::Render(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* hdr_texture)
{
	samplerState->Activate(immediate_context, 0, false);
	ID3D11ShaderResourceView* nullShaderResourceView = 0;
	constantBuffer->Activate(immediate_context, 0, false, true);

	framebuffer->Clear(immediate_context);
	framebuffer->Activate(immediate_context);

	immediate_context->PSSetShader(pixelShader.Get(), 0, 0);
	immediate_context->PSSetShaderResources(0, 1, &hdr_texture);
	fullscreenQuad->Blit(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, &nullShaderResourceView);

	framebuffer->Deactivate(immediate_context);
	samplerState->Deactivate(immediate_context);

	samplerState->Activate(immediate_context, 0, false);
	constantBuffer->Activate(immediate_context, 0, false, true);

	immediate_context->PSSetShader(pixelShader.Get(), 0, 0);
	immediate_context->PSSetShaderResources(0, 1, framebuffer->GetRenderTargetShaderResourceView().GetAddressOf());
	fullscreenQuad->Blit(immediate_context);

	constantBuffer->DeActivate(immediate_context);
	samplerState->Deactivate(immediate_context);
}
//
//void Vignette::Blit(ID3D11DeviceContext* immediate_context)
//{
//
//
//}
