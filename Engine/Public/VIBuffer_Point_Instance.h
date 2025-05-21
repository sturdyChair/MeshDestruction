#pragma once

#include "VIBuffer_instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance : public CVIBuffer_Instance
{
private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
public:
	virtual ~CVIBuffer_Point_Instance();

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Update_PtInstance_Style(_float fTimeDelta);
	virtual void Update_Spread(_float fTimeDelta) override;
	virtual void Update_Drop(_float fTimeDelta) override;
	virtual void Update_Fountain(_float fTimeDelta) override;
	virtual void Update_Rising(_float fTimeDelta) override;
	virtual void Update_Cycle(_float fTimeDelta) override;
	virtual void Update_Orbital(_float fTimeDelta) override;
	virtual void Reset() override;

	void Set_Particle_Positions(const vector<_float3>& pos);

private:
	VTXMATRIX* m_pInstanceVertex = nullptr;
	_uint m_iMaxPoint = 0;
public:
	static shared_ptr<CVIBuffer_Point_Instance> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END