

#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CDynamicCollider;
class CStaticCollider;
END


BEGIN(Client)



class CGeometryBrush : public CGameObject
{
public:
	static wstring ObjID;
	enum CSG_OP
	{
		UNION,
		INTERSECT,
		DIFF,
		OP_END,
	};
	struct FDO_DESC : public CTransform::TRANSFORM_DESC
	{
		_wstring strModelTag;

	};

private:
	CGeometryBrush(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGeometryBrush(const CGeometryBrush& rhs);
public:
	virtual ~CGeometryBrush();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Set_Model_Name(const string& strName);
	void Bake_Binary();
	void Apply_Inner_Texture(const string& szFilename, shared_ptr<CTexture> pTexture, aiTextureType eType);

	void Operation(shared_ptr<CGeometryBrush> rhs, CSG_OP eOperation);

private:
	HRESULT Render_Static();



private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };

	shared_ptr<CModel> m_pModelCom = { nullptr };


	_bool m_bWorking = false;
	thread m_Worker;

	_bool m_bHit = false;
	_wstring m_strModelTag;
	CSG_OP m_eOperation;
	_uint m_iCurrMesh = 0;
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CGeometryBrush> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END