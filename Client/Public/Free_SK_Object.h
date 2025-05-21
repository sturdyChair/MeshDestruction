#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)

class CFree_SK_Object : public CGameObject
{
public:
	struct DESC
	{
		string strModelPath;
	};

public:
	static wstring ObjID;

private:
	CFree_SK_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFree_SK_Object(const CFree_SK_Object& rhs);
public:
	virtual ~CFree_SK_Object();

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

private:
	HRESULT Ready_Components();

public:
	void GetAnimStrList(vector<string>& listAnim) const;
	void SwitchAnim(const string& strAnim);
	void SetAnimFrame(_float fFrame);

	void GetAnimInfo(shared_ptr<CAnimation>& ppAnim, _uint iAnimIndex);

	HRESULT ExportAnimInfo(const string& strFolderPath);

public:
	string SwitchPlayMode();
private:
	_bool m_isPlayAuto = false;

private:
	wstring m_strModelTag;


public:
	static shared_ptr<CFree_SK_Object> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END