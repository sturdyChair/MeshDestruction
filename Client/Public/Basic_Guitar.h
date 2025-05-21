#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CBasic_Guitar : public Engine::CPartObject
{
public:
	static wstring ObjID;
public:
	struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_uint* pState;
	};

private:
	CBasic_Guitar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBasic_Guitar(const CBasic_Guitar& rhs);


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };
	shared_ptr <CCollider> m_pColliderCom = { nullptr };

private:
	const _float4x4* m_pBoneMatrix = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CBasic_Guitar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual ~CBasic_Guitar();
};


END