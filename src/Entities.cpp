#include "project.h"


// ===================== SFML OBJECT ==================
SfmlObject::SfmlObject() : x(0), y(0), width(0), height(0), scaleX(1), scaleY(1) {
}

SfmlObject::SfmlObject(Texture& texture, float x, float y, float scale_x, float scale_y) : x(x), y(y), scaleX(scale_x), scaleY(scale_y) {
    Tex = texture;
    sprite.setTexture(Tex);
    sprite.setScale(scaleX, scaleY);
    width = Tex.getSize().x * scaleX;
    height = Tex.getSize().y * scaleY;
    sprite.setPosition(x, y);
}

void SfmlObject::centreSprite() {
    x = (screen_x / 2) - (width / 2);
    y = (screen_y / 2) - (height / 2);
    sprite.setPosition(x, y);
}

void SfmlObject::initializeScale() {
    sprite.setTexture(Tex);
    sprite.setScale(scaleX, scaleY);
}

void SfmlObject::initializeDimensions() {
    width = Tex.getSize().x * scaleX;
    height = Tex.getSize().y * scaleY;
}

float SfmlObject::getX() const {
    if (this == nullptr) {
        return 0.0f;
    }
    return x;
}

float SfmlObject::getY() const {
    if (this == nullptr) {
        return 0.0f;
    }
    return y;
}

Texture& SfmlObject::getTex() {
    return Tex;
}

const Texture& SfmlObject::getTex() const {
    return Tex;
}

Sprite& SfmlObject::getSprite() {
    return sprite;
}

const Sprite& SfmlObject::getSprite() const {
    return sprite;
}

float SfmlObject::getWidth() const {
    return width;
}

float SfmlObject::getHeight() const {
    return height;
}

float SfmlObject::getScaleX() const {
    return scaleX;
}

float SfmlObject::getScaleY() const {
    return scaleY;
}

void SfmlObject::setX(float x) {
    this->x = x;
    sprite.setPosition(x, y);
}

void SfmlObject::setY(float y) {
    this->y = y;
    sprite.setPosition(x, y);
}

void SfmlObject::setPosition(float x, float y) {
    this->x = x;
    this->y = y;
    sprite.setPosition(x, y);
}

void SfmlObject::setTex(Texture& texture) {
    Tex = texture;
    sprite.setTexture(Tex);
    width = Tex.getSize().x * scaleX;
    height = Tex.getSize().y * scaleY;
}

void SfmlObject::setSprite(const Sprite& sprite) {
    this->sprite = sprite;
}

void SfmlObject::setWidth(float width) {
    this->width = width;
}

void SfmlObject::setHeight(float height) {
    this->height = height;
}

void SfmlObject::setScaleX(float scaleX) {
    this->scaleX = scaleX;
    sprite.setScale(scaleX, scaleY);
    width = Tex.getSize().x * scaleX;
}

void SfmlObject::setScaleY(float scaleY) {
    this->scaleY = scaleY;
    sprite.setScale(scaleX, scaleY);
    height = Tex.getSize().y * scaleY;
}

void SfmlObject::setScale(float scaleX, float scaleY) {
    this->scaleX = scaleX;
    this->scaleY = scaleY;
    sprite.setScale(scaleX, scaleY);
    width = Tex.getSize().x * scaleX;
    height = Tex.getSize().y * scaleY;
}



// ===================== ENTITY =====================

int Entity::nextId = 0;

Entity::Entity() : level(nullptr), entityId(nextId++), topLeft(0), topMid(0), topRight(0), midLeft(0), midRight(0), bottomLeft(0), bottomMid(0), bottomRight(0), bottomLeftAbove(0),
bottomMidAbove(0), bottomRightAbove(0), belowLeft(0), belowMid(0), belowRight(0), velocityX(0), velocityY(0), max_speed(0), acceleration(0), onGround(false) {
}

Entity::Entity(Level* level) : level(level), entityId(nextId++), topLeft(0), topMid(0), topRight(0), midLeft(0), midRight(0), bottomLeft(0), bottomMid(0), bottomRight(0),
bottomLeftAbove(0), bottomMidAbove(0), bottomRightAbove(0), belowLeft(0), belowMid(0), belowRight(0), velocityX(0), velocityY(0), max_speed(0), acceleration(0),
onGround(false) {
}

