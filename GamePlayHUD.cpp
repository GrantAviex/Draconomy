#include "GamePlayHUD.h"
#include "RenderGUI.h"
#include "Game.h"
#include "GlobalDef.h"
#include "Client.h"
#include "Turret.h"
#include "UIFrame.h"
#include "VariantWaveHUD.h"
#include "WinLoseHUD.h"
#include "TowerUpgradeHUD.h"
#include "PauseMenuHUD.h"
#include "AudioSystemWwise.h"
#include "Wwise_IDs.h"
#include "WaveInfoBillboard.h"
#include "PlayerUpgradeHUD.h"
#include "CompanionHUD.h"

CGamePlayHUD::CGamePlayHUD()
{
	float screenRatio = GlobalDef::windowHeight / 1080.f;
	float width = (float)GlobalDef::windowWidth;
	float height = (float)GlobalDef::windowHeight;

	health = 120;
	maxHealth = health;
	mana = 500;//PLACEHOLDER
	//maxMana = mana;
	manaGemCount = 500; //PLACEHOLDER
	candyCount = 15;
	enemyCount = 0;
	//currencyCount = 500;

	displayTextPopped = false;
	displayTextTimer = 2.f;

	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateEnemiesRemaning, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateHealth, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateMana, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateGem, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateCandy, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::DisplayHUDText, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::DisplayCBTText, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateCurrency, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateCurrentWave, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateHUDBuildIcon, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateCandyOnPlayer, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::CursorVisibility, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::ChargingSpecial, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::Keypress, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::CrosshairTexture, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::Revived, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::PlayerDead, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::MostMana, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::MostKills, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::MostTowers, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::MostCandyReturned, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::MostDeaths, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::MostAcurrate, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateTowerCap, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateBuildPhaseTimer, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::SetTowerPriceOnHUD, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::RotateIconHUD, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UnlockedTower, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateMaxHealth, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::BossIncoming, this);



	renderGUI = new CRenderGUI(CGame::GetInstance()->GetRenderer());

	//============
	// Health bar
	//============
	healthBarPosition.x = int(125 * screenRatio);// int(0.05f * width);
	healthBarPosition.y = int(70 * screenRatio);// int(0.05f * height);

	renderGUI->LoadTexture("Bar", L"Assets/Textures/GSHealthBarFrame.dds");
	renderGUI->LoadTexture("HealthRedBar", L"Assets/Textures/GSHealthbar.dds");

	guiHealthBar = new CGUIElement(renderGUI->GetTextureSRV("Bar"), Layer(13), 0, 0, 512, 64, false);
	guiHealthBar->SetScreenPos((float)healthBarPosition.x, (float)healthBarPosition.y);
	//guiHealthBar->SetColor(DirectX::Colors::PaleGoldenrod);
	guiContainer.push_back(guiHealthBar);

	guiHealthRedBar = new CGUIElement(renderGUI->GetTextureSRV("HealthRedBar"), Layer(12), 0, 0, 512, 64, false);
	guiHealthRedBar->SetScreenPos((float)healthBarPosition.x, (float)healthBarPosition.y);
	guiHealthRedBar->SetColor(DirectX::Colors::Red);
	guiContainer.push_back(guiHealthRedBar);

	guiHealthRemaning = new CGUIElement(L"120/120", Layer(10), int(healthBarPosition.x + 160 * screenRatio), int(healthBarPosition.y + 8 * screenRatio), 32, 32, false);
	//guiHealthRemaning->SetColor(DirectX::Colors::Black);
	guiContainer.push_back(guiHealthRemaning);

	renderGUI->LoadTexture("BaseHead", L"Assets/Textures/DragonHeadIconBase.dds");
	renderGUI->LoadTexture("GSHead", L"Assets/Textures/DragonHeadIcon.dds");

	m_BaseHead = new CGUIElement(renderGUI->GetTextureSRV("BaseHead"), Layer(10), 0, 0, 272, 400, false);
	m_BaseHead->SetScreenPos(healthBarPosition.x - 83.f*screenRatio, healthBarPosition.y - 48.f*screenRatio);
	m_BaseHead->SetScale(0.30f);
	guiContainer.push_back(m_BaseHead);

	m_GSHead = new CGUIElement(renderGUI->GetTextureSRV("GSHead"), Layer(10), 0, 0, 272, 400, false);
	m_GSHead->SetScreenPos(healthBarPosition.x - 83.f*screenRatio, healthBarPosition.y - 48.f*screenRatio);
	m_GSHead->SetScale(0.30f);
	guiContainer.push_back(m_GSHead);

	int loadout = CGame::GetInstance()->GetLoadout();

	if (loadout == 1)
		m_GSHead->SetColor(DirectX::Colors::DeepSkyBlue);
	else if (loadout == 2)
		m_GSHead->SetColor(DirectX::Colors::Red);
	else if (loadout == 3)
		m_GSHead->SetColor(DirectX::Colors::LawnGreen);

	//==================================
	// Background for bottom left corner
	//====================================
	backgroundBoxPosition2.x =int( 32 * screenRatio);//int(0.048f * width);
	backgroundBoxPosition2.y = (int)(height - 225 * screenRatio - 64 * screenRatio);// int(0.75f * height);

	renderGUI->LoadTexture("BackgroundBox", L"Assets/Textures/Frame_BlackBG.dds");
	//guiBackgroundBox2 = new CGUIElement(renderGUI->GetTextureSRV("BackgroundBox"), Layer(10), 0, 0, 256, 132);
	//
	RECT rect;
	//rect.left = 0;
	//rect.top = 0;
	//rect.right = 250;
	//rect.bottom = 225;
	//guiBackgroundBox2->SetScreenDisplayRect(rect);
	//guiBackgroundBox2->SetScreenPos((float)backgroundBoxPosition2.x, (float)backgroundBoxPosition2.y);
	//guiBackgroundBox2->SetAlpha(0.85f);
	//guiContainer.push_back(guiBackgroundBox2);


	guiBottomLeftFrame = new CUIFrame(renderGUI);
	guiBottomLeftFrame->SetFrame(L"Assets/Textures/GSFrameRoundFancy.dds",
		48, 48, 240, 175, (float)backgroundBoxPosition2.x - 10 * screenRatio, (float)backgroundBoxPosition2.y + 55 * screenRatio, Layer(10));

	frameContainer.push_back(guiBottomLeftFrame);

	//==========
	// Mana Gem
	//==========
	manaGemPosition.x = int(backgroundBoxPosition2.x + 15 * screenRatio);// int(0.05f * width);
	manaGemPosition.y = int(backgroundBoxPosition2.y + 155 * screenRatio);// int(0.9f * height);

	renderGUI->LoadTexture("ManaGem", L"Assets/Textures/ManaCrystal.dds");

	guiManaGem = new CGUIElement(renderGUI->GetTextureSRV("ManaGem"), Layer(10), 0, 0, 128, 128, false);
	guiManaGem->SetScale(0.5f);
	guiManaGem->SetScreenPos((float)manaGemPosition.x, (float)manaGemPosition.y);
	guiContainer.push_back(guiManaGem);

	guiManaGemCount = new CGUIElement(std::to_wstring(manaGemCount), Layer(10), int(manaGemPosition.x + 70 * screenRatio), int(manaGemPosition.y + 10 * screenRatio), 32, 32, false);
	guiContainer.push_back(guiManaGemCount);

	FontEffect fe;
	fe.fontSize = 1.f;
	fe.maxFontSize = 1.5f;
	fe.fontEffectDuration = 0.f;
	fe.fontTimer = 1000;
	fe.bFontIncrese = false;
	fontEffectContainer[guiManaGemCount] = fe;

	IconScaleEffect ie;
	ie.bScaleIncrese = false;
	ie.scale = 0.5f;
	ie.maxScale = 1.0f;
	ie.scaleEffectDuration = 0.f;
	ie.scaleTimer = 10000;
	iconEffectContainer[guiManaGem] = ie;

	//=======
	// Candy
	//=======
	candyPosition.x = int(backgroundBoxPosition2.x + 15 * screenRatio);// int(0.05f * width);
	candyPosition.y = int(backgroundBoxPosition2.y + 80 * screenRatio);// int(0.82f * height);

	renderGUI->LoadTexture("Candy", L"Assets/Textures/CandyHUD.dds");

	guiCandy = new CGUIElement(renderGUI->GetTextureSRV("Candy"), Layer(10), 0, 0, 64, 64, false);
	guiCandy->SetScreenPos((float)candyPosition.x, (float)candyPosition.y);
	guiContainer.push_back(guiCandy);

	guiCandyCount = new CGUIElement(std::to_wstring(candyCount), Layer(10), int(candyPosition.x + 70 * screenRatio), int(candyPosition.y + 10 * screenRatio), 32, 32, false);
	guiContainer.push_back(guiCandyCount);

	fontEffectContainer[guiCandyCount] = fe;
	ie.scale = 1.0f;
	ie.maxScale = 1.6f;
	iconEffectContainer[guiCandy] = ie;

	//=======
	// Slowed by candy
	//=======
	renderGUI->LoadTexture("CarryingCandy", L"Assets/Textures/CarryingCandy.dds");
	guiSlowedByCandy = new CGUIElement(renderGUI->GetTextureSRV("CarryingCandy"), Layer(10), 0, 0, 512, 512, false, false, 0.5f);
	guiSlowedByCandy->SetScreenPos(width * 0.5f - 128.0f*screenRatio, height * 0.97f - 256.0f*screenRatio);
	guiContainer.push_back(guiSlowedByCandy);

	//================
	// Candy On Player
	//==================
	candyOnPlayerPosition.x = int(backgroundBoxPosition2.x + 10 * screenRatio);// int(0.05f * width);
	candyOnPlayerPosition.y = int(backgroundBoxPosition2.y + 15 * screenRatio);// (0.76f * height);

	renderGUI->LoadTexture("CandyOnPlayer", L"Assets/Textures/Cone.dds");

	guiCandyOnPlayer = new CGUIElement(renderGUI->GetTextureSRV("CandyOnPlayer"), Layer(10), 0, 0, 64, 64, false);
	guiCandyOnPlayer->SetScreenPos((float)candyOnPlayerPosition.x + 10 * screenRatio, (float)candyOnPlayerPosition.y);
	guiContainer.push_back(guiCandyOnPlayer);

	guiCandyOnPlayerText = new CGUIElement(std::to_wstring(candyOnPlayerCount), Layer(10), int(candyOnPlayerPosition.x + 70 * screenRatio), int(candyOnPlayerPosition.y + 10 * screenRatio), 32, 32, false);
	guiContainer.push_back(guiCandyOnPlayerText);

	fontEffectContainer[guiCandyOnPlayerText] = fe;
	iconEffectContainer[guiCandyOnPlayer] = ie;

	//=======================================
	// background for wave and enemy counter
	//=========================================
	backgroundBoxPosition.x = int(width - 256 * screenRatio - 100 * screenRatio);//int(0.80f * width);
	backgroundBoxPosition.y = int(32 * screenRatio);//int(0.05f * height);

	//guiBackgroundBox = new CGUIElement(renderGUI->GetTextureSRV("BackgroundBox"), Layer(10), 0, 0, 256, 132);
	//rect.left = backgroundBoxPosition.x -2;
	//rect.top = backgroundBoxPosition.y -2;
	//rect.right = rect.left + 315;
	//rect.bottom = rect.top + 70;

	//rect.left = 0;
	//rect.top = 0;
	//rect.right = 315;
	//rect.bottom = 70;
	//guiBackgroundBox->SetScreenDisplayRect(rect);
	//guiBackgroundBox->SetScreenPos((float)backgroundBoxPosition.x, (float)backgroundBoxPosition.y);
	//guiBackgroundBox->SetAlpha(0.85f);
	//guiContainer.push_back(guiBackgroundBox);

	//
	guiTopRightFrame = new CUIFrame(renderGUI);
	guiTopRightFrame->SetFrame(L"Assets/Textures/GSFrameRoundFancy2.dds", 24, 24, 315, 80, (float)backgroundBoxPosition.x, (float)backgroundBoxPosition.y - 10 * screenRatio, Layer(10));
	frameContainer.push_back(guiTopRightFrame);

	//========
	//Enemies
	//========
	enemyIconPosition.x = int(backgroundBoxPosition.x + 150 * screenRatio);// int(0.875f * width);
	enemyIconPosition.y = int(backgroundBoxPosition.y - 2 * screenRatio);// int(0.051f * height);
	renderGUI->LoadTexture("EnemyIcon", L"Assets/Textures/SkullIcon.dds");
	guiEnemyIcon = new CGUIElement(renderGUI->GetTextureSRV("EnemyIcon"), Layer(10), 0, 0, 128, 128, false, false, 0.5f);
	guiEnemyIcon->SetScreenPos(float(enemyIconPosition.x), float(enemyIconPosition.y));
	guiContainer.push_back(guiEnemyIcon);
	guiEnemyCount = new CGUIElement(std::to_wstring(enemyCount), Layer(10), int(enemyIconPosition.x + 70 * screenRatio), int(enemyIconPosition.y + 10 * screenRatio), 32, 32, true);
	guiContainer.push_back(guiEnemyCount);
	fe.maxFontSize = 1.2f;
	fontEffectContainer[guiEnemyCount] = fe;

	//==========
	//Crosshair
	//============
	float crosshairscale = 0.333f;
	renderGUI->LoadTexture("Crosshair", L"Assets/Textures/Crosshair.dds");
	guiCrosshair = new CGUIElement(renderGUI->GetTextureSRV("Crosshair"), Layer(20), 0, 0, 256, 256, false, false, crosshairscale);
	guiCrosshair->SetScreenPos((float)int(0.50f * width) - 128 * screenRatio * crosshairscale, (float)int(0.5f * height) - 128 * crosshairscale*screenRatio);
	guiContainer.push_back(guiCrosshair);
	//Crosshair red
	renderGUI->LoadTexture("CrosshairRed", L"Assets/Textures/CrosshairRed.dds");
	guiCrosshairRed = new CGUIElement(renderGUI->GetTextureSRV("CrosshairRed"), Layer(20), 0, 0, 256, 256, false, false, crosshairscale);
	guiCrosshairRed->SetScreenPos((float)int(0.50f * width) - 128 * crosshairscale*screenRatio, (float)int(0.5f * height) - 128 * crosshairscale*screenRatio);
	guiContainer.push_back(guiCrosshairRed);

	bShowCrossHair = true;

	//=============
	// Current Wave
	//==============
	wavePosition.x = int(backgroundBoxPosition.x + 10 * screenRatio);// int(0.80f * width);
	wavePosition.y = int(backgroundBoxPosition.y + 2 * screenRatio);// int(0.051f * height);

	renderGUI->LoadTexture("WaveIcon", L"Assets/Textures/WaveIcon.dds");

	guiWaveIcon = new CGUIElement(renderGUI->GetTextureSRV("WaveIcon"), Layer(10), 0, 0, 128, 128, false, false, 0.5f);
	guiWaveIcon->SetScreenPos((float)wavePosition.x, (float)wavePosition.y);
	guiContainer.push_back(guiWaveIcon);

	renderGUI->LoadTexture("Crosshair", L"Assets/Textures/Crosshair.dds");
	float scale = 0.333f;
	auto crosshair = new CGUIElement(renderGUI->GetTextureSRV("Crosshair"), Layer(20), 0, 0, 256, 256, false, false, scale);
	crosshair->SetScreenPos((float)int(0.50f * width) - 128 * scale*screenRatio, (float)int(0.5f * height) - 128 * scale*screenRatio);
	guiContainer.push_back(crosshair);

	guiWaveCount = new CGUIElement(L"0", Layer(10), int(wavePosition.x + 70 * screenRatio), int(wavePosition.y + 10 * screenRatio), 32, 32, false);
	guiContainer.push_back(guiWaveCount);

	fe.maxFontSize = 1.2f;
	fontEffectContainer[guiWaveCount] = fe;

	//===========
	//DisplayText
	//============
	displayTextPosition.x = int(0.50f * (width - 512 * screenRatio));
	displayTextPosition.y = int(0.15f * height);

	renderGUI->LoadTexture("DisplayTextBox", L"Assets/Textures/DisplayTextBox.dds");

	guiDisplayText = new CGUIElement(L"Display Text", Layer(14), int(displayTextPosition.x + 10 * screenRatio), int(displayTextPosition.y + 10 * screenRatio), 32, 32, false);
	guiContainer.push_back(guiDisplayText);

	//guiDisplayTextBox = new CGUIElement(renderGUI->GetTextureSRV("DisplayTextBox"), Layer(15), 0, 0, 512, 512, false);
	//guiDisplayTextBox->SetScreenPos((float)displayTextPosition.x, (float)displayTextPosition.y);
	//guiContainer.push_back(guiDisplayTextBox);


	guiDisplayTextFrame = new CUIFrame(renderGUI);
	guiDisplayTextFrame->SetFrame(L"Assets/Textures/GSFrameRoundFancy2.dds", 24, 24, 512, 64, (float)displayTextPosition.x, (float)displayTextPosition.y, Layer(14));
	frameContainer.push_back(guiDisplayTextFrame);
	guiDisplayTextFrame->SetVisibility(false);

	//====================
	//DisplayText blinking
	//======================
	displayBlinkingTextPosition.x = int(0.50f * (width - 512 * screenRatio));
	displayBlinkingTextPosition.y = int(0.23f * height);

	//guiDisplayBlinkingTextBox = new CGUIElement(renderGUI->GetTextureSRV("DisplayTextBox"), Layer(15), 0, 0, 512, 512, false);
	//guiDisplayBlinkingTextBox->SetScreenPos((float)displayBlinkingTextPosition.x, (float)displayBlinkingTextPosition.y);
	//guiDisplayBlinkingTextBox->SetAlpha(0.65f);
	//guiContainer.push_back(guiDisplayBlinkingTextBox);

	guiDisplayBlinkingTextFrame = new CUIFrame(renderGUI);
	guiDisplayBlinkingTextFrame->SetFrame(L"Assets/Textures/GSFrameRoundFancy2.dds", 24, 24, 512, 64, (float)displayBlinkingTextPosition.x, (float)displayBlinkingTextPosition.y, Layer(15));
	frameContainer.push_back(guiDisplayBlinkingTextFrame);
	guiDisplayBlinkingTextFrame->SetVisibility(false);
	guiDisplayBlinkingTextFrame->SetAlpha(0.75f);


	guiDisplayBlinkingText = new CGUIElement(L"Press 'G' to skip build phase.", Layer(14), int(displayBlinkingTextPosition.x + 10 * screenRatio), int(displayBlinkingTextPosition.y + 20 * screenRatio), 32, 32, false);
	guiDisplayBlinkingText->SetFontSize(DirectX::XMFLOAT2(0.60f, 0.60f));
	guiContainer.push_back(guiDisplayBlinkingText);

	bStartDisplayTextBlinking = false;

	//guiDisplayBlinkingTextBox->RenderActive(false);
	guiDisplayBlinkingText->RenderActive(false);

	//================
	// Opening message
	//=================
	OpeningMessagePosition.x = int(0.50f * (width - 512 * screenRatio));
	OpeningMessagePosition.y = int(0.15f * height);

	guiOpeningMessageBox = new CGUIElement(renderGUI->GetTextureSRV("DisplayTextBox"), Layer(11), 0, 0, 512, 512, false);
	guiOpeningMessageBox->SetScreenPos((float)OpeningMessagePosition.x, (float)OpeningMessagePosition.y);
	guiOpeningMessageBox->SetAlpha(0.85f);
	//guiContainer.push_back(guiOpeningMessageBox);


	guiOpeningMessageFrame = new CUIFrame(renderGUI);
	guiOpeningMessageFrame->SetFrame(L"Assets/Textures/GSFrameRoundFancy2.dds", 24, 24, 512, 64, (float)OpeningMessagePosition.x, (float)OpeningMessagePosition.y, Layer(11));
	guiOpeningMessageFrame->SetColor(DirectX::Colors::Yellow);
	frameContainer.push_back(guiOpeningMessageFrame);

	guiOpeningMessageBoxText = new CGUIElement(L"Protect your Gold!", Layer(10), int(OpeningMessagePosition.x + 10 * screenRatio), int(OpeningMessagePosition.y + 10 * screenRatio), 32, 32, false);
	guiContainer.push_back(guiOpeningMessageBoxText);

	bShowOpeningMessage = true;

	//============================
	// Revive instructions message
	//=============================
	reviveInstructionsPosition.x = int(0.50f * (width - 512 * screenRatio));
	reviveInstructionsPosition.y = int(0.15f * height);

	guiReviveInstructionsBox = new CGUIElement(renderGUI->GetTextureSRV("DisplayTextBox"), Layer(0), 0, 0, 512, 512, true);
	rect.left = 0;
	rect.top = 0;
	rect.right = 512;
	rect.bottom = 1024;
	guiReviveInstructionsBox->SetScreenDisplayRect(rect);
	guiReviveInstructionsBox->SetScreenPos((float)reviveInstructionsPosition.x, (float)reviveInstructionsPosition.y);
	guiContainer.push_back(guiReviveInstructionsBox);

	guiReviveInstructionsText = new CGUIElement(L"   Press 'R' to revive\n      Cost 1 Gold\n Or wait for Build phase", Layer(0), int(OpeningMessagePosition.x + 10 * screenRatio), int(OpeningMessagePosition.y + 10 * screenRatio), 32, 32, false);
	guiReviveInstructionsText->SetFontSize(DirectX::XMFLOAT2(0.75f, 0.75f));
	guiContainer.push_back(guiReviveInstructionsText);

	bShowReviveInstructions = true;

	//===========
	//First Tower
	//============
	firstTowerPosition.x = int(0.25f * width);
	firstTowerPosition.y = int(height - 164 * screenRatio);// int(0.85f * height);

	renderGUI->LoadTexture("FirstTower", L"Assets/Textures/FirstTower.dds");
	renderGUI->LoadTexture("DragonTower", L"Assets/Textures/DragonMeteorTower.dds");
	renderGUI->LoadTexture("Lock", L"Assets/Textures/Lock.dds");
	renderGUI->LoadTexture("FirstTowerUnlocked", L"Assets/Textures/DragonMeteorTowerUnlocked.dds");

	guiFirstTower = new CGUIElement(renderGUI->GetTextureSRV("DragonTower"), Layer(11), 0, 0, 128, 128, false, false, 0.5f);
	guiFirstTower->SetScreenPos((float)firstTowerPosition.x, (float)firstTowerPosition.y);
	guiFirstTower->SetColor(DirectX::Colors::Red);
	guiContainer.push_back(guiFirstTower);

	TowerIconEffect tie;
	tie.ColorChangeDuration = 0.2f;
	tie.ColorTimer = 1000.f;
	tie.bColorChanged = false;
	towerEffectContainer[guiFirstTower] = tie;

	// scale up effect
	ie.scale = 0.5f;
	ie.maxScale = 0.6f;
	iconEffectContainer[guiFirstTower] = ie;

	guiFirstTowerText = new CGUIElement(L"1", Layer(10), firstTowerPosition.x, firstTowerPosition.y, 32, 32, false);
	//guiFirstTowerText->SetScreenPos((float)firstTowerPosition.x, (float)firstTowerPosition.y);
	guiContainer.push_back(guiFirstTowerText);

	renderGUI->LoadTexture("TowerCostIcon", L"Assets/Textures/ManaCrystal.dds");
	guiFirstTowerCostIcon = new CGUIElement(renderGUI->GetTextureSRV("TowerCostIcon"), Layer(11), 0, 0, 128, 128, false);
	guiFirstTowerCostIcon->SetScale(0.25f);
	guiFirstTowerCostIcon->SetScreenPos((float)firstTowerPosition.x, (float)firstTowerPosition.y + 64 * screenRatio);
	guiContainer.push_back(guiFirstTowerCostIcon);

	guiFirstTowerCostText = new CGUIElement(L"150", Layer(10), int(firstTowerPosition.x + 32 * screenRatio), int(firstTowerPosition.y + 74 * screenRatio), 32, 32, false);
	guiFirstTowerCostText->SetFontSize(DirectX::XMFLOAT2(0.5f, 0.5f));
	guiContainer.push_back(guiFirstTowerCostText);

	//lock
	guiFirstTowerLocked = new CGUIElement(renderGUI->GetTextureSRV("Lock"), Layer(11), 0, 0, 64, 64, false, false, 0.9f);
	guiFirstTowerLocked->SetScreenPos(firstTowerPosition.x + (64 * screenRatio) * 0.1f, firstTowerPosition.y + (64 * screenRatio) * 0.1f);
	guiContainer.push_back(guiFirstTowerLocked);


	// First tower unlocked
	guiFirstTowerUnlocked = new CGUIElement(renderGUI->GetTextureSRV("FirstTowerUnlocked"), Layer(15), 0, 0, 512, 256, false);
	guiFirstTowerUnlocked->SetScreenPos(width * 0.5f - 256 * screenRatio, height * 0.5f - 128 * screenRatio);
	guiContainer.push_back(guiFirstTowerUnlocked);

	guiFirstTowerUnlocked->RenderActive(false);

	renderGUI->LoadTexture("SparkleAnimation", L"Assets/Textures/SparkleAnimation.dds");
	guiFirstTowerUnlockAnimation = new CGUIElement(renderGUI->GetTextureSRV("SparkleAnimation"), Layer(11), 0, 0, 960, 1920, false, false, 0.5f);
	guiFirstTowerUnlockAnimation->SetScreenPos(firstTowerPosition.x - 10.f*screenRatio, firstTowerPosition.y - 10.f*screenRatio);

	SPRITE_ANIMATION sparkleAnimation;
	sparkleAnimation.bLoop = false;
	sparkleAnimation.cellCount = 50;
	sparkleAnimation.cellWidth = 192;
	sparkleAnimation.cellHeight = 192;
	sparkleAnimation.duration = 1.f;
	sparkleAnimation.imageWidth = 960;
	sparkleAnimation.imageHeight = 1920;

	guiFirstTowerUnlockAnimation->SetAnimation(sparkleAnimation);
	guiFirstTowerUnlockAnimation->RenderActive(false);
	guiContainer.push_back(guiFirstTowerUnlockAnimation);

	//============
	// Second Tower
	//==============
	secondTowerPosition.x = int(firstTowerPosition.x + 100 * screenRatio);
	secondTowerPosition.y = firstTowerPosition.y;

	renderGUI->LoadTexture("PosionCloud", L"Assets/Textures/PosionCloud.dds");
	renderGUI->LoadTexture("PoisonCloudTrapUnlocked", L"Assets/Textures/PoisonCloudTrapUnlocked.dds");
	renderGUI->LoadTexture("ElectricWallUnlocked", L"Assets/Textures/ElectricWallUnlocked.dds");

	guiSecondTower = new CGUIElement(renderGUI->GetTextureSRV("PosionCloud"), Layer(11), 0, 0, 128, 128, false, false, 0.5f);
	guiSecondTower->SetScreenPos((float)secondTowerPosition.x, (float)secondTowerPosition.y);
	guiSecondTower->SetColor(DirectX::Colors::Red);
	guiContainer.push_back(guiSecondTower);

	towerEffectContainer[guiSecondTower] = tie;

	iconEffectContainer[guiSecondTower] = ie;

	guiSecondTowerText = new CGUIElement(L"2", Layer(0), 0, 0, 32, 32, false);
	guiSecondTowerText->SetScreenPos((float)secondTowerPosition.x, (float)secondTowerPosition.y);
	guiContainer.push_back(guiSecondTowerText);


	guiSecondTowerCostIcon = new CGUIElement(renderGUI->GetTextureSRV("TowerCostIcon"), Layer(11), 0, 0, 128, 128, false);
	guiSecondTowerCostIcon->SetScale(0.25f);
	guiSecondTowerCostIcon->SetScreenPos((float)secondTowerPosition.x, (float)secondTowerPosition.y + 64 * screenRatio);
	guiContainer.push_back(guiSecondTowerCostIcon);

	guiSecondtTowerCostText = new CGUIElement(L"80", Layer(10), int(secondTowerPosition.x + 32 * screenRatio), int(secondTowerPosition.y + 74 * screenRatio), 32, 32, false);
	guiSecondtTowerCostText->SetFontSize(DirectX::XMFLOAT2(0.5f, 0.5f));
	guiContainer.push_back(guiSecondtTowerCostText);


	//lock
	guiSecondTowerLocked = new CGUIElement(renderGUI->GetTextureSRV("Lock"), Layer(11), 0, 0, 64, 64, false, false, 0.9f);
	guiSecondTowerLocked->SetScreenPos(secondTowerPosition.x + (64 * screenRatio) * 0.1f, secondTowerPosition.y + (64 * screenRatio) * 0.1f);
	guiContainer.push_back(guiSecondTowerLocked);

	guiSecondTowerUnlocked = new CGUIElement(renderGUI->GetTextureSRV("PoisonCloudTrapUnlocked"), Layer(15), 0, 0, 512, 256, false);
	guiSecondTowerUnlocked->SetScreenPos(width * 0.5f - 256 * screenRatio, height * 0.5f - 128 * screenRatio);
	guiSecondTowerUnlocked->RenderActive(false);
	guiContainer.push_back(guiSecondTowerUnlocked);


	guiSecondTowerUnlockAnimation = new CGUIElement(renderGUI->GetTextureSRV("SparkleAnimation"), Layer(11), 0, 0, 960, 1920, false, false, 0.5f);
	guiSecondTowerUnlockAnimation->SetScreenPos(secondTowerPosition.x - 10.f*screenRatio, secondTowerPosition.y - 10.f*screenRatio);

	guiSecondTowerUnlockAnimation->SetAnimation(sparkleAnimation);
	guiSecondTowerUnlockAnimation->RenderActive(false);
	guiContainer.push_back(guiSecondTowerUnlockAnimation);

	//===========
	// Third Tower
	//============
	thirdTowerPosition.x = int(secondTowerPosition.x + 100 * screenRatio);
	thirdTowerPosition.y = secondTowerPosition.y;

	renderGUI->LoadTexture("ElectricWall", L"Assets/Textures/ElectricWall.dds");

	guiThirdTower = new CGUIElement(renderGUI->GetTextureSRV("ElectricWall"), Layer(11), 0, 0, 128, 128, false, false, 0.5f);
	guiThirdTower->SetScreenPos((float)thirdTowerPosition.x, (float)thirdTowerPosition.y);
	guiThirdTower->SetColor(DirectX::Colors::Red);
	guiContainer.push_back(guiThirdTower);

	towerEffectContainer[guiThirdTower] = tie;

	iconEffectContainer[guiThirdTower] = ie;

	guiThirdTowerText = new CGUIElement(L"3", Layer(10), 0, 0, 32, 32, false);
	guiThirdTowerText->SetScreenPos((float)thirdTowerPosition.x, (float)thirdTowerPosition.y);
	guiContainer.push_back(guiThirdTowerText);


	guiThirdTowerCostIcon = new CGUIElement(renderGUI->GetTextureSRV("TowerCostIcon"), Layer(11), 0, 0, 128, 128, false);
	guiThirdTowerCostIcon->SetScale(0.25f);
	guiThirdTowerCostIcon->SetScreenPos((float)thirdTowerPosition.x, (float)thirdTowerPosition.y + 64 * screenRatio);
	guiContainer.push_back(guiThirdTowerCostIcon);

	guiThirdTowerCostText = new CGUIElement(L"150", Layer(10), int(thirdTowerPosition.x + 32 * screenRatio), int(thirdTowerPosition.y + 74 * screenRatio), 32, 32, false);
	guiThirdTowerCostText->SetFontSize(DirectX::XMFLOAT2(0.5f, 0.5f));
	guiContainer.push_back(guiThirdTowerCostText);

	//lock
	guiThirdTowerLocked = new CGUIElement(renderGUI->GetTextureSRV("Lock"), Layer(11), 0, 0, 64, 64, false, false, 0.9f);
	guiThirdTowerLocked->SetScreenPos(thirdTowerPosition.x + (64 * screenRatio) * 0.1f, thirdTowerPosition.y + (64 * screenRatio)* 0.1f);
	guiContainer.push_back(guiThirdTowerLocked);

	guiThirdTowerUnlocked = new CGUIElement(renderGUI->GetTextureSRV("ElectricWallUnlocked"), Layer(15), 0, 0, 512, 256, false);
	guiThirdTowerUnlocked->SetScreenPos(width * 0.5f - 256 * screenRatio, height * 0.5f - 128 * screenRatio);
	guiThirdTowerUnlocked->RenderActive(false);
	guiContainer.push_back(guiThirdTowerUnlocked);

	guiThirdTowerUnlockAnimation = new CGUIElement(renderGUI->GetTextureSRV("SparkleAnimation"), Layer(11), 0, 0, 960, 1920, false, false, 0.5f);
	guiThirdTowerUnlockAnimation->SetScreenPos(thirdTowerPosition.x - 10.f*screenRatio, thirdTowerPosition.y - 10.f*screenRatio);

	guiThirdTowerUnlockAnimation->SetAnimation(sparkleAnimation);
	guiThirdTowerUnlockAnimation->RenderActive(false);
	guiContainer.push_back(guiThirdTowerUnlockAnimation);

	//==================
	//Chargin Special Bar
	//====================
	chargingSpecialBarPosition.x = (int)(width - (256 * screenRatio) - (32 * screenRatio));// int(0.80f * width);
	chargingSpecialBarPosition.y = (int)(height - (64 * screenRatio) - (32 * screenRatio));// int(0.93f * height);


	renderGUI->LoadTexture("ChargingBar", L"Assets/Textures/ChargingBar.dds");
	renderGUI->LoadTexture("ChargingBarBackground", L"Assets/Textures/ChargingBarBackground.dds");
	renderGUI->LoadTexture("ChargingSpecialIcon", L"Assets/Textures/SpecialAttackIcon.dds");

	guiChargingSpecialBarBackground = new CGUIElement(renderGUI->GetTextureSRV("ChargingBarBackground"), Layer(11), 0, 0, 512, 64, false, false, 0.5f);
	guiChargingSpecialBarBackground->SetScreenPos((float)chargingSpecialBarPosition.x, (float)chargingSpecialBarPosition.y);
	guiContainer.push_back(guiChargingSpecialBarBackground);

	renderGUI->LoadTexture("ManaBarBlue", L"Assets/Textures/ManaBarBlue.dds");
	guiChargingSpecialBlueBar = new CGUIElement(renderGUI->GetTextureSRV("ManaBarBlue"), Layer(11), 0, 0, 512, 512, false, false, 0.5f);
	guiChargingSpecialBlueBar->SetScreenPos((float)chargingSpecialBarPosition.x, (float)chargingSpecialBarPosition.y);
	guiContainer.push_back(guiChargingSpecialBlueBar);

	rect.left = 0;
	rect.top = 0;
	rect.bottom = 512;
	rect.right = 0;
	guiChargingSpecialBlueBar->SetScreenRec(rect);

	guiChargingSpecialBar = new CGUIElement(renderGUI->GetTextureSRV("ChargingBar"), Layer(11), 0, 0, 512, 64, false, false, 0.5f);
	guiChargingSpecialBar->SetScreenPos((float)chargingSpecialBarPosition.x, (float)chargingSpecialBarPosition.y);
	guiContainer.push_back(guiChargingSpecialBar);

	guiChargingSpecialIcon = new CGUIElement(renderGUI->GetTextureSRV("ChargingSpecialIcon"), Layer(11), 0, 0, 64, 64);
	guiChargingSpecialIcon->SetScreenPos((float)chargingSpecialBarPosition.x - 64 * screenRatio, (float)chargingSpecialBarPosition.y - 16 * screenRatio);
	guiContainer.push_back(guiChargingSpecialIcon);

	//=============
	// Towers Built
	//==============
	towersBuiltPosition.x = int(width * 0.66f);//  int(0.65f * width);
	towersBuiltPosition.y = (int)(height - (128 * screenRatio) - (48 * screenRatio));// int(0.83f * height);
	towerCount = 0;

	renderGUI->LoadTexture("TowerBuiltIcon", L"Assets/Textures/TowersBuilt.dds");
	guiTowersBuiltIcon = new CGUIElement(renderGUI->GetTextureSRV("TowerBuiltIcon"), Layer(11), 0, 0, 64, 128, false);
	guiTowersBuiltIcon->SetScreenPos((float)towersBuiltPosition.x, (float)towersBuiltPosition.y);
	guiContainer.push_back(guiTowersBuiltIcon);

	guiTowersBuiltText = new CGUIElement(L"0", Layer(10), int(towersBuiltPosition.x + 70 * screenRatio), int(towersBuiltPosition.y + 85 * screenRatio), 32, 32, false);
	guiContainer.push_back(guiTowersBuiltText);

	//=============
	// Sell Tower
	//=============
	backgroundBoxPosition4.x = int(0.2f *  width);
	backgroundBoxPosition4.y = int(0.8f *  height);


	//guiBackgroundBox4 = new CGUIElement(renderGUI->GetTextureSRV("BackgroundBox"), Layer(4), 0, 0, 256, 132);
	//rect.left = 0;
	//rect.top = 0;
	//rect.right = 500;
	//rect.bottom = 50;
	//guiBackgroundBox4->SetScreenDisplayRect(rect);
	//guiBackgroundBox4->SetScreenPos((float)backgroundBoxPosition4.x, (float)backgroundBoxPosition4.y);
	//guiBackgroundBox4->SetAlpha(0.65f);

	guiSellTextFrame = new CUIFrame(renderGUI);
	guiSellTextFrame->SetFrame(L"Assets/Textures/GSFrameRound2.dds", 24, 24, 500, 50, (float)backgroundBoxPosition4.x, (float)backgroundBoxPosition4.y, Layer(13));
	frameContainer.push_back(guiSellTextFrame);

	guiSellText = new CGUIElement(L"Press 'E' to modify a tower", Layer(13), 0, 0, 32, 32, false);
	guiSellText->SetScreenPos((float)backgroundBoxPosition4.x + 20 * screenRatio, (float)backgroundBoxPosition4.y + 10 * screenRatio);
	guiSellText->SetFontSize(DirectX::XMFLOAT2(0.5f, 0.8f));
	//	guiBackgroundBox4->SetColor()
	//guiContainer.push_back(guiBackgroundBox4);
	guiContainer.push_back(guiSellText);


