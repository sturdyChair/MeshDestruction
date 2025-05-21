#include "Monster_Saber.h"
#include "GameInstance.h"
#include "Collision_Manager.h"
#include "PlayerManager.h"
#include "Shader.h"
#include "Model.h"
#include "CharacterController.h"

#include "Collider.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

#include "Saber_Weapon.h"

#include "SaberState_Idle.h"
#include "SaberState_Move.h"
#include "SaberState_Attack.h"
#include "SaberState_Damaged.h"
#include "SaberState_Dead.h"
#include "CombatManager.h"
typedef CSaberState::State SaberState;


wstring CMonster_Saber::ObjID = TEXT("CMonster_Saber");

CMonster_Saber::CMonster_Saber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonsterBase{ pDevice, pContext }
{
}

CMonster_Saber::CMonster_Saber(const CMonster_Saber& rhs)
	: CMonsterBase{ rhs }
{
}

CMonster_Saber::~CMonster_Saber()
{
	Free();
}

HRESULT CMonster_Saber::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    _matrix		LocalTransformMatrix = XMMatrixIdentity();
    LocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));

    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Model_Monster_Saber"), TEXT("../Bin/Resources/Models/Monsters/Saber/Saber.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );

    m_List_Owning_ObjectID.push_back(CSaber_Weapon::ObjID);

    return S_OK;
}

HRESULT CMonster_Saber::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    if (FAILED(Ready_PartObjects()))
        return E_FAIL;
    PrepareCallbacks();

    // Physix setting
    m_pCCT->Disable_Gravity();
    m_pCCT->Disable_Pseudo_Physics();

    // Initialize state
    m_pCurState = new CSaberState_Idle(dynamic_pointer_cast<CMonster_Saber>(shared_from_this()), SaberState::IDLE);
    m_pCurState->ChangeState(nullptr, m_pCurState);

    CCombat_Manager::Get_Instance()->Register_Enemy(shared_from_this());
    return S_OK;
}

void CMonster_Saber::PriorityTick(_float fTimeDelta)
{
    _vector vPlayerPos = PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position();
    _vector vCurPos = m_pTransformCom->Get_Position();
    _vector vCurLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

    _vector vDirToPlayer = XMVectorSetY(vPlayerPos - vCurPos, 0.f);
    m_fDistance = XMVectorGetX(XMVector4Length(vDirToPlayer));
    vDirToPlayer = XMVector3Normalize(vDirToPlayer);
    XMStoreFloat4(&m_vfDirToPlayer, vDirToPlayer);

    _float fCrossY = XMVectorGetY(XMVector3Cross(vCurLook, vDirToPlayer));
    _float fDot = XMVectorGetX(XMVector3Dot(vCurLook, vDirToPlayer));

    m_fAngleDegree = XMConvertToDegrees(acosf(fDot));
    if (fCrossY < 0.f)
        m_fAngleDegree *= -1.f;
}

void CMonster_Saber::Tick(_float fTimeDelta)
{
    // Update state
    CSaberState* pNextState = m_pCurState->StepState(fTimeDelta);
    if (nullptr != pNextState)
    {
        m_pCurState->ChangeState(m_pCurState, pNextState);
        m_pCurState = pNextState;
    }
    m_pCurState->ActiveState(fTimeDelta);


    // Play Animation
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);

    if (m_pModelCom->IsRootAnim() && !m_pModelCom->IsForcedNonRoot())
    {
        _matrix matResult = XMLoadFloat4x4(&fmatResult);
        auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);

        _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
        auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);
        m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
        //m_pTransformCom->Set_Position(vCurPos);
    }

    // Update Physics
    m_pCCT->Update(fTimeDelta);

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
    auto matAttackResult = matrixRightHand* m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pCollider_Attack->Update(matAttackResult);

    XMStoreFloat4(&m_vCurDir_Attack, matAttackResult.r[3] - XMLoadFloat4(&m_vPrePos_Attack));
    XMStoreFloat4(&m_vPrePos_Attack, matAttackResult.r[3]);

    // Update PartObjects
    m_pWeapon->Tick(fTimeDelta);
}

void CMonster_Saber::LateTick(_float fTimeDelta)
{
    m_pWeapon->LateTick(fTimeDelta);

    if (SaberState::DEAD != m_pCurState->GetState())
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Hit);
    if (m_isAttackOn)
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Attack);

    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMonster_Saber::Render()
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
            continue;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SPECULAR)))
        //    return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        //    return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            continue;

        m_pShaderCom->Begin(0);

        if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
            continue;

        m_pModelCom->Render((_uint)i);
    }

// For Colliders
#ifdef _DEBUG
    if (SaberState::DEAD != m_pCurState->GetState())
        m_pCollider_Hit->Render();
    if (m_isAttackOn)
        m_pCollider_Attack->Render();
