#pragma once

#include "Client_Defines.h"
#include "CUi_TalkSystem.h"

BEGIN(Client)

class CUi_TalkWindow : public CUi_TalkSystem
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_TalkWinInfo : public CUi_2DInfo
	{
		_int iTextureNum;
		FontDescInfo Font;
	};

protected:
	CUi_TalkWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_TalkWindow(const CUi_TalkSystem& rhs);

public:
	virtual ~CUi_TalkWindow();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;

public:
	void Set_Font(FontDescInfo Info);

public:
	static shared_ptr<CUi_TalkWindow> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
