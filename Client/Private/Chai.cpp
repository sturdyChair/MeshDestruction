#include "Chai.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "CharacterController.h"
#include "Player_Camera.h"
#include "Basic_Guitar.h"

#include "Collider.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "CombatManager.h"
#include "Beat_Manager.h"
#include "CUi_Mgr.h"
#include "CUi_BeatMarkerSystem.h"
wstring CChai::ObjID = TEXT("CChai");

CChai::CChai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CChai::CChai(const CChai& rhs)
    : CGameObject(rhs)
{
}

CChai::~CChai()
{
    Free();
}

HRESULT CChai::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Chai"), TEXT("../Bin/Resources/Models/Player/Chai/Chai.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    return S_OK;
}

HRESULT CChai::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC         TransformDesc{};
    TransformDesc.fSpeedPerSec = 4.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
    _float4 vPos = _float4(0.f, 0.f, -0.f, 1.f);
    m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

    vector<shared_ptr<CAnimation>> animList = m_pModelCom->Get_Animation_List();
    for (auto& iter : animList)
    {
        m_vecAnimList.push_back(iter->Get_AnimName());
    }
    m_pModelCom->Set_AnimIndex("ch0000_idle_000", true);

    m_pCCT->Disable_Gravity();
    m_pCCT->Disable_Pseudo_Physics();

    Create_Attack_Tree();
    PrepareCallbacks();
    return S_OK;
}

void CChai::PriorityTick(_float fTimeDelta)
{
}

void CChai::Tick(_float fTimeDelta)
{



    if (!Attack(fTimeDelta) && m_eMoveState != CHAI_MOVE_STATE::DAMAGED)
        Move(fTimeDelta);
    Animation_End();
    Animation(fTimeDelta);
    m_pCCT->Update(fTimeDelta);
    /* 플레이어에게 카메라 고정 */
    shared_ptr <CTransform> camTransform = GAMEINSTANCE->Get_MainCamera()->Get_Transform();
    camTransform->LookAt(m_pTransformCom->Get_Position());

    // Update Colliders
    auto matrixSpine = XMLoadFloat4x4(m_pMat_Spine);
    matrixSpine.r[0] = XMVector3Normalize(matrixSpine.r[0]);
    matrixSpine.r[1] = XMVector3Normalize(matrixSpine.r[1]);
    matrixSpine.r[2] = XMVector3Normalize(matrixSpine.r[2]);
    m_pCollider_Hit->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());

    auto matrixRightHand = XMLoadFloat4x4(m_pMat_RightHand);
    matrixRightHand.r[0] = XMVector3Normalize(matrixRightHand.r[0]);
    matrixRightHand.r[1] = XMVector3Normalize(matrixRightHand.r[1]);
    matrixRightHand.r[2] = XMVector3Normalize(matrixRightHand.r[2]);
    m_pCollider_Attack->Update(matrixRightHand * m_pTransformCom->Get_WorldMatrix_XMMat());

    for (auto& pPartObj : m_pWeapon)
        pPartObj->Tick(fTimeDelta);

}

void CChai::LateTick(_float fTimeDelta)
{
    for (auto& pPartObj : m_pWeapon)
        pPartObj->LateTick(fTimeDelta);


    GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_PLAYER, m_pCollider_Hit);
    if(m_isAttackOn)
     GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_PLAYER, m_pCollider_Attack);

    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CChai::Render()
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
        {
            //if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            //    return E_FAIL;
            //m_pShaderCom->Bind_Float("g_OutlineWidth", 0.01f);
            //m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);


            //m_pModelCom->Render((_uint)i);
            continue;
        }
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SPECULAR)))
        //    return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        //    return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        //if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
        //   return E_FAIL;

        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG

    m_pCollider_Hit->Render();
    if(m_isAttackOn)
        m_pCollider_Attack->Render();
    
#endif
    return S_OK;
}

void CChai::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pCaller == m_pCollider_Attack)
    {
        DamageInfo damageInfo = {};
        damageInfo.eAttackType = EAttackType::Base;
       
        pOther->Get_Owner()->TakeDamage(damageInfo);
    }
}

void CChai::TakeDamage(const DamageInfo& damageInfo)
{
    shared_ptr <CTransform> pTargetTransform = static_cast<CGameObject*>(damageInfo.pObjectOwner)->Get_Transform();

    if(damageInfo.eAttackType == EAttackType::Base)
    {
        m_bTurning = false;
        m_pTransformCom->LookAt_Horizontal(pTargetTransform->Get_Position());
        m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_damage_Big", false);
        m_eMoveState = CHAI_MOVE_STATE::DAMAGED;
        m_fIdle_Motion_Offset = 0.f;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        m_vecCombo.clear();

    }
}

