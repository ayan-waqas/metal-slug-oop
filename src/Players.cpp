#include "project.h"


// ============= TRANSFORMATION STATE =============

TransformationState::TransformationState() {
    stateTimer = 10.0f;
    currentState = 0;
}

void TransformationState::setCurrentState(int state) {
    currentState = state;
}

int TransformationState::getCurrentState() {
    return currentState;
}

float TransformationState::getStateTimer() {
    return stateTimer;
}


// ============= BASE STATE =============

BaseState::BaseState() {
    currentState = 0;
    stateTimer = 0.0f;
}

void BaseState::applyEffects(PlayerSoldier* player) {
}

void BaseState::update(PlayerSoldier* player) {
}

void BaseState::removeEffects(PlayerSoldier* player) {
}


// ============= UNDEAD STATE =============

UndeadState::UndeadState() {
    currentState = 1;
    stateTimer = 10.0f;
}

void UndeadState::applyEffects(PlayerSoldier* player) {
    //player->applyTransformSprite("Sprites/player-zombie.png");
    //player->setMaxSpeed(player->getMaxSpeed() / 2);
}

void UndeadState::removeEffects(PlayerSoldier* player) {
    //player->restoreTransformSprite();
    //player->setMaxSpeed(player->getMaxSpeed() * 2);
}

void UndeadState::update(PlayerSoldier* player) {
    //stateTimer -= 1 / 60.0f;
    //if (stateTimer <= 0) {
    //    removeEffects(player);
    //}
}


// ============= MUMMY STATE =============

MummyState::MummyState() {
    currentState = 2;
    stateTimer = 10.0f;
}

void MummyState::applyEffects(PlayerSoldier* player) {
    //player->applyTransformSprite("Sprites/player-mummy.png");
    //player->getAndSetInventory().mummyState = true;
    //player->getAndSetInventory().currentWeapon = 0;
}

void MummyState::removeEffects(PlayerSoldier* player) {
    //player->restoreTransformSprite();
    //player->getAndSetInventory().mummyState = false;
    //player->getAndSetInventory().currentWeapon = 4;
}
void MummyState::update(PlayerSoldier* player) {
    //stateTimer -= 1 / 60.0f;
    //if (stateTimer <= 0) {
    //    removeEffects(player);
    //}
}


// ============= SOLDIER =============

Soldier::Soldier() {
    transformationState = new BaseState();
}

Soldier::Soldier(Level* level) : DamagableEntity(level) {
    transformationState = new BaseState();
}

Soldier::~Soldier() {
    if (transformationState != nullptr)    delete transformationState;
}


// ============= INVENTORY =============

Inventory::Inventory(PlayerSoldier* owner) : owner(owner), currentWeapon(1), melee(false), mummyState(false) {
    weapons.addByIndex(new Knife(owner), 0);
    weapons.addByIndex(new Pistol(owner), 1);
    handGrenade = new HandGrenade(owner);
    fireGrenade = new FireBombGrenade(owner);
}


Inventory::~Inventory() {
    for (int i = 0; i < weapons.getSize(); i++) {
        delete weapons[i];
    }
    delete handGrenade;
    delete fireGrenade;
}



// ============= PLAYER SOLDIER =============


PlayerSoldier::PlayerSoldier() : inventory(this) {
    this->x = 0;
    this->y = 0;
    this->spawnX = x;
    this->spawnY = y;
    this->damageStateTimer = 0;
    this->lives = 2;
    this->state = 0;
    this->vehicle = nullptr;
    this->damageState = 0;
    this->saturation = 70;
    this->fat = false;
    this->isJumping = false;
    this->immortal = false;
    this->jumpForce = 16.0f;
    this->max_speed = 5.0f;      // add this
    this->acceleration = 0.5f;   // add this
    this->isCrouching = false;
    this->usingMelee = false;
    this->powerupActive = false;
    this->powerupUsed = false;
    this->powerupTimer = 0.0f;
    this->lastWeapon = -1;
    this->handOffsetX = 0.0f;
    this->handOffsetY = 0.0f;
    this->xWasPressed = false;
    this->level = nullptr;
    this->fusionUsedThisLevel = false;
    this->fusionCooldown = 0.0f;
    this->respawnTimer = 0.0f;
    this->respawning = false;
    this->postRespawnInvincibilityTimer = 0.0f;

    //red hue
    this->redTop = new SfmlObject();
    this->redBottom = new SfmlObject();
    this->redLeft = new SfmlObject();
    this->redRight = new SfmlObject();

    redTop->getTex().loadFromFile("Sprites/redtop.png");
    redBottom->getTex().loadFromFile("Sprites/redbottom.png");
    redLeft->getTex().loadFromFile("Sprites/redleft.png");
    redRight->getTex().loadFromFile("Sprites/redright.png");

    // scale each strip to fill the screen edge
    redTop->setScaleX((float)screen_x / redTop->getTex().getSize().x);
    redTop->setScaleY(1.0f);
    redTop->initializeScale();
    redTop->initializeDimensions();

    redBottom->setScaleX((float)screen_x / redBottom->getTex().getSize().x);
    redBottom->setScaleY(1.0f);
    redBottom->initializeScale();
    redBottom->initializeDimensions();

    redLeft->setScaleX(1.0f);
    redLeft->setScaleY((float)screen_y / redLeft->getTex().getSize().y);
    redLeft->initializeScale();
    redLeft->initializeDimensions();

    redRight->setScaleX(1.0f);
    redRight->setScaleY((float)screen_y / redRight->getTex().getSize().y);
    redRight->initializeScale();
    redRight->initializeDimensions();

    this->redHue = new SfmlObject();
    this->redHue->getTex().loadFromFile("Sprites/white.png");
    this->redHue->setScaleX((float)screen_x);
    this->redHue->setScaleY((float)screen_y);
    this->redHue->initializeScale();
    this->redHue->initializeDimensions();

    // weapon overlay 
   // this->weaponSprite = new SfmlObject();

    // weapon textures 
    // weaponTextures[0] = Texture(); // knife
    // weaponTextures[1].loadFromFile("Sprites/weapon-pistol.png");
    // // slots 2,3,4,5 are loaded dynamically when weapon is picked up
    // for (int i = 2; i < 5; i++)
    //     weaponTextures[i] = Texture();
}

