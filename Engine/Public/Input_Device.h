
#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CInput_Device
{
private:
	CInput_Device(void);
public:
	virtual ~CInput_Device(void);

public:
	_byte	Get_DIKeyState(_ubyte byKeyID)	const {
		return m_byKeyState[byKeyID];
	}

	bool	Get_KeyDown(_ubyte byKeyID) const
	{
		return m_KeyStates[byKeyID].eKeyState == KEY_STATE::DOWN;
	}
	bool	Get_KeyPressing(_ubyte byKeyID) const
	{
		return m_KeyStates[byKeyID].eKeyState == KEY_STATE::PRESSING;
	}
	bool	Get_KeyUp(_ubyte byKeyID) const
	{
		return m_KeyStates[byKeyID].eKeyState == KEY_STATE::UP;
	}
	bool	Get_KeyNone(_ubyte byKeyID) const
	{
		return m_KeyStates[byKeyID].eKeyState == KEY_STATE::NONE;
	}
	bool	Get_MouseDown(MOUSEKEYSTATE byKeyID) const
	{
		return m_MouseState[byKeyID].eKeyState == KEY_STATE::DOWN;
	}
	bool	Get_MousePressing(MOUSEKEYSTATE byKeyID) const
	{
		return m_MouseState[byKeyID].eKeyState == KEY_STATE::PRESSING;
	}
	bool	Get_MouseUp(MOUSEKEYSTATE byKeyID) const
	{
		return m_MouseState[byKeyID].eKeyState == KEY_STATE::UP;
	}
	bool	Get_MouseNone(MOUSEKEYSTATE byKeyID) const
	{
		return m_MouseState[byKeyID].eKeyState == KEY_STATE::NONE;
	}


	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) const {
		return m_tMouseState.rgbButtons[eMouse];
	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

private:
	_byte					m_byKeyState[256] = {};		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			m_tMouseState = {};

private:
	KEY	m_KeyStates[256] = {};
	KEY m_MouseState[4] = {};

public:
	static shared_ptr<CInput_Device> Create(HINSTANCE hInst, HWND hWnd);
	void	Free(void);

};
END

