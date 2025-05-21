#include "Effect_Union.h"
//#include "Effect_Union.h"
//#include "GameInstance.h"
//#include "PipeLine.h"
//#include "Shader.h"
//#include "Texture.h"
//#include "VIBuffer_Cube.h"
//#include "Model.h"

wstring CEffect_Union::ObjID = TEXT("CEffect_Union");

CEffect_Union::CEffect_Union(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CEffect_Union::CEffect_Union(const CEffect_Union& rhs)
	: CGameObject(rhs)
{

}

CEffect_Union::~CEffect_Union()
{
	Free();
}


HRESULT CEffect_Union::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Union::Initialize(void* pArg)
{
    return S_OK;
}

void CEffect_Union::PriorityTick(_float fTimeDelta)
{
}

void CEffect_Union::Tick(_float fTimeDelta)
{
}

void CEffect_Union::LateTick(_float fTimeDelta)
{
}

HRESULT CEffect_Union::Render()
{
    return S_OK;
}

void CEffect_Union::UnionEffectPlay(_float fTimeDelta)
{
}

void CEffect_Union::UnionEffectResume(_float fTimeDelta)
{
}

void CEffect_Union::UnionEffectStop()
{
}

void CEffect_Union::UnionEffectStartEvent()
{
}

void CEffect_Union::UnionEffectEndEvent()
{
}

shared_ptr<CEffect_Union> CEffect_Union::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_Union
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_Union(pDevice, pContext) {}
	};

	shared_ptr<CEffect_Union> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Union"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CEffect_Union::Clone(void* pArg)
{
    return nullptr;
}

void CEffect_Union::Free()
{

}
