#include "project.h"

InteractableObject::InteractableObject() {
    interacted = false;
}

Collectible::Collectible(float x, float y, Level* lvlSkeleton) :Entity(lvlSkeleton) {
    this->x = x;
    this->y = y;
    this->collected = false;
}
bool Collectible::isCollected()const {
    return this->collected;
}
void Collectible::collect() {
    this->collected = true;
}

Collectible::~Collectible() {
}

Food::Food(float x, float y, Level* levelSkeleton) : Collectible(x, y, levelSkeleton) {}

void Food::movement() {
    entityGravity();
    clampToMap();
}

void Food::updateAnimations() {
    if (animations.getSize() == 0) 
        return;
    animations[0]->update();
    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[0]->getRect());
    this->sprite.setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
}

void Food::collect() {
    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr) 
        return;
    increaseSaturation(player);
    collected = true;
}

void Food::increaseSaturation(PlayerSoldier* player) {
    player->increaseSaturation(getSaturationAmount());
}

void Food::behaviour() {
    if (collected) 
        return;
    movement();
    PlayerSoldier* player = level->getCurrentPlayer();
    if (player != nullptr && checkCollision(player)) {
        collect();
        return;
    }
    updateAnimations();
    setAndDraw();
}

// ---- Turkey ----

Turkey::Turkey(float x, float y, Level* levelSkeleton) : Food(x, y, levelSkeleton) {
    float charW = 64.0f, charH = 64.0f;

    LoopAnimation* idle = new LoopAnimation(1, 1.0f, false);
    idle->loadTexture("Sprites/turkey.png");
    idle->setScale(charW / idle->getTexture().getSize().x,charH / idle->getTexture().getSize().y);
    idle->setActive(true);
    animations.add(idle);

    this->setTex(idle->getTexture());
    this->setScaleX(charW / this->getTex().getSize().x);
    this->setScaleY(charH / this->getTex().getSize().y);
    this->initializeScale();
    this->initializeDimensions();
    this->width = charW;
    this->height = charH;
}

// ---- Fruit ----

Fruit::Fruit(float x, float y, Level* levelSkeleton) : Food(x, y, levelSkeleton) {
    float charW = 64.0f, charH = 64.0f;

    LoopAnimation* idle = new LoopAnimation(1, 1.0f, false);
    idle->loadTexture("Sprites/fruit.png");
    idle->setScale(charW / idle->getTexture().getSize().x,
        charH / idle->getTexture().getSize().y);
    idle->setActive(true);
    animations.add(idle);

    this->setTex(idle->getTexture());
    this->setScaleX(charW / this->getTex().getSize().x);
    this->setScaleY(charH / this->getTex().getSize().y);
    this->initializeScale();
    this->initializeDimensions();
    this->width = charW;
    this->height = charH;
}

// =================== PRISONER ===================

Prisonor::Prisonor(float x, float y, Level* levelSkeleton) : InteractableObject() {
    this->x = x;
    this->y = y;
    this->level = levelSkeleton;
    float charW = 96.0f, charH = 128.0f;

    LoopAnimation* idle = new LoopAnimation(1, 1.0f, false);
    idle->loadTexture("Sprites/prisoner.png"); 
    idle->setScale(charW / idle->getTexture().getSize().x,charH / idle->getTexture().getSize().y);
    idle->setActive(true);
    animations.add(idle);

    this->setTex(idle->getTexture());
    this->setScaleX(charW / this->getTex().getSize().x);
    this->setScaleY(charH / this->getTex().getSize().y);
    this->initializeScale();
    this->initializeDimensions();
    this->width = charW;
    this->height = charH;
}

void Prisonor::movement() {
    entityGravity();
    clampToMap();
}

void Prisonor::updateAnimations() {
    if (animations.getSize() == 0)
        return;
    animations[0]->update();
    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[0]->getRect());
    this->sprite.setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
}

void Prisonor::dropSupplyBox() {
    level->queueSpawn(new SupplyBox(this->x, this->y, level));
}

void Prisonor::collect(DamagableEntity* owner) {
    interacted = true;
    dropSupplyBox();
}

void Prisonor::behaviour() {
    if (interacted)
        return;
    movement();

    PlayerSoldier* player = level->getCurrentPlayer();
    if (player != nullptr && checkCollision(player)) {
        if (Keyboard::isKeyPressed(Keyboard::P)) {
            collect(player);
            return;
        }
    }

    updateAnimations();
    setAndDraw();
}

// =================== SUPPLY BOX ===================

