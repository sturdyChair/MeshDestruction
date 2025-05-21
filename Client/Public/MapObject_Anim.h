#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CMapObject_Anim : public CGameObject
{
public:
	static wstring ObjID;

	struct ANIMOBJ_DESC : public CTransform::TRANSFORM_DESC
	{
		_float fCullingRange = { 0 };
		wstring strModelTag;
	};

private:
	CMapObject_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject_Anim(const CMapObject_Anim& rhs);
public:
	virtual ~CMapObject_Anim();

public:
	HRESULT Set_ModelComponent(const string& strPrototypeTag);

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

	_uint m_iCurrAnimIndex = { 0 };
	_uint m_iPrevBeat = { 0 };

	_bool m_bPlay = { false };

private:
	HRESULT Ready_Components(const ANIMOBJ_DESC* pDesc);
	HRESULT Ready_ModelPrototypes(const wstring& strLevelDir);

public:
	static shared_ptr<CMapObject_Anim> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END