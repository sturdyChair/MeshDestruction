#include "ResourceWizard.h"
#include "Component_Manager.h"

void CResourceWizard::LoadResources(const wstring& strProtypeTag, LEVEL level, weak_ptr<CGameInstance> pGameInstance, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	auto pObject = pGameInstance.lock()->Get_Object_Prototype(strProtypeTag);
	for (auto pResInfo : pObject->Get_ResourceInfo())
	{
		// Check duplicated resources
		if (nullptr != pGameInstance.lock()->Get_Component_Prototype(level, pResInfo->strTag))
			continue;

		if (pResInfo->type == RESOURCE_INFO::E_Res_Type::TEXTURE)
		{
			auto pResTex = static_pointer_cast<RES_TEXTURE>(pResInfo);
			pGameInstance.lock()->Add_Prototype(level, pResTex->strTag, CTexture::Create(pDevice, pContext, pResTex->strFilePath, pResTex->iNumTexture));
		}
		else if (pResInfo->type == RESOURCE_INFO::E_Res_Type::MODEL)
		{
			auto pResModel = static_pointer_cast<RES_MODEL>(pResInfo);

			string strPath = string(pResModel->strFilePath.begin(), pResModel->strFilePath.end());
			pGameInstance.lock()->Add_Prototype(level, pResModel->strTag, CModel::Create(pDevice, pContext, pResModel->modelType, strPath.c_str(), XMLoadFloat4x4(&pResModel->Transform)));
		}
		//else if (pResInfo->type == RESOURCE_INFO::E_Res_Type::SOUND)
		//{
		//	auto pResSound = static_pointer_cast<RES_SOUND>(pResInfo);
		//	pGameInstance.lock()->Add_Sound(pResSound->eSoundType, pResSound->strTag, pResSound->strFilePath);
		//	//pGameInstance.lock()->Add_Prototype(level, pResSound->strTag, CSoundComponent::Create(pResSound->strFilePath, pResSound->eSoundType));
		//}
		//else if (pResInfo->type == RESOURCE_INFO::E_Res_Type::SOUND_3D)
		//{
		//	auto pResSound = static_pointer_cast<RES_SOUND>(pResInfo);
		//	pGameInstance.lock()->Add_Sound_3D(pResSound->eSoundType, pResSound->strTag, pResSound->strFilePath);
		//}
	}

	// Pooling
	for (_uint i = 0; i < pObject->Get_PoolSize(); ++i)
	{
		pGameInstance.lock()->Pool_Object(strProtypeTag);
	}

	for (auto strObjID : pObject->Get_Owning_Objects())
	{
		LoadResources(strObjID, level, pGameInstance, pDevice, pContext);
	}
}
