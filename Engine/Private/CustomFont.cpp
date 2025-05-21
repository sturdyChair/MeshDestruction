#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CCustomFont::~CCustomFont()
{
	Free();
}

HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
	m_pBatch = new SpriteBatch(m_pContext);
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

	return S_OK;
}

HRESULT CCustomFont::Render(const _wstring& strText, const _float2& vPosition, _fvector vColor, _float fRotation, _float fScale)
{
	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, strText.c_str(), vPosition, vColor, fRotation, _float2(0.f, 0.f), fScale);

	m_pBatch->End();

	return S_OK;
}

HRESULT CCustomFont::Render(const _wstring& strText, _fvector vPosition, _fvector vColor, _float fRotation, _float fScale)
{
	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, strText.c_str(), vPosition, vColor, fRotation, XMVectorSet(0, 0, 0, 1), fScale);

	m_pBatch->End();

	return S_OK;
}

shared_ptr<CCustomFont> CCustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	struct MakeSharedEnable : public CCustomFont
	{
		MakeSharedEnable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCustomFont(pDevice, pContext)
		{
		}
	};
	shared_ptr<CCustomFont> pInstance = make_shared<MakeSharedEnable>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CCustomFont"));
		assert(false);
	}

	return pInstance;
}


void CCustomFont::Free()
{
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
