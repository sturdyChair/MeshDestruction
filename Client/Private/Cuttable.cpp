#include "Cuttable.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "DynamicCollider.h"
#include "GameManager.h"
#include "Mesh.h"
#include "CSG_Manager.h"
#include "iostream"
#include "Cutter_Controller.h"
#include "PxRagDoll.h"
#include "MultiCutterController.h"

wstring CCuttable::ObjID = TEXT("CCuttable");

CCuttable::CCuttable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCuttable::CCuttable(const CCuttable& rhs)
	: CGameObject(rhs)
{
}

CCuttable::~CCuttable()
{
	Free();
}

HRESULT CCuttable::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	_float4x4 PreTransform;
	XMStoreFloat4x4(&PreTransform, 
		XMMatrixAffineTransformation(XMVectorSet(0.0001f, 0.0001f, 0.0001f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), 
			XMQuaternionRotationRollPitchYaw(-XM_PIDIV2, 0.f, 0.f),
			XMVectorSet(0.f, 0.f, 0.f, 1.f)));
	m_List_ResourceInfo.push_back(
	make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Nathan"), TEXT("../Bin/Resources/Models/Nathan/Nathan.fbx"), MODEL_TYPE::ANIM, PreTransform));

	XMStoreFloat4x4(&PreTransform,
		XMMatrixAffineTransformation(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
			XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f),
			XMVectorSet(0.f, 0.f, 0.f, 1.f)));
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Spider"), TEXT("../Bin/Resources/Models/Spider/Spider.fbx"), MODEL_TYPE::ANIM, PreTransform));


	

	return S_OK;
}

HRESULT CCuttable::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (pArg)
	{
		m_strModelTag = ((FDO_DESC*)pArg)->strModelTag;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0, true);
	m_pModelCom->Reset_Bones();

	//CPxRagDoll::PXRAGDOLL_DESC desc;
	//desc.fDynamicFriction = 0.4f;
	//desc.fRestitution = 0.5f;
	//desc.fStaticFriction = 0.5f;
	//desc.pModel = m_pModelCom;
	//desc.pTransform = m_pTransformCom;
	//m_pRagDollCom->Build_Ragdoll(desc);

	return S_OK;
}

void CCuttable::PriorityTick(_float fTimeDelta)
{
}

void CCuttable::Tick(_float fTimeDelta)
{
	//if(!m_pRagDollCom->isActive())
	//{
	//	_float4x4 out;
	//	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.01f, out);
	//}
	//else
	//{
	//	m_pRagDollCom->Update(fTimeDelta);
	//}
	//m_pCutterController->Update(fTimeDelta);

	if (!m_bCut)
	{
		if (m_pModelCom->Get_Current_Track_Position() >= 67.0f)
		{
			m_pModelCom->Set_AnimFrame(0.5f);
		}
		_float4x4 out;
		m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta, out);
	}
	else
	{
		m_pMultiCutterController->Update(fTimeDelta);
		auto& timer = m_pMultiCutterController->Get_Timer();
		_bool bDead = true;
		for (auto& time : timer)
		{
			if (time.second <= m_fSliceLifeTime + 1.f)
			{
				bDead = false;
				break;
			}
		}
		if (bDead)
			Dead();
	}
}

