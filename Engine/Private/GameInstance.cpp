#include "GameInstance.h"
#include "Component_Manager.h"
#include "Object_Manager.h"
#include "Object_Pool.h"
#include "ResourceContainer.h"
#include "Graphic_Device.h"
#include "DelayedProcessManager.h"
#include "Level_Manager.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Collision_Manager.h"
#include "PhysxManager.h"
#include "Timer_Manager.h"
#include "Math_Manager.h"
//#include "SoundManager.h"
#include "Font_Manager.h"
#include "Frustum.h"
#include "Voronoi_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)


CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{
	Free();
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext)
{
	/* 엔진을 사용하기위해 필요한 기타 초기화 작업들을 거친다. */

	/* 그래픽 디바이스 초기화. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_iWinSizeX = EngineDesc.iWinSizeX;
	m_iWinSizeY = EngineDesc.iWinSizeY;
	m_hWnd = EngineDesc.hWnd;
	/* 인풋 디바이스 초기화. */

	/* 사운드 디바이스 초기화. */

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	/* MathManager Already*/
	m_pMath_Manager = CMath_Manager::Create();
	if (nullptr == m_pMath_Manager)
		return E_FAIL;

	/* 오브젝트 매니져 사용할 준비. . */
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pResource_Container = CResource_Container::Create(*ppDevice, *ppContext);
	if (nullptr == m_pResource_Container)
		return E_FAIL;

	/* 컴포넌트 매니져 사용할 준비. . */
	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;
	m_pPhysxManager = CPhysxManager::Create();
	if (nullptr == m_pPhysxManager)
		return E_FAIL;
	m_pDelayedProcessManager = CDelayedProcessManager::Create();
	if (nullptr == m_pDelayedProcessManager)
		return E_FAIL;
	m_pObject_Pool = CObject_Pool::Create();
	if (nullptr == m_pObject_Pool)
		return E_FAIL;
	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;
	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;
	//m_pSoundManager = CSoundManager::Create();
	//if (nullptr == m_pSoundManager)
	//	return E_FAIL;
	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pLightManager = CLight_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pLightManager)
		return E_FAIL;
	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;
	m_pCollisionManager = CCollision_Manager::Create();
	if (nullptr == m_pCollisionManager)
		return E_FAIL;

	m_pVoronoi_Manager = CVoronoi_Manager::Create(*ppDevice, *ppContext);
	if(nullptr == m_pVoronoi_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager)
		return;
	m_pDelayedProcessManager->Tick(fTimeDelta);
	
	m_pInput_Device->Update_InputDev();

	m_pCollisionManager->Tick(fTimeDelta);
	m_pObject_Manager->PriorityTick(fTimeDelta);

	/* 카메라가 갱신해준 뷰, 투영행렬을 이용하자. */
	m_pPipeLine->Update();
	m_pFrustum->Update();

	m_pObject_Manager->Tick(fTimeDelta);
	m_pObject_Manager->LateTick(fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);
	m_pPhysxManager->Simulate(fTimeDelta);
	//m_pSoundManager->Update();
}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}
void CGameInstance::Clear(_uint iLevelIndex)
{
	m_pRenderer->Clear();
	m_pObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);
}
#pragma region GRAPHIC_DEVICE
HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Clear_RTVs(_float4 vClearColor)
{
	return m_pGraphic_Device->Clear_RTVs(vClearColor);
}

HRESULT CGameInstance::Bind_BackBuffer()
{
	return m_pGraphic_Device->Bind_BackBuffer();
}

