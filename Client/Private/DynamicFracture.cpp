#include "DynamicFracture.h"
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
#include "Static_Cutter.h"
#include "Snapper.h"
#include "MyParticle.h"
#include "CSG_Tester.h"
#include "Cuttable.h"
#include "Math_Manager.h"
#include "CharacterController.h"

wstring CDynamicFracture::ObjID = TEXT("CDynamicFracture");

CDynamicFracture::CDynamicFracture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDynamicFracture::CDynamicFracture(const CDynamicFracture& rhs)
	: CGameObject(rhs)
{
}

CDynamicFracture::~CDynamicFracture()
{
	Free();
}

HRESULT CDynamicFracture::Initialize_Prototype()
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
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Nathan_Mani_Mani"), TEXT("../Bin/Resources/Models/Nathan/Nathan_Mani.fbx"), MODEL_TYPE::ANIM, PreTransform));

	XMStoreFloat4x4(&PreTransform,
		XMMatrixAffineTransformation(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
			XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f),
			XMVectorSet(0.f, 0.f, 0.f, 1.f)));
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Spider"), TEXT("../Bin/Resources/Models/Spider/Spider.fbx"), MODEL_TYPE::ANIM, PreTransform));




	return S_OK;
}

HRESULT CDynamicFracture::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	_uint iNumCell = 100;
	if (pArg)
	{
		m_strModelTag = ((FDO_DESC*)pArg)->strModelTag;
		iNumCell = ((FDO_DESC*)pArg)->iNumCell;
		m_strInD = ((FDO_DESC*)pArg)->strInD;
		m_strInN = ((FDO_DESC*)pArg)->strInN;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0, true);
	_float4x4 out;
	m_pModelCom->Set_CurrentTrackPosition(0.f);
	m_pModelCom->Play_Animation(0.f, 0.f, 0.0001f, out);
	//m_pModelCom->Reset_Bones();

	m_pFracturedModelCom->Set_AnimIndex(0, true);
	m_pFracturedModelCom->Set_CurrentTrackPosition(0.f);
	m_pFracturedModelCom->Play_Animation(0.f, 0.f, 0.0001f, out);
	//m_pFracturedModelCom->Reset_Bones();

	m_PreTransform = m_pModelCom->Get_ModelData()->TransformMatrix;

	PreFracture(iNumCell);
	AddCollider("rp_nathan_animated_003_walking_lowerarm_twist_l", 0.05f, 0.15f);
	AddCollider("rp_nathan_animated_003_walking_lowerarm_twist_r", 0.05f, 0.15f);
	AddCollider("rp_nathan_animated_003_walking_upperarm_twist_l", 0.05f, 0.09f);
	AddCollider("rp_nathan_animated_003_walking_upperarm_twist_r", 0.05f, 0.09f);

	AddCollider("rp_nathan_animated_003_walking_lowerleg_twist_l", 0.075f, 0.15f);
	AddCollider("rp_nathan_animated_003_walking_lowerleg_twist_r", 0.075f, 0.15f);
	AddCollider("rp_nathan_animated_003_walking_upperleg_twist_l", 0.075f, 0.13f);
	AddCollider("rp_nathan_animated_003_walking_upperleg_twist_r", 0.075f, 0.13f);

	AddCollider("rp_nathan_animated_003_walking_spine_02", 0.15f, 0.2f);
	AddCollider("rp_nathan_animated_003_walking_head", 0.1f, 0.03f);

	BindFragments();

	return S_OK;
}

void CDynamicFracture::PriorityTick(_float fTimeDelta)
{
}

void CDynamicFracture::Tick(_float fTimeDelta)
{
	if (!m_bSnape)
	{
		_float4x4 out;
		static _bool bStop = false;
		if (GAMEINSTANCE->Get_KeyDown(DIK_R))
			bStop = !bStop;
		if (bStop)
		{
			m_pModelCom->Set_CurrentTrackPosition(0.f);
			m_pModelCom->Play_Animation(0.f, 0.f, 0.0001f, out);
		}
		else
		{
			if (m_pModelCom->Get_Current_Track_Position() >= 67.0f)
			{
				m_pModelCom->Set_AnimFrame(0.5f);
			}
			m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta, out);
		}

		if (m_bFracture)
		{
			m_bSnape = true;
			ActivateFragments();
			m_pCCT->Disable(true);
		}
		m_pCCT->Update(fTimeDelta);
	
	}
	else
	{
		UpdateFragments();
		m_LifeTimer += fTimeDelta;
		if (m_LifeTimer >= m_fMaxLife + 1.f)
		{
			Dead();
			return;
		}
	}

}

