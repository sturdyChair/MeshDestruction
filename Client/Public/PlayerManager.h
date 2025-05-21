#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)

class CPlayer_Manager
{
	DECLARE_SINGLETON(CPlayer_Manager)

public:
	enum class Rank { None, D, C, B, A, S, SS, SSS, End };

	using Desc = struct PlayerDataInfo
	{
		Rank eRank = { Rank::None };
		_float fRankPercent = { 0 };

		_uint iPlayerCombo = { 101 };

		_uint iPlayerScore = { 12435 };

		_uint JustTimingCount = { 100 };

		_uint iPlayerGear = { 12345 };

		_uint iPlayerChip = { 1 };

		_float Reverve = { 30 };

		_float AllMinusHp = {};
		_float Hp = { 0 };
		_float MaxHp = { 0 };
	};

	using SkillDesc = struct PlayerSkillInfo
	{
		_float ConsumeReverve = {};	
	};



private:
	CPlayer_Manager();

public:
	~CPlayer_Manager();

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);
	void Render();


public:
	Rank Get_PlayerRank() const { return m_Desc.eRank; }
	_uint Get_PlayerScore() const { return m_Desc.iPlayerScore; }
	_uint Get_PlayerCombo() const { return m_Desc.iPlayerCombo; }
	_float Get_PlayerRankPercent() const { return m_Desc.fRankPercent; }
	vector<_uint>& Get_CorusScore() { return m_CorusScore; }

	_float Get_Hp() const { return m_Desc.Hp; }
	_float Get_MaxHp() const { return m_Desc.MaxHp; }
	_float Get_AllMinusHp() const { return m_Desc.AllMinusHp; }

	_float Get_Reverve() const { return m_Desc.Reverve; }

	_uint Get_JustTimingCount() const { return m_Desc.JustTimingCount; }

	_uint Get_PlayerGear() const { return m_Desc.iPlayerGear; }
	void Add_PlayerGear(_uint iGear) { m_Desc.iPlayerGear += iGear; }
	void Sub_PlayerGear(_uint iGear);
	_bool Is_HasSkill(wstring SkillName);
	void Add_Skill(wstring SkillName);


	void Set_Player(shared_ptr<CGameObject> pPlayer) { m_pPlayer = pPlayer; }
	shared_ptr<CGameObject> Get_Player() { return m_pPlayer; }



private:
	Desc m_Desc;
	map<wstring, SkillDesc> m_PlayerSkill;
	shared_ptr<CGameObject>  m_pPlayer = nullptr;
	vector<_uint> m_CorusScore;


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	void Free();
};

END