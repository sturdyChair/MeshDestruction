#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CDynamicCollider;
END


BEGIN(Client)

class CFreeDynamicObject : public CGameObject
{
public:
	static wstring ObjID;
	struct FDO_DESC : public CTransform::TRANSFORM_DESC
	{
		_wstring strModelTag;
	};

private:
	CFreeDynamicObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFreeDynamicObject(const CFreeDynamicObject& rhs);
public:
	virtual ~CFreeDynamicObject();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CDynamicCollider> m_pDynamicColliderCom = { nullptr };
	_wstring m_strModelTag;
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CFreeDynamicObject> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END