

#include <Windows.h>
#include <xnamath.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <./Device/WindowDevice.h>
#include <./Device/DirectXDevice.h>
#include <./Vertex.h>

#define SAFE_RELEASE(v) if(v) {v->Release(); v = nullptr;}

// Shader�ɑ���J�������
struct ConstantBuffer{
	XMMATRIX mWorld;		//���[���h�ϊ��s��
	XMMATRIX mView;			//�r���[�ϊ��s��
	XMMATRIX mProjection;	//�������e�ϊ��s��
};

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) {

	WindowDevice &window = WindowDevice::getInstance();
	DX11Device &device = DX11Device::getInstance();
	window.Init(hInstance, nCmdShow, TEXT("test window"), 1280, 720, true);
	device.Init(window.getHandle(), window.getWidth(), window.getHeight(), window.getWindowMode());

	// ���_�̐���
	Vertex4 vertex[4] = {
		{ 500.f, 500.f, 1.0f },
		{ -500.f, 500.f, 1.0f },
		{ 500.f, -500.f, 1.0f },
		{ -500.f, -500.f, 1.0f },
	};

	// VetexBuffer�̊i�[���錾
	ID3D11Buffer *vertexbuffer = nullptr;

	D3D11_BUFFER_DESC bd; // �������@
	ZeroMemory(&bd, sizeof(bd)); // ���g���N���A
	// Buffer�̐������@�̊i�[
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertex);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // �Ȃ�̃o�b�t�@�ł����H
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU����͏������݂̂ݍs���܂�
	// Buffer�̐���
	device.getDevice()->CreateBuffer(&bd, nullptr, &vertexbuffer);

	//�@���_�����i�[���Ă���
	D3D11_MAPPED_SUBRESOURCE ms; // Buffer���i�[����ׂɂƂ肠�������b�N�������Ȃ��Ƃ����Ȃ��B�ǂ��܂Ń��b�N��������H
	device.getContext()->Map(vertexbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // �A�N�Z�X��ms
	memcpy(ms.pData, vertex, sizeof(vertex));// pData = vetex�R�s�[�@��������
	device.getContext()->Unmap(vertexbuffer, NULL); // ���b�N����

	// �p�C�v���C���ɒ��_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(Vertex4); // ���_�̃T�C�Y
	UINT offset = 0;			   // ����̒���
	device.getContext()->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);

	// �V�F�[�_�[�̐���
	ID3D11VertexShader *vs_buf = nullptr;  // shader��buffer�@�R���p�C�������V�F�[�_�[�̊i�[�� 
	ID3D11PixelShader  *ps_buf = nullptr;  // shader��buffer�@�R���p�C�������V�F�[�_�[�̊i�[��


	// 
	ID3D10Blob *vsblob, *psblob;
	D3DX11CompileFromFile(TEXT("./Shader/VS.hlsl"), 0, 0, "main", "vs_5_0", 0, 0, 0, &vsblob, 0, 0);
	D3DX11CompileFromFile(TEXT("./Shader/PS.hlsl"), 0, 0, "main", "ps_5_0", 0, 0, 0, &psblob, 0, 0);
	// blob��_buf�Ɋi�[
	device.getDevice()->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), nullptr, &vs_buf);
	device.getDevice()->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), nullptr, &ps_buf);
	// Shager�̐ݒ�
	device.getContext()->VSSetShader(vs_buf, nullptr, 0);
	device.getContext()->PSSetShader(ps_buf, nullptr, 0);

	//�@�C���v�b�g���C�A�E�g���g�����߂ɕK�v�Ȃ��� 
	D3D11_INPUT_ELEMENT_DESC element[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	ID3D11InputLayout *inputlayout;
	device.getDevice()->CreateInputLayout(element, ARRAYSIZE(element), vsblob->GetBufferPointer(), vsblob->GetBufferSize(), &inputlayout); // �i�[
	//�C���v�b�g���C�A�E�g�̐ݒ�
	device.getContext()->IASetInputLayout(inputlayout);
	// ���X���C�U�̐���
	ID3D11RasterizerState* rasterizer = nullptr;
	D3D11_RASTERIZER_DESC rasterizerDesc = {
		D3D11_FILL_SOLID, // ���C���[�t���[�� (�������t���[��)
		D3D11_CULL_FRONT, // ���ʃ|���S�����J�����O
		FALSE,
		0,
		0.0f,
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		FALSE
	};
	device.getDevice()->CreateRasterizerState(&rasterizerDesc, &rasterizer); // ���X�^���C�U�[�̐���
	// ���X�^���C�U�[�ݒ�
	device.getContext()->RSSetState(rasterizer);

	// ���_�̕��ѕ��̐ݒ�
	device.getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Shader�ɑ���s��̐���
	ConstantBuffer mtx;

	XMVECTOR hEye = XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
	XMVECTOR hAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR hUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mtx.mView = XMMatrixLookAtLH(hEye, hAt, hUp);
	mtx.mWorld = XMMatrixIdentity();
	mtx.mProjection = XMMatrixOrthographicLH(1280, 720, 1, 5000);

	// constant�o�b�t�@����
	ID3D11Buffer *constantbuffer = nullptr;
	ZeroMemory(&bd, sizeof(bd)); // ���g���N���A
	// Buffer�̐������@�̊i�[
	bd.ByteWidth = sizeof(ConstantBuffer); // size�̎w��
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // �Ȃ�̃o�b�t�@�ł����H
	bd.CPUAccessFlags = 0; // CPU����͏������݂̂ݍs���܂�
	bd.MiscFlags = 0;
	bd.StructureByteStride = sizeof(float);
	device.getDevice()->CreateBuffer(&bd, NULL, &constantbuffer); // �o�b�t�@�̐���
	// Buffer�ɍX�V�������� 
	device.getContext()->UpdateSubresource(constantbuffer, 0, NULL, &mtx, 0, 0);
	// Buffer���p�C�v���C���ɃZ�b�g
	device.getContext()->VSSetConstantBuffers(0, 1, &constantbuffer);

	int ret = 0;
	while (ret != WM_QUIT){
		ret = window.MessageLoop();
		float clear[] = { 0.3f, 0.3f, 0.3f, 0.0f };
		device.getContext()->ClearRenderTargetView(device.getRTV(), clear);
		device.getContext()->ClearDepthStencilView(device.getDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		// Box�̕`��
		device.getContext()->Draw(4, 0);// ���_��:���Ԗڂ̒��_�����邩


		device.getSwapChain()->Present(0, 0);
	}


	SAFE_RELEASE(vertexbuffer);
	SAFE_RELEASE(vs_buf);
	SAFE_RELEASE(ps_buf);
	SAFE_RELEASE(inputlayout);
	SAFE_RELEASE(rasterizer);
	SAFE_RELEASE(constantbuffer);


	return ret;
}