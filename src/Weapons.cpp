#include "project.h"


// ============= WEAPON =============

Weapon::Weapon() {
    this->owner = nullptr;
    this->damageDealt = 0.0f;
    this->range = 0.0f;
    this->angle = 0.0f;
    this->fireRate = 0.0f;
    this->baseFireRate = 0.0f;
    this->fireInBothDirections = false;
    this->cooldownTimer = 0.0f;
    this->fireAnim = nullptr;
    this->muzzleOffsetX = 0.0f;
    this->muzzleOffsetY = 0.0f;
}


Weapon::Weapon(DamagableEntity* owner) {
    this->owner = owner;
    this->damageDealt = 0.0f;
    this->range = 0.0f;
    this->angle = 0.0f;
    this->fireRate = 0.0f;
    this->baseFireRate = 0.0f;
    this->fireInBothDirections = false;
    this->cooldownTimer = 0.0f;
    this->fireAnim = nullptr;
    this->muzzleOffsetX = 0.0f;
    this->muzzleOffsetY = 0.0f;
}

Weapon::~Weapon() {
    if (fireAnim != nullptr) {
        delete fireAnim;
        fireAnim = nullptr;
    }
}

void Weapon::resetFireRate() {
    fireRate = baseFireRate;
    cooldownTimer = 0.0f;
}

bool Weapon::canFire() {
    if (cooldownTimer > 0) return false;
    cooldownTimer = 1.0f / fireRate;
    return true;
}

void Weapon::update() {
    if (cooldownTimer > 0) {
        cooldownTimer -= 1.0f / 60.0f;
        if (cooldownTimer < 0) cooldownTimer = 0;
    }
	if (fireAnim != nullptr && fireAnim->isActive())
    fireAnim->update();
}

bool Weapon::isInRange(Entity* E) {
    // fill later
    return false;
}

float Weapon::getFireRate() {
    return this->fireRate;
}

void Weapon::boostFireRate(float multiplier) {
    if (baseFireRate == 0.0f) 
        baseFireRate = fireRate;  // store on first boost
    fireRate *= multiplier;
}

float Weapon::getAngle() {
    return this->angle;
}

float Weapon::getDamage() {
    return this->damageDealt;
}

void Weapon::setDamage(float d) {
    this->damageDealt = d;
}

float Weapon::getRange() {
    return this->range;
}

void Weapon::collect(DamagableEntity* owner) {
    this->owner = owner;
}


// ============= PROJECTILE WEAPON =============

ProjectileWeapon::ProjectileWeapon() {
    this->totalAmmo = 0;
    this->currentMag = 0;
    this->magazineSize = 0;
    this->reloadTime = 0.0f;
    this->reloadTimer = 0.0f;
    this->reloading = false;
}

void ProjectileWeapon::applyExplosionEffect(int x, int y, int damage) {
}

int ProjectileWeapon::getTotalAmmo() {
    return this->totalAmmo;
}

int ProjectileWeapon::getCurrentMag() {
    return this->currentMag;
}

bool ProjectileWeapon::isReloading() {
    return this->reloading;
}

bool ProjectileWeapon::isOutOfAmmo() {
    if (hasInfiniteAmmo()) return false;
    return totalAmmo <= 0 && currentMag <= 0;
}

void ProjectileWeapon::reload() {
    if (reloading) return;
    if (currentMag == magazineSize) return;
    if (totalAmmo <= 0) return;
    reloading = true;
    reloadTimer = reloadTime;
}

void ProjectileWeapon::update() {
    Weapon::update();
    if (hasInfiniteAmmo()) return;
    if (!reloading) return;

    reloadTimer -= 1.0f / 60.0f;
    if (reloadTimer > 0) return;

    int needed = magazineSize - currentMag;
    int give;
    if (totalAmmo < needed) give = totalAmmo;
    else give = needed;

    currentMag += give;
    totalAmmo -= give;
    reloading = false;
    reloadTimer = 0;
}

void ProjectileWeapon::fireAtAngle(float angle) {
    if (owner == nullptr) return;
    float rad = angle * 3.14159265f / 180.0f;
    float spawnX = owner->getX() + owner->getWidth() / 2 + cos(rad) * owner->getWidth() * 2;
    float spawnY = owner->getY() + owner->getHeight() / 5 + sin(rad) * owner->getWidth() * 2;
    Bullet* b = new Bullet(owner, (int)this->damageDealt, this->range, spawnX, spawnY, angle, level);
    level->queueSpawn(b);
}

// ============= KNIFE =============

Knife::Knife(DamagableEntity* owner) : Weapon(owner)
{
    this->fireRate = 2.0f;
    this->attacking = false;
    this->pierce = false;
    this->damageDealt = 2.0f;
    this->range = cell_size;
    this->max_speed = 0;
    this->level = owner->getLevel();
}