PlayerSoldier::PlayerSoldier(float x, float y, Level* level) : Soldier(level), inventory(this) {
    // fill later
    this->x = x;
    this->y = y;
    this->spawnX = x;
    this->spawnY = y;
    this->damageStateTimer = 0;
    this->lives = 2;
    this->state = 0;
    this->vehicle = nullptr;
    this->damageState = 0;
    this->saturation = 70;
    this->fat = false;
    this->isJumping = false;
    this->immortal = false;
    this->jumpForce = 16.0f;
    this->max_speed = 5.0f;      // add this
    this->acceleration = 0.5f;   // add this
    this->isCrouching = false;
    this->usingMelee = false;
    this->powerupActive = false;
    this->powerupUsed = false;
    this->powerupTimer = 0.0f;
    this->lastWeapon = -1;
    this->handOffsetX = 0.0f;
    this->handOffsetY = 0.0f;
    this->xWasPressed = false;
    this->fusionUsedThisLevel = false;
    this->fusionCooldown = 0.0f;
    this->respawnTimer = 0.0f;
    this->respawning = false;
    this->postRespawnInvincibilityTimer = 0.0f;
    //red hue
    this->redTop = new SfmlObject();
    this->redBottom = new SfmlObject();
    this->redLeft = new SfmlObject();
    this->redRight = new SfmlObject();

    redTop->getTex().loadFromFile("Sprites/redtop.png");
    redBottom->getTex().loadFromFile("Sprites/redbottom.png");
    redLeft->getTex().loadFromFile("Sprites/redleft.png");
    redRight->getTex().loadFromFile("Sprites/redright.png");

    // scale each strip to fill the screen edge
    redTop->setScaleX((float)screen_x / redTop->getTex().getSize().x);
    redTop->setScaleY(1.0f);
    redTop->initializeScale();
    redTop->initializeDimensions();

    redBottom->setScaleX((float)screen_x / redBottom->getTex().getSize().x);
    redBottom->setScaleY(1.0f);
    redBottom->initializeScale();
    redBottom->initializeDimensions();

    redLeft->setScaleX(1.0f);
    redLeft->setScaleY((float)screen_y / redLeft->getTex().getSize().y);
    redLeft->initializeScale();
    redLeft->initializeDimensions();

    redRight->setScaleX(1.0f);
    redRight->setScaleY((float)screen_y / redRight->getTex().getSize().y);
    redRight->initializeScale();
    redRight->initializeDimensions();

    this->redHue = new SfmlObject();
    this->redHue->getTex().loadFromFile("Sprites/white.png");
    this->redHue->setScaleX((float)screen_x);
    this->redHue->setScaleY((float)screen_y);
    this->redHue->initializeScale();
    this->redHue->initializeDimensions();

    // weapon overlay 
    //this->weaponSprite = new SfmlObject();

    // weapon textures 
    // weaponTextures[0] = Texture(); // knife
    // weaponTextures[1].loadFromFile("Sprites/weapon-pistol.png");
    // // slots 2,3,4,5 are loaded dynamically when weapon is picked up
    // for (int i = 2; i < 5; i++)
    //     weaponTextures[i] = Texture();
}

void PlayerSoldier::setWeaponTexture(int slot, const char* path) {
    if (slot < 0 || slot >= 5) return;
    weaponTextures[slot].loadFromFile(path);
    // if this is the currently held weapon, refresh the overlay immediately
    if (slot == inventory.currentWeapon) {
        lastWeapon = -1; // force refresh in updateWeaponSprite
    }
}

void PlayerSoldier::setDevMode(bool on) {
    for (int i = inventory.weapons.getSize() - 1; i >= 0; i--) {
        delete inventory.weapons[i];
        inventory.weapons.remove(i);
    }
    inventory.weapons.addByIndex(new Knife(this), 0);
    inventory.weapons.addByIndex(new Pistol(this), 1);
    if (on) {
        inventory.weapons.addByIndex(new HeavyMachineGun(this), 2);
        inventory.weapons.addByIndex(new RocketLauncher(this), 3);
        inventory.weapons.addByIndex(new FlameShot(this), 4);
        inventory.weapons.addByIndex(new LaserGun(this), 5);
        this->immortal = true;
    }
    else {
        this->immortal = false;
    }
    inventory.currentWeapon = 1;
}


void PlayerSoldier::drawWeaponOverlay() {
    // if (inventory.currentWeapon == 0) 
    //     return;
    // weaponSprite->setTex(weaponTextures[inventory.currentWeapon]);
    // weaponSprite->setScaleX(48.0f / (float)weaponTextures[inventory.currentWeapon].getSize().x);
    // weaponSprite->setScaleY(32.0f / (float)weaponTextures[inventory.currentWeapon].getSize().y);
    // weaponSprite->initializeScale();
    // weaponSprite->initializeDimensions();
    // lastWeapon = inventory.currentWeapon;
}

bool PlayerSoldier::canSpawnFusion() const { return !fusionUsedThisLevel && fusionCooldown <= 0.0f; }
void PlayerSoldier::consumeFusion() { fusionUsedThisLevel = true; fusionCooldown = 180.0f; }


void PlayerSoldier::assignHP(int hp) {
    this->HP = hp;
}

void PlayerSoldier::applyFireRate() {
    for (int i = 2; i < this->inventory.weapons.getSize(); i++) {
        this->inventory.weapons[i]->boostFireRate(1);
    }
}

void PlayerSoldier::setKnifeSprite() {
    for (int i = 0; i < animations.getSize(); i++)
        animations[i]->setActive(false);
    animations[2]->setActive(true);
    animations[2]->reset();
}

void PlayerSoldier::setShootSprite() {
    for (int i = 0; i < animations.getSize(); i++)
        animations[i]->setActive(false);
    animations[3]->setActive(true);
    animations[3]->reset();
}

void PlayerSoldier::resetSprite() {
    for (int i = 2; i < animations.getSize(); i++) {
        animations[i]->setActive(false);
        animations[i]->reset();
    }
}

void PlayerSoldier::updateWeaponSprite() {
    // if (usingMelee || inventory.mummyState || inventory.currentWeapon == 0) {
    //     weaponSprite->setScaleX(0.0f);
    //     weaponSprite->setScaleY(0.0f);
    //     return;
    // }
    // //restore sprite
    // float dispW = 96.0f;
    // float dispH = 48.0f;
    // if (weaponSprite->getTex().getSize().x > 0) {
    //     weaponSprite->setScaleX(dispW / weaponSprite->getTex().getSize().x);
    //     weaponSprite->setScaleY(dispH / weaponSprite->getTex().getSize().y);
    // }

    // // swap texture only if weapon changed
    // if (inventory.currentWeapon != lastWeapon) {
    //     lastWeapon = inventory.currentWeapon;
    //     if (inventory.currentWeapon < 5) {
    //         weaponSprite->setTex(weaponTextures[inventory.currentWeapon]);
    //     }
    //     if (weaponSprite->getTex().getSize().x > 0) {
    //         weaponSprite->setScaleX(dispW / weaponSprite->getTex().getSize().x);
    //         weaponSprite->setScaleY(dispH / weaponSprite->getTex().getSize().y);
    //     }
    //     weaponSprite->initializeScale();
    //     weaponSprite->initializeDimensions();
    // }
}
void PlayerSoldier::setAndDraw() {
    float drawX = this->x - level->getScreenLeft();
    float drawY = this->y - level->getScreenTop();

    if (sprite.getScale().x < 0)
        drawX += this->width;

    sprite.setPosition(drawX, drawY);
    window->draw(sprite);

    // // draw weapon overlay if not knife
    // if (!usingMelee && inventory.currentWeapon != 0 && !inventory.mummyState) {
    //     Weapon* w = inventory.weapons[inventory.currentWeapon];
    //     if (w == nullptr) return;

    //     // weapon sprite position — sits at the hand area
    //     float weapW = weaponSprite->getWidth();
    //     float weapH = weaponSprite->getHeight();

    //     float weapX, weapY;
    //     if (facingRight) {
    //         weapX = drawX + this->width * 0.4f;
    //     }
    //     else {
    //         weapX = drawX + this->width * 0.1f - weapW;
    //     }
    //     weapY = drawY + this->height * 0.35f;

    //     // flip sprite based on direction
    //     weaponSprite->getSprite().setScale(facingRight ? weaponSprite->getScaleX() : -weaponSprite->getScaleX(), weaponSprite->getScaleY());
    //     weaponSprite->getSprite().setPosition(facingRight ? weapX : weapX + weapW, weapY);
    //     window->draw(weaponSprite->getSprite());

    //     // draw muzzle flash if active
    //     if (w->getFireAnim() != nullptr && w->getFireAnim()->isActive()) {
    //         float muzzW = w->getFireAnim()->getScaleX() * w->getFireAnim()->getTexture().getSize().x / w->getFireAnim()->getTotalFrames();
    //         float muzzX, muzzY;
    //         if (facingRight) {
    //             muzzX = weapX + weapW + w->getMuzzleOffsetX();
    //         }
    //         else {
    //             muzzX = weapX - w->getMuzzleOffsetX() - muzzW;
    //         }
    //         muzzY = weapY + w->getMuzzleOffsetY();
    //         Sprite muzzSprite;
    //         muzzSprite.setTexture(w->getFireAnim()->getTexture());
    //         muzzSprite.setTextureRect(w->getFireAnim()->getRect());
    //         muzzSprite.setScale(facingRight ? w->getFireAnim()->getScaleX() : -w->getFireAnim()->getScaleX(), w->getFireAnim()->getScaleY());
    //         muzzSprite.setPosition(facingRight ? muzzX : muzzX + muzzW, muzzY);
    //         window->draw(muzzSprite);
    //     }
    // }


    // // debug collision box //TEMPORARAY ///////////////////////////////////////////////////////////////////////////////
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // RectangleShape box;
    // box.setSize(Vector2f(this->width, this->height));
    // box.setFillColor(Color(0, 0, 0, 0));
    // box.setOutlineColor(Color(255, 0, 0, 255));
    // box.setOutlineThickness(1.0f);
    // box.setPosition(this->x - level->getScreenLeft(), this->y - level->getScreenTop());
    // window->draw(box);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void PlayerSoldier::updateAnimations() {
    // action animations (knife, shoot, death) take priority
    for (int i = 2; i < animations.getSize(); i++) {
        if (animations[i]->isActive()) {
            animations[i]->update();
            Tex = animations[i]->getTexture();
            sprite.setTexture(Tex);
            sprite.setScale(facingRight ? animations[i]->getScaleX() : -animations[i]->getScaleX(), animations[i]->getScaleY());
            sprite.setTextureRect(animations[i]->getRect());
            if (!animations[i]->isActive()) {
                animations[i]->reset();
                setUsingMelee(false);
                setIsAttacking(false);
            }
            return;
        }
    }

    // idle vs walk
    if (velocityX != 0) {
        animations[0]->setActive(false);
        animations[0]->reset();
        animations[1]->setActive(true);
    }
    else {
        animations[1]->setActive(false);
        animations[1]->reset();
        animations[0]->setActive(true);
    }

    for (int i = 0; i <= 1; i++) {
        if (animations[i]->isActive()) {
            animations[i]->update();
            Tex = animations[i]->getTexture();
            sprite.setTexture(Tex);
            sprite.setScale(
                facingRight ? animations[i]->getScaleX() : -animations[i]->getScaleX(),
                animations[i]->getScaleY()
            );
            sprite.setTextureRect(animations[i]->getRect());
            break;
        }
    }
}

