#include "project.h"


// ===== EnemyAIState =====

void EnemyAIState::movement(Enemy* enemy)
{
    PlayerSoldier* player = enemy->getLevel()->getCurrentPlayer();
    if (player == nullptr) return;

    // check distance to player
    float dist = enemy->calculateDistance(player);

    // update alerted state
    if (dist <= enemy->getDetectionRadius() && (rand()% 100 < 3)) { 
        enemy->setAlerted(true);
    }
    else {
        enemy->setAlerted(false);
    }

    enemy->updateTileChecks(enemy->getX(), enemy->getY());

    if (enemy->isAlerted())
    {
        enemy->setVelocityX(0);
        if (player->getX() > enemy->getX()) enemy->setFacingRight(true);
        else enemy->setFacingRight(false);
    }
    else
    {
        // if currently pausing before a direction flip, just tick down and stand still
        if (enemy->getPatrolPauseTimer() > 0.0f)
        {
            enemy->setPatrolPauseTimer(enemy->getPatrolPauseTimer() - 1.0f / 60.0f);
            enemy->setVelocityX(0);
            enemy->entityGravity();
            return;
        }

        // patrol
        bool blockedRight = enemy->horizontalCollisionCheck(' ', enemy->getMidRight(), enemy->getBottomRight());
        bool blockedLeft = enemy->horizontalCollisionCheck(' ', enemy->getMidLeft(), enemy->getBottomLeft());

        if (blockedRight || blockedLeft)
        {
            enemy->setVelocityY(-16);
        }

        if (rand() % 100 < 3) // 3% chance to randomly change direction
        {
            enemy->setPatrolPauseTimer(1.0f);   // same pause for random flips too
            enemy->setPatrolDirection(-enemy->getPatrolDirection());
            enemy->setVelocityX(0);
            enemy->entityGravity();
            return;
        }

        enemy->setVelocityX(enemy->getPatrolDirection() * enemy->getMaxSpeed());
        enemy->setFacingRight(enemy->getPatrolDirection() > 0);
        enemy->setX(enemy->getX() + enemy->getVelocityX());
    }

    enemy->entityGravity();
    if (enemy->getWeapon() != nullptr) enemy->getWeapon()->update();
    enemy->clampToMap();
}

void EnemyAIState::attack(Enemy* enemy)
{
    if (!enemy->isAlerted()) return;
    if (enemy->getWeapon() == nullptr) return;

    PlayerSoldier* player = enemy->getLevel()->getCurrentPlayer();
    if (player == nullptr) return;

        enemy->getWeapon()->fire();
}

//==========REBEL

void RebelAIState::movement(Enemy* enemy) {
    EnemyAIState::movement(enemy);
}

void RebelAIState::attack(Enemy* enemy) {
    EnemyAIState::attack(enemy);
}


// ===== Shielded=================

void ShieldedAIState::movement(Enemy* enemy) {
    EnemyAIState::movement(enemy);
}

void ShieldedAIState::attack(Enemy* enemy) {
    EnemyAIState::attack(enemy);
}

// baki , lol
void BazookaAIState::movement(Enemy* e) { 
    EnemyAIState::movement(e); 
}
void BazookaAIState::attack(Enemy* e) { 
    EnemyAIState::attack(e); 
}

void GrenadeAIState::movement(Enemy* e) {
    EnemyAIState::movement(e); 
}
void GrenadeAIState::attack(Enemy* e) { 
    EnemyAIState::attack(e); 
}

void MummyAIState::movement(Enemy* e) { 
    EnemyAIState::movement(e); 
}
void MummyAIState::attack(Enemy* e) { 
    EnemyAIState::attack(e); 
}

void ZombieAIState::movement(Enemy* e) { 
    EnemyAIState::movement(e); 
}
void ZombieAIState::attack(Enemy* e) { 
    EnemyAIState::attack(e); 
}

void MartianGroundedAIState::movement(Enemy* enemy)
{
    if (!enemy->OnGround()) {
        enemy->entityGravity();
        return;
    }
    EnemyAIState::movement(enemy);

    // force alert based on distance, overrides the random 1% check in base
    PlayerSoldier* player = enemy->getLevel()->getCurrentPlayer();
    if (player != nullptr && enemy->calculateDistance(player) <= enemy->getDetectionRadius())
        enemy->setAlerted(true);
    enemy->clampToMap();
}


void MartianGroundedAIState::attack(Enemy* enemy)
{
    EnemyAIState::attack(enemy);   
}
void PodAIState::movement(Enemy* enemy)
{
    Martian* martian = (Martian*)enemy;

    // during destruction animation, freez the pod and the martian will fall
    if (martian->getTransitioning())
        return;

    PlayerSoldier* player = martian->getLevel()->getCurrentPlayer();
    if (player == nullptr) 
        return;

    float targetY = player->getY() - 3.0f * cell_size;
    float dyErr = targetY - martian->getY();
    if (dyErr > 4.0f) 
        martian->setY(martian->getY() + 4.0f);
    else if (dyErr < -4.0f) 
        martian->setY(martian->getY() - 4.0f);
    else                    
        martian->setY(martian->getY() + dyErr);

    float dx = player->getX() - martian->getX();
    if (dx > martian->getMaxSpeed()) 
        martian->setX(martian->getX() + martian->getMaxSpeed());
    else if (dx < -martian->getMaxSpeed()) 
        martian->setX(martian->getX() - martian->getMaxSpeed());
    else                                   
        martian->setX(martian->getX() + dx);

    martian->setFacingRight(player->getX() > martian->getX());
    martian->setAlerted(true);
    martian->clampToMap();
}

void PodAIState::attack(Enemy* enemy){
    Martian* martian = (Martian*)enemy;
    if (martian->getTransitioning())
        return;

    PlayerSoldier* player = martian->getLevel()->getCurrentPlayer();
    if (player == nullptr) 
        return;

    // only fire when above player
    float dx = player->getX() - martian->getX();
    if (dx < 0) 
        dx = -dx;
    if (dx > 4.0f * cell_size)
        return;

    if (martian->getWeapon() != nullptr)
        martian->getWeapon()->fire();
}

void ParatrooperAIState::movement(Enemy* enemy) { 
    PlayerSoldier* player = enemy->getLevel()->getCurrentPlayer();
    if (player == nullptr) return;

    // check distance to player
    float dist = enemy->calculateDistance(player);

    // update alerted state
    if (dist <= enemy->getDetectionRadius() && (rand() % 100 < 3)) {
        enemy->setAlerted(true);
    }
    else {
        enemy->setAlerted(false);
    }

    enemy->updateTileChecks(enemy->getX(), enemy->getY());

    if (enemy->isAlerted())
    {
        enemy->setVelocityX(0);
        if (player->getX() > enemy->getX()) enemy->setFacingRight(true);
        else enemy->setFacingRight(false);
    }
    else
    {
        // if currently pausing before a direction flip, just tick down and stand still
        if (enemy->getPatrolPauseTimer() > 0.0f)
        {
            enemy->setPatrolPauseTimer(enemy->getPatrolPauseTimer() - 1.0f / 60.0f);
            enemy->setVelocityX(0);

            float y = enemy->getY();
            enemy->entityGravity();
            enemy->setY(y + 1);
            return;
        }

        // patrol
        bool blockedRight = enemy->horizontalCollisionCheck(' ', enemy->getMidRight(), enemy->getBottomRight());
        bool blockedLeft = enemy->horizontalCollisionCheck(' ', enemy->getMidLeft(), enemy->getBottomLeft());

        if (blockedRight || blockedLeft)
        {
            enemy->setVelocityY(-16);
        }

        if (rand() % 100 < 3) // 3% chance to randomly change direction
        {
            enemy->setPatrolPauseTimer(1.0f);   // same pause for random flips too
            enemy->setPatrolDirection(-enemy->getPatrolDirection());
            enemy->setVelocityX(0);
            float y = enemy->getY();
            enemy->entityGravity();
            enemy->setY(y + 1);
            return;
        }

        enemy->setVelocityX(enemy->getPatrolDirection() * enemy->getMaxSpeed());
        enemy->setFacingRight(enemy->getPatrolDirection() > 0);
        enemy->setX(enemy->getX() + enemy->getVelocityX());
    }

    float y = enemy->getY();
    enemy->entityGravity();
    enemy->setY(y + 1);
    if (enemy->getWeapon() != nullptr) enemy->getWeapon()->update();
    enemy->clampToMap();
}
void ParatrooperAIState::attack(Enemy* e) {
    EnemyAIState::attack(e);
}


