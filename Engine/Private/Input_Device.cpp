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

	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// Ű���� ��ü ����
	if (FAILED((m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
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

