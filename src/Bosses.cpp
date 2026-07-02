#include "project.h"


// ============= BOSS BASE =============

Boss::Boss(float x, float y, Level* levelSkeleton) : DamagableEntity(levelSkeleton), currentWeapon(0), batchCooldown(0.0f),
retreating(false), maxHP(0), detectionRadius((float)screen_x * 1.5f)
{
    this->x = x;
    this->y = y;
    this->level = levelSkeleton;
    weapon[0] = nullptr;
    weapon[1] = nullptr;
    weapon[2] = nullptr;
}

Boss::~Boss() {
    for (int i = 0; i < 3; i++) {
        if (weapon[i] != nullptr) {
            delete weapon[i];
            weapon[i] = nullptr;
        }
    }
    // currentBatch holds non-owning pointers — entities are owned by EntityManager
}

void Boss::writeStateTo(CurrentState& s) const {
    if (s.bossCount >= 4) return;
    BossState& b = s.bosses[s.bossCount];
    b.id = entityId;
    b.x = getX();
    b.y = getY();
    b.HP = HP;
    b.currentWeapon = currentWeapon;
    b.dead = dead;
    s.bossCount++;
}

void Boss::applyStateFrom(const CurrentState& s) {
    for (int i = 0; i < s.bossCount; i++) {
        if (s.bosses[i].id != entityId) continue;
        const BossState& b = s.bosses[i];
        setX(b.x);
        setY(b.y);
        HP = b.HP;
        currentWeapon = b.currentWeapon;
        dead = b.dead;
        return;
    }
}

float Boss::getShootAngle() const {
    PlayerSoldier* p = level->getCurrentPlayer();
    if (p == nullptr) return facingRight ? 0.0f : 180.0f;
    float dx = (p->getX() + p->getWidth() / 2.0f) - (this->x + this->width / 2.0f);
    float dy = (p->getY() + p->getHeight() / 2.0f) - (this->y + this->height / 2.0f);
    float angle = atan2(dy, dx) * 180.0f / 3.14159265f;
    angle += (rand() % 41 - 20);
    return angle;
}

int Boss::getMaxHP() const {
    return maxHP;
}

bool Boss::isRetreating() const {
    return retreating;
}

bool Boss::playerInRange() const {
    PlayerSoldier* p = level->getCurrentPlayer();
    if (p == nullptr) return false;
    float dx = p->getX() - x;
    if (dx < 0) dx = -dx;
    return dx <= detectionRadius;
}

void Boss::pruneDeadMinions() {
    bool anyAliveBefore = currentBatch.getSize() > 0;

    for (int i = currentBatch.getSize() - 1; i >= 0; i--) {
        DamagableEntity* m = currentBatch[i];
        if (m == nullptr || m->isDead()) {
            currentBatch.remove(i);
        }
    }

    if (anyAliveBefore && currentBatch.getSize() == 0) {
        PlayerSoldier* p = level->getCurrentPlayer();
        if (p != nullptr) p->replenishOnBatchKill();
    }
}




// 

// 
// ============= IRON NOKANA =============

IronNokana::IronNokana(float x, float y, Level* levelSkeleton) : Boss(x, y, levelSkeleton)
{
    this->HP = 200;
    this->maxHP = 200;
    this->max_speed = 1.5f;
    this->acceleration = 0.2f;
    this->retreating = false;
    this->weaponRotateTimer = 0;
    float charH = 400.0f;
    float charW = 400.0f;
    this->width = charW;
    this->height = charH;

    // animation[0] — idle 
    LoopAnimation* idle = new LoopAnimation(4, 1.0f / 2.0f, false);
    idle->loadTexture("Sprites/iron-move.png");
    idle->setScale(charW / 471.0f, charH / 320.0f);
    idle->setActive(true);
    animations.add(idle);

    // animation[1] — move 
    LoopAnimation* move = new LoopAnimation(4, 1.0f / 5.0f, false);
    move->loadTexture("Sprites/iron-move.png");
    move->setScale(this->width / 471.0f, this->height / 320.0f);
    move->setActive(false);
    animations.add(move);

    // animation[2] — retreat/death: same sheet, faster interval
    LoopAnimation* retreat = new LoopAnimation(4, 1.0f / 16.0f, false);
    retreat->loadTexture("Sprites/iron-move.png");
    retreat->setScale(this->width / 471.0f, this->height / 320.0f);
    retreat->setActive(false);
    animations.add(retreat);


    weapon[0] = new RocketLauncher(this);
    weapon[1] = new FlameShot(this);
    weapon[2] = new FireBombGrenade(this);

    this->x = x;
    this->y = y;

}

IronNokana::~IronNokana() {
}

void IronNokana::movement() {
    PlayerSoldier* p = level->getCurrentPlayer();
    if (p == nullptr) { velocityX = 0.0f; return; }

    if (retreating) {
        if (p->getX() > this->x)
            velocityX = -max_speed * 6.0f;
        else
            velocityX = max_speed * 6.0f;

        facingRight = (velocityX > 0);
        x += velocityX;
        return;
    }

    if (!playerInRange()) {
        velocityX = 0.0f;
        entityGravity();
        clampToMap();
        return;
    }

    float dx = p->getX() - x;
    if (dx > 0) { 
        facingRight = true; 
        velocityX = max_speed;
    }
    else if (dx < 0) { 
        facingRight = false;
        velocityX = -max_speed;
    }

    float absDx = dx < 0 ? -dx : dx;
    if (absDx < screen_x * 0.15f) velocityX = 0.0f;

    if (velocityX != 0.0f) {
        updateTileChecks(x + velocityX, y - 32);
        if (velocityX > 0 && horizontalCollisionCheck(topRight, midRight, bottomRight)) velocityX = 0.0f;
        else if (velocityX < 0 && horizontalCollisionCheck(topLeft, midLeft, bottomLeft)) velocityX = 0.0f;
    }

    x += velocityX;
    entityGravity();
    clampToMap();
}


