#include "project.h"

sf::Music bgMusic;
sf::SoundBuffer sfxBuffers[20];
sf::Sound sfxSounds[20];
void playSFX(int id) {
	if (id < 0 || id >= 20) return;
	sfxSounds[id].setBuffer(sfxBuffers[id]);
	sfxSounds[id].play();
}

void playMusic(const char* path, bool loop) {
	bgMusic.stop();
	if (bgMusic.openFromFile(path)) {
		bgMusic.setLoop(loop);
		bgMusic.setVolume(60.f);
		bgMusic.play();
	}
}

// Load all SFX once at startup — call this in Game::Game()
void loadAllAudio() {
	sfxBuffers[SFX_SHOOT].loadFromFile("Sounds/pistol-shoot.mp3");
	sfxBuffers[SFX_EXPLOSION].loadFromFile("Sounds/explosion.mp3");
	sfxBuffers[SFX_PICKUP].loadFromFile("Sounds/pickup.wav");
	sfxBuffers[SFX_PLAYER_HIT].loadFromFile("Sounds/player_hit.wav");
	sfxBuffers[SFX_ENEMY_DIE].loadFromFile("Sounds/enemy_die.wav");
	sfxBuffers[SFX_KNIFE].loadFromFile("Sounds/knife.wav");
	sfxBuffers[SFX_RELOAD].loadFromFile("Sounds/reload.wav");
	sfxBuffers[SFX_GRENADE_BOUNCE].loadFromFile("Sounds/grenade.wav");
	sfxBuffers[SFX_MENU_SELECT].loadFromFile("Sounds/menu_select.wav");
	sfxBuffers[SFX_MENU_MOVE].loadFromFile("Sounds/menu_move.wav");
	sfxBuffers[SFX_FIRE].loadFromFile("Sounds/fire.mp3");
	sfxBuffers[SFX_SLASH].loadFromFile("Sounds/slash.mp3");
}

 //================== GAME =====================
//Game::Game() : window(VideoMode(screen_x, screen_y), "Metal Slug", Style::Close), stateManager(new GameStateManager()) {
//	window.setVerticalSyncEnabled(true);
//	window.setFramerateLimit(60);
//	::window = &this->window;
//	::hudFont.loadFromFile("Sprites/hud2.ttf");
//	loadAllAudio();
//	playMusic("Sounds/main-menu.ogg");
//}

Game::Game() : window(VideoMode::getDesktopMode(), "Metal Slug", Style::Fullscreen), stateManager(new GameStateManager()) {
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	::window = &this->window;
	::hudFont.loadFromFile("Sprites/hud2.ttf");
	::scoreFont.loadFromFile("Sprites/hud2.ttf");
	loadAllAudio();
	playMusic("Sounds/main-menu.ogg");
}


Game::~Game() {
	if (stateManager != nullptr)
	{
		delete stateManager;
	}
}