void CDynamicFracture::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CDynamicFracture::Render()
{
	if (m_bSnape)
		Render_Static();
	else
		Render_Dynamic();

	return S_OK;
}

void CDynamicFracture::OnPxContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPair, CGameObject* pOther)
{
	if (m_bSnape)
		return;
	m_bFracture = true;
}

void CDynamicFracture::Set_Position(const _float3& vPos)
{
	m_pTransformCom->Set_Position(XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
	m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
}


HRESULT CDynamicFracture::Render_Dynamic()
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

	//_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	for (size_t i = 0; i < iNumMeshes; i++)
	{

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

		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Slice);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CDynamicFracture::Snap_Mesh()
{
	if (m_bSnape)
		return;
	m_bSnape = true;
	_uint iNumMesh = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMesh; ++i)
	{
		CSnapper::MODEL_SNAP_DESC msDesc{};
		msDesc.iMeshIdx = i;
		msDesc.pModel = m_pModelCom;
		auto pSnapper = static_pointer_cast<CSnapper>(m_pGameInstance.lock()->Clone_Component(LEVEL_STATIC, CSnapper::s_PrototypeTag, &msDesc));
		vector<VTXMESH> snap_mesh = pSnapper->Snap();
		shared_ptr<MESH_DATA> md = m_pModelCom->Reconstruct_MeshData(i, snap_mesh, {});
		m_pModelCom->Set_ModelType(MODEL_TYPE::NONANIM);
		m_pModelCom->Replace_Mesh(md, i, false);

	}
	m_pModelCom->UpdateVertexInfo();
}

void CDynamicFracture::PreFracture(_uint iNumCell)
{
	_uint iNumMesh = m_pFracturedModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMesh; ++i)
	{
		CSnapper::MODEL_SNAP_DESC msDesc{};
		msDesc.iMeshIdx = i;
		msDesc.pModel = m_pFracturedModelCom;
		auto pSnapper = static_pointer_cast<CSnapper>(m_pGameInstance.lock()->Clone_Component(LEVEL_STATIC, CSnapper::s_PrototypeTag, &msDesc));
		vector<VTXMESH> snap_mesh = pSnapper->Snap();
		shared_ptr<MESH_DATA> md = m_pFracturedModelCom->Reconstruct_MeshData(i, snap_mesh, {});
		m_pFracturedModelCom->Set_ModelType(MODEL_TYPE::NONANIM);
		m_pFracturedModelCom->Replace_Mesh(md, i, false);
	}
	m_PreFracturedData = m_pFracturedModelCom->Blast_Voronoi(iNumCell);

}

void CDynamicFracture::AddCollider(const string& szBoneName, _float fRadius, _float fHeight)
{
	auto pX = GAMEINSTANCE->Get_Physics();
	//auto pScene = GAMEINSTANCE->Get_Scene();

	_float3 vScale = m_pTransformCom->Get_Scale();

	PxCapsuleGeometry CapsuleGeo{ fRadius * sqrtf(vScale.x * vScale.x * .5f + vScale.z * vScale.z * .5f), fHeight * vScale.y };

	//PxMaterial* mat = pX->createMaterial(0.5f, 0.4f, 0.5f);
	//PxShape* shape = pX->createShape(CapsuleGeo, *mat);


	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	World.r[3] = { 0.f,0.f,0.f,1.f };
	_matrix Bone = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(szBoneName.c_str()));
	PxMat44 ColliderMatrix = XMMatrixToPxMat(XMMatrixNormalize(Bone * World) );

	//auto pCollider = pX->createRigidDynamic(PxTransform{ ColliderMatrix });

	m_Colliders.push_back({ CapsuleGeo ,PxTransform{ ColliderMatrix } });
	m_ColliderBones.push_back(szBoneName);
	m_ColliderBoneOriginals.push_back({});
	XMStoreFloat4x4(&m_ColliderBoneOriginals.back(), Bone);


	//pCollider->userData = this;
	//pCollider->setName(m_ColliderBones.back().c_str());
	//pCollider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	//pCollider->attachShape(*shape);
	//pScene->addActor(*pCollider);
}

void CDynamicFracture::UpdateFragments()
{
	_float3 vScale = m_pTransformCom->Get_Scale();
	_matrix scale = XMMatrixScalingFromVector(XMLoadFloat3(&vScale));
	for (auto& Frag : m_Fragments)
	{
		PxMat44 pxWorld{ Frag.pActor->getGlobalPose() };
		_matrix World = scale * PxMatToXMMatrix(pxWorld);
		XMStoreFloat4x4(&Frag.Transform, World);
	}
}