// ================Enemy==============
Enemy::Enemy(float x, float y, Level* levelSkeleton) : DamagableEntity(levelSkeleton),
aiState(nullptr), aggression(1.0f), weapon(nullptr), alerted(false), patrolDirection(1.0f), detectionRadius(400.0f), patrolPauseTimer(0.0f) {
    this->x = x;
    this->y = y;
    this->level = levelSkeleton;
    this->max_speed = 3.0f;      // add this
    this->acceleration = 0.5f;   // add this
}

void Enemy::writeStateTo(CurrentState& s) const {
    if (s.enemyCount >= 100) return;
    EnemyState& e = s.enemies[s.enemyCount];
    e.id = entityId;
    e.x = getX();
    e.y = getY();
    e.HP = HP;
    e.dead = dead;
    s.enemyCount++;
}

void Enemy::applyStateFrom(const CurrentState& s) {
    for (int i = 0; i < s.enemyCount; i++) {
        if (s.enemies[i].id != entityId) continue;
        const EnemyState& e = s.enemies[i];
        setX(e.x);
        setY(e.y);
        HP = e.HP;
        dead = e.dead;
        return;
    }
}

float Enemy::getPatrolPauseTimer() { return this->patrolPauseTimer; }
void Enemy::setPatrolPauseTimer(float val) { this->patrolPauseTimer = val; }

float Enemy::getAggression() {
    return aggression;
}

void Enemy::setWeapon(Weapon& weapon) {
    this->weapon = &weapon;
}

void Enemy::removeWeapon() {
    weapon = nullptr;
}

Weapon* Enemy::getWeapon() {
    return weapon;
}

void Enemy::movement() {
	aiState->movement(this);
}

void Enemy::updateAnimations() {
    // TODO: animation updates
}

void Enemy::attack() {
	aiState->attack(this);
}

void Enemy::setAggression(float val) {
	aggression = val;
}

void Enemy::setAlerted(bool val) { this->alerted = val; }
bool Enemy::isAlerted() { return this->alerted; }
float Enemy::getDetectionRadius() { return this->detectionRadius; }
float Enemy::getPatrolDirection() { return this->patrolDirection; }
void Enemy::setPatrolDirection(float val) { this->patrolDirection = val; }

Enemy::~Enemy()
{
    delete weapon;
    delete aiState;
}

// ===== RebelSoldier =====
RebelSoldier::RebelSoldier(float x, float y, Level* levelSkeleton) : Enemy(x, y, levelSkeleton), dying(false),killedByBlast(false) {
    float charW = 96.f;
    float charH = 128.f;

    this->level = levelSkeleton;
    this->HP = 2;
    delete aiState;
    this->aiState = new RebelAIState();
    this->weapon = new Pistol(this);
    
    // animation[0] — idle
    LoopAnimation* idle = new LoopAnimation(4, 1.0f / 4.0f, true);
    idle->loadTexture("Sprites/rebel-idle.png");
    idle->setActive(true);
	IntRect idleFrames[4] = { IntRect(2, 2, 58, 56), IntRect(62, 2, 59, 56), IntRect(123, 3, 57, 55), IntRect(182, 2, 58, 56)};
    idle->setCustomFrames(idleFrames, 4);
    idle->setScale(charW / 59.0f, charH / 56.0f);
    animations.add(idle);

    // animation[1] — walk
    LoopAnimation* walk = new LoopAnimation(6, 1.0f / 6.0f, true);
    walk->loadTexture("Sprites/rebel-walk.png");
    walk->setActive(false);
	IntRect walkFrames[6] = { IntRect(2, 2, 58, 62), IntRect(61, 2, 63, 62), IntRect(125, 3, 71, 53), IntRect(197, 5, 70, 55), IntRect(269, 3, 63, 59), IntRect(518, 3, 68, 51) };
    walk->setCustomFrames(walkFrames, 6);
    walk->setScale(charW / 71.0f, charH / 59.0f);
    animations.add(walk);

    // animation[2] — shoot
    OneShotAnimation* shoot = new OneShotAnimation(3, 1.0f / 8.0f, true);
    shoot->loadTexture("Sprites/rebel-shoot.png");
    shoot->setActive(false);
	IntRect shootFrames[3] = { IntRect(2, 13, 74, 63), IntRect(78, 13, 79, 63), IntRect(158, 13, 80, 63) };
    shoot->setCustomFrames(shootFrames, 3);
    shoot->setScale(charW / 79.0f, charH / 63.0f);
    animations.add(shoot);

    // animation[3] — normal death
    OneShotAnimation* death = new OneShotAnimation(8, 1.0f / 10.0f, true);
    death->loadTexture("Sprites/rebel-death.png");
    death->setActive(false);
    IntRect deathFrames[8] = {
        IntRect(5 , 0 , 62 , 67),
        IntRect(77 , 0 , 72 , 67),
        IntRect(159 , 1 , 74 , 66),
        IntRect(243 , 9 , 82 , 58),
        IntRect(335 , 29 , 80 , 38),
        IntRect(425 , 33 , 88 , 34),
        IntRect(523 , 37 , 90 , 30),
        IntRect(623 , 37 , 90 , 30),
    };
	death->setCustomFrames(deathFrames, 8);
    death->setScale(charW / 90.0f, charH / 58.0f);
    animations.add(death);

    // animation[4] — blast death
    OneShotAnimation* blastDeath = new OneShotAnimation(10, 1.0f / 10.0f, true);
    blastDeath->loadTexture("Sprites/rebel-death-blast.png");
    blastDeath->setActive(false);
    IntRect blastDeathFrames[10] = {
        IntRect(0 , 7 , 56 , 80),
        IntRect(70 , 5 , 58 , 82),
        IntRect(142 , 1 , 62 , 86),
        IntRect(218 , 3 , 68 , 84),
        IntRect(304 , 3 , 74 , 84),
        IntRect(396 , 0 , 84 , 87),
        IntRect(496 , 5 , 84 , 82),
        IntRect(596 , 3 , 94 , 84),
        IntRect(706 , 0 , 94 , 87),
        IntRect(820 , 0 , 94 , 87),
	};
	blastDeath->setCustomFrames(blastDeathFrames, 10);
    blastDeath->setScale(charW / 94.0f, charH / 87.0f);
    animations.add(blastDeath);

	this->width = charW;
	this->height = charH;
}

void RebelSoldier::receiveDamageZone(DamageZone& zone)
{
    if (!dying)
        if (zone.getPiercesShield())
            killedByBlast = true;
    DamagableEntity::receiveDamageZone(zone);
}

void RebelSoldier::changeHP(int change)
{
    HP += change;
    if (HP <= 0 && !dying)
    {
        HP = 0;
        dying = true;
        for (int i = 0; i < animations.getSize(); i++)
            animations[i]->setActive(false);
        int deathAnim = killedByBlast ? 4 : 3;
        animations[deathAnim]->setActive(true);
        animations[deathAnim]->reset();
        if (rand() % 100 < 20) {
            if (rand() % 2 == 0)
                level->queueSpawn(new Turkey(this->x, this->y, level));
            else
                level->queueSpawn(new Fruit(this->x, this->y, level));
        }
        ScoreManager::instance().noteEnemyKill(ScoreManager::SCORE_REBEL);
    }
}
void RebelSoldier::updateAnimations()
{
    // death 
    if (dying)
    {
        int d = killedByBlast ? 4 : 3;
        if (animations[d]->isActive())
        {
            animations[d]->update();
            this->Tex = animations[d]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[d]->getRect());
            float sx = animations[d]->getScaleX();
            float sy = animations[d]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            if (animations[d]->getIsComplete())
                dead = true;
        }
        return;
    }

    // shoot
    if (animations[2]->isActive())
    {
        animations[2]->update();
        this->Tex = animations[2]->getTexture();
        this->sprite.setTexture(this->Tex);
        this->sprite.setTextureRect(animations[2]->getRect());
        float sx = animations[2]->getScaleX();
        float sy = animations[2]->getScaleY();
        this->sprite.setScale(facingRight ? -sx : sx, sy);
        if (!animations[2]->isActive()) animations[2]->reset();
        return;
    }

    // walk vs idle
    if (velocityX != 0)
    {
        animations[0]->setActive(false);
        animations[0]->reset();
        animations[1]->setActive(true);
    }
    else
    {
        animations[1]->setActive(false);
        animations[1]->reset();
        animations[0]->setActive(true);
    }

    for (int i = 0; i <= 1; i++)
    {
        if (animations[i]->isActive())
        {
            animations[i]->update();
            this->Tex = animations[i]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[i]->getRect());
            float sx = animations[i]->getScaleX();
            float sy = animations[i]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            break;
        }
    }
}