void IronNokana::changeHP(int change) {
    if (retreating) return;
    HP += change;
    if (HP <= 0) {
        HP = 0;
        retreating = true;
        ScoreManager::instance().addScore(ScoreManager::SCORE_NORMAL_BOSS);
    }
}

void IronNokana::updateAnimations()
{
    int activeIndex = 0;

    if (retreating)
    {
        // death/retreat
        activeIndex = 2;
    }
    else if (velocityX != 0)
    {
        activeIndex = 1; // move
    }
    else
    {
        activeIndex = 0; // idle
    }

    // deactivate all, activate the chosen one
    for (int i = 0; i < animations.getSize(); i++)
    {
        animations[i]->setActive(i == activeIndex);
    }

    animations[activeIndex]->update();
    this->Tex = animations[activeIndex]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[activeIndex]->getRect());
    float sx = animations[activeIndex]->getScaleX();
    float sy = animations[activeIndex]->getScaleY();
    this->sprite.setScale(facingRight ? -sx : sx, sy);
}
void IronNokana::attack() {
    if (retreating) return;

    PlayerSoldier* p = level->getCurrentPlayer();
    if (p == nullptr)
        return;

    if (weapon[currentWeapon]->isOutOfAmmo()) { weapon[currentWeapon]->addAmmo(50); }
    if (currentWeapon == 0)
        weapon[0]->fire();
    else if (currentWeapon == 1)
        weapon[1]->fire();
    else
        weapon[2]->fire();

    weapon[currentWeapon]->update();

    weaponRotateTimer += deltaTime;
    if (weaponRotateTimer > 2.5f) {
        currentWeapon = (currentWeapon + 1) % 2;
        weaponRotateTimer = 0.0f;
    }
}

void IronNokana::spawnNextBatch() {
    int batchSize = 2 + (rand() % 3);
    float spawnY = y;
    for (int i = 0; i < batchSize; i++) {
        float offset = (i + 1) * 90.0f;
        if (facingRight) offset = -offset;
        DamagableEntity* m;
        if (i % 2 == 0) m = new RebelSoldier(x + offset, spawnY, level);
        else m = new BazookaSoldier(x + offset, spawnY, level);
        level->queueSpawn(m);
        currentBatch.add(m);
    }
}

void IronNokana::behaviour() {
    if (dead)
        return;

    if (!retreating && HP <= maxHP / 2) {
        retreating = true;
    }

    if (retreating) {
        movement();
        if (x < level->getScreenLeft() - 256.0f || x > level->getScreenRight() + 256.0f) {
            dead = true;
            return;
        }
        updateAnimations();
        setAndDraw();
        return;
    }

    movement();
    attack();
    pruneDeadMinions();

    batchCooldown -= deltaTime;
    if (currentBatch.getSize() == 0 && batchCooldown <= 0.0f) {
        spawnNextBatch();
        batchCooldown = 5.0f;
    }
    updateAnimations();
    setAndDraw();
}




// ============= HAIR BUSTER =============

HairBuster::HairBuster(float x, float y, Level* levelSkeleton)
    : Boss(x, y, levelSkeleton)
{
    this->HP = 200;
    this->maxHP = 200;
    this->width = 192.0f;
    this->height = 96.0f;
    this->max_speed = 3.0f;
    this->acceleration = 0.3f;
    this->weaponRotateTimer = 0.0f;

    float charH = 400.0f;
    float charW = 400.0f;
    this->width = charW;
    this->height = charH;

    // animation[0] — idle 
    LoopAnimation* idle = new LoopAnimation(3, 1.0f / 2.0f, false);
    idle->loadTexture("Sprites/airbust-move.png");
    idle->setScale(charW / 496.0f, charH / 379.0f);
    idle->setActive(true);
    animations.add(idle);

    // animation[1] — move 
    LoopAnimation* move = new LoopAnimation(3, 1.0f / 5.0f, false);
    move->loadTexture("Sprites/airbust-move.png");
    move->setScale(charW / 496.0f, charH / 379.0f);
    move->setActive(false);
    animations.add(move);

    // animation[2] — retreat/death: same sheet, faster interval
    LoopAnimation* retreat = new LoopAnimation(3, 1.0f / 16.0f, false);
    retreat->loadTexture("Sprites/airbust-move.png");
    retreat->setScale(charW / 496.0f, charH / 379.0f);
    retreat->setActive(false);
    animations.add(retreat);

    weapon[0] = new RocketLauncher(this);
    weapon[1] = new FireBombGrenade(this);
    weapon[2] = nullptr;

    this->x = x;
    this->y = y;
}

HairBuster::~HairBuster() {
}