HRESULT CChai::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chai"), TEXT("Com_Model")));

    CCharacterController::CCT_DESC cctDesc;
    cctDesc.pTransform = m_pTransformCom;
    cctDesc.height = 2.0f;
    cctDesc.radius = 0.5f;
    cctDesc.position = PxExtendedVec3(0.0f, 10.0f, 0.0f);
    cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
    cctDesc.userData = this;

    /* For. Com_CCT*/
    m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"),&cctDesc));

    
    CBasic_Guitar::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");
    WeaponDesc.pParentTransform = m_pTransformCom;
    /* 추가된 파트오브젝트가 부모를 기준으로 회전하거나 이동할 수 있다. */
    WeaponDesc.fSpeedPerSec = 0.f;
    WeaponDesc.fRotationPerSec = 0.f;

    shared_ptr <CPartObject> pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(TEXT("CBasic_Guitar"), &WeaponDesc));
    if (nullptr == pWeapon)
        return E_FAIL;
    m_pWeapon.push_back(pWeapon);



    m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");
    m_pMat_RightHand = m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");

    CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
    sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
    sphereDesc.fRadius = 0.6f;
    m_pCollider_Hit =
        dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Hit"), &sphereDesc));
    m_pCollider_Hit->Set_Owner(shared_from_this());

    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vCenter = _float3{ 0.f, 0.f, -0.6f };
    obbDesc.vExtents = _float3{ 0.14f, 0.1f, 0.7f };
    obbDesc.vRotation = _float3{ 0.f, 0.f, 0.f };
    m_pCollider_Attack =
        dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Attack"), &obbDesc));
    m_pCollider_Attack->Set_Owner(shared_from_this());



    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

void CChai::Create_Attack_Tree()
{
    m_pTree_Attack = new AttacK_Tree("Root");

    AttacK_Tree* L = new AttacK_Tree("L");
    AttacK_Tree* R = new AttacK_Tree("R");
    AttacK_Tree* Rest = new AttacK_Tree("Rest");

    m_pTree_Attack->children[ATTACK_TREE_TYPE::L] = L;
    m_pTree_Attack->children[ATTACK_TREE_TYPE::R] = R;
    m_pTree_Attack->children[ATTACK_TREE_TYPE::REST] = Rest;

    L->parent = m_pTree_Attack;
    R->parent = m_pTree_Attack;
    Rest->parent = m_pTree_Attack;


    /* L */
    AttacK_Tree* L_L = new AttacK_Tree("LL");
    AttacK_Tree* L_R = new AttacK_Tree("LR");
    AttacK_Tree* L_Rest = new AttacK_Tree("L_Rest");


    L->children[ATTACK_TREE_TYPE::L] = L_L;
    L->children[ATTACK_TREE_TYPE::R] = (L_R);
    L->children[ATTACK_TREE_TYPE::REST] = (L_Rest);

    L_L->parent = L;
    L_R->parent = L;
    L_Rest->parent = L;
    /* L_Rest_L */
    AttacK_Tree* L_Rest_L = new AttacK_Tree("L_RestL");
   
    L_Rest->children[ATTACK_TREE_TYPE::L] = (L_Rest_L);
    L_Rest_L->parent = L_Rest;

    /* L_Rest_LL */
    AttacK_Tree* L_Rest_LL = new AttacK_Tree("L_RestLL");
    L_Rest_L->children[ATTACK_TREE_TYPE::L] = (L_Rest_LL);
    L_Rest_LL->parent = L_Rest_L;

    /* L_L */
    AttacK_Tree* L_L_L = new AttacK_Tree("LLL");
    AttacK_Tree* L_L_R = new AttacK_Tree("LLR");
    AttacK_Tree* L_L_Rest = new AttacK_Tree("LL_Rest");

    L_L->children[ATTACK_TREE_TYPE::L] = (L_L_L);
    L_L->children[ATTACK_TREE_TYPE::R] = (L_L_R);
    L_L->children[ATTACK_TREE_TYPE::REST] = (L_L_Rest);

    L_L_L->parent = L_L;
    L_L_R->parent = L_L;
    L_L_Rest->parent = L_L;

    /* LL_RestL */
    AttacK_Tree* L_L_Rest_L = new AttacK_Tree("LL_RestL");
    L_L_Rest->children[ATTACK_TREE_TYPE::L] = (L_L_Rest_L);
    L_L_Rest_L->parent = L_L_Rest;

    /* LL_RestLL */
    AttacK_Tree* L_L_Rest_L_L = new AttacK_Tree("LL_RestLL");
    L_L_Rest_L->children[ATTACK_TREE_TYPE::L] = (L_L_Rest_L_L);
    L_L_Rest_L_L->parent = L_L_Rest_L;


    /* LL_RestLLL */
    AttacK_Tree* L_L_Rest_L_L_L = new AttacK_Tree("LL_RestLLL");
    L_L_Rest_L_L->children[ATTACK_TREE_TYPE::L] = (L_L_Rest_L_L_L);
    L_L_Rest_L_L_L->parent = L_L_Rest_L_L;

    /* L_L_L */
    AttacK_Tree* L_L_L_L = new AttacK_Tree("LLLL");

    L_L_L->children[ATTACK_TREE_TYPE::L] = (L_L_L_L);
    L_L_L_L->parent = L_L_L;
    

    /* R */
    AttacK_Tree* R_L = new AttacK_Tree("RL");
    AttacK_Tree* R_R = new AttacK_Tree("RR");


    R->children[ATTACK_TREE_TYPE::L] = (R_L);
    R->children[ATTACK_TREE_TYPE::R] = (R_R);

    R_L->parent = R;
    R_R->parent = R;

    /* RL */
    AttacK_Tree* R_L_L = new AttacK_Tree("RLL");
    AttacK_Tree* R_L_R = new AttacK_Tree("RLR");

    R_L->children[ATTACK_TREE_TYPE::L] = (R_L_L);
    R_L->children[ATTACK_TREE_TYPE::R] = (R_L_R);

    R_L_L->parent = R_L;
    R_L_R->parent = R_L;

    /* RR */
    AttacK_Tree* R_R_L = new AttacK_Tree("RRL");
    AttacK_Tree* R_R_R = new AttacK_Tree("RRR");

    R_R->children[ATTACK_TREE_TYPE::L] = (R_R_L);
    R_R->children[ATTACK_TREE_TYPE::R] = (R_R_R);

    R_R_L->parent = R_R;
    R_R_R->parent = R_R;

    /* RRL */
    AttacK_Tree* R_R_L_L = new AttacK_Tree("RRLL");

    R_R_L->children[ATTACK_TREE_TYPE::L] = (R_R_L_L);

    R_R_L_L->parent = R_R_L;
}

