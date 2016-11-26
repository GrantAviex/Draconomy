
//CGamePad.h
//Created May 2015

#ifndef GAMEPAD_H
#define GAMEPAD_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <unordered_map>
#include <Xinput.h>
#pragma comment(lib, "Xinput9_1_0.lib ")

class CGamePad
{
	std::unordered_map<unsigned short, int> m_keyMap;
	std::unordered_map<unsigned short, std::string> m_buttons;
	 
	HWND wndTargWindow;

	//Player/Gamepad number1
	int nPortID;

	XINPUT_STATE curState;
	XINPUT_STATE prevState;



public:
	// Names for each axis of movement for each analog item
	enum AnalogButtons {
		LeftStickLeft,
		LeftStickRight,
		LeftStickUp,
		LeftStickDown,
		RightStickLeft,
		RightStickRight,
		RightStickUp,
		RightStickDown,
		LeftTrigger,
		RightTrigger,
		EndOfButtons
	};

	// Analog keyboard mappings have a percentage threshold and a target keyboard key
	struct AnalogMapping {
		float threshold;
		int key;
	};
	// The current state of the analog sticks and triggers (movement amount from 0-1)
	float leftStickX;
	float leftStickY;
	float rightStickX;
	float rightStickY;
	float leftTrigger;
	float rightTrigger;

	// Returns true if the specified XINPUT_GAMEPAD_* button is pressed
	bool IsPressed(WORD);

	void SetWindow(HWND _hWnd) { wndTargWindow = _hWnd; }
	void SetButtons();
	//Adds XINPUT_GAMEPAD keys mapped to WM_VK keys.
	void AddKeyMapping(unsigned short, int);

	// The X-axis analog stick deadzone (0-1)
	float deadzoneX;

	// The Y-axis analog stick deadzone (0-1)
	float deadzoneY;

	// Mapping of analog controller items to keys
	std::unordered_map<AnalogButtons, AnalogMapping> analogMap;

	// Repeat rate of generated key events from controller buttons
	std::unordered_map<WORD, unsigned int> repeatMs;

	// Repeat rate of generated key events from analog controller items
	std::unordered_map<AnalogButtons, unsigned int> analogRepeatMs;

	// The GetTickCount() of when each button was last pressed
	std::unordered_map<WORD, DWORD> lastPress;

	// The GetTickCount() of when each analog item was pressed (passed the threshold)
	std::unordered_map<AnalogButtons, DWORD> analogLastPress;

	// The previous state of the analog sticks and triggers
	float prevLeftStickX;
	float prevLeftStickY;
	float prevRightStickX;
	float prevRightStickY;
	float prevLeftTrigger;
	float prevRightTrigger;

	// Internal use only
	void sendKeysOnThreshold(AnalogButtons, float, float, float, int);

	// Enable gamepad support
	CGamePad() : deadzoneX(0.05f), deadzoneY(0.02f), wndTargWindow(NULL) { SetButtons(); SetRepeatIntervalMsAll(0); }

	// Enable gamepad support supplying default X and Y-axis deadzones
	CGamePad(float dzX, float dzY) : deadzoneX(dzX), deadzoneY(dzY), wndTargWindow(NULL) { SetButtons(); SetRepeatIntervalMsAll(0); }

	BYTE btPrevLTrigger, btCurLTrigger, btPrevRTrigger, btCurRTrigger;
	bool Refresh();

	//removes key mapping by XINPUT_GAMEPAD button
	void RemoveKeyMappingByButton(unsigned short);  

	bool CheckConnection();

	//Removes key mapping by WM_VK int
	void RemoveKeyMapping(int);

	//Clears all key mapping
	void ClearKeyMapping();

	// Set the X and Y-axis analog stick deadzones
	void SetDeadzone(float, float);

	// Add a key translation mapping from an analog item moved more than the specified threshold to a virtual key code (VK_*)
	void AddAnalogKeyMapping(AnalogButtons, float, int);

	// Remove a key translation mapping from an analog item
	void RemoveAnalogKeyMapping(AnalogButtons);

	// Remove all digital and analog key translation mappings
	void ClearMappings();

	// Set the global keyboard repeat interval for all buttons and analog items on the controller - overwrites any previous settings
	void SetRepeatIntervalMsAll(unsigned int);

	// Set the keyboard repeat interval for the specified XINPUT_GAMEPAD_* button in milliseconds
	void SetRepeatIntervalMs(WORD, unsigned int);

	// Set the keyboard repeat interval for the specified analog item in milliseconds
	void SetAnalogRepeatIntervalMs(AnalogButtons, unsigned int);

	~CGamePad();
};

#endif