SupplyBox::SupplyBox(float x, float y, Level* levelSkeleton)
    : InteractableObject(), containedWeapon(nullptr), opened(false) {
    this->x = x;
    this->y = y;
    this->level = levelSkeleton;

    float charW = 64.0f, charH = 64.0f;

    // roll weapon 
    int weaponRoll = rand() % 10;
    if (weaponRoll < 9) {
        int type = rand() % 3;
        if (type == 0)      
            containedWeapon = new HeavyMachineGun(x, y, levelSkeleton);
        else if (type == 1) 
            containedWeapon = new FlameShot(x, y, levelSkeleton);
        else                
            containedWeapon = new RocketLauncher(x, y, levelSkeleton);
    }
    else {
        containedWeapon = new LaserGun(x, y, levelSkeleton);
    }

    // roll grenades
    if (rand() % 2 == 0) {
        hasFireBombs = false;
        grenadeCount = rand() % 3 + 3; // 3-5
        fireBombCount = 0;
    }
    else {
        hasFireBombs = true;
        fireBombCount = rand() % 3 + 1; // 1-3
        grenadeCount = 0;
    }

    LoopAnimation* idle = new LoopAnimation(1, 1.0f, false);
    idle->loadTexture("Sprites/box.png"); 
    idle->setScale(charW / idle->getTexture().getSize().x,charH / idle->getTexture().getSize().y);
    idle->setActive(true);
    animations.add(idle);

    this->setTex(idle->getTexture());
    this->setScaleX(charW / this->getTex().getSize().x);
    this->setScaleY(charH / this->getTex().getSize().y);
    this->initializeScale();
    this->initializeDimensions();
    this->width = charW;
    this->height = charH;
}

void SupplyBox::movement() {
    entityGravity();
    clampToMap();
}

void SupplyBox::updateAnimations() {
    if (animations.getSize() == 0) 
        return;
    animations[0]->update();
    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[0]->getRect());
    this->sprite.setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
}

void SupplyBox::replenishAmmo(PlayerSoldier* player) {
    if (containedWeapon == nullptr)
        return;
    // find matching weapon slot and replenish its ammo
    Inventory& inv = player->getAndSetInventory();
    for (int i = 0; i < inv.weapons.getSize(); i++) {
        // replenish ammo based on weapon type , add a full mag type shit
        inv.weapons[i]->addAmmo(30);
        break; // only replenish the slot matching the crate weapon
    }
}

void SupplyBox::giveGrenades(PlayerSoldier* player) {
    Inventory& inv = player->getAndSetInventory();

    // Eri always gets fire bombs
    bool giveFireBombs = hasFireBombs;
    if (!inv.melee) // Eri has melee = false
        giveFireBombs = true;

    if (giveFireBombs) {
        int count = (fireBombCount > 0) ? fireBombCount : grenadeCount;
        inv.fireGrenade->addAmmo(count);
    }
    else {
        inv.handGrenade->addAmmo(grenadeCount);
    }
}

void SupplyBox::dropWeapon(PlayerSoldier* player) {
    if (containedWeapon == nullptr) return;
    // spawn weapon as a pickup at box location
    containedWeapon->collect(player); // set level pointer inside weapon
    level->queueSpawn(new DroppedWeapon(this->x, this->y - 32.0f, level, containedWeapon));
    containedWeapon = nullptr; // DroppedWeapon now owns it
}

void SupplyBox::openSupplyBox(PlayerSoldier* player) {
    replenishAmmo(player);
    giveGrenades(player);
    dropWeapon(player);
    opened = true;
    interacted = true;
}

void SupplyBox::collect(DamagableEntity* owner) {
    PlayerSoldier* player = (PlayerSoldier*)owner;
    if (player == nullptr) 
        return;
    openSupplyBox(player);
    interacted = true;
}

void SupplyBox::behaviour() {
    if (interacted) 
        return;
    movement();

    PlayerSoldier* player = level->getCurrentPlayer();
    if (player != nullptr && checkCollision(player)) {
        if (Keyboard::isKeyPressed(Keyboard::P)) {
            collect(player);
            return;
        }
    }

    updateAnimations();
    setAndDraw();
}

SupplyBox::~SupplyBox() {
    if (containedWeapon != nullptr) {
        delete containedWeapon;
        containedWeapon = nullptr;
    }
}

// =================== DROPPED WEAPON ===================

DroppedWeapon::DroppedWeapon(float x, float y, Level* levelSkeleton, Weapon* w)
    : Collectible(x, y, levelSkeleton), weapon(w) {
    float charW = 64.0f, charH = 64.0f;
    this->getTex().loadFromFile("Sprites/weapon-pistol.png"); // placeholder
    this->setScaleX(charW / this->getTex().getSize().x);
    this->setScaleY(charH / this->getTex().getSize().y);
    this->initializeScale();
    this->initializeDimensions();
    this->width = charW;
    this->height = charH;
}

void DroppedWeapon::movement() {
    entityGravity();
    clampToMap();
}

void DroppedWeapon::updateAnimations() {}


void DroppedWeapon::collect() {
    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr) return;

    Inventory& inv = player->getAndSetInventory();
    if (inv.weapons.getSize() <= 2) {
        inv.weapons.add(weapon);
    }
    else {
        delete inv.weapons[2];
        inv.weapons[2] = weapon;
    }
    weapon->collect(player);
    weapon = nullptr;
    collected = true;
}

void DroppedWeapon::behaviour() {
    if (collected) return;
    movement();
    PlayerSoldier* player = level->getCurrentPlayer();
    if (player != nullptr && checkCollision(player)) {
        collect();
        return;
    }
    updateAnimations();
    setAndDraw();
}

DroppedWeapon::~DroppedWeapon() {
    if (weapon != nullptr) {
        delete weapon;
        weapon = nullptr;
    }
}