void PlayerSoldier::checkPowerup() {
    if (powerupActive) {
        powerupTimer -= deltaTime;
        if (powerupTimer <= 0.0f) {
            powerupTimer = 0.0f;
            powerupActive = false;
            powerupEnd();
        }
    }
}
void PlayerSoldier::behaviour() {
    // ============= RESPAWN COUNTDOWN =============
    // While respawning, the player is hidden and frozen. After 1 second elapses,
    // we drop them 5 blocks behind their current column on whatever the surface
    // row is at that point.
    if (respawning) {
        respawnTimer -= deltaTime;
        if (respawnTimer <= 0.0f) {
            respawnTimer = 0.0f;
            respawning = false;

            if (level != nullptr && level->getMap() != nullptr) {
                Map* m = level->getMap();
                int cs = m->getCellSize();
                int mapW = m->getWidth();

                // figure out current column from current x
                int col = (int)((this->x + this->width / 2.0f) / (float)cs);

                // 5 blocks "behind" -> shift away from facing direction
// 5 blocks behind = 5 columns to the left, since levels progress
                // rightward. this is unambiguous regardless of where the mouse
                // (which controls facingRight) was pointing at death.
                int targetCol = col - 10;

                // clamp to map bounds so we never index out of the grid
                if (targetCol < 0) targetCol = 0;
                if (targetCol > mapW - 1) targetCol = mapW - 1;

                int sr = level->surfaceRow(m->getLvl(), targetCol);

                // place player so their feet rest on the surface block
                this->x = (float)(targetCol * cs);
                this->y = (float)(sr * cs) - this->height;
            }
            else {
                // no level info available -> fall back to the original spawn point
                this->x = spawnX;
                this->y = spawnY;
            }

            velocityX = 0;
            velocityY = 0;
            // grant a brief invincibility window after reappearing
            postRespawnInvincibilityTimer = 2.0f;
        }
        // while respawning, skip movement, attack, animations, and drawing
        // so the player visually disappears for the duration of the timer.
        return;
    
    }

    if (fusionCooldown > 0.0f) fusionCooldown -= deltaTime;

    if (vehicle != nullptr) {
        this->x = vehicle->getX();
        this->y = vehicle->getY();
        updateDamageState();
        return;
    }
    movement();
    attack();
    //updateWeaponSprite();
    updateAnimations();
    updateDamageState();
    checkPowerup();

    // ============= POST-RESPAWN INVINCIBILITY FLICKER =============
    // tick invincibility timer + flicker the sprite by skipping draw on alternating
    // ~0.1s windows. the player is still fully functional during this window.
    bool drawThisFrame = true;
    if (postRespawnInvincibilityTimer > 0.0f) {
        postRespawnInvincibilityTimer -= deltaTime;
        if (postRespawnInvincibilityTimer < 0.0f) postRespawnInvincibilityTimer = 0.0f;
        // alternate every ~0.1s: 0.0-0.1 hide, 0.1-0.2 show, 0.2-0.3 hide, ...
        int bucket = (int)(postRespawnInvincibilityTimer * 10.0f);
        drawThisFrame = (bucket % 2 == 0);
    }
    if (drawThisFrame) setAndDraw();
    drawRedHue();

}
void PlayerSoldier::slugBonus() {
}

void PlayerSoldier::vehicleFireRateMultiplier() {
}

void PlayerSoldier::vehicleDuribilityMultiplier() {
}

void PlayerSoldier::extraLife() {
}

void PlayerSoldier::movementMultiplier() {
}

void PlayerSoldier::blastRadiusMultiplier() {
}

// void PlayerSoldier::meleeDmgMultiplier() {
// }

void PlayerSoldier::ammoMultiplier() {
}

void PlayerSoldier::meleePierce() {
}

void PlayerSoldier::meleeDamageMultipler() {
    // fill later
}

void PlayerSoldier::fusionPowerup() {
    // fill later
}

void PlayerSoldier::movement() {
    updateTileChecks(this->x, this->y);

    if (Keyboard::isKeyPressed(Keyboard::D) && !horizontalCollisionCheck(' ', midRight, bottomRight)) {
        velocityX += acceleration;
        if (velocityX > max_speed) {
            velocityX = max_speed;
        }
        facingRight = true;
    }
    else if (Keyboard::isKeyPressed(Keyboard::A) && !horizontalCollisionCheck(' ', midLeft, bottomLeft)) {
        velocityX -= acceleration;
        if (velocityX < -max_speed) {
            velocityX = -max_speed;
        }
        facingRight = false;
    }

    else {
        velocityX = 0;
    }



    // ============= JUMPING =============
    if (Keyboard::isKeyPressed(Keyboard::W) && !isJumping && onGround) {
        velocityY = -jumpForce;
        isJumping = true;
    }

    if (onGround == true)
    {
        isJumping = false;
    }

    // ============= CROUCHING =============
    if (Keyboard::isKeyPressed(Keyboard::S) && !isJumping) {
        isCrouching = true;
    }
    else {
        isCrouching = false;
    }

    // ============= GRAVITY =============
    entityGravity();


    // ============= APPLY MOVEMENT =============
    x += velocityX;

    // player follows mouse 
    float mouseScreenX = (float)Mouse::getPosition(*window).x;
    float playerScreenX = this->x - level->getScreenLeft() + this->width / 2.0f;
    if (mouseScreenX > playerScreenX)
        facingRight = true;
    else
        facingRight = false;

    clampToMap();
}

