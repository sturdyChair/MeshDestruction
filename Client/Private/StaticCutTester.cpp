#include "StaticCutTester.h"
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

wstring CStaticCutTester::ObjID = TEXT("CStaticCutTester");

CStaticCutTester::CStaticCutTester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CStaticCutTester::CStaticCutTester(const CStaticCutTester& rhs)
	: CGameObject(rhs)
{
}

CStaticCutTester::~CStaticCutTester()
{
	Free();
}

HRESULT CStaticCutTester::Initialize_Prototype()
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
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Nathan_Mani"), TEXT("../Bin/Resources/Models/Nathan/Nathan_Mani.fbx"), MODEL_TYPE::ANIM, PreTransform));

	XMStoreFloat4x4(&PreTransform,
		XMMatrixAffineTransformation(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
			XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f),
			XMVectorSet(0.f, 0.f, 0.f, 1.f)));
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Spider"), TEXT("../Bin/Resources/Models/Spider/Spider.fbx"), MODEL_TYPE::ANIM, PreTransform));




	return S_OK;
}

HRESULT CStaticCutTester::Initialize(void* pArg)
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

	m_Transforms = vector(m_pModelCom->Get_NumMeshes(), Identity);
	m_TransformsTarget = vector(m_pModelCom->Get_NumMeshes(), _float3{0.f,0.f,1.f});
	m_PreTransform = m_pModelCom->Get_ModelData()->TransformMatrix;

	return S_OK;
}

void CStaticCutTester::PriorityTick(_float fTimeDelta)
{
	//if (GAMEINSTANCE->Get_KeyDown(DIK_F))
	//{
	//	for (_uint i = 0; i < 3; ++i)
	//	{
	//		_float4 vPlane;
	//		_vector vPos = m_pTransformCom->Get_Position();
	//		_vector vNormal = { cos((XM_PI * 0.1f +XM_PI * 0.3f * i)), sin((XM_PI * 0.1f + XM_PI * 0.3f * i)), 0.f , 0.f };
	//		XMStoreFloat4(&vPlane, XMPlaneFromPointNormal(vPos + _vector{ 0.f,15.f,0.f,0.f }, vNormal));
	//		Cut(vPlane);
	//	}
	//	for (_uint i = 0; i < 4; ++i)
	//	{
	//		_float4 vPlane;
	//		_vector vPos = m_pTransformCom->Get_Position();
	//		XMStoreFloat4(&vPlane, XMPlaneFromPointNormal(vPos + _vector{0.f,5.f + 3.f * i,0.f,0.f }, { 0.f, 1.f, 0.f ,0.f }));
	//		Cut(vPlane);
	//	}

	//}
}