void CDynamicFracture::ActivateFragments()
{
	auto pScene = GAMEINSTANCE->Get_Scene();
	for (auto& Frag : m_Fragments)
	{
		_matrix World = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ColliderBoneOriginals[Frag.iBindBoneIdx])) 
			* XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(m_ColliderBones[Frag.iBindBoneIdx].c_str())) * m_pTransformCom->Get_WorldMatrix_XMMat();
		PxMat44 pxMat = XMMatrixToPxMat(XMMatrixNormalize(World));
		Frag.pActor->setGlobalPose(PxTransform{ pxMat });
		pScene->addActor(*Frag.pActor);
	}
}

void CDynamicFracture::BindFragments()
{
	_float3 vScale = m_pTransformCom->Get_Scale();
	PxVec3 pxScale = { vScale.x,vScale.y,vScale.z };
	vector<shared_ptr<CBone>> temp;
	auto pPx = GAMEINSTANCE->Get_Physics();
	auto pScene = GAMEINSTANCE->Get_Scene();
	for (auto& md : m_PreFracturedData)
	{
		_uint iNbMesh = md->iNumMeshs;
		for (_uint i = 0; i < iNbMesh; ++i)
		{
			Fragment Frag{};
			Frag.pMesh = CMesh::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, md->Mesh_Datas[i], temp, XMMatrixIdentity());//XMLoadFloat4x4(& md->TransformMatrix));
			Frag.pMesh->Cooking_ConvexMesh(pxScale);
			PxConvexMeshGeometry Geo{ Frag.pMesh->Get_ConvexMesh() };
			Frag.iBindBoneIdx = FindBestMatchingBone(Geo);
			Frag.pActor = pPx->createRigidDynamic(PxTransform{PxIdentity});
			Frag.pActor->attachShape(*pPx->createShape(Geo, *pPx->createMaterial(0.5f, .4f, .5f)));
			m_Fragments.push_back(Frag);
			Frag.pActor->setName(m_ColliderBones[Frag.iBindBoneIdx].c_str());
			//pScene->addActor(*Frag.pActor);
			PxRigidBodyExt::updateMassAndInertia(*Frag.pActor, 50.f);
		}
	}
}

_uint CDynamicFracture::FindBestMatchingBone(PxConvexMeshGeometry& geo)
{
	vector<_uint> candidate;
	for (_uint i = 0; i < m_Colliders.size(); ++i)
	{
		if (PxGeometryQuery::overlap(geo, PxTransform{ PxIdentity }, m_Colliders[i].first, m_Colliders[i].second))
		{
			candidate.push_back(i);
		}
	}

	if (candidate.size() == 1)
		return candidate[0];
	if (candidate.empty())
	{
		for (_uint i = 0; i < m_Colliders.size(); ++i)
		{
			candidate.push_back(i);
		}
	}
	_float minDist = FLT_MAX; _uint iMin = 0;
	for (_uint iCand : candidate)
	{
		_float fDist = PxGeometryQuery::pointDistance(m_Colliders[iCand].second.p, geo, PxTransform{PxIdentity});
		if (fDist < minDist)
		{
			iMin = iCand;
			minDist = fDist;
		}
	}
	return iMin;
}



HRESULT CDynamicFracture::Render_Static()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	if (FAILED(m_pStaticShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pStaticShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	if (auto pInD = GAMEMANAGER->Find_GlobalTexture(m_strInD))
		pInD->Bind_ShaderResource(m_pStaticShaderCom, "g_Texture_In");
	else
		CGame_Manager::Get_Instance()->Get_InDiffuse()->Bind_ShaderResource(m_pStaticShaderCom, "g_Texture_In");

	if (auto pInD = GAMEMANAGER->Find_GlobalTexture(m_strInN))
		pInD->Bind_ShaderResource(m_pStaticShaderCom, "g_Normal_Texture_In");
	else
		CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pStaticShaderCom, "g_Normal_Texture_In");

	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	_uint	iNumMeshes = m_Fragments.size();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_float4x4 W = m_Fragments[i].Transform;

		if (FAILED(m_pStaticShaderCom->Bind_Matrix("g_WorldMatrix", &W)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pStaticShaderCom, "g_Texture", (_uint)0, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pStaticShaderCom, "g_ATOS_Texture", (_uint)0, aiTextureType_SHININESS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pStaticShaderCom, "g_ATOS_Texture");
		}
		if (FAILED(m_pModelCom->Bind_Material(m_pStaticShaderCom, "g_Normal_Texture", (_uint)0, aiTextureType_NORMALS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pStaticShaderCom, "g_Normal_Texture");
		}
		CGame_Manager::Get_Instance()->Get_DefaultNoise()->Bind_ShaderResource(m_pStaticShaderCom, "g_Noise_Texture");
		m_pStaticShaderCom->Bind_Float("g_Dissolve", m_LifeTimer - m_fMaxLife);
		m_pStaticShaderCom->Bind_Float("g_DissolveRange", 0.2f);
		
		m_pStaticShaderCom->Begin((_uint)MESH_PASS::SliceDissolve);

		m_Fragments[i].pMesh->Render();
	}

	return S_OK;
}



