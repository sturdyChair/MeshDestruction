#include "..\Public\Snapper.h"
#include "Model.h"
#include "Mesh.h"
#include "Bone.h"

_uint CSnapper::s_iSplitCount = 0;
const _wstring CSnapper::s_PrototypeTag = L"Prototype_Component_Model_Snapper";

CSnapper::CSnapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CSnapper::CSnapper(const CSnapper& rhs)
	: CComponent(rhs), m_pComputeShader(rhs.m_pComputeShader), m_Blob(rhs.m_Blob),
	m_pBoneBuffer(rhs.m_pBoneBuffer)
{
}

CSnapper::~CSnapper()
{
	Free();
}

HRESULT CSnapper::Initialize_Prototype(const _tchar* pComputeShaderFilePath)
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

	cbDesc.ByteWidth = sizeof(_float4x4) * 512;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pBoneBuffer);
#pragma endregion
	return S_OK;
}

HRESULT CSnapper::Initialize(void* pArg)
{
	MODEL_SNAP_DESC* pDesc = (MODEL_SNAP_DESC*)(pArg);
	m_pModel = pDesc->pModel;
	m_pModelData = m_pModel->Get_ModelData();
	m_iMeshIdx = pDesc->iMeshIdx;

	HRESULT rt;
#pragma region VTX_SRV
	{
		D3D11_BUFFER_DESC bufDesc = {};
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.ByteWidth = sizeof(VTXANIMMESH) * m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumVertices;
		bufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufDesc.StructureByteStride = sizeof(VTXANIMMESH);
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_pModelData->Mesh_Datas[m_iMeshIdx]->pAnimVertices.get();

		rt = m_pDevice->CreateBuffer(&bufDesc, &initData, &m_pInputVertexBuffer);

		// SRV 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementWidth = sizeof(VTXANIMMESH);//m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumVertices;//이거 진짜에요?
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.NumElements = m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumVertices;

		rt = m_pDevice->CreateShaderResourceView(m_pInputVertexBuffer, &srvDesc, &m_pInputVertexSRV);
		if (FAILED(rt))
			throw;
	}
#pragma region UAV
	_uint maxOutputCount = m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumFaces * 3;
	D3D11_BUFFER_DESC outDesc = {};
	outDesc.Usage = D3D11_USAGE_DEFAULT;
	outDesc.ByteWidth = sizeof(VTXMESH) * maxOutputCount;
	outDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outDesc.StructureByteStride = sizeof(VTXMESH);
	outDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	m_pDevice->CreateBuffer(&outDesc, nullptr, &m_pOutputBuffer);


	// UAV (AppendBuffer)
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = maxOutputCount;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

	rt = m_pDevice->CreateUnorderedAccessView(m_pOutputBuffer, &uavDesc, &m_pOutputUAV);
	if (FAILED(rt))
		throw;
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


#pragma endregion
	return S_OK;
}

vector<VTXMESH> CSnapper::Snap()
{
	Bind_Bones();
	Shader_Compute();

	return Get_Vertices();
}


void CSnapper::Shader_Compute()
{
	m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);

	// 자원 바인딩
	ID3D11ShaderResourceView* SRV[1]{ m_pInputVertexSRV };
	m_pContext->CSSetShaderResources(0, 1, SRV);

	ID3D11UnorderedAccessView* UAV[1]{ m_pOutputUAV}; // , m_pOutputNewEdgeUAV
	_uint InitCount[1]{ 0};
	m_pContext->CSSetUnorderedAccessViews(0, 1, UAV, InitCount);

	ID3D11Buffer* CB[1]{ m_pBoneBuffer };
	m_pContext->CSSetConstantBuffers(0, 1, CB);

	// Dispatch
	UINT dispatchCount = (m_pModelData->Mesh_Datas[m_iMeshIdx]->iNumVertices + 63) / 64;
	m_pContext->Dispatch(dispatchCount, 1, 1);

	// UAV 언바인딩
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
}


void CSnapper::Bind_Bones()
{
	_float4x4 Bone[512];
	m_pModel->Get_Mesh(m_iMeshIdx)->SetUp_BoneMatrices(Bone, m_pModel->Get_Bones());
	D3D11_MAPPED_SUBRESOURCE mapped;
	m_pContext->Map(m_pBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &Bone, sizeof(_float4x4) * 512);
	m_pContext->Unmap(m_pBoneBuffer, 0);
}

vector<VTXMESH> CSnapper::Get_Vertices()
{
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	m_pContext->CopyStructureCount(m_pCountBuffer, 0, m_pOutputUAV);
	m_pContext->Map(m_pCountBuffer, 0, D3D11_MAP_READ, 0, &mapped);
	_uint iVertexCount = (*(_uint*)mapped.pData);
	m_pContext->Unmap(m_pCountBuffer, 0);
	vector<VTXMESH> out;
	//if (iVertexCount)
	//{
		m_pContext->CopyResource(m_pStagingBuffer, m_pOutputBuffer);
		m_pContext->Map(m_pStagingBuffer, 0, D3D11_MAP_READ, 0, &mapped);

		VTXMESH* pVertices = (VTXMESH*)(mapped.pData);
		out = vector<VTXMESH>(pVertices, pVertices + iVertexCount);
		m_pContext->Unmap(m_pStagingBuffer, 0);
	//}
	return out;
}



shared_ptr<CSnapper> CSnapper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pComputeShaderFilePath)
{
	struct MakeSharedEnabler : public CSnapper
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSnapper(pDevice, pContext) {}
	};

	shared_ptr<CSnapper> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pComputeShaderFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CSnapper"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CSnapper::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CSnapper
	{
		MakeSharedEnabler(const CSnapper& rhs) : CSnapper(rhs) {}
	};

	shared_ptr<CSnapper> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSnapper"));
		return nullptr;
	}

	return pInstance;
}

void CSnapper::Free()
{
	Safe_Release(m_pInputVertexBuffer);
	Safe_Release(m_pInputVertexSRV);

	Safe_Release(m_pOutputBuffer);
	Safe_Release(m_pOutputUAV);


	Safe_Release(m_pStagingBuffer);

	//Safe_Release(m_pOutputNewEdgeBuffer);
	//Safe_Release(m_pOutputNewEdgeUAV);


	if (!m_isCloned)
	{
		Safe_Release(m_pComputeShader);
		Safe_Release(m_Blob);
		Safe_Release(m_pBoneBuffer);
	}
}
