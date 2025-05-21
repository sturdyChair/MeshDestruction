#include "Woundable.h"
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
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CWoundable::ObjID = TEXT("CWoundable");

CWoundable::CWoundable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CWoundable::CWoundable(const CWoundable& rhs)
	: CGameObject(rhs)
{
}

CWoundable::~CWoundable()
{
	Free();
}

HRESULT CWoundable::Initialize_Prototype()
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

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_SepSphere"), TEXT("../Bin/Resources/Models/Cube/SepSphere.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);



	return S_OK;
}

HRESULT CWoundable::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (pArg)
	{
		m_strModelTag = ((FDO_DESC*)pArg)->strModelTag;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_pWoundTransform[0] = CTransform::Create(m_pDevice, m_pContext);
	m_pWoundTransform[1] = CTransform::Create(m_pDevice, m_pContext);

	m_pModelCom->Set_AnimIndex(0, true);
	m_pModelCom->Reset_Bones();
	_matrix Elliposid = XMMatrixAffineTransformation({ 15.f, 15.f,30.f,0.f }, { 0.f,0.f,0.f,1.f }, XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f), { 0.f,1000.f, 0.f,1.f });
	_matrix ElliposidInv = XMMatrixInverse(nullptr, Elliposid);
	XMStoreFloat4x4(&m_Ellipsoid[0], Elliposid);
	XMStoreFloat4x4(&m_Ellipsoid[1], Elliposid);
	XMStoreFloat4x4(&m_EllipsoidInv[0], ElliposidInv);
	XMStoreFloat4x4(&m_EllipsoidInv[1], ElliposidInv);
	XMStoreFloat4x4(& m_PreTransform, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	XMStoreFloat4x4(&m_WoundTransform[0], XMLoadFloat4x4(&m_Ellipsoid[0]) * XMLoadFloat4x4(&m_PreTransform));
	XMStoreFloat4x4(&m_WoundTransform[1], XMLoadFloat4x4(&m_Ellipsoid[1]) * XMLoadFloat4x4(&m_PreTransform));
	m_pWoundTransform[0]->Set_WorldMatrix(m_WoundTransform[0]);
	m_pWoundTransform[1]->Set_WorldMatrix(m_WoundTransform[1]);
	m_pTransformCom->Get_Scale();
	//0	rp_nathan_animated_003_walking_lowerarm_twist_l
	//1	rp_nathan_animated_003_walking_lowerarm_twist_r
	//2	rp_nathan_animated_003_walking_upperarm_twist_l 
	//3	rp_nathan_animated_003_walking_upperarm_twist_r
	// 
	//4	rp_nathan_animated_003_walking_lowerleg_twist_l
	//5	rp_nathan_animated_003_walking_lowerleg_twist_r
	//6	rp_nathan_animated_003_walking_upperleg_twist_l
	//7	rp_nathan_animated_003_walking_upperleg_twist_r
	// 
	//8	rp_nathan_animated_003_walking_spine_02
	//9	rp_nathan_animated_003_walking_head
	AddCollider("rp_nathan_animated_003_walking_lowerarm_twist_l", 0.075f, 0.14f);
	AddCollider("rp_nathan_animated_003_walking_lowerarm_twist_r", 0.075f, 0.14f);
	AddCollider("rp_nathan_animated_003_walking_upperarm_twist_l", 0.075f, 0.11f);
	AddCollider("rp_nathan_animated_003_walking_upperarm_twist_r", 0.075f, 0.11f);

	AddCollider("rp_nathan_animated_003_walking_lowerleg_twist_l", 0.09f, 0.16f);
	AddCollider("rp_nathan_animated_003_walking_lowerleg_twist_r", 0.09f, 0.16f);
	AddCollider("rp_nathan_animated_003_walking_upperleg_twist_l", 0.09f, 0.16f);
	AddCollider("rp_nathan_animated_003_walking_upperleg_twist_r", 0.09f, 0.16f);

	AddCollider("rp_nathan_animated_003_walking_spine_02",		   0.15f, 0.25f);
	AddCollider("rp_nathan_animated_003_walking_head",			   0.1f,   0.03f);

	_uint iNumMesh = m_pSphereModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMesh; ++i)
	{
		m_pSphereModelCom->Get_Mesh(i)->Cooking_ConvexMesh();
		m_SphereGeos.push_back({ m_pSphereModelCom->Get_Mesh(i)->Get_ConvexMesh() });



		//
		//_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
		//
		//
		//PxMat44 ColliderMatrix = XMMatrixToPxMat(XMMatrixNormalize(World));
		//PxVec3 scale = {XMVector3Length(World.r[0]).m128_f32[0],XMVector3Length(World.r[1]).m128_f32[0],XMVector3Length(World.r[2]).m128_f32[0] };
		//
		//m_SphereGeos.back().scale = scale;
		//
		//PxMat44 ColliderPos = XMMatrixToPxMat(XMMatrixNormalize( World));
		//auto pDebug = GAMEINSTANCE->Get_Physics()->createRigidStatic(PxTransform{ ColliderPos });
		//
		//auto pShape = GAMEINSTANCE->Get_Physics()->createShape(m_SphereGeos.back(), *GAMEINSTANCE->Get_Physics()->createMaterial(0.5, 0.5, 0.5));
		//pDebug->attachShape(*pShape);
		//GAMEINSTANCE->Get_Scene()->addActor(*pDebug);
	}
	m_BestMatchingBoneIdx[0].resize(iNumMesh);
	m_BestMatchingBoneIdx[1].resize(iNumMesh);

	return S_OK;
}

