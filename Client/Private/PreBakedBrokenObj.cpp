#include "PreBakedBrokenObj.h"
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
#include "StaticCollider.h"

wstring CPreBakedBrokenObj::ObjID = TEXT("CPreBakedBrokenObj");

CPreBakedBrokenObj::CPreBakedBrokenObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPreBakedBrokenObj::CPreBakedBrokenObj(const CPreBakedBrokenObj& rhs)
	: CGameObject(rhs)
{
}

CPreBakedBrokenObj::~CPreBakedBrokenObj()
{
	Free();
}

HRESULT CPreBakedBrokenObj::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Torus"), TEXT("../Bin/Resources/Models/Cube/Torus.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Torus_0"), TEXT("../Bin/Resources/Models/Cube/Torus_0.fbx"), MODEL_TYPE::ANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Cube_0"), TEXT("../Bin/Resources/Models/Cube/Cube_0.fbx"), MODEL_TYPE::ANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);
	return S_OK;
}

HRESULT CPreBakedBrokenObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (pArg)
	{
		m_strModelTag = ((FDO_DESC*)pArg)->strModelTag;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Reset_Bones();
	Read_HitInfo();
	return S_OK;
}

void CPreBakedBrokenObj::PriorityTick(_float fTimeDelta)
{
}

void CPreBakedBrokenObj::Tick(_float fTimeDelta)
{
	if(!m_bHit)
		m_pStaticCollider->Update(fTimeDelta);
	else
	{
		_float4x4 out;
		m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.01f, out);
	}
	if (GAMEINSTANCE->Get_KeyDown(DIK_O))
	{
		End_Sim();
	}
}

void CPreBakedBrokenObj::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CPreBakedBrokenObj::Render()
{
	if (m_bHit)
		return Render_Dynamic();
	else
		return Render_Static();

	return S_OK;
}

