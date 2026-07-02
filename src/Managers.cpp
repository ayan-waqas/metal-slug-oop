#include "project.h"

// ========================== ENTITY MANAGER ==================
EntityManager::EntityManager() : currentPlayer(-1), totalBosses(0), totalBullets(0),
totalCollectibles(0), totalEnemies(0), totalPlayers(0), totalVehicles(0), totalWeapons(0),
damageZones(nullptr), pendingSpawns(nullptr), playerAlive(true), playerCount(0), zWasPressed(false),
finalBossWasSpawned(false), devMode(false), devToggleCooldown(0.0f), opWasPressed(false),
externalRewindActive(false), vWasPressed(false) {
}

void EntityManager::updateFinalBossTracker() {
    if (finalBossWasSpawned) return;
    for (int i = 0; i < entities.getSize(); i++) {
        if (entities[i] != nullptr && entities[i]->isFinalBoss()) {
            finalBossWasSpawned = true;
            return;
        }
    }
}

bool EntityManager::finalBossDefeated()  {
    if (!finalBossWasSpawned) return false;
    for (int i = 0; i < entities.getSize(); i++) {
        if (entities[i] != nullptr && entities[i]->isFinalBoss()) {
            return false;
        }
    }
    return true;
}
EntityManager::~EntityManager() {
    for (int i = 0; i < entities.getSize(); i++) {
        delete entities[i];
    }
    // damageZones is a pointer to vector owned by LevelManager — don't delete
}

void EntityManager::addEntity(Entity* entity) {
	entities.add(entity);
}

void EntityManager::checkSwitchingAndBehaviour() {
    if (devToggleCooldown > 0.0f) {
        devToggleCooldown -= deltaTime;
        if (devToggleCooldown < 0.0f) devToggleCooldown = 0.0f;
    }

    bool oNow = Keyboard::isKeyPressed(Keyboard::O);
    bool pNow = Keyboard::isKeyPressed(Keyboard::P);
    if (oNow && pNow) {
        if (!opWasPressed && devToggleCooldown <= 0.0f) {
            devMode = !devMode;
            for (int i = 0; i < playerCount; i++) {
                entities[i]->setDevMode(devMode);
            }
            devToggleCooldown = 0.5f;
            opWasPressed = true;
        }
    }
    else if (!oNow && !pNow) {
        opWasPressed = false;
    }

    if (Keyboard::isKeyPressed(Keyboard::Z) && !entities[currentPlayer]->inVehicle()) {
        if (!zWasPressed) {
            zWasPressed = true;
            switchToNextPlayer(entities[currentPlayer]->getLevel());
        }
    }
    else {
        zWasPressed = false;
    }

    if (Keyboard::isKeyPressed(Keyboard::V)) {
        if (!vWasPressed) {
            vWasPressed = true;
            PlayerSoldier* p = getCurrentPlayer();
            if (p != nullptr && p->canSpawnFusion()) {
                pendingSpawns->add(new Mahoraga(p->getX()-200, p->getY()-200, this, p->getLevel()));
                p->consumeFusion();
            }
        }
    }
    else {
        vWasPressed = false;
    }


    for (int i = entities.getSize() - 1; i >= 0; i--) {
        // for player slots, only run the active one
        if (i < playerCount) {
            if (i == currentPlayer && playerAlive[i])
                entities[i]->behaviour();
            // check if this player just died
            if (playerAlive[i] && entities[i]->shouldBeRemoved()) {
                playerAlive[i] = false;
                // auto switch to next alive player
                switchToNextPlayer(entities[currentPlayer]->getLevel(), true);
            }
        }
        else {
            if (entities[i]->isOffScreen()) continue;
            entities[i]->behaviour();
        }
    }
}

void EntityManager::enableDevMode() {
    devMode = true;
    for (int i = 0; i < playerCount; i++) {
        entities[i]->setDevMode(true);
    }
}



