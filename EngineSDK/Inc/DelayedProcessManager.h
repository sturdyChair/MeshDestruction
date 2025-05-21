

#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CDelayedProcessManager 
{
private:
	CDelayedProcessManager();
public:
	virtual ~CDelayedProcessManager();

public:
	void Initialize();

	void Add_Delayed(DELAYED _tEvent)
	{
		m_EventVec.push_back(_tEvent);
	}
	void Execute(DELAYED _tEvent);

	void Push_Dead_Obj(shared_ptr<class CGameObject> _pDeadObj);

	void Tick(_float fTimeDelta);


private:
	list<shared_ptr<class CGameObject>> m_DeadObjList;
	vector<DELAYED> m_EventVec;

public:
	static shared_ptr<CDelayedProcessManager> Create();
	void Free();
};

END