void PlayerSoldier::clampToMap() {
    if (level == nullptr) return;
    if (!level->hasFiniteBounds()) return;
    Map* m = level->getMap();
    if (m == nullptr) return;
    float rightEdge = (float)(m->getWidth() * m->getCellSize()) - this->width;
    if (x < 0) x = 0;
    if (x > rightEdge + 2 * m->getCellSize()) x = rightEdge + 2 * m->getCellSize();
}

void PlayerSoldier::setIsAttacking(bool val)
{
    this->isAttacking = val;
}

void PlayerSoldier::setUsingMelee(bool val)
{
    this->usingMelee = val;
}

void PlayerSoldier::respawn() {
    // start the respawn sequence: player hides, freezes, then reappears 5 blocks
    // behind on the surface layer when the timer hits 0 (handled in behaviour()).
    respawning = true;
    respawnTimer = 1.0f;
    velocityX = 0;
    velocityY = 0;
    damageState = 0;
    damageStateTimer = 0.0f;

    // restore base transformation state
    switchTransformationState(new BaseState());
}
Inventory& PlayerSoldier::getAndSetInventory() {
    return this->inventory;
}

void PlayerSoldier::makeImmortal() {
    this->immortal = true;
}

void PlayerSoldier::makeMortal() {
    this->immortal = false;
}

void PlayerSoldier::attack() {
    // mummy state locks to knife only
    if (inventory.mummyState) {
        inventory.currentWeapon = 0;
        if (Mouse::isButtonPressed(Mouse::Right)) {
            setUsingMelee(true);
            setIsAttacking(true);
            setKnifeSprite();
            inventory.weapons[0]->fire();
        }
        inventory.weapons[0]->update();
        inventory.handGrenade->update();
        inventory.fireGrenade->update();
        return;   // block all other weapon input
    }

    if (hasMelee() && Mouse::isButtonPressed(Mouse::Right))
    {
        setUsingMelee(true);
        setIsAttacking(true);
        setKnifeSprite();
        this->inventory.weapons[0]->fire();
    }

    if (Mouse::isButtonPressed(Mouse::Left))
    {
        this->inventory.weapons[this->inventory.currentWeapon]->fire();
    }

    if (Keyboard::isKeyPressed(Keyboard::R))
    {
        this->inventory.weapons[this->inventory.currentWeapon]->reload();
    }

    if (Keyboard::isKeyPressed(Keyboard::F))
    {
        inventory.handGrenade->fire();
    }

    if (Keyboard::isKeyPressed(Keyboard::E))
    {
        inventory.fireGrenade->fire();
    }



    if (Keyboard::isKeyPressed(Keyboard::Num1))
    {
        if (1 < this->inventory.weapons.getSize())
            this->inventory.currentWeapon = 1;
    }
    if (Keyboard::isKeyPressed(Keyboard::Num2))
    {
        if (2 < this->inventory.weapons.getSize())
            this->inventory.currentWeapon = 2;
    }
    if (Keyboard::isKeyPressed(Keyboard::Num3))
    {
        if (3 < this->inventory.weapons.getSize())
            this->inventory.currentWeapon = 3;
    }
    if (Keyboard::isKeyPressed(Keyboard::Num4))
    {
        if (4 < this->inventory.weapons.getSize())
            this->inventory.currentWeapon = 4;
    }

    if (Keyboard::isKeyPressed(Keyboard::Num5))
    {
        if (5 < this->inventory.weapons.getSize())
            this->inventory.currentWeapon = 5;
    }

    for (int i = 0; i < this->inventory.weapons.getSize(); i++)
    {
        this->inventory.weapons[i]->update();
    }

    this->inventory.handGrenade->update();
    this->inventory.fireGrenade->update();
}


void PlayerSoldier::drawHUD() {

    // ---- TOP LEFT : HP and Lives ----
    MyString hpText = "HP: ";
    {
        int n = getHP();
        char buf[16];
        int idx = 0;
        if (n < 0)
        {
            buf[idx] = '-';
            idx += 1;
            n = -n;
        }
        if (n == 0)
        {
            buf[idx] = '0';
            idx += 1;
        }
        else
        {
            char tmp[16];
            int t = 0;
            while (n > 0)
            {
                tmp[t] = '0' + (n % 10);
                t += 1;
                n /= 10;
            }
            while (t > 0)
            {
                t -= 1;
                buf[idx] = tmp[t];
                idx += 1;
            }
        }
        buf[idx] = '\0';
        hpText += buf;
    }

    MyString livesText = "LIVES: ";
    {
        int n = lives;
        char buf[16];
        int idx = 0;
        if (n < 0)
        {
            buf[idx] = '-';
            idx += 1;
            n = -n;
        }
        if (n == 0)
        {
            buf[idx] = '0';
            idx += 1;
        }
        else
        {
            char tmp[16];
            int t = 0;
            while (n > 0)
            {
                tmp[t] = '0' + (n % 10);
                t += 1;
                n /= 10;
            }
            while (t > 0)
            {
                t -= 1;
                buf[idx] = tmp[t];
                idx += 1;
            }
        }
        buf[idx] = '\0';
        livesText += buf;
    }

    Text hpDraw;
    hpDraw.setFont(hudFont);
    hpDraw.setCharacterSize(22);
    hpDraw.setFillColor(Color::White);
    hpDraw.setString(hpText.c_str());
    hpDraw.setPosition(20, 20);
    window->draw(hpDraw);

    Text livesDraw;
    livesDraw.setFont(hudFont);
    livesDraw.setCharacterSize(22);
    livesDraw.setFillColor(Color::White);
    livesDraw.setString(livesText.c_str());
    livesDraw.setPosition(20, 50);
    window->draw(livesDraw);

    if (inventory.currentWeapon >= 0 && inventory.currentWeapon < inventory.weapons.getSize())
    {
        Weapon* w = inventory.weapons[inventory.currentWeapon];
        if (w != nullptr)
        {
            MyString weaponLine = w->getDisplayName();

            Text weaponDraw;
            weaponDraw.setFont(hudFont);
            weaponDraw.setCharacterSize(20);
            weaponDraw.setFillColor(Color::Yellow);
            weaponDraw.setString(weaponLine.c_str());
            weaponDraw.setPosition(20, 85);
            window->draw(weaponDraw);
        }
    }

    //Text title;
    //title.setFont(hudFont);
    //title.setCharacterSize(60);
    //title.setFillColor(Color::Red);
    //title.setString("METAL SLUG");
    //title.setPosition((float)(screen_x/2 - 250), 15);
    //window->draw(title);

    if (level != nullptr)
    {
        MyString lvlText = level->getDisplayName();

        Text lvlDraw;
        lvlDraw.setFont(hudFont);
        lvlDraw.setCharacterSize(28);
        lvlDraw.setFillColor(Color::White);
        lvlDraw.setString(lvlText.c_str());
        int approxWidth = lvlText.length() * 14;
        lvlDraw.setPosition((float)((screen_x - approxWidth) / 2), (float)(screen_y - 50));
        window->draw(lvlDraw);
    }
}

