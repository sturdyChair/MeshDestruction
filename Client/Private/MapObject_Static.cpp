#include "MapObject_Static.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"

wstring CMapObject_Static::ObjID = TEXT("CMapObject_Static");

CMapObject_Static::CMapObject_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CMapObject_Static::CMapObject_Static(const CMapObject_Static& rhs)
    : CGameObject{rhs}
{
}

CMapObject_Static::~CMapObject_Static()
{
    Free();
}

HRESULT CMapObject_Static::Set_ModelComponent(const string& strPrototypeTag)
{
	_tchar szPrototypeTag[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, strPrototypeTag.c_str(), (_int)strPrototypeTag.length(), szPrototypeTag, MAX_PATH);

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, wstring(szPrototypeTag), TEXT("Com_Model")));

	if (!m_pModelCom)
		assert(false);
	
	return S_OK;
}

HRESULT CMapObject_Static::Initialize_Prototype()
{
	//_float4x4 FMat = {};
	//XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	//
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_HotelEntrance"),
	//		TEXT("../Bin/Resources/Models/Map/Location02/HotelEntrance/04_hotelentrance.fbx"),
	//		MODEL_TYPE::NONANIM, FMat)
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Arcade"),
	//		TEXT("../Bin/Resources/Models/Map/Location02/Arcade/01_arcade.fbx"),
	//		MODEL_TYPE::NONANIM, FMat)
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Street01"),
	//		TEXT("../Bin/Resources/Models/Map/Location02/Street01/02_street01.fbx"),
	//		MODEL_TYPE::NONANIM, FMat)
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Street02"),
	//		TEXT("../Bin/Resources/Models/Map/Location02/Street02/03_street02.fbx"),
	//		MODEL_TYPE::NONANIM, FMat)
	//);

	Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Stage01/"));
	Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/General/"));

    return S_OK;
}

HRESULT CMapObject_Static::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	STATICOBJ_DESC* pDesc = static_cast<STATICOBJ_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	//m_pTransformCom->Set_Scale(XMVectorSet(1000.f, 1000.f, 1000.f, 0.f));
	_float4 vPos = _float4(36.46f, -3.07f, -93.f, 1.f);
	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

    return S_OK;
}

void CMapObject_Static::PriorityTick(_float fTimeDelta)
{
}

void CMapObject_Static::Tick(_float fTimeDelta)
{
}

void CMapObject_Static::LateTick(_float fTimeDelta)
{
	//if (false == GAMEINSTANCE->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), m_fCullingRange)) return;

	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMapObject_Static::Render()
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
		m_pShaderCom->Begin(0);

		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//	return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

    return S_OK;
}

HRESULT CMapObject_Static::Ready_Components(const STATICOBJ_DESC* pDesc)
{	
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	if (pDesc->strModelTag.length() > 0)
	{
		/* For.Com_Model */
		m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, pDesc->strModelTag, TEXT("Com_Model")));

		if (!m_pModelCom)
			assert(false);
	}

	if (!m_pShaderCom /* || !m_pModelCom*/ )
		assert(false);

    return S_OK;
}

HRESULT CMapObject_Static::Ready_ModelPrototypes(const wstring& strLevelDir)
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
				strSectionName = strSectionName.substr(index+1, strSectionName.length());

				wstring strPrototypeTag = strSectionName;

				m_List_ResourceInfo.push_back(
					make_shared<RES_MODEL>(szFileName, wstring(szFullPath), MODEL_TYPE::NONANIM, FMat)
				);
				
			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}

	return S_OK;
}

shared_ptr<CMapObject_Static> CMapObject_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMapObject_Static
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMapObject_Static(pDevice, pContext)
		{}
	};


	shared_ptr<CMapObject_Static> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMapObject_Static"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CMapObject_Static::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMapObject_Static
	{
		MakeSharedEnabler(const CMapObject_Static& rhs) : CMapObject_Static(rhs)
		{}
	};


	shared_ptr<CMapObject_Static> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMapObject_Static"));
		assert(false);
	}

	return pInstance;
}

void CMapObject_Static::Free()
{
}
