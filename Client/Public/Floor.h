#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CStaticCollider;
END


BEGIN(Client)

class CFloor : public CGameObject
{
public:
	static wstring ObjID;

private:
	CFloor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFloor(const CFloor& rhs);
public:
	virtual ~CFloor();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnPxContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPair, CGameObject* pOther) override;
private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CStaticCollider> m_pStaticColliderCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CFloor> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END