void Knife::fire()
{
    if (!canFire()) return;
    if (owner == nullptr) return;
    owner->onMeleeStart();
    this->attacking = true;

    float zoneX;
    if (owner->getFacingRight())
        zoneX = owner->getX() + owner->getWidth();
    else
        zoneX = owner->getX() - this->range;

    DamageZone* zone = new OneShotZone(zoneX, owner->getY(), this->range, owner->getHeight(), this->damageDealt, owner, this->pierce);
    owner->getLevel()->addDamageZone(zone);
}

// virtual {}
// 


void Knife::update()
{
    bool wasOnCooldown = (cooldownTimer > 0);
    Weapon::update();
    if (wasOnCooldown && cooldownTimer == 0 && attacking) {
        attacking = false;
        owner->onMeleeEnd();
    }
}



void Knife::setPierceShield() {
    pierce = true;
}

void Knife::drop() {
    this->setX(owner->getX());
    this->setY(owner->getY());
    owner = nullptr;
}

void Knife::collect(DamagableEntity* owner) {
    this->owner = owner;
}

void Knife::movement() {
    if (owner == nullptr) return;
    this->setX(owner->getX());
    this->setY(owner->getY());
    clampToMap();
}

Knife::~Knife() {

}

bool Knife::canPierceShield() {
    return pierce;
}
void Knife::updateAnimations() {}
void Knife::behaviour(){
    this->movement();
    this->update();
    this->updateAnimations();
}

// ============= BULLET =============

Bullet::Bullet(bool damagesPlayers, int damage, float range, float spawnX, float spawnY, float angle, Level* level)
    : StraightProjectile(damagesPlayers, damage, range, angle, level) {
    this->max_speed = 25.0f;
    this->x = spawnX;
    this->y = spawnY;

    this->getTex().loadFromFile("Sprites/bullet.png");
    this->setScaleX(64.0f / 138.0f);
    this->setScaleY(25.0f / 25.0f);
    this->initializeScale();
    this->initializeDimensions();
}

void Bullet::movement() {
    StraightProjectile::movement();
    clampToMap();
}

void Bullet::fatPlayerBulletMovement() {
}

void Bullet::updateAnimations() {
}

void Bullet::behaviour() {
    if (*dead) {
        removeMe = true;
        return;
    }
    if (distanceTraveled >= range) {
        removeMe = true;
        return;
    }

    movement();

    updateTileChecks(this->x + this->width / 2, this->y + this->height / 2);
    if ((midLeft != ' ' && midLeft != '~') || (midRight != ' ' && midRight != '~') || (topMid != ' ' && topMid != '~') || (bottomMid != ' ' && bottomMid != '~')) {
        removeMe = true;
        return;
    }

    DamageZone* z = new OneShotZone(this->x, this->y, this->width, this->height, damage, damagesPlayers, false, dead);
    level->addDamageZone(z);

    setAndDraw();
}

bool Bullet::shouldBeRemoved() const {
    return removeMe;
}

void Bullet::writeStateTo(CurrentState& s) const {
    if (s.bulletCount >= 200) return;
    BulletState& b = s.bullets[s.bulletCount];
    b.id = entityId;
    b.x = getX();
    b.y = getY();
    b.angle = angle;
    b.active = true;
    s.bulletCount++;
}

void Bullet::applyStateFrom(const CurrentState& s) {
    for (int i = 0; i < s.bulletCount; i++) {
        if (s.bullets[i].id != entityId) continue;
        const BulletState& b = s.bullets[i];
        setX(b.x);
        setY(b.y);
        angle = b.angle;
        return;
    }
}


// ============= PISTOL =============

Pistol::Pistol(DamagableEntity* owner) : ProjectileWeapon() {
    this->owner = owner;
    this->level = owner->getLevel();
    this->damageDealt = 3.0f;
    this->range = screen_x * 0.75f;
    this->fireRate = 4.0f;
    this->totalAmmo = -1;
    this->magazineSize = -1;
    this->currentMag = -1;
    this->reloadTime = 0;
	this->baseFireRate = this->fireRate;

	 // // muzzle flash
	 // OneShotAnimation* muzzle = new OneShotAnimation(2, 1.0f / 12.0f, true);
	 // muzzle->loadTexture("Sprites/muzzle-pistol.png");
	 // if (muzzle->getTexture().getSize().x > 0)
	 // {
	 //     muzzle->setScale(32.0f / (muzzle->getTexture().getSize().x / 2.0f), 32.0f / muzzle->getTexture().getSize().y);
	 // }
	 // muzzle->setActive(false);
	 // this->fireAnim = muzzle;
	 // this->muzzleOffsetX = 4.0f;
	 // this->muzzleOffsetY = -4.0f;
}

Pistol::Pistol(float x, float y, Level* level) : ProjectileWeapon() {
    this->x = x;
    this->y = y;
    this->level = level;
    this->damageDealt = 3.0f;
    this->range = screen_x * 0.75f;
    this->fireRate = 4.0f;
    this->totalAmmo = -1;
    this->magazineSize = -1;
    this->currentMag = -1;
    this->reloadTime = 0;
	this->baseFireRate = this->fireRate;
}