HandGrenade* PlayerSoldier::getActiveGrenade() {
    return inventory.handGrenade;
}


int PlayerSoldier::getLives() {
    return this->lives;
}

void PlayerSoldier::setLives(int lives) {
    this->lives = lives;
}

void PlayerSoldier::onMeleeStart() {
    setUsingMelee(true);
    setIsAttacking(true);
    setKnifeSprite();
}

void PlayerSoldier::onMeleeEnd() {
    setUsingMelee(false);
    setIsAttacking(false);
    resetSprite();
}

float PlayerSoldier::getShootAngle() const {
    float mouseWorldX = (float)Mouse::getPosition(*window).x + level->getScreenLeft();
    float mouseWorldY = (float)Mouse::getPosition(*window).y + level->getScreenTop();
    float dx = mouseWorldX - (this->x + this->width / 2.0f);
    float dy = mouseWorldY - (this->y + this->height / 2.0f);
    float angle = atan2(dy, dx) * 180.0f / 3.14159265f;
    angle += (rand() % 11 - 5);
    return angle;
}

void PlayerSoldier::replenishOnBatchKill() {
    for (int i = 0; i < inventory.weapons.getSize(); i++) {
        Weapon* w = inventory.weapons[i];
        if (w == nullptr) {
            continue;
        }
        w->addAmmo(60);
    }
    if (inventory.handGrenade != nullptr) {
        inventory.handGrenade->addAmmo(2);
    }
    if (inventory.fireGrenade != nullptr) {
        inventory.fireGrenade->addAmmo(2);
    }
}


void PlayerSoldier::setSpawn(float x, float y) {
    spawnX = x;
    spawnY = y;
}

void PlayerSoldier::onHit() {
    if (immortal)
        return;

    if (damageState == 0) {
        damageState = 1;
        damageStateTimer = 1.0f;
    }
    else if (damageState == 1) {
        damageState = 2;
        damageStateTimer = 1.0f;
    }
    else if (damageState == 2) {
        damageState = 3;       // singnalling the coming of DEATHHHH!!!
        damageStateTimer = 0.0f;
        handleDeath();
    }
}

void PlayerSoldier::updateDamageState() {
    // tick transformation state every frame
    transformationState->update(this);

    // tick damage window
    if (damageState == 1 || damageState == 2) {
        damageStateTimer -= deltaTime;
        if (damageStateTimer <= 0.0f) {
            damageState = 0;   // window expired, recovered
            damageStateTimer = 0.0f;
        }
    }
}

void PlayerSoldier::handleDeath() {
    lives--;
    if (lives < 0) {
        // wipe transient state so leftover flags can't bleed into anything
        // that might reference this player after death.
        respawning = false;
        respawnTimer = 0.0f;
        postRespawnInvincibilityTimer = 0.0f;
        damageState = 0;
        damageStateTimer = 0.0f;
        velocityX = 0;
        velocityY = 0;
        dead = true;           // EntityManager sweeps this
        return;
    }
    respawn();
}


void PlayerSoldier::switchTransformationState(TransformationState* newState) {
    if (transformationState != nullptr) {
        transformationState->removeEffects(this);
        delete transformationState;
    }
    transformationState = newState;
    transformationState->applyEffects(this);
}

void PlayerSoldier::drawRedHue() {
    if (damageState == 0)
        return;

    // imma just draw the sprites more times for intensity (depending on the damage state), hence the "passes"
    int passes = (damageState == 1) ? 1 : 3;

    redTop->getSprite().setPosition(0, 0);
    redBottom->getSprite().setPosition(0, screen_y - redBottom->getHeight());
    redLeft->getSprite().setPosition(0, 0);
    redRight->getSprite().setPosition(screen_x - redRight->getWidth(), 0);

    for (int i = 0; i < passes; i++) {
        window->draw(redTop->getSprite());
        window->draw(redBottom->getSprite());
        window->draw(redLeft->getSprite());
        window->draw(redRight->getSprite());
    }
}

void PlayerSoldier::receiveDamageZone(DamageZone& zone) {
    if (postRespawnInvincibilityTimer > 0.0f)
        return;
    if (dead)
        return;
    if (immortal)
        return;
    if (vehicle != nullptr)
        return;

    if (!(zone.getX() < this->x + this->width && zone.getX() + zone.getWidth() > this->x && zone.getY() < this->y + this->height && zone.getY() + zone.getHeight() > this->y))
        return;

    if (!zone.damagesPlayers()) return;
    // hit machine starts
    onHit();
    // apply zombie/mummy transformation if zone carries one
    zone.applySpecialEffect(this);
    zone.onHit();
}
PlayerSoldier::~PlayerSoldier() {
    for (int i = 0; i < animations.getSize(); i++) {
        if (animations[i] != nullptr) {
            delete animations[i];
            animations[i] = nullptr;
        }
    }
    delete redHue;
    delete weaponSprite;
    redHue = nullptr;
    weaponSprite = nullptr;

    delete redTop;
    delete redBottom;
    delete redLeft;
    delete redRight;
}

void PlayerSoldier::setVehicle(Vehicle* v) {
    this->vehicle = v;
}

Vehicle* PlayerSoldier::getVehicle() const {
    return this->vehicle;
}

void PlayerSoldier::increaseSaturation(int amount) {
    saturation += amount;
    if (saturation >= 100) {
        saturation = 100;
        fat = true;
    }
}

void PlayerSoldier::applyTransformSprite(const char* path) {
    savedTransformTex = Tex;
    savedTransformScaleX = scaleX;
    savedTransformScaleY = scaleY;

    for (int i = 0; i < animations.getSize(); i++)
        animations[i]->setActive(false);

    Tex.loadFromFile(path);
    sprite.setTexture(Tex);
    sprite.setTextureRect(IntRect(0, 0, Tex.getSize().x, Tex.getSize().y));
    setScale(96.0f / Tex.getSize().x, 128.0f / Tex.getSize().y);
}

void PlayerSoldier::restoreTransformSprite() {
    Tex = savedTransformTex;
    sprite.setTexture(Tex);
    setScale(savedTransformScaleX, savedTransformScaleY);
    animations[0]->setActive(true);   // back to idle
}



void PlayerSoldier::writeStateTo(CurrentState& s) const {
    if (s.playerCount >= 4) return;    // don't overflow the players[4] array
    PlayerState& p = s.players[s.playerCount];
    p.id = entityId;      // matches us back on rewind
    p.x = getX();
    p.y = getY();
    p.HP = HP;
    p.lives = lives;
    p.activeWeaponIndex = inventory.currentWeapon;
    p.activeWeaponAmmo = 0;
    p.vehicleID = -1;
    p.immortal = immortal;
    p.dead = dead;
    s.playerCount++;
}

void PlayerSoldier::applyStateFrom(const CurrentState& s) {
    for (int i = 0; i < s.playerCount; i++) {
        if (s.players[i].id != entityId) continue;   // find our own slot by id
        const PlayerState& p = s.players[i];
        setX(p.x);
        setY(p.y);
        assignHP(p.HP);
        lives = p.lives;
        inventory.currentWeapon = p.activeWeaponIndex;
        if (p.immortal && !immortal) makeImmortal();
        else if (!p.immortal && immortal) makeMortal();
        dead = p.dead;
        return;
    }
    // not found in snapshot means we didn't exist yet at that frame — leave alone for v1
}


