#include <d3d9.h>
#include "ID3D9Wrapper_Device.h"
#include "../overlay/overlay.h"

IDirect3DDevice9* Direct3DDevice9Wrapper::m_Direct3DDevice9 = NULL;
IDirect3D9* Direct3DDevice9Wrapper::m_Direct3D9 = NULL;

Direct3DDevice9Wrapper::Direct3DDevice9Wrapper(IDirect3DDevice9* pDirect3DDevice9, IDirect3D9* pDirect3D9, D3DPRESENT_PARAMETERS* pPresentationParameters)
	: m_Stride(0)
{
	m_Direct3DDevice9 = pDirect3DDevice9;
	m_Direct3D9 = pDirect3D9;
}

Direct3DDevice9Wrapper::~Direct3DDevice9Wrapper(){}

HRESULT APIENTRY Direct3DDevice9Wrapper::QueryInterface(const IID &riid, void **ppvObj)
{
	HRESULT hRes = m_Direct3DDevice9->QueryInterface(riid, ppvObj);
	if (hRes == S_OK)
		*ppvObj = this;
	else
		*ppvObj = NULL;

	return hRes;
}

ULONG APIENTRY Direct3DDevice9Wrapper::AddRef()
{
	return m_Direct3DDevice9->AddRef();
}

ULONG APIENTRY Direct3DDevice9Wrapper::Release()
{
	ULONG res = m_Direct3DDevice9->Release();
	if (res == 0) {
		delete this;
	}
	return res;
}

HRESULT APIENTRY Direct3DDevice9Wrapper::TestCooperativeLevel()
{
	return m_Direct3DDevice9->TestCooperativeLevel();
}

UINT APIENTRY Direct3DDevice9Wrapper::GetAvailableTextureMem()
{
	return m_Direct3DDevice9->GetAvailableTextureMem();
}