#pragma region Gems
	//=========
	// GEMS

	renderGUI->LoadTexture("FireUpgrade", L"Assets/Textures/TowerUpgradeButton_Fire.dds");
	renderGUI->LoadTexture("IceUpgrade", L"Assets/Textures/TowerUpgradeButton_Frost.dds");
	renderGUI->LoadTexture("LightningUpgrade", L"Assets/Textures/TowerUpgradeButton_Lightning.dds");
	renderGUI->LoadTexture("RangeUpgrade", L"Assets/Textures/TowerUpgradeButton_Range.dds");
	renderGUI->LoadTexture("CurseUpgrade", L"Assets/Textures/TowerUpgradeButton_Curse.dds");
	renderGUI->LoadTexture("AoEUpgrade", L"Assets/Textures/TowerUpgradeButton_Area.dds");
	renderGUI->LoadTexture("MultishotUpgrade", L"Assets/Textures/TowerUpgradeButton_Multishot.dds");



#pragma endregion


	//=============
	// Cancel Structure
	//==============
	backgroundBoxPosition5.x = int(0.34f * width);
	backgroundBoxPosition5.y = int(0.78f * height);


	guiCancelText = new CGUIElement(L"Left Click to confirm, Right Click to Cancel", Layer(10), 0, 0, 32, 32, false);

	guiCancelText->SetScreenPos((float)backgroundBoxPosition5.x + 10 * screenRatio, (float)backgroundBoxPosition5.y + 10 * screenRatio);
	guiCancelText->SetFontSize(DirectX::XMFLOAT2(0.5f, 0.8f));
	guiCancelText->SetColor(DirectX::Colors::Yellow);


	//=============
	// Tower Rotate
	//==============
	backgroundBoxPosition3.x = (int)((width * .5f) - 320 * screenRatio);
	backgroundBoxPosition3.y = (int)((height *.58f) + 200 * screenRatio);

	guiRotateIconPosition.x = (int)((width * .5f) - 100 * screenRatio);
	guiRotateIconPosition.y = (int)((height * .62f) - 56 * screenRatio);

	renderGUI->LoadTexture("RotateCircle", L"Assets/Textures/RotateCircle.dds");

	guiRotateIcon = new CGUIElement(renderGUI->GetTextureSRV("RotateCircle"), Layer(10), 0, 0, 639, 640);
	rect.left = 0;
	rect.top = 0;
	rect.right = 200;
	rect.bottom = 200;
	guiRotateIcon->SetScreenDisplayRect(rect);
	guiRotateIcon->SetScreenPos((float)guiRotateIconPosition.x, (float)guiRotateIconPosition.y);
	guiRotateIcon->SetAlpha(0.75f);


	guiRotateText = new CGUIElement(L"         Rotate Structure with mouse.\n Left click to confirm, right click to cancel.", Layer(10), 0, 0, 32, 32, false);
	guiRotateText->SetScreenPos((float)backgroundBoxPosition3.x + 20 * screenRatio, (float)backgroundBoxPosition3.y + 10 * screenRatio);
	guiRotateText->SetFontSize(DirectX::XMFLOAT2(0.5f, 0.8f));
	guiRotateText->SetColor(DirectX::Colors::Aqua);

	guiRotateTextFrame = new CUIFrame(renderGUI);
	guiRotateTextFrame->SetFrame(L"Assets/Textures/GSFrameRound2.dds", 24, 24, 640, 100, (float)backgroundBoxPosition3.x, (float)backgroundBoxPosition3.y, Layer(10));
	guiRotateTextFrame->SetVisibility(false);
	guiRotateTextFrame->SetAlpha(0.95f);
	frameContainer.push_back(guiRotateTextFrame);


	guiContainer.push_back(guiRotateText);
	guiContainer.push_back(guiRotateIcon);

	//=============
	// Tower UNABLE TO PLACE
	//=============
	structDisplayTextPosition.x = int(0.50f * (width - 512 * screenRatio));
	structDisplayTextPosition.y = int(0.225f * height);

	renderGUI->LoadTexture("DisplayTextBox", L"Assets/Textures/DisplayTextBox.dds");

	guiStructText = new CGUIElement(L"Can't build here", Layer(11), int(structDisplayTextPosition.x + 65 * screenRatio), int(structDisplayTextPosition.y + 14 * screenRatio), 32, 32, false);
	guiStructText->SetFontSize(XMFLOAT2{ 0.9f, .8f });
	guiStructText->SetColor(DirectX::Colors::Red);
	guiContainer.push_back(guiStructText);


	guiCancelTextFrame = new CUIFrame(renderGUI);
	guiCancelTextFrame->SetFrame(L"Assets/Textures/GSFrameRound2.dds", 24, 24, 512, 64, (float)structDisplayTextPosition.x, (float)structDisplayTextPosition.y, Layer(11));
	guiCancelTextFrame->SetVisibility(false);
	frameContainer.push_back(guiCancelTextFrame);

	//==================
	// Build Phase Timer
	//====================
	DisplayPhaseTimerPosition.x = (int)(width * 0.5f - 64 * screenRatio);
	DisplayPhaseTimerPosition.y = (int)(height * 0.05f);


	guiDisplayPhaseTimerFrame = new CUIFrame(renderGUI);
	guiDisplayPhaseTimerFrame->SetFrame(L"Assets/Textures/GSFrameRoundFancy2.dds", 24, 24, 128, 100, (float)DisplayPhaseTimerPosition.x, (float)DisplayPhaseTimerPosition.y, Layer(10));
	guiDisplayPhaseTimerFrame->SetVisibility(false);
	frameContainer.push_back(guiDisplayPhaseTimerFrame);


	guiDisplayPhaseTimerText = new CGUIElement(L"0", Layer(10), DisplayPhaseTimerPosition.x, int(DisplayPhaseTimerPosition.y + 8 * screenRatio), 32, 32, false);
	guiDisplayPhaseTimerText->SetFontSize(DirectX::XMFLOAT2(2.f, 2.f));
	guiContainer.push_back(guiDisplayPhaseTimerText);
	guiDisplayPhaseTimerText->RenderActive(false);

	//===========
	//HTP Screeen
	//==============
	renderGUI->LoadTexture("HTP", L"Assets/Textures/HTP.dds");
	guiHTP = new CGUIElement(renderGUI->GetTextureSRV("HTP"), 0.0f, 0, 0, 1024, 768, false, false);
	guiHTP->SetScreenPos(0.23f * GlobalDef::windowWidth, 0.14f * GlobalDef::windowHeight);
	guiHTP->RenderActive(false);
	renderGUI->AddGUIElement(guiHTP);


	//=============
	// Variant Wave
	//===============
	VariantWave = new CVariantWaveHUD(renderGUI, width, height);

	//========
	// Win Lose=
	//=========
	WinLoseHUD = new CWinLoseHUD(renderGUI, width, height);
	WinLoseHUD->SetVisibility(false);

	// Tower Upgrade
	TowerUpgradeHUD = new CTowerUpgradeHUD(renderGUI, width, height);

	// Pause menu
	PauseMenuHUD = new CPauseMenuHUD(renderGUI, width, height);

	// Full screen death effect
	renderGUI->LoadTexture("GSBackground", L"Assets/Textures/GSBackground.dds");

	// Wave info Billboard
	WaveInfoBillboard = new CWaveInfoBillboard(CGame::GetInstance()->GetRenderer());

	// Player Upgrade menu
	PlayerUpgradeHUD = new CPlayerUpgradeHUD(renderGUI, width, height, CGame::GetInstance()->GetMyPlayerID());
	PlayerUpgradeHUD->SetMessageProc(&SendEvent);

	// Companion HUD
	CompanionHUD = new CCompanionHUD(renderGUI, (int)width, (int)height, CGame::GetInstance()->GetMyPlayerID());
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateCompanionHUD, CompanionHUD);

	// Boss Incoming
	m_BossIncoming = new CGUIElement(L"", Layer(10), int(width*0.2f), int(height * 0.3f), 32, 32, false);
	m_BossIncoming->RenderActive(false);
	m_BossIncoming->SetFontSize(2.f);
	guiContainer.push_back(m_BossIncoming);

	guiFullScreen = new CGUIElement(renderGUI->GetTextureSRV("GSBackground"), Layer(20), 0, 0, 2, 2, true, false);
	rect.left = 0;
	rect.top = 0;
	rect.right = (long)width;
	rect.bottom = (long)height;
	guiFullScreen->SetScreenDisplayRect(rect);
	renderGUI->AddGUIElement(guiFullScreen);
	DirectX::XMVECTOR color;
	color = DirectX::Colors::SandyBrown;
	//color = DirectX::Colors::Red;
	color.m128_f32[0] *= 0.25f;
	color.m128_f32[1] *= 0.25f;
	color.m128_f32[2] *= 0.25f;
	color.m128_f32[3] = 0.25f;
	guiFullScreen->SetColor(color);
	guiFullScreen->RenderActive(false);

	//===============================
	// Add gui elements to renderGUI
	//===============================
	for (unsigned int i = 0; i < guiContainer.size(); i++)
	{
		renderGUI->AddGUIElement(guiContainer[i]);
	}


	renderGUI->removeGUIElement(guiDisplayText);


	renderGUI->removeGUIElement(guiCrosshairRed);

	renderGUI->removeGUIElement(guiReviveInstructionsBox);
	renderGUI->removeGUIElement(guiReviveInstructionsText);

	renderGUI->removeGUIElement(guiSlowedByCandy);
	m_bShowingSlowDebuff = false;

	renderGUI->removeGUIElement(guiStructText);

	renderGUI->removeGUIElement(guiRotateIcon);
	renderGUI->removeGUIElement(guiRotateText);

	bhealthToggle = false;


	renderGUI->removeGUIElement(guiCandyOnPlayer);
	renderGUI->removeGUIElement(guiCandyOnPlayerText);

	CGame::GetInstance()->GetClient()->SendEventMessage(EventID::GetTowerPriceOnHUD, TEventData());


}

