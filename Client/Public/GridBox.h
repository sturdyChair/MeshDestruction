#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
END


BEGIN(Client)

class CGridBox : public CGameObject
{
private:
	CGridBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGridBox(const CGridBox& rhs);
public:
	virtual ~CGridBox();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom = { nullptr };
	shared_ptr<CVIBuffer_Cube> m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CGridBox> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END