void Entity::updateTileChecks(float check_x, float check_y) {
    float w = this->getWidth();
    float h = this->getHeight();

    topLeft = level->tileAtWorld(check_x + 0.2f * w, check_y);
    topMid = level->tileAtWorld(check_x + w / 2, check_y);
    topRight = level->tileAtWorld(check_x + 0.8f * w, check_y);

    midLeft = level->tileAtWorld(check_x, check_y + h / 2);
    midRight = level->tileAtWorld(check_x + w, check_y + h / 2);

    bottomLeft = level->tileAtWorld(check_x + 0.2f * w, check_y + h);
    bottomMid = level->tileAtWorld(check_x + w / 2, check_y + h);
    bottomRight = level->tileAtWorld(check_x + 0.8f * w, check_y + h);

    bottomLeftAbove = level->tileAtWorld(check_x, check_y + 0.8f * h);
    bottomMidAbove = level->tileAtWorld(check_x + w / 2, check_y + 0.8f * h);
    bottomRightAbove = level->tileAtWorld(check_x + w, check_y + 0.8f * h);

    belowLeft = level->tileAtWorld(check_x, check_y + 1.2f * h);
    belowMid = level->tileAtWorld(check_x + w / 2, check_y + 1.2f * h);
    belowRight = level->tileAtWorld(check_x + w, check_y + 1.2f * h);
}


bool Entity::checkCollision(Entity* other) const {
    if (this->getX() < other->getX() + other->getWidth() && this->getX() + this->getWidth() > other->getX() && this->getY() < other->getY() + other->getHeight() && this->getY() + this->getHeight() > other->getY()) {
        return true;
    }
    return false;
}

bool Entity::checkCollisionCentre(Entity* other) const
{
    if (this->getX() < other->getX() + other->getWidth() / 2 && this->getX() + this->getWidth()>other->getX() + other->getWidth() / 2 && this->getY() < other->getY() + other->getHeight() / 2 && this->getY() + this->getHeight() / 2 > other->getY() + other->getHeight() / 2)
    {
        return true;
    }
    return false;
}



float Entity::calculateDistance(Entity* other) const {
    float dx = this->getX() - other->getX();
    float dy = this->getY() - other->getY();
    return sqrt(dx * dx + dy * dy);
}

bool Entity::horizontalCollisionCheck(char top, char mid, char bottom) const {
    if ((top == '#' || mid == '#' || bottom == '#') || (top == 'D' || mid == 'D' || bottom == 'D')) {
        return true;
    }
    return false;
}

bool Entity::isOffScreen() const {
    if (level == nullptr) return false;
    float left = (float)level->getScreenLeft();
    float right = (float)level->getScreenRight();
    float top = (float)level->getScreenTop();
    float bottom = (float)level->getScreenBottom();
    float marginX = (float)screen_x * 0.5f;
    float marginY = (float)screen_y * 0.5f;
    return (this->x + this->width < left - marginX) ||
        (this->x > right + marginX) ||
        (this->y + this->height < top - marginY) ||
        (this->y > bottom + marginY);
}

void Entity::clampToMap() {
    if (level == nullptr) return;
    if (!level->hasFiniteBounds()) return;
    Map* m = level->getMap();
    if (m == nullptr) return;
    float rightEdge = (float)(m->getWidth() * m->getCellSize()) - this->width;
    if (x < 0) x = 0;
    if (x > rightEdge) x = rightEdge;
}

bool Entity::OnGround() {
    return this->onGround;
}

float Entity::getVelocityX() {
    return this->velocityX;
}

float Entity::getVelocityY() {
    return this->velocityY;
}

float Entity::getMaxSpeed() {
    return this->max_speed;
}

void Entity::setMaxSpeed(float speed) {
    this->max_speed = speed;
}

void Entity::setAcceleration(float accel) {
    this->acceleration = accel;
}

bool Entity::inAir() {
    return !this->onGround;
}

