#ifndef DIRECT3D_H
#define DIRECT3D_H

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <DirectXMath.h>
#include "MathsHelper.h"


class Shader;

class Direct3D {
private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	bool InitDepthBuffer(int width, int height);
	bool InitDepthStencil();
	bool InitRasteriser();
	void InitViewport(int width, int height);
	Shader* m_currentShader;

public:
	Direct3D();
	~Direct3D();

	bool Initialise(int width, int height, HWND windowHandle, bool fullscreen, bool vsync);
	void Terminate();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetVideoCardInfo(char*, int&);

	Shader* GetCurrentShader() { return m_currentShader; }
	void SetCurrentShader(Shader* shader) { m_currentShader = shader; }


};
#endif // !DIRECT3D_H
