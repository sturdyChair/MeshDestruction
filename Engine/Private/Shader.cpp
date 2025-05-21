#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect{ rhs.m_pEffect }
	, m_iPassIndex{ rhs.m_iPassIndex }
{
	Safe_AddRef(m_pEffect);
	m_vecInputLayouts.resize(m_iPassIndex);
	for (_uint i = 0; i < m_iPassIndex; ++i)
	{
		m_vecInputLayouts[i] = rhs.m_vecInputLayouts[i];
		Safe_AddRef(m_vecInputLayouts[i]);
	}
}

CShader::~CShader()
{
	Free();
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElemDesc, _uint iNumElem)
{
	_uint iHlslFlag = 0;
#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
	//iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

	//ID3DBlob* pBlob;

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
	{
		//(_char*)pBlob->GetBufferPointer();

		//Safe_Release(pBlob);
		return E_FAIL;
	}

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (!pTechnique)
	{
		MSG_BOX(L"No Technique!");
		return E_FAIL;
	}
	D3DX11_TECHNIQUE_DESC TechniqueDesc{};
	pTechnique->GetDesc(&TechniqueDesc);
	m_iPassIndex = TechniqueDesc.Passes;
	for (_uint i = 0; i < m_iPassIndex; ++i)
	{
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
		D3DX11_PASS_DESC PassDesc{};
		pPass->GetDesc(&PassDesc);
		ID3D11InputLayout* pInputLayout = nullptr;
		auto hr = m_pDevice->CreateInputLayout(pElemDesc, iNumElem, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout);
		if (FAILED(hr))
		{
			return E_FAIL;
		}
		m_vecInputLayouts.push_back(pInputLayout);

	}

	pTechnique->GetPassByIndex(0);

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	m_pContext->IASetInputLayout(m_vecInputLayouts[iPassIndex]);
	auto pPass = m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex);
	if (!pPass)
		return E_FAIL;
	pPass->Apply(0, m_pContext);

	return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (!pVariable)
		return E_FAIL;

	auto pMatrixVariable = pVariable->AsMatrix();
	if (!pMatrixVariable)
		return E_FAIL;
	return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Bind_Vector4(const _char* pConstantName, const _float4* pVector)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (!pVariable)
		return E_FAIL;

	auto pMatrixVariable = pVariable->AsVector();
	if (!pMatrixVariable)
		return E_FAIL;
	return pMatrixVariable->SetFloatVector((_float*)pVector);
}

HRESULT CShader::Bind_Float(const _char* pConstantName, _float pFloat)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (!pVariable)
		return E_FAIL;

	auto pMatrixVariable = pVariable->AsScalar();
	if (!pMatrixVariable)
		return E_FAIL;
	return pMatrixVariable->SetFloat(pFloat);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (!pVariable)
		return E_FAIL;
	auto pSR = pVariable->AsShaderResource();
	if (!pSR)
		return E_FAIL;
	return pSR->SetResource(pSRV);


}

HRESULT CShader::Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** pSRV, _uint iNumTextures)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (!pVariable)
		return E_FAIL;
	auto pSR = pVariable->AsShaderResource();
	if (!pSR)
		return E_FAIL;
	return pSR->SetResourceArray(pSRV, 0, iNumTextures);
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iCount)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (!pVariable)
		return E_FAIL;

	auto pMatrixVariable = pVariable->AsMatrix();
	if (!pMatrixVariable)
		return E_FAIL;
	return pMatrixVariable->SetMatrixArray((_float*)pMatrix, 0, iCount);
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, void* pData, _uint iSize)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (!pVariable)
		return E_FAIL;
	return pVariable->SetRawValue(pData, 0, iSize);
}

shared_ptr<CShader> CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElemDesc, _uint iNumElem)
{
	struct MakeSharedEnabler : public CShader
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CShader(pDevice, pContext) { }
	};

	shared_ptr<CShader> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElemDesc, iNumElem)))
	{
		MSG_BOX(TEXT("Failed to Created : CShader"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CShader::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CShader
	{
		MakeSharedEnabler(const CShader& rhs) : CShader(rhs) { }
	};

	shared_ptr<CShader> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShader"));
		return nullptr;
	}

	return pInstance;
}

void CShader::Free()
{
	for (auto& pInputLayout : m_vecInputLayouts)
	{
		Safe_Release(pInputLayout);
	}
	Safe_Release(m_pEffect);
}