CGamePlayHUD::~CGamePlayHUD()
{
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateEnemiesRemaning, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateHealth, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateMana, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateGem, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateCandy, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::DisplayHUDText, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::DisplayCBTText, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateCurrency, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateCurrentWave, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateHUDBuildIcon, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateCandyOnPlayer, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::ChargingSpecial, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::Keypress, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::CursorVisibility, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::CrosshairTexture, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::Revived, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::PlayerDead, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::MostMana, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::MostKills, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::MostTowers, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::MostCandyReturned, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::MostDeaths, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::MostAcurrate, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateTowerCap, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateBuildPhaseTimer, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::SetTowerPriceOnHUD, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::RotateIconHUD, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UnlockedTower, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateMaxHealth, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateCompanionHUD, CompanionHUD);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::BossIncoming, this);

	delete VariantWave;
	delete guiFullScreen;
	delete TowerUpgradeHUD;
	delete PauseMenuHUD;
	delete WaveInfoBillboard;
	delete PlayerUpgradeHUD;
	delete CompanionHUD;

	if (guiHTP)
	{
		delete guiHTP;
		guiHTP = nullptr;
	}

	for (unsigned int i = 0; i < guiContainer.size(); i++)
	{
		delete guiContainer[i];
		guiContainer[i] = nullptr;
	}

	for (unsigned int i = 0; i < frameContainer.size(); i++)
	{
		delete frameContainer[i];
		frameContainer[i] = nullptr;
	}

	delete renderGUI;
}