void RebelSoldier::behaviour(){
    if (!dying){
        Enemy::movement();
        if (alerted) 
            Enemy::attack();
    }
    if (alerted && !dying)
    {
        Enemy::attack();
        animations[2]->setActive(true);
        animations[2]->reset();
    }
    updateAnimations();
    setAndDraw();
}
RebelSoldier::~RebelSoldier()
{
    delete this->aiState;
    delete this->weapon;
    this->aiState = nullptr;
    this->weapon = nullptr;
}




// ===== ShieldedSoldier =====

ShieldedSoldier::ShieldedSoldier(float x, float y, Level* levelSkeleton): Enemy(x, y, levelSkeleton),dying(false),killedByBlast(false)
{
	float charW = 96.f;
	float charH = 128.f;
    this->level = levelSkeleton;
    this->HP = 5;
    this->max_speed = 2.5f;
    this->acceleration = 0.5f;
    delete aiState;
    this->aiState = new ShieldedAIState();
    this->weapon = new Pistol(this);

    // animation[0] — idle
    LoopAnimation* idle = new LoopAnimation(3, 1.0f / 4.0f, true);
    idle->loadTexture("Sprites/shielded-idle.png");
    idle->setActive(true);
    IntRect idleFrames[3] = { IntRect(9, 0, 137, 162), IntRect(153, 0, 136, 162), IntRect(296,0 , 133, 162) };
    idle->setCustomFrames(idleFrames, 3);
    idle->setScale(charW / 137.0f, charH / 162.0f);
    animations.add(idle);

    // animation[1] — walk
    LoopAnimation* walk = new LoopAnimation(6, 1.0f / 6.0f, true);
    walk->loadTexture("Sprites/shielded-walk.png");
    walk->setActive(false);
    IntRect walkFrames[6] = {
        IntRect(389, 1, 125, 166), 
        IntRect(131, 1, 121, 168), 
        IntRect(1, 1, 128, 169),   
        IntRect(254, 1, 133, 166), 
        IntRect(654, 1, 133, 157), 
        IntRect(516, 1, 136, 161)  
    };
    walk->setCustomFrames(walkFrames, 6);
    walk->setScale(charW / 133.0f, charH / 168.0f);
    animations.add(walk);

    // animation[2] — shoot
    OneShotAnimation* shoot = new OneShotAnimation(3, 1.0f / 6.0f, true);
    shoot->loadTexture("Sprites/shielded-shoot.png");
    shoot->setActive(false);
    IntRect shootFrames[3] = { IntRect(412, 17, 131, 163), IntRect(551, 17, 161, 163), IntRect(721, 17, 154, 163)};
    shoot->setCustomFrames(shootFrames, 3);
    shoot->setScale(charW / 161.0f, charH / 163.0f);
    animations.add(shoot);

    // animation[3] — normal death
    OneShotAnimation* death = new OneShotAnimation(8, 1.0f / 10.0f, true);
    death->loadTexture("Sprites/rebel-death.png");
    death->setActive(false);
    IntRect deathFrames[8] = {
        IntRect(5 , 0 , 62 , 67),
        IntRect(77 , 0 , 72 , 67),
        IntRect(159 , 1 , 74 , 66),
        IntRect(243 , 9 , 82 , 58),
        IntRect(335 , 29 , 80 , 38),
        IntRect(425 , 33 , 88 , 34),
        IntRect(523 , 37 , 90 , 30),
        IntRect(623 , 37 , 90 , 30),
    };
    death->setCustomFrames(deathFrames, 8);
    death->setScale(charW / 90.0f, charH / 58.0f);
    animations.add(death);

    // animation[4] — blast death
    OneShotAnimation* blastDeath = new OneShotAnimation(10, 1.0f / 10.0f, true);
    blastDeath->loadTexture("Sprites/rebel-death-blast.png");
    blastDeath->setActive(false);
    IntRect blastDeathFrames[10] = {
        IntRect(0 , 7 , 56 , 80),
        IntRect(70 , 5 , 58 , 82),
        IntRect(142 , 1 , 62 , 86),
        IntRect(218 , 3 , 68 , 84),
        IntRect(304 , 3 , 74 , 84),
        IntRect(396 , 0 , 84 , 87),
        IntRect(496 , 5 , 84 , 82),
        IntRect(596 , 3 , 94 , 84),
        IntRect(706 , 0 , 94 , 87),
        IntRect(820 , 0 , 94 , 87),
    };
	blastDeath->setCustomFrames(blastDeathFrames, 10);
    blastDeath->setScale(charW / 94.0f, charH / 87.0f);
    animations.add(blastDeath);

    this->width = charW;
	this->height = charH;
}

void ShieldedSoldier::movement() {
    aiState->movement(this);
}

void ShieldedSoldier::attack() {
    aiState->attack(this);
}

void ShieldedSoldier::updateAnimations() {
    // death 
    if (dying)
    {
        int d = killedByBlast ? 4 : 3;
        if (animations[d]->isActive())
        {
            animations[d]->update();
            this->Tex = animations[d]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[d]->getRect());
            float sx = animations[d]->getScaleX();
            float sy = animations[d]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            if (animations[d]->getIsComplete())
                dead = true;
        }
        return;
    }

    // shoot
    if (animations[2]->isActive())
    {
        animations[2]->update();
        this->Tex = animations[2]->getTexture();
        this->sprite.setTexture(this->Tex);
        this->sprite.setTextureRect(animations[2]->getRect());
        float sx = animations[2]->getScaleX();
        float sy = animations[2]->getScaleY();
        this->sprite.setScale(facingRight ? -sx : sx, sy);
        if (!animations[2]->isActive()) animations[2]->reset();
        return;
    }

    // walk vs idle
    if (velocityX != 0)
    {
        animations[0]->setActive(false);
        animations[0]->reset();
        animations[1]->setActive(true);
    }
    else
    {
        animations[1]->setActive(false);
        animations[1]->reset();
        animations[0]->setActive(true);
    }

    for (int i = 0; i <= 1; i++)
    {
        if (animations[i]->isActive())
        {
            animations[i]->update();
            this->Tex = animations[i]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[i]->getRect());
            float sx = animations[i]->getScaleX();
            float sy = animations[i]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            break;
        }
    }
}

bool ShieldedSoldier::absorbsHit(const DamageZone& zone) const
{
    if (zone.getPiercesShield()) 
        return false; //explosions always go through
    if (facingRight)
        return zone.getX() > this->x; // attacker is to the right
    else
        return zone.getX() < this->x + this->width; // attacker is to the left
}

void ShieldedSoldier::behaviour() {
    Enemy::movement();
    if (alerted) {
        Enemy::attack();
        animations[2]->setActive(true);
        animations[2]->reset();
    }
    updateAnimations();
    setAndDraw();
}

void ShieldedSoldier::receiveDamageZone(DamageZone& zone)
{
    if (dying) return;
    if (zone.getPiercesShield())
        killedByBlast = true;
    DamagableEntity::receiveDamageZone(zone);
}

void ShieldedSoldier::changeHP(int change)
{
    HP += change;
    if (HP <= 0 && !dying)
    {
        HP = 0;
        dying = true;
        for (int i = 0; i < animations.getSize(); i++)
            animations[i]->setActive(false);
        int deathAnim = killedByBlast ? 4 : 3;
        animations[deathAnim]->setActive(true);
        animations[deathAnim]->reset();
		if (rand() % 100 < 20) {
            if (rand() % 2 == 0)
                level->queueSpawn(new Turkey(this->x, this->y, level));
            else
                level->queueSpawn(new Fruit(this->x, this->y, level));
        }
        ScoreManager::instance().noteEnemyKill(ScoreManager::SCORE_SHIELDED);
    }
}

ShieldedSoldier::~ShieldedSoldier() {
    delete this->aiState;
    delete this->weapon;
    this->aiState = nullptr;
    this->weapon = nullptr;
}


