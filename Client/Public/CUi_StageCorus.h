#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StageCorus : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_StageCorusInfo : public CUi_2DInfo
	{
		_uint CorusStage = {};
		_uint CorusScore = {};
	};


protected:
	CUi_StageCorus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StageCorus(const CUi_2D& rhs);


public:
	virtual ~CUi_StageCorus();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	void Appear(_float fTimeDelta);
	void Adjust_Alpha(_float fTimeDelta);
	void Adjust_Scale(_float fTimeDelta);


private:
	void Initialize_Rank();
	void Initialize_Font(void* pArg);


private:
	_float m_fAppearTime = { 0 };
	_float2 m_fOriginScale = { 180, 85 };
	_float2 m_fOriginRankScale = { 80, 80 };
	FontDescInfo m_SecondFont;
	shared_ptr<class CUi_Default> m_pRank;


public:
	static shared_ptr<CUi_StageCorus> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
