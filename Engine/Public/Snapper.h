#pragma once

#include "Component.h"
#include "CSG_Data.h"

BEGIN(Engine)

class ENGINE_DLL CSnapper : public CComponent
{
public:
	struct MODEL_SNAP_DESC
	{
		shared_ptr<class CModel> pModel;
		_uint iMeshIdx;
	};
	static const _wstring s_PrototypeTag;

private:
	CSnapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSnapper(const CSnapper& rhs);
public:
	virtual ~CSnapper();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath);
	virtual HRESULT Initialize(void* pArg) override;

	vector<VTXMESH> Snap();

private:
	void Shader_Compute();
	void Bind_Bones();
	vector<VTXMESH> Get_Vertices();

private:
	ID3D11ComputeShader* m_pComputeShader = nullptr;
	ID3DBlob* m_Blob = nullptr;
	shared_ptr<MODEL_DATA> m_pModelData;
	shared_ptr<class CModel> m_pModel;
	_uint m_iMeshIdx = 0;

	static _uint s_iSplitCount;

	ID3D11Buffer* m_pInputVertexBuffer = nullptr;
	ID3D11ShaderResourceView* m_pInputVertexSRV = nullptr;

	ID3D11Buffer* m_pOutputBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pOutputUAV = nullptr;

	ID3D11Buffer* m_pBoneBuffer = nullptr;

	ID3D11Buffer* m_pStagingBuffer = nullptr;

	ID3D11Buffer* m_pCountBuffer = nullptr;
public:
	static shared_ptr<CSnapper> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();

};

END