void EntityManager::runEntities() {
    if (externalRewindActive) return;
    drainPendingSpawns();
    updateFinalBossTracker();
    checkSwitchingAndBehaviour();
    processDamageZones();
    sweepDead();
    drawMahoragaOverlay();        // NEW
}

void EntityManager::drawMahoragaOverlay() {
    if (window == nullptr) return;
    bool mahoragaActive = false;
    for (int i = 0; i < entities.getSize(); i++) {
        Entity* e = entities[i];
        if (e == nullptr) continue;
        if (e->shouldBeRemoved()) continue;
        if (dynamic_cast<Mahoraga*>(e) != nullptr) {
            mahoragaActive = true;
            break;
        }
    }
    if (!mahoragaActive) return;

    sf::RectangleShape overlay(sf::Vector2f((float)screen_x, (float)screen_y));
    overlay.setPosition(0.0f, 0.0f);
    overlay.setFillColor(sf::Color(0, 0, 0, 140));  // black, ~55% opacity
    //window->draw(overlay);
}

void EntityManager::processDamageZones() {
    if (damageZones == nullptr) return;
    for (int z = 0; z < damageZones->getSize(); z++) {
        DamageZone* zone = (*damageZones)[z];
        for (int i = 0; i < entities.getSize(); i++) {
            if (zone->shouldBeRemoved()) break;
            entities[i]->receiveDamageZone(*zone);
        }
        zone->update();
    }
    for (int z = damageZones->getSize() - 1; z >= 0; z--) {
        if ((*damageZones)[z]->shouldBeRemoved()) {
            delete (*damageZones)[z];
            damageZones->remove(z);
        }
    }
}

void EntityManager::drainPendingSpawns() {
    if (pendingSpawns == nullptr) return;
    for (int i = 0; i < pendingSpawns->getSize(); i++) {
        entities.add((*pendingSpawns)[i]);
    }
    while (pendingSpawns->getSize() > 0) {
        pendingSpawns->remove(pendingSpawns->getSize() - 1);
    }
}

void EntityManager::drawDebug() {
}

void EntityManager::sweepDead() {
    for (int i = entities.getSize() - 1; i >= 0; i--) {
        if (i < playerCount)
            continue; //dont remove players yet
        if (entities[i]->shouldBeRemoved()) {
            delete entities[i];
            entities.remove(i);
        }
    }
}

void EntityManager::drawHUD() {
    PlayerSoldier* p = getCurrentPlayer();
    if (p != nullptr) p->drawHUD();
    ScoreManager::instance().draw();
}

void EntityManager::killFarAwayEnemies(float centerX, float radiusPx) {
    for (int i = 0; i < entities.getSize(); i++) {
        Entity* e = entities[i];
        if (e == nullptr) continue;
        if (!e->canBeKilledWhenFar()) continue;
        float dx = e->getX() - centerX;
        if (dx < 0) dx = -dx;
        if (dx > radiusPx) e->onKilledFromFar();
    }
}

void EntityManager::removeEntity(int index) {
	entities.remove(index);
}


void EntityManager::setCurrentPlayer(int index) {
	currentPlayer = index;
}


PlayerSoldier* EntityManager::getCurrentPlayer() {
	if (currentPlayer < 0 || currentPlayer >= entities.getSize()) {
		return nullptr;
	}
	return (PlayerSoldier*)entities[currentPlayer];
}

void EntityManager::spawnPlayers(Level* level) {
    int col = 4;
    Map* m = level->getMap();
    int cs = m->getCellSize();
    int row = level->surfaceRow(m->getLvl(), col);
    float px = (float)(col * cs);
    float py = (float)(row * cs) - 128.0f;

    addEntity(new Marco(px, py, level));
	addEntity(new Tarma(px, py, level));  
	addEntity(new Eri(px, py, level));
	addEntity(new Germi(px, py, level));
    playerCount = 4; // for now
	for (int i = 0; i < 4; i++) {
		if (i < playerCount) 
        	playerAlive[i] = true;
    
    	else 
        	playerAlive[i] = false;
    }
	totalPlayers = playerCount;
	zWasPressed = false;
    setCurrentPlayer(0);
}