void Entity::entityGravity() {
    float offset_y = this->y;
    offset_y += velocityY;

    char** lvl = this->level->getMap()->getLvl();
	updateTileChecks(this->x, offset_y);

	bool groundedCheck = ((bottomLeft == '#' || bottomMid == '#' || bottomRight == '#') || (bottomLeft == 'D' || bottomMid == 'D' || bottomRight == 'D'))
        && (bottomLeftAbove == ' ' || bottomMidAbove == ' ' || bottomRightAbove == ' ');
    bool swimCheck = (midLeft == '~' || midRight == '~');
	if ((groundedCheck && (velocityY>0)) || swimCheck) {
		onGround = true;
        velocityY = 0;
	}

	else {
		this->setY(offset_y);
		onGround = false;
	}


    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY >= terminal_Velocity)
        {
            velocityY = terminal_Velocity;
        }
    }
    else {
        velocityY = 0;
    }
}

void Entity::updateXY() {
    x += velocityX;
    y += velocityY;
}

Level* Entity::getLevel() {
    return level;
}

Entity::~Entity() {
    for (int i = 0; i < animations.getSize(); i++) {
        delete animations[i];
    }
}

void Entity::updateAnimations() {}

void Entity::setAndDraw() {
	sprite.setPosition(this->x - level->getScreenLeft(), this->y - level->getScreenTop());
	window->draw(sprite);
}

char Entity::getBelowMid() const { return belowMid; }
char Entity::getTopLeft() const { return topLeft; }
char Entity::getTopRight() const { return topRight; }
char Entity::getMidRight() const { return midRight; }
char Entity::getMidLeft() const { return midLeft; }
char Entity::getBottomRight() const { return bottomRight; }
char Entity::getBottomLeft() const { return bottomLeft; }
void Entity::setVelocityX(float val) { this->velocityX = val; }
char Entity::getTopMid() const { return topMid; }
char Entity::getBottomMid() const { return bottomMid;}
void Entity::setLevel(Level* level) { this->level = level; }
float Entity::getAcceleration() { return acceleration; }


// ===================== DAMAGABLE ENTITY =====================
DamagableEntity::DamagableEntity() : Entity(), HP(0), isAttacking(false), facingRight(true), dead(false) {}

DamagableEntity::DamagableEntity(Level* level) : Entity(level), HP(0), isAttacking(false), facingRight(true), dead(false) {}

void DamagableEntity::changeHP(int change) {
    this->HP += change;
    if (this->HP <= 0) {
        this->HP = 0;
        this->dead = true;
    }
}


int DamagableEntity::getHP() {
    return this->HP;
}

bool DamagableEntity::getFacingRight() const {
    return this->facingRight;
}

void DamagableEntity::setFacingRight(bool val) {
    this->facingRight = val;
}

bool DamagableEntity::isBlockingHit(float attackX) const {
    if (facingRight)
        return attackX > this->x;
    else
        return attackX < this->x;
}

void DamagableEntity::receiveDamageZone(DamageZone& zone) {
    if (dead) return;
    if (zone.damagesPlayers() == this->isHostileToPlayers()) return;

    if (!(zone.getX() < this->x + this->width &&
        zone.getX() + zone.getWidth() > this->x &&
        zone.getY() < this->y + this->height &&
        zone.getY() + zone.getHeight() > this->y))
        return;

    if (absorbsHit(zone)) return;

    ExplosionZone* ez = dynamic_cast<ExplosionZone*>(&zone);
    if (ez != nullptr) ScoreManager::instance().setCurrentExplosionContext(ez->getZoneId());

    zone.igniteEntity(this);
    if (dead) {
        zone.onHit();
        ScoreManager::instance().clearCurrentExplosionContext();
        return;
    }
    changeHP(-zone.getDamage());
    zone.onHit();
    ScoreManager::instance().clearCurrentExplosionContext();
}

bool DamagableEntity::shouldBeRemoved() const {
    return dead;
}

void DamagableEntity::setAndDraw() {
    float drawX = this->x - level->getScreenLeft();
    float drawY = this->y - level->getScreenTop();

    // if sprite is flipped  offset by width
    if (sprite.getScale().x < 0)
        drawX += this->width;

    sprite.setPosition(drawX, drawY);
    window->draw(sprite);
}

void DamagableEntity::onKilledFromFar() {
    dead = true;
}

// ============================ PROJECTILES =======================================

Projectile::Projectile() : Entity(), damagesPlayers(false), damage(0), dead(nullptr), removeMe(false) {
}

Projectile::Projectile(bool damagesPlayers, int damage, Level* level)
    : Entity(level), damagesPlayers(damagesPlayers), damage(damage), removeMe(false) {
    dead = new bool(false);
}

