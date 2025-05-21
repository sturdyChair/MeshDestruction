#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera :
    public CGameObject
{
public:
    struct CAMERA_DESC : public CTransform::TRANSFORM_DESC
    {
        _float4  vEye = {};
        _float4  vAt = {};
        _float   fFovy = {};
        _float   fAspect = {};
        _float   fNear = {};
        _float   fFar = {};
        _wstring strTag = {};
    };

protected:
    CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCamera(const CCamera& rhs);
public:
    virtual ~CCamera();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void PriorityTick(_float fTimeDelta)override;
    virtual void Tick(_float fTimeDelta)override;
    virtual void LateTick(_float fTimeDelta)override;
    virtual HRESULT Render()override;

    void    Set_Transform();

    void    Set_Fov(_float fFov) { m_fFovy = fFov; }
    _float  Get_Fov() const { return m_fFovy; }

private:
    _float4  m_vEye = {};
    _float4  m_vAt = {};
    _float   m_fFovy = {};
    _float   m_fAspect = {};
    _float   m_fNear = {};
    _float   m_fFar = {};

    _float4x4 m_ProjMatrix;

public:
    virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
    void Free();


};

END