void CGamePlayHUD::Update()
{
	guiHTP->RenderActive(CGame::GetInstance()->GetHTP());

	TowerUpgradeHUD->SetMana(mana);
	TowerUpgradeHUD->Update(!PauseMenuHUD->PauseMenuOpen());

	float _dt = CGame::GetInstance()->GetDelta();

	guiFirstTowerUnlockAnimation->UpdateAnimation(_dt);
	guiSecondTowerUnlockAnimation->UpdateAnimation(_dt);
	guiThirdTowerUnlockAnimation->UpdateAnimation(_dt);

	if ((displayTextTimer -= 0.333f*_dt) < 0.f && !displayTextPopped)
	{
		renderGUI->removeGUIElement(guiDisplayText);
		//renderGUI->removeGUIElement(guiDisplayTextBox);
		guiDisplayTextFrame->SetVisibility(false);
		displayTextPopped = true;

		// start sending build pahse timer updates
		CGame::GetInstance()->GetClient()->SendEventMessage(EventID::UpdateBuildPhaseTimer, TEventData());

		bShowFirstTowerUnlocked = false;
		bShowSecondTowerUnlocked = false;
		bShowThirdTowerUnlocked = false;
		guiFirstTowerUnlocked->RenderActive(false);
		guiSecondTowerUnlocked->RenderActive(false);
		guiThirdTowerUnlocked->RenderActive(false);

		// for variant wave
		if (VariantWave->GetVariantWaveIncoming())
			VariantWave->ShowWarning();
	}
	else if (!displayTextPopped)
	{
		guiDisplayText->SetAlpha(guiDisplayText->GetAlpha() - _dt * 0.2f);
		//guiDisplayTextBox->SetAlpha(guiDisplayTextBox->GetAlpha() - _dt * 0.2f);
		guiDisplayTextFrame->SetAlpha(guiDisplayTextFrame->GetAlpha() - _dt * 0.4f);
		//guiDisplayTextFrame->SetVisibility(false);

		if (bShowFirstTowerUnlocked)
		{
			guiFirstTowerUnlocked->SetAlpha(guiFirstTowerUnlocked->GetAlpha() - _dt * 0.05f);
		}

		if (bShowSecondTowerUnlocked)
		{
			guiSecondTowerUnlocked->SetAlpha(guiSecondTowerUnlocked->GetAlpha() - _dt * 0.05f);
		}

		if (bShowThirdTowerUnlocked)
		{
			guiThirdTowerUnlocked->SetAlpha(guiThirdTowerUnlocked->GetAlpha() - _dt * 0.05f);
		}
	}


	std::map<CGUIElement*, TowerIconEffect>::iterator it = towerEffectContainer.begin();
	for (; it != towerEffectContainer.end(); it++)
	{
		if ((it->second.ColorTimer += _dt) > it->second.ColorChangeDuration && it->second.bColorChanged)
		{
			it->second.bColorChanged = false;

			DirectX::XMVECTOR c;
			c.m128_f32[0] = it->second.color.x;
			c.m128_f32[1] = it->second.color.y;
			c.m128_f32[2] = it->second.color.z;
			c.m128_f32[3] = it->second.color.w;

			it->first->SetColor(c);
		}
	}

	VariantWave->Update(_dt);
	UpdateFontEffect(_dt);
	UpdateIconEffect(_dt);

	//================
	//================
	renderGUI->UpdateButtons();
	PauseMenuHUD->Update(_dt);
	PlayerUpgradeHUD->Update(_dt);
	WaveInfoBillboard->Update();

	if ((chargingSpecialFlashTimer += _dt) > 0.1f && bChargingStartFlashing)
	{
		if ((bChargingFlashing = !bChargingFlashing))
		{
			guiChargingSpecialBlueBar->SetColor(DirectX::Colors::Yellow);
		}
		else
			guiChargingSpecialBlueBar->SetColor(DirectX::Colors::White);

		chargingSpecialFlashTimer = 0;
	}

	if (health == 0 && bShowReviveInstructions)
	{
		renderGUI->AddGUIElement(guiReviveInstructionsBox);
		renderGUI->AddGUIElement(guiReviveInstructionsText);
		bShowReviveInstructions = false;
	}

	if (m_BossIncomingTimer > 0.f)
	{
		m_BossIncomingTimer -= _dt;

		if (m_BossIncomingTimer < 0)
			m_BossIncoming->RenderActive(false);
	}

}

