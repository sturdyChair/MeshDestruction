#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEffect_Base : public CGameObject
{
public:
	static wstring ObjID;

public:
	enum EFFECT_STATE { EFFECT_START, EFFECT_LOOP, EFFECT_END, EFFECT_SLEEP };
	enum EFFECT_TYPE { EFFECT_NONE, EFFECT_MESH, EFFECT_PARTICLE, EFFECT_TEXTURE, EFFECT_TEXTURE_BILLBOARD };

protected:
	CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Base(const CEffect_Base& rhs);

public:
	virtual ~CEffect_Base();

public:
	virtual void EffectStartCallBack() PURE;
	virtual void EffectEndCallBack() PURE;
	
	virtual void EffectStart();
	virtual void EffectEnd();

	virtual void EffectSoftEnd() PURE;
	virtual void EffectHardEnd() PURE;

	void EffectTick(_float _fTimeDelta);
	void EffectInit();
	void EffectParamBind(shared_ptr<class CShader> _pShader);

	void TimeAcc(_float _fTimeDelta);
	void Turning(_float _fTimeDelta);
	void ToolTurning(_float _fTime);
	void UvStrain(_float _fTimeDelta);

	_float GetEffectTimeMin() { return m_fEffectTimeMin; }
	void SetEffectTimeMin(_float _fTimeMin) { m_fEffectTimeMin = _fTimeMin; }

	_float GetEffectTimeAcc() { return m_fEffectTimeAcc; }
	void SetEffectTimeAcc(_float _fTimeAcc) { m_fEffectTimeAcc = _fTimeAcc; }

	_float GetEffectTimeMax() { return m_fEffectTimeMax; }
	void SetEffectTimeMax(_float _fTimeMax) { m_fEffectTimeMax = _fTimeMax; }
	
	_float GetEffectTimeAccMultiplier() { return m_fEffectTimeAccMultiplier; }
	void SetEffectTimeAccMultiplier(_float _fTimeAccMultiplier) { m_fEffectTimeAccMultiplier = _fTimeAccMultiplier; }

	_bool GetEffectOn() { return m_bEffectOn; }
	_bool GetEffectPreEnd() { return m_bEffectPreEnd; }

	_bool GetEffectLoop() { return m_bEffectLoop; }
	void  SetEffectLoop(_bool _bLoop) { m_bEffectLoop = _bLoop; }
	
	_float GetEffectTimePercent() { return m_fEffectTimePercent; }
	_float GetEffectTImePercentReverse() { return m_fEffectTimePercentReverse; }

	_bool GetEffectTimeFreeze() { return m_bEffectTimeFreeze; }
	void SetEffectTimeFreeze(_bool _bEffectTimeFreeze) { m_bEffectTimeFreeze = _bEffectTimeFreeze; }

	_float4 GetEffectBaseColor() { return m_vEffectBaseColor; }
	void SetEffectBaseColorToPreset(_uint _iIndex) { m_vEffectBaseColor = m_vEffectColorPresets[_iIndex]; }
	void SetEffectBaseColorToColor(_float4 _vColor) { m_vEffectBaseColor = _vColor; }

	_float GetEffectColorBlendRate() { return m_fEffectColorBlendRate; }
	void SetEffectColorBlendRate(_float _fBlendRate) { m_fEffectColorBlendRate = _fBlendRate; }

	void AddEffectColorPreset(_float4 _vColor) { m_vEffectColorPresets.push_back(_vColor); }
	_float4 GetEffectColorPreset(_uint _iIndex) { return m_vEffectColorPresets[_iIndex]; }
	void DeleteEffectColorPreset(_uint _iIndex) { m_vEffectColorPresets.erase(m_vEffectColorPresets.begin() + _iIndex); }
	void ClearEffectColorPreset() { m_vEffectColorPresets.clear(); }

	_float2 GetEffectUvFlow() { return _float2(m_fEffectUvFlowX, m_fEffectUvFlowY); }
	void SetEffectUvFlow(_float _fUvFlowX, _float _fUvFlowY) { m_fEffectUvFlowX = _fUvFlowX; m_fEffectUvFlowY = _fUvFlowY; }

	_float GetEffectUvFlowX() { return m_fEffectUvFlowX; }
	void SetEffectUvFlowX(_float _fUvFlowX) { m_fEffectUvFlowX = _fUvFlowX; }

	_float GetEffectUvFlowY() { return m_fEffectUvFlowY; }
	void SetEffectUvFlowY(_float _fUvFlowY) { m_fEffectUvFlowY = _fUvFlowY; }

	_float GetEffectUvFlowXSpeed() { return m_fEffectUvFlowXSpeed; }
	void SetEffectUvFlowXSpeed(_float _fUvFlowXSpeed) { m_fEffectUvFlowXSpeed = _fUvFlowXSpeed; }

	_float GetEffectUvFlowYSpeed() { return m_fEffectUvFlowYSpeed; }
	void SetEffectUvFlowYSpeed(_float _fUvFlowYSpeed) { m_fEffectUvFlowYSpeed = _fUvFlowYSpeed; }

	_float2 GetEffectUvSizeMultiplier() { return _float2(m_fEffectUvSizeXMultiplier, m_fEffectUvSizeYMultiplier); }
	void SetEffectUvSizeMultiplier(_float _fUvSizeXMultiplier, _float _fUvSizeYMultiplier) { m_fEffectUvSizeXMultiplier = _fUvSizeXMultiplier; m_fEffectUvSizeYMultiplier = _fUvSizeYMultiplier; }
	void SetEffectUvSizeMultiplier(_float2 _vUvSizeMultiplier) { m_fEffectUvSizeXMultiplier = _vUvSizeMultiplier.x; m_fEffectUvSizeYMultiplier = _vUvSizeMultiplier.y; }

	_float GetEffectUvSizeXMultiplier() { return m_fEffectUvSizeXMultiplier; }
	void SetEffectUvSizeXMultiplier(_float _fUvSizeXMultiplier) { m_fEffectUvSizeXMultiplier = _fUvSizeXMultiplier; }

	_float GetEffectUvSizeYMultiplier() { return m_fEffectUvSizeYMultiplier; }
	void SetEffectUvSizeYMultiplier(_float _fUvSizeYMultiplier) { m_fEffectUvSizeYMultiplier = _fUvSizeYMultiplier; }

	_bool GetUseGlow() { return m_bUseGlow; }
	void SetUseGlow(_bool _bGlow) { m_bUseGlow = _bGlow; }

	_bool GetUseDistortion() { return m_bUseDistortion; }
	void SetUseDistortion(_bool _bDistortion) { m_bUseDistortion = _bDistortion; }

	_bool GetUseEffectUvFlowX() { return m_bUseEffectUvFlowX; }
	void SetUseEffectUvFlowX(_bool _bUseEffectUvFlowX) { m_bUseEffectUvFlowX = _bUseEffectUvFlowX; }

	_bool GetUseEffectUvFlowY() { return m_bUseEffectUvFlowY; }
	void SetUseEffectUvFlowY(_bool _bUseEffectUvFlowY) { m_bUseEffectUvFlowY = _bUseEffectUvFlowY; }

	_float GetEffectAlpha() { return m_fEffectAlpha; }
	void SetEffectAlpha(_float _fAlpha) { m_fEffectAlpha = _fAlpha; }

	_bool GetEffectAlphaLinkTime() { return m_bEffectAlphaLinkTimePer; }
	void SetEffectAlphaLinkTime(_bool _bAlphaLinkTimePer) { m_bEffectAlphaLinkTimePer = _bAlphaLinkTimePer; }

	_bool GetEffectIsView() { return m_bEffectView; }
	void SetEffectIsView(_bool _bEffectView) { m_bEffectView = _bEffectView; }

	_bool GetEffectIsTurn() { return m_bIsTurn; }
	void SetEffectIsTurn(_bool _bIsTurn) { m_bIsTurn = _bIsTurn; }

	_float GetTurnSpeed() { return m_fTurnSpeed; }
	void SetTurnSpeed(_float _fTurnSpeed) { m_fTurnSpeed = _fTurnSpeed; }

	_float4 GetColorPresetR();
	void	SetColorPresetR(const _float4& vColor);

	_float4 GetColorPresetG();
	void	SetColorPresetG(const _float4& vColor);

	_float4 GetColorPresetB();
	void	SetColorPresetB(const _float4& vColor);

	void	SetColorPresetRG(const _float4& vColorR, const _float4& vColorG);
	void	SetColorPresetRB(const _float4& vColorR, const _float4& vColorB);
	void	SetColorPresetGB(const _float4& vColorG, const _float4& vColorB);
	void	SetColorPresetRGB(const _float4& vColorR, const _float4& vColorG, const _float4& vColorB);

	_float4x4 GetRootMatrix() { return m_matRoot; }
	void SetRootMatrix(const _float4x4& matRoot) { m_matRoot = matRoot; }

	EFFECT_TYPE GetEffectType() { return m_eEffectType; }
	void SetEffectType(EFFECT_TYPE eEffectType) { m_eEffectType = eEffectType; }

	static const set<_wstring> & GetProtoTag() { return s_PrototypeTags; }

	_bool Get_EffectLink() { return m_bEffectLink; }
	void Set_EffectLink(_bool _bEffectLink) { m_bEffectLink = _bEffectLink; }

	void Set_EffectLinkTime(_float *_fEffectLinkTime) { m_fEffectLinkTime = _fEffectLinkTime; }

	void Set_ProgressBar_Inversion(_float _fProgressPer);

	_bool Get_Pause() { return m_eEffectPause; }
	void Set_Pause(_bool _bPause) { m_eEffectPause = _bPause; }

	_bool Get_PlayOnce() { return m_ePlayOnce; }
	void Set_PlayOnce(_bool _bPlayOnce) { m_ePlayOnce = _bPlayOnce; }


protected:
	_bool      m_bEffectView = true;

	_bool		m_bEffectOn = false;
	_bool		m_bEffectPreEnd = false;

	_bool       m_bEffectTimeFreeze = false;

	_float		m_fEffectTimePercent = 0.f;
	_float		m_fEffectTimePercentReverse = 1.f;

	_float		m_fEffectTimeAcc = 0.f;

	_bool	  m_ePlayOnce = false;
	_bool	  m_eEffectPause = false;
	_bool	  m_bEffectLink = false;
	_float* m_fEffectLinkTime = nullptr;

	vector<_float4> m_vEffectColorPresets;

	//Effect Data

	_float		m_fEffectTimeMin = 0.f;
	_float		m_fEffectTimeMax = 1.f;
	_float		m_fEffectTimeAccMultiplier = 1.f;

	_float4		m_vEffectBaseColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float      m_fEffectColorBlendRate = 1.f;

	_bool		m_bEffectLoop = false;

	_bool		m_bUseGlow = false;
	_bool		m_bUseDistortion = false;

	_bool		m_bIsTurn = false;

	_bool      m_bUseEffectUvFlowX = false;
	_bool      m_bUseEffectUvFlowY = false;
	
	_float     m_fEffectUvFlowX = 0.f;
	_float     m_fEffectUvFlowY = 0.f;

	_float     m_fEffectUvFlowXSpeed = 1.f;
	_float     m_fEffectUvFlowYSpeed = 1.f;

	_float	   m_fEffectUvSizeXMultiplier = 1.f;
	_float	   m_fEffectUvSizeYMultiplier = 1.f;

	_float	   m_fEffectAlpha = 1.f;
	_bool      m_bEffectAlphaLinkTimePer = false;

	_float	   m_fTurnSpeed = 1.f;

	_float4x4  m_matRoot = {};

	EFFECT_TYPE m_eEffectType = EFFECT_TYPE::EFFECT_NONE;

	static set<_wstring> s_PrototypeTags;

public:
	void Free();
};

END