void HairBuster::movement() {
    PlayerSoldier* p = level->getCurrentPlayer();
    if (p == nullptr) {
        velocityX = 0.0f;
        velocityY = 0.0f; return;
    }

    if (retreating) {
        velocityY = -max_speed * 6.0f;
        velocityX = 0.0f;
        y += velocityY;
        return;
    }

    if (!playerInRange()) {
        velocityX = 0.0f;
        velocityY = 0.0f;
        return;
    }

    float dx = p->getX() - x;
    if (dx > 0) { facingRight = true; velocityX = max_speed * 0.5f; }
    else if (dx < 0) { facingRight = false; velocityX = -max_speed * 0.5f; }

    float absDx = dx < 0 ? -dx : dx;
    if (absDx < screen_x * 0.15f) velocityX = 0.0f;

    float targetY = p->getY() - 320.0f;
    float dy = targetY - y;
    if (dy > 4.0f) velocityY = max_speed * 0.4f;
    else if (dy < -4.0f) velocityY = -max_speed * 0.4f;
    else velocityY = 0.0f;

    if (velocityX != 0.0f) {
        updateTileChecks(x + velocityX, y);
        if (velocityX > 0 && horizontalCollisionCheck(topRight, midRight, bottomRight)) velocityX = 0.0f;
        else if (velocityX < 0 && horizontalCollisionCheck(topLeft, midLeft, bottomLeft)) velocityX = 0.0f;
    }
    if (velocityY != 0.0f) {
        updateTileChecks(x, y + velocityY);
        
        if (velocityY < 0 && (topMid == '#' || topMid == 'D')) velocityY = 0.0f;
        else if (velocityY > 0 && (belowMid == '#' || belowMid == 'D')) velocityY = 0.0f;
    }

    x += velocityX;
    y += velocityY;
    clampToMap();
}

void HairBuster::updateAnimations() {
    int activeIndex = 0;

    if (retreating)
    {
        // death/retreat
        activeIndex = 2;
    }
    else if (velocityX != 0 || velocityY != 0)
    {
        activeIndex = 1; // move
    }
    else
    {
        activeIndex = 0; // idle
    }

    // deactivate all, activate the chosen one
    for (int i = 0; i < animations.getSize(); i++)
    {
        animations[i]->setActive(i == activeIndex);
    }

    animations[activeIndex]->update();
    this->Tex = animations[activeIndex]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[activeIndex]->getRect());
    float sx = animations[activeIndex]->getScaleX();
    float sy = animations[activeIndex]->getScaleY();
    this->sprite.setScale(facingRight ? sx : -sx, sy);
}

void HairBuster::attack() {
    if (retreating) return;

    PlayerSoldier* p = level->getCurrentPlayer();
    if (p == nullptr) return;

    if (weapon[currentWeapon]->isOutOfAmmo()) weapon[currentWeapon]->addAmmo(50);
    if (currentWeapon == 0) weapon[0]->fire();
    else weapon[1]->fire();

    weapon[currentWeapon]->update();

    weaponRotateTimer += deltaTime;
    if (weaponRotateTimer > 2.5f) {
        currentWeapon = (currentWeapon + 1) % 2;
        weaponRotateTimer = 0.0f;
    }
}

void HairBuster::spawnNextBatch() {
    int batchSize = 2 + (rand() % 3);
    for (int i = 0; i < batchSize; i++) {
        float offset = (i + 1) * 100.0f;
        if (facingRight) offset = -offset;
        float spawnY = y + 64.0f;
        DamagableEntity* m;
        if (i % 2 == 0) m = new ParaTrooper(x + offset, spawnY, level, rand() % 4);
        else m = new FlyingTara(x + offset, spawnY - 64.0f, level);
        level->queueSpawn(m);
        currentBatch.add(m);
    }
}

void HairBuster::behaviour() {
    if (dead) return;

    if (!retreating && HP <= maxHP / 2) {
        retreating = true;
    }

    if (retreating) {
        movement();
        if (y < level->getScreenTop() - 256.0f) {
            dead = true;
            return;
        }
        updateAnimations();
        setAndDraw();
        return;
    }

    movement();
    attack();
    pruneDeadMinions();


    batchCooldown -= deltaTime;
    if (currentBatch.getSize() == 0 && batchCooldown <= 0.0f) {
        spawnNextBatch();
        batchCooldown = 6.0f;
    }
    updateAnimations();
    setAndDraw();
}

void HairBuster::changeHP(int change) {
    if (retreating)
        return;
    HP += change;
    if (HP <= 0) {
        HP = 0;
        retreating = true;
    }
}



// ============= SEA SATAN =============

SeaSatan::SeaSatan(float x, float y, Level* levelSkeleton) : Boss(x, y, levelSkeleton) {
    this->HP = 200;
    this->maxHP = 200;
    this->width = 224.0f;
    this->height = 128.0f;
    this->max_speed = 1.0f;
    this->acceleration = 0.15f;
    this->weaponRotateTimer = 0.0f;

    float charH = 400.0f;
    float charW = 400.0f;
    this->width = charW;
    this->height = charH;

    // animation[0] — idle 
    LoopAnimation* idle = new LoopAnimation(4, 1.0f / 2.0f, false);
    idle->loadTexture("Sprites/seasatan-move.png");
    idle->setScale(charW / 188.0f, charH / 159.0f);
    idle->setActive(true);
    animations.add(idle);
    
    // animation[1] — move 
    LoopAnimation* move = new LoopAnimation(4, 1.0f / 5.0f, false);
    move->loadTexture("Sprites/seasatan-move.png");
    move->setScale(charW / 188.0f, charH / 159.0f);
    move->setActive(false);
    animations.add(move);
    
    // animation[2]
    LoopAnimation* retreat = new LoopAnimation(4, 1.0f / 16.0f, false);
    retreat->loadTexture("Sprites/seasatan-move.png");
    retreat->setScale(charW / 188.0f, charH / 159.0f);
    retreat->setActive(false);
    animations.add(retreat);

    weapon[0] = new HeavyMachineGun(this);
    weapon[1] = new RocketLauncher(this);
    weapon[2] = nullptr;

    this->x = x;
    this->y = y;
}

SeaSatan::~SeaSatan() {
}