HRESULT CGameInstance::Bind_RTVs()
{
	return m_pGraphic_Device->Bind_RTVs();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor)
{
	return m_pGraphic_Device->Add_RenderTarget(strRenderTargetTag, iWidth, iHeight, ePixel, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag)
{
	return m_pGraphic_Device->Add_MRT(strMRTTag, strRenderTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool bClear)
{
	return m_pGraphic_Device->Begin_MRT(strMRTTag, pDSV, bClear);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pGraphic_Device->End_MRT();
}

shared_ptr<CRenderTarget> CGameInstance::Find_RenderTarget(const _wstring& strRenderTargetTag)
{
	return m_pGraphic_Device->Find_RenderTarget(strRenderTargetTag);
}

list<shared_ptr<CRenderTarget>>* CGameInstance::Find_MRT(const _wstring& strMRTTag)
{
	return  m_pGraphic_Device->Find_MRT(strMRTTag);
}

HRESULT CGameInstance::Bind_RT_SRV(const _wstring& strRTTag, shared_ptr<CShader> pShader, const _char* pConstantName)
{
	return m_pGraphic_Device->Bind_RT_SRV(strRTTag, pShader, pConstantName);
}

void CGameInstance::Map_Picking_Screen()
{
	m_pGraphic_Device->Map_Picking_Screen();
}

_uint CGameInstance::Pick_Screen(_uint iX, _uint iY)
{
	return m_pGraphic_Device->Pick_Screen(iX, iY);
}

void CGameInstance::Unmap_Picking_Screen()
{
	m_pGraphic_Device->Unmap_Picking_Screen();
}

shared_ptr<CGameObject> CGameInstance::Pick_Object(_uint iX, _uint iY)
{
	m_pGraphic_Device->Map_Picking_Screen();
	_uint iID = m_pGraphic_Device->Pick_Screen(iX, iY);
	m_pGraphic_Device->Unmap_Picking_Screen();

	return CGameObject::Find_Object_With_ID(iID);
}



_float4 CGameInstance::Pick_Position(_uint iX, _uint iY)
{
	m_pGraphic_Device->Map_Picking_Position();
	_float4 vRet = m_pGraphic_Device->Pick_Position(iX, iY);
	m_pGraphic_Device->Unmap_Picking_Position();

	return vRet;
}


HRESULT CGameInstance::Bind_Default_ViewPort()
{
	return m_pGraphic_Device->Bind_Default_ViewPort();
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RTDebug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pGraphic_Device->Ready_Debug(strRenderTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_RTDebug(const _wstring& strMRTTag, shared_ptr< CShader> pShader, const _char* pConstantName, shared_ptr< CVIBuffer_Rect> pVIBuffer)
{
	return m_pGraphic_Device->Render_Debug(strMRTTag, pShader, pConstantName, pVIBuffer);
}
#endif
#pragma endregion //GRAPHIC

#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Change_Level(shared_ptr<class CLevel> pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Change_Level(pNewLevel);
}
_uint CGameInstance::Get_Current_LevelID()
{
	return m_pLevel_Manager->Get_Current_LevelID();
}
#pragma endregion


#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_Prototype(const wstring& strPrototypeTag, shared_ptr<CGameObject> pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);

}
HRESULT CGameInstance::Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Clone(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

shared_ptr<CGameObject> CGameInstance::Clone_Object(const wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Clone_Object(strPrototypeTag, pArg);
}

shared_ptr<CComponent> CGameInstance::Find_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

shared_ptr<CGameObject> CGameInstance::Add_Clone_Return(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Add_Clone_Return(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Push_Object(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<CGameObject> pObject)
{
	return m_pObject_Manager->Push_Object(iLevelIndex, strLayerTag, pObject);
}

shared_ptr<CLayer> CGameInstance::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

shared_ptr<CGameObject> CGameInstance::Get_Object_Prototype(const wstring& strPrototypeTag)
{
	return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}

shared_ptr<CComponent> CGameInstance::Get_Component_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	return m_pComponent_Manager->Find_Prototype(iLevelIndex, strPrototypeTag);
}

//HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<CComponent>  pPrototype)
//{
//	if (nullptr == m_pComponent_Manager)
//		return E_FAIL;
//
//	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
//}
//
//shared_ptr<CComponent> CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
//{
//	if (nullptr == m_pComponent_Manager)
//		return nullptr;
//
//	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
//}

void CGameInstance::Add_Delayed(DELAYED tEvent)
{
	m_pDelayedProcessManager->Add_Delayed(tEvent);
}

void CGameInstance::Push_Dead_Obj(shared_ptr<class CGameObject> _pDeadObj)
{
	m_pDelayedProcessManager->Push_Dead_Obj(_pDeadObj);
}

void CGameInstance::Release_Engine()
{
}

void CGameInstance::Free()
{
	m_pLevel_Manager = { nullptr };
	m_pRenderer = { nullptr };
	m_pObject_Manager = { nullptr };
	m_pObject_Pool = { nullptr };
	m_pComponent_Manager = { nullptr };
	m_pResource_Container = { nullptr };
	m_pDelayedProcessManager = { nullptr };
	m_pPipeLine = { nullptr };
	m_pInput_Device = nullptr;
	m_pLightManager = nullptr;
	m_pCollisionManager = nullptr;
	m_pFrustum = nullptr;
	m_pPhysxManager = nullptr;
	m_pTimer_Manager = nullptr;
	m_pMath_Manager = nullptr;
	m_pGraphic_Device = { nullptr };
}

#pragma endregion

#pragma region PIPELINE

void CGameInstance::Set_Transform(CPipeLine::PIPELINE_STATE eState, _fmatrix Matrix)
{
	m_pPipeLine->Set_Transform(eState, Matrix);
}

void CGameInstance::Set_Transform(CPipeLine::PIPELINE_STATE eState, const _float4x4& Matrix)
{
	m_pPipeLine->Set_Transform(eState, Matrix);
}

_fmatrix CGameInstance::Get_Transform_Matrix(CPipeLine::PIPELINE_STATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

const _float4x4& CGameInstance::Get_Transform_Float4x4(CPipeLine::PIPELINE_STATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

const _float4& CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}

_fvector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}

const _float4& CGameInstance::Get_CamLook_Float4() const
{
	return m_pPipeLine->Get_CamLook_Float4();
}

_fvector CGameInstance::Get_CamLook_Vector() const
{
	return m_pPipeLine->Get_CamLook_Vector();
}

void CGameInstance::Add_Camera(const _wstring& strTag, shared_ptr<CCamera> pCamera)
{
	m_pPipeLine->Add_Camera(strTag, pCamera);
}

shared_ptr<CCamera> CGameInstance::Get_Camera(const _wstring& strTag)
{
	return m_pPipeLine->Get_Camera(strTag);
}

shared_ptr<CCamera> CGameInstance::Get_MainCamera()
{
	return m_pPipeLine->Get_MainCamera();
}

void CGameInstance::Set_MainCamera(const _wstring& strTag)
{
	m_pPipeLine->Set_MainCamera(strTag);
}

void CGameInstance::Remove_Camera(const _wstring& strTag)
{
	m_pPipeLine->Remove_Camera(strTag);
}

void CGameInstance::Clear_Camera()
{
	m_pPipeLine->Clear_Camera();
}

#pragma endregion

#pragma region INPUT
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID) const
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

bool CGameInstance::Get_KeyDown(_ubyte byKeyID) const
{
	return m_pInput_Device->Get_KeyDown(byKeyID);
}

bool CGameInstance::Get_KeyPressing(_ubyte byKeyID) const
{
	return m_pInput_Device->Get_KeyPressing(byKeyID);
}

bool CGameInstance::Get_KeyUp(_ubyte byKeyID) const
{
	return m_pInput_Device->Get_KeyUp(byKeyID);
}


bool CGameInstance::Get_KeyNone(_ubyte byKeyID) const
{
	return m_pInput_Device->Get_KeyNone(byKeyID);
}


bool CGameInstance::Get_MouseDown(MOUSEKEYSTATE byKeyID) const
{
	return m_pInput_Device->Get_MouseDown(byKeyID);
}


bool CGameInstance::Get_MousePressing(MOUSEKEYSTATE byKeyID) const
{
	return m_pInput_Device->Get_MousePressing(byKeyID);
}


bool CGameInstance::Get_MouseUp(MOUSEKEYSTATE byKeyID) const
{
	return m_pInput_Device->Get_MouseUp(byKeyID);
}

bool CGameInstance::Get_MouseNone(MOUSEKEYSTATE byKeyID) const
{
	return m_pInput_Device->Get_MouseNone(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse) const
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}
#pragma endregion //INPUT

#pragma region LIGHT_MANAGER
HRESULT CGameInstance::Add_Light(void* LightDesc)
{
	return m_pLightManager->Add_Light(LightDesc);
}

shared_ptr<class CLight> CGameInstance::Add_Light_Return(void* LightDesc)
{
	return m_pLightManager->Add_Light_Return(LightDesc);
}

HRESULT CGameInstance::Bind_Lights(shared_ptr<class CShader> pShader)
{
	return m_pLightManager->Bind_Lights(pShader);
}

void CGameInstance::LightControl(string strLightTag, _bool bOn)
{
	m_pLightManager->LightControl(strLightTag, bOn);
}

void CGameInstance::LightControl(string strLightTag)
{
	m_pLightManager->LightControl(strLightTag);
}
#pragma endregion //LIGHT

#pragma region COLLISION
void CGameInstance::CollisionClear()
{
	m_pCollisionManager->CollisionClear();
}


void CGameInstance::Add_Collider(COLLIDER_GROUP _colliderType, shared_ptr<class CCollider> pCollider)
{
	m_pCollisionManager->Add_Collider(_colliderType, pCollider);
}

void CGameInstance::CollisionAccessBoth(COLLIDER_GROUP eDst, COLLIDER_GROUP eSrc)
{
	m_pCollisionManager->CollisionAccessBoth(eDst, eSrc);
}

void CGameInstance::CollisionAccess(COLLIDER_GROUP eDst, COLLIDER_GROUP eSrc)
{
	m_pCollisionManager->CollisionAccess(eDst, eSrc);
}

void CGameInstance::CollisionDenyBoth(COLLIDER_GROUP eDst, COLLIDER_GROUP eSrc)
{
	m_pCollisionManager->CollisionDenyBoth(eDst, eSrc);
}

void CGameInstance::CollisionDeny(COLLIDER_GROUP eDst, COLLIDER_GROUP eSrc)
{
	m_pCollisionManager->CollisionDeny(eDst, eSrc);
}

void CGameInstance::CollisionFlagClear()
{
	m_pCollisionManager->CollisionFlagClear();
}
#pragma endregion //COLLISION

#pragma region PHYSX
PxScene* CGameInstance::Get_Scene() const
{
	return m_pPhysxManager->Get_Scene();
}
PxControllerManager* CGameInstance::Get_CCT_Manager() const
{
	return m_pPhysxManager->Get_CCT_Manager();
}
PxPhysics* CGameInstance::Get_Physics() const
{
	return m_pPhysxManager->Get_Physics();
}
PxFoundation* CGameInstance::Get_Foundation() const
{
	return m_pPhysxManager->Get_Foundation();
}
HRESULT CGameInstance::Add_Shape(const wstring& strTag, const vector<_float3>& vecPosition, const vector<_uint>& vecIndicies)
{
	return m_pPhysxManager->Add_Shape(strTag, vecPosition, vecIndicies);
}
PxShape* CGameInstance::Get_Shape(const wstring& strTag)
{
	return m_pPhysxManager->Get_Shape(strTag);
}
void CGameInstance::Simulate_Physx(_bool bSim)
{
	m_pPhysxManager->Simulate_Physx(bSim);
}
bool CGameInstance::PxSweep(PxTransform& StartTransform, PxVec3& vDirection, _float fDistance, _float fRadius, PxSweepBuffer& HitOut)
{
	return m_pPhysxManager->Sweep(StartTransform, vDirection, fDistance, fRadius, HitOut);
}
//PxCudaContextManager* CGameInstance::Get_CudaContextManager()
//{
//	return m_pPhysxManager->Get_CudaContextManager();
//}
#pragma endregion



#pragma region OBJECT_POOL

HRESULT CGameInstance::Pool_Object(const wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Pool->Pool_Object(strPrototypeTag, pArg);
}

shared_ptr<CGameObject> CGameInstance::Push_Object_From_Pool(const wstring& strPrototypeTag, _uint iLevelIndex, const wstring& strLayerTag, void* pArg, _float4x4* TransformMat)
{
	return m_pObject_Pool->Push_Object_From_Pool(strPrototypeTag, iLevelIndex, strLayerTag, pArg, TransformMat);
}

shared_ptr<CGameObject> CGameInstance::Get_Object_From_Pool(const wstring& strPrototypeTag, void* pArg, _float4x4* TransformMat)
{
	return m_pObject_Pool->Get_Object_From_Pool(strPrototypeTag, pArg, TransformMat);
}

void CGameInstance::Clear_Pool()
{
	m_pObject_Pool->Clear_Pool();
}

void CGameInstance::Recall_Object_To_Pool()
{
	m_pObject_Pool->Recall_Object();
}

void CGameInstance::Recall_Object_To_Pool(const wstring& strPrototypeTag)
{
	m_pObject_Pool->Recall_Object(strPrototypeTag);
}

void CGameInstance::Clear_Pool(_wstring strTag)
{
	m_pObject_Pool->Clear_Pool(strTag);
}
#pragma endregion //POOL

#pragma region RENDERER
HRESULT CGameInstance::Add_RenderObjects(CRenderer::RENDERGROUP eRenderGroup, shared_ptr<class CGameObject> pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderObjects(eRenderGroup, pRenderObject);
}

HRESULT CGameInstance::Render_Priority()
{
	return m_pRenderer->Render_Priority();
}

HRESULT CGameInstance::Render_NonBlend()
{
	return m_pRenderer->Render_NonBlend();
}

HRESULT CGameInstance::Render_Blend()
{
	return m_pRenderer->Render_Blend();
}

HRESULT CGameInstance::Render_UI()
{
	return m_pRenderer->Render_UI();
}

_float4x4* CGameInstance::Get_ScreenQuadWorld()
{
	return m_pRenderer->Get_ScreenQuadWorld();
}

_float4x4* CGameInstance::Get_ScreenQuadProjection()
{
	return m_pRenderer->Get_ScreenQuadProjection();
}

void CGameInstance::Blur_On(const _float2& vCenter, const _float& fStrength)
{
	m_pRenderer->Blur_On(vCenter, fStrength);
}

void CGameInstance::Dissolve_On(shared_ptr<CTexture> pNoiseTexture, const _float& fDissolve, const _float& fRange)
{
	m_pRenderer->Dissolve_On(pNoiseTexture, fDissolve, fRange);
}


#pragma region COMPONENT_MANAGER
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<CComponent> pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

shared_ptr<CComponent> CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}
#pragma endregion

#pragma region RESOURCE_CONTAINER

shared_ptr<class CTexture> CGameInstance::Auto_Search_Texture(const wstring& strTextureTag)
{
	if (m_pResource_Container == nullptr)
		return nullptr;

	return m_pResource_Container->SearchMaterial(strTextureTag);
}

#pragma endregion

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}
HRESULT CGameInstance::Add_Timer(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}
void CGameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}
void CGameInstance::Set_TimeMultiplier(_float _fTimeMultiplier)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Set_TimeMultiplier(_fTimeMultiplier);
}
_float CGameInstance::Get_TimeMultiplier()
{
	if (nullptr == m_pTimer_Manager)
		assert(false);

	return m_pTimer_Manager->Get_TimeMultiplier();
}
#pragma endregion

