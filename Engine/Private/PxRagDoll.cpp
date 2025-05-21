#include "..\Public\PxRagDoll.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"
#include "Bone.h"

CPxRagDoll::CPxRagDoll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CPxRagDoll::CPxRagDoll(const CPxRagDoll& rhs)
	: CComponent(rhs),
	m_pPhysics{ rhs.m_pPhysics },
	m_pCurrentScene{ rhs.m_pCurrentScene }
{
}

CPxRagDoll::~CPxRagDoll()
{
	Free();
}

HRESULT CPxRagDoll::Initialize_Prototype()
{
	m_pPhysics = CGameInstance::Get_Instance()->Get_Physics();
	m_pCurrentScene = CGameInstance::Get_Instance()->Get_Scene();
	return S_OK;
}

HRESULT CPxRagDoll::Initialize(void* pArg)
{
	return S_OK;
}

void CPxRagDoll::Update(_float fTimeDelta)
{
	if (!m_bActive || !m_pModel || !m_pTransform)
		return;
	_matrix preTransform = XMLoadFloat4x4(&m_pModel->Get_ModelData()->TransformMatrix);
	_matrix InvWorld = XMMatrixInverse(nullptr, (m_pTransform->Get_WorldMatrix_XMMat()));
	_matrix Scale = XMMatrixScalingFromVector({ m_pTransform->Get_Scale().x * 0.01f, m_pTransform->Get_Scale().y * 0.01f, m_pTransform->Get_Scale().z * 0.01f });
	auto& bones = m_pModel->Get_Bones();
	_uint idx = 0;
	for (auto& pActor : m_Nodes)
	{
		if (!pActor)
			continue;
		PxTransform Pose = pActor->getGlobalPose();
		PxMat44 mat(Pose);
		_matrix world = PxMatToXMMatrix(mat);
		_float4x4 fWorld;

		XMStoreFloat4x4(&fWorld, Scale * world * InvWorld);
		bones[idx]->Set_CombinedTransformationFloat4x4Ptr(fWorld);

		++idx;
	}
	
	
}

void CPxRagDoll::Slice(const _float4& plane, _bool bReverse)
{
	Slice(XMLoadFloat4(&plane), bReverse);
}

void CPxRagDoll::Slice(_fvector plane, _bool bReverse )
{
	Deactivate();

	for (_uint i = 0; i < m_Nodes.size(); ++i)
	{
		if (!m_Nodes[i]->getNbShapes())
			continue;
		PxMat44 pxMat{ m_Nodes[i]->getGlobalPose() };
		_matrix world = PxMatToXMMatrix(pxMat);
		_float dist = XMVectorGetX(XMPlaneDot(plane, world.r[3]));
		if (dist >= 0.f == bReverse)
		{
			PxShape* pShape;
			m_Nodes[i]->getShapes(&pShape, 1, 0);
			m_Nodes[i]->detachShape(*pShape);
		}
	}

	Activate();
}

void CPxRagDoll::Slice(const vector<_float4>& planes, _bool bReverse)
{
	Deactivate();

	for (_uint i = 0; i < m_Nodes.size(); ++i)
	{
		if (!m_Nodes[i]->getNbShapes())
			continue;
		PxMat44 pxMat{ m_Nodes[i]->getGlobalPose() };
		_matrix world = PxMatToXMMatrix(pxMat);
		for (auto& plane : planes)
		{
			_vector vPlane = XMLoadFloat4(&plane);
			_float dist = XMVectorGetX(XMPlaneDot(vPlane, world.r[3]));
			if (dist >= 0.f == bReverse)
			{
				PxShape* pShape;
				m_Nodes[i]->getShapes(&pShape, 1, 0);
				m_Nodes[i]->detachShape(*pShape);
				break;
			}
		}
	}
	Activate();
}