//======BAZOOKA SOLDIER======
BazookaSoldier::BazookaSoldier(float x, float y,  Level* levelSkeleton) : Enemy(x, y, levelSkeleton), dying(false), killedByBlast(false)
{
    float charW = 96.f;
    float charH = 128.f;
    this->level = levelSkeleton;
    this->HP = 2;
    this->max_speed = 2.f;
    delete aiState;
    this->aiState = new EnemyAIState();
	this->weapon = new RocketLauncher(this);

    // animation[0] — idle
    LoopAnimation* idle = new LoopAnimation(4, 1.0f / 4.0f, true);
    idle->loadTexture("Sprites/bazooka-idle.png");
    idle->setActive(true);
    IntRect idleFrames[4] = { IntRect(8, 0, 180, 223), IntRect(213, 0, 190, 223), IntRect(428, 8  , 190, 215) };
    idle->setCustomFrames(idleFrames, 4);
    idle->setScale(charW / 180.0f, charH / 223.0f);
    animations.add(idle);

    // animation[1] — walk
    LoopAnimation* walk = new LoopAnimation(5, 1.0f / 5.0f, true);
    walk->loadTexture("Sprites/bazooka-walk.png");
    walk->setActive(false);
    IntRect walkFrames[5] = {
        IntRect(5, 7, 205, 195),
        IntRect(235, 2, 205, 200),
        IntRect(470, 2, 200, 200),
        IntRect(695, 7, 205, 195),
        IntRect(925, 12, 200, 190),
    };
    walk->setCustomFrames(walkFrames, 5);
    walk->setScale(charW / 200.0f, charH / 200.0f);
    animations.add(walk);

    // animation[2] — shoot
    OneShotAnimation* shoot = new OneShotAnimation(5, 1.0f / 5.0f, true);
    shoot->loadTexture("Sprites/bazooka-shoot.png");
    shoot->setActive(false);
    IntRect shootFrames[5] = { IntRect(0, 29, 193, 219), IntRect(218, 0, 185, 248), IntRect(428, 0, 175, 248), IntRect(628 , 0 , 175 , 248) , IntRect(828,0,175,248) };
    shoot->setCustomFrames(shootFrames, 5);
    shoot->setScale(charW / 193.0f, charH / 248.0f);
    animations.add(shoot);

    // animation[3] — normal death
    OneShotAnimation* death = new OneShotAnimation(8, 1.0f / 10.0f, true);
    death->loadTexture("Sprites/rebel-death.png");
    death->setActive(false);
    IntRect deathFrames[8] = {
        IntRect(5 , 0 , 62 , 67),
        IntRect(77 , 0 , 72 , 67),
        IntRect(159 , 1 , 74 , 66),
        IntRect(243 , 9 , 82 , 58),
        IntRect(335 , 29 , 80 , 38),
        IntRect(425 , 33 , 88 , 34),
        IntRect(523 , 37 , 90 , 30),
        IntRect(623 , 37 , 90 , 30),
    };
    death->setCustomFrames(deathFrames, 8);
    death->setScale(charW / 90.0f, charH / 58.0f);
    animations.add(death);

    // animation[4] — blast death
    OneShotAnimation* blastDeath = new OneShotAnimation(10, 1.0f / 10.0f, true);
    blastDeath->loadTexture("Sprites/rebel-death-blast.png");
    blastDeath->setActive(false);
    IntRect blastDeathFrames[10] = {
        IntRect(0 , 7 , 56 , 80),
        IntRect(70 , 5 , 58 , 82),
        IntRect(142 , 1 , 62 , 86),
        IntRect(218 , 3 , 68 , 84),
        IntRect(304 , 3 , 74 , 84),
        IntRect(396 , 0 , 84 , 87),
        IntRect(496 , 5 , 84 , 82),
        IntRect(596 , 3 , 94 , 84),
        IntRect(706 , 0 , 94 , 87),
        IntRect(820 , 0 , 94 , 87),
    };
	blastDeath->setCustomFrames(blastDeathFrames, 10);
    blastDeath->setScale(charW / 94.0f, charH / 87.0f);
    animations.add(blastDeath);

    this->width = charW;
    this->height = charH;

}
void BazookaSoldier::receiveDamageZone(DamageZone& zone)
{
    if (!dying)
        if (zone.getPiercesShield())
            killedByBlast = true;
    DamagableEntity::receiveDamageZone(zone);
}

void BazookaSoldier::changeHP(int change)
{
    HP += change;
    if (HP <= 0 && !dying)
    {
        HP = 0;
        dying = true;
        for (int i = 0; i < animations.getSize(); i++)
            animations[i]->setActive(false);
        int deathAnim = killedByBlast ? 4 : 3;
        animations[deathAnim]->setActive(true);
        animations[deathAnim]->reset();
		if (rand() % 100 < 20) {
            if (rand() % 2 == 0)
                level->queueSpawn(new Turkey(this->x, this->y, level));
            else
                level->queueSpawn(new Fruit(this->x, this->y, level));
        }
        ScoreManager::instance().noteEnemyKill(ScoreManager::SCORE_BAZOOKA);
    }
}

void BazookaSoldier::movement() {
    aiState->movement(this);
}
void BazookaSoldier::attack() {
    aiState->attack(this);
}
void BazookaSoldier::updateAnimations() {
    // death 
    if (dying)
    {
        int d = killedByBlast ? 4 : 3;
        if (animations[d]->isActive())
        {
            animations[d]->update();
            this->Tex = animations[d]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[d]->getRect());
            float sx = animations[d]->getScaleX();
            float sy = animations[d]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            if (animations[d]->getIsComplete())
                dead = true;
        }
        return;
    }

    // shoot
    if (animations[2]->isActive())
    {
        animations[2]->update();
        this->Tex = animations[2]->getTexture();
        this->sprite.setTexture(this->Tex);
        this->sprite.setTextureRect(animations[2]->getRect());
        float sx = animations[2]->getScaleX();
        float sy = animations[2]->getScaleY();
        this->sprite.setScale(facingRight ? -sx : sx, sy);
        if (!animations[2]->isActive()) animations[2]->reset();
        return;
    }

    // walk vs idle
    if (velocityX != 0)
    {
        animations[0]->setActive(false);
        animations[0]->reset();
        animations[1]->setActive(true);
    }
    else
    {
        animations[1]->setActive(false);
        animations[1]->reset();
        animations[0]->setActive(true);
    }

    for (int i = 0; i <= 1; i++)
    {
        if (animations[i]->isActive())
        {
            animations[i]->update();
            this->Tex = animations[i]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[i]->getRect());
            float sx = animations[i]->getScaleX();
            float sy = animations[i]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            break;
        }
    }
}
void BazookaSoldier::behaviour() {
    Enemy::movement();
    if (alerted && !dying) {
        Enemy::attack();
        if (!animations[2]->isActive()) {
            animations[2]->setActive(true);
            animations[2]->reset();
        }
    }
    updateAnimations();
    setAndDraw();
}
BazookaSoldier::~BazookaSoldier() {
    delete this->aiState;
    delete this->weapon;
    this->aiState = nullptr;
    this->weapon = nullptr;
}

