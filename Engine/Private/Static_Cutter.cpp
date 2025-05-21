#include "..\Public\Static_Cutter.h"
#include "Model.h"
#include "Mesh.h"
#include "Bone.h"

_uint CStatic_Cutter::s_iSplitCount = 0;
const _wstring CStatic_Cutter::s_PrototypeTag = L"Prototype_Component_Static_Cutter";

CStatic_Cutter::CStatic_Cutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CStatic_Cutter::CStatic_Cutter(const CStatic_Cutter& rhs)
	: CComponent(rhs), m_pComputeShader(rhs.m_pComputeShader), m_Blob(rhs.m_Blob), m_pCutPlaneBuffer(rhs.m_pCutPlaneBuffer)
{
}

CStatic_Cutter::~CStatic_Cutter()
{
	Free();
}

HRESULT CStatic_Cutter::Initialize_Prototype(const _tchar* pComputeShaderFilePath)
{
	_uint iHlslFlag = 0;
#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG
	ID3DBlob* error = nullptr;
	HRESULT rt = D3DCompileFromFile(
		pComputeShaderFilePath,
		nullptr, nullptr,
		"main", "cs_5_0",
		0, 0, &m_Blob, &error
	);
	if (FAILED(rt))
	{
		auto pBuffer = error->GetBufferPointer();
		char* szBuffer = (char*)pBuffer;
	}
	else
		Safe_Release(error);

	m_pDevice->CreateComputeShader(
		m_Blob->GetBufferPointer(), m_Blob->GetBufferSize(),
		nullptr, &m_pComputeShader
	);
#pragma region CONSTANT
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.ByteWidth = sizeof(_float4);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pCutPlaneBuffer);
#pragma endregion
	return S_OK;
}

HRESULT CStatic_Cutter::Initialize(void* pArg)
{
	MODEL_CUTTER_DESC* pDesc = (MODEL_CUTTER_DESC*)(pArg);
	m_pModel = pDesc->pModel;
	m_pModelData = m_pModel->Get_ModelData();
	m_iMeshIdx = pDesc->iMeshIdx;


	HRESULT rt;
#pragma region VTX_SRV
	{
		D3D11_BUFFER_DESC bufDesc = {};
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.ByteWidth = sizeof(VTXMESH) * m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumVertices;
		bufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufDesc.StructureByteStride = sizeof(VTXMESH);
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_pModelData->Mesh_Datas[m_iMeshIdx]->pVertices.get();

		rt = m_pDevice->CreateBuffer(&bufDesc, &initData, &m_pInputVertexBuffer);

		// SRV 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementWidth = sizeof(VTXMESH);//m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumVertices;//이거 진짜에요?
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.NumElements = m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumVertices;

		rt = m_pDevice->CreateShaderResourceView(m_pInputVertexBuffer, &srvDesc, &m_pInputVertexSRV);
		if (FAILED(rt))
			throw rt;
	}
#pragma endregion
#pragma region IDX_SRV
	{
		D3D11_BUFFER_DESC bufDesc = {};
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.ByteWidth = sizeof(FACEINDICES32) * m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumFaces;
		bufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufDesc.StructureByteStride = sizeof(FACEINDICES32);
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_pModelData->Mesh_Datas[m_iMeshIdx]->pIndices.get();

		m_pDevice->CreateBuffer(&bufDesc, &initData, &m_pInputIndexBuffer);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementWidth = sizeof(FACEINDICES32);//m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumFaces;//이거 진짜에요?
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.NumElements = m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumFaces;

		rt = m_pDevice->CreateShaderResourceView(m_pInputIndexBuffer, &srvDesc, &m_pInputIndexSRV);
		if (FAILED(rt))
			throw rt;
	}
#pragma endregion
#pragma region UAV
	_uint maxOutputCount = m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumFaces * 3;
	D3D11_BUFFER_DESC outDesc = {};
	outDesc.Usage = D3D11_USAGE_DEFAULT;
	outDesc.ByteWidth = sizeof(_uint3) * maxOutputCount;
	outDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outDesc.StructureByteStride = sizeof(_uint3);
	outDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	m_pDevice->CreateBuffer(&outDesc, nullptr, &m_pOutputUpBuffer);
	m_pDevice->CreateBuffer(&outDesc, nullptr, &m_pOutputDownBuffer);

	// UAV (AppendBuffer)
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = maxOutputCount;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	rt = m_pDevice->CreateUnorderedAccessView(m_pOutputUpBuffer, &uavDesc, &m_pOutputUpUAV);
	if (FAILED(rt))
		throw rt;
	rt = m_pDevice->CreateUnorderedAccessView(m_pOutputDownBuffer, &uavDesc, &m_pOutputDownUAV);
	if (FAILED(rt))
		throw rt;
	outDesc.Usage = D3D11_USAGE_DEFAULT;
	outDesc.ByteWidth = sizeof(VTXMESH) * maxOutputCount;
	outDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outDesc.StructureByteStride = sizeof(VTXMESH);
	outDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	m_pDevice->CreateBuffer(&outDesc, nullptr, &m_pOutputNewVertBuffer);


	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = maxOutputCount;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	rt = m_pDevice->CreateUnorderedAccessView(m_pOutputNewVertBuffer, &uavDesc, &m_pOutputNewVerUAV);
	if (FAILED(rt))
		throw rt;
#pragma endregion

#pragma region STAGING
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(VTXMESH) * maxOutputCount;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.StructureByteStride = sizeof(VTXMESH);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	m_pDevice->CreateBuffer(&desc, nullptr, &m_pStagingBuffer);

	desc.ByteWidth = sizeof(_uint);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.StructureByteStride = sizeof(_uint);
	desc.MiscFlags = 0;
	m_pDevice->CreateBuffer(&desc, nullptr, &m_pCountBuffer);

	desc = {};
	desc.ByteWidth = sizeof(_uint3) * maxOutputCount;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.StructureByteStride = sizeof(_uint3);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	m_pDevice->CreateBuffer(&desc, nullptr, &m_pStagingIndiciesBuffer);
#pragma endregion
	return S_OK;
}

