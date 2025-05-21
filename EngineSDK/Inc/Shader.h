#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
public:
	virtual ~CShader();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElemDesc, _uint iNumElem);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Begin(_uint iPassIndex);

	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT Bind_Vector4(const _char* pConstantName, const _float4* pVector);
	HRESULT Bind_Float(const _char* pConstantName, _float pFloat);
	HRESULT Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** pSRV, _uint iNumTextures = 1);
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iCount = 1);
	HRESULT Bind_RawValue(const _char* pConstantName, void* pData, _uint iSize);

private:
	ID3DX11Effect* m_pEffect = { nullptr };
	_uint						m_iPassIndex = 0;
	vector<ID3D11InputLayout*>	m_vecInputLayouts;

public:
	static shared_ptr<CShader> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElemDesc, _uint iNumElem);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END