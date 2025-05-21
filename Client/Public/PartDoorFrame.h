#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
END

BEGIN(Client)

class CPartDoorFrame : public CPartObject
{
public:
	static _wstring ObjID;

	struct FRAME_PARTDESC : public CPartObject::PARTOBJ_DESC
	{
		_wstring strModelTag;
	};

private:
	CPartDoorFrame (ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartDoorFrame (const CPartDoorFrame & rhs);
public:
	~CPartDoorFrame ();

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

private:
	HRESULT Ready_Components(FRAME_PARTDESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CPartDoorFrame > Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END