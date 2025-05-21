#include "CUi_RankSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Mgr.h"
#include "PlayerManager.h"
#include "CUi_ComboNum.h"
#include "CUi_ComboWordHits.h"
#include "CUi_Default.h"
#include "Beat_Manager.h"




wstring CUi_RankSystem::ObjID = L"CUi_RankSystem";

CUi_RankSystem::CUi_RankSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_RankSystem::CUi_RankSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_RankSystem::~CUi_RankSystem()
{
}

HRESULT CUi_RankSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_ComboNum::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_ComboWordHits::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);

	return S_OK;
}

HRESULT CUi_RankSystem::Initialize(void* pArg)
{
	Initialize_Transform();

	Initialize_Rank();
	Initialize_ComboNum();
	Initialize_ComboWordHit();
	Initialize_Beatmeter();


	return S_OK;
}

void CUi_RankSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_RankSystem::Tick(_float fTimeDelta)
{
	Check_PlayerCombo();

	Check_PlayerScore();

	Check_PlayerRank();

	Set_Beatmeter();

	if (m_iBeatChangeCheck != CBeat_Manager::Get_Instance()->Get_BeatCount())
	{
		m_iBeatChangeCheck = CBeat_Manager::Get_Instance()->Get_BeatCount();
		m_bBeatChange = true;
	}
	
	if (m_bBeatChange)
	{
		Set_BeatChange(fTimeDelta);
	}
	else
	{
		BeatChangeProgress(fTimeDelta);
	}
}

void CUi_RankSystem::LateTick(_float fTimeDelta)
{
}

HRESULT CUi_RankSystem::Render()
{
	return S_OK;
}

void CUi_RankSystem::Respawn()
{
}

void CUi_RankSystem::Check_PlayerCombo()
{
	_uint OriginPlayerCombo = PLAYERMANAGER->Get_PlayerCombo();
	_uint PlayerCombo = OriginPlayerCombo;
	_uint Count = { 0 };
	while (PlayerCombo > 0)
	{
		PlayerCombo /= 10;
		++Count;
	}

	if (Count > 5)
		assert(false);

	for (auto& iter : m_pComboNumVec)
		iter->Set_Active(false);


	_float SizeX = m_pComboNumVec[0]->Get_Scale().x;
	_float3 Pos = m_ComboNumVecOriginPos;

	for (size_t i = 0; i < Count; ++i)
	{
			m_pComboNumVec[i]->Set_Pos(Pos.x + (SizeX * (i - Count / 2.f) * 0.7f), Pos.y);	
	}

	for (_uint i = 0; i < Count; ++i)
	{
		m_pComboNumVec[i]->Set_Active(true);
	}

	Cal_PlayerCombo(Count);
}

void CUi_RankSystem::Cal_PlayerCombo(_uint iDigit)
{
	_uint iDivision = { 1 };
	_uint iNumber[10] = { 0, };
	_uint Combo = PLAYERMANAGER->Get_PlayerCombo();

	for (_uint i = 0; i < iDigit - 1; ++i)
	{
		iDivision *= 10;
	}

	for (_uint i = 0; i < iDigit; ++i)
	{
		_uint iShare = Combo / iDivision;
		Combo -= iShare * iDivision;
		iNumber[i] = iShare;
		iDivision /= 10;
	}

	for (_uint i = 0; i < iDigit; ++i)
	{
		m_pComboNumVec[i]->Set_TextureNum(iNumber[i]);
	}
}

void CUi_RankSystem::Check_PlayerScore()
{
}

void CUi_RankSystem::Cal_PlayerScore(_uint iDigit)
{

}

void CUi_RankSystem::Check_PlayerRank()
{
	_uint PlayerRank = (_uint)PLAYERMANAGER->Get_PlayerRank();

	for (auto& iter : m_pBeatmeterSpikeVec)
		iter->Set_Active(false);

	
	for (_uint i = 0; i < PlayerRank + 1; ++i)
	{
		if (_uint(CPlayer_Manager::Rank::None) < i && i < _uint(CPlayer_Manager::Rank::S))
		{
			m_pBeatmeterSpikeVec[i - 1]->Set_Active(true);
		}
		else if (i >= _uint(CPlayer_Manager::Rank::S))
		{
			m_pBeatmeterSpikeVec[_int(CPlayer_Manager::Rank::S) - 1]->Set_Active(true);
			m_pBeatmeterSpikeVec[_uint(CPlayer_Manager::Rank::S)]->Set_Active(true);
		}
	}


	if (0 < PlayerRank && PlayerRank < _uint(CPlayer_Manager::Rank::SS))
	{
		m_pRank->Set_Active(true);
		m_pRank->Set_TextureNum(PlayerRank);
	}
	else if ( PlayerRank == 0)
	{
		m_pRank->Set_Active(false);
	}
}