void EntityManager::switchToNextPlayer(Level* level, bool forced) {
    // grab the previous player's position so a forced switch (after a death)
    // can drop the incoming player near where the previous one fell.
    float prevX = 0, prevY = 0;
    if (currentPlayer >= 0 && currentPlayer < playerCount) {
        prevX = entities[currentPlayer]->getX();
        prevY = entities[currentPlayer]->getY();
    }

    for (int i = 1; i <= playerCount; i++) {
        int next = (currentPlayer + i) % playerCount;
        if (playerAlive[next]) {
            currentPlayer = next;
            PlayerSoldier* np = (PlayerSoldier*)entities[currentPlayer];

            if (forced) {
                // came in because the previous player just ran out of lives.
                // park the new player at the death spot and run them through
                // the full respawn pipeline (hide -> reappear 5 cols behind
                // -> flicker invincibility) so they get a safe re-entry.
                np->setX(prevX);
                np->setY(prevY);
                np->respawn();
            }
            else {
                // voluntary swap (Z key). just teleport them to the previous
                // player's current spot with no respawn animation/invincibility.
                np->setX(prevX);
                np->setY(prevY);
            }

            level->setCurrentPlayer(np);
            return;
        }
    }
    // if no one else is alive we naturally stay on currentPlayer
}

bool EntityManager::allPlayersDead() const {
    for (int i = 0; i < playerCount; i++) {
        if (playerAlive[i]) return false;
    }
    return true;
}

