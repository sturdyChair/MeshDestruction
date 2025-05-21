#include "..\Public\Cutter_Controller.h"
#include "Model_Cutter.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"

CCutter_Controller::CCutter_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CCutter_Controller::CCutter_Controller(const CCutter_Controller& rhs)
	: CComponent(rhs)
{
}

CCutter_Controller::~CCutter_Controller()
{
	Free();
}

HRESULT CCutter_Controller::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CCutter_Controller::Initialize(void* pArg)
{
	CGameObject* pOwner = ((CUTTER_DESC*)pArg)->pOwner;

	if (((CUTTER_DESC*)pArg)->strModelComTag != L"")
		m_pModel = dynamic_pointer_cast<CModel>(pOwner->Find_Component(((CUTTER_DESC*)pArg)->strModelComTag));
	else
		m_pModel = dynamic_pointer_cast<CModel>(pOwner->Find_Component_By_Type<CModel>());
	m_fApartAmount = ((CUTTER_DESC*)pArg)->fApartAmount;
	assert(m_pModel);

	_uint iNumMesh = m_pModel->Get_NumMeshes();
	CModel_Cutter::MODEL_CUTTER_DESC Desc{};
	Desc.pModel = m_pModel;
	for (_uint i = 0; i < iNumMesh; ++i)
	{
		Desc.iMeshIdx = i;
		auto pMC = static_pointer_cast<CModel_Cutter>(GAMEINSTANCE->Clone_Component(0, CModel_Cutter::s_PrototypeTag, &Desc));
		m_Computers.push_back(pMC);
		m_Offsets.push_back({ 0.f,0.f,0.f });
		m_RealOffsets.push_back({ 0.f,0.f,0.f });
		m_Recursions.push_back(m_iMaxRecursion);
		m_LifeTimes.push_back(m_fMaxLifeTime);
	}
	return S_OK;
}

void CCutter_Controller::Update(_float fTimeDelta)
{
	for (_uint i = 0; i < m_RealOffsets.size(); ++i)
	{
		_vector Off = XMLoadFloat3(&m_Offsets[i]), Real = XMLoadFloat3(&m_RealOffsets[i]);
		_vector Diff = Off - Real;
		if (XMVectorGetX(XMVector3Length(Diff)) < 0.001f)
		{
			m_RealOffsets[i] = m_Offsets[i];
		}
		else
		{
			XMStoreFloat3(&m_RealOffsets[i], Real + Diff * fTimeDelta / m_fOffsetInterpolTime);
		}
		if (m_Recursions[i] < m_iMaxRecursion)
		{
			m_LifeTimes[i] -= fTimeDelta;
			if (m_LifeTimes[i] <= 0.f)
			{
				m_LifeTimes[i] = 0.f;
			}
		}
	}
}

void CCutter_Controller::Cut(const _float4& plane)
{
	_matrix World = Get_Owner()->Get_Transform()->Get_WorldMatrix_XMMat();
	_vector vPlane = XMLoadFloat4(&plane);
	_uint iNumComputers = m_Computers.size();
	CModel_Cutter::MODEL_CUTTER_DESC Desc{};
	Desc.pModel = m_pModel;
	for (_uint i = 0; i < iNumComputers; ++i)
	{
		if (!m_Computers[i])
			continue;
		_float4 LocalPlane;
		
		XMStoreFloat4(&LocalPlane, XMPlaneNormalize( XMPlaneTransform(vPlane, 
			XMMatrixTranspose(XMMatrixTranslation(m_Offsets[i].x, m_Offsets[i].y, m_Offsets[i].z) * World)
		)));
		vector<VTXANIMMESH> newVerti; vector<_uint2> newEdge;
		auto [vecUp, vecDown] = m_Computers[i]->Cut(LocalPlane, newVerti);
		if (vecUp.size() && vecDown.size() && newVerti.size())
		{
			_uint iCurrRecur = m_Recursions[i];
			m_pModel->Divide(i, newVerti, vecUp, vecDown, plane);
			

			Desc.iMeshIdx = i;
			if (iCurrRecur == 1)
			{
				m_Computers[i] = nullptr;
				m_Recursions[i] = iCurrRecur - 1;
			}
			else
			{
				auto pMC = static_pointer_cast<CModel_Cutter>(GAMEINSTANCE->Clone_Component(0, CModel_Cutter::s_PrototypeTag, &Desc));
				m_Computers[i] = pMC;
				m_Recursions[i] = iCurrRecur - 1;
			}
		
			Desc.iMeshIdx = m_Computers.size();
			if (iCurrRecur == 1)
			{
				m_Computers.push_back(nullptr);
				m_Recursions.push_back(iCurrRecur - 1);
			}
			else
			{
				auto pMC = static_pointer_cast<CModel_Cutter>(GAMEINSTANCE->Clone_Component(0, CModel_Cutter::s_PrototypeTag, &Desc));
				m_Computers.push_back(pMC);
				m_Recursions.push_back(iCurrRecur - 1);
			}
			if (iCurrRecur == 1)
			{
				m_LifeTimes[i] = m_fMaxLifeTime;
				m_LifeTimes.push_back(m_fMaxLifeTime);
			}
			else
			{
				m_LifeTimes[i] = m_fMaxLifeTime * 10.f;
				m_LifeTimes.push_back(m_fMaxLifeTime * 10.f);
			}
			m_Offsets.push_back({ m_Offsets[i].x, m_Offsets[i].y, m_Offsets[i].z });
			m_RealOffsets.push_back({ m_Offsets[i].x, m_Offsets[i].y, m_Offsets[i].z });
			m_Offsets[i] = { m_Offsets[i].x + LocalPlane.x * m_fApartAmount, 
							 m_Offsets[i].y + LocalPlane.y * m_fApartAmount,
							 m_Offsets[i].z + LocalPlane.z * m_fApartAmount };
			
			m_Offsets[Desc.iMeshIdx] = { m_Offsets[i].x - LocalPlane.x * m_fApartAmount * 2.f,
							 m_Offsets[i].y - LocalPlane.y * m_fApartAmount * 2.f,
							 m_Offsets[i].z - LocalPlane.z * m_fApartAmount * 2.f };
		}
	}
}

const vector<_float3>& CCutter_Controller::Get_Offsets() const
{
	return m_RealOffsets;
}

const vector<_float>& CCutter_Controller::Get_LifeTimes() const
{
	return m_LifeTimes;
}

shared_ptr<CCutter_Controller> CCutter_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CCutter_Controller
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCutter_Controller(pDevice, pContext) {}
	};

	shared_ptr<CCutter_Controller> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CCutter_Controller"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CCutter_Controller::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CCutter_Controller
	{
		MakeSharedEnabler(const CCutter_Controller& rhs) : CCutter_Controller(rhs) {}
	};

	shared_ptr<CCutter_Controller> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CCutter_Controller"));
		return nullptr;
	}

	return pInstance;
}

void CCutter_Controller::Free()
{
	m_Computers.clear();
	m_pModel = nullptr;
}
