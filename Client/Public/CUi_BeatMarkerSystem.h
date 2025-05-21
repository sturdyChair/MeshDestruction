#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_BeatMarkerSystem : public CUi_2D
{
public:
	static wstring ObjID;


public:
	enum class JudgeType { Perfect, Good, Miss, End };


protected:
	CUi_BeatMarkerSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_BeatMarkerSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_BeatMarkerSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Create_BeatMarker();
	void Check_Beat();
	void Check_Judge();
	void Adjust_Beat(_float fTimeDelta);
	void Cal_Judge();
	void Create_Judge(JudgeType Type);
	void Delete_Marker();


private:
	_float Cal_Speed();
	void Adjust_DefaultVec(_float fTimeDelta);
	void DefaultVec_ColorAdjust(_float fTimeDelta, _bool Decrease);
	void DefaultVec_ScaleAdjust(_float fTimeDelta, _bool Decrease);
	void Initialize_DefaultVec();


private:
	_bool m_bBeatChange = { false };
	_uint m_iCheckBeat = { 0 };
	_float m_fBeatMarkerSizeX = { 40 };
	_float m_fBeatMarkerPosX = { 450 };


private:
	list<shared_ptr<class CUi_BeatMarker>> m_pMarkerList;
	vector<shared_ptr<class CUi_Default>> m_DefaultVec;


public:
	static shared_ptr<CUi_BeatMarkerSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