void Pistol::fire() {
    if (!canFire()) return;
    if (owner == nullptr) return;

    float a = owner->getShootAngle();
    
	float spawnX;
    if (owner->getFacingRight()) {
        spawnX = owner->getX() + owner->getWidth();
    }
    else {
        spawnX = owner->getX();
    }
    float spawnY = owner->getY() + owner->getHeight()/5;

    Bullet* b = new Bullet(owner->isHostileToPlayers(), (int)this->damageDealt, this->range, spawnX, spawnY, a, level);
    level->queueSpawn(b);

	if (fireAnim != nullptr) {
        fireAnim->reset();
        fireAnim->setActive(true);
    }
    playSFX(SFX_SHOOT);
}

void Pistol::reload() {
}

bool Pistol::hasInfiniteAmmo() {
    return true;
}





// ==================== HEAVYYYYYY MACHINE GUN=================
HeavyMachineGun::HeavyMachineGun(DamagableEntity* owner) : ProjectileWeapon() {
    this->owner = owner;
    this->level = owner->getLevel();
    this->damageDealt = 3.0f;
    this->range = screen_x * 0.75f;
    this->fireRate = 8.0f;
    this->magazineSize = 30;
    this->currentMag = 30;
    this->totalAmmo = 90;
    this->reloadTime = 1.5f;
	this->baseFireRate = this->fireRate;
	//OneShotAnimation* muzzle = new OneShotAnimation(1, 1.0f / 12.0f, false);
    //muzzle->loadTexture("Sprites/marco-idle.png"); // PLACEHOLDER
    //muzzle->setScale(48.0f / 100.0f, 48.0f / 100.0f);
    //muzzle->setActive(false);
    //this->fireAnim = muzzle;
    //this->muzzleOffsetX = 50.0f;
    //this->muzzleOffsetY = 8.0f;
}

HeavyMachineGun::HeavyMachineGun(float x, float y, Level* level) : ProjectileWeapon() {
    this->x = x;
    this->y = y;
    this->level = level;
    this->damageDealt = 3.0f;
    this->range = screen_x * 0.75f;
    this->fireRate = 8.0f;
    this->magazineSize = 30;
    this->currentMag = 30;
    this->totalAmmo = 90;
    this->reloadTime = 1.5f;
	this->baseFireRate = this->fireRate;
}

void HeavyMachineGun::fire() {
    if (!canFire()) return;
    if (reloading) return;
    if (owner == nullptr) return;
    if (currentMag <= 0) {
        reload();
        return;
    }

    float a = owner->getShootAngle();
	
	float spawnX;
    if (owner->getFacingRight()) spawnX = owner->getX() + owner->getWidth();
    else spawnX = owner->getX();
    float spawnY = owner->getY() + owner->getHeight() / 5;

    Bullet* b = new Bullet(owner->isHostileToPlayers(), (int)this->damageDealt, this->range, spawnX, spawnY, a, level);
    level->queueSpawn(b);

    currentMag--;
    if (currentMag == 0) reload();
	if (fireAnim != nullptr) {
        fireAnim->reset();
        fireAnim->setActive(true);
    }
    playSFX(SFX_SHOOT);
}





// ============= ROCKET =============

Rocket::Rocket(bool damagesPlayers, int damage, float range, float blastRadius, float spawnX, float spawnY, float angle, Level* level)
    : StraightProjectile(damagesPlayers, damage, range, angle, level) {
    this->blastRadius = blastRadius;
    this->max_speed = 10.0f;
    this->x = spawnX;
    this->y = spawnY;

    this->getTex().loadFromFile("Sprites/bullet.png");
    this->setScaleX(80.0f / 138.0f);
    this->setScaleY(50.0f / 25.0f);
    this->initializeScale();
    this->initializeDimensions();
}

void Rocket::movement() {
    StraightProjectile::movement();
    clampToMap();
}

void Rocket::updateAnimations() {
}

