#include "MapObject_Anim.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

#include "Beat_Manager.h"

wstring CMapObject_Anim::ObjID = TEXT("CMapObject_Anim");

CMapObject_Anim::CMapObject_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CMapObject_Anim::CMapObject_Anim(const CMapObject_Anim& rhs)
    : CGameObject{rhs}
{
}

CMapObject_Anim::~CMapObject_Anim()
{
    Free();
}

HRESULT CMapObject_Anim::Set_ModelComponent(const string& strPrototypeTag)
{
	_tchar szPrototypeTag[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, strPrototypeTag.c_str(), (_int)strPrototypeTag.length(), szPrototypeTag, MAX_PATH);

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, wstring(szPrototypeTag), TEXT("Com_Model")));

	if (!m_pModelCom)
		assert(false);

	m_pModelCom->Set_AnimIndex(rand() % m_pModelCom->Get_NumAnimation(), true);

    return S_OK;
}

HRESULT CMapObject_Anim::Initialize_Prototype()
{
	Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/AnimObject/"));

    return S_OK;
}

HRESULT CMapObject_Anim::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ANIMOBJ_DESC* pDesc = static_cast<ANIMOBJ_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	//m_pTransformCom->Set_Scale(XMVectorSet(1000.f, 1000.f, 1000.f, 0.f));
	_float4 vPos = _float4(36.46f, -3.07f, -93.f, 1.f);
	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	if(m_pModelCom != nullptr)
	{
		m_iCurrAnimIndex = rand() % m_pModelCom->Get_NumAnimation();
		m_pModelCom->Set_AnimIndex(m_iCurrAnimIndex, true);
	}
	
	CBeat_Manager::Get_Instance()->Set_Beat(0.5f);
	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

    return S_OK;
}

void CMapObject_Anim::PriorityTick(_float fTimeDelta)
{
}

void CMapObject_Anim::Tick(_float fTimeDelta)
{
	_uint iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurrBeat)
	{
		m_iPrevBeat = iCurrBeat;
		m_bPlay = !m_bPlay;
		m_pModelCom->Set_Finished(m_bPlay);
	}

	_float4x4 Mat;
	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 1.f, Mat);
}

void CMapObject_Anim::LateTick(_float fTimeDelta)
{
	if (false == GAMEINSTANCE->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 10.f)) return;

	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMapObject_Anim::Render()
{
	if (!m_pModelCom)
		return S_OK;

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
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
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//	return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

    return S_OK;
}

HRESULT CMapObject_Anim::Ready_Components(const ANIMOBJ_DESC* pDesc)
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

	if (pDesc->strModelTag.length() > 0)
	{
		/* For.Com_Model */
		m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, pDesc->strModelTag, TEXT("Com_Model")));

		if (!m_pModelCom)
			assert(false);
	}

	if (!m_pShaderCom /* || !m_pModelCom*/)
		assert(false);

	return S_OK;
}

HRESULT CMapObject_Anim::Ready_ModelPrototypes(const wstring& strLevelDir)
{
	_float4x4 FMat = {};
	//XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	XMStoreFloat4x4(&FMat, XMMatrixIdentity());

	wstring strFilePath = strLevelDir + L"*.*";

	WIN32_FIND_DATA	fd;
	HANDLE	hFind = FindFirstFile(strFilePath.c_str(), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		BOOL bContinue = true;

		do
		{
			if (!lstrcmp(L".", fd.cFileName) || !lstrcmp(L"..", fd.cFileName))
				continue;

			DWORD dwFileAttribute = fd.dwFileAttributes;

			if (dwFileAttribute == FILE_ATTRIBUTE_DIRECTORY)
			{
				wstring strChildPath = strLevelDir + fd.cFileName + L"/";

				Ready_ModelPrototypes(strChildPath);
			}
			else if (dwFileAttribute == FILE_ATTRIBUTE_ARCHIVE)
			{
				wstring strFileName = fd.cFileName;

				_tchar szFullPath[MAX_PATH] = {};

				_tchar szFileName[MAX_PATH] = {};
				_tchar szExt[MAX_PATH] = {};

				_wsplitpath_s(strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

				if (lstrcmp(L".FBX", szExt) && lstrcmp(L".fbx", szExt))
					continue;

				wcscpy_s(szFullPath, strLevelDir.c_str());
				wcscat_s(szFullPath, strFileName.c_str());

				size_t index = strLevelDir.find_last_of(L'/');
				wstring strSectionName = strLevelDir.substr(0, index);
				index = strSectionName.find_last_of(L'/');
				strSectionName = strSectionName.substr(index + 1, strSectionName.length());

				wstring strPrototypeTag = strSectionName;

				m_List_ResourceInfo.push_back(
					make_shared<RES_MODEL>(szFileName, wstring(szFullPath), MODEL_TYPE::ANIM, FMat)
				);

			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}

	return S_OK;

}

shared_ptr<CMapObject_Anim> CMapObject_Anim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMapObject_Anim
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMapObject_Anim(pDevice, pContext)
		{}
	};


	shared_ptr<CMapObject_Anim> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMapObject_Anim"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CMapObject_Anim::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMapObject_Anim
	{
		MakeSharedEnabler(const CMapObject_Anim& rhs) : CMapObject_Anim(rhs)
		{}
	};


	shared_ptr<CMapObject_Anim> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMapObject_Anim"));
		assert(false);
	}

	return pInstance;
}

void CMapObject_Anim::Free()
{
}
