
#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
public:
	virtual ~CVIBuffer();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

	virtual HRESULT Draw();
	virtual HRESULT Bind_Buffer();

	virtual HRESULT Bind_VB();
	virtual HRESULT Bind_IB(_uint iIdx);

	const vector<_float3>& Get_VertexPos()const { return m_vecVertexPositions; }
	const vector<_uint>& Get_Indicies()const { return m_vecIndicies; }

protected:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC				m_BufferDesc = {};
	D3D11_SUBRESOURCE_DATA			m_InitialData = {};
	_uint							m_iVertexStride = {};
	_uint							m_iIndexStride = {};
	_uint							m_iNumVertices = {};
	_uint							m_iNumIndices = {};
	_uint							m_iNumVertexBuffers = {};
	DXGI_FORMAT						m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY		m_ePrimitiveTopology = {};

	vector<_float3>					m_vecVertexPositions;
	vector<_uint>					m_vecIndicies;


	vector<ID3D11Buffer*>			m_vecIB;


protected:
	HRESULT Create_Buffer(ID3D11Buffer** ppOut);


public:
	virtual shared_ptr<CComponent> Clone(void* pArg) = 0;
	void Free();
};

END