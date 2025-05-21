#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CFont_Manager
{
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CFont_Manager();

public:
	HRESULT Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath);
	HRESULT Render(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float rotation = 0.f, _float fScale = 1.f);
	HRESULT Render(const _wstring& strFontTag, const _wstring& strText, _fvector vPosition, _fvector vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float rotation = 0.f, _float fScale = 1.f);
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	map<const _wstring, shared_ptr<class CCustomFont>>			m_Fonts;

private:
	shared_ptr<class CCustomFont> Find_Font(const _wstring& strFontTag);

public:
	static shared_ptr<CFont_Manager> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() ;
};

END