#include "ResourceContainer.h"
#include "Texture.h"

CResource_Container::CResource_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CResource_Container::~CResource_Container()
{
	Free();	
}

void CResource_Container::Initialize()
{

}

void CResource_Container::Tick(_float fTimeDelta)
{

}

void CResource_Container::Clear()
{

}

void CResource_Container::Render()
{

}

shared_ptr<CTexture> CResource_Container::SearchMaterial(const _wstring& pMaterialTag)
{
	auto iter = m_MaterialList.find(pMaterialTag);

	if (iter == m_MaterialList.end())
	{
		shared_ptr<CTexture> pMaterial = CTexture::Create(m_pDevice, m_pContext, pMaterialTag);

		m_MaterialList.insert(make_pair(pMaterialTag, pMaterial));

		return pMaterial;
	}
	else {
		return iter->second;
	}
}

shared_ptr<CResource_Container> CResource_Container::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CResource_Container
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CResource_Container(pDevice, pContext) { }
	};

	shared_ptr<CResource_Container> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	pInstance->Initialize();

	return pInstance;
}

void CResource_Container::Free()
{

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

}
