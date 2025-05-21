#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CComputeShader : public CComponent
{
private:
	CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComputeShader(const CComputeShader& rhs);
public:
	virtual ~CComputeShader();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElemDesc, _uint iNumElem);
	virtual HRESULT Initialize(void* pArg) override;


private:
	ID3D11ComputeShader* m_pComputeShader = nullptr;
	ID3DBlob* m_Blob = nullptr;

public:
	static shared_ptr<CComputeShader> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElemDesc, _uint iNumElem);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END