#pragma region SOUND_MANAGER
//void CGameInstance::RegisterToBGM(shared_ptr<CSound> _pSound)
//{
//	m_pSoundManager->RegisterToBGM(_pSound);
//}
//
//void CGameInstance::Set_BGM_Tag(const wstring& strTag)
//{
//	m_pSoundManager->Set_BGM_Tag(strTag);
//}
//
//wstring CGameInstance::Get_BGM_Tag() const
//{
//	return m_pSoundManager->Get_BGM_Tag();
//}
//
//void CGameInstance::Stop_BGM()
//{
//	m_pSoundManager->Stop_BGM();
//}
//
//void CGameInstance::Start_BGM()
//{
//	m_pSoundManager->Start_BGM();
//}
//
//bool CGameInstance::IsBGMExist()
//{
//	return m_pSoundManager->IsBGMExist();
//}
//
//bool CGameInstance::IsBGMPlaying()
//{
//	return m_pSoundManager->IsBGMPlaying();
//}
//
//HRESULT CGameInstance::Add_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, const wstring& strSoundPath)
//{
//	return m_pSoundManager->Add_Sound(eChannel, strSoundTag, strSoundPath);
//}
//
//HRESULT CGameInstance::Add_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, const wstring& strSoundPath)
//{
//	return m_pSoundManager->Add_Sound_3D(eChannel, strSoundTag, strSoundPath);
//}
//
//void CGameInstance::Set_Master_Volume(float fVolume)
//{
//	m_pSoundManager->Set_Master_Volume(fVolume);
//}
//
//void CGameInstance::Set_Channel_Volume(SOUND_CHANNEL eChannel, float fVolume)
//{
//	m_pSoundManager->Set_Channel_Volume(eChannel, fVolume);
//}
//
//void CGameInstance::Play_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bLoop)
//{
//	m_pSoundManager->Play_Sound(eChannel, strSoundTag, bLoop);
//}
//
//void CGameInstance::Play_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, _fvector vPos, _fvector vVel, bool bLoop)
//{
//	m_pSoundManager->Play_Sound_3D(eChannel, strSoundTag, vPos, vVel, bLoop);
//}
//
//void CGameInstance::Pause_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bPausing)
//{
//	m_pSoundManager->Pause_Sound(eChannel, strSoundTag, bPausing);
//}
//
//void CGameInstance::Pause_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bPausing)
//{
//	m_pSoundManager->Pause_Sound_3D(eChannel, strSoundTag, bPausing);
//}
//
//void CGameInstance::Stop_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag)
//{
//	m_pSoundManager->Stop_Sound(eChannel, strSoundTag);
//}
//
//void CGameInstance::Stop_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag)
//{
//	m_pSoundManager->Stop_Sound_3D(eChannel, strSoundTag);
//}
//
//void CGameInstance::Set_Sound_Volume(SOUND_CHANNEL eChannel, const wstring& strSoundTag, float fVolume)
//{
//	m_pSoundManager->Set_Sound_Volume(eChannel, strSoundTag, fVolume);
//}
//
//void CGameInstance::Set_Master_Speed(float fSpeed)
//{
//	m_pSoundManager->Set_Master_Speed(fSpeed);
//}
//
//void CGameInstance::Set_Channel_Speed(SOUND_CHANNEL eChannel, float fSpeed)
//{
//	m_pSoundManager->Set_Channel_Speed(eChannel, fSpeed);
//}
//
//void CGameInstance::Set_Sound_Speed(SOUND_CHANNEL eChannel, const wstring& strSoundTag, float fSpeed)
//{
//	m_pSoundManager->Set_Sound_Speed(eChannel, strSoundTag, fSpeed);
//}
//
//void CGameInstance::Set_BGM_Position(_float fPosition)
//{
//	m_pSoundManager->Set_BGM_Position(fPosition);
//}
//
//shared_ptr<CSound> CGameInstance::Find_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag)
//{
//	return m_pSoundManager->Find_Sound(eChannel, strSoundTag);
//}
//
//shared_ptr<C3D_Sound> CGameInstance::Find_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag)
//{
//	return m_pSoundManager->Find_Sound_3D(eChannel, strSoundTag);
//}
//
//void CGameInstance::Set_Listener(shared_ptr<class CGameObject> pListener)
//{
//	m_pSoundManager->Set_Listener(pListener);
//}