void CGamePlayHUD::ReceiveEvent(EventID id, TEventData const* data)
{

	switch (id)
	{
	case RotateIconHUD:
	{
		if (data->i == 0)
		{
			//renderGUI->removeGUIElement(guiBackgroundBox3);
			guiRotateTextFrame->SetVisibility(false);
			renderGUI->removeGUIElement(guiRotateIcon);
			renderGUI->removeGUIElement(guiRotateText);

			//renderGUI->removeGUIElement(guiBackgroundBox5);
			//renderGUI->removeGUIElement(guiCancelText);


		}
		else
		{
			//renderGUI->AddGUIElement(guiBackgroundBox3);
			guiRotateTextFrame->SetVisibility(true);
			renderGUI->AddGUIElement(guiRotateIcon);
			renderGUI->AddGUIElement(guiRotateText);

			//renderGUI->AddGUIElement(guiBackgroundBox5);
			//renderGUI->AddGUIElement(guiCancelText);

		}

		break;
	}
	case SetTowerPriceOnHUD:
	{
		guiFirstTowerCostText->SetText(std::to_wstring((int)data->m[0]));
		guiSecondtTowerCostText->SetText(std::to_wstring((int)data->m[1]));
		guiThirdTowerCostText->SetText(std::to_wstring((int)data->m[2]));

		firstTowerCost = int(data->m[0]);
		secondTowerCost = int(data->m[1]);
		thirdTowerCost = int(data->m[2]);

		break;
	}
	case DisplayCBTText:
	{
		if (data->i == 1)
		{
			//renderGUI->AddGUIElement(guiStructTextBox);
			guiCancelTextFrame->SetVisibility(true);
			renderGUI->AddGUIElement(guiStructText);

		}
		else
		{
			renderGUI->removeGUIElement(guiStructText);
			//renderGUI->removeGUIElement(guiStructTextBox);
			guiCancelTextFrame->SetVisibility(false);
		}

		break;
	}
	case Keypress:
	{

		if (data->i == 'G' && guiDisplayBlinkingText->GetRenderActive())
		{
			//guiDisplayBlinkingText->SetText(L"   Waiting for other players");
		}

		break;
	}
	case CursorVisibility:
	{

		break;
	}
	case UpdateEnemiesRemaning:
	{
		enemyCount = data->i;
		guiEnemyCount->SetText(std::to_wstring(enemyCount));

		fontEffectContainer[guiEnemyCount].fontTimer = 0;
		fontEffectContainer[guiEnemyCount].fontEffectDuration = 0.1f;
		fontEffectContainer[guiEnemyCount].bFontIncrese = true;
		break;
	}

	case UpdateHealth:
	{
		if (data->i < 0)
			health = 0;
		else
			health = data->i;

		UpdateHealthOnHUD();

		break;
	}
	case UpdateMaxHealth:
	{
		maxHealth = data->i;
		UpdateHealthOnHUD();
		break;
	}
	case UpdateGem:
	{
		mana = data->i;
		break;
	}

	case UpdateMana:
	{
		mana = data->i;
		guiManaGemCount->SetText(std::to_wstring(mana));

		fontEffectContainer[guiManaGemCount].fontTimer = 0;
		fontEffectContainer[guiManaGemCount].fontEffectDuration = 0.1f;
		fontEffectContainer[guiManaGemCount].bFontIncrese = true;

		iconEffectContainer[guiManaGem].scaleTimer = 0;
		iconEffectContainer[guiManaGem].scaleEffectDuration = 0.1f;
		iconEffectContainer[guiManaGem].bScaleIncrese = true;

		// Towers stay red untill unlocked
		if (data->m[0] == 0)
			guiFirstTower->SetColor(DirectX::Colors::Red);
		else if (bFirstTowerUnlocked)
			guiFirstTower->SetColor(DirectX::Colors::White);

		if (data->m[1] == 0)
			guiSecondTower->SetColor(DirectX::Colors::Red);
		else if (bSecondTowerUnlocked)
			guiSecondTower->SetColor(DirectX::Colors::White);

		if (data->m[2] == 0)
			guiThirdTower->SetColor(DirectX::Colors::Red);
		else if (bThirdTowerUnlocked)
			guiThirdTower->SetColor(DirectX::Colors::White);
		break;
	}

	case UpdateCandy:
	{
		candyCount = data->i;
		guiCandyCount->SetText(std::to_wstring(candyCount));

		fontEffectContainer[guiCandyCount].fontTimer = 0;
		fontEffectContainer[guiCandyCount].fontEffectDuration = 0.1f;
		fontEffectContainer[guiCandyCount].bFontIncrese = true;

		iconEffectContainer[guiCandy].scaleTimer = 0;
		iconEffectContainer[guiCandy].scaleEffectDuration = 0.1f;
		iconEffectContainer[guiCandy].bScaleIncrese = true;
		break;
	}

	case UpdateCurrency:
	{
		//currencyCount = data->i;
		//guiCurrencyCount->SetText(std::to_wstring(currencyCount));
		break;
	}

	case DisplayHUDText:
	{
		if (!bShowScoreBoard)
		{

			displayText.assign(data->s.begin(), data->s.end());

			//guiDisplayText->SetScreenPos((float)displayTextPosition.x, (float)displayTextPosition.y);
			//guiDisplayTextBox->SetScreenPos(displayTextPosition.x - 30.f, displayTextPosition.y - 10.f);
			if (data->m[4] != 1)
			{

				guiDisplayText->SetText(displayText);
				guiDisplayText->SetAlpha(1.f);
				//guiDisplayTextBox->SetAlpha(1.f);
				guiDisplayTextFrame->SetAlpha(1.f);

				//renderGUI->AddGUIElement(guiDisplayTextBox);
				guiDisplayTextFrame->SetVisibility(true);
				renderGUI->AddGUIElement(guiDisplayText);

				displayTextPopped = false;
				displayTextTimer = data->m[0];
			}

			// m[3] use as bool to start/stop blinking text
			if (data->m[3] == 0)
			{
				bStartDisplayTextBlinking = true;
				displayTextBlinkingTimer = 0.5f;

				//guiDisplayBlinkingTextBox->RenderActive(true);
				guiDisplayBlinkingText->SetText(displayText);
				guiDisplayBlinkingTextFrame->SetVisibility(true);
				guiDisplayBlinkingText->RenderActive(true);
				displayTextBlinkingPopped = false;
			}
			else if (data->m[3] == 1)
			{
				bStartDisplayTextBlinking = false;

				//guiDisplayBlinkingTextBox->RenderActive(false);
				guiDisplayBlinkingTextFrame->SetVisibility(false);
				guiDisplayBlinkingText->RenderActive(false);
				displayTextBlinkingPopped = true;
			}

			if (bShowOpeningMessage)
			{
				bShowOpeningMessage = false;
				//renderGUI->removeGUIElement(guiOpeningMessageBox);
				guiOpeningMessageFrame->SetVisibility(false);
				renderGUI->removeGUIElement(guiOpeningMessageBoxText);
			}
		}
		break;
	}

	case UpdateCurrentWave:
	{
		guiWaveCount->SetText(std::to_wstring(data->i));

		fontEffectContainer[guiWaveCount].fontTimer = 0;
		fontEffectContainer[guiWaveCount].fontEffectDuration = 0.1f;
		fontEffectContainer[guiWaveCount].bFontIncrese = true;
		break;
	}

	case UpdateHUDBuildIcon:
	{
		//std::wstring towerCapString = L"";
		//towerCapString += std::to_wstring((int)data->m[14]);
		//towerCapString += L"/";
		//towerCapString += std::to_wstring((int)data->m[15]);
		//guiTowersBuiltText->SetText(towerCapString);

		switch (data->i)
		{
		case 1:
		{
			towerEffectContainer[guiFirstTower].bColorChanged = true;
			towerEffectContainer[guiFirstTower].color.x = guiFirstTower->GetColor().m128_f32[0];
			towerEffectContainer[guiFirstTower].color.y = guiFirstTower->GetColor().m128_f32[1];
			towerEffectContainer[guiFirstTower].color.z = guiFirstTower->GetColor().m128_f32[2];
			towerEffectContainer[guiFirstTower].color.w = guiFirstTower->GetColor().m128_f32[3];

			towerEffectContainer[guiFirstTower].ColorTimer = 0.f;

			guiFirstTower->SetColor(DirectX::Colors::Yellow);
			break;
		}

		case 2:
		{
			towerEffectContainer[guiSecondTower].bColorChanged = true;
			towerEffectContainer[guiSecondTower].color.x = guiSecondTower->GetColor().m128_f32[0];
			towerEffectContainer[guiSecondTower].color.y = guiSecondTower->GetColor().m128_f32[1];
			towerEffectContainer[guiSecondTower].color.z = guiSecondTower->GetColor().m128_f32[2];
			towerEffectContainer[guiSecondTower].color.w = guiSecondTower->GetColor().m128_f32[3];
			towerEffectContainer[guiSecondTower].ColorTimer = 0.f;

			guiSecondTower->SetColor(DirectX::Colors::Yellow);
			break;
		}
		case 3:
		{
			towerEffectContainer[guiThirdTower].bColorChanged = true;
			towerEffectContainer[guiThirdTower].color.x = guiThirdTower->GetColor().m128_f32[0];
			towerEffectContainer[guiThirdTower].color.y = guiThirdTower->GetColor().m128_f32[1];
			towerEffectContainer[guiThirdTower].color.z = guiThirdTower->GetColor().m128_f32[2];
			towerEffectContainer[guiThirdTower].color.w = guiThirdTower->GetColor().m128_f32[3];
			towerEffectContainer[guiThirdTower].ColorTimer = 0.f;

			guiThirdTower->SetColor(DirectX::Colors::Yellow);
			break;
		}
		default:
			break;
		}
		break;
	}

	case UpdateCandyOnPlayer:
	{
		guiCandyOnPlayerText->SetText(std::to_wstring(data->i));

		fontEffectContainer[guiCandyOnPlayerText].fontTimer = 0;
		fontEffectContainer[guiCandyOnPlayerText].fontEffectDuration = 0.1f;
		fontEffectContainer[guiCandyOnPlayerText].bFontIncrese = true;

		iconEffectContainer[guiCandyOnPlayer].scaleTimer = 0;
		iconEffectContainer[guiCandyOnPlayer].scaleEffectDuration = 0.1f;
		iconEffectContainer[guiCandyOnPlayer].bScaleIncrese = true;

		//Show slow debuff icon
		if (data->i == 0 && m_bShowingSlowDebuff)
		{
			renderGUI->removeGUIElement(guiSlowedByCandy);
			m_bShowingSlowDebuff = false;
		}
		if (data->i != 0 && !m_bShowingSlowDebuff)
		{
			renderGUI->AddGUIElement(guiSlowedByCandy);
			m_bShowingSlowDebuff = true;
		}
		break;
	}

	case ChargingSpecial:
	{
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.bottom = 512;
		rect.right = LONG(512 * data->m[0]);

		guiChargingSpecialBlueBar->SetScreenRec(rect);

		if (data->m[0] >= 1)
			bChargingStartFlashing = true;
		else if (data->m[0] == 0)
		{
			bChargingStartFlashing = false;
			guiChargingSpecialBlueBar->SetColor(DirectX::Colors::White);
		}

		break;
	}
	case CrosshairTexture:
	{
		if (bShowCrossHair)
		{
			int crosshairTexture = data->i;

			renderGUI->removeGUIElement(guiCrosshair);
			renderGUI->removeGUIElement(guiCrosshairRed);

			if (crosshairTexture == 0)
			{
				renderGUI->AddGUIElement(guiCrosshair);
			}
			else if (crosshairTexture == 1)
			{
				renderGUI->AddGUIElement(guiCrosshairRed);
			}
		}
		break;
	}
	case Revived:
	{
		renderGUI->removeGUIElement(guiReviveInstructionsBox);
		renderGUI->removeGUIElement(guiReviveInstructionsText);
		bShowReviveInstructions = true;
		guiFullScreen->RenderActive(false);

		guiFullScreen->SetLayerDepth(Layer(20));
		renderGUI->Sort();

		break;
	}
	case PlayerDead:
	{
		guiFullScreen->SetLayerDepth(Layer(5));
		renderGUI->Sort();
		guiFullScreen->RenderActive(true);
		bShowReviveInstructions = true;
		break;
	}
	case UpdateTowerCap:
	{
		std::wstring towerCapString = L"";
		towerCapString += std::to_wstring((int)data->m[14]);
		towerCapString += L"/";
		towerCapString += std::to_wstring((int)data->m[15]);
		guiTowersBuiltText->SetText(towerCapString);
		break;
	}
	case UpdateBuildPhaseTimer:
	{
		guiDisplayPhaseTimerText->SetText(std::to_wstring(data->i));

		if (data->i == -1)
		{
			guiDisplayPhaseTimerText->RenderActive(false);
			//guiDisplayPhaseTimerBox->RenderActive(false);
			guiDisplayPhaseTimerFrame->SetVisibility(false);
		}
		else
		{
			guiDisplayPhaseTimerText->RenderActive(true);
			//guiDisplayPhaseTimerBox->RenderActive(true);
			guiDisplayPhaseTimerFrame->SetVisibility(true);
		}

		if (data->i <= 10)
		{
			if (previousPhaseTime != data->i && previousPhaseTime > 0)
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_BEEP);
			previousPhaseTime = data->i;

			guiDisplayPhaseTimerText->SetColor(DirectX::Colors::Yellow);
			if (data->i <= 9)
				guiDisplayPhaseTimerText->SetScreenPos(DisplayPhaseTimerPosition.x + 30.f, DisplayPhaseTimerPosition.y + 8.f);
		}
		else
		{
			guiDisplayPhaseTimerText->SetColor(DirectX::Colors::White);
			guiDisplayPhaseTimerText->SetScreenPos((float)DisplayPhaseTimerPosition.x, (float)DisplayPhaseTimerPosition.y + 8);
		}

		break;
	}
	case UnlockedTower:
	{
		switch (data->i)
		{
		case 1:
		{
			if (firstTowerCost <= mana && !bFirstTowerUnlocked)
			{
				guiFirstTower->SetColor(DirectX::Colors::White);

				towerEffectContainer[guiFirstTower].bColorChanged = true;
				towerEffectContainer[guiFirstTower].color.x = guiFirstTower->GetColor().m128_f32[0];
				towerEffectContainer[guiFirstTower].color.y = guiFirstTower->GetColor().m128_f32[1];
				towerEffectContainer[guiFirstTower].color.z = guiFirstTower->GetColor().m128_f32[2];
				towerEffectContainer[guiFirstTower].color.w = guiFirstTower->GetColor().m128_f32[3];

				towerEffectContainer[guiFirstTower].ColorTimer = 0.f;

				guiFirstTower->SetColor(DirectX::Colors::Yellow);

				iconEffectContainer[guiFirstTower].scaleTimer = 0;
				iconEffectContainer[guiFirstTower].scaleEffectDuration = 0.2f;
				iconEffectContainer[guiFirstTower].bScaleIncrese = true;
			}
			bFirstTowerUnlocked = true;
			bShowFirstTowerUnlocked = true;
			guiFirstTowerLocked->RenderActive(false);
			if (!data->m[0])
				guiFirstTowerUnlocked->RenderActive(true);
			guiFirstTowerUnlockAnimation->StartAnimation();
			break;
		}
		case 2:
		{
			if (secondTowerCost <= mana && !bSecondTowerUnlocked)
			{
				guiSecondTower->SetColor(DirectX::Colors::White);


				towerEffectContainer[guiSecondTower].bColorChanged = true;
				towerEffectContainer[guiSecondTower].color.x = guiSecondTower->GetColor().m128_f32[0];
				towerEffectContainer[guiSecondTower].color.y = guiSecondTower->GetColor().m128_f32[1];
				towerEffectContainer[guiSecondTower].color.z = guiSecondTower->GetColor().m128_f32[2];
				towerEffectContainer[guiSecondTower].color.w = guiSecondTower->GetColor().m128_f32[3];

				towerEffectContainer[guiSecondTower].ColorTimer = 0.f;

				guiSecondTower->SetColor(DirectX::Colors::Yellow);

				iconEffectContainer[guiSecondTower].scaleTimer = 0;
				iconEffectContainer[guiSecondTower].scaleEffectDuration = 0.2f;
				iconEffectContainer[guiSecondTower].bScaleIncrese = true;
			}
			bSecondTowerUnlocked = true;
			bShowSecondTowerUnlocked = true;
			guiSecondTowerLocked->RenderActive(false);
			if (!data->m[0])
				guiSecondTowerUnlocked->RenderActive(true);
			guiSecondTowerUnlockAnimation->StartAnimation();
			break;
		}
		case 3:
		{
			if (thirdTowerCost <= mana && !bThirdTowerUnlocked)
			{
				guiThirdTower->SetColor(DirectX::Colors::White);

				towerEffectContainer[guiThirdTower].bColorChanged = true;
				towerEffectContainer[guiThirdTower].color.x = guiThirdTower->GetColor().m128_f32[0];
				towerEffectContainer[guiThirdTower].color.y = guiThirdTower->GetColor().m128_f32[1];
				towerEffectContainer[guiThirdTower].color.z = guiThirdTower->GetColor().m128_f32[2];
				towerEffectContainer[guiThirdTower].color.w = guiThirdTower->GetColor().m128_f32[3];

				towerEffectContainer[guiThirdTower].ColorTimer = 0.f;

				guiThirdTower->SetColor(DirectX::Colors::Yellow);

				iconEffectContainer[guiThirdTower].scaleTimer = 0;
				iconEffectContainer[guiThirdTower].scaleEffectDuration = 0.2f;
				iconEffectContainer[guiThirdTower].bScaleIncrese = true;
			}
			bThirdTowerUnlocked = true;
			bShowThirdTowerUnlocked = true;
			guiThirdTowerLocked->RenderActive(false);
			if (!data->m[0])
				guiThirdTowerUnlocked->RenderActive(true);
			guiThirdTowerUnlockAnimation->StartAnimation();
			break;
		}
		default:
			break;
		}
		break;
	}
	case BossIncoming:
	{
		std::wstring s;
		s.assign(data->s.begin(), data->s.end());

		m_BossIncoming->SetText(s);
		m_BossIncoming->RenderActive(true);
		m_BossIncomingTimer = 5.0f;

		m_BossIncoming->SetScreenPos(GlobalDef::windowWidth * 0.5f - s.size() * 24 * (GlobalDef::windowHeight / 1080.f), m_BossIncoming->GetScreenPosY());
		break;
	}
	default:
		break;
	}
}

