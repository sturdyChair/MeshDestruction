#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CDynamicCollider;
class CStaticCollider;
END


BEGIN(Client)



class CPreBakedBrokenObj : public CGameObject
{
public:
	static wstring ObjID;
	struct FDO_DESC : public CTransform::TRANSFORM_DESC
	{
		_wstring strModelTag;
	};

private:
	CPreBakedBrokenObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPreBakedBrokenObj(const CPreBakedBrokenObj& rhs);
public:
	virtual ~CPreBakedBrokenObj();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Simulate(const PxSweepHit& Hit, const _float3& vDir);
	void End_Sim();

private:
	void Read_HitInfo();
	HRESULT Render_Static();
	HRESULT Render_Dynamic();
	_bool m_bHit = false;
private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CShader> m_pAnimShaderCom = { nullptr };
	shared_ptr<CModel> m_pSphereModelCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CStaticCollider> m_pStaticCollider = { nullptr };

	vector<pair<string, HIT>> m_HitInfos;

	_wstring m_strModelTag;
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CPreBakedBrokenObj> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END