void SeaSatan::changeHP(int change) {
    if (retreating)
        return;
    HP += change;
    if (HP <= 0) {
        HP = 0;
        retreating = true;
    }
}

void SeaSatan::movement() {
    PlayerSoldier* p = level->getCurrentPlayer();
    if (p == nullptr) {
        velocityX = 0.0f;
        velocityY = 0.0f;
        return;
    }

    if (retreating) {
        if (p->getX() > this->x)
            velocityX = -max_speed * 10.0f;
        else
            velocityX = max_speed * 10.0f;

        facingRight = (velocityX > 0);
        x += velocityX;
        return;
    }

    if (!playerInRange()) {
        velocityX = 0.0f;
        entityGravity();
        if (topMid != '~') y += gravity;
        clampToMap();
        return;
    }

    float dx = p->getX() - x;
    if (dx > 0) { facingRight = true;  velocityX = max_speed * 0.4f; }
    else if (dx < 0) { facingRight = false; velocityX = -max_speed * 0.4f; }

    float absDx = dx < 0 ? -dx : dx;
    if (absDx < screen_x * 0.15f) velocityX = 0.0f;

    if (velocityX != 0.0f) {
        updateTileChecks(x + velocityX, y);
        if (velocityX > 0 && (topRight !='~' ||midRight !='~' || bottomRight!='~')) velocityX = 0.0f;
        else if (velocityX < 0 && (topLeft != '~' || midLeft != '~' || bottomLeft != '~')) velocityX = 0.0f;
    }

    x += velocityX;
    entityGravity();
    if (topMid != '~') y += gravity;
    clampToMap();
}

void SeaSatan::updateAnimations() {
    int activeIndex = 0;

    if (retreating)
    {
        // death/retreat
        activeIndex = 2;
    }
    else if (velocityX != 0)
    {
        activeIndex = 1; // move
    }
    else
    {
        activeIndex = 0; // idle
    }

    // deactivate all, activate the chosen one
    for (int i = 0; i < animations.getSize(); i++)
    {
        animations[i]->setActive(i == activeIndex);
    }

    animations[activeIndex]->update();
    this->Tex = animations[activeIndex]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[activeIndex]->getRect());
    float sx = animations[activeIndex]->getScaleX();
    float sy = animations[activeIndex]->getScaleY();
    this->sprite.setScale(facingRight ? -sx : sx, sy);
}

void SeaSatan::attack() {
    if (retreating) {
        return;
    }

    PlayerSoldier* p = level->getCurrentPlayer();
    if (p == nullptr) {
        return;
    }

    if (weapon[currentWeapon]->isOutOfAmmo()) weapon[currentWeapon]->addAmmo(50);
    if (currentWeapon == 0) {
        weapon[0]->fire();
    }
    else {
        weapon[1]->fire();
    }

    weapon[currentWeapon]->update();

    weaponRotateTimer += deltaTime;
    if (weaponRotateTimer > 1.8f) {
        currentWeapon = (currentWeapon + 1) % 2;
        weaponRotateTimer = 0.0f;
    }
}

void SeaSatan::spawnNextBatch() {
    int batchSize = 2 + (rand() % 3);
    for (int i = 0; i < batchSize; i++) {
        float offset = (i + 1) * 100.0f;
        if (facingRight) {
            offset = -offset;
        }
        float spawnY = y + 32.0f;
        DamagableEntity* m = new Zombie(x + offset, spawnY, level);
        level->queueSpawn(m);
        currentBatch.add(m);
    }
}

void SeaSatan::behaviour() {
    if (dead) {
        return;
    }

    if (!retreating && HP <= maxHP / 2) {
        retreating = true;
    }

    if (retreating) {
        movement();
        if (x < level->getScreenLeft() - 256.0f || x > level->getScreenRight() + 256.0f) {
            dead = true;
            return;
        }
        updateAnimations();
        setAndDraw();
        return;
    }

    movement();
    attack();

    pruneDeadMinions();

    batchCooldown -= deltaTime;
    if (currentBatch.getSize() == 0 && batchCooldown <= 0.0f) {
        spawnNextBatch();
        batchCooldown = 5.5f;
    }
    updateAnimations();
    setAndDraw();
}







// ============= BOSS MODE BASE =============

BossMode::BossMode(int hp) : hp(hp), minionTimer(0.0f) {
}

BossMode::~BossMode() {
}

int BossMode::getHP() const {
    return hp;
}

void BossMode::changeHP(int delta) {
    hp += delta;
    if (hp < 0) {
        hp = 0;
    }
}



bool BossMode::isDead() const {
    return hp <= 0;
}

void BossMode::tickMinions(UltimateBoss* host) {
    minionTimer -= deltaTime;
    if (host->getCurrentMode() != this) {
        return;
    }
    if (host->getBatch().getSize() == 0 && minionTimer <= 0.0f) {
        spawnMinionBatch(host);
        minionTimer = 6.0f;
    }
}


// ============= GROUND MODE =============

GroundMode::GroundMode(int hp, UltimateBoss* host) : BossMode(hp) {
    rocket = new RocketLauncher(host);
    flame = new FlameShot(host);
    firebomb = new FireBombGrenade(host);
    currentWeapon = 0;
    weaponRotateTimer = 0.0f;
    sprite.loadFromFile("Sprites/iron-move.png");

    anim = new LoopAnimation(4, 1.0f / 5.0f, false);
    anim->loadTexture("Sprites/iron-move.png");
    anim->setScale(400.0f / 471.0f , 400.0f / 320.0f);
    anim->setActive(true);
}

GroundMode::~GroundMode() {
    delete rocket;
    delete flame;
    delete firebomb;
    delete anim;
}