void CGamePlayHUD::ShowWin()
{

	WinLoseHUD->SetVisibility(true);
	for (unsigned int i = 0; i < guiContainer.size(); i++)
		renderGUI->removeGUIElement(guiContainer[i]);
	//
	//
	//renderGUI->AddGUIElement(guiScoreBoardBackground);
	//renderGUI->AddGUIElement(guiMostGemsCollectedText);
	//renderGUI->AddGUIElement(guiMostEnemiesKilledText);
	//renderGUI->AddGUIElement(guiMostTowersText);
	//renderGUI->AddGUIElement(guiMostCandyReturnedText);
	//renderGUI->AddGUIElement(guiMostDeathsText);
	//renderGUI->AddGUIElement(guiMostAccurateText);
	//
	//renderGUI->AddGUIElement(guiYouWin);
	//renderGUI->AddGUIElement(guiExitButton);
	//
	for (unsigned int i = 0; i < frameContainer.size(); i++)
	{
		frameContainer[i]->SetVisibility(false);
	}
	bShowCrossHair = false;

	CompanionHUD->SetVisibility(false);
	PlayerUpgradeHUD->SetVisibility(false, true);
}

void CGamePlayHUD::ShowLose()
{
	WinLoseHUD->SetVisibility(true, false);
	for (unsigned int i = 0; i < guiContainer.size(); i++)
		renderGUI->removeGUIElement(guiContainer[i]);

	//renderGUI->AddGUIElement(guiScoreBoardBackground);
	//renderGUI->AddGUIElement(guiMostGemsCollectedText);
	//renderGUI->AddGUIElement(guiMostEnemiesKilledText);
	//renderGUI->AddGUIElement(guiMostTowersText);
	//renderGUI->AddGUIElement(guiMostCandyReturnedText);
	//renderGUI->AddGUIElement(guiMostDeathsText);
	//renderGUI->AddGUIElement(guiMostAccurateText);
	//
	//renderGUI->AddGUIElement(guiYouLose);
	//renderGUI->AddGUIElement(guiExitButton);
	for (unsigned int i = 0; i < frameContainer.size(); i++)
	{
		frameContainer[i]->SetVisibility(false);
	}
	bShowCrossHair = false;

	CompanionHUD->SetVisibility(false);
	PlayerUpgradeHUD->SetVisibility(false, true);
}