void CPxRagDoll::Build_Ragdoll(const PXRAGDOLL_DESC& desc, const vector<_bool>& detache)
{
	Deactivate();
	m_Articulations.resize(1);
	m_pModel = desc.pModel;
	m_pTransform = desc.pTransform;
	m_Articulations[0] = m_pPhysics->createArticulationReducedCoordinate();
	m_fSFriction = desc.fStaticFriction;
	m_fDFriction = desc.fDynamicFriction;
	m_fRestitution = desc.fRestitution;
	
	PxMaterial* material = m_pPhysics->createMaterial(desc.fStaticFriction, desc.fDynamicFriction, desc.fRestitution);
	auto& bones = m_pModel->Get_Bones();
	_uint iNumBone = bones.size();
	_bool bDetache = detache.size() == iNumBone;


	m_Nodes.resize(iNumBone);
	m_Scales.resize(iNumBone);
	m_Children = vector<vector<_uint>>(iNumBone, vector<_uint>());
	_uint iRootIdx = 0;
	for (_uint i = 0; i < iNumBone; ++i)
	{
		if(bones[i]->Get_Parent() == -1)
		{
			iRootIdx = i;
		}
		else
		{
			m_Children[bones[i]->Get_Parent()].push_back(i);
		}
	}

	//_matrix preTransform = XMLoadFloat4x4(&m_pModel->Get_ModelData()->TransformMatrix);
	_matrix worldMat =  m_pTransform->Get_WorldMatrix_XMMat();
	_matrix BoneMat = bones[iRootIdx]->Get_CombinedTransformationMatrix();
	_matrix transform = BoneMat * worldMat;
	_float3 vScale{ XMVector3Length(BoneMat.r[0]).m128_f32[0],
			   XMVector3Length(BoneMat.r[1]).m128_f32[0],
			   XMVector3Length(BoneMat.r[2]).m128_f32[0] };
	
	m_Scales[iRootIdx] = vScale;
	m_Nodes[iRootIdx] = m_Articulations[0]->createLink(NULL, PxTransform{XMMatrixToPxMat(XMMatrixNormalize(transform))});
	PxSphereGeometry sphereGeo{ 0.15f * vScale.y * 0.1f };
	PxShape* pShape = m_pPhysics->createShape(sphereGeo, *material);
	if(!bDetache || !detache[iRootIdx])
		m_Nodes[iRootIdx]->attachShape(*pShape);
	
	queue<pair<_uint, _uint>> Q;

	for (_uint firstGen : m_Children[iRootIdx])
	{
		Q.push({ iRootIdx, firstGen });
	}
	while (Q.size())
	{
		auto [p, c] = Q.front(); Q.pop();
		// connect p, c;
		BoneMat = bones[c]->Get_CombinedTransformationMatrix();
		PxTransform pxTrans = PxTransform{ XMMatrixToPxMat(XMMatrixNormalize(BoneMat * worldMat)) };
		m_Nodes[c] = m_Articulations[0]->createLink(m_Nodes[p],  pxTrans);
		if (!bDetache || !detache[c])
			m_Nodes[c]->attachShape(*pShape);
		m_Scales[c] = { XMVector3Length(BoneMat.r[0]).m128_f32[0],
			   XMVector3Length(BoneMat.r[1]).m128_f32[0],
			   XMVector3Length(BoneMat.r[2]).m128_f32[0] };

		PxTransform pTrans = m_Nodes[p]->getGlobalPose();
		PxTransform cTrans = m_Nodes[c]->getGlobalPose();
		PxArticulationJointReducedCoordinate* joint = m_Nodes[c]->getInboundJoint();
		joint->setParentPose(m_Nodes[p]->getGlobalPose().getInverse() * pxTrans);
		joint->setChildPose(m_Nodes[c]->getGlobalPose().getInverse() * pxTrans);

		joint->setJointType(PxArticulationJointType::eSPHERICAL);

		joint->setLimitParams(PxArticulationAxis::eSWING1, PxArticulationLimit{ -PX_PIDIV2 * 0.25f, PX_PIDIV2 * 0.25f });
		joint->setLimitParams(PxArticulationAxis::eSWING2, PxArticulationLimit{ -PX_PIDIV2 * 0.5f , PX_PIDIV2 * 0.5f });

		joint->setMotion(PxArticulationAxis::eSWING1, PxArticulationMotion::eLIMITED);
		joint->setMotion(PxArticulationAxis::eSWING2, PxArticulationMotion::eLIMITED);


		for (_uint next : m_Children[c])
		{
			Q.push({ c, next });
		}
	}
	m_Roots.resize(1);
	m_Roots[0] = iRootIdx;
	m_iRootIdx = iRootIdx;
}