void CChai::Delete_Attack_Tree(AttacK_Tree* node)
{
    if (!node)
        return;

    // 자식 노드들을 먼저 해제
    for (AttacK_Tree* child : node->children)
    {
        Delete_Attack_Tree(child);
    }

    // 현재 노드를 해제
    delete node;
}

void CChai::Initialize_Attack_Tree(AttacK_Tree* node)
{
    if (!node)
        return;

    for (AttacK_Tree* child : node->children)
    {
        Initialize_Attack_Tree(child);
    }

    node->bActivate = false;
}

void CChai::PrepareCallbacks()
{ 
    // For Attack motions
    m_pModelCom->SetAnimCallback("(Combo_LLLL_1) ch0000_atk-guitar_000", 0, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("(Combo_LLLL_1) ch0000_atk-guitar_000", 1, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("(Combo_LLLL_1) ch0000_atk-guitar_000", [&]() { m_isAttackOn = false; });

    m_pModelCom->SetAnimCallback("(Combo_LLLL_2) ch0000_atk-guitar_001", 0, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("(Combo_LLLL_2) ch0000_atk-guitar_001", 1, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("(Combo_LLLL_2) ch0000_atk-guitar_001", [&]() { m_isAttackOn = false; });

    m_pModelCom->SetAnimCallback("(Combo_LLLL_3) ch0000_atk-guitar_002", 0, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("(Combo_LLLL_3) ch0000_atk-guitar_002", 1, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("(Combo_LLLL_3) ch0000_atk-guitar_002", [&]() { m_isAttackOn = false; });

    m_pModelCom->SetAnimCallback("(Combo_LLLL_4) ch0000_atk-guitar_003", 0, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("(Combo_LLLL_4) ch0000_atk-guitar_003", 1, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("(Combo_LLLL_4) ch0000_atk-guitar_003", [&]() { m_isAttackOn = false; });
}

shared_ptr<CChai> CChai::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CChai
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CChai(pDevice, pContext)
        {}
    };


    shared_ptr<CChai> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CChai"));
        assert(false);
    }

    return pInstance;
}


shared_ptr<CGameObject> CChai::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CChai
    {
        MakeSharedEnabler(const CChai& rhs) : CChai(rhs)
        {}
    };


    shared_ptr<CChai> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CChai"));
        assert(false);
    }

    return pInstance;
}

void CChai::Free()
{
    delete m_pTree_Attack;
}

void CChai::Animation_End()
{
    _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    if (m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_080") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_fIdle_Motion_Offset = 0.f;
        m_fAttack_Motion_Offset = 0.f;
        m_eMoveState = CHAI_MOVE_STATE::RUN;
        m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_mvnt-run_000", true);
    }
    if (m_pModelCom->Get_StringAnimIndex("(Combo_RRR_1_Ready) ch0000_atk-guitar_023") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
        m_fAttack_Motion_Offset = fTimer;
        m_pModelCom->Set_AnimIndexNonCancle("(Combo_RRR_1_Attack) ch0000_atk-guitar_024", false);
    }
    if ((m_pModelCom->Get_StringAnimIndex("(Combo_L_LL_2) ch0000_atk-guitar_053") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("(Combo_LL_LLL_5) ch0000_atk-guitar_095") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("(Combo_RRLL_3) ch0000_atk-guitar_140") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("(Combo_RRLL_4) ch0000_atk-guitar_141") == m_pModelCom->Get_CurrentAnimIndex()

        )
        && m_pModelCom->Get_Finished())
    {
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    else if (m_pModelCom->Get_StringAnimIndex("(Combo_RLR_3) ch0000_atk-guitar_060") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.909f)
    {
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    else if ((m_pModelCom->Get_StringAnimIndex("(Combo_LLLL_1) ch0000_atk-guitar_000") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("(Combo_LLLL_2) ch0000_atk-guitar_001") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("(Combo_LLLL_3) ch0000_atk-guitar_002") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("(Combo_LLLL_4) ch0000_atk-guitar_003") == m_pModelCom->Get_CurrentAnimIndex()||
        m_pModelCom->Get_StringAnimIndex("(Combo_LLLL_4) ch0000_atk-guitar_003") == m_pModelCom->Get_CurrentAnimIndex()||


        m_pModelCom->Get_StringAnimIndex("(Combo_LL_LLL_4) ch0000_atk-guitar_094") == m_pModelCom->Get_CurrentAnimIndex()||
        m_pModelCom->Get_StringAnimIndex("(Combo_LL_LLL_3) ch0000_atk-guitar_093") == m_pModelCom->Get_CurrentAnimIndex()||
       
        m_pModelCom->Get_StringAnimIndex("(Combo_L_LL_1) ch0000_atk-guitar_051") == m_pModelCom->Get_CurrentAnimIndex()||
        m_pModelCom->Get_StringAnimIndex("(Combo_RLL_3) ch0000_atk-guitar_031") == m_pModelCom->Get_CurrentAnimIndex()||
        m_pModelCom->Get_StringAnimIndex("(Combo_RLL_2) ch0000_atk-guitar_030") == m_pModelCom->Get_CurrentAnimIndex()||
        m_pModelCom->Get_StringAnimIndex("(Combo_RRR_3) ch0000_atk-guitar_022") == m_pModelCom->Get_CurrentAnimIndex()||
        m_pModelCom->Get_StringAnimIndex("(Combo_RRR_2) ch0000_atk-guitar_021") == m_pModelCom->Get_CurrentAnimIndex()||
        m_pModelCom->Get_StringAnimIndex("(Combo_RRR_1_Attack) ch0000_atk-guitar_024") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage()>0.85f)
    {
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    
    if (m_pModelCom->Get_StringAnimIndex("ch0000_damage_Big") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }

}

void CChai::Animation(_float fTimeDelta)
{
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    /* 반박자에 끝나는 모션 ex) 단타 공격모션 */
    _float One_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat;
    _float Two_Beat= m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.5f;
    _float Three_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.3333f);
    _float Four_Beat= m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.25f;
    _float Five_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.2f);
    _float Six_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.1666f);
    _float Eight_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.125f);

    _uint index = m_pModelCom->Get_CurrentAnimIndex();


    _float rhythmOffset = false;
    _float animationOffset = 1.f;
    /* 가장 가까운 박자에 맞춰서 빨리 재생 */


  _float leftBeatTime = fBeat - m_fAttack_Motion_Offset;
  _float leftIdleBeatTime = fBeat - m_fIdle_Motion_Offset;
  if (m_pModelCom->Get_Current_Anim_Track_Position_Percentage() < 0.5f)
  {
          animationOffset += leftBeatTime / fBeat;
          rhythmOffset = true;
  }


    if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_idle_000"))
    {
        if(!rhythmOffset)
            m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
        else
            m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_damage_Big"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_080") )
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * One_Beat, 0.1f, fmatResult);


    /* Attack Animation*/
    else if ( // 강공격 1타 준비모션 전용
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_RRR_1_Ready) ch0000_atk-guitar_023"))
    {
        if (!rhythmOffset)
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * One_Beat) * animationOffset, 0.1f, fmatResult);
        else
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset, 0.1f, fmatResult);
    }
    else if (// 강공격 1타 공격 전용
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_RRR_1_Attack) ch0000_atk-guitar_024") 
        )
    {
        if (!rhythmOffset)
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Five_Beat) * animationOffset, 0.1f, fmatResult);
        else
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset, 0.1f, fmatResult);
    }
    else if ( // 5박 공격
       
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_L_LL_1) ch0000_atk-guitar_051")
        )
    {
        if (!rhythmOffset)
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Five_Beat) * animationOffset, 0.1f, fmatResult);
        else
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset, 0.1f, fmatResult);
    }
    else if ( // 3박 공격
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_L_LL_2) ch0000_atk-guitar_053") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_LL_LLL_3) ch0000_atk-guitar_093") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_LL_LLL_4) ch0000_atk-guitar_094") 
        )
    {
        if (!rhythmOffset)
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Three_Beat) * animationOffset, 0.1f, fmatResult);
        else
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset, 0.1f, fmatResult);
    }
    else if ( // 4박 공격
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_RLL_2) ch0000_atk-guitar_030") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_RRLL_3) ch0000_atk-guitar_140") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_LL_LLL_5) ch0000_atk-guitar_095") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_LLLL_1) ch0000_atk-guitar_000") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_LLLL_2) ch0000_atk-guitar_001") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_LLLL_3) ch0000_atk-guitar_002")
        )
    {
        if (!rhythmOffset)
             m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Four_Beat) * animationOffset, 0.1f, fmatResult);
        else
            m_pModelCom->Play_Animation(fTimeDelta,(fTimeDelta)*animationOffset, 0.1f, fmatResult);
    }
    else if ( // 8박 공격 마무리
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_RLR_3) ch0000_atk-guitar_060")
        )
    {
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Eight_Beat), 0.1f, fmatResult);

    }
    else if ( // 6박 공격 마무리
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_RLL_3) ch0000_atk-guitar_031")||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_LLLL_4) ch0000_atk-guitar_003") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_LLLL_4_Big_Jump) ch0000_atk-guitar_004")
        )
    {
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Six_Beat), 0.1f, fmatResult);

    }
    else if ( // 4박 공격(우클릭)
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_RRR_2) ch0000_atk-guitar_021"))
    {
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Four_Beat), 0.1f, fmatResult);
    }
    else if (// 5박 공격(우클릭)
       m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_RRR_3) ch0000_atk-guitar_022")||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("(Combo_RRLL_4) ch0000_atk-guitar_141")
        )
    {
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Five_Beat), 0.1f, fmatResult);
    }
    else
       m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);




    //m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta * 60.f, fmatResult);
    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

   
    if (!m_bTurning)
        m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
    
}



