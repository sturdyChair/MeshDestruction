#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
END

BEGIN(Client)

class CPartDoor : public CPartObject
{
public:
	static _wstring ObjID;

	enum DOOR_DIR { LEFT, RIGHT, UP, DOWN, DIR_END };

	struct DOOR_PARTDESC : public CPartObject::PARTOBJ_DESC
	{
		_bool bOpen;
		DOOR_DIR eDir;
		_float fMoveDist;
		_wstring strModelTag;
	};

private:
	CPartDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartDoor(const CPartDoor& rhs);
public:
	~CPartDoor();

public:
	void Execute_Door(_bool bExecute) { m_bExecute = bExecute; }

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

	_bool m_bExecute = { false };
	_bool m_bOpen = { false };
	DOOR_DIR m_eDoorDir = { DIR_END };
	_float m_fMoveDist = {};

private:
	HRESULT Ready_Components(DOOR_PARTDESC* pDesc);
	HRESULT Bind_ShaderResources();

	void Open_Door(_float fTimeDelta);
	void Close_Door(_float fTimeDelta);

public:
	static shared_ptr<CPartDoor> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END