void CCuttable::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CCuttable::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture_In");
	CGame_Manager::Get_Instance()->Get_InDiffuse()->Bind_ShaderResource(m_pShaderCom, "g_Texture_In");

	/*
	auto& offsets = m_pCutterController->Get_Offsets();
	auto& LifeTimes = m_pCutterController->Get_LifeTimes();
	_float maxLife = m_pCutterController->Get_MaxLifeTime();
	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (LifeTimes[i] <= 0.f)
			continue;
		_float4x4 offsetedWorld;
		XMStoreFloat4x4(&offsetedWorld, XMMatrixTranslationFromVector(XMLoadFloat3(&offsets[i])) * World);
		m_pShaderCom->Bind_Matrices("g_WorldMatrix", &offsetedWorld);
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)0, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)0, aiTextureType_SHININESS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pShaderCom, "g_ATOS_Texture");
		}
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)0, aiTextureType_NORMALS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture");
		}
		
		if (LifeTimes[i] < maxLife)
		{
			CGame_Manager::Get_Instance()->Get_DefaultNoise()->Bind_ShaderResource(m_pShaderCom, "g_Noise_Texture");
			m_pShaderCom->Bind_Float("g_Dissolve", 1.f - (LifeTimes[i] / maxLife));
			m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Slice_Dissolve);
		}
		else
		{
			m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Slice);
		}
		m_pModelCom->Render(i);
	}
	*/
	auto& models = m_pMultiCutterController->Get_Models();
	auto& meshIdices = m_pMultiCutterController->Get_ControlledMeshs();
	auto& Timers = m_pMultiCutterController->Get_Timer();
	_uint idx = 0;
	for (auto& pModel : models)
	{
		for (auto i : meshIdices[idx])
		{
			if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i)))
				return E_FAIL;
			if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_ATOS_Texture", i, aiTextureType_SHININESS)))
			{
				CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pShaderCom, "g_ATOS_Texture");
			}
			if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_Normal_Texture", i, aiTextureType_NORMALS)))
			{
				CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture");
			}
			if (m_bCut)
			{
				if (Timers[idx].second >= m_fSliceLifeTime)
				{
					CGame_Manager::Get_Instance()->Get_DefaultNoise()->Bind_ShaderResource(m_pShaderCom, "g_Noise_Texture");
					m_pShaderCom->Bind_Float("g_Dissolve", (Timers[idx].second - m_fSliceLifeTime));
					m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Slice_Dissolve);
				}
				else if (Timers[idx].first >= m_fSliceLifeTime * 5.f)
				{
					CGame_Manager::Get_Instance()->Get_DefaultNoise()->Bind_ShaderResource(m_pShaderCom, "g_Noise_Texture");
					m_pShaderCom->Bind_Float("g_Dissolve", (Timers[idx].first - m_fSliceLifeTime * 5.f));
					m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Slice_Dissolve);
				}
				else
				{
					m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Slice);
				}
			}
			else
			{
				m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Slice);
			}
			pModel->Render(i);
		}

		++idx;
	}
	return S_OK;
}

void CCuttable::Cut(const _float4& plane)
{
	//if(!m_pRagDollCom->isActive())
	//{
	//	CPxRagDoll::PXRAGDOLL_DESC desc;
	//	desc.fDynamicFriction = 0.4f;
	//	desc.fRestitution = 0.5f;
	//	desc.fStaticFriction = 0.5f;
	//	desc.pModel = m_pModelCom;
	//	desc.pTransform = m_pTransformCom;
	//	m_pRagDollCom->Build_Ragdoll(desc);
	//	m_pRagDollCom->Activate();
	//}
	//m_pCutterController->Cut(plane);
	m_bCut = true;
	m_pMultiCutterController->Cut(plane);
}

void CCuttable::RemoveFromScene()
{
	m_pMultiCutterController->Deactivate();
}

void CCuttable::AddToScene()
{
	m_pMultiCutterController->Activate();
}

void CCuttable::ApplyBones()
{
	m_pMultiCutterController->ApplyBones();
}

void CCuttable::ApplyAnimation(_uint iIdx, _float fTrackPos)
{
	m_pMultiCutterController->ApplyAnimation(iIdx, fTrackPos);
}





HRESULT CCuttable::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_SphereModel")));
	//CCutter_Controller::CUTTER_DESC Desc{};
	//Desc.fApartAmount = 0.1f;
	//Desc.pOwner = this;
	//Desc.strModelComTag = TEXT("Com_SphereModel");
	//m_pCutterController = static_pointer_cast<CCutter_Controller>(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Cutter_Controller", TEXT("Con_Cutter"), &Desc));
	//m_pRagDollCom = static_pointer_cast<CPxRagDoll>(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_PxRagDoll", TEXT("Con_RagDoll")));
	//

	CMultiCutterController::CUTTER_DESC desc{};
	desc.fApartAmount = 0.1f;
	desc.pOwner = this;
	desc.strModelComTag = L"Com_SphereModel";
	m_pMultiCutterController = static_pointer_cast<CMultiCutterController>(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_MultiCutterController", TEXT("Con_Cutter"), &desc));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CCuttable> CCuttable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CCuttable
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCuttable(pDevice, pContext)
		{
		}
	};


	shared_ptr<CCuttable> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCuttable"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CCuttable::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CCuttable
	{
		MakeSharedEnabler(const CCuttable& rhs) : CCuttable(rhs)
		{
		}
	};


	shared_ptr<CCuttable> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCuttable"));
		assert(false);
	}

	return pInstance;
}

void CCuttable::Free()
{
}
