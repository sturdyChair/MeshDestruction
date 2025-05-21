#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CDynamicCollider;
class CStaticCollider;
class CStatic_Cutter;
END


BEGIN(Client)



class CStaticCutTester : public CGameObject
{
public:
	static wstring ObjID;
	struct FDO_DESC : public CTransform::TRANSFORM_DESC
	{
		_wstring strModelTag;
	};


private:
	CStaticCutTester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticCutTester(const CStaticCutTester& rhs);
public:
	virtual ~CStaticCutTester();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Cut(const _float4& plane);

private:
	HRESULT Render_Static();
	HRESULT Render_Dynamic();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CShader> m_pStaticShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	vector<shared_ptr<CStatic_Cutter>> m_Cutters;
	vector<_float4x4> m_Transforms;
	vector<_float3> m_TransformsTarget;
	vector<PxRigidDynamic*> m_pxActors;
	_bool m_bFollowPhysx = false;
	_float4x4 m_PreTransform;

	_float m_fApart = 0.03f;
	_wstring m_strModelTag;
	_bool m_bCut = false;
	vector<_float> m_LifeTimer;
	vector<_uint> m_CutCount;
	_uint m_iMaxCutCount = 5;
	_float m_fSliceLifeTime = 5.f;
	_float m_fLT = 0.f;
	_bool m_bOver = false;

	_uint m_iSlicingEffectCount = 100;
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CStaticCutTester> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END