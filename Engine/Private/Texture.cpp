#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& rhs) :
	CComponent(rhs),
	m_iNumTexture(rhs.m_iNumTexture),
	m_SRVs(rhs.m_SRVs)
{
	for (auto& pSRV : m_SRVs)
	{
		Safe_AddRef(pSRV);
	}
}

CTexture::~CTexture()
{
	Free();
}

HRESULT CTexture::Initialize_Prototype(const _wstring& strTextureFilePath, _uint iNumTexture)
{
	m_iNumTexture = iNumTexture;
	m_SRVs.resize(m_iNumTexture);
	for (_uint i = 0; i < iNumTexture; ++i)
	{
		_tchar szFilePath[MAX_PATH] = TEXT("");
		_tchar szDrive[MAX_PATH] = TEXT("");
		_tchar szDir[MAX_PATH] = TEXT("");
		_tchar szFileName[MAX_PATH] = TEXT("");
		_tchar szExtension[MAX_PATH] = TEXT("");


		wsprintf(szFilePath, strTextureFilePath.c_str(), i);

		_wsplitpath_s(szFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, szExtension, MAX_PATH);
		if (lstrcmp(szExtension, TEXT(".dds")) == 0)
		{
			HRESULT hr = CreateDDSTextureFromFile(m_pDevice, szFilePath, nullptr, &m_SRVs[i]);
			if (FAILED(hr))
			{
				wstring errMsg = L"Failed to load texture : \"";
				errMsg += strTextureFilePath;
				errMsg += L"\"";

				MSG_BOX(errMsg.c_str());

				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(CreateWICTextureFromFile(m_pDevice, szFilePath, nullptr, &m_SRVs[i])))
			{
				wstring errMsg = L"Failed to load texture : \"";
				errMsg += strTextureFilePath;
				errMsg += L"\"";

				MSG_BOX(errMsg.c_str());

				return E_FAIL;
			}
			//m_pContext->GenerateMips(m_SRVs[i]);
		}
	}
	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Render()
{
	return S_OK;
}

_bool CTexture::MoveFrame(_float fTimeDelta, _bool bLoop)
{
	m_fTimeAcc += fTimeDelta;
	_float fFrame = 1.f / (m_fSpeed * (_float)m_iNumTexture);
	if (bLoop)
	{
		if (m_fTimeAcc > fFrame)
		{
			if (m_iCurrentTex < m_iNumTexture - 1)
				m_iCurrentTex++;
			else
			{
				m_iCurrentTex = 0;
				m_fTimeAcc = 0.f;
				return false;
			}

			m_fTimeAcc -= fFrame;

			return false;
		}
	}
	else
	{
		if (m_fTimeAcc > fFrame)
		{
			if (m_iCurrentTex < m_iNumTexture - 1)
				m_iCurrentTex++;
			else
			{
				m_iCurrentTex = 0;
				m_fTimeAcc = 0.f;
				return true;
			}

			m_fTimeAcc -= fFrame;

			return false;
		}
	}
	return false;
}

HRESULT CTexture::Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iTextureIdx)
{
	return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIdx]);
}

HRESULT CTexture::Bind_ShaderResources(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iFromIdx)
{
	return pShader->Bind_SRVs(pConstantName, m_SRVs.data() + iFromIdx, m_iNumTexture - iFromIdx);
}

shared_ptr<CTexture> CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strTextureFilePath, _uint iNumTexture)
{
	struct MakeSharedEnabler : public CTexture
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTexture(pDevice, pContext) { }
	};

	shared_ptr<CTexture> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTexture)))
	{
		MSG_BOX(TEXT("Failed to Created : CTexture"));
		return nullptr;
		//Safe_Release(pInstance);
	}

	return pInstance;
}

shared_ptr<CComponent> CTexture::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CTexture
	{
		MakeSharedEnabler(const CTexture& rhs) : CTexture(rhs) { }
	};
	shared_ptr<CTexture> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTexture"));
		return nullptr;
		//Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	for (auto& pSRV : m_SRVs)
	{
		Safe_Release(pSRV);
	}
	m_SRVs.clear();
}
