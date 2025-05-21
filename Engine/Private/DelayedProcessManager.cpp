#include "DelayedProcessManager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Level.h"

CDelayedProcessManager::CDelayedProcessManager()
{}

CDelayedProcessManager::~CDelayedProcessManager()
{
	Free();
}


void CDelayedProcessManager::Initialize()
{
}

void CDelayedProcessManager::Execute(DELAYED _tEvent)
{
	DELAYED_TYPE eType = _tEvent.eType;
	switch (eType)
	{
	case DELAYED_TYPE::CREATE_OBJECT:
		break;
	case DELAYED_TYPE::DELETE_OBJECT:
	{
		CGameObject* pObj = ((CGameObject*)_tEvent.wParam);
		pObj->Set_Dead();
		//m_DeadObjList.push_back(pObj);
		//Safe_AddRef(pObj);
	}
	break;
	case DELAYED_TYPE::CHANGE_LEVEL_DELAY:
	{
		CGameInstance::Get_Instance()->Change_Level(((CLevel*)_tEvent.wParam)->shared_from_this());
		m_EventVec.clear();
	}
	break;

	default:
		break;
	}
}

void CDelayedProcessManager::Push_Dead_Obj(shared_ptr<class CGameObject> _pDeadObj)
{
	m_DeadObjList.push_back(_pDeadObj);
}

void CDelayedProcessManager::Tick(_float fTimeDelta)
{
	m_DeadObjList.clear();
	for (auto& elem : m_EventVec)
	{
		Execute(elem);
	}
	m_EventVec.clear();

}

shared_ptr<CDelayedProcessManager> CDelayedProcessManager::Create()
{
	MAKE_SHARED_ENABLER(CDelayedProcessManager)
	shared_ptr<CDelayedProcessManager> pInstance = make_shared <MakeSharedEnabler>();

	return pInstance;
}

void CDelayedProcessManager::Free()
{
	m_EventVec.clear();
	m_DeadObjList.clear();
}