void CChai::Move(_float fTimeDelta)
{
#pragma region Run
    /* RUN */
    _vector prevPlayerLookVec = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0));
    _vector camLookVec = XMVectorSetY(GAMEINSTANCE->Get_CamLook_Vector(), 0);

    _vector camRightvec = {};
    shared_ptr <CTransform> camTrasnform = static_pointer_cast<CTransform> (GAMEINSTANCE->Get_MainCamera()->Get_Transform());
    camRightvec = XMVectorSetY(camTrasnform->Get_State(CTransform::STATE_RIGHT), 0);



    /* 직진 */
    if ((GAMEINSTANCE->Get_KeyPressing(DIK_W) || GAMEINSTANCE->Get_KeyPressing(DIK_S) || GAMEINSTANCE->Get_KeyPressing(DIK_A) || GAMEINSTANCE->Get_KeyPressing(DIK_D))
        && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_000"))
    {
        m_eMoveState = CHAI_MOVE_STATE::RUN_START;
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-run_080", false);
        m_vecCombo.clear();
    }
    else if ((GAMEINSTANCE->Get_KeyNone(DIK_W) || GAMEINSTANCE->Get_KeyNone(DIK_S) || GAMEINSTANCE->Get_KeyNone(DIK_A) || GAMEINSTANCE->Get_KeyNone(DIK_D))
        && m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_000"))
        m_pModelCom->Set_AnimIndexNonCancle_NoInterpole("ch0000_mvnt-run_000", true);

    /* 중립 입력 */

    if (GAMEINSTANCE->Get_KeyNone(DIK_W) && GAMEINSTANCE->Get_KeyNone(DIK_S) && GAMEINSTANCE->Get_KeyNone(DIK_A) && GAMEINSTANCE->Get_KeyNone(DIK_D)
        && (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_000") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_000")))
    {
        if (m_eMoveState != CHAI_MOVE_STATE::IDLE)
            Adjust_Idle_Timing();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;

    }

    if(m_eMoveState == CHAI_MOVE_STATE::RUN && !m_bTurning)
        m_pCCT->Move_Forward(m_pTransformCom->Get_Speed());


   

#pragma endregion 

    Turn(fTimeDelta);


}