void GroundMode::movement(UltimateBoss* host) {
    PlayerSoldier* p = host->getLevel()->getCurrentPlayer();
    if (p == nullptr) return;

    if (!host->playerInRange()) {
        host->setVelocityX(0.0f);
        host->entityGravity();
        host->clampToMap();
        return;
    }

    float dx = p->getX() - host->getX();
    float vx = 0.0f;
    if (dx > 0) { host->setFacingRight(true);  vx = 1.5f * 0.4f; }
    else if (dx < 0) { host->setFacingRight(false); vx = -1.5f * 0.4f; }

    float absDx = dx < 0 ? -dx : dx;
    if (absDx < screen_x * 0.15f) host->setVelocityX(0.0f);

    if (vx != 0.0f) {
        host->updateTileChecks(host->getX() + vx, host->getY());
        if (vx > 0 && host->horizontalCollisionCheck(host->getTopMid(), host->getMidRight(), host->getBottomRight())) vx = 0.0f;
        else if (vx < 0 && host->horizontalCollisionCheck(host->getTopMid(), host->getMidLeft(), host->getBottomLeft())) vx = 0.0f;
    }

    host->setVelocityX(vx);
    host->setX(host->getX() + vx);
    host->entityGravity();
    host->clampToMap();
}

void GroundMode::attack(UltimateBoss* host) {
    PlayerSoldier* p = host->getLevel()->getCurrentPlayer();
    if (p == nullptr) {
        return;
    }

    if (currentWeapon == 0) {
        if (rocket->isOutOfAmmo()) rocket->addAmmo(50);
        rocket->fire();
        rocket->update();
    }
    else if (currentWeapon == 1) {
        if (flame->isOutOfAmmo()) flame->addAmmo(50);
        flame->fire();
        flame->update();
    }
    else {
        if (firebomb->isOutOfAmmo()) firebomb->addAmmo(50);
        firebomb->fire();
        firebomb->update();
    }

    weaponRotateTimer += deltaTime;
    if (weaponRotateTimer > 2.0f) {
        currentWeapon = (currentWeapon + 1) % 3;
        weaponRotateTimer = 0.0f;
    }
}

void GroundMode::updateAnimations(UltimateBoss* host) {
    anim->update();

    host->setTex(anim->getTexture());
    host->getSprite().setTexture(host->getTex());
    host->getSprite().setTextureRect(anim->getRect());

    float sx = anim->getScaleX();
    float sy = anim->getScaleY();
    host->getSprite().setScale(host->getFacingRight() ? -sx : sx, sy);
}

void GroundMode::applySprite(UltimateBoss* host) {
}

void GroundMode::spawnMinionBatch(UltimateBoss* host) {
    int batchSize = 2 + (rand() % 3);
    Level* lvl = host->getLevel();
    for (int i = 0; i < batchSize; i++) {
        float offset = (i + 1) * 90.0f;
        if (host->getFacingRight()) {
            offset = -offset;
        }
        DamagableEntity* m;
        if (i % 2 == 0) {
            m = new RebelSoldier(host->getX() + offset, host->getY(), lvl);
        }
        else {
            m = new BazookaSoldier(host->getX() + offset, host->getY(), lvl);
        }
        lvl->queueSpawn(m);
        host->getBatch().add(m);
    }
}


// ============= AERIAL MODE =============

AerialMode::AerialMode(int hp, UltimateBoss* host) : BossMode(hp) {
    rocket = new RocketLauncher(host);
    firebomb = new FireBombGrenade(host);
    currentWeapon = 0;
    weaponRotateTimer = 0.0f;
    sprite.loadFromFile("Sprites/airbust-move.png");

    anim = new LoopAnimation(3, 1.0f / 5.0f, false);
    anim->loadTexture("Sprites/airbust-move.png");
    anim->setScale(400.0f / 496.0f, 400.0f / 379.0f);
    anim->setActive(true);
}

AerialMode::~AerialMode() {
    delete rocket;
    delete firebomb;
    delete anim;
}

void AerialMode::movement(UltimateBoss* host) {
    PlayerSoldier* p = host->getLevel()->getCurrentPlayer();
    if (p == nullptr) return;

    if (!host->playerInRange()) {
        host->setVelocityX(0.0f);
        host->setVelocityY(0.0f);
        return;
    }

    float dx = p->getX() - host->getX();
    float vx = 0.0f;
    if (dx > 0) { host->setFacingRight(true);  vx = 3.0f * 0.5f; }
    else if (dx < 0) { host->setFacingRight(false); vx = -3.0f * 0.5f; }

    float absDx = dx < 0 ? -dx : dx;
    if (absDx < screen_x * 0.15f) vx = 0.0f;

    float targetY = p->getY() - 320.0f;
    float dy = targetY - host->getY();
    float vy = 0.0f;
    if (dy > 4.0f) vy = 1.2f;
    else if (dy < -4.0f) vy = -1.2f;

    if (vx != 0.0f) {
        host->updateTileChecks(host->getX() + vx, host->getY());
        if (vx > 0 && host->horizontalCollisionCheck(host->getTopRight(), host->getMidRight(), host->getBottomRight())) vx = 0.0f;
        else if (vx < 0 && host->horizontalCollisionCheck(host->getTopLeft(), host->getMidLeft(), host->getBottomLeft())) vx = 0.0f;
    }
    if (vy != 0.0f) {
        host->updateTileChecks(host->getX(), host->getY() + vy);
        char t = host->getTopMid();
        char b = host->getBelowMid();
        if (vy < 0 && (t == '#' || t == 'D')) vy = 0.0f;
        else if (vy > 0 && (b == '#' || b == 'D')) vy = 0.0f;
    }

    host->setVelocityX(vx);
    host->setVelocityY(vy);
    host->setX(host->getX() + vx);
    host->setY(host->getY() + vy);
    host->clampToMap();
}

