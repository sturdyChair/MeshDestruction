#pragma once

#include "Component.h"
#include "CSG_Data.h"

BEGIN(Engine)

class ENGINE_DLL CStatic_Cutter : public CComponent
{
public:
	struct MODEL_CUTTER_DESC
	{
		shared_ptr<class CModel> pModel;
		_uint iMeshIdx = 0;
	};
	static const _wstring s_PrototypeTag;

private:
	CStatic_Cutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatic_Cutter(const CStatic_Cutter& rhs);
public:
	virtual ~CStatic_Cutter();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath);
	virtual HRESULT Initialize(void* pArg) override;

	pair<vector<_uint3>, vector<_uint3>> Cut(const _float4& Plane, vector<VTXMESH>& out);

private:
	void Shader_Compute();
	void Bind_CutPlane();
	void Get_NewVertices(vector<VTXMESH>& out);
	pair<vector<_uint3>, vector<_uint3>> Rebuild_Mesh();
	vector<_uint3> Build_Mesh_Internal(ID3D11UnorderedAccessView* pUAV, ID3D11Buffer* pBuffer, _uint iVertCount);
private:
	ID3D11ComputeShader* m_pComputeShader = nullptr;
	ID3DBlob* m_Blob = nullptr;
	shared_ptr<MODEL_DATA> m_pModelData;
	shared_ptr<class CModel> m_pModel;
	_uint m_iMeshIdx = 0;


	static _uint s_iSplitCount;

	ID3D11Buffer* m_pInputVertexBuffer = nullptr;
	ID3D11ShaderResourceView* m_pInputVertexSRV = nullptr;

	ID3D11Buffer* m_pInputIndexBuffer = nullptr;
	ID3D11ShaderResourceView* m_pInputIndexSRV = nullptr;

	ID3D11Buffer* m_pOutputUpBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pOutputUpUAV = nullptr;
	ID3D11Buffer* m_pOutputDownBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pOutputDownUAV = nullptr;

	ID3D11Buffer* m_pOutputNewVertBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pOutputNewVerUAV = nullptr;


	ID3D11Buffer* m_pCutPlaneBuffer = nullptr;
	_float4						m_CutPlane;

	ID3D11Buffer* m_pStagingBuffer = nullptr;
	ID3D11Buffer* m_pStagingIndiciesBuffer = nullptr;
	ID3D11Buffer* m_pCountBuffer = nullptr;
public:
	static shared_ptr<CStatic_Cutter> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();


	
};

END