void CGamePlayHUD::UpdateFontEffect(float _dt)
{
	std::map<CGUIElement*, FontEffect>::iterator it = fontEffectContainer.begin();

	for (; it != fontEffectContainer.end(); it++)
	{
		if ((it->second.fontTimer += _dt) < it->second.fontEffectDuration)
		{
			float ratio = it->second.fontTimer / it->second.fontEffectDuration;
			float size = it->second.maxFontSize - it->second.fontSize;
			float finalSize = it->second.fontSize + ratio *size;
			it->first->SetFontSize(DirectX::XMFLOAT2(finalSize, finalSize));
		}
		else if (it->second.fontTimer > it->second.fontEffectDuration && it->second.bFontIncrese)
		{
			it->second.bFontIncrese = false;
			it->first->SetFontSize(DirectX::XMFLOAT2(it->second.fontSize, it->second.fontSize));
		}
	}
}

void CGamePlayHUD::UpdateIconEffect(float _dt)
{
	std::map<CGUIElement*, IconScaleEffect>::iterator it = iconEffectContainer.begin();

	for (; it != iconEffectContainer.end(); it++)
	{
		if ((it->second.scaleTimer += _dt) < it->second.scaleEffectDuration)
		{
			float ratio = it->second.scaleTimer / it->second.scaleEffectDuration;
			float size = it->second.maxScale - it->second.scale;
			float finalSize = it->second.scale + ratio *size;
			it->first->SetScale(finalSize);
		}
		else if (it->second.scaleTimer > it->second.scaleEffectDuration && it->second.bScaleIncrese)
		{
			it->second.bScaleIncrese = false;
			it->first->SetScale(it->second.scale);
		}
	}
}