void CChai::Turn(_float fTimeDelta)
{
    _vector prevPlayerLookVec = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0));
    _vector camLookVec = XMVectorSetY(GAMEINSTANCE->Get_CamLook_Vector(), 0);

    _vector camRightvec = {};
    shared_ptr <CTransform> camTrasnform = static_pointer_cast<CTransform> (GAMEINSTANCE->Get_MainCamera()->Get_Transform());
    camRightvec = XMVectorSetY(camTrasnform->Get_State(CTransform::STATE_RIGHT), 0);
    /* 오른쪽 위 대각*/
    if (GAMEINSTANCE->Get_KeyPressing(DIK_W) && GAMEINSTANCE->Get_KeyPressing(DIK_D))
    {
        _vector diagonalVec = XMVector3Normalize( camLookVec + camRightvec);
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning)
            m_pCCT->Add_Displacement(diagonalVec * m_pTransformCom->Get_Speed());
    }
    else if (GAMEINSTANCE->Get_KeyPressing(DIK_W) && GAMEINSTANCE->Get_KeyPressing(DIK_A))
    {
        _vector diagonalVec = XMVector3Normalize(camLookVec + -camRightvec);
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if(m_bTurning)
            m_pCCT->Add_Displacement(diagonalVec * m_pTransformCom->Get_Speed());
         /*    m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + diagonalVec * m_pTransformCom->Get_Speed() * fTimeDelta);*/
    }
    else if (GAMEINSTANCE->Get_KeyPressing(DIK_S) && GAMEINSTANCE->Get_KeyPressing(DIK_D))
    {
        _vector diagonalVec = XMVector3Normalize(-camLookVec + camRightvec);
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        } 
        if (m_bTurning)
            m_pCCT->Add_Displacement(diagonalVec * m_pTransformCom->Get_Speed());
    }
    else if (GAMEINSTANCE->Get_KeyPressing(DIK_S) && GAMEINSTANCE->Get_KeyPressing(DIK_A))
    {
        _vector diagonalVec = XMVector3Normalize(-camLookVec + -camRightvec);
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning)
            m_pCCT->Add_Displacement(diagonalVec * m_pTransformCom->Get_Speed());
    }
    else if (GAMEINSTANCE->Get_KeyPressing(DIK_W))
    {
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(camLookVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(camLookVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning)
            m_pCCT->Add_Displacement(camLookVec * m_pTransformCom->Get_Speed());
    }
    else if (GAMEINSTANCE->Get_KeyPressing(DIK_D))
    {
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(camRightvec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(camRightvec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning)
            m_pCCT->Add_Displacement(camRightvec * m_pTransformCom->Get_Speed());
    }
    else if (GAMEINSTANCE->Get_KeyPressing(DIK_A))
    {
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(-camRightvec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(-camRightvec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning)
            m_pCCT->Add_Displacement(-camRightvec * m_pTransformCom->Get_Speed());
    }
    else if (GAMEINSTANCE->Get_KeyPressing(DIK_S))
    {
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(-camLookVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(-camLookVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning)
            m_pCCT->Add_Displacement(-camLookVec * m_pTransformCom->Get_Speed());
    }
    if (m_fLookDiffDegree != 0.f && m_fTurnTimer > 0.f && m_bTurning == true)
    {
        m_fTurnTimer -= fTimeDelta;
        _vector turnDir = {};
        if (m_bTrunRight)
            turnDir = { 0.f, 1.f, 0.f, 0.f };
        else
            turnDir = { 0.f, -1.f, 0.f, 0.f };
        m_pTransformCom->Turn(turnDir, XMConvertToRadians(m_fLookDiffDegree), fTimeDelta * 10.f);
   
    }
    else if (m_fTurnTimer <= 0.f)
    {
        m_bTurning = false;
        m_fTurnTimer = 0.1f;
        m_fLookDiffDegree = 0.f;
    }
}

_bool CChai::Attack(_float fTimeDelta)
{
    _bool bAttacking = false;
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    m_fAttackTimer += fTimeDelta;


    if (m_bNextAttack && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.25f && m_eNextAttack != ATTACK_TREE_TYPE::ATTACK_TYPE_END)
    {
        if (m_eNextAttack == ATTACK_TREE_TYPE::R && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() < 0.25f
            && m_pModelCom->Get_StringAnimIndex("(Combo_RRR_1_Attack) ch0000_atk-guitar_024") == m_pModelCom->Get_CurrentAnimIndex())
        {
            return true;
        }
        else if (m_eNextAttack == ATTACK_TREE_TYPE::R && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() < 0.5f
            && m_pModelCom->Get_StringAnimIndex("(Combo_RRR_1_Attack) ch0000_atk-guitar_024") != m_pModelCom->Get_CurrentAnimIndex())
            return true;
        _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
        m_fAttack_Motion_Offset = fTimer;
        m_bTurning = false;
        m_bAttack_Input = false;
        Traverse_AttacK_Tree(m_pTree_Attack, m_eNextAttack , m_vecCombo);
        m_eNextAttack = ATTACK_TREE_TYPE::ATTACK_TYPE_END;
    }
    else
    {
        if (!m_bAttack_Input && !m_bNextAttack)
        {
            shared_ptr<CUi_BeatMarkerSystem> beatMarkersys = static_pointer_cast<CUi_BeatMarkerSystem> (CUi_Mgr::Get_Instance()->Find_UI(TEXT("CUi_BeatMarkerSystem")));
            if (GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON))
            {
                beatMarkersys->Cal_Judge();
                _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
                m_fIdle_Motion_Offset = 0.f;
                m_fAttack_Motion_Offset = fTimer;
                m_bAttack_Input = true;
                m_bTurning = false;
                Traverse_AttacK_Tree(m_pTree_Attack, ATTACK_TREE_TYPE::L, m_vecCombo);
            }

            else if (GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON))
            {
                beatMarkersys->Cal_Judge();
                _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
                m_fIdle_Motion_Offset = 0.f;
                m_fAttack_Motion_Offset = fTimer;
                m_bAttack_Input = true;
                m_bTurning = false;
                Traverse_AttacK_Tree(m_pTree_Attack, ATTACK_TREE_TYPE::R, m_vecCombo);
            }
        }
        else
        {
            if (m_pModelCom->Get_StringAnimIndex("(Combo_RRR_1_Ready) ch0000_atk-guitar_023") == m_pModelCom->Get_CurrentAnimIndex())
                return true;
            shared_ptr<CUi_BeatMarkerSystem> beatMarkersys = static_pointer_cast<CUi_BeatMarkerSystem> (CUi_Mgr::Get_Instance()->Find_UI(TEXT("CUi_BeatMarkerSystem")));
            if (GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON))
            {
                beatMarkersys->Cal_Judge();
                _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
                m_fIdle_Motion_Offset = 0.f;
                m_fAttack_Motion_Offset = fTimer;
                m_bNextAttack = true;
                m_bTurning = false;
                m_eNextAttack = ATTACK_TREE_TYPE::L;
            }

            else if (GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON))
            {
                beatMarkersys->Cal_Judge();
                _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
                m_fIdle_Motion_Offset = 0.f;
                m_fAttack_Motion_Offset = fTimer;
                m_bNextAttack = true;
                m_bTurning = false;
                m_eNextAttack = ATTACK_TREE_TYPE::R;
            }
            else if (((m_vecCombo.size() == 1 && m_vecCombo.back() == ATTACK_TREE_TYPE::L) || (m_vecCombo.size() == 2 && m_vecCombo.back() == ATTACK_TREE_TYPE::L))
                && !Find_R()
                && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.45f)
            {
                m_bNextAttack = true;
                m_bTurning = false;
                m_eNextAttack = ATTACK_TREE_TYPE::REST;
            }
        }
    }
    //if (m_fAttackTimer > fBeat * 2.f)
    //{

    //    m_fAttackTimer = 0.f;
    //    m_bAttack_Input = false;
    //}
    string combo = {};
    for (auto& iter : m_vecCombo)
    {
        if (iter == L)
            combo += "L";
        else if (iter == R)
            combo += "R";
        else if (iter == REST)
            combo += "_Rest";
    }

    if (!combo.empty())
    {
        Attack_Turn(fTimeDelta);
        bAttacking = true;
        if (combo == "L")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_LLLL_1) ch0000_atk-guitar_000", false);

        else if (combo == "LL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_LLLL_2) ch0000_atk-guitar_001", false);

        else if (combo == "LLL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_LLLL_3) ch0000_atk-guitar_002", false);

        else if (combo == "LLLL")
           m_pModelCom->Set_AnimIndexNonCancle("(Combo_LLLL_4) ch0000_atk-guitar_003", false);

        else if (combo == "L_RestL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_L_LL_1) ch0000_atk-guitar_051", false);

        else if (combo == "L_RestLL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_L_LL_2) ch0000_atk-guitar_053", false);

        else if (combo == "LL_RestL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_LL_LLL_3) ch0000_atk-guitar_093", false);

        else if (combo == "LL_RestLL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_LL_LLL_4) ch0000_atk-guitar_094", false);

        else if (combo == "LL_RestLLL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_LL_LLL_5) ch0000_atk-guitar_095", false);

        else if (combo == "R" && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("(Combo_RRR_1_Attack) ch0000_atk-guitar_024") )
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_RRR_1_Ready) ch0000_atk-guitar_023", false);

        else if (combo == "RR")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_RRR_2) ch0000_atk-guitar_021", false);

        else if (combo == "RRR")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_RRR_3) ch0000_atk-guitar_022", false);

        else if (combo == "RL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_RLL_2) ch0000_atk-guitar_030", false);

        else if (combo == "RLL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_RLL_3) ch0000_atk-guitar_031", false);

        else if (combo == "RLR")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_RLR_3) ch0000_atk-guitar_060", false);

        else if (combo == "RRL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_RRLL_3) ch0000_atk-guitar_140", false);

        else if (combo == "RRLL")
            m_pModelCom->Set_AnimIndexNonCancle("(Combo_RRLL_4) ch0000_atk-guitar_141", false);
    }
   


    if (m_fLookDiffDegree != 0.f && m_fTurnTimer > 0.f && m_bAttacKTurning == true)
    {
        m_fTurnTimer -= fTimeDelta;
        _vector turnDir = {};
        if (m_bTrunRight)
            turnDir = { 0.f, 1.f, 0.f, 0.f };
        else
            turnDir = { 0.f, -1.f, 0.f, 0.f };
        m_pTransformCom->Turn(turnDir, XMConvertToRadians(m_fLookDiffDegree), fTimeDelta * 10.f);

    }
    else if (m_fTurnTimer <= 0.f)
    {
        m_bAttacKTurning = false;
        m_fTurnTimer = 0.1f;
        m_fLookDiffDegree = 0.f;
    }
    return bAttacking;
}

