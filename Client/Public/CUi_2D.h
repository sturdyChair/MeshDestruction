#pragma once

#include "Client_Defines.h"
#include "CUi.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUi_2D abstract : public CUi
{
public:
	enum class ColorType { R, G, B, A, End };
	using Desc = struct CUi_2DInfo : public Ui_DefaultStructInfo
	{
		LEVEL TextureLevel = { LEVEL_STATIC };
		wstring TextureFileName;
	};

	using FontDescInfo = struct SpriteFontInfo
	{
		wstring FontTag = L"Nexon";
		wstring String = L"";
		_float2 DirectPos = { 0, 0 };
		_float Size = { 0.7f };
		_float Rotation = { 0 };
		_float4 Color = { 1, 1, 1, 1 };
	};

protected:
	CUi_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_2D(const CUi_2D& rhs);


public:
	virtual ~CUi_2D();


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	virtual void Move(MoveDir Dir, _float fTimeDelta) override;
	virtual void Rotation(_float fAngle, _fvector vAxis = XMVectorSet(0, 0, 1, 1)) override;
	virtual void Add_Scale(_float fX, _float fY);
	virtual void Mul_Scale(_float Multiplier);
	virtual void Add_TextureNum(_int Num);
	virtual void Set_TextureNum(_uint Num);
	virtual void Set_Color(_float4 Color) { m_fColor = Color; }
	virtual void Set_Color(ColorType Type, _float Color);
	virtual void Add_Color(_float4 Color);
	virtual void Add_Color(ColorType Type, _float Color);
	_float4 Get_Color() const { return m_fColor; }
	_uint Get_TextureNum() const { return m_iTextureNum; }


protected:
	virtual void RenderFont(FontDescInfo Desc, _uint yGap = 20, _uint EnterCount = 0);
	_uint Num_Quotation(FontDescInfo& Desc, _uint Number);
	

protected:
	virtual HRESULT Ready_Components(void* pArg) override;


protected:
	_float4x4 m_ProjMatrix = {};
	_uint m_iTextureNum = { 0 };
	FontDescInfo m_FontDesc = {};


protected:
	shared_ptr<CTexture> m_pTextureCom = { nullptr };
	shared_ptr<CVIBuffer_Rect>	m_pVIBufferCom = { nullptr };


public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) PURE;
};

END