void EntityManager::scanAndSpawnEnemies(Level* level)
{
    int mapCount = level->getMapCount();
    int cs = 0;

    for (int mi = 0; mi < mapCount; mi++) {
        Map* map = level->getMapAt(mi);
        if (map == nullptr) continue;
        char** lvl = map->getLvl();
        int H = map->getHeight();
        int W = map->getWidth();
        cs = map->getCellSize();
        long long originPx = map->getChunkOriginX() * cs;

        int colStart = (int)((level->getScreenLeft() - originPx - screen_x/4) / cs);
        int colEnd = (int)((level->getScreenRight() - originPx + screen_x/4) / cs);
        if (colStart < 0) colStart = 0;
        if (colEnd >= W) colEnd = W - 1;
        if (colEnd < 0 || colStart >= W) continue;

        for (int r = 0; r < H; r++) {
            for (int c = colStart; c <= colEnd; c++) {
                char tile = lvl[r][c];
                float spawnX = (float)(originPx + c * cs);
                float spawnY = (float)(r * cs) - 128.0f;

                if (tile == 'R') {
                    RebelSoldier* first = new RebelSoldier(spawnX, spawnY, level);
                    int batchSize = first->getBatchSize();
                    this->addEntity(first); totalEnemies++;
                    for (int i = 1; i < batchSize; i++) {
                        this->addEntity(new RebelSoldier(spawnX + i * 100.0f, spawnY, level));
                        totalEnemies++;
                    }
                    lvl[r][c] = ' ';
                }
                else if (tile == 'S') {
                    ShieldedSoldier* first = new ShieldedSoldier(spawnX, spawnY, level);
                    int batchSize = first->getBatchSize();
                    this->addEntity(first); totalEnemies++;
                    for (int i = 1; i < batchSize; i++) {
                        this->addEntity(new ShieldedSoldier(spawnX + i * 110.0f, spawnY, level));
                        totalEnemies++;
                    }
                    lvl[r][c] = ' ';
                }
                else if (tile == 'B') {
                    BazookaSoldier* first = new BazookaSoldier(spawnX, spawnY, level);
                    int batchSize = first->getBatchSize();
                    this->addEntity(first); totalEnemies++;
                    for (int i = 1; i < batchSize; i++) {
                        this->addEntity(new BazookaSoldier(spawnX + i * 100.0f, spawnY, level));
                        totalEnemies++;
                    }
                    lvl[r][c] = ' ';
                }
                else if (tile == 'G') {
                    GrenadeSoldier* first = new GrenadeSoldier(spawnX, spawnY, level);
                    int batchSize = first->getBatchSize();
                    this->addEntity(first); totalEnemies++;
                    for (int i = 1; i < batchSize; i++) {
                        this->addEntity(new GrenadeSoldier(spawnX + i * 100.0f, spawnY, level));
                        totalEnemies++;
                    }
                    lvl[r][c] = ' ';
                }
                else if (tile == 'M') {
                    MummyWarrior* first = new MummyWarrior(spawnX, spawnY, level);
                    int batchSize = first->getBatchSize();
                    this->addEntity(first); totalEnemies++;
                    for (int i = 1; i < batchSize; i++) {
                        this->addEntity(new MummyWarrior(spawnX + i * 100.0f, spawnY, level));
                        totalEnemies++;
                    }
                    lvl[r][c] = ' ';
                }
                else if (tile == 'Z') {
                    Zombie* first = new Zombie(spawnX, spawnY, level);
                    int batchSize = first->getBatchSize();
                    this->addEntity(first); totalEnemies++;
                    for (int i = 1; i < batchSize; i++) {
                        this->addEntity(new Zombie(spawnX + i * 100.0f, spawnY, level));
                        totalEnemies++;
                    }
                    lvl[r][c] = ' ';
                }
                else if (tile == 'P') {
                    int type = rand() % 4;
                    ParaTrooper* first = new ParaTrooper(spawnX, spawnY, level, type);
                    int batchSize = first->getBatchSize();
                    this->addEntity(first); totalEnemies++;
                    for (int i = 1; i < batchSize; i++) {
                        this->addEntity(new ParaTrooper(spawnX + i * 80.0f, spawnY, level, type));
                        totalEnemies++;
                    }
                    lvl[r][c] = ' ';
                }
                else if (tile == 'A') {
                    this->addEntity(new Martian(spawnX, spawnY, level));
                    totalEnemies++;
                    lvl[r][c] = ' ';
                }
				else if (tile == 'F'){ //flying tara
	                float spawnX = (float)(c * cs);
	                float spawnY = (float)(r * cs);
	                FlyingTara* first = new FlyingTara(spawnX, spawnY,  level);
	                int batchSize = first->getBatchSize();
	                this->addEntity(first);
	                for (int i = 1; i < batchSize; i++){
	                    this->addEntity(new FlyingTara(spawnX + i * 150.0f, spawnY,  level));
						totalEnemies++;
					}
	                lvl[r][c] = ' ';
				 }
				 else if (tile == 'V')
				 {
					 float spawnX = (float)(originPx + c * cs);
					 float spawnY = (float)(r * cs) - 128.0f;
					 this->addEntity(new Bradley(spawnX, spawnY,  level));
					 lvl[r][c] = ' ';
				 }

                else if (tile == 'I') {
                    this->addEntity(new IronNokana(spawnX, spawnY, level));
                    totalBosses++;
                    lvl[r][c] = ' ';
                }
                else if (tile == 'H') {
                    this->addEntity(new HairBuster(spawnX, spawnY, level));
                    totalBosses++;
                    lvl[r][c] = ' ';
                }
                else if (tile == 'E') {
                    this->addEntity(new SeaSatan(spawnX, spawnY, level));
                    totalBosses++;
                    lvl[r][c] = ' ';
                }
                else if (tile == 'U') {
                    // only spawn UltimateBoss once its tile is on-screen
                    int sl = level->getScreenLeft();
                    int sr_ = level->getScreenRight();
                    int st = level->getScreenTop();
                    int sb = level->getScreenBottom();
                    if (spawnX + cs >= sl && spawnX <= sr_ &&
                        spawnY + cs >= st && spawnY <= sb) {
                        this->addEntity(new UltimateBoss(spawnX, spawnY, level));
                        totalBosses++;
                        lvl[r][c] = ' ';
                    }
                }
                else if (tile == 'L') {
                    this->addEntity(new MetalSlug(spawnX, spawnY, level));
                    totalVehicles++;
                    lvl[r][c] = ' ';
                }
                else if (tile == 'Y') {
                    this->addEntity(new SlugFlyer(spawnX, spawnY, level));
                    totalVehicles++;
                    lvl[r][c] = ' ';
                }
                else if (tile == 'Q') {
                    this->addEntity(new SlugMariner(spawnX, spawnY, level));
                    totalVehicles++;
                    lvl[r][c] = '~';
                }
                else if (tile == 'C') {
                    this->addEntity(new AmphibiousSlug(spawnX, spawnY, level));
                    totalVehicles++;
                    lvl[r][c] = ' ';
                }
                else if (tile == 'N')
                {
                    this->addEntity(new EnemySub(spawnX, spawnY, level));
                    totalEnemies++;
                    lvl[r][c] = ' ';
                }
				else if (tile == '2') {
                    this->addEntity(new Prisonor(spawnX, spawnY, level));
                    totalCollectibles++;
                    lvl[r][c] = ' ';
                }
            }
        }
    }
}