HRESULT APIENTRY Direct3DDevice9Wrapper::EvictManagedResources()
{
	return m_Direct3DDevice9->EvictManagedResources();
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetDirect3D(IDirect3D9** ppD3D9)
{
	HRESULT hRet = m_Direct3DDevice9->GetDirect3D(ppD3D9);
	if (SUCCEEDED(hRet))
		*ppD3D9 = m_Direct3D9;
	return hRet;
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetDeviceCaps(D3DCAPS9* pCaps)
{
	return m_Direct3DDevice9->GetDeviceCaps(pCaps);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
	return m_Direct3DDevice9->GetDisplayMode(iSwapChain, pMode);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	return m_Direct3DDevice9->GetCreationParameters(pParameters);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
	return m_Direct3DDevice9->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

void APIENTRY Direct3DDevice9Wrapper::SetCursorPosition(int X, int Y, DWORD Flags)
{
	return m_Direct3DDevice9->SetCursorPosition(X, Y, Flags);
}

BOOL APIENTRY Direct3DDevice9Wrapper::ShowCursor(BOOL bShow)
{
	return m_Direct3DDevice9->ShowCursor(bShow);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
	return m_Direct3DDevice9->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
	return m_Direct3DDevice9->GetSwapChain(iSwapChain, pSwapChain);
}

UINT APIENTRY Direct3DDevice9Wrapper::GetNumberOfSwapChains()
{
	return m_Direct3DDevice9->GetNumberOfSwapChains();
}

HRESULT APIENTRY Direct3DDevice9Wrapper::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	InvalidateImGuiDeviceObjects();

	HRESULT ret = m_Direct3DDevice9->Reset(pPresentationParameters);

	CreateImGuiDeviceObjects();

	return ret;
}

HRESULT APIENTRY Direct3DDevice9Wrapper::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	return m_Direct3DDevice9->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	return m_Direct3DDevice9->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
	return m_Direct3DDevice9->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetDialogBoxMode(BOOL bEnableDialogs)
{
	return m_Direct3DDevice9->SetDialogBoxMode(bEnableDialogs);
}

void APIENTRY Direct3DDevice9Wrapper::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
	return m_Direct3DDevice9->SetGammaRamp(iSwapChain, Flags, pRamp);
}

void APIENTRY Direct3DDevice9Wrapper::GetGammaRamp(UINT iSwapChaiTn, D3DGAMMARAMP* pRamp)
{
	return m_Direct3DDevice9->GetGammaRamp(iSwapChaiTn, pRamp);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	return m_Direct3DDevice9->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	return m_Direct3DDevice9->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	return m_Direct3DDevice9->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	return m_Direct3DDevice9->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	return m_Direct3DDevice9->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return m_Direct3DDevice9->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return m_Direct3DDevice9->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
	return m_Direct3DDevice9->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	return m_Direct3DDevice9->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	return m_Direct3DDevice9->GetRenderTargetData(pRenderTarget, pDestSurface);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	return m_Direct3DDevice9->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	return m_Direct3DDevice9->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
	return m_Direct3DDevice9->ColorFill(pSurface, pRect, color);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return m_Direct3DDevice9->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	return m_Direct3DDevice9->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	return m_Direct3DDevice9->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	return m_Direct3DDevice9->SetDepthStencilSurface(pNewZStencil);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
	return m_Direct3DDevice9->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::BeginScene()
{
	return m_Direct3DDevice9->BeginScene();
}

HRESULT APIENTRY Direct3DDevice9Wrapper::EndScene()
{
	return m_Direct3DDevice9->EndScene();
}

HRESULT APIENTRY Direct3DDevice9Wrapper::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	return m_Direct3DDevice9->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	return m_Direct3DDevice9->SetTransform(State, pMatrix);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	return m_Direct3DDevice9->GetTransform(State, pMatrix);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	return m_Direct3DDevice9->MultiplyTransform(State, pMatrix);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
	return m_Direct3DDevice9->SetViewport(pViewport);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetViewport(D3DVIEWPORT9* pViewport)
{
	return m_Direct3DDevice9->GetViewport(pViewport);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
	return m_Direct3DDevice9->SetMaterial(pMaterial);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetMaterial(D3DMATERIAL9* pMaterial)
{
	return m_Direct3DDevice9->GetMaterial(pMaterial);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetLight(DWORD Index, CONST D3DLIGHT9* pLight)
{
	return m_Direct3DDevice9->SetLight(Index, pLight);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetLight(DWORD Index, D3DLIGHT9* pLight)
{
	return m_Direct3DDevice9->GetLight(Index, pLight);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::LightEnable(DWORD Index, BOOL Enable)
{
	return m_Direct3DDevice9->LightEnable(Index, Enable);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetLightEnable(DWORD Index, BOOL* pEnable)
{
	return m_Direct3DDevice9->GetLightEnable(Index, pEnable);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetClipPlane(DWORD Index, CONST float* pPlane)
{
	return m_Direct3DDevice9->SetClipPlane(Index, pPlane);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetClipPlane(DWORD Index, float* pPlane)
{
	return m_Direct3DDevice9->GetClipPlane(Index, pPlane);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	return m_Direct3DDevice9->SetRenderState(State, Value);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{
	return m_Direct3DDevice9->GetRenderState(State, pValue);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
	return m_Direct3DDevice9->CreateStateBlock(Type, ppSB);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::BeginStateBlock()
{
	return m_Direct3DDevice9->BeginStateBlock();
}

HRESULT APIENTRY Direct3DDevice9Wrapper::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
	return m_Direct3DDevice9->EndStateBlock(ppSB);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
	return m_Direct3DDevice9->SetClipStatus(pClipStatus);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
	return m_Direct3DDevice9->GetClipStatus(pClipStatus);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
	return m_Direct3DDevice9->GetTexture(Stage, ppTexture);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	return m_Direct3DDevice9->SetTexture(Stage, pTexture);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
	return m_Direct3DDevice9->GetTextureStageState(Stage, Type, pValue);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	return m_Direct3DDevice9->SetTextureStageState(Stage, Type, Value);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
	return m_Direct3DDevice9->GetSamplerState(Sampler, Type, pValue);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	return m_Direct3DDevice9->SetSamplerState(Sampler, Type, Value);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::ValidateDevice(DWORD* pNumPasses)
{
	return m_Direct3DDevice9->ValidateDevice(pNumPasses);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
	return m_Direct3DDevice9->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
{
	return m_Direct3DDevice9->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetCurrentTexturePalette(UINT PaletteNumber)
{
	return m_Direct3DDevice9->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetCurrentTexturePalette(UINT *PaletteNumber)
{
	return m_Direct3DDevice9->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetScissorRect(CONST RECT* pRect)
{
	return m_Direct3DDevice9->SetScissorRect(pRect);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetScissorRect(RECT* pRect)
{
	return m_Direct3DDevice9->GetScissorRect(pRect);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	return m_Direct3DDevice9->SetSoftwareVertexProcessing(bSoftware);
}

BOOL APIENTRY Direct3DDevice9Wrapper::GetSoftwareVertexProcessing()
{
	return m_Direct3DDevice9->GetSoftwareVertexProcessing();
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetNPatchMode(float nSegments)
{
	return m_Direct3DDevice9->SetNPatchMode(nSegments);
}

float APIENTRY Direct3DDevice9Wrapper::GetNPatchMode()
{
	return m_Direct3DDevice9->GetNPatchMode();
}

HRESULT APIENTRY Direct3DDevice9Wrapper::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	return m_Direct3DDevice9->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return m_Direct3DDevice9->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return m_Direct3DDevice9->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return m_Direct3DDevice9->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
	return m_Direct3DDevice9->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
	return m_Direct3DDevice9->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	return m_Direct3DDevice9->SetVertexDeclaration(pDecl);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
	return m_Direct3DDevice9->GetVertexDeclaration(ppDecl);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetFVF(DWORD FVF)
{
	return m_Direct3DDevice9->SetFVF(FVF);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetFVF(DWORD* pFVF)
{
	return m_Direct3DDevice9->GetFVF(pFVF);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
	return m_Direct3DDevice9->CreateVertexShader(pFunction, ppShader);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	return m_Direct3DDevice9->SetVertexShader(pShader);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
	return m_Direct3DDevice9->GetVertexShader(ppShader);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	return m_Direct3DDevice9->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return m_Direct3DDevice9->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	return m_Direct3DDevice9->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return m_Direct3DDevice9->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
	return m_Direct3DDevice9->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return m_Direct3DDevice9->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	if (StreamNumber == 0)
		m_Stride = Stride;

	return m_Direct3DDevice9->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
	return m_Direct3DDevice9->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
{
	return m_Direct3DDevice9->SetStreamSourceFreq(StreamNumber, Setting);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting)
{
	return m_Direct3DDevice9->GetStreamSourceFreq(StreamNumber, pSetting);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	return m_Direct3DDevice9->SetIndices(pIndexData);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
	return m_Direct3DDevice9->GetIndices(ppIndexData);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
	return m_Direct3DDevice9->CreatePixelShader(pFunction, ppShader);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	return m_Direct3DDevice9->SetPixelShader(pShader);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
	return m_Direct3DDevice9->GetPixelShader(ppShader);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	return m_Direct3DDevice9->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return m_Direct3DDevice9->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	return m_Direct3DDevice9->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return m_Direct3DDevice9->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
	return m_Direct3DDevice9->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return m_Direct3DDevice9->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	return m_Direct3DDevice9->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	return m_Direct3DDevice9->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::DeletePatch(UINT Handle)
{
	return m_Direct3DDevice9->DeletePatch(Handle);
}

HRESULT APIENTRY Direct3DDevice9Wrapper::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	return m_Direct3DDevice9->CreateQuery(Type, ppQuery);
}