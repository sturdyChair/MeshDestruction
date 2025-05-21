#include "Monster_Gunner.h"
#include "GameInstance.h"
#include "Collision_Manager.h"
#include "PlayerManager.h"
#include "Shader.h"
#include "Model.h"
#include "CharacterController.h"

#include "CombatManager.h"

#include "Collider.h"
#include "Bounding_Sphere.h"

#include "GunnerState_Idle.h"
#include "GunnerState_Move.h"
#include "GunnerState_Attack.h"
#include "GunnerState_Damaged.h"
#include "GunnerState_Dead.h"
typedef CGunnerState::State GunnerState;


wstring CMonster_Gunner::ObjID = TEXT("CMonster_Gunner");

CMonster_Gunner::CMonster_Gunner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonsterBase{ pDevice, pContext }
{
}

CMonster_Gunner::CMonster_Gunner(const CMonster_Gunner& rhs)
    : CMonsterBase{ rhs }
{
}

CMonster_Gunner::~CMonster_Gunner()
{
    Free();
}

HRESULT CMonster_Gunner::Initialize_Prototype()
{
    _matrix		LocalTransformMatrix = XMMatrixIdentity();
    LocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));

    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Model_Monster_Gunner"), TEXT("../Bin/Resources/Models/Monsters/Gunner/Gunner.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );

    return S_OK;
}

HRESULT CMonster_Gunner::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    PrepareCallbacks();

    // Physix setting
    m_pCCT->Disable_Gravity();
    m_pCCT->Disable_Pseudo_Physics();

    // Initialize state
    m_pCurState = new CGunnerState_Idle(dynamic_pointer_cast<CMonster_Gunner>(shared_from_this()), GunnerState::IDLE);
    m_pCurState->ChangeState(nullptr, m_pCurState);

    CCombat_Manager::Get_Instance()->Register_Enemy(shared_from_this());

    return S_OK;
}

void CMonster_Gunner::PriorityTick(_float fTimeDelta)
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

    // Update AttackTimer
    if (m_pCurState->GetState() != GunnerState::ATTACK && m_pCurState->GetState() != GunnerState::DAMAGED)
        m_fTimer_Attack -= fTimeDelta;
}

void CMonster_Gunner::Tick(_float fTimeDelta)
{
    // Update state
    CGunnerState* pNextState = m_pCurState->StepState(fTimeDelta);
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
    }

    // Update Physics
    m_pCCT->Update(fTimeDelta);

    // Update Colliders
    auto matrixSpine = XMLoadFloat4x4(m_pMat_Spine);
    matrixSpine.r[0] = XMVector3Normalize(matrixSpine.r[0]);
    matrixSpine.r[1] = XMVector3Normalize(matrixSpine.r[1]);
    matrixSpine.r[2] = XMVector3Normalize(matrixSpine.r[2]);
    m_pCollider_Hit->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CMonster_Gunner::LateTick(_float fTimeDelta)
{
    if (GunnerState::DEAD != m_pCurState->GetState())
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Hit);

    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMonster_Gunner::Render()
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
    if (GunnerState::DEAD != m_pCurState->GetState())
        m_pCollider_Hit->Render();
#endif

    return S_OK;
}

void CMonster_Gunner::TakeDamage(const DamageInfo& damageInfo)
{
    if (GunnerState::DEAD == m_pCurState->GetState())
        return;

    m_fCurHp -= damageInfo.fAmountDamage;
    if (m_fCurHp <= 0.f)
    {
        auto pStateDead = new CGunnerState_Dead(dynamic_pointer_cast<CMonster_Gunner>(shared_from_this()), m_pCurState->GetState(), damageInfo);
        m_pCurState->ChangeState(m_pCurState, pStateDead);
        m_pCurState = pStateDead;
    }
    else
    {
        auto pStateDamaged = new CGunnerState_Damaged(dynamic_pointer_cast<CMonster_Gunner>(shared_from_this()), m_pCurState->GetState(), damageInfo);
        m_pCurState->ChangeState(m_pCurState, pStateDamaged);
        m_pCurState = pStateDamaged;
    }
}

HRESULT CMonster_Gunner::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

    // Init Model and Matrices
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Monster_Gunner"), TEXT("Com_Model")));
    m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");

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

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
    return S_OK;
}

void CMonster_Gunner::PrepareCallbacks()
{
}

_bool CMonster_Gunner::IsAnimFinished()
{
    return m_pModelCom->Get_Finished();
}

void CMonster_Gunner::SetAnim(const string& strAnimName, _bool isLoop)
{
    m_pModelCom->Set_AnimIndexNonCancle(strAnimName, isLoop);
}

void CMonster_Gunner::SetAnimHard(const string& strAnimName, _bool isLoop)
{
    m_pModelCom->Set_AnimIndex_NoInterpole(strAnimName, isLoop);
}

_bool CMonster_Gunner::IsNeedToMove()
{
    return m_fDistance < m_fDistance_Near_C || m_fDistance > m_fDistance_Far_C;
}

shared_ptr<CMonster_Gunner> CMonster_Gunner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMonster_Gunner
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_Gunner(pDevice, pContext) {}
    };

    shared_ptr<CMonster_Gunner> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMonster_Gunner"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMonster_Gunner::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMonster_Gunner
    {
        MakeSharedEnabler(const CMonster_Gunner& rhs) : CMonster_Gunner(rhs) {}
    };

    shared_ptr<CMonster_Gunner> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMonster_Gunner"));
        assert(false);
    }

    return pInstance;
}

void CMonster_Gunner::Free()
{
    Safe_Delete(m_pCurState);
}