void Rocket::behaviour() {
    if (*dead) {
        float cx = this->x + this->width / 2;
        float cy = this->y + this->height / 2;
        float zoneX = cx - blastRadius;
        float zoneY = cy - blastRadius;
        float zoneSize = blastRadius * 2;
        DamageZone* boom = new ExplosionZone(zoneX, zoneY, zoneSize, zoneSize, damage, damagesPlayers, true, nullptr);
        level->addDamageZone(boom);
		level->queueSpawn(new Explosion(cx, cy, blastRadius, level));
        removeMe = true;
        return;
    }
    if (distanceTraveled >= range) {
        float cx = this->x + this->width / 2;
        float cy = this->y + this->height / 2;
        float zoneX = cx - blastRadius;
        float zoneY = cy - blastRadius;
        float zoneSize = blastRadius * 2;
        DamageZone* boom = new ExplosionZone(zoneX, zoneY, zoneSize, zoneSize, damage, damagesPlayers, true, nullptr);
        level->addDamageZone(boom);
		level->queueSpawn(new Explosion(cx, cy, blastRadius, level));
        removeMe = true;
        return;
    }

    movement();

    updateTileChecks(this->x + this->width / 2, this->y + this->height / 2);
    if ((midLeft != ' ' && midLeft != '~') || (midRight != ' ' && midRight != '~') || (topMid != ' ' && topMid != '~') || (bottomMid != ' ' && bottomMid != '~')) {
        float cx = this->x + this->width / 2;
        float cy = this->y + this->height / 2;
        float zoneX = cx - blastRadius;
        float zoneY = cy - blastRadius;
        float zoneSize = blastRadius * 2;
        DamageZone* boom = new ExplosionZone(zoneX, zoneY, zoneSize, zoneSize, damage, damagesPlayers, true, nullptr);
        level->addDamageZone(boom);
		level->queueSpawn(new Explosion(cx, cy, blastRadius * 2, level));
        removeMe = true;
        return;
    }

    DamageZone* contact = new OneShotZone(this->x, this->y, this->width, this->height, 0, damagesPlayers, false, dead);
    level->addDamageZone(contact);

    setAndDraw();
}

bool Rocket::shouldBeRemoved() const {
    return removeMe;
}




// ============= ROCKET LAUNCHER =============

RocketLauncher::RocketLauncher(DamagableEntity* owner) : ProjectileWeapon() {
    this->owner = owner;
    this->level = owner->getLevel();
    this->damageDealt = 5.0f;
    this->range = screen_x * 0.75f;
    this->fireRate = 1.0f;
    this->magazineSize = 1;
    this->currentMag = 1;
    this->totalAmmo = 10;
    this->reloadTime = 2.0f;
    this->blastRadius = 3 * 64.0f;
	this->baseFireRate = this->fireRate;
	//OneShotAnimation* muzzle = new OneShotAnimation(1, 1.0f / 12.0f, false);
    //muzzle->loadTexture("Sprites/marco-idle.png"); // PLACEHOLDER
    //muzzle->setScale(48.0f / 100.0f, 48.0f / 100.0f);
    //muzzle->setActive(false);
    //this->fireAnim = muzzle;
    //this->muzzleOffsetX = 50.0f;
    //this->muzzleOffsetY = 8.0f;
}

RocketLauncher::RocketLauncher(float x, float y, Level* level) : ProjectileWeapon() {
    this->x = x;
    this->y = y;
    this->level = level;
    this->damageDealt = 5.0f;
    this->range = screen_x * 0.75f;
    this->fireRate = 1.0f;
    this->magazineSize = 1;
    this->currentMag = 1;
    this->totalAmmo = 10;
    this->reloadTime = 2.0f;
    this->blastRadius = 3 * 64.0f;
	this->baseFireRate = this->fireRate;
}

void RocketLauncher::fire() {
    if (!canFire()) return;
    if (reloading) return;
    if (owner == nullptr) return;
    if (currentMag <= 0) {
        reload();
        return;
    }

    float a = owner->getShootAngle();
    
    float spawnX;
    if (owner->getFacingRight()) spawnX = owner->getX() + owner->getWidth();
    else spawnX = owner->getX();
    float spawnY = owner->getY() + owner->getHeight() / 5;

    Rocket* r = new Rocket(owner->isHostileToPlayers(), (int)this->damageDealt, this->range, this->blastRadius, spawnX, spawnY, a, level);
    level->queueSpawn(r);

    currentMag--;
    if (currentMag == 0) reload();
	 if (fireAnim != nullptr) {
        fireAnim->reset();
        fireAnim->setActive(true);
    }
}







// ============= GRENADE =============

Grenade::Grenade(bool damagesPlayers, int damage, float blastRadius, float spawnX, float spawnY, float angle, float speed, Level* level)
    : BallisticProjectile() {
    this->damagesPlayers = damagesPlayers;
    this->damage = damage;
    this->blastRadius = blastRadius;
    this->x = spawnX;
    this->y = spawnY;
    this->level = level;
    this->dead = new bool(false);

    float rad = angle * 3.14159265f / 180.0f;
    this->velocityX = cos(rad) * speed;
    this->velocityY = -sin(rad) * speed;

    this->getTex().loadFromFile("Sprites/grenade.png");
    this->setScaleX(30.0f / 87.0f);
    this->setScaleY(30.0f / 107.0f);
    this->initializeScale();
    this->initializeDimensions();
}

void Grenade::movement() {
    entityGravity();
	this->x += velocityX;
    clampToMap();
}
void Grenade::updateAnimations() {
}