// ==================== LEVEEL MANAGER ============================
LevelManager::LevelManager() : currentLevel(nullptr) {
}

void LevelManager::generateLevel() {
    currentLevel->generateMap();
}

void LevelManager::RenderLevel() {
    currentLevel->renderScreen();
}

Level* LevelManager::getCurrentLevel() {
    return currentLevel;
}


LevelManager::LevelManager(Level* level) {
    this->currentLevel = level;
}

void LevelManager::setLevel(Level* level) {
    if ( currentLevel!=nullptr) delete currentLevel;
    this->currentLevel = level;
}

LevelManager::~LevelManager() {
    if (currentLevel != nullptr) delete currentLevel;
    for (int i = 0; i < damageZones.getSize(); i++) {
        delete damageZones[i];
    }
    for (int i = 0; i < pendingSpawns.getSize(); i++) {
        delete pendingSpawns[i];
    }
}




// FORKS OF FATE
States::States() : id(-1), x(0), y(0) {}    // -1 id means "not yet assigned"

PlayerState::PlayerState() : HP(0), lives(0), activeWeaponIndex(0), activeWeaponAmmo(0), vehicleID(-1), immortal(false), dead(false) {}

EnemyState::EnemyState() : HP(0), dead(false) {}

BossState::BossState() : HP(0), currentWeapon(0), dead(false) {}

VehicleState::VehicleState() : HP(0), occupiedByPlayerID(-1), dead(false) {}

BulletState::BulletState() : angle(0), active(false) {}

CurrentState::CurrentState() : timestamp(0), playerCount(0), activePlayerIndex(0),
enemyCount(0), bossCount(0), vehicleCount(0), bulletCount(0),
screenLeft(0), screenRight(0), screenTop(0), screenBottom(0) {
}

ForksOfFateFileHandler::ForksOfFateFileHandler() : open(false) {}

ForksOfFateFileHandler::~ForksOfFateFileHandler() {
    if (open) out.close();
}

void ForksOfFateFileHandler::openTimeline(const char* name) {
    if (open) out.close();                          // close any leftover from previous run
    out.open(name, ios::binary | ios::trunc);       // binary mode, wipe contents
    open = out.is_open();
}

void ForksOfFateFileHandler::closeTimeline() {
    if (open) {
        out.close();
        open = false;
    }
}

void ForksOfFateFileHandler::appendSnapshot(const CurrentState& s) {
    if (!open) return;
    out.write((char*)&s, sizeof(CurrentState));    // dump raw struct bytes to file
}

bool ForksOfFateFileHandler::readSnapshotAt(const char* name, int index, CurrentState& destination) {
    ifstream in(name, ios::binary);
    if (!in.is_open()) return false;
    in.seekg(index * sizeof(CurrentState));        // jump to the start of snapshot #index
    in.read((char*)&destination, sizeof(CurrentState));    // read one struct's worth into dst
    return in.good();                              // true if read worked
}

