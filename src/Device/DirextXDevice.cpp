#include <./Device/DirectXDevice.h>

DX11Device::DX11Device() {

}
DX11Device::~DX11Device() {
}
unsigned long DX11Device::Init(HWND handle, unsigned long width, unsigned long height, bool windowmode) {
	unsigned long ret = 0;
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	DEBUG(creationFlags |= D3D11_CREATE_DEVICE_DEBUG);
	DXGI_SWAP_CHAIN_DESC desc = {};
	desc.BufferCount = 1;
	desc.BufferDesc.Width = width;
	desc.BufferDesc.Height = height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;

	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = windowmode;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.OutputWindow = handle;

	D3D_FEATURE_LEVEL features [] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	D3D_DRIVER_TYPE devicetype [] = {
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_SOFTWARE,
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_REFERENCE
	};
	ID3D11Device *device = nullptr;
	ID3D11DeviceContext *context = nullptr;
	IDXGISwapChain *swapchain = nullptr;
	for (auto dt : devicetype) {
		if (SUCCEEDED(
			D3D11CreateDeviceAndSwapChain(
				nullptr,
				dt,
				nullptr,
				creationFlags,
				features,
				ARRAYSIZE(features),
				D3D11_SDK_VERSION,
				&desc,
				&swapchain,
				&device,
				&this->feature,
				&context
				)
			)) {
			this->device.Attach(device);
			this->context.Attach(context);
			this->swapchain.Attach(swapchain);
			this->drivertype = dt;
			break;
		}
	}
	if (!device) {
		ret = -1;
		return ret;
	}

	//デバッグ用デバイスの生成
	DEBUG(SUCCEEDED(this->context->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**) &this->anotation)));
	
	//BuckBufferの生成
	D3D11_TEXTURE2D_DESC descDepth;
	ID3D11Texture2D* pBackBuffer = NULL;
	ID3D11RenderTargetView*		rendertargetview;// レンダーターゲットビュー
	ID3D11DepthStencilView*		depthstencilview;// ステンシルビュー
	ret = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer);
	if (FAILED(ret)) {
		return ret;
	}
	ret = this->device->CreateRenderTargetView(pBackBuffer, NULL, &rendertargetview);
	if (FAILED(ret)) {
		return ret;
	}
	pBackBuffer->Release();
	pBackBuffer = nullptr;

	//ステンシルビューの生成
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width; descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1; descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ret = this->device->CreateTexture2D(&descDepth, NULL, &pBackBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDepthStencilView;
	ZeroMemory(&descDepthStencilView, sizeof(descDepthStencilView));
	descDepthStencilView.Format = descDepth.Format;
	descDepthStencilView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDepthStencilView.Texture2D.MipSlice = 0;


	ret = this->device->CreateDepthStencilView(pBackBuffer, &descDepthStencilView, &depthstencilview);
	if (pBackBuffer) {
		pBackBuffer->Release();
		pBackBuffer = nullptr;
	}
	this->rendertargetview.Attach(rendertargetview);
	this->depthstencilview.Attach(depthstencilview);

	D3D11_VIEWPORT vp;
	vp.Width = (float) width;
	vp.Height = (float) height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	this->context->RSSetViewports(1, &vp);

	//レンダーターゲットの設定
	ID3D11RenderTargetView *rtv [] = {
		this->getRTV(),
	};
	this->context->OMSetRenderTargets(1, rtv, nullptr);
	return ret;
}

