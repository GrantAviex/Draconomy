#ifndef GAMEPLAYHUD_H
#define GAMEPLAYHUD_H

#include "EventSystem.h"
#include "Renderer.h"
class CRenderGUI;
class CGUIElement;
class CUIFrame;
class CVariantWaveHUD;
class CWinLoseHUD;
class CTowerUpgradeHUD;
class COptionsMenuHUD;
class CPauseMenuHUD;
class CWaveInfoBillboard;
class CPlayerUpgradeHUD;
class CCompanionHUD;

__declspec(align(16))class CGamePlayHUD : public IEventSubscriber
{
public:
	bool m_bDontClose = false;
	CGamePlayHUD();
	~CGamePlayHUD();

	void Update();
	virtual void ReceiveEvent(EventID id, TEventData const* data) override;
	CRenderGUI* GetRenderGUI() { return renderGUI; }

	void ShowWin();
	void ShowLose();

	bool GetExitButtonPressed(bool PauseMenu = false, bool OptionsMenu = false);
	bool GetResumeButtonPressed();
	bool GetSELessButtonPressed();
	bool GetSEMoreButtonPressed();
	bool GetMusicLessButtonPressed();
	bool GetMusicMoreButtonPressed();

	bool PauseMenuOpen();
	bool UpgradeMenuOpen();
	void TogglePauseMenu();
	void ToggleUpgradeMenu();
	bool PlayerUpgradeOpen();
	void UpdatePlayerUpgrade(EventID ID, TEventData const * Data);

	void* operator new(size_t i){ return _mm_malloc(i, 16); }
	void operator delete(void* p){ _mm_free(p); }

private:
	struct vi2
	{
		vi2(){};
		vi2(int _x, int _y) { x = _x; y = _y; }
		int x;
		int y;
	};

	struct FontEffect
	{
		float			fontSize;
		float			maxFontSize;
		float			fontTimer;
		float			fontEffectDuration;
		bool			bFontIncrese;
	};

	struct IconScaleEffect
	{
		float			scale;
		float			maxScale;
		float			scaleTimer;
		float			scaleEffectDuration;
		bool			bScaleIncrese;
	};

	struct TowerIconEffect
	{
		float				ColorTimer;
		float				ColorChangeDuration;
		bool				bColorChanged;
		DirectX::XMFLOAT4	color;
	};

	CRenderGUI*			renderGUI;

	// Health
	CGUIElement*		guiHealthBar;
	CGUIElement*		guiHealthRedBar;
	CGUIElement*		guiHealthRemaning;
	vi2					healthBarPosition;
	wstring				healthString;
	int					health;
	int					maxHealth;


	CGUIElement*		guiHTP;
	// Mana
	//CGUIElement*		guiManaBar;
	//CGUIElement*		guiManaBlueBar;
	//CGUIElement*		guiManaRemaining;
	//vi2					manaBarPosition;
	//wstring				manaString;
	int					mana;
	//int					maxMana;

	// Gems
	CGUIElement*		guiManaGem;
	CGUIElement*		guiManaGemCount;
	vi2					manaGemPosition;
	int					manaGemCount;

	// Candy
	CGUIElement*		guiCandy;
	CGUIElement*		guiCandyCount;
	vi2					candyPosition;
	int					candyCount;

	// Enemies remaining
	CGUIElement*		guiEnemyIcon;
	CGUIElement*		guiEnemyCount;
	vi2					enemyIconPosition;
	int					enemyCount;
	
	//=========
	// Currency
	//==========
	//CGUIElement*		guiCurrencyIcon;
	//CGUIElement*		guiCurrencyCount;
	//vi2					currencyPosition;
	//int					currencyCount;

	//=============
	// Current wave
	//==============
	CGUIElement*		guiWaveIcon;
	CGUIElement*		guiWaveCount;
	vi2					wavePosition;


	//===========
	//Can'tBuildText
	//============
	CGUIElement*		guiStructText;
	vi2					structDisplayTextPosition;
	wstring				structDisplayText;

	//===============
	// Can'tBuildTextBox
	//================
	//CGUIElement*		guiStructTextBox;
	vi2					structTextBoxPosition;

	//===========
	//DisplayText phase
	//============
	CGUIElement*		guiDisplayText;
	vi2					displayTextPosition;
	wstring				displayText;
	float				displayTextTimer;
	bool				displayTextPopped;


	//===============
	// DisplayTextBox phase
	//================
	CUIFrame*			guiDisplayTextFrame;
	//CGUIElement*		guiDisplayTextBox;
	vi2					displayTextBoxPosition;

	//==========================
	// Display Build Phase timer
	//============================
	CUIFrame*			guiDisplayPhaseTimerFrame;
	//CGUIElement*		guiDisplayPhaseTimerBox;
	CGUIElement*		guiDisplayPhaseTimerText;
	vi2					DisplayPhaseTimerPosition;


	//==============================
	// Display Text message blinking
	//===============================
	CUIFrame*			guiDisplayBlinkingTextFrame;
	//CGUIElement*		guiDisplayBlinkingTextBox;
	CGUIElement*		guiDisplayBlinkingText;
	vi2					displayBlinkingTextPosition;
	float				displayTextBlinkingTimer;
	bool				displayTextBlinkingPopped;
	bool				bStartDisplayTextBlinking;

	//Rotate Tower
	//CGUIElement*		guiBackgroundBox3;
	CUIFrame*			guiRotateTextFrame;
	vi2					backgroundBoxPosition3;
	CGUIElement*		guiRotateText;
	CGUIElement*		guiRotateIcon;
	vi2					guiRotateIconPosition;

	//Cancel Indicator
	//CGUIElement*		guiBackgroundBox5;
	CUIFrame*			guiCancelTextFrame;
	vi2					backgroundBoxPosition5;
	CGUIElement*		guiCancelText;

	//Selling Indicator
	//CGUIElement*		guiBackgroundBox4;
	CUIFrame*			guiSellTextFrame;
	vi2					backgroundBoxPosition4;
	CGUIElement*		guiSellText;

	//=============
	//Towers
	// Fisrt Tower
	CGUIElement*		guiFirstTower;
	CGUIElement*		guiFirstTowerText;
	CGUIElement*		guiFirstTowerCostIcon;
	CGUIElement*		guiFirstTowerCostText;
	CGUIElement*		guiFirstTowerLocked;
	CGUIElement*		guiFirstTowerUnlocked;
	CGUIElement*		guiFirstTowerUnlockAnimation;
	vi2					firstTowerPosition;

	// Second Tower
	CGUIElement*		guiSecondTower;
	CGUIElement*		guiSecondTowerText;
	CGUIElement*		guiSecondTowerCostIcon;
	CGUIElement*		guiSecondtTowerCostText;
	CGUIElement*		guiSecondTowerLocked;
	CGUIElement*		guiSecondTowerUnlocked;
	CGUIElement*		guiSecondTowerUnlockAnimation;
	vi2					secondTowerPosition;

	// Third Tower
	CGUIElement*		guiThirdTower;
	CGUIElement*		guiThirdTowerText;
	CGUIElement*		guiThirdTowerCostIcon;
	CGUIElement*		guiThirdTowerCostText;
	CGUIElement*		guiThirdTowerLocked;
	CGUIElement*		guiThirdTowerUnlocked;
	CGUIElement*		guiThirdTowerUnlockAnimation;
	vi2					thirdTowerPosition;

	//Background Box bottom left
	CUIFrame*			guiBottomLeftFrame;
	CGUIElement*		guiBackgroundBox;
	CUIFrame*			guiTopRightFrame;
	//CGUIElement*		guiBackgroundBox2;
	vi2					backgroundBoxPosition;
	vi2					backgroundBoxPosition2;


	//================
	// Candy on Player
	//=================
	CGUIElement*		guiCandyOnPlayer;
	CGUIElement*		guiCandyOnPlayerText;
	vi2					candyOnPlayerPosition;
	int					candyOnPlayerCount;

	//=================
	// Special Charging
	//==================
	CGUIElement*		guiChargingSpecialBar;
	CGUIElement*		guiChargingSpecialBarBackground;
	CGUIElement*		guiChargingSpecialBlueBar;
	CGUIElement*		guiChargingSpecialIcon;
	vi2					chargingSpecialBarPosition;
	float				chargingSpecialFlashTimer;
	bool				bChargingStartFlashing;
	bool				bChargingFlashing;

	//=============
	// Towers built
	//===============
	CGUIElement*		guiTowersBuiltIcon;
	CGUIElement*		guiTowersBuiltText;
	vi2					towersBuiltPosition;
	int					towerCount;

	//==========================
	// Protect the candy message
	//===========================
	CUIFrame*			guiOpeningMessageFrame;
	CGUIElement*		guiOpeningMessageBox;
	CGUIElement*		guiOpeningMessageBoxText;
	vi2					OpeningMessagePosition;
	bool				bShowOpeningMessage;

	bool				bhealthToggle;

	//==========================
	// Crosshair
	//===========================
	CGUIElement*		guiCrosshair;
	CGUIElement*		guiCrosshairRed;
	bool				bShowCrossHair;

	//===============
	// Revive message
	//================
	CGUIElement*		guiReviveInstructionsBox;
	CGUIElement*		guiReviveInstructionsText;
	vi2					reviveInstructionsPosition;
	bool				bShowReviveInstructions;

	//=============
	// Score board
	//===============
	bool				bShowScoreBoard;

	//=============
	// Candy carry slow warning
	//===============
	CGUIElement*		guiSlowedByCandy;
	bool				m_bShowingSlowDebuff;


	// Player Icon
	CGUIElement*		m_BaseHead;
	CGUIElement*		m_GSHead;

	CGUIElement*		m_BossIncoming;
	float				m_BossIncomingTimer;
	// Variant Wave
	CVariantWaveHUD*	VariantWave;

	CWinLoseHUD*		WinLoseHUD;

	CTowerUpgradeHUD*	TowerUpgradeHUD;

	CPauseMenuHUD*		PauseMenuHUD;
	bool				bPauseMenuShowing;

	CPlayerUpgradeHUD* PlayerUpgradeHUD;

	COptionsMenuHUD* OptionsMenuHUD;
	CGUIElement*		guiFullScreen;

	CWaveInfoBillboard*		WaveInfoBillboard;

	CCompanionHUD*			CompanionHUD;

	// Containers
	std::vector<CGUIElement*>					guiContainer;
	std::map<CGUIElement*,FontEffect>			fontEffectContainer;
	std::map<CGUIElement*,TowerIconEffect>		towerEffectContainer;
	std::map<CGUIElement*,IconScaleEffect>		iconEffectContainer;
	std::vector<CUIFrame*>						frameContainer;

	bool bFirstTowerUnlocked = false;
	bool bSecondTowerUnlocked = false;
	bool bThirdTowerUnlocked = false;
	bool bShowFirstTowerUnlocked = false;
	bool bShowSecondTowerUnlocked = false;
	bool bShowThirdTowerUnlocked = false;

	int firstTowerCost;
	int secondTowerCost;
	int thirdTowerCost;

	int previousPhaseTime;

	void UpdateHealthOnHUD();

	//float Layer(int _x) { return _x * 0.01f + 0.01f; }
	void UpdateFontEffect(float _dt);
	void UpdateIconEffect(float _dt);

	void SetScoreText(std::string _display, std::string _name, int _score, CGUIElement * _element);

	static void SendEvent(EventID ID, TEventData & Data);
};

#endif 