void CStaticCutTester::Tick(_float fTimeDelta)
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
		for (auto& t : m_LifeTimer)
		{
			t += fTimeDelta;
		}
		if(!m_bFollowPhysx)
			m_fLT += fTimeDelta;
		m_bOver = true;
		for (auto& i : m_CutCount)
		{
			if (m_fLT < m_fSliceLifeTime && i < m_iMaxCutCount)
				m_bOver = false;
		}
		


		if (m_bOver)
		{
			for (auto& i : m_CutCount)
			{
				i = 0;
			}
			for (auto& t : m_LifeTimer)
			{
				t = m_fSliceLifeTime + GAMEINSTANCE->Random_Float(-0.5f, 0.5f);
			}
			m_fLT = 0.f;
			m_bFollowPhysx = true;
			_uint iNumMesh = m_Transforms.size();
			m_pxActors.resize(iNumMesh);
			_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
			_matrix PreTrans = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PreTransform));
			auto pX = GAMEINSTANCE->Get_Physics();
			auto pScene = GAMEINSTANCE->Get_Scene();
			for (_uint i = 0; i < iNumMesh; ++i)
			{
				auto pMesh = m_pModelCom->Get_Mesh(i);
				_matrix W;
				W = PreTrans * XMLoadFloat4x4(&m_Transforms[i]) * World;
				_vector scale, rot, trans;
				XMMatrixDecompose(&scale, &rot, &trans, W);
				W = XMMatrixNormalize(W);
				m_pxActors[i] = pX->createRigidDynamic(PxTransform{ XMMatrixToPxMat(W) });

				pMesh->Cooking_ConvexMesh({scale.m128_f32[0],scale.m128_f32[1],scale.m128_f32[2] });
				auto pConvexMesh = pMesh->Get_ConvexMesh();

				PxMaterial* mat = pX->createMaterial(0.4f, 0.5f, 0.5f);
				PxConvexMeshGeometry Geo(pConvexMesh);//, PxVec3{ scale.m128_f32[0], scale.m128_f32[1], scale.m128_f32[2] });
				
				auto pShape = pX->createShape(Geo , *mat);
				
				m_pxActors[i]->attachShape(*pShape);
				m_pxActors[i]->setMass(1.f);
				PxRigidBodyExt::updateMassAndInertia(*m_pxActors[i], 0.5f);
				pScene->addActor(*m_pxActors[i]);
			}
		}

		if (!m_bFollowPhysx)
		{
			_uint iSize = m_Transforms.size();

			for (_uint i = 0; i < iSize; ++i)
			{
				_vector curPos = { m_Transforms[i]._41,m_Transforms[i]._42 ,m_Transforms[i]._43, 1.f };
				_vector TargetVec = XMLoadFloat3(&m_TransformsTarget[i]);
	
				curPos += XMVector3Normalize(TargetVec) * fTimeDelta * m_fApart;
				
				m_Transforms[i]._41 = curPos.m128_f32[0];
				m_Transforms[i]._42 = curPos.m128_f32[1];
				m_Transforms[i]._43 = curPos.m128_f32[2];
			}
		}
		else
		{
			_matrix Pre = XMMatrixNormalize(XMLoadFloat4x4(&m_PreTransform));
			_matrix WorldI = XMMatrixInverse(nullptr, ( m_pTransformCom->Get_WorldMatrix_XMMat()));
			_uint iSize = m_pxActors.size();
			for (_uint i = 0; i < iSize; ++i)
			{
				auto pxMat = PxMat44(m_pxActors[i]->getGlobalPose());
				XMStoreFloat4x4(&m_Transforms[i], XMMatrixNormalize( Pre * PxMatToXMMatrix(pxMat) * WorldI));

			}
			_uint iNumMesh = m_LifeTimer.size();
			_bool bDead = true;
			for (_uint i = 0; i < iNumMesh; ++i)
			{
				if (m_LifeTimer[i] - m_fSliceLifeTime * 3.f < 1.f)
				{
					bDead = false;
					break;
				}
			}
			if (bDead)
			{
				Dead();
				return;
			}
			
		}

	}
}

void CStaticCutTester::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CStaticCutTester::Render()
{
	if (!m_bCut)
		Render_Dynamic();
	else
		Render_Static();
	return S_OK;
}