// ===== GrenadeSoldier =====
GrenadeSoldier::GrenadeSoldier(float x, float y,  Level* level): Enemy(x, y, level), dying(false), killedByBlast(false)
{
    float charW = 96.f;
    float charH = 128.f;

    this->level = level;
    this->HP = 2;
	this->max_speed = 4.0f;
	this->acceleration = 0.7f;
    delete aiState;
    this->aiState = new GrenadeAIState();
    this->weapon = new HandGrenade(this);   

    // animation[0] — idle
    LoopAnimation* idle = new LoopAnimation(2, 1.0f / 3.0f, true);
    idle->loadTexture("Sprites/grenade-idle.png");
    idle->setActive(true);
    IntRect idleFrames[2] = { IntRect(11, 1, 135, 190), IntRect(171, 6, 135, 185) };
    idle->setCustomFrames(idleFrames, 2);
    idle->setScale(charW / 135.0f, charH / 190.0f);
    animations.add(idle);

    // animation[1] — walk
    LoopAnimation* walk = new LoopAnimation(4, 1.0f / 6.0f, true);
    walk->loadTexture("Sprites/grenade-walk.png");
    walk->setActive(false);
    IntRect walkFrames[4] = {
        IntRect(0, 0, 118, 191),
        IntRect(143, 1, 120, 190),
        IntRect(288, 1, 110, 190),
        IntRect(423, 1, 105, 190),
    };
    walk->setCustomFrames(walkFrames, 4);
    walk->setScale(charW / 120.0f, charH / 190.0f);
    animations.add(walk);

    // animation[2] — shoot
    OneShotAnimation* shoot = new OneShotAnimation(4, 1.0f / 6.0f, true);
    shoot->loadTexture("Sprites/grenade-shoot.png");
    shoot->setActive(false);
    IntRect shootFrames[4] = { IntRect(4, 5, 150, 133), IntRect(172, 0, 126, 138), IntRect(315, 0, 116, 138),IntRect(448 , 15, 123, 123) };
    shoot->setCustomFrames(shootFrames, 4);
    shoot->setScale(charW / 150.0f, charH / 138.0f);
    animations.add(shoot);

    // animation[3] — normal death
    OneShotAnimation* death = new OneShotAnimation(8, 1.0f / 10.0f, true);
    death->loadTexture("Sprites/rebel-death.png");
    death->setActive(false);
    IntRect deathFrames[8] = {
        IntRect(5 , 0 , 62 , 67),
        IntRect(77 , 0 , 72 , 67),
        IntRect(159 , 1 , 74 , 66),
        IntRect(243 , 9 , 82 , 58),
        IntRect(335 , 29 , 80 , 38),
        IntRect(425 , 33 , 88 , 34),
        IntRect(523 , 37 , 90 , 30),
        IntRect(623 , 37 , 90 , 30),
    };
    death->setCustomFrames(deathFrames, 8);
    death->setScale(charW / 90.0f, charH / 58.0f);
    animations.add(death);

    // animation[4] — blast death
    OneShotAnimation* blastDeath = new OneShotAnimation(10, 1.0f / 10.0f, true);
    blastDeath->loadTexture("Sprites/rebel-death-blast.png");
    blastDeath->setActive(false);
    IntRect blastDeathFrames[10] = {
        IntRect(0 , 7 , 56 , 80),
        IntRect(70 , 5 , 58 , 82),
        IntRect(142 , 1 , 62 , 86),
        IntRect(218 , 3 , 68 , 84),
        IntRect(304 , 3 , 74 , 84),
        IntRect(396 , 0 , 84 , 87),
        IntRect(496 , 5 , 84 , 82),
        IntRect(596 , 3 , 94 , 84),
        IntRect(706 , 0 , 94 , 87),
        IntRect(820 , 0 , 94 , 87),
    };
    blastDeath->setCustomFrames(blastDeathFrames, 10);
    blastDeath->setScale(charW / 94.0f, charH / 87.0f);
    animations.add(blastDeath);

    
    this->width = charW;
    this->height = charH;
}

void GrenadeSoldier::movement() { 
    aiState->movement(this); 
}
void GrenadeSoldier::attack() { 
    aiState->attack(this); 
}
void GrenadeSoldier::receiveDamageZone(DamageZone& zone)
{
    if (!dying)
        if (zone.getPiercesShield())
            killedByBlast = true;
    DamagableEntity::receiveDamageZone(zone);
}

void GrenadeSoldier::changeHP(int change)
{
    HP += change;
    if (HP <= 0 && !dying)
    {
        HP = 0;
        dying = true;
        for (int i = 0; i < animations.getSize(); i++)
            animations[i]->setActive(false);
        int deathAnim = killedByBlast ? 4 : 3;
        animations[deathAnim]->setActive(true);
        animations[deathAnim]->reset();
		if (rand() % 100 < 20) {
            if (rand() % 2 == 0)
                level->queueSpawn(new Turkey(this->x, this->y, level));
            else
                level->queueSpawn(new Fruit(this->x, this->y, level));
        }
        ScoreManager::instance().noteEnemyKill(ScoreManager::SCORE_GRENADE);
    }
}
void GrenadeSoldier::updateAnimations() {
    // death 
    if (dying)
    {
        int d = killedByBlast ? 4 : 3;
        if (animations[d]->isActive())
        {
            animations[d]->update();
            this->Tex = animations[d]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[d]->getRect());
            float sx = animations[d]->getScaleX();
            float sy = animations[d]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            if (animations[d]->getIsComplete())
                dead = true;
        }
        return;
    }

    // shoot
    if (animations[2]->isActive())
    {
        animations[2]->update();
        this->Tex = animations[2]->getTexture();
        this->sprite.setTexture(this->Tex);
        this->sprite.setTextureRect(animations[2]->getRect());
        float sx = animations[2]->getScaleX();
        float sy = animations[2]->getScaleY();
        this->sprite.setScale(facingRight ? -sx : sx, sy);
        if (!animations[2]->isActive()) 
            animations[2]->reset();
        return;
    }

    // walk vs idle
    if (velocityX != 0)
    {
        animations[0]->setActive(false);
        animations[0]->reset();
        animations[1]->setActive(true);
    }
    else
    {
        animations[1]->setActive(false);
        animations[1]->reset();
        animations[0]->setActive(true);
    }

    for (int i = 0; i <= 1; i++)
    {
        if (animations[i]->isActive())
        {
            animations[i]->update();
            this->Tex = animations[i]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[i]->getRect());
            float sx = animations[i]->getScaleX();
            float sy = animations[i]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            break;
        }
    }
}
void GrenadeSoldier::behaviour() {
    Enemy::movement();
    if (alerted && !dying) {
        Enemy::attack();
        if (!animations[2]->isActive()) {
            animations[2]->setActive(true);
            animations[2]->reset();
        }
    }
    updateAnimations();
    setAndDraw();
}
GrenadeSoldier::~GrenadeSoldier() {
    delete this->aiState;
    delete this->weapon;
    this->aiState = nullptr;
    this->weapon = nullptr;
}

// ===== MummyWarrior =====

MummyWarrior::MummyWarrior(float x, float y, Level* level): Enemy(x, y, level),isCrumbled(false),resurrectionTimer(0.0f),meleeCooldown(0.0f),isDying(false),killedByFire(false)
{
    float charW = 96.f;
    float charH = 128.f;
    this->level = level;
    this->HP = 5;
    this->max_speed = 1.5f;
    this->acceleration = 0.3f;
    delete aiState;
    this->aiState = new MummyAIState();
    this->weapon = nullptr;   // melee only

    // animation[0] — idle
    LoopAnimation* idle = new LoopAnimation(3, 1.0f / 6.0f, false);
    idle->loadTexture("Sprites/mummy-idle.png");
    idle->setActive(true);
    idle->setScale(charW / 176.0f, charH / 265.0f);
    animations.add(idle);

    // animation[1] — walk
    LoopAnimation* walk = new LoopAnimation(7, 1.0f / 8.0f, true);
    walk->loadTexture("Sprites/mummy-walk.png");
    walk->setActive(false);
    IntRect walkFrames[7] = {
        IntRect(9, 8, 137, 198),
        IntRect(185, 8, 137, 198),
        IntRect(361, 8, 137, 198),
        IntRect(532, 8, 146, 198),
        IntRect(713, 12, 137, 194),
        IntRect(884, 16, 137, 190),
		IntRect(1051, 12, 138, 194),
	};
	walk->setCustomFrames(walkFrames, 7);
    walk->setScale(charW / 146.0f, charH / 198.0f);
    animations.add(walk);

    // animation[2] — crumble / die  (oneshot)
    OneShotAnimation* crumble = new OneShotAnimation(5, 1.0f / 10.0f, true);
    crumble->loadTexture("Sprites/mummy-crumble.png");
    crumble->setActive(false);
    IntRect crumbleFrames[5] = {
        IntRect(8, 13, 214, 160),
        IntRect(263, 34, 214, 136),
        IntRect(528, 19, 226, 169),
        IntRect(810, 37, 229, 142),
        IntRect(1099, 37, 232, 136),
	};
    crumble->setCustomFrames(crumbleFrames , 5);
    crumble->setScale(charW / 232.0f, charH / 169.0f);
    animations.add(crumble);

    // animation[3] — resurrect  (oneshot, plays before returning to idle)
    OneShotAnimation* resurrect = new OneShotAnimation(12, 1.0f / 10.0f, true);
    resurrect->loadTexture("Sprites/mummy-resurrect.png");
    resurrect->setActive(false);
    IntRect resurrectFrames[12] = {
        IntRect(6, 70, 60, 16),
        IntRect(85, 68, 64, 18),
        IntRect(170, 59, 69, 27),
        IntRect(256, 50, 65, 36),
        IntRect(333, 44, 66, 42),
        IntRect(411, 37, 63, 49),
        IntRect(490, 26, 58, 60),
        IntRect(569, 22, 56, 64),
        IntRect(644, 22, 54, 64),
        IntRect(718, 15, 54, 71),
        IntRect(784, 8, 54, 78),
        IntRect(852 , 4, 54 ,82),
	};
	resurrect->setCustomFrames(resurrectFrames, 12);
    resurrect->setScale(charW / 96.0f, charH / 128.0f);
    animations.add(resurrect);

	this->width = charW;
	this->height = charH;
}

