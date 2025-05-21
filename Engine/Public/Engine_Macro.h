#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define DEBUG_ASSERT assert(false)
#define BEGIN(NAMESPACE) namespace NAMESPACE { 
#define END }


#define MSG_BOX(message)								\
::MessageBox(nullptr, message, L"System Message", MB_OK)

#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static shared_ptr<ClassName> Get_Instance();		\
	static void		  Destroy_Instance();				\
private:												\
	static shared_ptr<ClassName> m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
shared_ptr<ClassName> ClassName::m_pInstance = nullptr;			\
shared_ptr<ClassName> ClassName::Get_Instance()					\
{														\
	struct MakeSharedEnabler : public ClassName {\
		MakeSharedEnabler() : ClassName() { }\
	};													\
	if (nullptr == m_pInstance)							\
		m_pInstance = make_shared<MakeSharedEnabler>();\
	return m_pInstance;									\
}														\
void ClassName::Destroy_Instance()						\
{														\
	if(nullptr != m_pInstance)							\
	{													\
														\
		m_pInstance = nullptr;							\
	}													\
}

#define MAKE_SHARED_ENABLER(ClassName)					\
	struct MakeSharedEnabler : public ClassName {\
		MakeSharedEnabler() : ClassName() { }\
	};

#define GET_SINGLE(type)	(type::Get_Instance())

#define GAMEINSTANCE	GET_SINGLE(CGameInstance)
#define PLAYERMANAGER	GET_SINGLE(CPlayer_Manager)
#define GAMEMANAGER		GET_SINGLE(CGame_Manager)
#define IMGUIMANAGER	GET_SINGLE(CImgui_Manager)

#define D3DTS_VIEW CPipeLine::PIPELINE_STATE::TS_VIEW
#define D3DTS_PROJ CPipeLine::PIPELINE_STATE::TS_PROJ

#define GET_TRANSFORM_FLOAT4X4(eState) m_pGameInstance->Get_Transform_Float4x4(eState)
#define GET_TRANSFORM_MATRIX(eState) m_pGameInstance->Get_Transform_Matrix(eState)

#define COLLIDER_GROUP CCollision_Manager::COLLIDER_TYPE

// 선언과 동시에 초기화 불가능, 먼저 선언된 전역변수와 자료형, 변수명까지 일치하는 상태에서만 사용 가능