void CUi_RankSystem::Set_Beatmeter()
{
	_uint PlayerRank = (_uint)PLAYERMANAGER->Get_PlayerRank();

	if (PlayerRank < (_uint)CPlayer_Manager::Rank::SS)
		m_pBeatmeter->Set_TextureNum(PlayerRank);

	if (PlayerRank == (_uint)CPlayer_Manager::Rank::None)
	{
		m_pBeatmeterBack->Set_Active(false);
	}
	else
	{
		m_pBeatmeterBack->Set_Active(true);
	}

}

void CUi_RankSystem::Set_BeatChange(_float fTimeDelta)
{
	m_fBeatTimeCount += fTimeDelta;
	_float ChangeScaleRate = fTimeDelta * 750;
	if (m_fBeatTimeCount > CBeat_Manager::Get_Instance()->Get_Beat() * 0.3f)
	{
		m_fBeatTimeCount = 0;
		m_bBeatChange = false;
	}
	
	m_pBeatmeterBack->Add_Scale(ChangeScaleRate, ChangeScaleRate);
	m_pBeatmeterSpikeBack->Add_Scale(ChangeScaleRate, ChangeScaleRate);
	for (auto& iter : m_pBeatmeterSpikeVec)
		iter->Add_Scale(ChangeScaleRate, ChangeScaleRate);

	if (m_pBeatmeterBack->Get_Scale().x > m_fRankSize.x * 1.5f)
	{
		m_pBeatmeterBack->Set_Scale(m_fRankSize.x * 1.5f, m_fRankSize.y * 1.5f);
	}
	if (m_pBeatmeterSpikeBack->Get_Scale().x > m_fRankSize.x * 1.4f)
	{
		m_pBeatmeterBack->Set_Scale(m_fRankSize.x * 1.4f, m_fRankSize.y * 1.4f);
	}
	for (auto& iter : m_pBeatmeterSpikeVec)
	{
		if (iter->Get_Scale().x > m_fRankSize.x * 1.4f)
		{
			iter->Set_Scale(m_fRankSize.x * 1.4f, m_fRankSize.y * 1.4f);
		}
	}
}

void CUi_RankSystem::BeatChangeProgress(_float fTimeDelta)
{
	_float ChangeScaleRate = fTimeDelta * 450;
	m_pBeatmeterBack->Add_Scale(-ChangeScaleRate, -ChangeScaleRate);
	m_pBeatmeterSpikeBack->Add_Scale(-ChangeScaleRate, -ChangeScaleRate);
	for (auto& iter : m_pBeatmeterSpikeVec)
		iter->Add_Scale(-ChangeScaleRate, -ChangeScaleRate);

	if (m_pBeatmeterBack->Get_Scale().x < m_fRankSize.x * 0.8f)
	{
		m_pBeatmeterBack->Set_Scale(m_fRankSize.x * 0.8f, m_fRankSize.y * 0.8f);
	}
	if (m_pBeatmeterSpikeBack->Get_Scale().x < m_fRankSize.x * 0.8f)
	{
		m_pBeatmeterBack->Set_Scale(m_fRankSize.x * 0.8f, m_fRankSize.y * 0.8f);
	}
	for (auto& iter : m_pBeatmeterSpikeVec)
	{
		if (iter->Get_Scale().x < m_fRankSize.x * 0.8f)
		{
			iter->Set_Scale(m_fRankSize.x * 0.8f, m_fRankSize.y * 0.8f);
		}
	}

}

void CUi_RankSystem::Initialize_Rank()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = m_fRankSize.x;
	Desc.fSizeY = m_fRankSize.y;
	Desc.fX = m_fRankPos.x;
	Desc.fY = m_fRankPos.y;
	Desc.fZ = 0.1f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = CUi::ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_Rank";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pRank = static_pointer_cast<CUi_Default>
		(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi_Rank", CUi_Default::ObjID, &Desc));
}

