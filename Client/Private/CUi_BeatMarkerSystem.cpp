#include "CUi_BeatMarkerSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_BeatMarker.h"
#include "CUi_Default.h"
#include "Beat_Manager.h"
#include "iostream"
#include "CUi_BeatJudge.h"


wstring CUi_BeatMarkerSystem::ObjID = L"CUi_BeatMarkerSystem";

CUi_BeatMarkerSystem::CUi_BeatMarkerSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_BeatMarkerSystem::CUi_BeatMarkerSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_BeatMarkerSystem::~CUi_BeatMarkerSystem()
{
}

HRESULT CUi_BeatMarkerSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_BeatMarker::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_BeatJudge::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);


	return S_OK;
}

HRESULT CUi_BeatMarkerSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_DefaultVec();


	return S_OK;
}

void CUi_BeatMarkerSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_BeatMarkerSystem::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Check_Judge();
		Check_Beat();
		Adjust_DefaultVec(fTimeDelta);
		Adjust_Beat(fTimeDelta);
	}
}

void CUi_BeatMarkerSystem::LateTick(_float fTimeDelta)
{

}

HRESULT CUi_BeatMarkerSystem::Render()
{

	return S_OK;
}

void CUi_BeatMarkerSystem::Create_BeatMarker()
{
	CUi_BeatMarker::Desc Desc;
	Desc.fSizeX = m_fBeatMarkerSizeX;
	Desc.fSizeY = 75;
	Desc.fX = m_fBeatMarkerPosX;
	Desc.fY = -220;
	Desc.fZ = 0.1f;
	Desc.fSpeed = Cal_Speed();
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = CUi::ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_BeatMarker";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.bRight = true;

	m_pMarkerList.emplace_back(static_pointer_cast<CUi_BeatMarker>(m_pGameInstance.lock()->Add_Clone_Return
	(LEVEL_TEST, L"CUi", CUi_BeatMarker::ObjID, &Desc)));
	
	Desc.fX = -m_fBeatMarkerPosX;
	Desc.bRight = false;

	m_pMarkerList.emplace_back(static_pointer_cast<CUi_BeatMarker>(m_pGameInstance.lock()->Add_Clone_Return
	(LEVEL_TEST, L"CUi", CUi_BeatMarker::ObjID, &Desc)));
}

void CUi_BeatMarkerSystem::Check_Beat()
{
	if (m_iCheckBeat != CBeat_Manager::Get_Instance()->Get_BeatCount())
	{
		m_iCheckBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		Create_BeatMarker();
		m_bBeatChange = false;
	}
}

void CUi_BeatMarkerSystem::Check_Judge()
{
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_V))
	//{
	//	Cal_Judge();
	//}
}

void CUi_BeatMarkerSystem::Adjust_Beat(_float fTimeDelta)
{
	_float PosX = { 0 };
	_float ScaleX = { 0 };

	auto iter = m_pMarkerList.begin();
	for (; iter != m_pMarkerList.end();)
	{
		PosX = (*iter)->Get_Pos().x;
		ScaleX = (*iter)->Get_Scale().x;
		if ((*iter)->Get_Right())
		{
			(*iter)->Move(MoveDir::Left, fTimeDelta);
			if (PosX - ScaleX * 0.5f < 0)
			{
				(*iter)->Set_Dead();
				iter = m_pMarkerList.erase(iter);
			}
			else
				++iter;
		}
		else
		{
			(*iter)->Move(MoveDir::Right, fTimeDelta);
			if (PosX + ScaleX * 0.5f > 0)
			{
				(*iter)->Set_Dead();
				iter = m_pMarkerList.erase(iter);
			}
			else
				++iter;
		}
	}


}

void CUi_BeatMarkerSystem::Cal_Judge()
{
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
	_float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
	JudgeType Type = JudgeType::Miss;
	//if (fBeat - fTimer <= 0.1)
	//{
	//	cout << "Perfect" << endl;
	//	Type = JudgeType::Perfect;
	//}
	//else if (fBeat - fTimer <= 0.15)
	//{
	//	cout << "Good" << endl;
	//	Type = JudgeType::Good;
	//}
	//else
	//{
	//	cout << "miss" << endl;
	//	Type = JudgeType::Miss;
	//}
	//cout << fBeat - fTimer << endl;
	if (CBeat_Manager::Get_Instance()->Beat_Move(0.4f))
	{
		Type = JudgeType::Perfect;
	}
	Create_Judge(Type);
	//Delete_Marker();
}


void CUi_BeatMarkerSystem::Create_Judge(JudgeType Type)
{
	CUi_BeatJudge::Desc Desc;
	Desc.fSizeX = 100;
	Desc.fSizeY = 40;
	Desc.fX = 0;
	Desc.fY = -200;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_BeatJudge";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.Type = Type;
	Desc.fZ = 0.5f;
	Desc.fSpeed = 200;
	m_pGameInstance.lock()->Add_Clone(m_iLevelIndex, L"CUi", CUi_BeatJudge::ObjID, &Desc);
}

void CUi_BeatMarkerSystem::Delete_Marker()
{
	if (m_pMarkerList.size() >= 2)
	{
		auto iter = m_pMarkerList.begin();

		(*iter)->Set_Dead();
		iter = m_pMarkerList.erase(iter);

		(*iter)->Set_Dead();
		m_pMarkerList.erase(iter);
	}
}

_float CUi_BeatMarkerSystem::Cal_Speed()
{
	_float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
	_float Time = Beat * 4.f;
	_float Speed = (m_fBeatMarkerPosX - m_fBeatMarkerSizeX * 0.5f) / Time;
	if (Speed < 0)
	{
		Speed = -Speed;
	}
	return Speed;
}