Projectile::~Projectile() {
    delete dead;
}

StraightProjectile::StraightProjectile() : Projectile(), angle(0), distanceTraveled(0), range(0) {
}

StraightProjectile::StraightProjectile(bool damagesPlayers, int damage, float range, float angle, Level* level)
    : Projectile(damagesPlayers, damage, level), angle(angle), distanceTraveled(0), range(range) {
}

float StraightProjectile::getAngle() {
    return this->angle;
}

float StraightProjectile::setAngle(float angle) {
    this->angle = angle;
    return 0;
}

void StraightProjectile::movement() {
    float rad = angle * 3.14159265f / 180.0f;
    this->x += cos(rad) * max_speed;
    this->y += sin(rad) * max_speed;
    distanceTraveled += max_speed;
}

StraightProjectile::~StraightProjectile() {
}

BallisticProjectile::BallisticProjectile() : Projectile() {
}

void BallisticProjectile::movement() {
}



// ===================== DAMAGE ZONE =====================

DamageZone::DamageZone() : x(0), y(0), width(0), height(0), damage(0), damagesPlayersFlag(false), piercesShield(false), consumed(false), deadFlag(nullptr), wasProcessed(false) {
}

DamageZone::DamageZone(float x, float y, float w, float h, int dmg, bool damagesPlayers, bool pierces, bool* deadFlag)
    : x(x), y(y), width(w), height(h), damage(dmg), damagesPlayersFlag(damagesPlayers), piercesShield(pierces), consumed(false), deadFlag(deadFlag), wasProcessed(false) {
}

DamageZone::~DamageZone() {}

float DamageZone::getX() const { return x; }
float DamageZone::getY() const { return y; }
float DamageZone::getWidth() const { return width; }
float DamageZone::getHeight() const { return height; }
int DamageZone::getDamage() const { return damage; }
bool DamageZone::damagesPlayers() const { return damagesPlayersFlag; }
bool DamageZone::getPiercesShield() const { return piercesShield; }

void DamageZone::update() { wasProcessed = true; }
void DamageZone::onHit() {}
bool DamageZone::shouldBeRemoved() const { return consumed; }

// ========= ONE-SHOT ZONE ==================

OneShotZone::OneShotZone(float x, float y, float w, float h, int dmg, bool damagesPlayers, bool pierces, bool* deadFlag)
    : DamageZone(x, y, w, h, dmg, damagesPlayers, pierces, deadFlag) {
}

void OneShotZone::onHit() {
    consumed = true;
    if (deadFlag != nullptr) {
        *deadFlag = true;
    }
}

bool OneShotZone::shouldBeRemoved() const {
    return consumed || wasProcessed;
}

// ===================== TIMED ZONE =================

TimedZone::TimedZone(float x, float y, float w, float h, int dmg, bool damagesPlayers, bool pierces, float life, bool* deadFlag)
    : DamageZone(x, y, w, h, dmg, damagesPlayers, pierces, deadFlag), remainingLife(life) {
}
void TimedZone::update() {
    remainingLife -= 1.0f / 60.0f;
}

bool TimedZone::shouldBeRemoved() const {
    return remainingLife <= 0;
}


// =============== EXPLOSION ZONE ===================

ExplosionZone::ExplosionZone(float x, float y, float w, float h, int dmg, bool damagesPlayers, bool pierces, bool* deadFlag) : DamageZone(x, y, w, h, dmg, damagesPlayers, pierces, deadFlag) {
    zoneId = ScoreManager::instance().allocateExplosionZoneId();
}

bool ExplosionZone::shouldBeRemoved() const {
    return wasProcessed;
}

//==================FIRE ZONE======================
FireZone::FireZone(float x, float y, float w, float h, int dmg, bool damagesPlayers)
    : DamageZone(x, y, w, h, dmg, damagesPlayers, false, nullptr) {
}

bool FireZone::shouldBeRemoved() const {
    return wasProcessed;
}

void FireZone::igniteEntity(DamagableEntity* victim) {
    victim->onIgnite();
}

//===========zombie and mummy hit zone=============
void ZombieHitZone::applySpecialEffect(PlayerSoldier* player) {
    player->switchTransformationState(new UndeadState());
}

void MummyCurseZone::applySpecialEffect(PlayerSoldier* player) {
    player->switchTransformationState(new MummyState());
}