void CUi_RankSystem::Initialize_ComboNum()
{
	CUi_ComboNum::Desc Desc;
	Desc.fSizeX = 60;
	Desc.fSizeY = 60;
	Desc.fX = m_fRankPos.x - 100;
	Desc.fY = m_fRankPos.y - 100;
	m_ComboNumVecOriginPos.x = Desc.fX;
	m_ComboNumVecOriginPos.y = Desc.fY;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = CUi::ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_ComboNum";
	Desc.TextureLevel = LEVEL_STATIC;

	for (size_t i = 0; i < 5; ++i)
	{
		if (i == 3)
			Desc.fZ = 0.2f;
		else
			Desc.fZ = 0.7f;
		m_pComboNumVec.emplace_back(static_pointer_cast<CUi_ComboNum>
			(m_pGameInstance.lock()->Add_Clone_Return
			(LEVEL_TEST, L"CUi", CUi_ComboNum::ObjID, &Desc)));
	}
}

void CUi_RankSystem::Initialize_ComboWord()
{
}

void CUi_RankSystem::Initialize_ComboWordHit()
{
	CUi_ComboWordHits::Desc Desc;
	Desc.fSizeX = 100;
	Desc.fSizeY = 50;
	Desc.fX = m_fRankPos.x - 130;
	Desc.fY = m_fRankPos.y - 150;
	Desc.fZ = 0.7f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = CUi::ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_ComboWordHits";
	Desc.TextureLevel = LEVEL_STATIC;


	m_pWordHits = static_pointer_cast<CUi_ComboWordHits>
		(m_pGameInstance.lock()->Add_Clone_Return
		(LEVEL_TEST, L"CUi", CUi_ComboWordHits::ObjID, &Desc));
}

void CUi_RankSystem::Initialize_Beatmeter()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = m_fRankSize.x * 1.5f;
	Desc.fSizeY = m_fRankSize.y * 1.5f;
	Desc.fX = m_fRankPos.x;
	Desc.fY = m_fRankPos.y;
	Desc.fZ = 0.7f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = CUi::ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_Beatmeter";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBeatmeter = static_pointer_cast<CUi_Default>
		(m_pGameInstance.lock()->Add_Clone_Return
		(LEVEL_TEST, L"CUi", CUi_Default::ObjID, &Desc));


	Desc.fSizeX = m_fRankSize.x * 1.5f;;
	Desc.fSizeY = m_fRankSize.y * 1.5f;;
	Desc.fX = m_fRankPos.x;
	Desc.fY = m_fRankPos.y;
	Desc.fZ = 0.8f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = CUi::ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_BeatmeterBack";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBeatmeterBack = static_pointer_cast<CUi_Default>
		(m_pGameInstance.lock()->Add_Clone_Return
		(LEVEL_TEST, L"CUi", CUi_Default::ObjID, &Desc));


	Desc.fSizeX = m_fRankSize.x * 1.5f;;
	Desc.fSizeY = m_fRankSize.y * 1.5f;;
	Desc.fX = m_fRankPos.x;
	Desc.fY = m_fRankPos.y;
	Desc.fZ = 0.8f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = CUi::ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_BeatmeterSpike";
	Desc.TextureLevel = LEVEL_STATIC;

	for (_uint i = 0; i < _uint(CPlayer_Manager::Rank::SS); i++)
	{
		//Desc.fZ = 0.85f - (0.1f * i);
		//if (i == _uint(CPlayer_Manager::Rank::S))
		//	Desc.fZ = 0.9f;

		m_pBeatmeterSpikeVec.emplace_back(static_pointer_cast<CUi_Default>
			(m_pGameInstance.lock()->Add_Clone_Return
			(LEVEL_TEST, L"CUi", CUi_Default::ObjID, &Desc)));

		m_pBeatmeterSpikeVec[i]->Set_TextureNum(i);
	}



	Desc.fSizeX = m_fRankSize.x * 1.5f;;
	Desc.fSizeY = m_fRankSize.y * 1.5f;;
	Desc.fX = m_fRankPos.x;
	Desc.fY = m_fRankPos.y;
	Desc.fZ = 0.8f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = CUi::ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_BeatmeterSpikeBack";
	Desc.TextureLevel = LEVEL_STATIC;

	m_pBeatmeterSpikeBack = static_pointer_cast<CUi_Default>
		(m_pGameInstance.lock()->Add_Clone_Return
		(LEVEL_TEST, L"CUi", CUi_Default::ObjID, &Desc));
}

shared_ptr<CUi_RankSystem> CUi_RankSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_RankSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_RankSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_RankSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_RankSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_RankSystem
	{
		MakeSharedEnabler(const CUi_RankSystem& rhs) : CUi_RankSystem(rhs) { }
	};

	shared_ptr<CUi_RankSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