bool PlayerSoldier::shouldDrawDuringRewind() const {
    return level->getCurrentPlayer() == this;     // only the active player renders during rewind
}

// ============= MARCO =============

Marco::Marco(float x, float y, Level* level) : PlayerSoldier(x, y, level) {
    float charW = 96.0f;
    float charH = 128.0f;
    this->HP = 100;
    this->max_speed = 5.0f;
    //idle animation , animation[0]
    LoopAnimation* idle = new LoopAnimation(1, 1 / 8.f, false);
    idle->loadTexture("Sprites/marco-idle.png");
    idle->setActive(true);   // idle is default
    idle->setScale(0.5f, 0.5f);
    animations.add(idle);

    // walk animation , animation[1]aaaaaaaa
    LoopAnimation* walk = new LoopAnimation(12, 1 / 12.f, true);
    walk->loadTexture("Sprites/marco-walk.png");
    walk->setActive(false);
    IntRect walkFrames[12] = {
        IntRect(0, 0, 32, 40),
        IntRect(32, 0, 32, 41),
        IntRect(64, 0, 32, 37),
        IntRect(96, 0, 27, 40),
        IntRect(123, 0, 29,40),
        IntRect(152, 0, 30, 40),
        IntRect(182, 0, 32, 40),
        IntRect(214, 0, 35, 41),
        IntRect(249, 0, 36, 37),
        IntRect(285, 0, 31, 40),
        IntRect(316, 0, 31, 40),
        IntRect(347, 0, 31, 40)
    };
    walk->setCustomFrames(walkFrames, 12);
    walk->setScale(charW / 32.f, charH / 41.f);
    animations.add(walk);

    // knife attack animation , animation[2]
    OneShotAnimation* knife = new OneShotAnimation(4, 1 / 12.f, true);
    knife->loadTexture("Sprites/marco-knife.png");
    knife->setActive(false);
    IntRect knifeFrames[4] = {
        IntRect(1, 1, 292, 363),
        IntRect(295, 1, 277, 363),
        IntRect(574, 1, 268, 363),
        IntRect(844, 1, 397, 355)
    };
    knife->setCustomFrames(knifeFrames, 4);
    knife->setScale(charW / ((float)knife->getTexture().getSize().x / 4), charH / (float)knife->getTexture().getSize().y);
    animations.add(knife);

    // sync main sprite to idle texture to start
    this->setTex(animations[0]->getTexture());
    this->setScaleX(0.5f);
    this->setScaleY(0.5f);
    this->initializeScale();
    this->initializeDimensions();
    this->width = (this->getTex().getSize().x) * this->getScaleX();
    this->height = this->getTex().getSize().y * this->getScaleY();

    //set the spawn point for resurrection 
    this->spawnX = x;
    this->spawnY = y;
    this->damageState = 0;
    this->damageStateTimer = 0.0f;

    this->powerupActive = false;
    this->powerupUsed = false;
    this->powerupTimer = 10.0f;
    this->inventory.handGrenade->addAmmo(-2);
    this->level = level;

    drawWeaponOverlay();
    meleePierce();
    applyFireRate();
}

void Marco::meleePierce() {
    this->getAndSetInventory().weapons[0]->setPierceShield();
}

void Marco::applyFireRate() {
    for (int i = 2; i < this->inventory.weapons.getSize(); i++) {
        this->inventory.weapons[i]->boostFireRate(1.25);
    }
}

void Marco::attack() {
    if (Keyboard::isKeyPressed(Keyboard::Q))
        powerup();
    PlayerSoldier::attack();
    if (powerupActive && Mouse::isButtonPressed(Mouse::Left) && inventory.currentWeapon != 0) { //weapon != 0 to prevent knife
        Weapon* w = inventory.weapons[inventory.currentWeapon];
        if (w->getCooldownTimer() == 0.0f) {  // only fired if cooldown just reset
            ProjectileWeapon* projectileWeapon = (ProjectileWeapon*)w;
            projectileWeapon->fireAtAngle(getShootAngle() + 180.0f);
        }
    }
}

void Marco::powerup() {
    if (powerupUsed || powerupActive) return;
    powerupActive = true;
    powerupUsed = true;
    powerupTimer = 10.0f;
}

// ============= TARMA =============

Tarma::Tarma(float x, float y, Level* level) : PlayerSoldier(x, y, level) {
    float charW = 96.0f;
    float charH = 128.0f;

    // animation[0] — idle PLACEHOLDER
    LoopAnimation* idle = new LoopAnimation(1, 1.0f / 1.0f, false);
    idle->loadTexture("Sprites/tarma-idle.png");
    idle->setActive(true);
    idle->setScale(96.f / 121.f, 128.f / 122.f);
    animations.add(idle);

    // animation[1] — walk PLACEHOLDER
    LoopAnimation* walk = new LoopAnimation(1, 1.0f / 1.0f, false);
    walk->loadTexture("Sprites/tarma-idle.png");;
    //IntRect walkFrames[12] = {
    //    IntRect(0,0,32,40), IntRect(32,0,32,41),
    //    IntRect(64,0,32,37), IntRect(96,0,27,40),
    //    IntRect(123,0,29,40), IntRect(152,0,30,40),
    //    IntRect(182,0,32,40), IntRect(214,0,35,41),
    //    IntRect(249,0,36,37), IntRect(285,0,31,40),
    //    IntRect(316,0,31,40), IntRect(347,0,31,40)
    //};
    //walk->setCustomFrames(walkFrames, 12);
    //walk->setScale(charW / 32.f, charH / 41.f);
    walk->setScale(96.f / 121.f, 128.f / 122.f);
    walk->setActive(false);
    animations.add(walk);

    // animation[2] — knife PLACEHOLDER
    OneShotAnimation* knife = new OneShotAnimation(1, 1.0f / 1.0f, false);
    knife->loadTexture("Sprites/tarma-idle.png");
    knife->setActive(false);
    //IntRect knifeFrames[4] = {
    //    IntRect(1,1,292,363), IntRect(295,1,277,363),
    //    IntRect(574,1,268,363), IntRect(844,1,397,355)
    //};
    //knife->setCustomFrames(knifeFrames, 4);
    //knife->setScale(charW / ((float)knife->getTexture().getSize().x / 4), charH / (float)knife->getTexture().getSize().y);
    knife->setScale(96.f / 121.f, 128.f / 122.f);
    animations.add(knife);

    // animation[3] — shoot PLACEHOLDER
    OneShotAnimation* shoot = new OneShotAnimation(1, 1.0f / 1.0f, false);
    shoot->loadTexture("Sprites/tarma-idle.png");
    shoot->setActive(false);
    //IntRect shootFrames[4] = {
    //    IntRect(1,1,292,363), IntRect(295,1,277,363),
    //    IntRect(574,1,268,363), IntRect(844,1,397,355)
    //};
    //shoot->setCustomFrames(shootFrames, 4);
    //shoot->setScale(charW / ((float)shoot->getTexture().getSize().x / 4), charH / (float)shoot->getTexture().getSize().y);
    shoot->setScale(96.f / 121.f, 128.f / 122.f);
    animations.add(shoot);

    // animation[4] — death PLACEHOLDER
    OneShotAnimation* death = new OneShotAnimation(1, 1.0f / 1.0f, false);
    death->loadTexture("Sprites/tarma-idle.png");
    death->setActive(false);
    death->setScale(96.f / 121.f, 128.f / 122.f);
    animations.add(death);

    this->setTex(animations[0]->getTexture());
    this->setScaleX(charW / 121.f);
    this->setScaleY(charH / 122.f);
    this->initializeScale();
    this->initializeDimensions();
    this->width = this->getTex().getSize().x * this->getScaleX();
    this->height = this->getTex().getSize().y * this->getScaleY();
    this->spawnX = x;
    this->spawnY = y;
    this->damageState = 0;
    this->damageStateTimer = 0.0f;
    this->level = level;


    drawWeaponOverlay();
    movementMultiplier();
    assignHP();
}
//
void Tarma::assignHP() {
    this->HP = 80;
}

