
//CGamePad.cpp
//Created May 2015

#include "GamePad.h"
#include <string>
#include "InputManager.h"

CGamePad::~CGamePad()
{
}
bool CGamePad::CheckConnection()
{
	int nControllerId = -1;

	// Check each port for a connection until one is found
	for (DWORD i = 0; i < XUSER_MAX_COUNT && nControllerId == -1; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) == ERROR_SUCCESS)
			nControllerId = i;
	}

	nPortID = nControllerId;

	return nControllerId != -1;
}
//Returns false if the controller disconnected.
//bool CGamePad::Refresh()
//{
//	if (nPortID == -1)
//		CheckConnection();
//
//	// If the controller is connected...
//	if (nPortID != -1)
//	{
//		// Store previous state
//		prevState = curState;
//		XInputGetState(nPortID, &curState);
//		btPrevLTrigger = btCurLTrigger;
//		btPrevRTrigger = btCurRTrigger;
//
//	
//		// Check state and check for disconnection
//		ZeroMemory(&curState, sizeof(XINPUT_STATE));
//		if (XInputGetState(nPortID, &curState) != ERROR_SUCCESS)
//		{
//			nPortID = -1;
//			return false;
//		}
//		btCurLTrigger = curState.Gamepad.bLeftTrigger;
//		btCurRTrigger = curState.Gamepad.bRightTrigger;
//
//		bool triggers = false;
//		//TESTING CODE
//		if (btCurLTrigger >= 204)
//		{
//			
//			printf("Left Trigger Pressed");
//			triggers = true;
//		}
//		if (btCurRTrigger >= 204)
//		{
//
//			printf(" Right Trigger Pressed");
//			triggers = true;
//		}
//		if (triggers)
//			printf("\n");
//		//TESTING CODE
//		if (wndTargWindow != NULL)
//			{
//			for (auto button : m_buttons)
//			{
//				// If button is pushed
//				if ((curState.Gamepad.wButtons & button.first) != 0)
//				{ 
//					// Get key mapping or use XINPUT_GAMEPAD_* value if no mapping exists
//					WORD wMapping = (m_keyMap.find(button.first) != m_keyMap.end() ?
//						m_keyMap.find(button.first)->second : button.first);
//
//					// Send keyboard event
//					SendMessage(wndTargWindow, WM_KEYDOWN, wMapping,
//						((prevState.Gamepad.wButtons & button.first) == 0 ? 0 << 30 : 1 << 30));
//				}
//
//				// Checking for button release events, will cause the state
//				// packet number to be incremented
//				if (prevState.dwPacketNumber < curState.dwPacketNumber)
//				{
//					// If the button was pressed but is no longer pressed
//					if ((curState.Gamepad.wButtons & button.first) == 0
//						&& (prevState.Gamepad.wButtons & button.first) != 0)
//					{
//						// Get key mapping if one exists
//						WORD wMapping = (m_keyMap.find(button.first) != m_keyMap.end() ?
//							m_keyMap.find(button.first)->second : button.first);
//
//						// Send keyboard event
//						SendMessage(wndTargWindow, WM_KEYUP, wMapping, 0);
//					}
//				}
//			}
//			}
//	}
//	return true;
//}
//void CGamePad::SetButtons()
//{
//	
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_A, "A"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_B, "B"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_X, "X"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_Y, "Y"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_DPAD_LEFT, "Left"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_DPAD_RIGHT, "Right"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_DPAD_UP, "Up"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_DPAD_DOWN, "Down"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_LEFT_SHOULDER, "LB"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_RIGHT_SHOULDER, "RB"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_BACK, "Back"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_START, "Start"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_LEFT_THUMB, "LS"));
//	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_RIGHT_THUMB, "RS"));
//	
//		
//}

void CGamePad::AddKeyMapping(unsigned short button, int key)
{
	m_keyMap.erase(button);
	m_keyMap.insert(std::unordered_map<unsigned short, int>::value_type(button, key));
}

void CGamePad::RemoveKeyMapping(int key)
{
	for (auto it = m_keyMap.begin(); it != m_keyMap.end(); ++it)
		if (it->second == key)
		{
		m_keyMap.erase(it);
		break;
		}
}

void CGamePad::RemoveKeyMappingByButton(unsigned short button)
{
	m_keyMap.erase(button);
}

void CGamePad::ClearKeyMapping()
{
	m_keyMap.clear();
}

