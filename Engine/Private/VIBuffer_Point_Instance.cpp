#include "..\Public\VIBuffer_Point_Instance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs)
	: CVIBuffer_Instance(rhs), m_pInstanceVertex(rhs.m_pInstanceVertex), m_iMaxPoint(rhs.m_iMaxPoint)
{
}

CVIBuffer_Point_Instance::~CVIBuffer_Point_Instance()
{
	Free();
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const INSTANCE_DESC* desc)
{
	__super::Initialize_Prototype(desc);
	m_iMaxPoint = m_tDesc.iNumInstance;
	m_iNumIndexPerInstance = 1;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iIndexStride = 2;
	m_iNumVertices = 1;
	m_iNumIndices = m_iNumIndexPerInstance * m_tDesc.iNumInstance;
	m_iNumVertexBuffers = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	/* 생성하고자하는 버퍼의 속성을 설정하낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 생성하고자하는 버퍼의 초기값을 설정한다. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = XMFLOAT3(0.f, 0.f, 0.f);
	pVertices[0].vSize = XMFLOAT2(1.f, 1.f);

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER

	m_pInstanceVertex = new VTXMATRIX[m_tDesc.iNumInstance];
	ZeroMemory(m_pInstanceVertex, sizeof(VTXMATRIX) * m_tDesc.iNumInstance);
	_float2& vScale = m_tDesc.vScale;
	_float2& vLT = m_tDesc.vLifeTime;
	_float3& vRange = m_tDesc.vRange;
	_float3& vCenter = m_tDesc.vCenter;
	m_vecInitPos = new _float4[m_tDesc.iNumInstance];
	m_vecTorque = new _float3[m_tDesc.iNumInstance];
	for (size_t i = 0; i < m_tDesc.iNumInstance; ++i)
	{

		_float fScale = fRand(vScale.x, vScale.y);
		_float fLT = fRand(vLT.x, vLT.y);
		m_pInstanceVertex[i].vRight = { fScale,0.f,0.f,0.f };
		m_pInstanceVertex[i].vUp = { 0.f,fScale,0.f,0.f };
		m_pInstanceVertex[i].vLook = { 0.f,0.f,fScale,0.f };
		m_pInstanceVertex[i].vTranslation =
		{
			vCenter.x + fRand(-vRange.x * .5f, vRange.x * .5f),
			vCenter.y + fRand(-vRange.y * .5f, vRange.y * .5f),
			vCenter.z + fRand(-vRange.z * .5f, vRange.z * .5f),
			1.f };
		m_vecInitPos[i] = m_pInstanceVertex[i].vTranslation;
		m_vecTorque[i].x = fRand(-1.f, 1.f) * m_tDesc.vTorque.x;
		m_vecTorque[i].y = fRand(-1.f, 1.f) * m_tDesc.vTorque.y;
		m_vecTorque[i].z = fRand(-1.f, 1.f) * m_tDesc.vTorque.z;
		m_pInstanceVertex[i].vLifeTime.x = fLT;
		m_pInstanceVertex[i].vLifeTime.y = 0.f;
	}

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void* pArg)
{
#pragma region INSTANCE_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_iInstanceVertexStride = sizeof(VTXMATRIX);
	m_BufferDesc.ByteWidth = m_iInstanceVertexStride * m_tDesc.iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pInstanceVertex;
	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
	{
		return E_FAIL;
	}

	//Safe_Delete_Array(pInstance);

#pragma endregion

	return S_OK;
}

void CVIBuffer_Point_Instance::Update_PtInstance_Style(_float fTimeDelta)
{
	switch (m_eStyle)
	{
	case STYLE_SPREAD:
		Update_Spread(fTimeDelta);
		break;
	case STYLE_DROP:
		Update_Drop(fTimeDelta);
		break;
	case STYLE_FOUNTAIN:
		Update_Fountain(fTimeDelta);
		break;
	case STYLE_RISING:
		Update_Rising(fTimeDelta);
		break;
	case STYLE_CYCLE:
		Update_Cycle(fTimeDelta);
		break;
	case STYLE_ORBITAL:
		Update_Orbital(fTimeDelta);
		break;
	}
}

void CVIBuffer_Point_Instance::Update_Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX* pVertices = static_cast<VTXMATRIX*>(SubResource.pData);

	_matrix Transform = XMMatrixIdentity();
	_matrix Rotation = XMMatrixIdentity();
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		Rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_vecTorque[i]) * fTimeDelta);
		Transform.r[0] = XMLoadFloat4(&pVertices[i].vRight);
		Transform.r[1] = XMLoadFloat4(&pVertices[i].vUp);
		Transform.r[2] = XMLoadFloat4(&pVertices[i].vLook);
		Transform.r[3] = XMLoadFloat4(&pVertices[i].vTranslation);

		_vector VertexPos = XMLoadFloat4(&pVertices[i].vTranslation);
		_vector vDir = Transform.r[3] - XMLoadFloat3(&m_tDesc.vPivot);
		vDir = XMVector3Normalize(XMVectorSetW(vDir, 0.f)) * m_pSpeeds[i] * fTimeDelta;
		XMStoreFloat4(&pVertices[i].vTranslation, Transform.r[3] + vDir);
		Transform = Rotation * Transform;
		XMStoreFloat4(&pVertices[i].vRight, Transform.r[0]);
		XMStoreFloat4(&pVertices[i].vUp, Transform.r[1]);
		XMStoreFloat4(&pVertices[i].vLook, Transform.r[2]);

		pVertices[i].vLifeTime.y += fTimeDelta;
		if (m_tDesc.bLoop && pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.y = 0.f;
			_matrix InitTransform = XMMatrixIdentity();
			InitTransform.r[0] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight)));
			InitTransform.r[1] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp)));
			InitTransform.r[2] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook)));
			InitTransform.r[3] = XMLoadFloat4(&m_vecInitPos[i]);

			XMStoreFloat4(&pVertices[i].vRight, InitTransform.r[0]);
			XMStoreFloat4(&pVertices[i].vUp, InitTransform.r[1]);
			XMStoreFloat4(&pVertices[i].vLook, InitTransform.r[2]);
			XMStoreFloat4(&pVertices[i].vTranslation, InitTransform.r[3]);
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Update_Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX* pVertices = static_cast<VTXMATRIX*>(SubResource.pData);

	_matrix Transform = XMMatrixIdentity();
	_matrix Rotation = XMMatrixIdentity();
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		Rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_vecTorque[i]) * fTimeDelta);
		Transform.r[0] = XMLoadFloat4(&pVertices[i].vRight);
		Transform.r[1] = XMLoadFloat4(&pVertices[i].vUp);
		Transform.r[2] = XMLoadFloat4(&pVertices[i].vLook);
		Transform.r[3] = XMLoadFloat4(&pVertices[i].vTranslation);

		_vector VertexPos = XMLoadFloat4(&pVertices[i].vTranslation);
		_vector vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
		vDir = XMVector3Normalize(XMVectorSetW(vDir, 0.f)) * m_pSpeeds[i] * fTimeDelta;
		XMStoreFloat4(&pVertices[i].vTranslation, Transform.r[3] + vDir);
		Transform = Rotation * Transform;
		XMStoreFloat4(&pVertices[i].vRight, Transform.r[0]);
		XMStoreFloat4(&pVertices[i].vUp, Transform.r[1]);
		XMStoreFloat4(&pVertices[i].vLook, Transform.r[2]);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (m_tDesc.bLoop && pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.y = 0.f;
			_matrix InitTransform = XMMatrixIdentity();
			InitTransform.r[0] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight)));
			InitTransform.r[1] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp)));
			InitTransform.r[2] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook)));
			InitTransform.r[3] = XMLoadFloat4(&m_vecInitPos[i]);

			XMStoreFloat4(&pVertices[i].vRight, InitTransform.r[0]);
			XMStoreFloat4(&pVertices[i].vUp, InitTransform.r[1]);
			XMStoreFloat4(&pVertices[i].vLook, InitTransform.r[2]);
			XMStoreFloat4(&pVertices[i].vTranslation, InitTransform.r[3]);
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Update_Fountain(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX* pVertices = static_cast<VTXMATRIX*>(SubResource.pData);

	_matrix Transform = XMMatrixIdentity();
	_matrix Rotation = XMMatrixIdentity();
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		Rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_vecTorque[i]) * fTimeDelta);
		Transform.r[0] = XMLoadFloat4(&pVertices[i].vRight);
		Transform.r[1] = XMLoadFloat4(&pVertices[i].vUp);
		Transform.r[2] = XMLoadFloat4(&pVertices[i].vLook);
		Transform.r[3] = XMLoadFloat4(&pVertices[i].vTranslation);

		_vector VertexPos = XMLoadFloat4(&pVertices[i].vTranslation);
		_vector vDir = Transform.r[3] - XMLoadFloat3(&m_tDesc.vPivot);
		vDir = XMVectorSetY(vDir, 10.f - 20.f * pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x);
		vDir = XMVector3Normalize(XMVectorSetW(vDir, 0.f)) * m_pSpeeds[i] * fTimeDelta;
		XMStoreFloat4(&pVertices[i].vTranslation, Transform.r[3] + vDir);
		Transform = Rotation * Transform;
		XMStoreFloat4(&pVertices[i].vRight, Transform.r[0]);
		XMStoreFloat4(&pVertices[i].vUp, Transform.r[1]);
		XMStoreFloat4(&pVertices[i].vLook, Transform.r[2]);

		pVertices[i].vLifeTime.y += fTimeDelta;
		if (m_tDesc.bLoop && pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.y = 0.f;
			_matrix InitTransform = XMMatrixIdentity();
			InitTransform.r[0] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight)));
			InitTransform.r[1] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp)));
			InitTransform.r[2] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook)));
			InitTransform.r[3] = XMLoadFloat4(&m_vecInitPos[i]);

			XMStoreFloat4(&pVertices[i].vRight, InitTransform.r[0]);
			XMStoreFloat4(&pVertices[i].vUp, InitTransform.r[1]);
			XMStoreFloat4(&pVertices[i].vLook, InitTransform.r[2]);
			XMStoreFloat4(&pVertices[i].vTranslation, InitTransform.r[3]);
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Update_Rising(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX* pVertices = static_cast<VTXMATRIX*>(SubResource.pData);

	_matrix Transform = XMMatrixIdentity();
	_matrix Rotation = XMMatrixIdentity();
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		Rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_vecTorque[i]) * fTimeDelta);
		Transform.r[0] = XMLoadFloat4(&pVertices[i].vRight);
		Transform.r[1] = XMLoadFloat4(&pVertices[i].vUp);
		Transform.r[2] = XMLoadFloat4(&pVertices[i].vLook);
		Transform.r[3] = XMLoadFloat4(&pVertices[i].vTranslation);

		_vector VertexPos = XMLoadFloat4(&pVertices[i].vTranslation);
		_vector vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		vDir = XMVector3Normalize(XMVectorSetW(vDir, 0.f)) * m_pSpeeds[i] * fTimeDelta;
		XMStoreFloat4(&pVertices[i].vTranslation, Transform.r[3] + vDir);
		Transform = Rotation * Transform;
		XMStoreFloat4(&pVertices[i].vRight, Transform.r[0]);
		XMStoreFloat4(&pVertices[i].vUp, Transform.r[1]);
		XMStoreFloat4(&pVertices[i].vLook, Transform.r[2]);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (m_tDesc.bLoop && pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.y = 0.f;
			_matrix InitTransform = XMMatrixIdentity();
			InitTransform.r[0] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight)));
			InitTransform.r[1] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp)));
			InitTransform.r[2] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook)));
			InitTransform.r[3] = XMLoadFloat4(&m_vecInitPos[i]);

			XMStoreFloat4(&pVertices[i].vRight, InitTransform.r[0]);
			XMStoreFloat4(&pVertices[i].vUp, InitTransform.r[1]);
			XMStoreFloat4(&pVertices[i].vLook, InitTransform.r[2]);
			XMStoreFloat4(&pVertices[i].vTranslation, InitTransform.r[3]);
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Update_Cycle(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX* pVertices = static_cast<VTXMATRIX*>(SubResource.pData);

	_matrix Transform = XMMatrixIdentity();
	_vector Rotation;
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		Rotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&m_vecTorque[i]) * fTimeDelta);
		Transform.r[0] = XMLoadFloat4(&pVertices[i].vRight);
		Transform.r[1] = XMLoadFloat4(&pVertices[i].vUp);
		Transform.r[2] = XMLoadFloat4(&pVertices[i].vLook);
		Transform.r[3] = XMLoadFloat4(&pVertices[i].vTranslation);

		_vector vDir = Transform.r[3] - XMLoadFloat3(&m_tDesc.vPivot);
		Transform.r[3] = XMVectorSetW(XMVector3Rotate(vDir, Rotation), 1.f);

		//vDir = XMVector3Normalize(XMVectorSetW(vDir, 0.f)) * m_pSpeeds[i];
		XMStoreFloat4(&pVertices[i].vTranslation, Transform.r[3]);
		//Transform = Rotation * Transform;

		pVertices[i].vLifeTime.y += fTimeDelta;
		if (m_tDesc.bLoop && pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.y = 0.f;
			_matrix InitTransform = XMMatrixIdentity();
			InitTransform.r[0] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight)));
			InitTransform.r[1] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp)));
			InitTransform.r[2] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook)));
			InitTransform.r[3] = XMLoadFloat4(&m_vecInitPos[i]);

			XMStoreFloat4(&pVertices[i].vRight, InitTransform.r[0]);
			XMStoreFloat4(&pVertices[i].vUp, InitTransform.r[1]);
			XMStoreFloat4(&pVertices[i].vLook, InitTransform.r[2]);
			XMStoreFloat4(&pVertices[i].vTranslation, InitTransform.r[3]);
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Update_Orbital(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX* pVertices = static_cast<VTXMATRIX*>(SubResource.pData);
	_vector vYaw = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_matrix Transform = XMMatrixIdentity();
	_matrix Rotation;
	_vector vPivot = XMVectorSetW(XMLoadFloat3(&m_tDesc.vPivot), 1.f);
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		Rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_vecTorque[i]) * fTimeDelta);
		Transform.r[0] = XMLoadFloat4(&pVertices[i].vRight);
		Transform.r[1] = XMLoadFloat4(&pVertices[i].vUp);
		Transform.r[2] = XMLoadFloat4(&pVertices[i].vLook);
		Transform.r[3] = XMLoadFloat4(&pVertices[i].vTranslation);


		_vector vDir = XMLoadFloat4(&m_vecInitPos[i]) - vPivot;
		_vector vRight = XMVector3Cross(vYaw, vDir);
		_vector vRotQut = XMQuaternionRotationAxis(vRight, fTimeDelta * m_pSpeeds[i]);

		Transform.r[3] = vPivot + XMVector3Rotate(Transform.r[3] - vPivot, vRotQut);

		//vDir = XMVector3Normalize(XMVectorSetW(vDir, 0.f)) * m_pSpeeds[i];
		XMStoreFloat4(&pVertices[i].vTranslation, Transform.r[3]);
		Transform = Rotation * Transform;
		XMStoreFloat4(&pVertices[i].vRight, Transform.r[0]);
		XMStoreFloat4(&pVertices[i].vUp, Transform.r[1]);
		XMStoreFloat4(&pVertices[i].vLook, Transform.r[2]);

		//pVertices[i].vLifeTime.y += fTimeDelta;
		//if (m_tDesc.bLoop && pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
		//{
		//	pVertices[i].vLifeTime.y = 0.f;
		//	_matrix InitTransform = XMMatrixIdentity();
		//	InitTransform.r[0] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight)));
		//	InitTransform.r[1] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp)));
		//	InitTransform.r[2] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook)));
		//	InitTransform.r[3] = XMLoadFloat4(&m_vecInitPos[i]);

		//	XMStoreFloat4(&pVertices[i].vRight, InitTransform.r[0]);
		//	XMStoreFloat4(&pVertices[i].vUp, InitTransform.r[1]);
		//	XMStoreFloat4(&pVertices[i].vLook, InitTransform.r[2]);
		//	XMStoreFloat4(&pVertices[i].vTranslation, InitTransform.r[3]);
		//}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Reset()
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX* pVertices = static_cast<VTXMATRIX*>(SubResource.pData);

	_matrix Transform;
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		Transform = XMMatrixIdentity();
		Transform.r[0] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight)));
		Transform.r[1] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp)));
		Transform.r[2] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook)));
		Transform.r[3] = XMLoadFloat4(&m_vecInitPos[i]);

		XMStoreFloat4(&pVertices[i].vRight, Transform.r[0]);
		XMStoreFloat4(&pVertices[i].vUp, Transform.r[1]);
		XMStoreFloat4(&pVertices[i].vLook, Transform.r[2]);
		XMStoreFloat4(&pVertices[i].vTranslation, Transform.r[3]);

		pVertices[i].vLifeTime.y = 0.f;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Set_Particle_Positions(const vector<_float3>& pos)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX* pVertices = static_cast<VTXMATRIX*>(SubResource.pData);

	_matrix Transform;
	m_tDesc.iNumInstance = min(m_iMaxPoint, (_uint)pos.size());
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		Transform = XMMatrixIdentity();
		Transform.r[0] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight)));
		Transform.r[1] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp)));
		Transform.r[2] *= XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook)));
		Transform.r[3] = { pos[i].x,  pos[i].y, pos[i].z, 1.f };

		XMStoreFloat4(&pVertices[i].vRight, Transform.r[0]);
		XMStoreFloat4(&pVertices[i].vUp, Transform.r[1]);
		XMStoreFloat4(&pVertices[i].vLook, Transform.r[2]);
		XMStoreFloat4(&pVertices[i].vTranslation, Transform.r[3]);

		pVertices[i].vLifeTime.y = 0.f;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


shared_ptr<CVIBuffer_Point_Instance> CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* desc)
{
	struct MakeSharedEnabler : public CVIBuffer_Point_Instance
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CVIBuffer_Point_Instance(pDevice, pContext) { }
	};

	shared_ptr<CVIBuffer_Point_Instance> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point_Instance"));
		return nullptr;
		//Safe_Release(pInstance);
	}

	return pInstance;
}


shared_ptr<CComponent> CVIBuffer_Point_Instance::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CVIBuffer_Point_Instance
	{
		MakeSharedEnabler(const CVIBuffer_Point_Instance& rhs) : CVIBuffer_Point_Instance(rhs) { }
	};

	shared_ptr<CVIBuffer_Point_Instance> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTransform"));
		return nullptr;
		//Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	//__super::Free();
	if (!m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertex);
	}
}
