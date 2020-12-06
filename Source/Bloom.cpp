#include "Bloom.h"
#include "ShaderEX.h"
//Bloom 明るいところがぼやけるように
Bloom::Bloom(ID3D11Device* device, unsigned int width, unsigned int height)
{
	fullscreenQuad = std::make_unique<FullscreenQuad>(device);
	constantBuffer = std::make_unique<ConstantBuffer<ShaderConstants>>(device);

	glowExtraction = std::make_unique<FrameBuffer>(device, width, height, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);

	//縮小バッファ　きれいにぼかすため
	gaussianBlur[0][0] = std::make_unique<FrameBuffer>(device, width >> 0, height >> 0, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussianBlur[0][1] = std::make_unique<FrameBuffer>(device, width >> 0, height >> 0, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussianBlur[1][0] = std::make_unique<FrameBuffer>(device, width >> 1, height >> 1, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussianBlur[1][1] = std::make_unique<FrameBuffer>(device, width >> 1, height >> 1, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussianBlur[2][0] = std::make_unique<FrameBuffer>(device, width >> 2, height >> 2, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussianBlur[2][1] = std::make_unique<FrameBuffer>(device, width >> 2, height >> 2, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussianBlur[3][0] = std::make_unique<FrameBuffer>(device, width >> 3, height >> 3, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussianBlur[3][1] = std::make_unique<FrameBuffer>(device, width >> 3, height >> 3, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussianBlur[4][0] = std::make_unique<FrameBuffer>(device, width >> 4, height >> 4, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussianBlur[4][1] = std::make_unique<FrameBuffer>(device, width >> 4, height >> 4, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);

	//光取得
	create_ps_from_cso(device, "Data/shaders/cso/glow_extraction_ps.cso", glowExtractionShader.GetAddressOf());
	//縦と横をぼかすやつ
	create_ps_from_cso(device, "Data/shaders/cso/gaussian_blur_horizontal_ps.cso", gaussianBlurHorizontalShader.GetAddressOf());
	create_ps_from_cso(device, "Data/shaders/cso/gaussian_blur_vertical_ps.cso", gaussianBlurVerticalShader.GetAddressOf());
	//合体
	create_ps_from_cso(device, "Data/shaders/cso/gaussian_blur_convolution_ps.cso", gaussianBlurConvolutionShader.GetAddressOf());

	samplerStates[LINEAR_BORDER] = std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER);
	samplerStates[POINT] = std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
	samplerStates[LINEAR] = std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
	samplerStates[ANISOTROPIC] = std::make_unique<Sampler>(device, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);

	blendState = std::make_unique<BlendState>(device, BLEND_MODE::ADD);
}

void Bloom::Generate(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* hdr_texture)
{
	samplerStates[POINT]->Activate(immediate_context, 0, false);
	samplerStates[LINEAR]->Activate(immediate_context, 1, false);
	samplerStates[ANISOTROPIC]->Activate(immediate_context, 2, false);

	constantBuffer->Activate(immediate_context, 0, false, true);

	ID3D11ShaderResourceView* nullShaderResourceView = 0;
	glowExtraction->Clear(immediate_context);
	glowExtraction->Activate(immediate_context);
	
	immediate_context->PSSetShader(glowExtractionShader.Get(), 0, 0);
	immediate_context->PSSetShaderResources(0, 1, &hdr_texture);

	fullscreenQuad->Blit(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, &nullShaderResourceView);

	glowExtraction->Deactivate(immediate_context);

	
	samplerStates[POINT]->Deactivate(immediate_context);
	samplerStates[LINEAR]->Deactivate(immediate_context);
	samplerStates[ANISOTROPIC]->Deactivate(immediate_context);

	//Gaussian blur
	//Efficient Gaussian blur with linear sampling
	//http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	samplerStates[LINEAR_BORDER]->Activate(immediate_context, 0, false);

	//ひたすらぼかす
	immediate_context->PSSetShaderResources(0, 1, glowExtraction->GetRenderTargetShaderResourceView().GetAddressOf());
	gaussianBlur[0][0]->Clear(immediate_context);
	gaussianBlur[0][0]->Activate(immediate_context);

	immediate_context->PSSetShader(gaussianBlurHorizontalShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);

	gaussianBlur[0][0]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussianBlur[0][0]->GetRenderTargetShaderResourceView().GetAddressOf());
	gaussianBlur[0][1]->Clear(immediate_context);
	gaussianBlur[0][1]->Activate(immediate_context);

	immediate_context->PSSetShader(gaussianBlurVerticalShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);
	
	gaussianBlur[0][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussianBlur[0][1]->GetRenderTargetShaderResourceView().GetAddressOf());
	gaussianBlur[1][0]->Clear(immediate_context);
	gaussianBlur[1][0]->Activate(immediate_context);

	immediate_context->PSSetShader(gaussianBlurHorizontalShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);
	
	gaussianBlur[1][0]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussianBlur[1][0]->GetRenderTargetShaderResourceView().GetAddressOf());
	gaussianBlur[1][1]->Clear(immediate_context);
	gaussianBlur[1][1]->Activate(immediate_context);

	immediate_context->PSSetShader(gaussianBlurVerticalShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);
	
	gaussianBlur[1][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussianBlur[1][1]->GetRenderTargetShaderResourceView().GetAddressOf());
	gaussianBlur[2][0]->Clear(immediate_context);
	gaussianBlur[2][0]->Activate(immediate_context);

	immediate_context->PSSetShader(gaussianBlurHorizontalShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);

	gaussianBlur[2][0]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussianBlur[2][0]->GetRenderTargetShaderResourceView().GetAddressOf());
	gaussianBlur[2][1]->Clear(immediate_context);
	gaussianBlur[2][1]->Activate(immediate_context);

	immediate_context->PSSetShader(gaussianBlurVerticalShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);

	gaussianBlur[2][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussianBlur[2][1]->GetRenderTargetShaderResourceView().GetAddressOf());
	gaussianBlur[3][0]->Clear(immediate_context);
	gaussianBlur[3][0]->Activate(immediate_context);

	immediate_context->PSSetShader(gaussianBlurHorizontalShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);

	gaussianBlur[3][0]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussianBlur[3][0]->GetRenderTargetShaderResourceView().GetAddressOf());
	gaussianBlur[3][1]->Clear(immediate_context);
	gaussianBlur[3][1]->Activate(immediate_context);

	immediate_context->PSSetShader(gaussianBlurVerticalShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);

	gaussianBlur[3][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussianBlur[3][1]->GetRenderTargetShaderResourceView().GetAddressOf());
	gaussianBlur[4][0]->Clear(immediate_context);
	gaussianBlur[4][0]->Activate(immediate_context);
	
	immediate_context->PSSetShader(gaussianBlurHorizontalShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);

	gaussianBlur[4][0]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussianBlur[4][0]->GetRenderTargetShaderResourceView().GetAddressOf());
	gaussianBlur[4][1]->Clear(immediate_context);
	gaussianBlur[4][1]->Activate(immediate_context);

	immediate_context->PSGetShader(defaultPixelShader.ReleaseAndGetAddressOf(), 0, 0);
	immediate_context->PSSetShader(gaussianBlurVerticalShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);
	
	gaussianBlur[4][1]->Deactivate(immediate_context);
	immediate_context->PSSetShader(defaultPixelShader.Get(), 0, 0);
	samplerStates[LINEAR_BORDER]->Deactivate(immediate_context);
}
void Bloom::Blit(ID3D11DeviceContext* immediate_context)
{
	samplerStates[POINT]->Activate(immediate_context, 0, false);
	samplerStates[LINEAR]->Activate(immediate_context, 1, false);
	samplerStates[ANISOTROPIC]->Activate(immediate_context, 2, false);
	blendState->Activate(immediate_context);
	constantBuffer->Activate(immediate_context, 0, false, true);

	ID3D11ShaderResourceView* shaderResourceViews[5] =
	{
		gaussianBlur[0][1]->GetRenderTargetShaderResourceView().Get(),
		gaussianBlur[1][1]->GetRenderTargetShaderResourceView().Get(),
		gaussianBlur[2][1]->GetRenderTargetShaderResourceView().Get(),
		gaussianBlur[3][1]->GetRenderTargetShaderResourceView().Get(),
		gaussianBlur[4][1]->GetRenderTargetShaderResourceView().Get()
	};
	immediate_context->PSSetShaderResources(0, 5, shaderResourceViews);
	immediate_context->PSSetShader(gaussianBlurConvolutionShader.Get(), 0, 0);
	fullscreenQuad->Blit(immediate_context);

	ID3D11ShaderResourceView* nullShaderResourceViews[5] = { 0 };
	immediate_context->PSSetShaderResources(0, 5, nullShaderResourceViews);

	constantBuffer->DeActivate(immediate_context);
	blendState->Deactivate(immediate_context);
	samplerStates[POINT]->Deactivate(immediate_context);
	samplerStates[LINEAR]->Deactivate(immediate_context);
	samplerStates[ANISOTROPIC]->Deactivate(immediate_context);

}