void CPxRagDoll::Build_Ragdoll_With_Validity(const PXRAGDOLL_DESC& desc, const vector<_bool>& validBones)
{
	Deactivate();
	m_pModel = desc.pModel;
	m_pTransform = desc.pTransform;
	m_fSFriction = desc.fStaticFriction;
	m_fDFriction = desc.fDynamicFriction;
	m_fRestitution = desc.fRestitution;

	auto& bones = m_pModel->Get_Bones();
	_uint iNumBone = bones.size();
	m_Nodes.clear();
	m_Nodes.resize(iNumBone);
	m_Scales.resize(iNumBone);
	m_Children = vector<vector<_uint>>(iNumBone, vector<_uint>());

	for (_uint i = 0; i < iNumBone; ++i)
	{
		if (bones[i]->Get_Parent() == -1)
		{
			m_iRootIdx = i;
		}
		else
		{
			m_Children[bones[i]->Get_Parent()].push_back(i);
		}
	}
	m_Roots.resize(1);
	m_Roots[0] = m_iRootIdx;


	queue<pair<_uint, _uint>> Q;
	for(_uint iRootIdx : m_Roots)
	{
		for (_uint firstGen : m_Children[iRootIdx])
		{
			Q.push({ iRootIdx, firstGen });
		}
	}

	while (Q.size())
	{
		auto [p, c] = Q.front(); Q.pop();
		if (!validBones[p])
		{
			auto iter = find(m_Roots.begin(), m_Roots.end(), p);
			if(m_Roots.end() != iter)
			m_Roots.erase(iter);
			if (validBones[c])
			{
				if(m_Roots.end() == find(m_Roots.begin(), m_Roots.end(), c))
					m_Roots.push_back(c);
			}
		}
		if (!validBones[c])
		{
			auto iter = find(m_Children[p].begin(), m_Children[p].end(), c);
			if(m_Children[p].end() != iter)
				m_Children[p].erase(iter);
		}

		for (_uint next : m_Children[c])
		{
			Q.push({ c,next });
		}
	}
	


	Rebuild();
}

void CPxRagDoll::Activate()
{
	m_bActive = true;
	for (auto Articulation : m_Articulations)
	{
		m_pCurrentScene->addArticulation(*Articulation);
	}
}

void CPxRagDoll::Deactivate()
{
	m_bActive = false;
	for (auto Articulation : m_Articulations)
	{
		m_pCurrentScene->removeArticulation(*Articulation);
	}
}

void CPxRagDoll::Purge_Nodes()
{
	for (auto& pActor : m_Nodes)
	{
		m_pCurrentScene->removeActor(*pActor);
	}
	m_Nodes.clear();
}

vector<_bool> CPxRagDoll::Get_Detached() const
{
	vector<_bool> rt = {};
	for (auto& actor : m_Nodes)
	{
		if (actor)
			rt.push_back(actor->getNbShapes() == 0);
		else
			rt.push_back(false);
	}
	return rt;
}

void CPxRagDoll::Set_Velocity(_uint iArticulationIdx, const _float3& vVelocity)
{
	PxVec3 pxVel{ vVelocity.x, vVelocity.y, vVelocity.z };
	Set_Velocity(iArticulationIdx, pxVel);
}

void CPxRagDoll::Set_Velocity(_uint iArticulationIdx, const _float4& vVelocity)
{
	PxVec3 pxVel{ vVelocity.x, vVelocity.y, vVelocity.z };
	Set_Velocity(iArticulationIdx, pxVel);
}

void CPxRagDoll::Set_Velocity(_uint iArticulationIdx, _fvector vVelocity)
{
	PxVec3 pxVel{ vVelocity.m128_f32[0],  vVelocity.m128_f32[1],  vVelocity.m128_f32[2] };
	Set_Velocity(iArticulationIdx, pxVel);
}

void CPxRagDoll::Set_Velocity(_uint iArticulationIdx, const PxVec3& vVelocity)
{
	m_Articulations[iArticulationIdx]->setRootLinearVelocity(vVelocity);
}

void CPxRagDoll::ApplyBones()
{
	if (!m_pModel)
		return;
	_matrix preTransform = XMLoadFloat4x4(&m_pModel->Get_ModelData()->TransformMatrix);
	_matrix World =  (m_pTransform->Get_WorldMatrix_XMMat());
	_matrix Scale = XMMatrixScalingFromVector({  m_pTransform->Get_Scale().x, m_pTransform->Get_Scale().y, m_pTransform->Get_Scale().z });
	auto& bones = m_pModel->Get_Bones();
	_uint idx = 0;
	for (auto& pActor : m_Nodes)
	{
		if (!pActor)
			continue;
		_matrix BoneMat = bones[idx]->Get_CombinedTransformationMatrix();

		PxMat44 mat;
		mat = XMMatrixToPxMat(XMMatrixNormalize( BoneMat * World));

		pActor->setGlobalPose(PxTransform{ mat });

		++idx;
	}
}

