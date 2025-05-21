#include "Free_SK_Object.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Model.h"

wstring CFree_SK_Object::ObjID = TEXT("CFree_SK_Object");

CFree_SK_Object::CFree_SK_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CFree_SK_Object::CFree_SK_Object(const CFree_SK_Object& rhs)
    : CGameObject(rhs)
{
}

CFree_SK_Object::~CFree_SK_Object()
{
    Free();
}

HRESULT CFree_SK_Object::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CFree_SK_Object::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC TransformDesc{};
    TransformDesc.fSpeedPerSec = 1.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    // Loaded prototype of selected model
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);

    auto pDesc = static_cast<DESC*>(pArg);
    string strPath = pDesc->strModelPath;
    m_strModelTag = wstring(strPath.begin(), strPath.end());
    if (nullptr == GAMEINSTANCE->Get_Component_Prototype(m_iLevelIndex, m_strModelTag))
    {
        GAMEINSTANCE->Add_Prototype(
            m_iLevelIndex, m_strModelTag, CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, strPath.c_str(), LocalTransformMatrix)
        );
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CFree_SK_Object::PriorityTick(_float fTimeDelta)
{
}

void CFree_SK_Object::Tick(_float fTimeDelta)
{
    if (m_isPlayAuto)
    {
        _float4x4 matResult;
        m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta, matResult);
    }
}

void CFree_SK_Object::LateTick(_float fTimeDelta)
{
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CFree_SK_Object::Render()
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
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

HRESULT CFree_SK_Object::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, m_strModelTag, TEXT("Com_Model")));

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

void CFree_SK_Object::GetAnimStrList(vector<string>& listAnim) const
{
    vector<shared_ptr< CAnimation>> vecAnimList = m_pModelCom->Get_Animation_List();
    for (size_t i = 0; i < vecAnimList.size(); i++)
    {
        listAnim.push_back(vecAnimList[i]->Get_AnimName());
    }
}

void CFree_SK_Object::SwitchAnim(const string& strAnim)
{
    m_pModelCom->Set_AnimIndex(strAnim, true);
}

void CFree_SK_Object::SetAnimFrame(_float fFrame)
{
    if (!m_isPlayAuto)
        m_pModelCom->Set_AnimFrame(fFrame);
}

void CFree_SK_Object::GetAnimInfo(shared_ptr<CAnimation>& ppAnim, _uint iAnimIndex)
{
    auto pAnim = m_pModelCom->Get_Animation_List()[iAnimIndex];
    ppAnim = pAnim;
}

HRESULT CFree_SK_Object::ExportAnimInfo(const string& strFolderPath)
{
    return m_pModelCom->ExportAnimInfo(strFolderPath);
}

string CFree_SK_Object::SwitchPlayMode()
{
    if (!m_isPlayAuto)
        m_pModelCom->Set_AnimFrame(0.f);

    m_isPlayAuto = !m_isPlayAuto;
    if (m_isPlayAuto)
        return "Stop";
    else
        return "Play";
}

shared_ptr<CFree_SK_Object> CFree_SK_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CFree_SK_Object
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFree_SK_Object(pDevice, pContext)
        {}
    };


    shared_ptr<CFree_SK_Object> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFree_SK_Object"));
        assert(false);
    }

    return pInstance;
}


shared_ptr<CGameObject> CFree_SK_Object::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CFree_SK_Object
    {
        MakeSharedEnabler(const CFree_SK_Object& rhs) : CFree_SK_Object(rhs)
        {}
    };


    shared_ptr<CFree_SK_Object> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFree_SK_Object"));
        assert(false);
    }

    return pInstance;
}

void CFree_SK_Object::Free()
{
}