// Returns false if the controller has been disconnected
bool CGamePad::Refresh()
{
	// Try to establish a connection with the controller if none was connected last time
	if (nPortID == -1)
		CheckConnection();

	// If the controller is connected...
	if (nPortID != -1)
	{
		// Store previous state
		prevState = curState;

		prevLeftStickX = leftStickX;
		prevLeftStickY = leftStickY;
		prevRightStickX = rightStickX;
		prevRightStickY = rightStickY;
		prevLeftTrigger = leftTrigger;
		prevRightTrigger = rightTrigger;


	
		// Check state and check for disconnection
		ZeroMemory(&curState, sizeof(XINPUT_STATE));
		if (XInputGetState(nPortID, &curState) != ERROR_SUCCESS)
		{
			nPortID = -1;
			return false;
		}

		// Calculate deadzone-normalized percentages of movement for the
		// analog sticks and triggers

		float normLX = max(-1, (float)curState.Gamepad.sThumbLX / 32767);
		float normLY = max(-1, (float)curState.Gamepad.sThumbLY / 32767);

		leftStickX = (abs(normLX) < deadzoneX ? 0 : (fabsf(normLX) - deadzoneX) * (normLX / fabsf(normLX)));
		leftStickY = (abs(normLY) < deadzoneY ? 0 : (fabsf(normLY) - deadzoneY) * (normLY / fabsf(normLY)));

		if (deadzoneX > 0) leftStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) leftStickY *= 1 / (1 - deadzoneY);

		float normRX = max(-1, (float)curState.Gamepad.sThumbRX / 32767);
		float normRY = max(-1, (float)curState.Gamepad.sThumbRY / 32767);

		rightStickX = (abs(normRX) < deadzoneX ? 0 : (fabsf(normRX) - deadzoneX) * (normRX / fabsf(normRX)));
		rightStickY = (abs(normRY) < deadzoneY ? 0 : (fabsf(normRY) - deadzoneY) * (normRY / fabsf(normRY)));

		if (deadzoneX > 0) rightStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) rightStickY *= 1 / (1 - deadzoneY);

		leftTrigger = (float)curState.Gamepad.bLeftTrigger / 255;
		rightTrigger = (float)curState.Gamepad.bRightTrigger / 255;

		// Dispatch keyboard events if desired
		if (wndTargWindow != NULL)
		{
			for (auto button : m_buttons)
			{
				// If button is pushed
				if ((curState.Gamepad.wButtons & button.first) != 0)
				{
					// Get key mapping or use XINPUT_GAMEPAD_* value if no mapping exists
					WORD mapping = (m_keyMap.find(button.first) != m_keyMap.end() ?
						m_keyMap.find(button.first)->second : button.first);

					// Get current time and last WM_KEYDOWN message for repeat interval check
					DWORD now = GetTickCount();
					DWORD last = (lastPress.find(button.first) != lastPress.end() ?
						lastPress.find(button.first)->second : 0);

					// Find desired repeat interval for this button
					unsigned int ms = repeatMs.find(button.first)->second;

					// If first press, or repeat interval passed (and repeat interval != 0)
					if ((now - last >= ms && ms > 0)
						|| last == 0
						|| (ms == 0 && (prevState.Gamepad.wButtons & button.first) == 0))
					{
						// Update last press time
						lastPress.erase(button.first);
						lastPress.insert(std::unordered_map<WORD, DWORD>::value_type(button.first, now));

						// Send keyboard event
						SendMessage(wndTargWindow, WM_KEYDOWN, mapping,
							((prevState.Gamepad.wButtons & button.first) == 0 ? 0 << 30 : 1 << 30));
					}
				}

				// Checking for button release events, will cause the state
				// packet number to be incremented
				if (prevState.dwPacketNumber < curState.dwPacketNumber)
				{
					// If the button was pressed but is no longer pressed
					if ((curState.Gamepad.wButtons & button.first) == 0
						&& (prevState.Gamepad.wButtons & button.first) != 0)
					{
						// Get key mapping if one exists
						WORD mapping = (m_keyMap.find(button.first) != m_keyMap.end() ?
							m_keyMap.find(button.first)->second : button.first);

						// Remove last press time
						lastPress.erase(button.first);

						// Send keyboard event
						SendMessage(wndTargWindow, WM_KEYUP, mapping, 0);
					}
				}
			}

			// Do keyboard event dispatch processing for analog items
			// (unmapped items won't have events generated for them)
			for (auto item : analogMap)
			{
				WORD mapping = item.second.key;

				switch (item.first) {
				case AnalogButtons::LeftStickLeft:
					sendKeysOnThreshold(AnalogButtons::LeftStickLeft, leftStickX, prevLeftStickX, -item.second.threshold, mapping);
					break;

				case AnalogButtons::LeftStickRight:
					sendKeysOnThreshold(AnalogButtons::LeftStickRight, leftStickX, prevLeftStickX, item.second.threshold, mapping);
					break;

				case AnalogButtons::LeftStickUp:
					sendKeysOnThreshold(AnalogButtons::LeftStickUp, leftStickY, prevLeftStickY, item.second.threshold, mapping);
					break;

				case AnalogButtons::LeftStickDown:
					sendKeysOnThreshold(AnalogButtons::LeftStickDown, leftStickY, prevLeftStickY, -item.second.threshold, mapping);
					break;

				case AnalogButtons::RightStickLeft:
					sendKeysOnThreshold(AnalogButtons::RightStickLeft, rightStickX, prevRightStickX, -item.second.threshold, mapping);
					break;

				case AnalogButtons::RightStickRight:
					sendKeysOnThreshold(AnalogButtons::RightStickRight, rightStickX, prevRightStickX, item.second.threshold, mapping);
					break;

				case AnalogButtons::RightStickUp:
					sendKeysOnThreshold(AnalogButtons::RightStickUp, rightStickY, prevRightStickY, item.second.threshold, mapping);
					break;

				case AnalogButtons::RightStickDown:
					sendKeysOnThreshold(AnalogButtons::RightStickDown, rightStickY, prevRightStickY, -item.second.threshold, mapping);
					break;

				case AnalogButtons::LeftTrigger:
					sendKeysOnThreshold(AnalogButtons::LeftTrigger, leftTrigger, prevLeftTrigger, item.second.threshold, mapping);
					break;

				case AnalogButtons::RightTrigger:
					sendKeysOnThreshold(AnalogButtons::RightTrigger, rightTrigger, prevRightTrigger, item.second.threshold, mapping);
					break;
				}
			}
		}

		return true;
	}
	return false;
}