void CPreBakedBrokenObj::Simulate(const PxSweepHit& Hit, const _float3& vDir)
{
	/*if (m_bHit)
		return;*/
	m_bHit = true;
	m_pStaticCollider->Remove_Actor();
	string strTag;
	vector<pair<_float, _uint>> score;
	vector<pair<_float, _uint>> dist;
	
	auto worldMat = XMLoadFloat4x4(&m_pModelCom->Get_ModelData()->TransformMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat();
	auto InvWorldMat = XMMatrixInverse(nullptr, worldMat);

	_vector vLocalPoint = XMVector4Transform(XMVectorSet(Hit.position.x, Hit.position.y, Hit.position.z, 1.f), InvWorldMat);
	_vector vLocalNorm = XMVector3Transform(XMVectorSet(Hit.normal.x, Hit.normal.y, Hit.normal.z, 0.f), InvWorldMat);
	_vector vLocalDir = XMVector3Transform(XMLoadFloat3(&vDir), InvWorldMat);
	for (_uint i = 0; i < m_HitInfos.size(); ++i)
	{
		_vector vCanPoint = XMLoadFloat4(&m_HitInfos[i].second.vHitPoint);
		_vector vCanNorm = XMLoadFloat3(&m_HitInfos[i].second.vNormal);
		_vector vCanDir = XMLoadFloat3(&m_HitInfos[i].second.vDirection);
		
		_float fNdot = XMVectorGetX(XMVector3Dot(vLocalNorm, vCanNorm));
		_float fDdot = XMVectorGetX(XMVector3Dot(vLocalDir, vCanDir));
		score.push_back({fNdot * 0.1f + fDdot * 0.3f ,i});
		dist.push_back({ XMVectorGetX(XMVector3Length(vCanDir - vLocalPoint)), i });
	}
	sort(dist.begin(), dist.end());
	_float largeDist = dist.back().first;
	_float smallDist = dist.front().first;
	if(largeDist - smallDist > 1e-5f)
	{
		for (_uint i = 0; i < dist.size(); ++i)
		{
			score[dist[i].second].first += (largeDist - dist[i].first) / (largeDist - smallDist);
		}
	}
	sort(score.begin(), score.end());
	strTag = m_HitInfos[score.back().second].first;
	m_pModelCom->Set_AnimIndex(strTag, false);
}

void CPreBakedBrokenObj::End_Sim()
{
	m_bHit = false;
	m_pStaticCollider->Add_Actor();
}


void CPreBakedBrokenObj::Read_HitInfo()
{
	auto pMD = m_pModelCom->Get_ModelData();

	string          szBinFilePath;
	char			szDir[MAX_PATH] = "";
	char			szFileName[MAX_PATH] = "";

	_splitpath_s(pMD->szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	szBinFilePath = szDir;
	szBinFilePath += szFileName;
	szBinFilePath += ".hit";

	ifstream is(szBinFilePath, ios::binary);
	size_t nbHit = 0;
	read_typed_data(is, nbHit);

	for (size_t i = 0; i < nbHit; ++i)
	{
		size_t iStringSize = 0;
		read_typed_data(is, iStringSize);
		string szName;
		szName.resize(iStringSize);
		is.read(&szName[0],iStringSize);
		HIT hit{};
		read_typed_data(is, hit.vHitPoint);
		read_typed_data(is, hit.vDirection);
		read_typed_data(is, hit.vNormal);
		m_HitInfos.push_back({ szName, hit });
	}
}

HRESULT CPreBakedBrokenObj::Render_Static()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint	iNumMeshes = m_pSphereModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pSphereModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)0, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pSphereModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)0, aiTextureType_SHININESS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pShaderCom, "g_ATOS_Texture");
		}
		if (FAILED(m_pSphereModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)0, aiTextureType_NORMALS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture");
		}
		m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		m_pSphereModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CPreBakedBrokenObj::Render_Dynamic()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pAnimShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pAnimShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pAnimShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pAnimShaderCom, "g_BoneMatrix", 0u)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Mat_MatIdx(m_pAnimShaderCom, "g_Normal_Texture_In", (_uint)1, aiTextureType_NORMALS)))
	{
		CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pAnimShaderCom, "g_Normal_Texture_In");
	}
	m_pModelCom->Bind_Mat_MatIdx(m_pAnimShaderCom, "g_Texture_In", (_uint)1, aiTextureType_DIFFUSE);

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(m_pAnimShaderCom, "g_Texture", (_uint)0, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pAnimShaderCom, "g_ATOS_Texture", (_uint)0, aiTextureType_SHININESS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pAnimShaderCom, "g_ATOS_Texture");
		}
		if (FAILED(m_pModelCom->Bind_Material(m_pAnimShaderCom, "g_Normal_Texture", (_uint)0, aiTextureType_NORMALS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pAnimShaderCom, "g_Normal_Texture");
		}
		m_pAnimShaderCom->Begin((_uint)ANIMMESH_PASS::Destroy);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CPreBakedBrokenObj::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pAnimShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_AnimShader")));
	m_pSphereModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Torus"), TEXT("Com_SphereModel")));
	auto pSphereModelData = m_pSphereModelCom->Get_ModelData();

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_Model")));

	CPxCollider::PXCOLLIDER_DESC Desc{};
	Desc.pOwner = this;

	Desc.fStaticFriction = 0.4f;
	Desc.fDynamicFriction = 0.5f;
	Desc.fRestitution = 0.5f;
	Desc.strModelComTag = L"Com_SphereModel";
	m_pStaticCollider = static_pointer_cast<CStaticCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StaticCollider"), TEXT("Com_StaticCollider"), &Desc));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CPreBakedBrokenObj> CPreBakedBrokenObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CPreBakedBrokenObj
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPreBakedBrokenObj(pDevice, pContext)
		{
		}
	};


	shared_ptr<CPreBakedBrokenObj> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPreBakedBrokenObj"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CPreBakedBrokenObj::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CPreBakedBrokenObj
	{
		MakeSharedEnabler(const CPreBakedBrokenObj& rhs) : CPreBakedBrokenObj(rhs)
		{
		}
	};


	shared_ptr<CPreBakedBrokenObj> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPreBakedBrokenObj"));
		assert(false);
	}

	return pInstance;
}

void CPreBakedBrokenObj::Free()
{
}
