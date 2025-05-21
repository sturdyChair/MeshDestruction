#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_RectNorm.h"
#include "VIBuffer_Point_Instance.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
//#include "Rigidbody.h"
#include "Component.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "CharacterController.h"
#include "StaticCollider.h"
#include "DynamicCollider.h"
#include "Cutter_Controller.h"
#include "Model_Cutter.h"
#include "PxRagDoll.h"
#include "MultiCutterController.h"
#include "Snapper.h"
#include "Static_Cutter.h"

/* 객체화된 컴포넌트(원형)를 레벨별로 저장한다. <-> 사본객체를 보관하지 않는다. 사본객체는 각 객체(플레이어, 몬스터, 지형 등등)들이 보관한다.  */
/* 내가 지정한 원형객체를 찾아서 복제(사본을 생성한다)하여 리턴해준다. */

BEGIN(Engine)

class CComponent_Manager
{
private:
	CComponent_Manager();
public:
	virtual ~CComponent_Manager();

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<class CComponent> pPrototype);
	shared_ptr<class CComponent> Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

private:
	_uint														m_iNumLevels = { 0 };
	vector<map<const wstring, shared_ptr<class CComponent>>>	m_pPrototypes;
	typedef map<const wstring, shared_ptr<class CComponent>>	PROTOTYPES;

public:
	shared_ptr<class CComponent> Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	static shared_ptr<CComponent_Manager> Create(_uint iNumLevels);
	void Free();
};

END
