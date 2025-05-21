#pragma once
#include "Component.h"
#include "Engine_Defines.h"
#include "Octree.h"

BEGIN(Engine)

class ENGINE_DLL COctComp : public CComponent, public enable_shared_from_this<COctComp>
{
private:
	COctComp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COctComp(const COctComp& rhs);
public:
	virtual ~COctComp();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void Calc_Octree(const _float3& vCenter, _float fRadius);
	void Set_Up();
	void Update();
	_bool IsCulled();

private:
	_bool m_bValidity = false;
	_bool m_bIsIn = false;
	_float3 m_vCenter = {};
	_float m_fRadius = 0.f;

public:
	static shared_ptr<COctComp> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();

	friend class COctree;
	friend struct COctree::Node;

};

END