int ForksOfFateFileHandler::snapshotCount(const char* name) {
    ifstream in(name, ios::binary | ios::ate);     // ios::ate opens at the end so tellg gives file size
    if (!in.is_open()) return 0;
    int count = (int)in.tellg() / sizeof(CurrentState);
    return count;
}


// STATE BUFFER==========

StatesBuffer::StatesBuffer(ForksOfFateFileHandler* fh) : writeHead(0), filled(0), fileHandler(fh) {}

void StatesBuffer::push(const CurrentState& s) {
    ring[writeHead] = s;                           // overwrite oldest slot
    writeHead = (writeHead + 1) % 600;             // wrap around at 600
    if (filled < 600) filled++;                    // cap at 600 so we know how much is real data
    if (fileHandler) fileHandler->appendSnapshot(s);   // also persist to disk
}

const CurrentState& StatesBuffer::peekRelative(int framesAgo) const {
    int idx = writeHead - 1 - framesAgo;           // newest is one slot before writeHead
    while (idx < 0) idx += 600;                    // wrap negative back into range
    return ring[idx];
}


//FORKS OF FATE MANAGER=======================================
ForksOfFateManager::ForksOfFateManager() : buffer(0), entityManager(0), isReversing(false), rewindCursor(0), runTimer(0) {
    buffer = new StatesBuffer(&fileHandler);       // ring buffer needs the file handler so it can forward each push
}

ForksOfFateManager::~ForksOfFateManager() {
    delete buffer;
    fileHandler.closeTimeline();
}

void ForksOfFateManager::setEntityManager(EntityManager* em) {
    entityManager = em;
}

void ForksOfFateManager::beginSession(const char* timelineFile) {
    fileHandler.openTimeline(timelineFile);
    runTimer = 0;
}

void ForksOfFateManager::endSession() {
    fileHandler.closeTimeline();
}

void ForksOfFateManager::takeSnapshot() {
    if (!entityManager) return;
    runTimer += deltaTime;                         // tracks total elapsed time
    CurrentState s;                                // constructor zeros all the count fields
    s.timestamp = runTimer;

    MyVector<Entity*>& ents = entityManager->getEntities();
    for (int i = 0; i < ents.getSize(); i++) {
        ents[i]->writeStateTo(s);                  // each entity puts itself into the right slot
    }

    PlayerSoldier* curr = entityManager->getCurrentPlayer();
    if (curr) s.activePlayerIndex = curr->getId();   // store id of currently active player

    Level* lvl = (curr ? curr->getLevel() : nullptr);   // grab camera bounds from level for rewind
    if (lvl) {
        s.screenLeft = lvl->getScreenLeft();
        s.screenRight = lvl->getScreenRight();
        s.screenTop = lvl->getScreenTop();
        s.screenBottom = lvl->getScreenBottom();
    }

    buffer->push(s);
}

void ForksOfFateManager::startRewind() {
    if (isReversing) return;
    isReversing = true;
    rewindCursor = 0;       // start from newest snapshot, walk backward
}

void ForksOfFateManager::stepRewind() {
    if (!isReversing || !entityManager) return;
    if (rewindCursor >= buffer->available()) {     // ran out of history
        stopRewind();
        return;
    }
    const CurrentState& s = buffer->peekRelative(rewindCursor);
    MyVector<Entity*>& ents = entityManager->getEntities();
    for (int i = 0; i < ents.getSize(); i++) {
        ents[i]->applyStateFrom(s);                          // restore old state
        if (ents[i]->shouldDrawDuringRewind()) {
            ents[i]->setAndDraw();                           // and draw if appropriate
        }
    }
    rewindCursor++;
}



void ForksOfFateManager::stopRewind() {
    isReversing = false;
    rewindCursor = 0;
}









// ===================== SCORE MANAGER =====================