pair<vector<_uint3>, vector<_uint3>>  CStatic_Cutter::Cut(const _float4& Plane, vector<VTXMESH>& out)
{
	m_CutPlane = Plane;
	Bind_CutPlane();
	Shader_Compute();
	Get_NewVertices(out);
	return Rebuild_Mesh();
}

void CStatic_Cutter::Shader_Compute()
{
	m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);

	// 자원 바인딩
	ID3D11ShaderResourceView* SRV[2]{ m_pInputVertexSRV, m_pInputIndexSRV };
	m_pContext->CSSetShaderResources(0, 2, SRV);

	ID3D11UnorderedAccessView* UAV[3]{ m_pOutputUpUAV, m_pOutputDownUAV, m_pOutputNewVerUAV };
	_uint InitCount[3]{ 0,0,0 };
	m_pContext->CSSetUnorderedAccessViews(0, 3, UAV, InitCount);

	ID3D11Buffer* CB[1]{ m_pCutPlaneBuffer };
	m_pContext->CSSetConstantBuffers(0, 1, CB);

	// Dispatch
	UINT dispatchCount = (m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumFaces + 63) / 64;
	m_pContext->Dispatch(dispatchCount, 1, 1);

	// UAV 언바인딩
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
}

void CStatic_Cutter::Bind_CutPlane()
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	m_pContext->Map(m_pCutPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &m_CutPlane, sizeof(_float4));
	m_pContext->Unmap(m_pCutPlaneBuffer, 0);
}

void CStatic_Cutter::Get_NewVertices(vector<VTXMESH>& out)
{
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	m_pContext->CopyStructureCount(m_pCountBuffer, 0, m_pOutputNewVerUAV);
	m_pContext->Map(m_pCountBuffer, 0, D3D11_MAP_READ, 0, &mapped);
	if (mapped.pData == nullptr)
		return;

	_uint iVertexCount = (*(_uint*)mapped.pData);
	m_pContext->Unmap(m_pCountBuffer, 0);

	if (iVertexCount)
	{
		m_pContext->CopyResource(m_pStagingBuffer, m_pOutputNewVertBuffer);
		m_pContext->Map(m_pStagingBuffer, 0, D3D11_MAP_READ, 0, &mapped);

		VTXMESH* pVertices = (VTXMESH*)(mapped.pData);
		out = vector<VTXMESH>(pVertices, pVertices + iVertexCount);
		m_pContext->Unmap(m_pStagingBuffer, 0);
	}

}

