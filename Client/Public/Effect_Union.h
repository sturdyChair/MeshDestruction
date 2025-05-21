#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEffect_Union : public CGameObject
{
public:
	static wstring ObjID;

public:
	enum EFFECT_UNION_STATE { EFFECT_UNION_SLEEP, EFFECT_UNION_START, EFFECT_UNION_PLAY, EFFECT_UNION_STOP, EFFECT_UNION_END };

protected:
	CEffect_Union(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Union(const CEffect_Union& rhs);

public:
	virtual ~CEffect_Union();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void UnionEffectPlay(_float fTimeDelta);
	void UnionEffectResume(_float fTimeDelta);
	void UnionEffectStop();

	void UnionEffectStartEvent();
	void UnionEffectEndEvent();

public:
	_wstring GetEffectUnionKey() { return m_wstrEffectUnionKey; }

private:
	vector<shared_ptr<class CEffect_Base>>	m_vecEffects;

private:
	EFFECT_UNION_STATE		m_eEffectUnionState = EFFECT_UNION_SLEEP;

	_float					m_fEffectUnionTime = 0.f;
	_float					m_fEffectUnionMaxTime = 1.f;
	
	_wstring				m_wstrEffectUnionKey = L"";

public:
	static shared_ptr<CEffect_Union> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END