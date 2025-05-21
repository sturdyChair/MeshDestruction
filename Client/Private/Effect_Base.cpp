#include "Effect_Base.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"

wstring CEffect_Base::ObjID = TEXT("CEffect_Base");
set<_wstring> CEffect_Base::s_PrototypeTags = {};

CEffect_Base::CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffect_Base::CEffect_Base(const CEffect_Base& rhs)
	: CGameObject(rhs)
{
}

CEffect_Base::~CEffect_Base()
{
	Free();
}

void CEffect_Base::EffectStart()
{
	SetEffectTimeAcc(GetEffectTimeMin());
	SetEffectIsView(true);
	SetEffectTimeFreeze(false);
	m_bEffectOn = true;
	EffectStartCallBack();
}

void CEffect_Base::EffectEnd()
{
	EffectEndCallBack();
	m_bEffectOn = false;
	SetEffectIsView(false);
	SetEffectTimeFreeze(true);
	SetEffectTimeAcc(GetEffectTimeMax());
	m_pTransformCom->Set_WorldMatrix(GetRootMatrix());
}

void CEffect_Base::EffectTick(_float fTimeDelta)
{

	TimeAcc(fTimeDelta);
	Turning(fTimeDelta);
	UvStrain(fTimeDelta);
}

void CEffect_Base::EffectInit()
{
	AddEffectColorPreset(_float4{ 1.f,1.f,1.f,1.f });
	AddEffectColorPreset(_float4{ 1.f,1.f,1.f,1.f });
	AddEffectColorPreset(_float4{ 1.f,1.f,1.f,1.f });
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CEffect_Base::EffectParamBind(shared_ptr<CShader> _pShader)
{
	_pShader->Bind_RawValue("g_vEffectBaseColor", &m_vEffectBaseColor, sizeof(_float4));

	_pShader->Bind_RawValue("g_vEffectBaseColorR", &m_vEffectColorPresets[0], sizeof(_float4));
	_pShader->Bind_RawValue("g_vEffectBaseColorG", &m_vEffectColorPresets[1], sizeof(_float4));
	_pShader->Bind_RawValue("g_vEffectBaseColorB", &m_vEffectColorPresets[2], sizeof(_float4));

	_pShader->Bind_RawValue("g_vEffectColorBlendRate", &m_fEffectColorBlendRate, sizeof(_float));

	_pShader->Bind_RawValue("g_fEffectTimeAcc", &m_fEffectTimeAcc, sizeof(_float));

	_float2 vFlowSpeed = _float2{ m_fEffectUvFlowX, m_fEffectUvFlowY };

	_pShader->Bind_RawValue("g_vUVFlow", &vFlowSpeed, sizeof(_float2));

	_pShader->Bind_RawValue("g_fEffectAlpha", m_bEffectAlphaLinkTimePer ? &m_fEffectTimePercentReverse : &m_fEffectAlpha, sizeof(_float));

	_float2 vUvMultiplier = _float2{ m_fEffectUvSizeXMultiplier, m_fEffectUvSizeYMultiplier };

	_pShader->Bind_RawValue("g_vUVMultiplier", &vUvMultiplier, sizeof(_float2));
}

void CEffect_Base::TimeAcc(_float fTimeDelta)
{
	if (Get_EffectLink()) {
		_float fLinkTime = *m_fEffectLinkTime;
		if (fLinkTime > m_fEffectTimeMin) {
			if (!Get_PlayOnce()) {
				Set_PlayOnce(true);
				EffectStart();
			}
		}
	}

	if (Get_Pause()) return;

	if(!GetEffectTimeFreeze()) SetEffectTimeAcc(GetEffectTimeAcc() + (fTimeDelta * GetEffectTimeAccMultiplier()));
	
	m_fEffectTimePercent = (GetEffectTimeAcc() - GetEffectTimeMin()) / (GetEffectTimeMax() - GetEffectTimeMin());
	m_fEffectTimePercentReverse = 1.f - m_fEffectTimePercent;
	if (GetEffectTimeAcc() >= GetEffectTimeMax())
	{
		if (GetEffectLoop()) {
			SetEffectTimeAcc(GetEffectTimeMin());
			EffectEnd();
			EffectStart();
		} else {
			SetEffectTimeAcc(GetEffectTimeMax());
			EffectEnd();
		}
	}
}

void CEffect_Base::Turning(_float _fTimeDelta)
{
	if(GetEffectIsTurn() && !GetEffectTimeFreeze())
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.f, _fTimeDelta * GetTurnSpeed() );
}