void CWoundable::PriorityTick(_float fTimeDelta)
{
}

void CWoundable::Tick(_float fTimeDelta)
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

	for(_uint i = 0; i < 2; ++i)
	{
		
		m_WoundTransform[i] = m_pWoundTransform[i]->Get_WorldMatrix();
		_matrix Ellip = XMLoadFloat4x4(&m_WoundTransform[i]) * XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PreTransform));
	
		XMStoreFloat4x4(&m_Ellipsoid[i], Ellip);
		XMStoreFloat4x4(&m_EllipsoidInv[i], XMMatrixInverse(nullptr, Ellip));
	}
	m_pColliderCom->Update(m_pWoundTransform[0]->Get_WorldMatrix_XMMat() * m_pTransformCom->Get_WorldMatrix_XMMat());
	m_pColliderCom2->Update(m_pWoundTransform[1]->Get_WorldMatrix_XMMat()* m_pTransformCom->Get_WorldMatrix_XMMat());
	UpdateColliders();
	m_fTimer += fTimeDelta;
}

void CWoundable::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CWoundable::Render()
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

	CGame_Manager::Get_Instance()->Get_InNormal()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture_In");
	CGame_Manager::Get_Instance()->Get_InDiffuse()->Bind_ShaderResource(m_pShaderCom, "g_Texture_In");

	_matrix World = XMMatrixAffineTransformation({ 0.9f,0.9f,0.9f,0.f }, {0.f,0.f,0.f,1.f},
		XMQuaternionRotationRollPitchYaw(0.f,0.f,0.f), {0.f,0.15f,0.f,0.f})* m_pTransformCom->Get_WorldMatrix_XMMat();
	_float4x4 interWorld;
	XMStoreFloat4x4(&interWorld, World);
	//m_pShaderCom->Bind_Float("g_Dissolve", abs(sin(m_fTimer)));
	m_pShaderCom->Bind_Float("g_Dissolve", 1.f);
	m_pShaderCom->Bind_Float("fRotationSpeed", 1.f);
	m_pShaderCom->Bind_Float("fSpreadIntensity", sin(m_fTimer) * sin(m_fTimer) * 3.f);
	_float2 vUVFlow = { m_fTimer, m_fTimer };
	m_pShaderCom->Bind_RawValue("vUVFlow", &vUVFlow, sizeof(_float2));

	CGame_Manager::Get_Instance()->Get_DefaultNoise()->Bind_ShaderResource(m_pShaderCom, "g_Noise_Texture");
	CGame_Manager::Get_Instance()->Get_DefaultRGBNoise()->Bind_ShaderResource(m_pShaderCom, "g_RGBNoise_Texture");
	CGame_Manager::Get_Instance()->Get_DefaultFlow()->Bind_ShaderResource(m_pShaderCom, "g_Flow_Texture");
	m_pShaderCom->Bind_Matrix("g_EllipsoidInverse", &m_EllipsoidInv[0]);
	m_pShaderCom->Bind_Matrix("g_Ellipsoid", &m_Ellipsoid[0]);
	m_pShaderCom->Bind_Matrix("g_EllipsoidInverse2", &m_EllipsoidInv[1]);
	m_pShaderCom->Bind_Matrix("g_Ellipsoid2", &m_Ellipsoid[1]);

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

		//m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Wound_InnerStencil);
		//m_pModelCom->Render(i);

		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Wound);

		m_pModelCom->Render(i);


		//_float4 Color = { 1.f,0.f,0.f,1.f };
		//m_pShaderCom->Bind_Vector4("g_vColor", &Color);
		//
		//m_pShaderCom->Begin((_uint)ANIMMESH_PASS::MonoColor);
		//
		//m_pModelCom->Render(i);
	}
	Render_Sphere();

	
	//m_pColliderCom->Render();
	//m_pColliderCom2->Render();

	return S_OK;
}

