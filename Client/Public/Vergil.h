#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)

class CVergil : public CGameObject
{
public:
	static wstring ObjID;

private:
	enum class VIRGIL_MOVE_STATE {
		JOG, RUN, DASH, JUMP, EVADE, MOVE_END
	};
	enum class VIRGIL_ATTACK_STATE {
		COMBO_A_1, COMBO_A_2, COMBO_A_3, COMBO_A_4, COMBO_B_1, COMBO_C_1, COMBO_C_2, ATTACK_END

	};
private:
	CVergil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVergil(const CVergil& rhs);
public:
	virtual ~CVergil();

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
	static shared_ptr<CVergil> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();

private:
	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);
	void Attack(_float fTimeDelta);
private:
	_int m_iAnimIndex = 2;
	_bool m_bDebugBool = false;

	vector<string> m_vecAnimList;

	VIRGIL_MOVE_STATE m_eMoveState = VIRGIL_MOVE_STATE::MOVE_END;
};

END