ScoreManager::ScoreManager()
    : score(0),
    killCount(0),
    campaignKillQuota(50),
    campaignClearAwarded(false),
    survivalClearAwarded(false),
    nextExplosionZoneId(1),
    explSlotsUsed(0),
    currentExplosionZoneId(0)
{
    for (int i = 0; i < 128; i++) {
        explKillIds[i] = 0;
        explKillCount[i] = 0;
        explFlawlessGiven[i] = false;
    }
}

ScoreManager g_scoreManager;

ScoreManager& ScoreManager::instance() {
    return g_scoreManager;
}

void ScoreManager::reset() {
    score = 0;
    killCount = 0;
    campaignClearAwarded = false;
    survivalClearAwarded = false;
    nextExplosionZoneId = 1;
    explSlotsUsed = 0;
    currentExplosionZoneId = 0;
    for (int i = 0; i < 128; i++) {
        explKillIds[i] = 0;
        explKillCount[i] = 0;
        explFlawlessGiven[i] = false;
    }
}

void ScoreManager::addScore(int amount) {
    score += amount;
}

void ScoreManager::registerKill(int amount) {
    score += amount;
    killCount += 1;
}

int ScoreManager::findOrAllocSlot(int zoneId) {
    for (int i = 0; i < explSlotsUsed; i++) {
        if (explKillIds[i] == zoneId) return i;
    }
    int slot;
    if (explSlotsUsed < 128) {
        slot = explSlotsUsed;
        explSlotsUsed += 1;
    }
    else {
        for (int i = 0; i < 127; i++) {
            explKillIds[i] = explKillIds[i + 1];
            explKillCount[i] = explKillCount[i + 1];
            explFlawlessGiven[i] = explFlawlessGiven[i + 1];
        }
        slot = 127;
    }
    explKillIds[slot] = zoneId;
    explKillCount[slot] = 0;
    explFlawlessGiven[slot] = false;
    return slot;
}

void ScoreManager::registerExplosionKill(int zoneId, int amount) {
    registerKill(amount);
    if (zoneId <= 0) return;
    int slot = findOrAllocSlot(zoneId);
    explKillCount[slot] += 1;
    if (explKillCount[slot] >= 2 && !explFlawlessGiven[slot]) {
        explFlawlessGiven[slot] = true;
        score += SCORE_FLAWLESS;
    }
}

void ScoreManager::noteEnemyKill(int amount) {
    int before = score;
    if (currentExplosionZoneId > 0) {
        registerExplosionKill(currentExplosionZoneId, amount);
    }
    else {
        registerKill(amount);
    }
}

int ScoreManager::allocateExplosionZoneId() {
    int id = nextExplosionZoneId;
    nextExplosionZoneId += 1;
    if (nextExplosionZoneId <= 0) nextExplosionZoneId = 1;
    return id;
}

void ScoreManager::awardSurvivalClear() {
    if (survivalClearAwarded) return;
    survivalClearAwarded = true;
    score += SCORE_SURVIVAL_CLEAR;
}

void ScoreManager::awardCampaignClear() {
    if (campaignClearAwarded) return;
    if (killCount < campaignKillQuota) return;
    campaignClearAwarded = true;
    score += SCORE_CAMPAIGN_CLEAR;
}

void ScoreManager::draw() {
    if (window == nullptr) return;

    static sf::Text label;
    static bool initialized = false;
    if (!initialized) {
        label.setFont(scoreFont);
        label.setCharacterSize(36);
        label.setStyle(sf::Text::Bold);
        label.setFillColor(sf::Color(255, 230, 80));
        label.setOutlineThickness(0.0f);     // NO outline — outline kills the cache
        initialized = true;
    }

    MyString text = "SCORE: ";
    text += MyString::fromInt(score);
    label.setString(text.c_str());

    sf::View prevView = window->getView();
    window->setView(window->getDefaultView());

    sf::FloatRect bounds = label.getLocalBounds();
    float posX = ((float)screen_x - bounds.width) * 0.5f - bounds.left;
    float posY = 20.0f;
    
    label.setPosition(200, posY);

    window->draw(label);

    window->setView(prevView);
}