bool MummyWarrior::absorbsHit(const DamageZone& zone) const {
    return !zone.getPiercesShield();
}

void MummyWarrior::receiveDamageZone(DamageZone& zone)
{
    if (isDying) return;

    // invincible during crumble animation
    if (isCrumbled && animations[2]->isActive()) return;

    // die if hit during resurrection
    if (isCrumbled && animations[3]->isActive() && zone.getPiercesShield()) {
        isDying = true;
        dead = true;
        return;
    }

    // absorbsHit() handles blocking bullets, so just call base
    if (zone.getPiercesShield())
        killedByFire = true;

    DamagableEntity::receiveDamageZone(zone);
}


void MummyWarrior::changeHP(int change)
{
    if (change >= 0) return;

    if (!isCrumbled && !isDying) {
        startCrumble();
        if (killedByFire && rand() % 100 < 20) {
            if (rand() % 2 == 0)
                level->queueSpawn(new Turkey(this->x, this->y, level));
            else
                level->queueSpawn(new Fruit(this->x, this->y, level));
        }
        return;
    }

    if (isCrumbled && !isDying) {
        isDying = true;
        dead = true;  
        if (killedByFire && rand() % 100 < 20) {
            if (rand() % 2 == 0)
                level->queueSpawn(new Turkey(this->x, this->y, level));
            else
                level->queueSpawn(new Fruit(this->x, this->y, level));
        }
        ScoreManager::instance().noteEnemyKill(ScoreManager::SCORE_MUMMY);
        return;
    }
}
bool MummyWarrior::isOffScreen() const  {
    if (isCrumbled) 
        return false;  // always tick while crumbled so resurrection works
    if (level == nullptr) 
        return false;
    float margin = 1.5f * screen_x;
    return (this->x + this->width < level->getScreenLeft() - margin ||
        this->x > level->getScreenRight() + margin);
}

void MummyWarrior::startCrumble(){
    isCrumbled = true;
    resurrectionTimer = 3.0f;
    velocityX = 0;
    for (int i = 0; i < animations.getSize(); i++)
        animations[i]->setActive(false);

    animations[2]->setActive(true);
    animations[2]->reset();
}

void MummyWarrior::updateResurrectionTimer()
{
    if (!isCrumbled)
        return;

    if (animations[2]->isActive())
        return;

    if (isDying) {
        dead = true;
        return;
    }

    if (!animations[3]->isActive() && !animations[3]->getIsComplete())
    {
        resurrectionTimer -= deltaTime;
        if (resurrectionTimer <= 0.0f)
        {
            animations[3]->setActive(true);
            animations[3]->reset();
        }
        return;
    }

    // resurrection anim finished
    if (animations[3]->getIsComplete())
    {
        isCrumbled = false;
        isDying = false;
        killedByFire = false;
        resurrectionTimer = 0.0f;
        this->HP = 5;

        animations[3]->setActive(false);
        animations[3]->reset();
        animations[0]->setActive(true);
    }
}

void MummyWarrior::movement()
{
    if (isCrumbled){
        velocityX = 0;
        entityGravity();
        return;
    }

    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr)
    {
        entityGravity();
        return;
    }

    float dx = player->getX() - this->x;
    if (dx > 0) { 
        velocityX = max_speed;  
        facingRight = true; 
    }
    else { 
        velocityX = -max_speed; 
        facingRight = false; 
    }

    updateTileChecks(this->x, this->y);

    if (facingRight && horizontalCollisionCheck(' ', midRight, bottomRight)) 
        velocityX = 0;
    if (!facingRight && horizontalCollisionCheck(' ', midLeft, bottomLeft))  
        velocityX = 0;
    this->x += velocityX;
    entityGravity();
    setAlerted(true);
    clampToMap();
}

void MummyWarrior::attack() {
    if (isCrumbled || isDying) return;
    if (meleeCooldown > 0.0f) return;

    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr) return;
    if (!checkCollision(player)) return;

    DamageZone* hit = new MummyCurseZone(
        player->getX(), player->getY(),
        player->getWidth(), player->getHeight(),
        2
    );
    level->addDamageZone(hit);
    meleeCooldown = 1.0f;
    clampToMap();
}


void MummyWarrior::updateAnimations()
{
    if (isCrumbled || isDying)
    {
        if (animations[2]->isActive()) {
            animations[2]->update();
            applyAnimation(2);
            return;
        }
        if (animations[3]->isActive()) {
            animations[3]->update();
            applyAnimation(3);
            return;
        }
        // hold last frame of whichever finished
        // if resurrect anim is done, hold its last frame
        // if crumble anim is done but resurrect hasn't started, hold crumble last frame
        if (animations[3]->getIsComplete())
            applyAnimation(3);
        else
            applyAnimation(2);
        return;
    }

    // walk vs idle — only reached when NOT crumbled/dying
    if (velocityX != 0)
    {
        animations[0]->setActive(false);
        animations[0]->reset();
        animations[1]->setActive(true);
    }
    else
    {
        animations[1]->setActive(false);
        animations[1]->reset();
        animations[0]->setActive(true);
    }

    for (int i = 0; i <= 1; i++)
    {
        if (animations[i]->isActive())
        {
            animations[i]->update();
            applyAnimation(i);
            break;
        }
    }
}
void MummyWarrior::applyAnimation(int index){
    this->Tex = animations[index]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[index]->getRect());
    float sx = animations[index]->getScaleX();
    float sy = animations[index]->getScaleY();
    this->sprite.setScale(facingRight ? -sx : sx, sy);
}

void MummyWarrior::behaviour()
{
    if (meleeCooldown > 0.0f)
        meleeCooldown -= deltaTime;

    updateResurrectionTimer();

    if (!isCrumbled && !isDying)
    {
        movement();
        attack();
    }
    else
    {
        // still need gravity while crumbled on the ground
        entityGravity();
    }

    updateAnimations();
    setAndDraw();
}
bool MummyWarrior::IsCrumbled()const {
    return isCrumbled;
}


void MummyWarrior::onIgnite() {
    isDying = true;
    killedByFire = true;
    HP = 0;
    dead = true;
}

MummyWarrior::~MummyWarrior(){
    delete this->aiState;
    this->aiState = nullptr;
}


// ===== Zombie =====

Zombie::Zombie(float x, float y, Level* level): Enemy(x, y, level), meleeCooldown(0.0f),dying(false),killedByBlast(false){
    float charW = 96.f;
    float charH = 128.f;

    this->level = level;
    this->HP = 5;
    this->max_speed = 1.5f;
    this->acceleration = 0.3f;

    delete aiState;
    this->aiState = new ZombieAIState();
    this->weapon = nullptr;   // melee only

    // animation[0] — idle
    LoopAnimation* idle = new LoopAnimation(3, 1.0f / 6.0f, false);
    idle->loadTexture("Sprites/zombie-idle.png");
    idle->setActive(true);
    idle->setScale(charW / 210.0f, charH / 281.0f);
    animations.add(idle);

    // animation[1] — walk
    LoopAnimation* walk = new LoopAnimation(4, 1.0f / 8.0f, true);
    walk->loadTexture("Sprites/zombie-walk.png");
    walk->setActive(false);
    IntRect walkFrames[4] = {
        IntRect(19 , 7 , 203, 270), 
        IntRect(250 , 7 , 210, 270), 
        IntRect(488 , 7 , 210, 270), 
        IntRect(726 , 7 , 210, 270), 
    };
	walk->setCustomFrames(walkFrames, 4);
    walk->setScale(charW / 210.0f, charH / 270.0f);
    animations.add(walk);

    // animation[2] — normal death (oneshot)
    OneShotAnimation* death = new OneShotAnimation(5, 1.0f / 8.0f, true);
    death->loadTexture("Sprites/zombie-death.png");
    death->setActive(false);
    IntRect deathFrames[5] = {
        IntRect(0,   8,  204, 179),
        IntRect(214,  2,  206, 176),
        IntRect(432, 14,  202, 173),
        IntRect(644, 32,  96, 126),
        IntRect(744, 38, 94, 118),
    };
    death->setCustomFrames(deathFrames, 5);
    death->setScale(charW / 150.0f, charH / 100.0f);
    animations.add(death);

    // animation[3] — blast death (oneshot)
    OneShotAnimation* blastDeath = new OneShotAnimation(5, 1.0f / 8.0f, true);
    blastDeath->loadTexture("Sprites/zombie-death.png");
    blastDeath->setActive(false);
    IntRect deathBlastFrames[5] = {
        IntRect(0,   8,  204, 179),
        IntRect(214,  2,  206, 176),
        IntRect(432, 14,  202, 173),
        IntRect(644, 32,  96, 126),
        IntRect(744, 38, 94, 118),
    };
    blastDeath->setCustomFrames(deathBlastFrames, 5);
    blastDeath->setScale(charW / 150.0f, charH / 100.0f);
    animations.add(blastDeath);

    this->width = charW;
    this->height = charH;
}

