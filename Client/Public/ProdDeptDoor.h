#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CProdDeptDoor : public CGameObject
{
public:
	static _wstring ObjID;

private:
	enum ELEMENTS { DOOR_R, DOOR_L, ELEM_END };

private:
	CProdDeptDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProdDeptDoor(const CProdDeptDoor& rhs);
public:
	~CProdDeptDoor();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	vector<shared_ptr<class CPartObject>> m_vecParts;
	//shared_ptr<class CDoorTrigger> m_pDoorTrigger = { nullptr };

private:
	HRESULT Ready_Parts();
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr< CProdDeptDoor> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END