void Game::run() {
	Clock clock;
	while (window.isOpen()) {
		::deltaTime = clock.restart().asSeconds();
		if (::deltaTime > 0.05f) 
			::deltaTime = 0.05f; // cap deltaTime to prevent extreme values
		Event ev;
		while (window.pollEvent(ev)) {
			if (ev.type == Event::Closed)
				window.close();
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		window.clear();

		stateManager->run();

		window.display();
	}
}

// ========================= GAMESTATEMANAGER =======================
GameStateManager::GameStateManager() : currentGameMode(0), currentState(nullptr) {
}

void GameStateManager::setGameState(int gameMode) {
	if (gameMode == 1 || gameMode == 2 || gameMode == 3 || gameMode == 4) {
		if (currentState != nullptr) delete currentState;
		currentState = new Survival(gameMode);
	}
	else if (gameMode == 5) {
		if (currentState != nullptr) delete currentState;
		currentState = new Campaign();
	}
	//else if (gameMode == 6) {
	//	currentState = new SelfPlay();
	//}
	else {
		if (currentState != nullptr) delete currentState;
		currentState = nullptr;
	}
}

GameStateManager::~GameStateManager() {
	if (currentState != nullptr) {
		delete currentState;
	}
}

void GameStateManager::run() {
	switch (currentGameMode) {

	case 0:
		// displays menu sprite then takes input then calls selectOption 
		mainMenu.displayMenu();
		currentGameMode = mainMenu.returnSelectedOption();
		setGameState(currentGameMode);

		break;

	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		currentState->runGame();
		if (currentState->getModeComplete()) {
			currentGameMode = 0;
			setGameState(0);
			mainMenu.selectOption(0);   // reset
		}
		break;

	case 6:
		break;
	}
}


// ======================= MAIN MENU ==============================
MainMenu::MainMenu() : selectedOption(0), detectedPopup(false), menu(new SfmlObject()), popup(new SfmlObject()) {
	// AYANNN ADD spriteS INTIALIZING
	menu->getTex().loadFromFile("Sprites/MainMenu.png");
	menu->setScaleX((float)screen_x / menu->getTex().getSize().x);
	menu->setScaleY((float)screen_y / menu->getTex().getSize().y);
	menu->initializeScale();
	menu->initializeDimensions();
}

void MainMenu::selectOption(int option) {
	selectedOption = option;
}

void MainMenu::displayMenu() {
	menu->centreSprite();
	window->draw(menu->getSprite());
	if (detectedPopup) {
		popup->centreSprite();
		window->draw(popup->getSprite());
	}

	if (Keyboard::isKeyPressed(Keyboard::Num1)) {
		selectOption(1);
	}
	if (Keyboard::isKeyPressed(Keyboard::Num2)) {
		selectOption(2);
	}
	if (Keyboard::isKeyPressed(Keyboard::Num3)) {
		selectOption(3);
	}
	if (Keyboard::isKeyPressed(Keyboard::Num4)) {
		selectOption(4);
	}
	if (Keyboard::isKeyPressed(Keyboard::Num5)) {
		selectOption(5);
	}
	if (Keyboard::isKeyPressed(Keyboard::Num6)) {
		selectOption(6);
	}
}

int MainMenu::returnSelectedOption() {
	return selectedOption;
}

MainMenu::~MainMenu() {
	if (menu!=nullptr) delete menu;
	if (popup!=nullptr) delete popup;
}




//===================GAMESTATE=====================

GameState::GameState() : modeComplete(false), developerMode(false), currentGameMode(false) {}

GameState::~GameState() {}

void GameState::activateDeveloperMode() {
	this->developerMode = true;
}

void GameState::deactivateDeveloperMode() {
	this->developerMode = false;
}

bool GameState::getDeveloperMode() {
	return this->developerMode;
}

void GameState::setModeComplete() {
	this->modeComplete = true;
}

bool GameState::getModeComplete() {
	return this->modeComplete;
}




//===================== PLAYSTATE ===================

PlayState::PlayState() : GameState(), entityManager(new EntityManager()), levelManager(new LevelManager()), forks(new ForksOfFateManager()), rWasPressed(false) {
	currentGameMode = 0;
	forks->setEntityManager(entityManager);
	forks->beginSession("timeline.bin");   // start writing snapshots to this file
}


PlayState::~PlayState() {
	if (entityManager != nullptr) delete this->entityManager;
	if (levelManager != nullptr) delete this->levelManager;
	if (forks != nullptr) delete forks;
}


// ===================== SURVIVAL =====================


Survival::Survival(int level) : currentLevel(level) {
	loadLevel(currentLevel);
}


void Survival::runGame() {
	levelManager->getCurrentLevel()->scrollLevel();
	levelManager->RenderLevel();

	bool rNow = Keyboard::isKeyPressed(Keyboard::T);
	if (rNow && !rWasPressed) {           // edge-trigger so holding R doesn't toggle every frame
		if (!forks->reversing()) forks->startRewind();
		else forks->stopRewind();
	}
	rWasPressed = rNow;

	entityManager->setRewindActive(forks->reversing());

	if (forks->reversing()) {
		forks->stepRewind();        // rewind drives the frame
	}
	else {
		entityManager->scanAndSpawnEnemies(levelManager->getCurrentLevel());
		entityManager->runEntities();
		forks->takeSnapshot();          // record the frame we just played
	}

	if (entityManager->isDevMode()) activateDeveloperMode();
	else deactivateDeveloperMode();

	entityManager->drawHUD();
	entityManager->drawDebug();

	Level* lvl = levelManager->getCurrentLevel();
	lvl->checkCompletion(entityManager);
	if (lvl->isLevelComplete()) {
		ScoreManager::instance().awardSurvivalClear();
		if (currentLevel >= 4) {
			setModeComplete();
		}
		else {
			currentLevel++;
			transitionToNextLevel();
		}
	}
}



void Survival::loadLevel(int n) {
	if (n == 1) ScoreManager::instance().reset();
	playMusic("Sounds/level.mp3");
	levelManager->setLevel(new SurvivalLevel(n == 4, n));
	levelManager->generateLevel();
	levelManager->getCurrentLevel()->setDamageZones(levelManager->getDamageZones());
	entityManager->setDamageZones(levelManager->getDamageZones());
	levelManager->getCurrentLevel()->setPendingSpawns(levelManager->getPendingSpawns());
	entityManager->setPendingSpawns(levelManager->getPendingSpawns());
	entityManager->spawnPlayers(levelManager->getCurrentLevel());
	levelManager->getCurrentLevel()->setCurrentPlayer(entityManager->getCurrentPlayer());
}

void Survival::transitionToNextLevel() {
	delete entityManager;
	delete levelManager;
	entityManager = new EntityManager();
	levelManager = new LevelManager();
	forks->setEntityManager(entityManager);    // re-point forks at the new EntityManager
	loadLevel(currentLevel);
	if (getDeveloperMode()) entityManager->enableDevMode();
}

Survival::~Survival() {
}





// CAMPAIGN=============================

Campaign::Campaign() : profileSelected(false), chosenProfile(0), profileMenu(new SfmlObject()) {
	currentGameMode = 5;
	profileMenu->getTex().loadFromFile("Sprites/profileMenu.png");
	profileMenu->setScaleX(1600.0f / 1297.0f);
	profileMenu->setScaleY(900.0f / 720.0f);
	profileMenu->initializeScale();
	profileMenu->initializeDimensions();
}

Campaign::~Campaign() {
	if (profileMenu != nullptr) delete profileMenu;
}

void Campaign::selectProfileScreen() {
	profileMenu->centreSprite();
	window->draw(profileMenu->getSprite());

	if (Keyboard::isKeyPressed(Keyboard::Num1)) {
		chosenProfile = 1;
		buildLevel();
	}
	else if (Keyboard::isKeyPressed(Keyboard::Num2)) {
		chosenProfile = 2;
		buildLevel();
	}
	else if (Keyboard::isKeyPressed(Keyboard::Num3)) {
		chosenProfile = 3;
		buildLevel();
	}
}

void Campaign::buildLevel() {
	ScoreManager::instance().reset();
	levelManager->setLevel(new InfiniteLevel(nullptr, chosenProfile));
	levelManager->generateLevel();
	levelManager->getCurrentLevel()->setDamageZones(levelManager->getDamageZones());
	entityManager->setDamageZones(levelManager->getDamageZones());
	levelManager->getCurrentLevel()->setPendingSpawns(levelManager->getPendingSpawns());
	entityManager->setPendingSpawns(levelManager->getPendingSpawns());
	entityManager->spawnPlayers(levelManager->getCurrentLevel());
	PlayerSoldier* p = entityManager->getCurrentPlayer();
	levelManager->getCurrentLevel()->setCurrentPlayer(p);

	long long col = (long long)(p->getX() / cell_size);
	int h = levelManager->getCurrentLevel()->getHeight(col);
	int surfaceRow = 40 - h;
	p->setY((float)(surfaceRow * cell_size - 200));

	profileSelected = true;

	if (getDeveloperMode()) entityManager->enableDevMode();
}

void Campaign::runGame() {
	if (!profileSelected) {
		selectProfileScreen();
		return;
	}
	levelManager->getCurrentLevel()->scrollLevel();

	levelManager->RenderLevel();

	entityManager->scanAndSpawnEnemies(levelManager->getCurrentLevel());

	entityManager->killFarAwayEnemies(entityManager->getCurrentPlayer()->getX(), 50.0f * levelManager->getCurrentLevel()->getMapAt(0)->getCellSize());

	entityManager->runEntities();

	if (entityManager->isDevMode()) activateDeveloperMode();
	else deactivateDeveloperMode();

	if (ScoreManager::instance().campaignQuotaReached()) {
		ScoreManager::instance().awardCampaignClear();
	}
	entityManager->drawHUD();
	entityManager->drawDebug();
}









//-----------animation-----------------
Animation::Animation(int totalFrames, float frameInterval , bool customFrames) : totalFrames(totalFrames), currentFrame(0), frameTime(0), frameInterval(frameInterval), isComplete(false), active(false), scaleX(1),scaleY(0) ,useCustomFrames(customFrames){}
void Animation::reset() {
	currentFrame = 0;
	frameTime = 0;
	isComplete = false;
}

bool Animation::getIsComplete() {
	return this -> isComplete;
}
void Animation::loadTexture(const char* path) {
	tex.loadFromFile(path);
}
Texture& Animation::getTexture() {
	return tex;
}
IntRect Animation::getRect() {
	if(useCustomFrames)
		return frames[currentFrame]; //no rect needed cuz custom frames already 
	int frameWidth = tex.getSize().x / totalFrames;
	int frameHeight = tex.getSize().y ;
	return IntRect(currentFrame * frameWidth,0,frameWidth,frameHeight);
}
bool Animation::isActive() const {
	return active;
}
void Animation::setActive(bool val) {
	active = val;
}
void Animation::setScale(float x, float y) {
	scaleX = x;
	scaleY = y;
}
float Animation::getScaleX() const {
	return scaleX;
}
float Animation::getScaleY() const {
	return scaleY;
}
// ============= LOOP ANIMATION =============
LoopAnimation::LoopAnimation(int totalFrames, float frameInterval, bool customFrames): Animation(totalFrames, frameInterval, customFrames) {}
void LoopAnimation::update() {
	frameTime += ::deltaTime;
	if (frameTime >= frameInterval) {
		currentFrame++;
		if (currentFrame >= totalFrames)
			currentFrame = 0;      // loops back, never completes
		frameTime = 0;
	}
}
// ============= ONESHOT ANIMATION =============
OneShotAnimation::OneShotAnimation(int totalFrames, float frameInterval, bool customFrames): Animation(totalFrames, frameInterval, customFrames) {}
void OneShotAnimation::update() {
	if (isComplete) 
		return;        // already done, dont advance
	frameTime += ::deltaTime;
	if (frameTime >= frameInterval) {
		if (currentFrame < totalFrames - 1) {
			currentFrame++;        
		}
		else {
			isComplete = true;     // hit last frame
			active = false;        // deactivate after completion
		}
		frameTime = 0;
	}
}
void Animation::setCustomFrames(IntRect* rects, int count) {
	count = count > 30 ? 30 : count; // limit to max 30 frames
	for (int i = 0; i < count; i++) {
		frames[i] = rects[i];
	}
	totalFrames = count;
	useCustomFrames = true;
}