void AerialMode::attack(UltimateBoss* host) {
    PlayerSoldier* p = host->getLevel()->getCurrentPlayer();
    if (p == nullptr) {
        return;
    }

    if (currentWeapon == 0) {
        if (rocket->isOutOfAmmo()) rocket->addAmmo(50);
        rocket->fire();
        rocket->update();
    }
    else {
        if (firebomb->isOutOfAmmo()) firebomb->addAmmo(50);
        firebomb->fire();
        firebomb->update();
    }

    weaponRotateTimer += deltaTime;
    if (weaponRotateTimer > 2.5f) {
        currentWeapon = (currentWeapon + 1) % 2;
        weaponRotateTimer = 0.0f;
    }
}

void AerialMode::updateAnimations(UltimateBoss* host) {
    anim->update();

    host->setTex(anim->getTexture());
    host->getSprite().setTexture(host->getTex());
    host->getSprite().setTextureRect(anim->getRect());

    float sx = anim->getScaleX();
    float sy = anim->getScaleY();
    host->getSprite().setScale(host->getFacingRight() ? -sx : sx, sy);
}

void AerialMode::applySprite(UltimateBoss* host) {
 
}

void AerialMode::spawnMinionBatch(UltimateBoss* host) {
    int batchSize = 2 + (rand() % 3);
    Level* lvl = host->getLevel();
    for (int i = 0; i < batchSize; i++) {
        float offset = (i + 1) * 100.0f;
        if (host->getFacingRight()) {
            offset = -offset;
        }
        DamagableEntity* m;
        if (i % 2 == 0) {
            m = new ParaTrooper(host->getX() + offset, host->getY() + 64.0f, lvl, rand() % 4);
        }
        else {
            m = new FlyingTara(host->getX() + offset, host->getY(),  lvl);
        }
        lvl->queueSpawn(m);
        host->getBatch().add(m);
    }
}


// ============= AQUATIC MODE =============

AquaticMode::AquaticMode(int hp, UltimateBoss* host) : BossMode(hp) {
    hmg = new HeavyMachineGun(host);
    rocket = new RocketLauncher(host);
    currentWeapon = 0;
    weaponRotateTimer = 0.0f;
    sprite.loadFromFile("Sprites/seasatan-move.png");

    anim = new LoopAnimation(4, 1.0f / 5.0f, false);
    anim->loadTexture("Sprites/seasatan-move.png");
    anim->setScale(400.0f / 190.f, 400.0f / 159.0f);
    anim->setActive(true);
}

AquaticMode::~AquaticMode() {
    delete hmg;
    delete rocket;
    delete anim;
}

void AquaticMode::movement(UltimateBoss* host) {
    PlayerSoldier* p = host->getLevel()->getCurrentPlayer();
    if (p == nullptr) return;

    if (!host->playerInRange()) {
        host->setVelocityX(0.0f);
        host->entityGravity();
        float y = host->getY();
        if (host->getTopMid() != '~') host->setY( y += gravity);
        host->clampToMap();
        return;
    }

    float dx = p->getX() - host->getX();
    float vx = 0.0f;
    if (dx > 0) { host->setFacingRight(true);  vx = 1.0f; }
    else if (dx < 0) { host->setFacingRight(false); vx = -1.0f; }

    float absDx = dx < 0 ? -dx : dx;
    if (absDx < screen_x * 0.15f) vx = 0.0f;

    if (vx != 0.0f) {
        host->updateTileChecks(host->getX() + vx, host->getY());
        if (vx > 0 && (host->getTopRight() != '~' || host->getMidRight() != '~' || host->getBottomRight() != '~')) vx = 0.0f;
        else if (vx < 0 && (host->getTopLeft() != '~' || host->getMidLeft() != '~' || host->getBottomLeft() != '~')) vx = 0.0f;
    }

    host->setVelocityX(vx);
    host->setX(host->getX() + vx);

    float targetY = p->getY() + 2.0f * cell_size;
    float dy = targetY - host->getY();
    host->updateTileChecks(host->getX(), host->getY());
    if (dy < -4.0f && host->getTopMid() == '~') {
        host->setY(host->getY() - 1.0f * 0.5f * 2.0f);
    }
    else if (dy > 4.0f && host->getBottomMid() == '~') {
        host->setY(host->getY() + 1.0f * 0.5f * 2.0f);
    }
    else {
        host->entityGravity();
        if (host->getTopMid() != '~') host->setY(host->getY() + gravity);
    }

    host->clampToMap();
}

void AquaticMode::attack(UltimateBoss* host) {
    PlayerSoldier* p = host->getLevel()->getCurrentPlayer();
    if (p == nullptr) {
        return;
    }

    if (currentWeapon == 0) {
        if (hmg->isOutOfAmmo()) hmg->addAmmo(50);
        hmg->fire();
        hmg->update();
    }
    else {
        if (rocket->isOutOfAmmo()) rocket->addAmmo(50);
        rocket->fire();
        rocket->update();
    }

    weaponRotateTimer += deltaTime;
    if (weaponRotateTimer > 1.8f) {
        currentWeapon = (currentWeapon + 1) % 2;
        weaponRotateTimer = 0.0f;
    }
}