void CWoundable::TakeDamage(const DamageInfo& Info)
{


}

void CWoundable::Sweeped(const PxSweepHit& hit, const DamageInfo& damageInfo)
{

	string name = string(hit.actor->getName());
	auto iter = find(m_ColliderBones.begin(), m_ColliderBones.end(), name);
	if (iter == m_ColliderBones.end())
		return;
	
	auto worldMat = m_pTransformCom->Get_WorldMatrix_XMMat();
	m_pBoneMatrix[m_iWoundCount % 2] = m_pModelCom->Get_BoneMatrixPtr(name.c_str());
	_matrix Bone = (XMLoadFloat4x4(m_pBoneMatrix[m_iWoundCount % 2]));
	auto InvWorldMat = XMMatrixInverse(nullptr, Bone * worldMat);
	_vector vLocalPoint = XMVector4Transform(XMVectorSet(hit.position.x, hit.position.y, hit.position.z, 1.f), InvWorldMat);
	_matrix OriBone = XMLoadFloat4x4(& m_ColliderBoneOriginals[iter - m_ColliderBones.begin()]);
	m_pOriBoneMatrix[m_iWoundCount % 2] = &m_ColliderBoneOriginals[iter - m_ColliderBones.begin()];
	//_matrix PreInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_pModelCom->Get_ModelData()->TransformMatrix));
	_vector vPreSkinnedHitPos = XMVector4Transform(vLocalPoint, OriBone);
	m_pWoundTransform[m_iWoundCount % 2]->Set_Position(vPreSkinnedHitPos);

	_matrix Wound = m_pWoundTransform[m_iWoundCount % 2]->Get_WorldMatrix_XMMat() * 
		XMMatrixScalingFromVector({ XMVector3Length(worldMat.r[0]).m128_f32[0], XMVector3Length(worldMat.r[1]).m128_f32[0] , XMVector3Length(worldMat.r[2]).m128_f32[0] });
	PxVec3 Scale = PxVec3{XMVectorGetX(XMVector3Length(Wound.r[0])), XMVectorGetX(XMVector3Length(Wound.r[1])) ,XMVectorGetX(XMVector3Length(Wound.r[2])) } * 100.f;
	PxTransform pose{ XMMatrixToPxMat(XMMatrixNormalize(Wound)) };
	_uint idx = 0;
	for (auto& Geo : m_SphereGeos)
	{
		Geo.scale = Scale;
		m_BestMatchingBoneIdx[m_iWoundCount % 2][idx] = FindBestMatchingBone(Geo, pose);
		++idx;
	}

	++m_iWoundCount;
}

void CWoundable::AddCollider(const string& szBoneName, _float fRadius, _float fHeight)
{
	auto pX = GAMEINSTANCE->Get_Physics();
	auto pScene = GAMEINSTANCE->Get_Scene();

	_float3 vScale = m_pTransformCom->Get_Scale();

	PxCapsuleGeometry CapsuleGeo{ fRadius * sqrtf(vScale.x * vScale.x * .5f + vScale.z * vScale.z * .5f), fHeight * vScale.y };

	PxMaterial* mat = pX->createMaterial(0.5f, 0.4f, 0.5f);
	PxShape* shape = pX->createShape(CapsuleGeo, *mat);
	

	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	_matrix Bone = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(szBoneName.c_str()));
	PxMat44 ColliderMatrix = XMMatrixToPxMat(XMMatrixNormalize(Bone * World));
	World.r[3] = { 0.f,0.f,0.f,1.f };
	PxMat44 ColliderPos = XMMatrixToPxMat(XMMatrixNormalize(Bone * World));

	auto pCollider = pX->createRigidDynamic(PxTransform{ ColliderMatrix });
	m_Colliders.push_back(pCollider);

	m_ColliderGeometrys.push_back({ CapsuleGeo, PxTransform{ColliderPos} });

	m_ColliderBones.push_back(szBoneName);
	m_ColliderBoneOriginals.push_back({});
	XMStoreFloat4x4(&m_ColliderBoneOriginals.back(), Bone);
	pCollider->userData = this;
	pCollider->setName(m_ColliderBones.back().c_str());
	pCollider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	pCollider->attachShape(*shape);
	pScene->addActor(*pCollider);
	

}

