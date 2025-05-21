#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_LoadingNote : public CUi_2D
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_NoteInfo : public CUi_2DInfo
	{
		_float fAppearTime;
		_float fDisappearTime;
	};

protected:
	CUi_LoadingNote(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_LoadingNote(const CUi_2D& rhs);


public:
	virtual ~CUi_LoadingNote();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_bool  m_bVisible        = { false };
	_float m_fAppearTime     = { 0.f };
	_float m_fDisappearTime  = { 0.f };
	_float m_fAnimationTimer = { 0.f };

public:
	static shared_ptr<CUi_LoadingNote> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
