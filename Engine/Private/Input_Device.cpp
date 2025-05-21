#include "..\Public\Input_Device.h"

Engine::CInput_Device::CInput_Device(void)
{

}

CInput_Device::~CInput_Device(void)
{
	Free();
}

HRESULT Engine::CInput_Device::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED((m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();


	return S_OK;
}

void Engine::CInput_Device::Update_InputDev(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
	for (int i = 0; i < 256; ++i)
	{
		if (m_byKeyState[i] & 0x80)
		{
			if (m_KeyStates[i].bPrevPush)
				m_KeyStates[i].eKeyState = KEY_STATE::PRESSING;
			else
				m_KeyStates[i].eKeyState = KEY_STATE::DOWN;

			m_KeyStates[i].bPrevPush = true;

		}
		else
		{
			if (m_KeyStates[i].bPrevPush)
				m_KeyStates[i].eKeyState = KEY_STATE::UP;
			else
				m_KeyStates[i].eKeyState = KEY_STATE::NONE;

			m_KeyStates[i].bPrevPush = false;
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		if (m_tMouseState.rgbButtons[i] & 0x80)
		{
			if (m_MouseState[i].bPrevPush)
				m_MouseState[i].eKeyState = KEY_STATE::PRESSING;
			else
				m_MouseState[i].eKeyState = KEY_STATE::DOWN;

			m_MouseState[i].bPrevPush = true;

		}
		else
		{
			if (m_MouseState[i].bPrevPush)
				m_MouseState[i].eKeyState = KEY_STATE::UP;
			else
				m_MouseState[i].eKeyState = KEY_STATE::NONE;

			m_MouseState[i].bPrevPush = false;
		}
	}
}

shared_ptr<CInput_Device> CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	struct MakeSharedEnabler : public CInput_Device
	{
		MakeSharedEnabler() : CInput_Device() {};
	};

	shared_ptr<CInput_Device> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Ready_InputDev(hInst, hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CInput_Device"));
		assert(false);
	}

	return pInstance;
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