#pragma endregion

#pragma region MATH_MANAGER

_float2 CGameInstance::Lissajous_Curve(IN _float _fTimeDelta, IN OUT _float& _fLissajousTime, IN _float _fWitth, IN _float _fHeight, IN _float _fLagrangianX, IN _float _fLagrangianY, IN _float _fPhaseDelta, IN _float _fLissajousSpeed)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Lissajous_Curve(_fTimeDelta, _fLissajousTime, _fWitth, _fHeight, _fLagrangianX, _fLagrangianY, _fPhaseDelta, _fLissajousSpeed);
}

_float3 CGameInstance::Vector3x3_BezierCurve(IN _float3 _vStart, IN _float3 _vControl, IN _float3 _vEnd, IN OUT _float& _fTimeControl, IN _float _fTimeDelta, IN _float _fSpeed)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Vector3x3_BezierCurve(_vStart, _vControl, _vEnd, _fTimeControl, _fTimeDelta, _fSpeed);
}

_float CGameInstance::Random_Float(_float _fMin, _float _fMax)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Float(_fMin, _fMax);
}

_float2 CGameInstance::Random_Float2(_float2 _fMin, _float2 _fMax)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Float2(_fMin, _fMax);
}

_float3 CGameInstance::Random_Float3(_float3 _fMin, _float3 _fMax)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Float3(_fMin, _fMax);
}

