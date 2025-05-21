#include "VIBuffer_Mesh_Instance.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CVIBuffer_Instance{pDevice, pContext}
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& rhs) :
    CVIBuffer_Instance{rhs}
{
}

CVIBuffer_Mesh_Instance::~CVIBuffer_Mesh_Instance()
{
    Free();
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Prototype(const _char* pModelFilePath, _fmatrix TransformMatrix)
{
    return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Initialize(void* pArg)
{
    return S_OK;
}

shared_ptr<CVIBuffer_Mesh_Instance> CVIBuffer_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	struct MakeSharedEnabler : public CVIBuffer_Mesh_Instance
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CVIBuffer_Mesh_Instance(pDevice, pContext) { }
	};

	shared_ptr<CVIBuffer_Mesh_Instance> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Mesh_Instance"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CVIBuffer_Mesh_Instance::Clone(void* pArg)
{

	struct MakeSharedEnabler : public CVIBuffer_Mesh_Instance
	{
		MakeSharedEnabler(const CVIBuffer_Mesh_Instance& rhs) : CVIBuffer_Mesh_Instance(rhs) { }
	};

	shared_ptr<CVIBuffer_Mesh_Instance> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Mesh_Instance"));
		assert(false);
	}

	return pInstance;
}

void CVIBuffer_Mesh_Instance::Free()
{
}
