#include "TestModel.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"

wstring CTestModel::ObjID = TEXT("CTestModel");

CTestModel::CTestModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTestModel::CTestModel(const CTestModel& rhs)
	: CGameObject(rhs)
{
}

CTestModel::~CTestModel()
{
	Free();
}

HRESULT CTestModel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Test"), TEXT("../Bin/Resources/Models/Test/katana.fbx"), MODEL_TYPE::NONANIM, _float4x4{0.1f, 0.f,  0.f,  0.f,
																																					   0.f,	 0.1f, 0.f,  0.f,
																																					   0.f,  0.f,  0.1f, 0.f,
																																					   0.f,  0.f,  0.f,  1.f})
	);

	return S_OK;
}

HRESULT CTestModel::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_pTransformCom->Set_Scale(XMVectorSet(1000.f, 1000.f, 1000.f, 0.f));
	_float4 vPos = _float4(60.f, 20.f, -30.f, 1.f);
	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	return S_OK;
}

void CTestModel::PriorityTick(_float fTimeDelta)
{
}

void CTestModel::Tick(_float fTimeDelta)
{


}

void CTestModel::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_DISTORTION, shared_from_this());
	//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
}

HRESULT CTestModel::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
	m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));
	m_pShaderCom->Bind_Float("g_fNoiseStrength", GAMEINSTANCE->Random_Float(0.02f,0.04f));
	m_pShaderCom->Bind_Float("g_fTime", GAMEINSTANCE->Random_Float(0.f, 1.f));

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SHININESS)))
		//	return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
		//	return E_FAIL;
		if (FAILED(m_pGameInstance.lock()->Bind_RT_SRV(L"RT_BeforeBack", m_pShaderCom, "g_Texture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Noise_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		//m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		m_pShaderCom->Begin((_uint)MESH_PASS::Distortion);

		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//	return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CTestModel::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));


	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Test"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CTestModel> CTestModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CTestModel
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTestModel(pDevice, pContext)
		{}
	};


	shared_ptr<CTestModel> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTestModel"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CTestModel::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CTestModel
	{
		MakeSharedEnabler(const CTestModel& rhs) : CTestModel(rhs)
		{}
	};


	shared_ptr<CTestModel> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTestModel"));
		assert(false);
	}

	return pInstance;
}

void CTestModel::Free()
{
}