_int CGameInstance::Random_Int(_int _iMin, _int _iMax)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Int(_iMin, _iMax);
}

_vector CGameInstance::XMQuaternionRotationBetweenVectors(_vector start, _vector end)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->XMQuaternionRotationBetweenVectors(start, end);
}

void CGameInstance::QuaternionToEulerAngles(const XMVECTOR& quaternion, float& yaw, float& pitch, float& roll)
{
	m_pMath_Manager->QuaternionToEulerAngles(quaternion, yaw, pitch, roll);
}

_int CGameInstance::Random_Binary()
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Binary();
}

_bool CGameInstance::Random_Bool()
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Bool();
}

PxVec3 CGameInstance::Add_PxVec3toXMVECTOR(PxVec3 _Pxvec3, _vector _Xmvec)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Add_PxVec3toXMVECTOR(_Pxvec3, _Xmvec);
}

#pragma endregion

#pragma region FONT_MANAGER

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor, _float rotation, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, rotation, fScale);
}

HRESULT CGameInstance::Render_Font(const _wstring& strFontTag, const _wstring& strText, _fvector vPosition, _fvector vColor, _float rotation, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, rotation, fScale);
}
#pragma endregion

#pragma region FRUSTUM
void CGameInstance::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix)
{
	m_pFrustum->Transform_ToLocalSpace(WorldMatrix);
}

_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRange)
{
	return m_pFrustum->isIn_WorldSpace(vWorldPos,fRange);
}

_bool CGameInstance::isIn_Frustum_LocalSpace(_fvector vLocalPos, _float fRange)
{
	return m_pFrustum->isIn_LocalSpace(vLocalPos, fRange);
}

#pragma endregion

void CGameInstance::Voronoi_Debug_Draw()
{
	m_pVoronoi_Manager->Debug_Render();
}

void CGameInstance::Voronoi_CellToMeshData(hwiVoro::CHV_Cell_Base& cell, shared_ptr<MESH_DATA>& pMeshData, _float3 vOffset, const string& szName, _uint pid, const _float3& vScale, MESH_VTX_INFO& Info)
{
	m_pVoronoi_Manager->CellToMeshData(cell, pMeshData, vOffset, szName, pid, vScale, Info);
}
