#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StoreEntry : public CUi_2D
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_StoreEntryInfo : public CUi_2DInfo
	{
		FontDescInfo Font;
	};


protected:
	CUi_StoreEntry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreEntry(const CUi_2D& rhs);


public:
	virtual ~CUi_StoreEntry();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Font(FontDescInfo Info);
	void Set_Focus(_bool Focus) { m_bFocused = Focus; }


private:
	void Check_Focus(_float fTimeDelta);


private:
	_float2 Font_Distance = {};
	_float2 m_OriginPos = {};
	_bool m_bFocused = { false };


public:
	static shared_ptr<CUi_StoreEntry> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