void CGamePad::SetButtons()
{

	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_A, "A"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_B, "B"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_X, "X"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_Y, "Y"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_DPAD_LEFT, "Left"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_DPAD_RIGHT, "Right"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_DPAD_UP, "Up"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_DPAD_DOWN, "Down"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_LEFT_SHOULDER, "LB"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_RIGHT_SHOULDER, "RB"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_BACK, "Back"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_START, "Start"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_LEFT_THUMB, "LS"));
	m_buttons.insert(std::unordered_map<unsigned short, std::string>::value_type(XINPUT_GAMEPAD_RIGHT_THUMB, "RS"));


}

// Processing of analog item key event dispatch
void CGamePad::sendKeysOnThreshold(AnalogButtons button, float now, float before, float threshold, int key)
{
	// Check whether the item is and was passed the threshold or not
	bool isPressed = (now >= threshold && threshold > 0) || (now <= threshold && threshold < 0);
	bool wasPressed = (before >= threshold && threshold > 0) || (before <= threshold && threshold < 0);
	// If currently pressed
	if (isPressed)
	{
		// Repeat interval calculation
		DWORD now = GetTickCount();
		DWORD last = (analogLastPress.find(button) != analogLastPress.end() ?
			analogLastPress.find(button)->second : 0);

		unsigned int ms = analogRepeatMs.find(button)->second;

		// Send message (uses same logic as digital buttons)
		if ((now - last >= ms && ms > 0) || last == 0 || (ms == 0 && !wasPressed))
		{
			analogLastPress.erase(button);
			analogLastPress.insert(std::unordered_map<AnalogButtons, DWORD>::value_type(button, now));

			if (button == AnalogButtons::RightTrigger)
				SendMessage(wndTargWindow, WM_LBUTTONDOWN, key, (wasPressed ? 1 << 30 : 0 << 30));
			else if (button == AnalogButtons::LeftTrigger)
				SendMessage(wndTargWindow, WM_RBUTTONDOWN, key, (wasPressed ? 1 << 30 : 0 << 30));
			else
				SendMessage(wndTargWindow, WM_KEYDOWN, key, (wasPressed ? 1 << 30 : 0 << 30));
		}
	}

	// Same logic as digital buttons
	if (prevState.dwPacketNumber < curState.dwPacketNumber)
		if (!isPressed && wasPressed)
		{
		analogLastPress.erase(button);

		SendMessage(wndTargWindow, WM_KEYUP, key, 0);
		}
}

bool CGamePad::IsPressed(WORD button)
{
	return (curState.Gamepad.wButtons & button) != 0;
}

void CGamePad::SetDeadzone(float x, float y)
{
	deadzoneX = x;
	deadzoneY = y;
}

void CGamePad::AddAnalogKeyMapping(AnalogButtons button, float threshold, int key)
{
	AnalogMapping a = { threshold, key };

	analogMap.erase(button);
	analogMap.insert(std::make_pair(button, a));
}

void CGamePad::RemoveAnalogKeyMapping(AnalogButtons button)
{
	analogMap.erase(button);
}

void CGamePad::ClearMappings()
{
	m_keyMap.clear();
	analogMap.clear();
}

void CGamePad::SetRepeatIntervalMsAll(unsigned int ms)
{
	repeatMs.clear();

	for (auto button : m_buttons)
		repeatMs.insert(std::unordered_map<WORD, unsigned int>::value_type(button.first, ms));

	analogRepeatMs.clear();

	for (int i = 0; i < AnalogButtons::EndOfButtons; i++)
		analogRepeatMs.insert(std::unordered_map<AnalogButtons, unsigned int>::value_type((AnalogButtons)i, ms));
}

void CGamePad::SetRepeatIntervalMs(WORD button, unsigned int ms)
{
	repeatMs.erase(button);
	repeatMs.insert(std::unordered_map<WORD, unsigned int>::value_type(button, ms));
}

void CGamePad::SetAnalogRepeatIntervalMs(AnalogButtons button, unsigned int ms)
{
	analogRepeatMs.erase(button);
	analogRepeatMs.insert(std::unordered_map<AnalogButtons, unsigned int>::value_type(button, ms));
}