void CChai::Attack_Turn(_float fTimeDelta)
{
    _vector prevPlayerLookVec = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0));
    shared_ptr <CGameObject> pTarget = static_pointer_cast<CCamera_Player> (GAMEINSTANCE->Get_MainCamera())->Get_Lock_On_Target();
    if (!pTarget)
        return;

    shared_ptr <CTransform > pTargetTransform = pTarget->Get_Transform();

    _float fDistance = XMVectorGetX( XMVector3Length( pTargetTransform->Get_Position() - m_pTransformCom->Get_Position()));

    if (fDistance > 4.f)
        return;
    _vector tagetVector = XMVectorSetY(XMVector3Normalize( pTargetTransform->Get_Position() - m_pTransformCom->Get_Position()),0);



    _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(tagetVector));
    m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

    _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(tagetVector));
    _float turnFloat = XMVectorGetY(LookCross);
    if (turnFloat < 0)
        m_bTrunRight = false;
    else
        m_bTrunRight = true;

    if (m_fLookDiffDegree > 1.f)
    {
        m_fTurnTimer = 0.1f;
        m_bAttacKTurning = true;
    }
    else
    {
        m_fTurnTimer = 0.f;
        m_bAttacKTurning = false;
    }
    //if (m_bAttacKTurning)
    //    m_pCCT->Add_Displacement(diagonalVec * m_pTransformCom->Get_Speed());
    


}

