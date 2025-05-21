#include "PartDoor.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

wstring CPartDoor::ObjID = TEXT("CPartDoor");

CPartDoor::CPartDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CPartDoor::CPartDoor(const CPartDoor& rhs)
	: CPartObject{ rhs }
{
}

CPartDoor::~CPartDoor()
{
	Free();
}

HRESULT CPartDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	DOOR_PARTDESC* pDesc = static_cast<DOOR_PARTDESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_bOpen = pDesc->bOpen;
	m_eDoorDir = pDesc->eDir;
	m_fMoveDist = pDesc->fMoveDist;

	return S_OK;
}

void CPartDoor::PriorityTick(_float fTimeDelta)
{
}

void CPartDoor::Tick(_float fTimeDelta)
{
	if (m_bExecute)
	{
		if (m_bOpen)
			Close_Door(fTimeDelta);

		else
			Open_Door(fTimeDelta);
	}

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CPartDoor::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CPartDoor::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
		//	return E_FAIL;
		m_pShaderCom->Begin(0);

		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//	return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CPartDoor::Ready_Components(DOOR_PARTDESC* pDesc)
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, pDesc->strModelTag, TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CPartDoor::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

void CPartDoor::Open_Door(_float fTimeDelta)
{
	_vector vCurPos = m_pTransformCom->Get_Position();

	switch (m_eDoorDir)
	{
	case Client::CPartDoor::LEFT:
	{
		if (fabs(fabs(XMVectorGetZ(vCurPos)) - m_fMoveDist) < 0.1f)
		{
			m_bOpen = true;
			m_bExecute = false;
		}
		else
			m_pTransformCom->Go_Backward(fTimeDelta);
	
		break;
	}
	case Client::CPartDoor::RIGHT:
	{
		if (fabs(fabs(XMVectorGetZ(vCurPos)) - m_fMoveDist) < 0.1f)
		{
			m_bOpen = true;
			m_bExecute = false;
		}
		else
			m_pTransformCom->Go_Straight(fTimeDelta);
	
		break;
	}
	case Client::CPartDoor::UP:
	{
		if (fabs(fabs(XMVectorGetY(vCurPos)) - m_fMoveDist) < 0.1f)
		{
			m_bOpen = true;
			m_bExecute = false;
		}
		else
			m_pTransformCom->Go_Up(fTimeDelta);

		break;
	}
	case Client::CPartDoor::DOWN:
	{
		if (fabs(fabs(XMVectorGetY(vCurPos)) - m_fMoveDist) < 0.1f)
		{
			m_bOpen = true;
			m_bExecute = false;
		}
		else
			m_pTransformCom->Go_Down(fTimeDelta);

		break;
	}
	}
}

void CPartDoor::Close_Door(_float fTimeDelta)
{
	_vector vCurPos = m_pTransformCom->Get_Position();

	switch (m_eDoorDir)
	{
	case Client::CPartDoor::LEFT:
	{
		if (fabs(XMVectorGetZ(vCurPos)) < 0.1f)
		{
			m_bOpen = false;
			m_bExecute = false;
		}
		else
			m_pTransformCom->Go_Straight(fTimeDelta);

		break;
	}
	case Client::CPartDoor::RIGHT:
	{
		if (fabs(XMVectorGetZ(vCurPos)) < 0.1f)
		{
			m_bOpen = false;
			m_bExecute = false;
		}
		else
			m_pTransformCom->Go_Backward(fTimeDelta);

		break;
	}
	case Client::CPartDoor::UP:
	{
		if (fabs(XMVectorGetY(vCurPos)) < 0.1f)
		{
			m_bOpen = false;
			m_bExecute = false;
		}
		else
			m_pTransformCom->Go_Down(fTimeDelta);

		break;
	}
	case Client::CPartDoor::DOWN:
	{
		if (fabs(XMVectorGetY(vCurPos)) < 0.1f)
		{
			m_bOpen = false;
			m_bExecute = false;
		}
		else
			m_pTransformCom->Go_Up(fTimeDelta);

		break;
	}
	}
}

shared_ptr<CPartDoor> CPartDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CPartDoor
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartDoor(pDevice, pContext)
		{}
	};

	shared_ptr<CPartDoor> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPartDoor"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CPartDoor::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CPartDoor
	{
		MakeSharedEnabler(const CPartDoor& rhs) : CPartDoor(rhs)
		{}
	};


	shared_ptr<CPartDoor> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPartDoor"));
		assert(false);
	}

	return pInstance;
}

void CPartDoor::Free()
{
}