void CWoundable::UpdateColliders()
{
	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	for (_uint i = 0; i < m_Colliders.size(); ++i)
	{
		_matrix Bone = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(m_ColliderBones[i].c_str()));
		PxMat44 ColliderMatrix = XMMatrixToPxMat(XMMatrixNormalize(Bone * World));
		m_Colliders[i]->setKinematicTarget(PxTransform{ ColliderMatrix });
	}
}

void CWoundable::Render_Sphere()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pSphereShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return;
	if (FAILED(m_pSphereShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return;
	_uint	iNumMeshes = m_pSphereModelCom->Get_NumMeshes();


	for (_uint j = 0; j < iNumMeshes; ++j)
	{
		m_pSphereModelCom->Bind_Buffers(j);
		m_pSphereModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)0, aiTextureType_DIFFUSE);
		if (FAILED(m_pSphereModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)0, aiTextureType_SHININESS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pShaderCom, "g_ATOS_Texture");
		}
		if (FAILED(m_pSphereModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)0, aiTextureType_NORMALS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture");
		}
		_float4 color{ 1.f,0.f,0.f,1.f };
		m_pSphereShaderCom->Bind_Vector4("g_vColor",&color);
		//_matrix World = XMMatrixScaling(10000.f, 10000.f, 10000.f)
		//	* m_pTransformCom->Get_WorldMatrix_XMMat();
		//_float4x4 fWorld;
		//XMStoreFloat4x4(&fWorld, World);
		//
		//m_pSphereShaderCom->Bind_Matrix("g_WorldMatrix", &fWorld);
		//m_pSphereShaderCom->Begin((_uint)MESH_PASS::Wound);
		//m_pSphereModelCom->Render(j);
		for (_uint i = 0; i < 2; ++i)
		{
			if (m_pBoneMatrix[i])
			{
				for(auto BoneIdx : m_BestMatchingBoneIdx[i][j])
				{
					_matrix World = XMMatrixScaling(105.f, 105.f, 105.f)
						* m_pWoundTransform[i]->Get_WorldMatrix_XMMat()
						* XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ColliderBoneOriginals[BoneIdx]))
						* XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(m_ColliderBones[BoneIdx].c_str()))
						* m_pTransformCom->Get_WorldMatrix_XMMat();
					_float4x4 fWorld;
					XMStoreFloat4x4(&fWorld, World);

					m_pSphereShaderCom->Bind_Matrix("g_WorldMatrix", &fWorld);
					m_pSphereShaderCom->Begin((_uint)MESH_PASS::Wound);
					m_pSphereModelCom->Render(j);
				}
			}
		}
	}
}

vector<_uint> CWoundable::FindBestMatchingBone(PxConvexMeshGeometry& geo, PxTransform& pose)
{
	vector<_uint> candidate;
	for (_uint i = 0; i < m_ColliderGeometrys.size(); ++i)
	{
		if (PxGeometryQuery::overlap(geo, pose, m_ColliderGeometrys[i].first, m_ColliderGeometrys[i].second))
		{
			candidate.push_back(i);
		}
	}


	if (candidate.empty())
	{
		for (_uint i = 0; i < m_ColliderGeometrys.size(); ++i)
		{
			candidate.push_back(i);
		}
		_float minDist = FLT_MAX; _uint iMin = 0;
		for (_uint iCand : candidate)
		{
			_float fDist = PxGeometryQuery::pointDistance(m_ColliderGeometrys[iCand].second.p, geo, pose);
			if (fDist < minDist)
			{
				iMin = iCand;
				minDist = fDist;
			}
		}
		return { iMin };
	}
	return candidate;
}

HRESULT CWoundable::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
	m_pSphereShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_SphereShader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_SphereModel")));
	m_pSphereModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_SepSphere", TEXT("Com_Model")));



	CBounding_OBB::BOUNDING_OBB_DESC desc{};
	desc.vCenter = { 0.f,0.f,0.f };
	desc.vExtents = { 1.f,1.f,1.f };
	desc.vRotation = { 0.f,0.f,0.f };
	m_pColliderCom = static_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), &desc));
	m_pColliderCom2 = static_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider2"), &desc));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CWoundable> CWoundable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CWoundable
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CWoundable(pDevice, pContext)
		{
		}
	};


	shared_ptr<CWoundable> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWoundable"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CWoundable::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CWoundable
	{
		MakeSharedEnabler(const CWoundable& rhs) : CWoundable(rhs)
		{
		}
	};


	shared_ptr<CWoundable> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWoundable"));
		assert(false);
	}

	return pInstance;
}

void CWoundable::Free()
{
}