void AquaticMode::updateAnimations(UltimateBoss* host) {
    anim->update();

    host->setTex(anim->getTexture());
    host->getSprite().setTexture(host->getTex());
    host->getSprite().setTextureRect(anim->getRect());

    float sx = anim->getScaleX();
    float sy = anim->getScaleY();
    host->getSprite().setScale(host->getFacingRight() ? -sx : sx, sy);
}

void AquaticMode::applySprite(UltimateBoss* host) {
   // host->setTex(sprite);
}

void AquaticMode::spawnMinionBatch(UltimateBoss* host) {
    int batchSize = 2 + (rand() % 3);
    Level* lvl = host->getLevel();
    for (int i = 0; i < batchSize; i++) {
        float offset = (i + 1) * 100.0f;
        if (host->getFacingRight()) {
            offset = -offset;
        }
        DamagableEntity* m = new Zombie(host->getX() + offset, host->getY() + 32.0f, lvl);
        lvl->queueSpawn(m);
        host->getBatch().add(m);
    }
}


// ============= ULTIMATE BOSS =============

UltimateBoss::UltimateBoss(float x, float y, Level* levelSkeleton) : Boss(x, y, levelSkeleton) {
    this->HP = 1;
    this->maxHP = 1;
    this->width = 192.0f;
    this->height = 128.0f;
    this->max_speed = 2.0f;
    this->acceleration = 0.3f;
    this->modeTimer = 0.0f;
    this->modeDuration = 12.0f;
    this->currentModeIndex = 0;

    this->transitioning = false;
    this->transitionTimer = 0.0f;
    this->transitionDuration = 1.0f;
    this->pendingModeIndex = 0;
    this->startY = y;
    this->targetY = y;

     this->introPlaying = true;
     this->introPhase = 0;
     this->introTimer = 0.0f;
     this->explosionSpawned = false;
    
     // collision point = boss spawn
     this->collisionX = x;
     this->collisionY = y;
    
     // ground Y: find the surface row near spawn so ground sprites sit on the floor
     Map* m = levelSkeleton->getMap();
     float groundY = y;  // fallback
     if (m != nullptr) {
         int cs = m->getCellSize();
         int col = (int)(x / cs);
         char**& lvl = m->getLvl();
         for (int r = 0; r < m->getHeight(); r++) {
             if (lvl[r][col] == '#' || lvl[r][col] == 'D' || lvl[r][col] == 'X') {
                 groundY = (float)(r * cs) - 400.0f;  // sit on surface, sprite is 400 tall
                 break;
             }
         }
     }
    
     float ironTravelDist = 500.0f;   // how far off-screen left
     float satanTravelDist = 500.0f;   // how far off-screen right
     float hairTravelDist = 500.0f;   // how far off-screen top
    
     // Iron Nokana: comes from left, sits on ground
     this->ironIntroX = collisionX - ironTravelDist;
     this->ironIntroY = groundY;
    
     // Sea Satan: comes from right, sits on ground
     this->satanIntroX = collisionX + satanTravelDist;
     this->satanIntroY = groundY;
    
     // Hair Buster: comes from top, centered on collision X
     this->hairIntroX = collisionX;
     this->hairIntroY = collisionY - hairTravelDist;
    
     // Iron Nokana move animation
     introIronAnim = new LoopAnimation(4, 1.0f / 5.0f, false);
     introIronAnim->loadTexture("Sprites/iron-move.png");
     introIronAnim->setScale(400.0f / 471.0f, 400.0f / 320.0f);
     introIronAnim->setActive(true);
    
     // Hair Buster move animation
     introHairAnim = new LoopAnimation(3, 1.0f / 5.0f, false);
     introHairAnim->loadTexture("Sprites/airbust-move.png");
     introHairAnim->setScale(400.0f / 496.0f , 400.0f / 379.0f);
     introHairAnim->setActive(true);
    
     // Sea Satan move animation
     introSatanAnim = new LoopAnimation(4, 1.0f / 5.0f, false);
     introSatanAnim->loadTexture("Sprites/seasatan-move.png");
     introSatanAnim->setScale(400.0f / 190.f, 400.0f / 150.f);
     introSatanAnim->setActive(true);
    
     modes[0] = new GroundMode(100, this);
     modes[1] = new AerialMode(100, this);
     modes[2] = new AquaticMode(100, this);
    
     this->setScaleX(this->width / 400.0f);
     this->setScaleY(this->height / 400.0f);
     this->initializeScale();
     this->initializeDimensions();
}

UltimateBoss::~UltimateBoss() {
    for (int i = 0; i < 3; i++) {
        if (modes[i] != nullptr) {
            delete modes[i];
            modes[i] = nullptr;
        }
    }
    delete introIronAnim;
    delete introHairAnim;
    delete introSatanAnim;
}

BossMode* UltimateBoss::getCurrentMode() {
    if (currentModeIndex < 0 || currentModeIndex >= 3) {
        return nullptr;
    }
    return modes[currentModeIndex];
}

MyVector<DamagableEntity*>& UltimateBoss::getBatch() {
    return currentBatch;
}

