
#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CObject_Pool
{
private:
	CObject_Pool();
public:
	virtual ~CObject_Pool();

public:
	HRESULT Pool_Object(const wstring& strPrototypeTag, void* pArg = nullptr);
	shared_ptr<class CGameObject> Push_Object_From_Pool(const wstring& strPrototypeTag, _uint iLevelIndex, const wstring& strLayerTag, void* pArg, _float4x4* TransformMat);
	shared_ptr<class CGameObject> Get_Object_From_Pool(const wstring& strPrototypeTag, void* pArg, _float4x4* TransformMat);
	void	Clear_Pool();
	void	Recall_Object();
	void	Recall_Object(const wstring& strPrototypeTag);
	void	Clear_Pool(_wstring strTag);
public:
	static shared_ptr<CObject_Pool> Create();
	void Free();

private:
	weak_ptr<class CGameInstance> m_pGameInstance;
	map< const wstring, vector<shared_ptr<CGameObject>>> m_mapPool;
};

END