//
void Tarma::vehicleFireRateMultiplier() {
    if (vehicle == nullptr) return;
    vehicle->getWeapon()->boostFireRate(1.25f);
}

//
void Tarma::vehicleDuribilityMultiplier() {
    if (vehicle == nullptr) return;
    vehicle->changeHP(vehicle->getHP() * 0.2f);
}
//
void Tarma::extraLife() {
    /*
    // called when vehicle destroyed
    // Tarma ejects and survives once
    // wire to vehicle destruction event
    */
}
//
void Tarma::powerup() {
    if (powerupUsed || powerupActive)
        return;
    powerupActive = true;
    powerupUsed = true;
    powerupTimer = 20.0f;
    makeImmortal();
}
//
void Tarma::movementMultiplier() {
    this->max_speed = 4.0f;
}
void Tarma::powerupEnd() {
    makeMortal();
}
//
//
//// ============= ERI =============

Eri::Eri(float x, float y, Level* level) : PlayerSoldier(x, y, level) {
    float charW = 96.0f;
    float charH = 128.0f;
    this->HP = 100;

    // animation[0] — idle PLACEHOLDER
    LoopAnimation* idle = new LoopAnimation(1, 1.0f / 1.0f, false);
    idle->loadTexture("Sprites/eri-idle.png");
    idle->setActive(true);
    idle->setScale(96.f / 300.f, 128.f / 258.f);
    animations.add(idle);

    // animation[1] — walk PLACEHOLDER
    LoopAnimation* walk = new LoopAnimation(1, 1.0f / 1.0f, false);
    walk->loadTexture("Sprites/eri-idle.png");;
    //IntRect walkFrames[12] = {
    //    IntRect(0,0,32,40), IntRect(32,0,32,41),
    //    IntRect(64,0,32,37), IntRect(96,0,27,40),
    //    IntRect(123,0,29,40), IntRect(152,0,30,40),
    //    IntRect(182,0,32,40), IntRect(214,0,35,41),
    //    IntRect(249,0,36,37), IntRect(285,0,31,40),
    //    IntRect(316,0,31,40), IntRect(347,0,31,40)
    //};
    //walk->setCustomFrames(walkFrames, 12);
    //walk->setScale(charW / 32.f, charH / 41.f);
    walk->setScale(96.f / 300.f, 128.f / 258.f);
    walk->setActive(false);
    animations.add(walk);

    // animation[2] — knife PLACEHOLDER
    OneShotAnimation* knife = new OneShotAnimation(1, 1.0f / 1.0f, false);
    knife->loadTexture("Sprites/eri-idle.png");
    knife->setActive(false);
    //IntRect knifeFrames[4] = {
    //    IntRect(1,1,292,363), IntRect(295,1,277,363),
    //    IntRect(574,1,268,363), IntRect(844,1,397,355)
    //};
    //knife->setCustomFrames(knifeFrames, 4);
    //knife->setScale(charW / ((float)knife->getTexture().getSize().x / 4), charH / (float)knife->getTexture().getSize().y);
    knife->setScale(96.f / 300.f, 128.f / 258.f);
    animations.add(knife);

    // animation[3] — shoot PLACEHOLDER
    OneShotAnimation* shoot = new OneShotAnimation(1, 1.0f / 1.0f, false);
    shoot->loadTexture("Sprites/eri-idle.png");
    shoot->setActive(false);
    //IntRect shootFrames[4] = {
    //    IntRect(1,1,292,363), IntRect(295,1,277,363),
    //    IntRect(574,1,268,363), IntRect(844,1,397,355)
    //};
    //shoot->setCustomFrames(shootFrames, 4);
    //shoot->setScale(charW / ((float)shoot->getTexture().getSize().x / 4), charH / (float)shoot->getTexture().getSize().y);
    shoot->setScale(96.f / 300.f, 128.f / 258.f);
    animations.add(shoot);

    // animation[4] — death PLACEHOLDER
    OneShotAnimation* death = new OneShotAnimation(1, 1.0f / 1.0f, false);
    death->loadTexture("Sprites/eri-idle.png");
    death->setActive(false);
    death->setScale(96.f / 300.f, 128.f / 258.f);
    animations.add(death);

    this->setTex(animations[0]->getTexture());
    this->setScaleX(charW / 300.f);
    this->setScaleY(charH / 258.f);
    this->initializeScale();
    this->initializeDimensions();
    this->width = this->getTex().getSize().x * this->getScaleX();
    this->height = this->getTex().getSize().y * this->getScaleY();
    this->spawnX = x;
    this->spawnY = y;
    this->damageState = 0;
    this->damageStateTimer = 0.0f;
    this->level = level;

    fWasPressed = false;
    eWasPressed = false;

    inventory.handGrenade->addAmmo(10);
    inventory.fireGrenade->addAmmo(10);

    hasMelee();
    blastRadiusMultiplier();
    applyFireRate();
    drawWeaponOverlay();
}
//
void Eri::blastRadiusMultiplier() {
    for (int i = 0; i < inventory.weapons.getSize(); i++)
        inventory.weapons[i]->scaleBlastRadius(1.5f);
    inventory.handGrenade->scaleBlastRadius(1.5f);
    inventory.fireGrenade->scaleBlastRadius(1.5f);
}
//
void Eri::powerup() {
    if (powerupUsed || powerupActive)
        return;
    powerupActive = true;
    powerupUsed = true;
    powerupTimer = 10.0f;
}
//
void Eri::applyFireRate() {
    // 20% lower fire rate for all weapons
    for (int i = 1; i < inventory.weapons.getSize(); i++)
        inventory.weapons[i]->boostFireRate(0.8f);
}
//}
//
void Eri::attack() {
    if (Keyboard::isKeyPressed(Keyboard::Q)) powerup();

    bool fNow = Keyboard::isKeyPressed(Keyboard::F);
    bool eNow = Keyboard::isKeyPressed(Keyboard::E);

    PlayerSoldier::attack();  // fires first grenade normally

    if (powerupActive) {
        if (fNow && !fWasPressed) {
            inventory.handGrenade->setOffset(2.0f * cell_size);
            inventory.handGrenade->fire();
        }
        if (eNow && !eWasPressed) {
            inventory.fireGrenade->setOffset(2.0f * cell_size);
            inventory.fireGrenade->fire();
        }
    }

    fWasPressed = fNow;
    eWasPressed = eNow;
}