void CUi_BeatMarkerSystem::Adjust_DefaultVec(_float fTimeDelta)
{
	_float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
	_float Timer = CBeat_Manager::Get_Instance()->Get_Timer();
	if (Beat - Timer < 0.15f)
	{
		DefaultVec_ColorAdjust(fTimeDelta, true);
		DefaultVec_ScaleAdjust(fTimeDelta, true);
	}
	else
	{
		DefaultVec_ColorAdjust(fTimeDelta, false);
		DefaultVec_ScaleAdjust(fTimeDelta, false);
	}

}

void CUi_BeatMarkerSystem::DefaultVec_ColorAdjust(_float fTimeDelta, _bool Decrease)
{
	if (Decrease)
	{
		_float4 CatAddColor = { fTimeDelta * 10.f, 0, 0, 0 };
		_float4 CircleAddColor = { 0, -fTimeDelta * 10.f, -fTimeDelta * 10.f, 0 };
		m_DefaultVec[2]->Add_Color(CatAddColor);
		m_DefaultVec[3]->Add_Color(CircleAddColor);

		if (m_DefaultVec[2]->Get_Color().x > 1)
		{
			_float4 RedColor = { 1, 0, 0, 1 };
			m_DefaultVec[2]->Set_Color(RedColor);
			m_DefaultVec[3]->Set_Color(RedColor);
		}
	}
	else
	{
		_float4 CatIncreseColor = { -fTimeDelta * 10.f, 0, 0, 0 };
		_float4 CircleIncreseColor = { 0, fTimeDelta * 10.f, fTimeDelta * 10.f, 0 };

		m_DefaultVec[2]->Add_Color(CatIncreseColor);
		m_DefaultVec[3]->Add_Color(CircleIncreseColor);

		if (m_DefaultVec[2]->Get_Color().x < 0)
		{
			_float4 CatOriginColor = { 0, 0, 0, 1 };
			_float4 CircleOriginColor = { 1, 1, 1, 1 };
			m_DefaultVec[2]->Set_Color(CatOriginColor);
			m_DefaultVec[3]->Set_Color(CircleOriginColor);
		}
	}
}

void CUi_BeatMarkerSystem::DefaultVec_ScaleAdjust(_float fTimeDelta, _bool Decrease)
{
	if (Decrease)
	{		
		m_DefaultVec[1]->Add_Scale(-fTimeDelta * 1000, 0);
		m_DefaultVec[2]->Add_Scale(-fTimeDelta * 150, -fTimeDelta * 150);
		m_DefaultVec[3]->Add_Scale(-fTimeDelta * 75, -fTimeDelta * 75);

		if (m_DefaultVec[2]->Get_Scale().x < 55)
		{
			m_DefaultVec[1]->Set_Scale(g_iWinSizeX * 0.2f, 40);
			m_DefaultVec[2]->Set_Scale(55, 55);
			m_DefaultVec[3]->Set_Scale(27.5f, 27.5f);
		}
	}
	else
	{
		m_DefaultVec[1]->Add_Scale(fTimeDelta * 1000, 0);
		m_DefaultVec[2]->Add_Scale(fTimeDelta * 150, fTimeDelta * 150);
		m_DefaultVec[3]->Add_Scale(fTimeDelta * 75, fTimeDelta * 75);
		if (m_DefaultVec[2]->Get_Scale().x > 110)
		{
			m_DefaultVec[1]->Set_Scale(g_iWinSizeX * 0.4f, 40);
			m_DefaultVec[2]->Set_Scale(110, 110);
			m_DefaultVec[3]->Set_Scale(55, 55);
		}
	}
}

void CUi_BeatMarkerSystem::Initialize_DefaultVec()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = g_iWinSizeX * 0.6f;
	Desc.fSizeY = 40;
	Desc.fX = 0;
	Desc.fY = -220;
	Desc.fZ = 0.8f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_BeatMarkerBack";
	Desc.TextureLevel = LEVEL_STATIC;
	shared_ptr<CUi_Default> m_pDefault = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pDefault->Set_Color(ColorType::A, 0.3f);
	m_DefaultVec.emplace_back(m_pDefault);


	Desc.fSizeX = g_iWinSizeX * 0.4f;
	Desc.fSizeY = 40;
	Desc.fZ = 0.7f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_BeatMarkerBackSmall";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pDefault = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pDefault->Set_Color(ColorType::A, 0.5f);
	m_DefaultVec.emplace_back(m_pDefault);


	Desc.fSizeX = 110;
	Desc.fSizeY = 110;
	Desc.fZ = 0.6f;
	Desc.TextureFileName = L"Prototype_Component_Texture_BeatMarkerCat";
	m_pDefault = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	_float4 CatColor = { 0, 0, 0, 1 };
	m_pDefault->Set_Color(CatColor);
	m_DefaultVec.emplace_back(m_pDefault);

	Desc.fSizeX = 55;
	Desc.fSizeY = 55;
	Desc.fZ = 0.5f;
	Desc.TextureFileName = L"Prototype_Component_Texture_BeatMarkerCircle";
	m_pDefault = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_DefaultVec.emplace_back(m_pDefault);
}

shared_ptr<CUi_BeatMarkerSystem> CUi_BeatMarkerSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_BeatMarkerSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_BeatMarkerSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_BeatMarkerSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_BeatMarkerSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_BeatMarkerSystem
	{
		MakeSharedEnabler(const CUi_BeatMarkerSystem& rhs) : CUi_BeatMarkerSystem(rhs) { }
	};

	shared_ptr<CUi_BeatMarkerSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
