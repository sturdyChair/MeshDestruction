#include "CUi_2D.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"


CUi_2D::CUi_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi(pDevice, pContext) 
{
	XMStoreFloat4x4(&m_ProjMatrix, 
		XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
}


CUi_2D::CUi_2D(const CUi_2D& rhs)
	:CUi(rhs)
{
	memcpy(&m_ProjMatrix, &rhs.m_ProjMatrix, sizeof(rhs.m_ProjMatrix));
}


CUi_2D::~CUi_2D()
{

}

HRESULT CUi_2D::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_2D::Initialize(void* pArg)
{
	return S_OK;
}

void CUi_2D::PriorityTick(_float fTimeDelta)
{
}

void CUi_2D::Tick(_float fTimeDelta)
{
}

void CUi_2D::LateTick(_float fTimeDelta)
{
}

HRESULT CUi_2D::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	return S_OK;
}

void CUi_2D::Move(MoveDir Dir, _float fTimeDelta)
{
	switch (Dir)
	{
	case MoveDir::Right:
		m_pTransformCom->Go_Right(fTimeDelta * m_fSpeed);
		break;
	case MoveDir::Left:
		m_pTransformCom->Go_Left(fTimeDelta * m_fSpeed);
		break;
	case MoveDir::Down:
		m_pTransformCom->Go_Down(fTimeDelta * m_fSpeed);
		break;
	case MoveDir::Up:
		m_pTransformCom->Go_Up(fTimeDelta * m_fSpeed);
		break;
	default:
		assert(false);
		break;
	}

}

void CUi_2D::Rotation(_float fAngle, _fvector vAxis)
{
	m_pTransformCom->Rotation(vAxis, fAngle);
}

void CUi_2D::Add_Scale(_float fX, _float fY)
{
	_float3 Scale = m_pTransformCom->Get_Scale();
	Scale.x += fX;
	Scale.y += fY;
	m_pTransformCom->Set_Scale(XMLoadFloat3(&Scale));
}

void CUi_2D::Mul_Scale(_float Multiplier)
{
	_float3 Scale = m_pTransformCom->Get_Scale();
	Scale.x *= Multiplier;
	Scale.y *= Multiplier;
	m_pTransformCom->Set_Scale(XMLoadFloat3(&Scale));
}

void CUi_2D::Add_TextureNum(_int Num)
{
	m_iTextureNum += Num;
}

void CUi_2D::Set_TextureNum(_uint Num)
{
	m_iTextureNum = Num;
}

void CUi_2D::Set_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x = Color;
		break;
	case ColorType::G:
		m_fColor.y = Color;
		break;
	case ColorType::B:
		m_fColor.z = Color;
		break;
	case ColorType::A:
		m_fColor.w = Color;
		break;
	default:
		assert(false);
		break;
	}
}

void CUi_2D::Add_Color(_float4 Color)
{
	m_fColor.x += Color.x;
	m_fColor.y += Color.y;
	m_fColor.z += Color.z;
	m_fColor.w += Color.w;
}

void CUi_2D::Add_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x += Color;
		break;
	case ColorType::G:
		m_fColor.y += Color;
		break;
	case ColorType::B:
		m_fColor.z += Color;
		break;
	case ColorType::A:
		m_fColor.w += Color;
		break;
	default:
		assert(false);
		break;
	}
}


void CUi_2D::RenderFont(FontDescInfo Desc, _uint yGap, _uint EnterCount)
{
	if (EnterCount == 0)
	{
		if (Desc.String.size() != 0)
		{
			m_pGameInstance.lock()->Render_Font(Desc.FontTag, Desc.String,
				Change_APICoordinate(Desc.DirectPos), XMLoadFloat4(&Desc.Color), Desc.Rotation, Desc.Size);
		}
	}
	else
	{
		_uint Temp = 0;
		size_t PreviousCount = 0;
		size_t Find = 0;
		FontDescInfo TempFont = Desc;
		for (_uint i = 0; i < EnterCount; ++i)
		{
			if (i == 0)
			{
				Find = Desc.String.find(L"&");
				TempFont.String = Desc.String.substr(0, Find);
				PreviousCount = Find + 1;
			}
			else
			{
				Find = Desc.String.find(L"&", PreviousCount);
				TempFont.DirectPos.y += yGap;
				TempFont.String = Desc.String.substr(PreviousCount, Find - PreviousCount);
				PreviousCount = Find + 1;
			}
			m_pGameInstance.lock()->Render_Font(TempFont.FontTag, TempFont.String,
				Change_APICoordinate(TempFont.DirectPos), XMLoadFloat4(&TempFont.Color), TempFont.Rotation, TempFont.Size);
		}
		TempFont.DirectPos.y += yGap;
		TempFont.String = Desc.String.substr(PreviousCount, Desc.String.size() - PreviousCount);
		m_pGameInstance.lock()->Render_Font(TempFont.FontTag, TempFont.String,
			Change_APICoordinate(TempFont.DirectPos), XMLoadFloat4(&TempFont.Color), TempFont.Rotation, TempFont.Size);
	}
}

_uint CUi_2D::Num_Quotation(FontDescInfo& Desc, _uint Number)
{
	_int Digit = Number;
	_int SingleDigit = 0;
	_int Count = { 0 };
	_int ReturnDigit = { 0 };
	while (Digit > 0)
	{
		Digit /= 10;
		++Count;
	}
	if (Count == 0)
	{
		Desc.String += L"0";
		return 0;
	}
	ReturnDigit = Count;
	_int QuotationCount = (Count-1) / 3;
	_int Quotationrest = Count % 3;
	while (Count > 0)
	{
		if (Quotationrest == 0 && QuotationCount > 0)
		{
			--QuotationCount;
			Quotationrest = 3;
			Desc.String += L",";
		}

		SingleDigit = _int(Number / pow(10, Count-1));

		Desc.String += to_wstring(SingleDigit);

		Number -= _int(SingleDigit * pow(10, Count - 1));
		--Count;
		--Quotationrest;
	}
	return ReturnDigit;
}

HRESULT CUi_2D::Ready_Components(void* pArg)
{
	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, Arg->ShaderFileName,
		L"Com_Shader"));

	if (nullptr == m_pShaderCom)
		assert(false);

	m_eShaderPass = Arg->ShaderPass;
	/* For.Com_Texture */
	m_pTextureCom = static_pointer_cast<CTexture>(Add_Component(Arg->TextureLevel, Arg->TextureFileName,
		L"Com_Texture"));

	if (nullptr == m_pTextureCom)
		assert(false);

	/* For.Com_VIBuffer */
	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect",
		TEXT("Com_VIBuffer")));

	if (nullptr == m_pVIBufferCom)
		assert(false);

	return S_OK;
}
