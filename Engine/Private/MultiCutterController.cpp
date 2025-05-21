#include "..\Public\MultiCutterController.h"
#include "Model_Cutter.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"
#include "PxRagDoll.h"

CMultiCutterController::CMultiCutterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CMultiCutterController::CMultiCutterController(const CMultiCutterController& rhs)
	: CComponent(rhs)
{
}

CMultiCutterController::~CMultiCutterController()
{
	Free();
}

HRESULT CMultiCutterController::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CMultiCutterController::Initialize(void* pArg)
{
	CGameObject* pOwner = ((CUTTER_DESC*)pArg)->pOwner;
	m_Models.resize(1);
	m_Computers.resize(1);
	m_ControlledMeshIdx.resize(1);
	m_RagDolls.resize(16);
	m_Planes.resize(1);
	m_Timers.resize(1);
	for (auto& pRagDoll : m_RagDolls)
	{
		pRagDoll = CPxRagDoll::Create(m_pDevice, m_pContext);
	}
	if (((CUTTER_DESC*)pArg)->strModelComTag != L"")
		m_Models[0] = dynamic_pointer_cast<CModel>(pOwner->Find_Component(((CUTTER_DESC*)pArg)->strModelComTag));
	else
		m_Models[0] = dynamic_pointer_cast<CModel>(pOwner->Find_Component_By_Type<CModel>());
	assert(m_Models[0]);

	_uint iNumMesh = m_Models[0]->Get_NumMeshes();
	CModel_Cutter::MODEL_CUTTER_DESC Desc{};
	Desc.pModel = m_Models[0];
	for (_uint i = 0; i < iNumMesh; ++i)
	{
		Desc.iMeshIdx = i;
		auto pMC = static_pointer_cast<CModel_Cutter>(GAMEINSTANCE->Clone_Component(0, CModel_Cutter::s_PrototypeTag, &Desc));
		m_Computers[0].push_back(pMC);
		m_ControlledMeshIdx[0].push_back(i);
	}
	return S_OK;
}

void CMultiCutterController::Update(_float fTimeDelta)
{
	for (auto& pRagDoll : m_RagDolls)
	{
		pRagDoll->Update(fTimeDelta);
	}
	if(m_Timers.size() >= 2)
	{
		for (auto& timer : m_Timers)
		{
			timer.first += fTimeDelta;
			timer.second += fTimeDelta;
		}
	}
}

