#include "ProdDeptDoor.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "PartDoor.h"
#include "DoorTrigger.h"

wstring CProdDeptDoor::ObjID = TEXT("CProdDeptDoor");

CProdDeptDoor::CProdDeptDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CProdDeptDoor::CProdDeptDoor(const CProdDeptDoor& rhs)
    : CGameObject{rhs}
{
}

CProdDeptDoor::~CProdDeptDoor()
{
    Free();
}

HRESULT CProdDeptDoor::Initialize_Prototype()
{
    _float4x4 fMat = {};
    //XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_ProdDeptDoorFrame"), TEXT("../Bin/Resources/Models/Map/AnimObject/SK_bg1110.fbx"), MODEL_TYPE::ANIM, fMat)
    );

    return S_OK;
}

HRESULT CProdDeptDoor::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Parts()))
        return E_FAIL;

    //m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
    //m_pTransformCom->Rotation_Quaternion(XMVectorSet(XMConvertToRadians(90.f), XMConvertToRadians(90.f), 0.f, 0.f));
    
    m_pModelCom->Set_AnimIndex(1, true);

    return S_OK;
}

void CProdDeptDoor::PriorityTick(_float fTimeDelta)
{
}

void CProdDeptDoor::Tick(_float fTimeDelta)
{
    if (GAMEINSTANCE->Get_KeyDown(DIK_H))
    {
        static_pointer_cast<CPartDoor>(m_vecParts[DOOR_L])->Execute_Door(true);
        static_pointer_cast<CPartDoor>(m_vecParts[DOOR_R])->Execute_Door(true);
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

    _float4x4 fMat = {};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fMat);
}

void CProdDeptDoor::LateTick(_float fTimeDelta)
{
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CProdDeptDoor::Render()
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

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

        m_pShaderCom->Begin(0);

        if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
        	return E_FAIL;

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

HRESULT CProdDeptDoor::Ready_Parts()
{
    //CPartDoorFrame::FRAME_PARTDESC frameDesc{};
    //frameDesc.fRotationPerSec = { 0.f };
    //frameDesc.fSpeedPerSec = { 0.f };
    //frameDesc.pParentTransform = m_pTransformCom;
    //frameDesc.strModelTag = TEXT("SM_ProdDeptDoorFrameA_MW_01");
    //frameDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    //shared_ptr<CGameObject> pFrame = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CPartDoorFrame::ObjID, &frameDesc);
    //pFrame->Get_Transform()->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //m_vecParts.emplace_back(static_pointer_cast<CPartObject>(pFrame));

    CPartDoor::DOOR_PARTDESC doorDesc{};
    doorDesc.fRotationPerSec = { 0.f };
    doorDesc.fSpeedPerSec = { 10.f };
    doorDesc.pParentTransform = m_pTransformCom;
    doorDesc.strModelTag = TEXT("SM_ProdDeptDoorA_DoorL_MW_01");
    doorDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    doorDesc.bOpen = false;
    doorDesc.eDir = CPartDoor::DOOR_DIR::LEFT;
    doorDesc.fMoveDist = 5.f;
    shared_ptr<CGameObject> pDoorL = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CPartDoor::ObjID, &doorDesc);
    pDoorL->Get_Transform()->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //pDoorL->Get_Transform()->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
    m_vecParts.emplace_back(static_pointer_cast<CPartObject>(pDoorL));

    doorDesc.fSpeedPerSec = { 10.f };
    doorDesc.strModelTag = TEXT("SM_ProdDeptDoorA_DoorR_MW_01");
    doorDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    doorDesc.bOpen = false;
    doorDesc.eDir = CPartDoor::DOOR_DIR::RIGHT;
    doorDesc.fMoveDist = 5.f;
    shared_ptr<CGameObject> pDoorR = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CPartDoor::ObjID, &doorDesc);
    pDoorR->Get_Transform()->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //pDoorR->Get_Transform()->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
    m_vecParts.emplace_back(static_pointer_cast<CPartObject>(pDoorR));

    //CDoorTrigger::DOORTRIGGER_DESC triggerDesc{};
    //triggerDesc.pDoorObject = shared_from_this();
    //shared_ptr<CGameObject> pTrigger = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CDoorTrigger::ObjID, &triggerDesc);
    //m_pDoorTrigger = static_pointer_cast<CDoorTrigger>(pTrigger);

    //shared_ptr<CGameObject> pDoorFrame = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CProdDeptDoorFrame::ObjID);
    //m_vecParts.

    return S_OK;
}

HRESULT CProdDeptDoor::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ProdDeptDoorFrame"), TEXT("Model_Com")));

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
    aabbDesc.vCenter = {};
    aabbDesc.vExtents = { 1.f, 1.f, 1.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));

    if (/*!m_pShaderCom || !m_pModelCom ||*/ !m_pColliderCom)
        assert(false);

    return S_OK;
}

HRESULT CProdDeptDoor::Bind_ShaderResources()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
    //    return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CProdDeptDoor> CProdDeptDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CProdDeptDoor
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CProdDeptDoor(pDevice, pContext)
        {}
    };

    shared_ptr<CProdDeptDoor> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProdDeptDoor"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CProdDeptDoor::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CProdDeptDoor
    {
        MakeSharedEnabler(const CProdDeptDoor& rhs) : CProdDeptDoor(rhs)
        {}
    };

    shared_ptr<CProdDeptDoor> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProdDeptDoor"));
        assert(false);
    }

    return pInstance;
}

void CProdDeptDoor::Free()
{
}
