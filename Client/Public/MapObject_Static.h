#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CMapObject_Static : public CGameObject
{
public:
	static wstring ObjID;

	struct STATICOBJ_DESC : public CTransform::TRANSFORM_DESC
	{
		_bool bCollision = { true };
		_float fCullingRange = { 0 };
		wstring strModelTag;
	};

private:
	CMapObject_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject_Static(const CMapObject_Static& rhs);
public:
	virtual ~CMapObject_Static();

public:
	HRESULT Set_ModelComponent(const string& strPrototypeTag);
	void Set_CullingRange(_float fCullingRange) { m_fCullingRange = fCullingRange; }

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

	_float m_fCullingRange = { 0 };

private:
	HRESULT Ready_Components(const STATICOBJ_DESC* pDesc);
	HRESULT Ready_ModelPrototypes(const wstring& strLevelDir);

public:
	static shared_ptr<CMapObject_Static> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END