void Zombie::receiveDamageZone(DamageZone& zone)
{
    if (dying) return;
    if (zone.getPiercesShield())
        killedByBlast = true;
    DamagableEntity::receiveDamageZone(zone);
}

void Zombie::changeHP(int change)
{
    HP += change;
    if (HP <= 0 && !dying)
    {
        HP = 0;
        dying = true;
        for (int i = 0; i < animations.getSize(); i++)
            animations[i]->setActive(false);
        int deathAnim = killedByBlast ? 3 : 2;
        animations[deathAnim]->setActive(true);
        animations[deathAnim]->reset();
		if (rand() % 100 < 20) {
            if (rand() % 2 == 0)
                level->queueSpawn(new Turkey(this->x, this->y, level));
            else
                level->queueSpawn(new Fruit(this->x, this->y, level));
        }
        ScoreManager::instance().noteEnemyKill(ScoreManager::SCORE_ZOMBIE);
    }
}

void Zombie::movement()
{
    if (dying)
    {
        entityGravity();
        return;
    }

    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr)
    {
        entityGravity();
        return;
    }

    float dx = player->getX() - this->x;
    if (dx > 0) { 
        velocityX = max_speed;  
        facingRight = true; 
    }
    else { 
        velocityX = -max_speed; 
        facingRight = false; 
    }

    updateTileChecks(this->x, this->y);

    if (facingRight && horizontalCollisionCheck(' ', midRight, bottomRight)) 
        velocityX = 0;
    if (!facingRight && horizontalCollisionCheck(' ', midLeft, bottomLeft))  
        velocityX = 0;

    this->x += velocityX;
    entityGravity();
    setAlerted(true);
    clampToMap();
}

void Zombie::attack() {
    if (dying) return;
    if (meleeCooldown > 0.0f) return;

    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr) return;
    if (!checkCollision(player)) return;

    DamageZone* hit = new ZombieHitZone( player->getX(), player->getY(),player->getWidth(), player->getHeight(), 2 );
    level->addDamageZone(hit);
    meleeCooldown = 1.0f;
}

void Zombie::updateAnimations()
{
    if (dying)
    {
        int d = killedByBlast ? 3 : 2;
        if (animations[d]->isActive())
        {
            animations[d]->update();
            this->Tex = animations[d]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[d]->getRect());
            float sx = animations[d]->getScaleX();
            float sy = animations[d]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            if (animations[d]->getIsComplete())
                dead = true;
        }
        return;
    }

    if (velocityX != 0)
    {
        animations[0]->setActive(false);
        animations[0]->reset();
        animations[1]->setActive(true);
    }
    else
    {
        animations[1]->setActive(false);
        animations[1]->reset();
        animations[0]->setActive(true);
    }

    for (int i = 0; i <= 1; i++)
    {
        if (animations[i]->isActive())
        {
            animations[i]->update();
            this->Tex = animations[i]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[i]->getRect());
            float sx = animations[i]->getScaleX();
            float sy = animations[i]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            break;
        }
    }
}

void Zombie::behaviour()
{
    if (meleeCooldown > 0.0f)
        meleeCooldown -= deltaTime;

    if (!dying)
    {
        movement();
        attack();
    }
    else
    {
        entityGravity();
    }

    updateAnimations();
    setAndDraw();
}

Zombie::~Zombie()
{
    delete this->aiState;
    this->aiState = nullptr;
    // weapon is nullptr, no delete needed
}
// ===== Martian ============

Martian::Martian(float x, float y, Level* level): Enemy(x, y, level), phase(0), podHP(5),transitioning(false), killedByBlast(false), dying(false) {
    this->level = level;
    this->HP = 3;
    this->max_speed = 2.0f;
    this->acceleration = 0.3f;
    delete aiState;
    this->aiState = new PodAIState();
    this->weapon = new PodLaserGun(this);

    // animation[0] ,  pod hover
    LoopAnimation* hover = new LoopAnimation(8, 1.0f / 8.0f, false);
    hover->loadTexture("Sprites/martian-pod.png");
    hover->setScale(96.0f / 43.0f , 128.0f / 37.0f);
    hover->setActive(true);
    animations.add(hover);

    // animation[1] ,  pod destruction (oneshot)
    OneShotAnimation* destroy = new OneShotAnimation(6, 1.0f / 6.0f, true);
    destroy->loadTexture("Sprites/martian-pod-destroy.png");
    destroy->setActive(false);
    IntRect destroyFrames[6] = {
        IntRect(0, 16, 44, 38),
        IntRect(47, 14, 49, 40),
        IntRect(99, 4, 54, 50),
        IntRect(159, 12, 55, 42),
        IntRect(221, 11, 57, 43),
        IntRect(290, 0, 56, 54)
	};
	destroy->setCustomFrames(destroyFrames, 6);
    destroy->setScale(96.0f / 40.0f, 96.0f / 30.0f);
    animations.add(destroy);

    // animation[2] ,  grounded idle
    LoopAnimation* idle = new LoopAnimation(6, 1.0f / 8.0f, true);
    idle->loadTexture("Sprites/martian-walk.png");
    idle->setActive(false);
    IntRect idleFrames[6] = {
        IntRect(0 , 0 , 196 , 172),
        IntRect(216, 0,200,172),
        IntRect(436 , 4, 200 , 168),
        IntRect(656, 4 , 204, 168),
        IntRect(880 , 4 , 208 , 168),
        IntRect(1108 , 4 , 204 , 168),
    };
    idle->setCustomFrames(idleFrames, 6);
    idle->setScale(96.0f / 204.0f, 128.0f / 172.0f);
    animations.add(idle);

    // animation[3] ,  grounded walk
    LoopAnimation* walk = new LoopAnimation(6, 1.0f / 8.0f, true);
    walk->loadTexture("Sprites/martian-walk.png");
    walk->setActive(false);
    IntRect walkFrames[6] = {
        IntRect(0 , 0 , 196 , 172),
        IntRect(216, 0,200,172),
        IntRect(436 , 4, 200 , 168),
        IntRect(656, 4 , 204, 168),
        IntRect(880 , 4 , 208 , 168),
        IntRect(1108 , 4 , 204 , 168),

    };
    walk->setCustomFrames(walkFrames, 6);
    walk->setScale(96.0f / 204.0f, 128.0f / 172.0f);
    animations.add(walk);

    // animation[4] ,  grounded shoot (oneshot)
    OneShotAnimation* shoot = new OneShotAnimation(3, 1.0f / 6.0f, true);
    shoot->loadTexture("Sprites/martian-shoot.png");
    shoot->setActive(false);
    IntRect shootFrames[3] = {
        IntRect( 0 , 0 , 276 , 152),
        IntRect(296 ,4 ,280 , 148),
        IntRect(628 , 8 , 268 , 144)
    };
    shoot->setCustomFrames(shootFrames, 3);
    shoot->setScale(96.0f / 280.0f, 128.0f / 152.0f);
    animations.add(shoot);

    // animation[5]  , grounded blast death
    OneShotAnimation* blastDeath = new OneShotAnimation(16, 1.0f / 16.0f, false);
    blastDeath->loadTexture("Sprites/martian-blast-death.png");
    blastDeath->setScale(96.0f / 196.0f, 128.0f / 172.0f);
    blastDeath->setActive(false);
    animations.add(blastDeath);

    // animation[6] ,  grounded normal death
    OneShotAnimation* death = new OneShotAnimation(16, 1.0f / 16.0f, false);
    death->loadTexture("Sprites/martian-death.png");
    death->setScale(96.0f / 196.0f, 128.0f / 172.0f);
    death->setActive(false);
    animations.add(death);

    // sync main sprite
    this->width = 96.0f;
	this->height = 128.0f;
}