void CMultiCutterController::Cut(const _float4& plane)
{
	_matrix World = Get_Owner()->Get_Transform()->Get_WorldMatrix_XMMat();
	_vector vPlane = XMLoadFloat4(&plane);
	_uint iNumComputers = m_Computers.size();
	

	for (_uint i = 0; i < iNumComputers; ++i)
	{
		CModel_Cutter::MODEL_CUTTER_DESC Desc{}, Desc2{};
		_bool bNeedNewModel = true;
		Desc.pModel = m_Models[i];
		if (!m_Computers[i].size())
			continue;
		_float4 LocalPlane;

		XMStoreFloat4(&LocalPlane, XMPlaneNormalize(XMPlaneTransform(vPlane,
			XMMatrixTranspose( World)
		)));
		m_Planes[i].push_back(plane);
		_uint iNumMesh = m_Computers[i].size();
		_uint iNumBone = m_Models[i]->Get_Bones().size();
		//vector<_bool> upValidBones(iNumBone, false), downValidBones(iNumBone, false);

		for(_uint j = 0; j < iNumMesh; ++j)
		{
			vector<VTXANIMMESH> newVerti; vector<_uint2> newEdge;
			auto [vecUp, vecDown] = m_Computers[i][j]->Cut(LocalPlane, newVerti);
			if (vecUp.size() && vecDown.size() && newVerti.size())
			{
				if (bNeedNewModel)
				{
					m_Models.push_back(static_pointer_cast<CModel>(m_Models[i]->Clone(nullptr)));
					m_Timers[i].second = 0.f;
					m_Timers.push_back({ m_Timers[i].first, 0.f});
					Desc2.pModel = m_Models.back();
					m_Computers.push_back({});
					m_ControlledMeshIdx.push_back({});
					bNeedNewModel = false;
				}

				auto [upBones, downBones] = m_Models[i]->Divide(m_Computers[i][j]->m_iMeshIdx, newVerti, vecUp, vecDown, plane);

				for (_uint k = 0; k < m_Models.size(); ++k)
				{
					if (i == k)
						continue;
					m_Models[k]->Push_Back_Mesh(m_Models[i]->Get_Mesh(m_Models[i]->Get_NumMeshes() - 1));
					m_Models[k]->Replace_Mesh(m_Models[i]->Get_Mesh(m_Computers[i][j]->m_iMeshIdx), m_Computers[i][j]->m_iMeshIdx);
				}

				Desc.iMeshIdx = m_Computers[i][j]->m_iMeshIdx;

				auto pMC = static_pointer_cast<CModel_Cutter>(GAMEINSTANCE->Clone_Component(0, CModel_Cutter::s_PrototypeTag, &Desc));
				m_Computers[i][j] = pMC;


				Desc2.iMeshIdx = m_Models[i]->Get_NumMeshes() - 1;

				pMC = static_pointer_cast<CModel_Cutter>(GAMEINSTANCE->Clone_Component(0, CModel_Cutter::s_PrototypeTag, &Desc2));
				m_Computers.back().push_back(pMC);
				m_ControlledMeshIdx.back().push_back(Desc2.iMeshIdx);
			}
		}
		if(!bNeedNewModel)
		{
			CPxRagDoll::PXRAGDOLL_DESC rdDesc{};
			rdDesc.fDynamicFriction = 0.4f;
			rdDesc.fRestitution = 0.5f;
			rdDesc.fStaticFriction = 0.5f;
			rdDesc.pTransform = Get_Owner()->Get_Transform();
			rdDesc.pModel = m_Models[i];

			_vector vLook = XMVector3Normalize(GAMEINSTANCE->Get_CamLook_Vector());
			_vector vPlane = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&plane), 0.f));
			auto detache = m_RagDolls[i]->Get_Detached();
			m_RagDolls[i]->Build_Ragdoll(rdDesc, detache);
			m_RagDolls[i]->Slice(plane);
			m_RagDolls[i]->Activate();
			m_RagDolls[i]->Set_Velocity(0,(vLook + vPlane) * 5.f);
			


			rdDesc.pModel = m_Models.back();

			if (m_RagDolls.size() < m_Models.size())
				m_RagDolls.push_back(CPxRagDoll::Create(m_pDevice, m_pContext));
			m_RagDolls[m_Models.size() - 1]->Build_Ragdoll(rdDesc, detache);
			m_Planes.push_back(m_Planes[i]);
			
			auto& lastplane = m_Planes.back().back();

			lastplane.x = -lastplane.x;
			lastplane.y = -lastplane.y;
			lastplane.z = -lastplane.z;
			lastplane.w = -lastplane.w;

			
			m_RagDolls[m_Models.size() - 1]->Slice(plane, true);
			m_RagDolls[m_Models.size() - 1]->Activate();
			m_RagDolls[m_Models.size() - 1]->Set_Velocity(0,(vLook - vPlane) * 5.f);
		}
	}
}

void CMultiCutterController::Deactivate()
{
	for (auto& pRagDoll : m_RagDolls)
	{
		pRagDoll->Deactivate();
	}
}

void CMultiCutterController::Activate()
{
	for (auto& pRagDoll : m_RagDolls)
	{
		pRagDoll->Activate();
	}
}

void CMultiCutterController::ApplyAnimation(_uint iIdx, _float fTrackPos)
{
	for (auto& model : m_Models)
	{
		model->Set_AnimIndex(iIdx, true);
		model->Set_CurrentTrackPosition(fTrackPos);
		_float4x4 out;
		model->Play_Animation(0.f, 0.f, 0.0001f, out); 
	}
}

void CMultiCutterController::ApplyBones()
{
	for (auto& pRagDoll : m_RagDolls)
	{
		pRagDoll->ApplyBones();
	}
}

shared_ptr<CMultiCutterController> CMultiCutterController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CMultiCutterController
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMultiCutterController(pDevice, pContext) {}
	};

	shared_ptr<CMultiCutterController> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CMultiCutterController"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CMultiCutterController::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CMultiCutterController
	{
		MakeSharedEnabler(const CMultiCutterController& rhs) : CMultiCutterController(rhs) {}
	};

	shared_ptr<CMultiCutterController> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CMultiCutterController"));
		return nullptr;
	}

	return pInstance;
}

void CMultiCutterController::Free()
{
	m_Computers.clear();
	m_Models.clear();
}