HRESULT CDynamicFracture::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
	m_pStaticShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_StaticShader")));
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_SphereModel")));

	m_pFracturedModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_FracturedModel")));

	CCharacterController::CCT_DESC cctDesc{};
	_vector pos = m_pTransformCom->Get_Position();
	cctDesc.pTransform = m_pTransformCom;
	cctDesc.height = 1.0f * m_pTransformCom->Get_Scale().y;
	cctDesc.radius = 0.5f * m_pTransformCom->Get_Scale().x;
	cctDesc.position = PxExtendedVec3{ pos.m128_f32[0],pos.m128_f32[1],pos.m128_f32[2] };
	cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
	cctDesc.userData = this;

	m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));
	//m_pCCT->Disable_Gravity();

	//m_pCCT->Enable_Foot_Pos_Align();
	m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
	m_pCCT->Enable_Pseudo_Physics();

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CDynamicFracture> CDynamicFracture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CDynamicFracture
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CDynamicFracture(pDevice, pContext)
		{
		}
	};


	shared_ptr<CDynamicFracture> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDynamicFracture"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CDynamicFracture::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CDynamicFracture
	{
		MakeSharedEnabler(const CDynamicFracture& rhs) : CDynamicFracture(rhs)
		{
		}
	};


	shared_ptr<CDynamicFracture> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDynamicFracture"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CDynamicFracture> CDynamicFracture::ReClone()
{
	shared_ptr<CDynamicFracture> pPrototype = static_pointer_cast<CDynamicFracture>((GAMEINSTANCE->Get_Object_Prototype(ObjID)));

	struct MakeSharedEnabler :public CDynamicFracture
	{
		MakeSharedEnabler(const CDynamicFracture& rhs) : CDynamicFracture(rhs)
		{
		}
	};

	shared_ptr<CDynamicFracture> pInstance = make_shared<MakeSharedEnabler>(*this);

	pInstance->m_strModelTag = m_strModelTag;
	pInstance->m_strInD = m_strInD;
	pInstance->m_strInN = m_strInN;
	pInstance->CGameObject::Initialize(nullptr);
	pInstance->Get_Transform()->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());

	pInstance->Ready_Components();
	pInstance->m_PreFracturedData = m_PreFracturedData;
	pInstance->m_PreTransform = m_PreTransform;
	pInstance->m_pModelCom->Set_AnimIndex(0, true);
	_float4x4 out;
	pInstance->m_pModelCom->Set_CurrentTrackPosition(0.f);
	pInstance->m_pModelCom->Play_Animation(0.f, 0.f, 0.0001f, out);
	//pInstance->m_pModelCom->Reset_Bones();

	pInstance->AddCollider("rp_nathan_animated_003_walking_lowerarm_twist_l", 0.05f, 0.15f);
	pInstance->AddCollider("rp_nathan_animated_003_walking_lowerarm_twist_r", 0.05f, 0.15f);
	pInstance->AddCollider("rp_nathan_animated_003_walking_upperarm_twist_l", 0.05f, 0.09f);
	pInstance->AddCollider("rp_nathan_animated_003_walking_upperarm_twist_r", 0.05f, 0.09f);
	pInstance->AddCollider("rp_nathan_animated_003_walking_lowerleg_twist_l", 0.075f, 0.15f);
	pInstance->AddCollider("rp_nathan_animated_003_walking_lowerleg_twist_r", 0.075f, 0.15f);
	pInstance->AddCollider("rp_nathan_animated_003_walking_upperleg_twist_l", 0.075f, 0.13f);
	pInstance->AddCollider("rp_nathan_animated_003_walking_upperleg_twist_r", 0.075f, 0.13f);
	pInstance->AddCollider("rp_nathan_animated_003_walking_spine_02", 0.15f, 0.2f);
	pInstance->AddCollider("rp_nathan_animated_003_walking_head", 0.1f, 0.03f);
	pInstance->BindFragments();
	pInstance->m_pModelCom->Set_CurrentTrackPosition(pInstance->m_pModelCom->Get_Duration() * 0.5f);

	return pInstance;
}

void CDynamicFracture::Free()
{
	for (auto& Frag : m_Fragments)
	{
		if (Frag.pActor && Frag.pActor->isReleasable())
			Frag.pActor->release();
	}
	m_Fragments.clear();
}
