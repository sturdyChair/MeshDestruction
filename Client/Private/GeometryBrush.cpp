#include "GeometryBrush.h"
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
#include "Imgui_Manager.h"
#include "Math_Manager.h"

wstring CGeometryBrush::ObjID = TEXT("CGeometryBrush");

CGeometryBrush::CGeometryBrush(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CGeometryBrush::CGeometryBrush(const CGeometryBrush& rhs)
	: CGameObject(rhs)
{
}

CGeometryBrush::~CGeometryBrush()
{
	Free();
}

HRESULT CGeometryBrush::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Cube"), TEXT("../Bin/Resources/Models/Cube/Cube.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																					   0.f,	 0.01f, 0.f,  0.f,
																																					   0.f,  0.f,  0.01f, 0.f,
																																					   0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Sphere"), TEXT("../Bin/Resources/Models/Cube/Sphere.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Monkey"), TEXT("../Bin/Resources/Models/Cube/Monkey.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Cube_Cut"), TEXT("../Bin/Resources/Models/Cube/Cube_Cut.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Torus"), TEXT("../Bin/Resources/Models/Cube/Torus.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);


	return S_OK;
}

HRESULT CGeometryBrush::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (pArg)
	{
		m_strModelTag = ((FDO_DESC*)pArg)->strModelTag;

	}

	if (FAILED(Ready_Components()))
		return E_FAIL;
	return S_OK;
}

void CGeometryBrush::PriorityTick(_float fTimeDelta)
{
}

void CGeometryBrush::Tick(_float fTimeDelta)
{
	if (GAMEINSTANCE->Get_KeyPressing(DIK_LCONTROL) && GAMEINSTANCE->Get_KeyDown(DIK_S))
	{
		Bake_Binary();
	}
}

void CGeometryBrush::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CGeometryBrush::Render()
{
	return Render_Static();
}

void CGeometryBrush::Set_Model_Name(const string& strName)
{
	m_pModelCom->Get_ModelData()->szModelFileName = strName;
}

void CGeometryBrush::Bake_Binary()
{
	auto pMD = m_pModelCom->Get_ModelData();
	pMD->iNumAnimations = pMD->Animation_Datas.size();
	pMD->iNumMeshs = pMD->Mesh_Datas.size();
	pMD->iNumMaterials = pMD->Material_Datas.size();
	pMD->eModelType = MODEL_TYPE::NONANIM;
	pMD->Bake_Binary();

}

void CGeometryBrush::Apply_Inner_Texture(const string& szFilename, shared_ptr<CTexture> pTexture, aiTextureType eType)
{
	if (!m_pModelCom)
		return;
	m_pModelCom->Get_ModelData()->Material_Datas[1]->szTextureName[eType] = szFilename;
	m_pModelCom->Chander_Material(1, pTexture, eType);
}

void CGeometryBrush::Operation(shared_ptr<CGeometryBrush> rhs, CSG_OP eOperation)
{
	auto lhsMesh = CCSG_Manager::ConvertMeshData(*m_pModelCom->Get_ModelData()->Mesh_Datas[m_iCurrMesh], 0.f);
	auto rhsMesh = CCSG_Manager::ConvertMeshData(*rhs->m_pModelCom->Get_ModelData()->Mesh_Datas[rhs->m_iCurrMesh], 1.f);
	_matrix lhsWorld = m_pTransformCom->Get_WorldMatrix_XMMat();
	_matrix rhsWorld = rhs->m_pTransformCom->Get_WorldMatrix_XMMat();
	lhsWorld.r[3] *= 100.f;
	rhsWorld.r[3] *= 100.f;
	CCSG_Manager::CSG_ApplyMatrix(rhsMesh, rhsWorld * XMMatrixInverse(nullptr, lhsWorld));
	CSG_Mesh result;
	switch (eOperation)
	{
	case Client::CGeometryBrush::UNION:
		result = CCSG_Manager::CSG_Union(lhsMesh, rhsMesh);
		break;
	case Client::CGeometryBrush::INTERSECT:
		result = CCSG_Manager::CSG_Intersect(lhsMesh, rhsMesh);
		break;
	case Client::CGeometryBrush::DIFF:
		result = CCSG_Manager::CSG_Subtract(lhsMesh, rhsMesh);
		break;
	case Client::CGeometryBrush::OP_END:
		return;
		break;
	default:
		break;
	}
	auto meshdata = CCSG_Manager::RevertMeshData(result, false);
	m_pModelCom->Push_Back_Mesh(meshdata);
	m_iCurrMesh = m_pModelCom->Get_NumMeshes() - 1;
}

HRESULT CGeometryBrush::Render_Static()
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

	m_pShaderCom->Begin((_uint)MESH_PASS::Default);
	
	m_pModelCom->Render(m_iCurrMesh);
	
	return S_OK;
}




HRESULT CGeometryBrush::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_BModel")));



	if (!m_pShaderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CGeometryBrush> CGeometryBrush::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CGeometryBrush
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGeometryBrush(pDevice, pContext)
		{
		}
	};


	shared_ptr<CGeometryBrush> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CGeometryBrush"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CGeometryBrush::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CGeometryBrush
	{
		MakeSharedEnabler(const CGeometryBrush& rhs) : CGeometryBrush(rhs)
		{
		}
	};


	shared_ptr<CGeometryBrush> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CGeometryBrush"));
		assert(false);
	}

	return pInstance;
}

void CGeometryBrush::Free()
{
}
