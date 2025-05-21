#pragma once
#include "VIBuffer_Instance.h"

BEGIN(Engine)

class CVIBuffer_Mesh_Instance : public CVIBuffer_Instance
{
private:
	CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& rhs);
public:
	~CVIBuffer_Mesh_Instance();

public:
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static shared_ptr< CVIBuffer_Mesh_Instance> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END