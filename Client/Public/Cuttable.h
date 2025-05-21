#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CDynamicCollider;
class CStaticCollider;
class CCutter_Controller;
class CMultiCutterController;
class CPxRagDoll;
END


BEGIN(Client)



class CCuttable : public CGameObject
{
public:
	static wstring ObjID;
	struct FDO_DESC : public CTransform::TRANSFORM_DESC
	{
		_wstring strModelTag;
	};


private:
	CCuttable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCuttable(const CCuttable& rhs);
public:
	virtual ~CCuttable();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Cut(const _float4& plane);

	void RemoveFromScene();
	void AddToScene();
	void ApplyBones();
	void ApplyAnimation(_uint iIdx, _float fTrackPos);

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };

	shared_ptr<CModel> m_pModelCom = { nullptr };
	//shared_ptr<CCutter_Controller> m_pCutterController = { nullptr };
	//shared_ptr<CPxRagDoll> m_pRagDollCom;
	_wstring m_strModelTag;
	_bool m_bCut = false;
	shared_ptr< CMultiCutterController> m_pMultiCutterController;
	_float m_fSliceLifeTime = 5.f;

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CCuttable> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END