bool CGamePlayHUD::GetExitButtonPressed(bool PauseMenu, bool OptionsMenu)
{
	if (PauseMenu)
		return PauseMenuHUD->GetExitButtonPressed();
	else
		return WinLoseHUD->GetExitButtonPressed();
}

bool CGamePlayHUD::GetResumeButtonPressed()
{
	return PauseMenuHUD->GetResumeButtonPressed();
}
bool CGamePlayHUD::GetMusicLessButtonPressed()
{
	return PauseMenuHUD->GetMusicLessButtonPressed();
}
bool CGamePlayHUD::GetMusicMoreButtonPressed()
{
	return PauseMenuHUD->GetMusicMoreButtonPressed();
}
bool CGamePlayHUD::GetSELessButtonPressed()
{
	return PauseMenuHUD->GetSELessButtonPressed();
}
bool CGamePlayHUD::GetSEMoreButtonPressed()
{
	return PauseMenuHUD->GetSEMoreButtonPressed();
}

void CGamePlayHUD::SetScoreText(std::string _display, std::string _name, int _score, CGUIElement * _element)
{
	int l = 0;

	std::string s;

	s.append(_display);
	l = s.size();
	for (; l < 21; l++)
		s.append(" ");

	s.append(_name);

	l = _name.size();
	for (; l < 14; l++)
		s.append(" ");
	s.append(std::to_string(_score));

	std::wstring temp;
	temp.assign(s.begin(), s.end());
	_element->SetText(temp);
}

bool CGamePlayHUD::PauseMenuOpen()
{
	return PauseMenuHUD->PauseMenuOpen();
}

void CGamePlayHUD::TogglePauseMenu()
{
	PauseMenuHUD->Show(!PauseMenuHUD->PauseMenuOpen());
}

bool CGamePlayHUD::UpgradeMenuOpen()
{
	return TowerUpgradeHUD->GetDontClose();
}

void CGamePlayHUD::UpdatePlayerUpgrade(EventID ID, TEventData const * Data)
{
	PlayerUpgradeHUD->HandleEvent(ID, Data);
}

void CGamePlayHUD::ToggleUpgradeMenu()
{
	PlayerUpgradeHUD->SetVisibility(!PlayerUpgradeHUD->IsOpen());
}

bool CGamePlayHUD::PlayerUpgradeOpen()
{
	return PlayerUpgradeHUD->IsOpen();
}


void CGamePlayHUD::SendEvent(EventID ID, TEventData & Data)
{
	Data.i = CGame::GetInstance()->GetMyPlayerID();
	CGame::GetInstance()->GetClient()->SendEventMessage(ID, Data);
}

void CGamePlayHUD::UpdateHealthOnHUD()
{
	healthString.clear();
	healthString.append(std::to_wstring(health));
	healthString.append(L"/");
	healthString.append(std::to_wstring(maxHealth));
	guiHealthRemaning->SetText(healthString);

	float healthRatio = (health / (float)maxHealth);
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.bottom = 512;
	rect.right = LONG(512 * healthRatio);

	guiHealthRedBar->SetScreenRec(rect);

	if (healthRatio < 0.50f)
	{
		DirectX::XMVECTOR color;
		color = DirectX::Colors::Red;
		color.m128_f32[0] *= 0.25f;
		color.m128_f32[1] *= 0.25f;
		color.m128_f32[2] *= 0.25f;
		color.m128_f32[3] = 0.5f - healthRatio;
		guiFullScreen->SetColor(color);
		guiFullScreen->RenderActive(true);
	}
	else
		guiFullScreen->RenderActive(false);
}