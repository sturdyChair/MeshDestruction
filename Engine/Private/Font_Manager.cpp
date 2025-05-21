#include "..\Public\Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CFont_Manager::~CFont_Manager()
{
	Free();
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath)
{
	if (nullptr != Find_Font(strFontTag))
		return E_FAIL;

	shared_ptr<CCustomFont> pFont = CCustomFont::Create(m_pDevice, m_pContext, strFontFilePath.c_str());
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor, _float fRotation, _float fScale)
{
	shared_ptr<CCustomFont> pFont = Find_Font(strFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(strText, vPosition, vColor, fRotation, fScale);
}

HRESULT CFont_Manager::Render(const _wstring& strFontTag, const _wstring& strText, _fvector vPosition, _fvector vColor, _float fRotation, _float fScale)
{
	shared_ptr<CCustomFont> pFont = Find_Font(strFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(strText, vPosition, vColor, fRotation, fScale);
}

shared_ptr<CCustomFont> CFont_Manager::Find_Font(const _wstring& strFontTag)
{
	auto	iter = m_Fonts.find(strFontTag);

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;
}

shared_ptr<CFont_Manager> CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CFont_Manager
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFont_Manager(pDevice,pContext)
		{}
	};

	return make_shared<MakeSharedEnabler>(pDevice, pContext);
}

void CFont_Manager::Free()
{

	m_Fonts.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