void Grenade::explode() {
    float cx = this->x + this->width / 2;
    float cy = this->y + this->height / 2;
    float zoneX = cx - blastRadius;
    float zoneY = cy - blastRadius;
    float zoneSize = blastRadius * 2;
    DamageZone* boom = new ExplosionZone(zoneX, zoneY, zoneSize, zoneSize, damage, damagesPlayers, true, nullptr);
    level->addDamageZone(boom);
    level->queueSpawn(new Explosion(cx, cy, blastRadius, level));
	onExplode();
}

void Grenade::behaviour() {
    if (*dead) {
        explode();
        removeMe = true;
        return;
    }

    movement();

    updateTileChecks(this->x + this->width / 2, this->y + this->height / 2);
    if (midLeft != ' ' || midRight != ' ' || topMid != ' ' || bottomMid != ' ') {
        explode();
        removeMe = true;
        return;
    }

    DamageZone* contact = new OneShotZone(this->x, this->y, this->width, this->height, 0, damagesPlayers, false, dead);
    level->addDamageZone(contact);

    setAndDraw();
}

bool Grenade::shouldBeRemoved() const {
    return removeMe;
}


// FIRE BOMB  GRENADE=========================
FireBomb::FireBomb(bool damagesPlayers, int damage, float blastRadius, float spawnX, float spawnY, float angle, float speed, Level* level)
    : Grenade(damagesPlayers, damage, blastRadius, spawnX, spawnY, angle, speed, level) {
}

void FireBomb::onExplode() {
    float cx = this->x + this->width / 2;
    float cy = this->y + this->height / 2;
    float poolSize = 6.0f * cell_size;
    float poolX = cx - poolSize / 2;
    float poolY = cy - poolSize / 2;
    Fire* pool = new Fire(poolX, poolY + 3*cell_size, poolSize, poolSize, 10.0f, 2, damagesPlayers, level);
    level->queueSpawn(pool);
}

// ============= HAND GRENADE =============

HandGrenade::HandGrenade(DamagableEntity* owner) : Weapon(owner) {
    this->level = owner->getLevel();
    this->damageDealt = 5.0f;
    this->fireRate = 1.0f;
    this->cooldownTimer = 0.0f;
    this->blastRadius = 3 * 64.0f;
    this->ammo = 10;
    this->throwSpeed = 12.0f;
    this->gravity = 0.5f;
	this->baseFireRate = this->fireRate;
    this->throwOffset = 0.0f;
}

HandGrenade::HandGrenade(float x, float y, Level* levelSkeleton) : Weapon() {
    this->x = x;
    this->y = y;
    this->level = levelSkeleton;
    this->damageDealt = 5.0f;
    this->fireRate = 1.0f;
    this->cooldownTimer = 0.0f;
    this->blastRadius = 3 * 64.0f;
    this->ammo = 10;
    this->throwSpeed = 12.0f;
    this->gravity = 0.5f;
	this->baseFireRate = this->fireRate;
    this->throwOffset = 0.0f;
}

void HandGrenade::fire() {
    if (!canFire()) return;
    if (owner == nullptr) return;
    if (ammo <= 0) return;

    float a;
    if (owner->getFacingRight()) a = 45.0f;
    else a = 135.0f;

    float spawnX;
    if (owner->getFacingRight())
        spawnX = owner->getX() + owner->getWidth() + throwOffset;
    else
        spawnX = owner->getX() - throwOffset;
    float spawnY = owner->getY() + owner->getHeight() / 5;

    Grenade* g = new Grenade(owner->isHostileToPlayers(), this->damageDealt, this->blastRadius, spawnX, spawnY, a, throwSpeed, level);
    level->queueSpawn(g);

    ammo--;
    cooldownTimer = 1.0f / fireRate;
	throwOffset = 0.0f;  // reset after every fire
}

void HandGrenade::onImpact() {
}

void HandGrenade::applyExplosionEffect(int x, int y, int damage) {
}

void HandGrenade::addAmmo(int delta) {
    ammo += delta;
    if (ammo < 0) ammo = 0;
}

int HandGrenade::getAmmo() {
    return ammo;
}

bool HandGrenade::isOutOfAmmo() {
    return ammo <= 0;
}


// ============= FIRE BOMB GRENADE =============

FireBombGrenade::FireBombGrenade(DamagableEntity* owner) : HandGrenade(owner) {
    this->damageDealt = 7.0f;
    this->blastRadius = 5 * 64.0f;
    this->ammo = 10;
}

FireBombGrenade::FireBombGrenade(float x, float y, Level* levelSkeleton) : HandGrenade(x, y, levelSkeleton) {
    this->damageDealt = 7.0f;
    this->blastRadius = 5 * 64.0f;
    this->ammo = 10;
}