void Martian::changeHP(int change)
{
    if (phase == 0)
    {
        podHP += change;
        if (podHP <= 0 && !transitioning)
        {
            podHP = 0;
            changePhase();
        }
    }
    else
    {
        HP += change;
        if (HP <= 0 && !dying)
        {
            HP = 0;
            dying = true;

            // deactivate everything
            for (int i = 0; i < animations.getSize(); i++)
                animations[i]->setActive(false);

            // trigger correct death animation
            if (killedByBlast)
            {
                animations[5]->setActive(true);
                animations[5]->reset();
            }
            else
            {
                animations[6]->setActive(true);
                animations[6]->reset();
            }
			if (rand() % 100 < 20) {
	            if (rand() % 2 == 0)
	                level->queueSpawn(new Turkey(this->x, this->y, level));
	            else
	                level->queueSpawn(new Fruit(this->x, this->y, level));
        	}
            ScoreManager::instance().noteEnemyKill(ScoreManager::SCORE_MARTIAN);
        }
    }
}

void Martian::changePhase(){
    transitioning = true;
    animations[0]->setActive(false);
    animations[1]->reset();     
    animations[1]->setActive(true);   // start destruction anim
    //fall gravity se hojayga
}

void Martian::finalizePhase(){
    phase = 1;
    transitioning = false;
    this->velocityY = 0.0f;
	this->onGround = false; // just in case, to prevent any weird physics bugs during the transition

	//swaping for grounded phase
    delete this->weapon;
	this->weapon = new Pistol(this);
    delete aiState;
    aiState = new MartianGroundedAIState();

    // activate grounded idle
    animations[1]->setActive(false);
    animations[2]->setActive(true);

    // update main sprite dimensions to grounded size
	this->width = 96.0f;
	this->height = 128.0f;
}

void Martian::movement(){
    aiState->movement(this);
}

void Martian::attack()
{
    if (transitioning) return;
    if (!alerted) return;

    aiState->attack(this);

    if (phase == 1 && !animations[4]->isActive()
        && !animations[4]->getIsComplete())
    {
        animations[4]->setActive(true);
        animations[4]->reset();
    }

    if (animations[4]->getIsComplete())
        animations[4]->reset();
}
void Martian::updateAnimations(){
    // --- pod phase ---
    if (phase == 0 || transitioning)
    {
        // destruction oneshot — check completion to finalize phase
        if (animations[1]->isActive())
        {
            animations[1]->update();
            if (animations[1]->getIsComplete())
            {
                // store current position before finalizing
                float savedX = this->x;
                float savedY = this->y;
                finalizePhase();
                // restore to prevent bugs
                this->x = savedX;
                this->y = savedY;
                return;
            }
            this->Tex = animations[1]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[1]->getRect());
            float sx = animations[1]->getScaleX();
            float sy = animations[1]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            return;
        }

        // hover loop
        animations[0]->update();
        this->Tex = animations[0]->getTexture();
        this->sprite.setTexture(this->Tex);
        this->sprite.setTextureRect(animations[0]->getRect());
        float sx = animations[0]->getScaleX();
        float sy = animations[0]->getScaleY();
        this->sprite.setScale(facingRight ? -sx : sx, sy);
        return;
    }

    // --- grounded phase ---
    if (dying)
    {
        int deathAnim = killedByBlast ? 5 : 6;
        if (animations[deathAnim]->isActive())
        {
            animations[deathAnim]->update();
            this->Tex = animations[deathAnim]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[deathAnim]->getRect());
            float sx = animations[deathAnim]->getScaleX();
            float sy = animations[deathAnim]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);

            // ab yahan jakr mrega lol
            if (animations[deathAnim]->getIsComplete())
                dead = true;
        }
        return;
    }


    // shoot oneshot 
    if (animations[4]->isActive())
    {
        animations[4]->update();
        this->Tex = animations[4]->getTexture();
        this->sprite.setTexture(this->Tex);
        this->sprite.setTextureRect(animations[4]->getRect());
        float sx = animations[4]->getScaleX();
        float sy = animations[4]->getScaleY();
        this->sprite.setScale(facingRight ? -sx : sx, sy);
        if (!animations[4]->isActive()) animations[4]->reset();
        return;
    }

    // walk vs idle
    if (velocityX != 0)
    {
        animations[2]->setActive(false);
        animations[2]->reset();
        animations[3]->setActive(true);
    }
    else
    {
        animations[3]->setActive(false);
        animations[3]->reset();
        animations[2]->setActive(true);
    }

    for (int i = 2; i <= 3; i++)
    {
        if (animations[i]->isActive())
        {
            animations[i]->update();
            this->Tex = animations[i]->getTexture();
            this->sprite.setTexture(this->Tex);
            this->sprite.setTextureRect(animations[i]->getRect());
            float sx = animations[i]->getScaleX();
            float sy = animations[i]->getScaleY();
            this->sprite.setScale(facingRight ? -sx : sx, sy);
            break;
        }
    }
}

void Martian::receiveDamageZone(DamageZone& zone){
    if (phase == 1 && !dying){
        // record whether this hit is a blast before passing to base
        if (zone.getPiercesShield())
            killedByBlast = true;
    }
    DamagableEntity::receiveDamageZone(zone);
}

void Martian::behaviour() {
    if (!dying) {
        movement();
        if (alerted)
            attack();
    }
    updateAnimations();
    setAndDraw();
}

int Martian::getPhase() { 
    return phase; 
}
bool Martian::getTransitioning() const { 
    return transitioning; 
}


Martian::~Martian() {
    delete this->aiState;
    delete this->weapon;
    this->aiState = nullptr;
	this->weapon = nullptr;
}

// ===== ParaTrooper =====

ParaTrooper::ParaTrooper(float x, float y, Level* level, int typeAfterLanding) : Enemy(x, y, level), landingEnemy(nullptr), scoredAerial(false)
{
    this->getTex().loadFromFile("Sprites/paratrooper.png");
    this->setScaleX(200.0f / 1024.0f);
    this->setScaleY(300.0f / 1536.0f);
    this->initializeScale();
    this->initializeDimensions();
    this->level = level;
    this->weapon = nullptr;
    delete aiState;
    this->aiState = new ParatrooperAIState();

    if (typeAfterLanding == 0)      
        landingEnemy = new RebelSoldier(x, y, level);
    else if (typeAfterLanding == 1) 
        landingEnemy = new ShieldedSoldier(x, y, level);
    else if (typeAfterLanding == 2) 
        landingEnemy = new BazookaSoldier(x, y, level);
    else  
        landingEnemy = new GrenadeSoldier(x, y,level);
}

void ParaTrooper::movement() { aiState->movement(this); } //empty because gravity goes brrrr
void ParaTrooper::attack() { aiState->attack(this); }
void ParaTrooper::updateAnimations() {}
bool ParaTrooper::isLanded() { 
    return onGround;
}

void ParaTrooper::changeHP(int change) {
    HP += change;
    if (HP <= 0 && !dead && !scoredAerial) {
        HP = 0;
        dead = true;
        scoredAerial = true;
        // pick base infantry score by what would have landed, +25 aerial bonus
        int base = ScoreManager::SCORE_REBEL;
        if (dynamic_cast<ShieldedSoldier*>(landingEnemy)) base = ScoreManager::SCORE_SHIELDED;
        else if (dynamic_cast<BazookaSoldier*>(landingEnemy)) base = ScoreManager::SCORE_BAZOOKA;
        else if (dynamic_cast<GrenadeSoldier*>(landingEnemy)) base = ScoreManager::SCORE_GRENADE;
        ScoreManager::instance().noteEnemyKill(base + ScoreManager::SCORE_PARA_BONUS);
    }
}

void ParaTrooper::behaviour() {
    if (!onGround) {
        movement();
        setAndDraw();
        return;
    }

    if (landingEnemy != nullptr) {
        landingEnemy->setPosition(this->x, this->y - cell_size);
        level->queueSpawn(landingEnemy);
        landingEnemy = nullptr;
    }
    dead = true;

}

ParaTrooper::~ParaTrooper() {
    if (aiState != nullptr) {
        delete aiState;      
        aiState = nullptr; 
    }
    if (landingEnemy != nullptr) { 
        delete landingEnemy; 
        landingEnemy = nullptr;
    }
}

