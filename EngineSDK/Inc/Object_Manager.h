

#pragma once

#include "GameObject.h"

/* 1. ������ü���� �����Ѵ�. */
/* 2. ������ü�� ã�Ƽ� �����Ͽ� �纻��ü�� �����Ѵ�. */
/* 3. ������ ������ �纻��ü�� �����Ѵ�. */
/* 4. �����ϰ��ִ� ���̾���� Tick���� ȣ���Ѵ�. */
/* 5. �����ϰ��ִ� ���̾���� Render (x : �׸��� ������ �����ؾ� �� �ʿ䰡 �־. ) */

BEGIN(Engine)

class CObject_Manager
{
private:
	CObject_Manager();
public:
	virtual ~CObject_Manager();

public:
	HRESULT Initialize(_uint iNumLevels);
	void Clear(_uint iLevelIndex);
public:
	HRESULT Add_Prototype(const wstring& strPrototypeTag, shared_ptr<CGameObject>pPrototype);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	shared_ptr<CGameObject> Add_Clone_Return(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	shared_ptr<CGameObject> Clone_Object(const wstring& strPrototypeTag, void* pArg);
	shared_ptr<CGameObject> Clone_Object(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);
	HRESULT Push_Object(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<CGameObject> pObject);
	shared_ptr<class CLayer> Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);


	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	shared_ptr<class CComponent> Find_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex);

private:
	map<const wstring, shared_ptr<CGameObject>>			m_Prototypes;
private:
	_uint											m_iNumLevels = { 0 };
	map<const wstring, shared_ptr<class CLayer>>* m_pLayers = { nullptr };
	typedef map<const wstring, shared_ptr<class CLayer>>		LAYERS;

public:
	shared_ptr<CGameObject> Find_Prototype(const wstring& strPrototypeTag);



public:
	static shared_ptr<CObject_Manager> Create(_uint iNumLevels);
	void Free();
};

END