void FireBombGrenade::fire() {
    if (!canFire()) return;
    if (owner == nullptr) return;
    if (ammo <= 0) return;

    float a;
    if (owner->getFacingRight()) a = 45.0f;
    else a = 135.0f;

    float spawnX;
    if (owner->getFacingRight()) spawnX = owner->getX() + owner->getWidth();
    else spawnX = owner->getX();
    float spawnY = owner->getY() + owner->getHeight() / 5;

    FireBomb* fb = new FireBomb(owner->isHostileToPlayers(), this->damageDealt, this->blastRadius, spawnX, spawnY, a, throwSpeed, level);
    level->queueSpawn(fb);

    ammo--;
    cooldownTimer = 1.0f / fireRate;
}

void FireBombGrenade::onImpact() {
}

void FireBombGrenade::applyFireEffect(int x, int y) {
}




// ===========================LASER=========================

Laser::Laser(bool damagesPlayers, int damage, float spawnX, float spawnY, float beamLength, float angle, Level* level)
    : StraightProjectile(damagesPlayers, damage, beamLength, angle, level) {
    this->max_speed = 0.0f;
    this->x = spawnX;
    this->y = spawnY;
    this->lifetime = 1;

    this->getTex().loadFromFile("Sprites/laser.png");
    this->setScaleX(screen_x / 494.0f);
    this->setScaleY(120.0f / 505.0f);
    this->initializeScale();
    this->initializeDimensions();
}

void Laser::movement() {
}

void Laser::updateAnimations() {
}

void Laser::behaviour() {
    if (*dead) {
        removeMe = true;
        return;
    }
    if (lifetime <= 0) {
        removeMe = true;
        return;
    }

    DamageZone* z = new OneShotZone(this->x, this->y, this->width, this->height, damage, damagesPlayers, false, dead);
    level->addDamageZone(z);

    lifetime--;
    setAndDraw();
}

bool Laser::shouldBeRemoved() const {
    return removeMe;
}

// LASER GUN==================================
LaserGun::LaserGun(DamagableEntity* owner) : ProjectileWeapon() {
    this->owner = owner;
    this->level = owner->getLevel();
    this->damageDealt = 1000.0f;
    this->range = (float)screen_x;
    this->fireRate = 60.0f;
    this->totalAmmo = -1;
    this->magazineSize = -1;
    this->currentMag = -1;
    this->reloadTime = 0;
	this->baseFireRate = this->fireRate;
	//OneShotAnimation* muzzle = new OneShotAnimation(1, 1.0f / 60.0f, false);
    //muzzle->loadTexture("Sprites/marco-idle.png"); // PLACEHOLDER
    //muzzle->setScale(128.0f / 100.0f, 16.0f / 100.0f);
    //muzzle->setActive(false);
    //this->fireAnim = muzzle;
    //this->muzzleOffsetX = 48.0f;
    //this->muzzleOffsetY = 15.0f;
}

LaserGun::LaserGun(float x, float y, Level* level) : ProjectileWeapon() {
    this->x = x;
    this->y = y;
    this->level = level;
    this->damageDealt = 1000.0f;
    this->range = (float)screen_x;
    this->fireRate = 60.0f;
    this->totalAmmo = -1;
    this->magazineSize = -1;
    this->currentMag = -1;
    this->reloadTime = 0;
}



void LaserGun::fire() {
    if (!canFire()) return;
    if (owner == nullptr) return;

    float a = owner->getShootAngle();

    float muzzleX;
    if (owner->getFacingRight()) {
        muzzleX = owner->getX() + owner->getWidth();
    }
    else {
        muzzleX = owner->getX();
    }
    float muzzleY = owner->getY() + owner->getHeight() / 5;

    float spawnX;
    if (owner->getFacingRight()) {
        spawnX = muzzleX;
    }
    else {
        spawnX = muzzleX - (float)screen_x;
    }

    Laser* l = new Laser(owner->isHostileToPlayers(), this->damageDealt, spawnX, muzzleY, screen_x, a, level);
    level->queueSpawn(l);
	 if (fireAnim != nullptr) {
        fireAnim->reset();
        fireAnim->setActive(true);
    }
}

// ====Pod laser gun====
PodLaserGun::PodLaserGun(DamagableEntity* owner) : LaserGun(owner) {
    this->fireRate = 0.5f;  // every 2 sec
    this->damageDealt = 10.0f;
}

void PodLaserGun::fire() {
    if (!canFire()) 
        return;
    if (owner == nullptr) 
        return;

    // spawn beam straight down from pod center
    float spawnX = owner->getX() + owner->getWidth() / 2.0f - 8.0f;
    float spawnY = owner->getY() + owner->getHeight();

    // angle 90 = straight down in your StraightProjectile system
    Laser* l = new Laser(owner, this->damageDealt,spawnX, spawnY, screen_y, 90.0f, level);
    level->queueSpawn(l);
}