void CPxRagDoll::Rebuild()
{
	auto& bones = m_pModel->Get_Bones();
	PxMaterial* material = m_pPhysics->createMaterial(m_fSFriction,m_fDFriction,m_fRestitution);
	m_Articulations.resize(m_Roots.size());
	for(_uint i = 0; i < m_Roots.size(); ++i)
	{
		m_Articulations[i] = m_pPhysics->createArticulationReducedCoordinate();
		_uint iRootIdx = m_Roots[i];
		_matrix worldMat = m_pTransform->Get_WorldMatrix_XMMat();
		_matrix BoneMat = bones[iRootIdx]->Get_CombinedTransformationMatrix();
		_matrix transform = BoneMat * worldMat;
		_float3 vScale{ XMVector3Length(BoneMat.r[0]).m128_f32[0],
				   XMVector3Length(BoneMat.r[1]).m128_f32[0],
				   XMVector3Length(BoneMat.r[2]).m128_f32[0] };

		m_Scales[iRootIdx] = vScale;
		m_Nodes[iRootIdx] = m_Articulations[i]->createLink(NULL, PxTransform{ XMMatrixToPxMat(XMMatrixNormalize(transform)) });
		PxSphereGeometry sphereGeo{ 0.15f * vScale.y * 0.1f };
		PxShape* pShape = m_pPhysics->createShape(sphereGeo, *material);
		m_Nodes[iRootIdx]->attachShape(*pShape);

		queue<pair<_uint, _uint>> Q;

		for (_uint firstGen : m_Children[iRootIdx])
		{
			Q.push({ iRootIdx, firstGen });
		}
		while (Q.size())
		{
			auto [p, c] = Q.front(); Q.pop();
			// connect p, c;
			BoneMat = bones[c]->Get_CombinedTransformationMatrix();
			PxTransform pxTrans = PxTransform{ XMMatrixToPxMat(XMMatrixNormalize(BoneMat * worldMat)) };
			m_Nodes[c] = m_Articulations[i]->createLink(m_Nodes[p], pxTrans);
			m_Nodes[c]->attachShape(*pShape);
			m_Scales[c] = { XMVector3Length(BoneMat.r[0]).m128_f32[0],
				   XMVector3Length(BoneMat.r[1]).m128_f32[0],
				   XMVector3Length(BoneMat.r[2]).m128_f32[0] };

			PxTransform pTrans = m_Nodes[p]->getGlobalPose();
			PxTransform cTrans = m_Nodes[c]->getGlobalPose();
			PxArticulationJointReducedCoordinate* joint = m_Nodes[c]->getInboundJoint();
			joint->setParentPose(m_Nodes[p]->getGlobalPose().getInverse() * pxTrans);
			joint->setChildPose(m_Nodes[c]->getGlobalPose().getInverse() * pxTrans);

			joint->setJointType(PxArticulationJointType::eSPHERICAL);

			joint->setLimitParams(PxArticulationAxis::eSWING1, PxArticulationLimit{ -PX_PIDIV2 * 0.25f, PX_PIDIV2 * 0.25f });
			joint->setLimitParams(PxArticulationAxis::eSWING2, PxArticulationLimit{ -PX_PIDIV2 * 0.5f , PX_PIDIV2 * 0.5f });

			joint->setMotion(PxArticulationAxis::eSWING1, PxArticulationMotion::eLIMITED);
			joint->setMotion(PxArticulationAxis::eSWING2, PxArticulationMotion::eLIMITED);


			for (_uint next : m_Children[c])
			{
				Q.push({ c, next });
			}
		}
	}
}

shared_ptr<CPxRagDoll> CPxRagDoll::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CPxRagDoll
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPxRagDoll(pDevice, pContext) {}
	};

	shared_ptr<CPxRagDoll> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CPxRagDoll"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CPxRagDoll::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CPxRagDoll
	{
		MakeSharedEnabler(const CPxRagDoll& rhs) : CPxRagDoll(rhs) {}
	};

	shared_ptr<CPxRagDoll> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPxRagDoll"));
		return nullptr;
	}

	return pInstance;
}

void CPxRagDoll::Free()
{
	Deactivate();
}