void CStaticCutTester::Cut(const _float4& plane)
{
	if (m_bFollowPhysx)
		return;
	_uint iNumMesh = m_pModelCom->Get_NumMeshes();
	if(!m_bCut)
	{
		for (_uint i = 0; i < iNumMesh; ++i)
		{
			CSnapper::MODEL_SNAP_DESC msDesc{};
			msDesc.iMeshIdx = i;
			msDesc.pModel = m_pModelCom;
			auto pSnapper = static_pointer_cast<CSnapper>(m_pGameInstance.lock()->Clone_Component(LEVEL_STATIC, CSnapper::s_PrototypeTag, &msDesc));
			vector<VTXMESH> snap_mesh = pSnapper->Snap();
			shared_ptr<MESH_DATA> md = m_pModelCom->Reconstruct_MeshData(i, snap_mesh, {});
			m_pModelCom->Set_ModelType(MODEL_TYPE::NONANIM);
			m_pModelCom->Replace_Mesh(md, i);
			m_LifeTimer.push_back(0.f);
			m_CutCount.push_back(0);
		}
	}
	CStatic_Cutter::MODEL_CUTTER_DESC desc{};
	desc.pModel = m_pModelCom;
	_vector vPlane = XMLoadFloat4(&plane);
	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	_matrix PreT = m_bCut ? XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PreTransform)) : XMMatrixIdentity();
	for (_uint i = 0; i < iNumMesh; ++i)
	{
		if (m_CutCount[i] >= m_iMaxCutCount || m_LifeTimer[i] >= m_fSliceLifeTime)
			continue;
		_float4 LocalPlane;
		_matrix Offset = XMLoadFloat4x4(&m_Transforms[i]);

		XMStoreFloat4(&LocalPlane, XMPlaneNormalize(XMPlaneTransform(vPlane,
			XMMatrixTranspose(/*PreT **/ Offset * World)
		)));

		desc.iMeshIdx = i;
		try
		{
			auto pCutter = static_pointer_cast<CStatic_Cutter>(m_pGameInstance.lock()->Clone_Component(LEVEL_STATIC, CStatic_Cutter::s_PrototypeTag, &desc));
			vector<VTXMESH> newVertices;
			auto [up, down] = pCutter->Cut(LocalPlane, newVertices);
			
			if (up.size() && down.size())
			{
				if(newVertices.size())
				{
					vector<_float3> partDesc;
					for (_uint j = 0; j < newVertices.size(); j += (newVertices.size() + m_iSlicingEffectCount - 1)/ m_iSlicingEffectCount)
					{
						_float3 vPos = newVertices[j].vPosition;
						_float4x4 particleWorld;
						XMStoreFloat4x4(&particleWorld, PreT * XMMatrixTranslation(vPos.x,vPos.y,vPos.z) * World );
						partDesc.push_back({ particleWorld._41,particleWorld._42,particleWorld._43 });
					}

					partDesc.push_back({m_Transforms[i]._41,m_Transforms[i]._42,m_Transforms[i]._43 });
					GAMEINSTANCE->Push_Object_From_Pool(CMyParticle::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"Layer_Projectile",
						&partDesc);
				}

				m_pModelCom->Divide(i, newVertices, up, down, LocalPlane);
				m_Transforms.push_back(m_Transforms[i]);
				m_TransformsTarget.push_back({ -plane.x,-plane.y,-plane.z });
				m_LifeTimer.push_back(m_LifeTimer[i] * 0.7f );
				m_CutCount.push_back(m_CutCount[i] + 1);
				m_CutCount[i] += 1;
				m_TransformsTarget[i] = { plane.x,plane.y,plane.z };
			
			}
		}
		catch (HRESULT e)
		{
			m_bCut = true;
			return;
		}
	}

	m_bCut = true;

}

HRESULT CStaticCutTester::Render_Static()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	if (FAILED(m_pStaticShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pStaticShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pStaticShaderCom, "g_Normal_Texture_In");
	CGame_Manager::Get_Instance()->Get_InDiffuse()->Bind_ShaderResource(m_pStaticShaderCom, "g_Texture_In");

	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	_matrix PreTrans = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PreTransform));
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_float4x4 W;
		XMStoreFloat4x4(&W, PreTrans * XMLoadFloat4x4(&m_Transforms[i]) * World);
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
		m_pStaticShaderCom->Bind_Float("g_Dissolve", (m_LifeTimer[i] - m_fSliceLifeTime * 3.f));
		m_pStaticShaderCom->Bind_Float("g_DissolveRange", 0.2f);
		m_pStaticShaderCom->Begin((_uint)MESH_PASS::SliceDissolve);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CStaticCutTester::Render_Dynamic()
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

	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
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





HRESULT CStaticCutTester::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
	m_pStaticShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_StaticShader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_SphereModel")));


	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CStaticCutTester> CStaticCutTester::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CStaticCutTester
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CStaticCutTester(pDevice, pContext)
		{
		}
	};


	shared_ptr<CStaticCutTester> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CStaticCutTester"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CStaticCutTester::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CStaticCutTester
	{
		MakeSharedEnabler(const CStaticCutTester& rhs) : CStaticCutTester(rhs)
		{
		}
	};


	shared_ptr<CStaticCutTester> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CStaticCutTester"));
		assert(false);
	}

	return pInstance;
}

void CStaticCutTester::Free()
{
	for (auto pActor : m_pxActors)
	{
		if (pActor && pActor->isReleasable())
		{
			pActor->release();
		}
	}
}