void CEffect_Base::ToolTurning(_float _fTime)
{
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.f, _fTime * GetTurnSpeed());
}

void CEffect_Base::UvStrain(_float _fTimeDelta)
{
	m_fEffectUvFlowX = 0.f; m_fEffectUvFlowY = 0.f;

	if (GetUseEffectUvFlowX()) m_fEffectUvFlowX = m_fEffectUvFlowXSpeed * m_fEffectTimePercent;
	if (GetUseEffectUvFlowY()) m_fEffectUvFlowY = m_fEffectUvFlowYSpeed * m_fEffectTimePercent;
}

_float4 CEffect_Base::GetColorPresetR()
{
	if (m_vEffectColorPresets.empty())
		return { 0.f,0.f,0.f,0.f };
	return m_vEffectColorPresets[0];
}

void CEffect_Base::SetColorPresetR(const _float4& vColor)
{
	if (m_vEffectColorPresets.size() < 1)
		m_vEffectColorPresets.push_back(vColor);
	else
		m_vEffectColorPresets[0] = vColor;
}

_float4 CEffect_Base::GetColorPresetG()
{
	if (m_vEffectColorPresets.size() < 2)
		return { 0.f,0.f,0.f,0.f };
	return m_vEffectColorPresets[1];
}

void CEffect_Base::SetColorPresetG(const _float4& vColor)
{
	if (m_vEffectColorPresets.size() == 1)
		m_vEffectColorPresets.push_back(vColor);
	else if (m_vEffectColorPresets.size() == 0)
	{
		m_vEffectColorPresets.push_back(vColor);
		m_vEffectColorPresets.push_back(vColor);
	}
	else
		m_vEffectColorPresets[1] = vColor;
}

_float4 CEffect_Base::GetColorPresetB()
{
	if (m_vEffectColorPresets.size() < 3)
		return { 0.f,0.f,0.f,0.f };
	return m_vEffectColorPresets[2];
}

void CEffect_Base::SetColorPresetB(const _float4& vColor)
{
	if (m_vEffectColorPresets.size() == 2)
		m_vEffectColorPresets.push_back(vColor);
	else if (m_vEffectColorPresets.size() == 1)
	{
		m_vEffectColorPresets.push_back(vColor);
		m_vEffectColorPresets.push_back(vColor);
	}
	else if (m_vEffectColorPresets.size() == 0)
	{
		m_vEffectColorPresets.push_back(vColor);
		m_vEffectColorPresets.push_back(vColor);
		m_vEffectColorPresets.push_back(vColor);
	}
	else
		m_vEffectColorPresets[2] = vColor;
}

void CEffect_Base::SetColorPresetRG(const _float4& vColorR, const _float4& vColorG)
{
	SetColorPresetR(vColorR); SetColorPresetG(vColorG);
}

void CEffect_Base::SetColorPresetRB(const _float4& vColorR, const _float4& vColorB)
{
	SetColorPresetR(vColorR); SetColorPresetB(vColorB);
}

void CEffect_Base::SetColorPresetGB(const _float4& vColorG, const _float4& vColorB)
{
	SetColorPresetG(vColorG); SetColorPresetB(vColorB);
}

void CEffect_Base::SetColorPresetRGB(const _float4& vColorR, const _float4& vColorG, const _float4& vColorB)
{
	SetColorPresetR(vColorR); SetColorPresetG(vColorG); SetColorPresetB(vColorB);
}

void CEffect_Base::Set_ProgressBar_Inversion(_float _fProgressPer)
{
	m_pTransformCom->Set_WorldMatrix(GetRootMatrix());
	SetEffectTimeAcc(GetEffectTimeMin() + (GetEffectTimeMax() - GetEffectTimeMin()) * _fProgressPer);
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.f, _fProgressPer * GetTurnSpeed());

}

void CEffect_Base::Free()
{

}