void UltimateBoss::cycleMode() {
    int start = currentModeIndex;
    int next = (currentModeIndex + 1) % 3;
    while (next != start) {
        if (modes[next] != nullptr) {
            if (next == 0) {
                this->y -= 640;
            }
            if (next == 1) {
                this->y -= 640;
            }

            if (next == 2) {
                this->y += 320;
            }
            currentModeIndex = next;
            modeTimer = 0.0f;
            modes[next]->applySprite(this);
            return;
        }
        next = (next + 1) % 3;
    }
    if (modes[currentModeIndex] == nullptr) {
        dead = true;
    }
}
void UltimateBoss::changeHP(int delta) {
    if (introPlaying)
        return;
    if (currentModeIndex < 0 || currentModeIndex >= 3) {
        return;
    }
    BossMode* m = modes[currentModeIndex];
    if (m == nullptr) {
        return;
    }
    m->changeHP(delta);
    if (m->isDead()) {
        delete modes[currentModeIndex];
        modes[currentModeIndex] = nullptr;
        ScoreManager::instance().addScore(ScoreManager::SCORE_NORMAL_BOSS);
        bool anyAlive = false;
        for (int i = 0; i < 3; i++) if (modes[i] != nullptr) { anyAlive = true; break; }
        if (!anyAlive) {
            ScoreManager::instance().addScore(ScoreManager::SCORE_ULTIMATE_BOSS);
        }
        cycleMode();
    }
}

bool UltimateBoss::shouldBeRemoved() const {
    if (dead) {
        return true;
    }
    bool anyAlive = false;
    for (int i = 0; i < 3; i++) {
        if (modes[i] != nullptr) {
            anyAlive = true;
        }
    }
    return !anyAlive;
}

void UltimateBoss::movement() {
    BossMode* m = getCurrentMode();
    if (m == nullptr) {
        return;
    }
    m->movement(this);
}

void UltimateBoss::attack() {
    BossMode* m = getCurrentMode();
    if (m == nullptr) {
        return;
    }
    m->attack(this);
}

void UltimateBoss::updateAnimations() {
    BossMode* m = getCurrentMode();
    if (m == nullptr) {
        return;
    }
    m->updateAnimations(this);
}


void UltimateBoss::drawIntroSprite(LoopAnimation* anim, float worldX, float worldY, bool flipX) {
    anim->update();
    Sprite s;
    s.setTexture(anim->getTexture());
    s.setTextureRect(anim->getRect());

    float sx = anim->getScaleX();
    float sy = anim->getScaleY();

    float drawX = worldX - level->getScreenLeft();
    float drawY = worldY - level->getScreenTop();

    if (flipX) {
        s.setScale(-sx, sy);
        drawX += 400.0f;
    }
    else {
        s.setScale(sx, sy);
    }

    s.setPosition(drawX, drawY);
    window->draw(s);
}

void UltimateBoss::runIntro() {
    float slideSpeed = 5.0f;

    if (introPhase == 0) {

        // --- Iron Nokana: move right until it reaches collisionX ---
        bool ironArrived = false;
        if (ironIntroX < collisionX) {
            ironIntroX += slideSpeed;
            if (ironIntroX >= collisionX) {
                ironIntroX = collisionX;
                ironArrived = true;
            }
        }
        else {
            ironArrived = true;
        }

        // --- Sea Satan: move left until it reaches collisionX ---
        bool satanArrived = false;
        if (satanIntroX > collisionX) {
            satanIntroX -= slideSpeed;
            if (satanIntroX <= collisionX) {
                satanIntroX = collisionX;
                satanArrived = true;
            }
        }
        else {
            satanArrived = true;
        }

        // --- Hair Buster: move down until it reaches collisionY ---
        bool hairArrived = false;
        if (hairIntroY < collisionY) {
            hairIntroY += slideSpeed;
            if (hairIntroY >= collisionY) {
                hairIntroY = collisionY;
                hairArrived = true;
            }
        }
        else {
            hairArrived = true;
        }

        // draw all three with their move animations
        drawIntroSprite(introIronAnim, ironIntroX, ironIntroY, false); // faces right
        drawIntroSprite(introSatanAnim, satanIntroX, satanIntroY, true);  // faces left (flipped)
        drawIntroSprite(introHairAnim, hairIntroX, hairIntroY, false); // comes from top

        if (ironArrived && satanArrived && hairArrived) {
            if (!explosionSpawned) {
                float cx = collisionX + 200.0f;
                float cy = collisionY + 150.0f;
                level->queueSpawn(new Explosion(cx, cy, 350.0f, level));
                level->queueSpawn(new Explosion(cx - 180.0f, cy + 80.0f, 250.0f, level));
                level->queueSpawn(new Explosion(cx + 180.0f, cy - 60.0f, 250.0f, level));
                explosionSpawned = true;
            }
            introPhase = 1;
            introTimer = 2.0f;
        }
    }

    else if (introPhase == 1) {
        // hold all three at collision point while explosions play
        // stop updating their animations (they're merged now)
        introTimer -= deltaTime;
        if (introTimer <= 0.0f)
            introPhase = 2;
    }

    else if (introPhase == 2) {
        this->x = collisionX;
        this->y = collisionY;
        introPlaying = false;
        modes[0]->applySprite(this);
    }
}

void UltimateBoss::behaviour() {
    if (introPlaying) {
    runIntro();
    return;
    }
    
    if (shouldBeRemoved()) {
        return;
    }

    if (transitioning) {
        transitionTimer -= deltaTime;
        if (transitionTimer <= 0.0f) {
            currentModeIndex = pendingModeIndex;
            modeTimer = 0.0f;
            transitioning = false;
            BossMode* m = getCurrentMode();
            if (m != nullptr) m->applySprite(this);
            y = targetY;
        }
        else {
            if (y < targetY) y += 5;
            else if (y > targetY) y -= 5;
        }
        setAndDraw();
        return;
    }

    modeTimer += deltaTime;
    if (modeTimer >= modeDuration) {
        cycleMode();
    }

    BossMode* m = getCurrentMode();
    if (m == nullptr) {
        return;
    }

    m->movement(this);
    m->attack(this);
    m->updateAnimations(this);

    pruneDeadMinions();
    m->tickMinions(this);
    setAndDraw();
}
