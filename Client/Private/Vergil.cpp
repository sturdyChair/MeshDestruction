#include "Vergil.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Player_Camera.h"
wstring CVergil::ObjID = TEXT("CVergil");

CVergil::CVergil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CVergil::CVergil(const CVergil& rhs)
    : CGameObject(rhs)
{
}

CVergil::~CVergil()
{
    Free();
}

HRESULT CVergil::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Vergil"), TEXT("../Bin/Resources/Models/Player/Chai/chai_move.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );


    return S_OK;
}

HRESULT CVergil::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC         TransformDesc{};
    TransformDesc.fSpeedPerSec = 1.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
    _float4 vPos = _float4(0.f, 0.f, -0.f, 1.f);
    m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

    vector<shared_ptr<CAnimation>> animList =  m_pModelCom->Get_Animation_List();
    for (auto& iter : animList)
    {
        m_vecAnimList.push_back(iter->Get_AnimName());
    }
    m_pModelCom->Set_AnimIndex(0, true);

    return S_OK;
}

void CVergil::PriorityTick(_float fTimeDelta)
{
}

void CVergil::Tick(_float fTimeDelta)
{
    Animation_End();
    Animation(fTimeDelta);



    Move(fTimeDelta);
    Attack(fTimeDelta);


    /* 플레이어에게 카메라 고정 */
    shared_ptr <CTransform> camTransform =  GAMEINSTANCE->Get_MainCamera()->Get_Transform();
    camTransform->LookAt(m_pTransformCom->Get_Position());
}

void CVergil::LateTick(_float fTimeDelta)
{
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CVergil::Render()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
    //   return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;
    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SPECULAR)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        //if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
        //   return E_FAIL;

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

HRESULT CVergil::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Vergil"), TEXT("Com_Model")));

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

shared_ptr<CVergil> CVergil::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CVergil
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CVergil(pDevice, pContext)
        {}
    };


    shared_ptr<CVergil> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CVergil"));
        assert(false);
    }

    return pInstance;
}


shared_ptr<CGameObject> CVergil::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CVergil
    {
        MakeSharedEnabler(const CVergil& rhs) : CVergil(rhs)
        {}
    };


    shared_ptr<CVergil> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CVergil"));
        assert(false);
    }

    return pInstance;
}

void CVergil::Free()
{
}

void CVergil::Animation_End()
{
    if (m_pModelCom->Get_StringAnimIndex("pl0800_Run_Start (29 frames) ID 1010") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_pModelCom->Set_AnimIndex("pl0800_Run_Loop (34 frames) ID 1000", true);
    }
    if (m_pModelCom->Get_StringAnimIndex("pl0800_Run_Stop (88 frames) ID 1020") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_pModelCom->Set_AnimIndex("pl0800_Idle_Narrow (105 frames) ID 20", true);
    }
}

void CVergil::Animation(_float fTimeDelta)
{
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);
    //m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta * 60.f, fmatResult);
    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    m_pTransformCom->Set_Position(vCurPos);
}

void CVergil::Move(_float fTimeDelta)
{    
#pragma region Run
    ///* RUN */
    //_vector playerLookVec = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0);
    //_vector camLookVec = XMVectorSetY(GAMEINSTANCE->Get_CamLook_Vector(), 0);
    //;
    ///*    if (XMVectorGetX(XMVector3Dot(XMVector3Normalize(playerLookVec), XMVector3Normalize(camLookVec)))< 0.f)
    //{
    //    m_eMoveState = VIRGIL_MOVE_STATE::RUN;
    //    m_pTransformCom->LookAt(m_pTransformCom->Get_Position() + XMVector3Normalize(XMVectorSetY(GAMEINSTANCE->Get_CamLook_Vector(), 0)) * 5000.f);
    //    m_pModelCom->Set_AnimIndexNonCancle("pl0800_Run_Turn_180 (26 frames) ID: 1030", false);
    //}
    //else*/ if (GAMEINSTANCE->Get_KeyDown(DIK_W) && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("pl0800_Run_Loop (34 frames) ID 1000"))
    //{
    //    m_eMoveState = VIRGIL_MOVE_STATE::RUN;
    //    m_pTransformCom->LookAt(m_pTransformCom->Get_Position() + XMVector3Normalize(XMVectorSetY(GAMEINSTANCE->Get_CamLook_Vector(), 0)) * 5000.f);
    //    m_pModelCom->Set_AnimIndexNonCancle("pl0800_Run_Start (29 frames) ID 1010", false);
    //}
    //else if (GAMEINSTANCE->Get_KeyPressing(DIK_W) && m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("pl0800_Run_Loop (34 frames) ID 1000"))
    //{
    //    m_pTransformCom->LookAt(m_pTransformCom->Get_Position() + XMVector3Normalize(XMVectorSetY(GAMEINSTANCE->Get_CamLook_Vector(), 0)) * 5000.f);
    //    m_pModelCom->Set_AnimIndexNonCancle("pl0800_Run_Loop (34 frames) ID 1000", true);
    //}
    //else if (GAMEINSTANCE->Get_KeyNone(DIK_W) && GAMEINSTANCE->Get_KeyNone(DIK_S) && GAMEINSTANCE->Get_KeyNone(DIK_A) && GAMEINSTANCE->Get_KeyNone(DIK_D)
    //    && (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("pl0800_Run_Start (29 frames) ID 1010") ||
    //        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("pl0800_Run_Loop (34 frames) ID 1000")))
    //{
    //    m_pModelCom->Set_AnimIndexNonCancle("pl0800_Jog_Stop (94 frames) ID 2520", false);
    //}


    //if (m_pModelCom->Get_StringAnimIndex("pl0800_Run_Start (29 frames) ID 1010") == m_pModelCom->Get_CurrentAnimIndex())
    //{
    //    m_pTransformCom->LookAt(m_pTransformCom->Get_Position() + XMVector3Normalize(XMVectorSetY(GAMEINSTANCE->Get_CamLook_Vector(), 0)) * 5000.f);
    //}
#pragma endregion 
}

void CVergil::Attack(_float fTimeDelta)
{
    //if (GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON))
    //{
    //    m_pModelCom->Set_AnimIndexNonCancle("pl0800_Jog_Stop (94 frames) ID 2520", false);
    //}
}