#endif

    return S_OK;
}

void CMonster_Saber::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pCaller == m_pCollider_Attack)
    {
        DamageInfo damageInfo{};
        damageInfo.eAttackType = EAttackType::Base;
        damageInfo.pObjectOwner = this;
        pOther->Get_Owner()->TakeDamage(damageInfo);
    }
}

void CMonster_Saber::TakeDamage(const DamageInfo& damageInfo)
{
    if (SaberState::DEAD == m_pCurState->GetState())
        return;

    m_fCurHp -= damageInfo.fAmountDamage;
    if (m_fCurHp <= 0.f)
    {
        auto pStateDead = new CSaberState_Dead(dynamic_pointer_cast<CMonster_Saber>(shared_from_this()), m_pCurState->GetState(), damageInfo);
        m_pCurState->ChangeState(m_pCurState, pStateDead);
        m_pCurState = pStateDead;
    }
    else
    {
        auto pStateDamaged = new CSaberState_Damaged(dynamic_pointer_cast<CMonster_Saber>(shared_from_this()), m_pCurState->GetState(), damageInfo);
        m_pCurState->ChangeState(m_pCurState, pStateDamaged);
        m_pCurState = pStateDamaged;
    }
}

HRESULT CMonster_Saber::Ready_PartObjects()
{
    CSaber_Weapon::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pBoneMatrix = m_pMat_RightHand;
    WeaponDesc.pParentTransform = m_pTransformCom;
    WeaponDesc.fSpeedPerSec = 0.f;
    WeaponDesc.fRotationPerSec = 0.f;
    WeaponDesc.iLevelIndex = m_iLevelIndex;

    m_pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(CSaber_Weapon::ObjID, &WeaponDesc));
    if (nullptr == m_pWeapon)
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Saber::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

    // Init Model and Matrices
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Monster_Saber"), TEXT("Com_Model")));
    m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");
    m_pMat_RightHand = m_pModelCom->Get_BoneMatrixPtr("r_hand_attach_00");

    CCharacterController::CCT_DESC cctDesc;
    cctDesc.pTransform = m_pTransformCom;
    cctDesc.height = 2.0f;
    cctDesc.radius = 0.5f;
    cctDesc.position = PxExtendedVec3(0.0f, 1.0f, 0.0f);
    cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
    cctDesc.userData = this;

    /* For. Com_CCT*/
    m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));

    {
        CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
        sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
        sphereDesc.fRadius = 0.6f;
        m_pCollider_Hit =
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Hit"), &sphereDesc));
        m_pCollider_Hit->Set_Owner(shared_from_this());
        m_pCollider_Hit->Set_ColliderTag(TEXT("Monster_Hit"));
    }
    {
        CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
        obbDesc.vCenter = _float3{ 0.f, 0.f, -0.6f };
        obbDesc.vExtents = _float3{ 0.14f, 0.1f, 0.7f };
        obbDesc.vRotation = _float3{ 0.f, 0.f, 0.f };
        m_pCollider_Attack =
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Attack"), &obbDesc));
        m_pCollider_Attack->Set_Owner(shared_from_this());
        m_pCollider_Attack->Set_ColliderTag(TEXT("Monster_Attack"));
    }

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
    return S_OK;
}

void CMonster_Saber::PrepareCallbacks()
{
    // For Attack motions
    m_pModelCom->SetAnimCallback("em0100_sword-a_000", 0, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("em0100_sword-a_000", 1, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("em0100_sword-a_000", [&]() { m_isAttackOn = false; });
    m_pModelCom->SetAnimCallback("em0100_sword-a_001", 0, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("em0100_sword-a_001", 1, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("em0100_sword-a_001", [&]() { m_isAttackOn = false; });
}

_bool CMonster_Saber::IsAnimFinished()
{
    return m_pModelCom->Get_Finished();
}

void CMonster_Saber::SetAnim(const string& strAnimName, _bool isLoop)
{
    m_pModelCom->Set_AnimIndexNonCancle(strAnimName, isLoop);
}

void CMonster_Saber::SetAnimHard(const string& strAnimName, _bool isLoop)
{
    m_pModelCom->Set_AnimIndex_NoInterpole(strAnimName, isLoop);
}

shared_ptr<CMonster_Saber> CMonster_Saber::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMonster_Saber
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_Saber(pDevice, pContext) {}
    };

    shared_ptr<CMonster_Saber> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMonster_Saber"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMonster_Saber::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMonster_Saber
    {
        MakeSharedEnabler(const CMonster_Saber& rhs) : CMonster_Saber(rhs) {}
    };

    shared_ptr<CMonster_Saber> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMonster_Saber"));
        assert(false);
    }

    return pInstance;
}

void CMonster_Saber::Free()
{
    Safe_Delete(m_pCurState);
}