//// ============= GERMI =============
Germi::Germi(float x, float y, Level* level) : PlayerSoldier(x, y, level) {
    float charW = 96.0f;
    float charH = 128.0f;
    this->HP = 100;

    LoopAnimation* idle = new LoopAnimation(1, 1.0f / 1.0f, false);
    idle->loadTexture("Sprites/germi-idle.png");
    idle->setActive(true);
    idle->setScale(charW / 333.f, charH / 264.f);
    animations.add(idle);

    LoopAnimation* walk = new LoopAnimation(1, 1.0f / 1.0f, false);
    walk->loadTexture("Sprites/germi-idle.png"); // PLACEHOLDER
    //IntRect walkFrames[12] = {
    //    IntRect(0,0,32,40), IntRect(32,0,32,41),
    //    IntRect(64,0,32,37), IntRect(96,0,27,40),
    //    IntRect(123,0,29,40), IntRect(152,0,30,40),
    //    IntRect(182,0,32,40), IntRect(214,0,35,41),
    //    IntRect(249,0,36,37), IntRect(285,0,31,40),
    //    IntRect(316,0,31,40), IntRect(347,0,31,40)
    //};
    //walk->setCustomFrames(walkFrames, 12);
    walk->setActive(false);
    walk->setScale(charW / 333.f, charH / 264.f);
    animations.add(walk);

    OneShotAnimation* knife = new OneShotAnimation(1, 1.0f / 1.0f, false);
    knife->loadTexture("Sprites/germi-idle.png"); // PLACEHOLDER
    knife->setActive(false);
    //IntRect knifeFrames[4] = {
    //    IntRect(1,1,292,363), IntRect(295,1,277,363),
    //    IntRect(574,1,268,363), IntRect(844,1,397,355)
    //};
    //knife->setCustomFrames(knifeFrames, 4);
    knife->setScale(charW / 333.f, charH / 264.f);
    animations.add(knife);

    OneShotAnimation* death = new OneShotAnimation(1, 1.0f / 1.0f, 0);
    death->loadTexture("Sprites/germi-idle.png"); // PLACEHOLDER
    death->setActive(false);
    death->setScale(charW / 333.0f, charH / 264.f);
    animations.add(death);

    this->setTex(animations[0]->getTexture());
    this->setScaleX(charW / 333.f);
    this->setScaleY(charH / 264.f);
    this->initializeScale();
    this->initializeDimensions();
    this->width = this->getTex().getSize().x * this->getScaleX();
    this->height = this->getTex().getSize().y * this->getScaleY();
    this->spawnX = x;
    this->spawnY = y;
    this->damageState = 0;
    this->damageStateTimer = 0.0f;
    this->level = level;

    inventory.handGrenade->addAmmo(-2);
    inventory.fireGrenade->addAmmo(-2);

    drawWeaponOverlay();
    meleeDamageMultipler();
    applyFireRate();
}
//
void Germi::meleeDamageMultipler() {
    inventory.weapons[0]->setDamage(inventory.weapons[0]->getDamage() * 0.75f);
}

void Germi::ammoMultiplier(int weaponSlot, int baseAmmo) {
    inventory.weapons[weaponSlot]->addAmmo((int)(baseAmmo * 0.5f));
}

void Germi::applyFireRate() {
    for (int i = 1; i < inventory.weapons.getSize(); i++)
        inventory.weapons[i]->boostFireRate(1.1f);
}

void Germi::powerup() {
    if (powerupUsed || powerupActive) return;
    powerupActive = true;
    powerupUsed = true;
    powerupTimer = 10.0f;
    for (int i = 1; i < inventory.weapons.getSize(); i++)
        inventory.weapons[i]->boostFireRate(2.0f);
}

void Germi::powerupEnd() {
    for (int i = 1; i < inventory.weapons.getSize(); i++)
        inventory.weapons[i]->resetFireRate();
    // reapply base 1.1x since resetFireRate goes back to original
    applyFireRate();
}

void Germi::attack() {
    if (Keyboard::isKeyPressed(Keyboard::Q))
        powerup();
    PlayerSoldier::attack();
}









// ============= MAHORAGA =============

void MahoragaAIState::movement(Mahoraga* m) {
    PlayerSoldier* p = m->getLevel()->getCurrentPlayer();;
    if (p == nullptr) return;

    m->updateTileChecks(m->getX(), m->getY());

    float dx = p->getX() - m->getX();
    float absDx = dx < 0 ? -dx : dx;
    float stopAt = 3.0f * cell_size;

    if (absDx > stopAt) {
        float dir = dx > 0 ? 1.0f : -1.0f;
        bool blocked = dir > 0 ? m->horizontalCollisionCheck(' ', m->getMidRight(), m->getBottomRight()) : m->horizontalCollisionCheck(' ', m->getMidLeft(), m->getBottomLeft());
        if (!blocked) {
            m->setVelocityX(dir * m->getMaxSpeed());
            m->setX(m->getX() + m->getVelocityX());
        }
        else {
            m->setVelocityY(-20);
        }
        m->setFacingRight(dir > 0);
    }
    else {
        m->setVelocityX(0);
    }

    m->entityGravity();
    m->clampToMap();
}

void MahoragaAIState::attack(Mahoraga* m) {
    if (m->getWeapon() == nullptr) return;
    EntityManager* em = m->getEntityManager();
    if (em == nullptr) return;

    MyVector<Entity*>& list = em->getEntities();
    Entity* nearest = nullptr;
    float bestDist = 0.0f;

    for (int i = 0; i < list.getSize(); i++) {
        Entity* e = list[i];
        if (e == nullptr) continue;
        if (!e->isHostileToPlayers()) continue;
        if (e->isOffScreen()) continue;

        float distance = m->calculateDistance(e);
        if (nearest == nullptr || distance < bestDist) {
            nearest = e;
            bestDist = distance;
        }
    }
    m->setNearestTarget(nearest);
    if (nearest == nullptr) return;
    m->getWeapon()->update();
    m->setFacingRight(nearest->getX() > m->getX());

    if (rand() % 2) {
        m->getWeapon()->fire();
    }
    else {
        m->getFireBomb()->fire();
    }

}

Mahoraga::Mahoraga(float x, float y, EntityManager* mgr, Level* lvl)
    : DamagableEntity(lvl), aiState(new MahoragaAIState()), weapon(nullptr),
    em(mgr), lifeTimer(30.0f), nearestTarget(nullptr)
{
    this->level = lvl;
    this->x = x;
    this->y = y;

    // averaged stats from Marco + Eri (100/100, both have same base speed)
    this->HP = 100;
    this->max_speed = 6.0f;
    this->acceleration = 0.5f;
    this->facingRight = true;

    Tex.loadFromFile("Sprites/fusion-companion.png");
    this->setScaleX(180.0f / 234.0f);
    this->setScaleY(256.0f / 288.0f);
    this->initializeScale();
    this->initializeDimensions();

    firebomb = new FireBombGrenade(this);
    weapon = new SlashGun(this);
    // union of fire-rate buffs: Marco +25% averaged with Eri -20% = +2.5%
    weapon->boostFireRate(1.025f);
    playMusic("Sounds/mahoraga.mp3");
}

void Mahoraga::attack() {
    aiState->attack(this);
}

float Mahoraga::getShootAngle() const {
    if (nearestTarget == nullptr) return facingRight ? 0.0f : 180.0f;
    float dx = (nearestTarget->getX() + nearestTarget->getWidth() / 2.0f) - (this->x + this->width / 2.0f);
    float dy = (nearestTarget->getY() + nearestTarget->getHeight() / 2.0f) - (this->y + this->height / 2.0f);
    float angle = atan2(dy, dx) * 180.0f / 3.14159265f;
    return angle;
}


void Mahoraga::behaviour() {
    lifeTimer -= deltaTime;
    if (lifeTimer <= 0.0f) {
        this->dead = true;
        return;
    }
    aiState->movement(this);
    attack();
    updateAnimations();
    setAndDraw();
}

void Mahoraga::updateAnimations() {
}

Mahoraga::~Mahoraga() {
    delete aiState;
    delete weapon;
    delete firebomb;
    firebomb = nullptr;
    aiState = nullptr;
    weapon = nullptr;
    playMusic("Sounds/level.mp3");
}