// ============ FIRE===============
Fire::Fire(float x, float y, float w, float h, float life, int dmg, bool damagesPlayers, Level* level)
    : Entity(level), lifetime(life), tickTimer(1.0f), damagePerTick(dmg), damagesPlayers(damagesPlayers) {
    this->x = x;
    this->y = y - 3*cell_size;
    this->width = w;
    this->height = h;

    LoopAnimation* anim = new LoopAnimation(6, 1.0f / 12.0f, true);
    anim->loadTexture("Sprites/fire-anim.png");
    IntRect frames[6] = {
        IntRect(0,   75, 93, 77),
        IntRect(106,  40, 125, 112),
        IntRect(249, 19, 131, 133),
        IntRect(394, 19, 140, 133),
        IntRect(543, 24, 134, 128),
        IntRect(692, 43, 132, 109),
    };
    anim->setCustomFrames(frames, 6);
    anim->setScale(5*cell_size / 140.0f, 5*cell_size / 133.0f);
    anim->setActive(true);
    animations.add(anim);

    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[0]->getRect());
    this->sprite.setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
    this->sprite.setPosition(x, y);
}

void Fire::behaviour() {
    lifetime -= 1.0f / 60.0f;
    tickTimer += 1.0f / 60.0f;
    if (tickTimer >= 1.0f) {
        tickTimer -= 1.0f;
        level->addDamageZone(new FireZone(x, y + 3 * cell_size, width, height,damagePerTick, damagesPlayers));
    }
    playSFX(SFX_FIRE);
    animations[0]->update();
    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[0]->getRect());
    this->sprite.setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
    setAndDraw();
}

bool Fire::shouldBeRemoved() const {
    return lifetime <= 0;
}






// =============== FLAME SHOT======================
FlameShot::FlameShot(float x, float y, Level* levelSkeleton) : ProjectileWeapon() {
    this->x = x;
    this->y = y;
    this->level = levelSkeleton;
    this->damageDealt = 2;
    this->fireRate = 1.0f;
    this->cooldownTimer = 0.0f;
    this->range = 5.0f * cell_size;
    this->totalAmmo = 30;
    this->currentMag = 30;
    this->magazineSize = 30;
    this->reloadTime = 0.0f;
    this->reloadTimer = 0.0f;
    this->reloading = false;
	this->baseFireRate = this->fireRate;
}

FlameShot::FlameShot(DamagableEntity* owner) : ProjectileWeapon() {
    this->level = owner->getLevel();
    this->owner = owner;
    this->damageDealt = 2;
    this->fireRate = 1.0f;
    this->cooldownTimer = 0.0f;
    this->range = 5.0f * cell_size;
    this->totalAmmo = 30;
    this->currentMag = 30;
    this->magazineSize = 30;
    this->reloadTime = 0.0f;
    this->reloadTimer = 0.0f;
    this->reloading = false;
	//OneShotAnimation* muzzle = new OneShotAnimation(1, 1.0f / 8.0f, false);
    //muzzle->loadTexture("Sprites/marco-idle.png"); // PLACEHOLDER
    //muzzle->setScale(96.0f / 100.0f, 64.0f / 100.0f);
    //muzzle->setActive(false);
    //this->fireAnim = muzzle;
    //this->muzzleOffsetX = 48.0f;
    //this->muzzleOffsetY = 0.0f;
}

void FlameShot::fire() {
    if (!canFire()) return;
    if (owner == nullptr) return;
    if (currentMag <= 0) return;

    float streamLen = 5.0f * cell_size;
    float streamH = owner->getHeight();
    float spawnX;
    if (owner->getFacingRight()) spawnX = owner->getX() + owner->getWidth();
    else spawnX = owner->getX() - streamLen;
    float spawnY = owner->getY();

    Fire* stream = new Fire(spawnX, spawnY, streamLen, streamH, 1.05f, 2, owner->isHostileToPlayers(), level);
    level->queueSpawn(stream);

    currentMag--;
    cooldownTimer = 1.0f / fireRate;
	 if (fireAnim != nullptr) {
        fireAnim->reset();
        fireAnim->setActive(true);
    } 
}

void FlameShot::applyEffect() {}

//==============explosion=================
Explosion::Explosion(float cx, float cy, float radius, Level* level) : Entity(level), blastRadius(radius), done(false) {
    this->width = radius * 2 ;
    this->height = radius * 2;
    this->x = cx - this->width / 2;   
    this->y = cy - this->height / 2;

    OneShotAnimation* anim = new OneShotAnimation(4, 1.0f / 12.0f, true);
    anim->loadTexture("Sprites/grenade-explode.png");
    IntRect frames[4] = {
        IntRect(96,   456, 136, 121),
        IntRect(270, 420, 227, 170),
        IntRect(503, 402, 243, 196),
        IntRect(751, 408, 226, 190),
    };
    anim->setCustomFrames(frames, 4);
    anim->setScale(this->width / 227.0f, this->height/ 196.0f);
    anim->setActive(true);
    animations.add(anim);

    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[0]->getRect());
    this->sprite.setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
    this->sprite.setPosition(x, y);
}

Explosion::~Explosion() {}

