#include "PartDoorFrame.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

_wstring CPartDoorFrame::ObjID = TEXT("CPartDoorFrame");

CPartDoorFrame::CPartDoorFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{pDevice, pContext}
{
}

CPartDoorFrame::CPartDoorFrame(const CPartDoorFrame& rhs)
    : CPartObject{rhs}
{
}

CPartDoorFrame::~CPartDoorFrame()
{
    Free();
}

HRESULT CPartDoorFrame::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPartDoorFrame::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    FRAME_PARTDESC* pDesc = static_cast<FRAME_PARTDESC*>(pArg);

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CPartDoorFrame::PriorityTick(_float fTimeDelta)
{
}

void CPartDoorFrame::Tick(_float fTimeDelta)
{
    SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CPartDoorFrame::LateTick(_float fTimeDelta)
{
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CPartDoorFrame::Render()
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

HRESULT CPartDoorFrame::Ready_Components(FRAME_PARTDESC* pDesc)
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, pDesc->strModelTag, TEXT("Com_Model")));

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

HRESULT CPartDoorFrame::Bind_ShaderResources()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    //if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
    //    return E_FAIL;
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

shared_ptr<CPartDoorFrame> CPartDoorFrame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CPartDoorFrame
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartDoorFrame(pDevice, pContext)
        {}
    };

    shared_ptr<CPartDoorFrame> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPartDoorFrame"));
        assert(false);
    }

    return pInstance;
}


shared_ptr<CGameObject> CPartDoorFrame::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CPartDoorFrame
    {
        MakeSharedEnabler(const CPartDoorFrame& rhs) : CPartDoorFrame(rhs)
        {}
    };

    shared_ptr<CPartDoorFrame> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CPartDoorFrame"));
        assert(false);
    }

    return pInstance;
}

void CPartDoorFrame::Free()
{
}