void CChai::Adjust_Idle_Timing()
{
    _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
   _float currentTimeRatio = fTimer / fBeat;
   m_pModelCom->Set_AnimIndexNonCancle("ch0000_idle_000", true);

   m_pModelCom->Set_CurrentTrackPosition(m_pModelCom->Get_Duration() * currentTimeRatio * 0.25f);
   _float curTP = m_pModelCom->Get_Current_Anim_Track_Position_Percentage();
}

_bool CChai::Find_R()
{
    for (auto& iter : m_vecCombo)
    {
        if (iter == ATTACK_TREE_TYPE::R)
            return true;
    }
    return false;
}

void CChai::Traverse_AttacK_Tree(AttacK_Tree* node, ATTACK_TREE_TYPE eAttackType, vector<ATTACK_TREE_TYPE> vecCombo)
{
    if (!node)
        return;

    _bool temp = true;
    for (AttacK_Tree* child : node->children)
    {
        if (child && child->bActivate == true)
        {
            Traverse_AttacK_Tree(child, eAttackType, vecCombo);
            temp = false;
            return;
        }
    }
    
    if(temp)
    {
        if(node->children[eAttackType])
        {
            node->children[eAttackType]->bActivate = true;
            m_vecCombo.push_back(eAttackType);
        }
    }
}