void Explosion::behaviour() {
    playSFX(SFX_EXPLOSION);
    if (animations[0]->getIsComplete()) {
        done = true;
        return;
    }
    animations[0]->update();
    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[0]->getRect());
    this->sprite.setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
    setAndDraw();
}




MyString Knife::getDisplayName() const {
    return MyString("KNIFE");
}

MyString Pistol::getDisplayName() const {
    return MyString("PISTOL");
}

MyString HeavyMachineGun::getDisplayName() const {
    return MyString("HMG");
}

MyString RocketLauncher::getDisplayName() const {
    return MyString("ROCKET LAUNCHER");
}

MyString FlameShot::getDisplayName() const {
    return MyString("FLAME SHOT");
}

MyString LaserGun::getDisplayName() const {
    return MyString("LASER");
}

MyString HandGrenade::getDisplayName() const {
    return MyString("GRENADE");
}

MyString FireBombGrenade::getDisplayName() const {
    return MyString("FIRE BOMB");
}



// ============= SLASH =============

Slash::Slash(bool damagesPlayers, int damage, float range, float spawnX, float spawnY, float angle, Level* level)
    : StraightProjectile(damagesPlayers, damage, range, angle, level) {
    this->max_speed = 25.0f;
    this->x = spawnX;
    this->y = spawnY;

    this->getTex().loadFromFile("Sprites/slash.png");
    this->setScaleX(128.0f / 384.0f);
    this->setScaleY(50.0f / 111.0f);
    this->initializeScale();
    this->initializeDimensions();
}

void Slash::movement() {
    StraightProjectile::movement();
    clampToMap();
}

void Slash::fatPlayerBulletMovement() {
}

void Slash::updateAnimations() {
}

void Slash::behaviour() {
    if (*dead) {
        removeMe = true;
        return;
    }
    if (distanceTraveled >= range) {
        removeMe = true;
        return;
    }

    movement();

    updateTileChecks(this->x + this->width / 2, this->y + this->height / 2);
    if ((midLeft != ' ' && midLeft != '~') || (midRight != ' ' && midRight != '~') || (topMid != ' ' && topMid != '~') || (bottomMid != ' ' && bottomMid != '~')) {
        removeMe = true;
        return;
    }


    DamageZone* z = new OneShotZone(this->x, this->y, this->width, this->height, damage, damagesPlayers, false, dead);
    level->addDamageZone(z);

    setAndDraw();
}

bool Slash::shouldBeRemoved() const {
    return removeMe;
}

void Slash::writeStateTo(CurrentState& s) const {
    if (s.bulletCount >= 200) return;
    BulletState& b = s.bullets[s.bulletCount];
    b.id = entityId;
    b.x = getX();
    b.y = getY();
    b.angle = angle;
    b.active = true;
    s.bulletCount++;
}

void Slash::applyStateFrom(const CurrentState& s) {
    for (int i = 0; i < s.bulletCount; i++) {
        if (s.bullets[i].id != entityId) continue;
        const BulletState& b = s.bullets[i];
        setX(b.x);
        setY(b.y);
        angle = b.angle;
        return;
    }
}


// ============= SLASH GUN =============

SlashGun::SlashGun(DamagableEntity* owner) : ProjectileWeapon() {
    this->owner = owner;
    this->level = owner->getLevel();
    this->damageDealt = 10.0f;
    this->range = screen_x;
    this->fireRate = 2.0f;
    this->totalAmmo = -1;
    this->magazineSize = -1;
    this->currentMag = -1;
    this->reloadTime = 0;
    this->baseFireRate = this->fireRate;
}

SlashGun::SlashGun(float x, float y, Level* level) : ProjectileWeapon() {
    this->x = x;
    this->y = y;
    this->level = level;
    this->damageDealt = 10.0f;
    this->range = screen_x;
    this->fireRate = 1.0f;
    this->totalAmmo = -1;
    this->magazineSize = -1;
    this->currentMag = -1;
    this->reloadTime = 0;
    this->baseFireRate = this->fireRate;
}

void SlashGun::fire() {
    if (!canFire()) return;
    if (owner == nullptr) return;

    float a = owner->getShootAngle();

    float spawnX;
    if (owner->getFacingRight()) {
        spawnX = owner->getX() + owner->getWidth();
    }
    else {
        spawnX = owner->getX();
    }
    float spawnY = owner->getY() + owner->getHeight() / 5;

    Slash* s = new Slash(owner->isHostileToPlayers(), (int)this->damageDealt, this->range, spawnX, spawnY, a, level);
    level->queueSpawn(s);

    if (fireAnim != nullptr) {
        fireAnim->reset();
        fireAnim->setActive(true);
    }
    playSFX(SFX_SLASH);
}

void SlashGun::reload() {
}

bool SlashGun::hasInfiniteAmmo() {
    return true;
}

MyString SlashGun::getDisplayName() const {
    return MyString("SLASH GUN");
}