pair<vector<_uint3>, vector<_uint3>> CStatic_Cutter::Rebuild_Mesh()
{
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	m_pContext->CopyStructureCount(m_pCountBuffer, 0, m_pOutputUpUAV);
	m_pContext->Map(m_pCountBuffer, 0, D3D11_MAP_READ, 0, &mapped);
	if (mapped.pData == nullptr)
		return {};
	_uint iVertexCountUp = (*(_uint*)mapped.pData);
	m_pContext->Unmap(m_pCountBuffer, 0);

	m_pContext->CopyStructureCount(m_pCountBuffer, 0, m_pOutputDownUAV);
	m_pContext->Map(m_pCountBuffer, 0, D3D11_MAP_READ, 0, &mapped);
	if (mapped.pData == nullptr)
		return {};
	_uint iVertexCountDown = (*(_uint*)mapped.pData);
	m_pContext->Unmap(m_pCountBuffer, 0);

	if (iVertexCountUp <= 1 || iVertexCountDown <= 1)
		return {};

	return { Build_Mesh_Internal(m_pOutputUpUAV, m_pOutputUpBuffer, iVertexCountUp) ,Build_Mesh_Internal(m_pOutputDownUAV, m_pOutputDownBuffer, iVertexCountDown) };
}

vector<_uint3> CStatic_Cutter::Build_Mesh_Internal(ID3D11UnorderedAccessView* pUAV, ID3D11Buffer* pBuffer, _uint iVertexCount)
{
	m_pContext->CopyResource(m_pStagingIndiciesBuffer, pBuffer);

	D3D11_MAPPED_SUBRESOURCE mapped = {};

	//---------------------Copy Vertices-------------------
	m_pContext->Map(m_pStagingIndiciesBuffer, 0, D3D11_MAP_READ, 0, &mapped);

	_uint3* pIndicies = (_uint3*)(mapped.pData);
	vector<_uint3> rt(pIndicies, pIndicies + iVertexCount);

	m_pContext->Unmap(m_pStagingIndiciesBuffer, 0);
	//-----------------------------------------------------
	return rt;
}



shared_ptr<CStatic_Cutter> CStatic_Cutter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pComputeShaderFilePath)
{
	struct MakeSharedEnabler : public CStatic_Cutter
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CStatic_Cutter(pDevice, pContext) {}
	};

	shared_ptr<CStatic_Cutter> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pComputeShaderFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CStatic_Cutter"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CStatic_Cutter::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CStatic_Cutter
	{
		MakeSharedEnabler(const CStatic_Cutter& rhs) : CStatic_Cutter(rhs) {}
	};

	shared_ptr<CStatic_Cutter> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CStatic_Cutter"));
		return nullptr;
	}

	return pInstance;
}

void CStatic_Cutter::Free()
{
	Safe_Release(m_pInputVertexBuffer);
	Safe_Release(m_pInputVertexSRV);

	Safe_Release(m_pInputIndexBuffer);
	Safe_Release(m_pInputIndexSRV);

	Safe_Release(m_pOutputUpBuffer);
	Safe_Release(m_pOutputUpUAV);

	Safe_Release(m_pOutputDownBuffer);
	Safe_Release(m_pOutputDownUAV);

	Safe_Release(m_pOutputNewVertBuffer);
	Safe_Release(m_pOutputNewVerUAV);

	Safe_Release(m_pStagingBuffer);
	Safe_Release(m_pStagingIndiciesBuffer);

	Safe_Release(m_pOutputNewVerUAV);
	Safe_Release(m_pOutputNewVertBuffer);


	if (!m_isCloned)
	{
		Safe_Release(m_pComputeShader);
		Safe_Release(m_Blob);
		Safe_Release(m_pCutPlaneBuffer);
	}
}
