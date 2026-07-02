#include "project.h"
Vehicle::Vehicle(float x, float y,  Level* levelSkeleton): DamagableEntity(levelSkeleton), weapon(nullptr){
    this->x = x;
    this->y = y;
    this->level = levelSkeleton;
}

Weapon* Vehicle::getWeapon() {
    return this->weapon;
}

void Vehicle::writeStateTo(CurrentState& s) const {
    if (s.vehicleCount >= 10) return;
    VehicleState& v = s.vehicles[s.vehicleCount];
    v.id = entityId;
    v.x = getX();
    v.y = getY();
    v.HP = HP;
    v.occupiedByPlayerID = -1;     // skipped for v1
    v.dead = dead;
    s.vehicleCount++;
}

void Vehicle::applyStateFrom(const CurrentState& s) {
    for (int i = 0; i < s.vehicleCount; i++) {
        if (s.vehicles[i].id != entityId) continue;
        const VehicleState& v = s.vehicles[i];
        setX(v.x);
        setY(v.y);
        HP = v.HP;
        dead = v.dead;
        return;
    }
}

Vehicle::~Vehicle() { delete weapon; }

// ============= ENEMY VEHICLE =============

EnemyVehicle::EnemyVehicle(float x, float y, Level* level)
    : Vehicle(x, y,level), alerted(false), detectionRadius(500.0f), dying(false)
{
}

void EnemyVehicle::changeHP(int change)
{
    HP += change;
    if (HP <= 0 && !dying)
    {
        HP = 0;
        dying = true;
        //dead = true;
        // deactivate all animations
        for (int i = 0; i < animations.getSize(); i++)
            animations[i]->setActive(false);
        // last animation is always the destroy oneshot by convention
        animations[animations.getSize() - 1]->setActive(true);
        animations[animations.getSize() - 1]->reset();
    }
}


// ===== FLYING TARA =====

FlyingTara::FlyingTara(float x, float y,  Level* level): EnemyVehicle(x, y , level){
    this->HP = 5;
    this->detectionRadius = 500.0f;
    this->weapon = new HandGrenade(this);
    this->max_speed = 2.0f;
    this->acceleration = 0.3f;

    float charW = 128.0f;
    float charH = 128.0f;

    // animation[0] — hover loop
    LoopAnimation* hover = new LoopAnimation(6, 1.0f / 12.0f, true);
    hover->loadTexture("Sprites/flyingtara-fly.png"); 
    hover->setActive(true);
    IntRect hoverFrames[6] = {
        IntRect(18 , 449 , 240 , 125),
        IntRect(288 , 449 , 237 , 123),
        IntRect(557 , 452, 244, 123),
        IntRect(815 , 454, 225, 121),
        IntRect(1056 , 447, 217, 122),
        IntRect(1296 , 450, 223, 119),
    };
    hover->setCustomFrames(hoverFrames, 6);
    hover->setScale(charW / 237.0f, charH / 125.0f);     
    animations.add(hover);

    // animation[1] — destruction oneshot
    OneShotAnimation* destroy = new OneShotAnimation(6, 1.0f / 12.0f, false);
    destroy->loadTexture("Sprites/flyingtara-destroy.png"); 
    destroy->setScale(charW / 237.0f, charH / 125.0f);       
    destroy->setActive(false);
    animations.add(destroy);

    this->x = x;
    this->y = y;
    this->width = charW;
    this->height = charH;
}

void FlyingTara::movement()
{
    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr) 
        return;

    float targetY = player->getY() - 3.0f * cell_size;

    float dyErr = targetY - this->y;
    if (dyErr > 4.0f) 
        this->y += 4.0f;
    else if (dyErr < -4.0f) 
        this->y -= 4.0f;
    else                    
        this->y += dyErr;

    float dx = player->getX() - this->x;
    if (dx > max_speed) 
        this->x += max_speed;
    else if (dx < -max_speed)
        this->x -= max_speed;
    else                      
        this->x += dx;

    this->facingRight = (player->getX() > this->x);
    clampToMap();
}

void FlyingTara::attack()
{
    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr) return;

    // only fire when horizontally close enough
    float dx = player->getX() - this->x;
    if (dx < 0) dx = -dx;
    if (dx > 4.0f * cell_size) return;

    if (weapon != nullptr)
        weapon->fire();
}

void FlyingTara::updateAnimations()
{
    // during destruction — play oneshot then die
    if (animations[1]->isActive())
    {
        animations[1]->update();
        this->Tex = animations[1]->getTexture();
        this->sprite.setTexture(this->Tex);
        this->sprite.setTextureRect(animations[1]->getRect());
        float sx = animations[1]->getScaleX();
        float sy = animations[1]->getScaleY();
        this->sprite.setScale(facingRight ? -sx : sx, sy);

        if (animations[1]->getIsComplete())
            this->dead = true;

        return;
    }

    // hover loop
    animations[0]->update();
    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[0]->getRect());
    float sx = animations[0]->getScaleX();
    float sy = animations[0]->getScaleY();
    this->sprite.setScale(facingRight ? sx : -sx, sy);
}

void FlyingTara::behaviour()
{
    // if destroy anim is playing, freeze movement
    if (!animations[1]->isActive())
    {
        movement();
        attack();
    }

    if (weapon != nullptr)
        weapon->update();

    updateAnimations();
    setAndDraw();
}

FlyingTara::~FlyingTara() {
}

// ============= BRADLEY =============

Bradley::Bradley(float x, float y,  Level* level): EnemyVehicle(x, y, level)
{
    this->HP = 7;
    this->detectionRadius = 500.0f;
    this->weapon = new RocketLauncher(this);
    this->max_speed = 2.0f;
    this->acceleration = 0.3f;

    float charW = 192.0f;
    float charH = 128.0f;

    // animation[0] — idle
    LoopAnimation* idle = new LoopAnimation(1, 1.0f / 1.0f, false);
    idle->loadTexture("Sprites/bradley-idle.png");
    idle->setScale(charW / 332.f, charH / 311.0f);
    idle->setActive(true);
    animations.add(idle);

    // animation[1] — move
    LoopAnimation* move = new LoopAnimation(4, 1.0f / 6.0f, false);
    move->loadTexture("Sprites/bradley-move.png");
    move->setScale(charW / 240.f, charH / 233.0f);
    move->setActive(false);
    animations.add(move);

    // animation[2] — shoot oneshot
    OneShotAnimation* shoot = new OneShotAnimation(1, 1.0f / 1.0f, false);
    shoot->loadTexture("Sprites/bradley-idle.png");
    shoot->setScale(charW / 332.f, charH / 311.0f);
    shoot->setActive(false);
    animations.add(shoot);

    // animation[3] — destruction oneshot (must always be last)
    OneShotAnimation* destroy = new OneShotAnimation(4, 1.0f / 6.0f, false);
    destroy->loadTexture("Sprites/bradley-destroy.png");
    destroy->setScale(charW / 196.f, charH / 160.0f);
    destroy->setActive(false);
    animations.add(destroy);

    this->x = x;
    this->y = y;
    this->width = charW;
    this->height = charH;
}

Bradley::~Bradley() { }

void Bradley::movement()
{
    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr)
        return;

    float dist = this->calculateDistance(player);

    if (dist <= detectionRadius) 
        alerted = true;
    else                        
        alerted = false;

    if (!alerted)
    {
        velocityX = 0;
        entityGravity();
        return;
    }

    updateTileChecks(this->x, this->y);

    float dx = player->getX() - this->x;
    float absDx = dx < 0 ? -dx : dx;
    float stopRange = 5.0f * cell_size;

    if (absDx > stopRange)
    {
        if (dx > 0)
        {
            facingRight = true;
            if (!horizontalCollisionCheck(' ', midRight, bottomRight))
            {
                velocityX = max_speed;
                this->x += velocityX;
            }
            else
                velocityX = 0;
        }
        else
        {
            facingRight = false;
            if (!horizontalCollisionCheck(' ', midLeft, bottomLeft))
            {
                velocityX = -max_speed;
                this->x += velocityX;
            }
            else
                velocityX = 0;
        }
    }
    else
    {
        velocityX = 0;
        facingRight = (dx > 0);
    }

    entityGravity();
    clampToMap();
}

void Bradley::attack()
{
    if (!alerted) return;
    if (weapon == nullptr)
        return;

    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr) 
        return;

    float dx = player->getX() - this->x;
    float absDx = dx < 0 ? -dx : dx;
    float stopRange = 5.0f * cell_size;
    float maxRange = detectionRadius;

    if (absDx >= stopRange && absDx <= maxRange)
    {
        weapon->fire();

        if (!animations[2]->isActive())
        {
            animations[2]->setActive(true);
            animations[2]->reset();
        }
    }
}

void Bradley::updateAnimations()
{
    // destruction takes full priority
    if (animations[3]->isActive())
    {
        animations[3]->update();
        this->Tex = animations[3]->getTexture();
        this->sprite.setTexture(this->Tex);
        this->sprite.setTextureRect(animations[3]->getRect());
        float sx = animations[3]->getScaleX();
        float sy = animations[3]->getScaleY();
        this->sprite.setScale(facingRight ? sx : -sx, sy);

        if (animations[3]->getIsComplete())
            this->dead = true;

        return;
    }

    // shoot oneshot
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

    // move vs idle
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

void Bradley::behaviour()
{
    if (!dying)
    {
        movement();
        attack();
    }

    if (weapon != nullptr)
        weapon->update();

    updateAnimations();
    setAndDraw();
}



// ============= ENEMY SUB =============

EnemySub::EnemySub(float x, float y, Level* level) : EnemyVehicle(x, y, level)
{
    this->HP = 7;
    this->detectionRadius = 600.0f;
    this->weapon = new RocketLauncher(this);
    this->max_speed = 1.5f;
    float charW = 192.0f;
    float charH = 128.0f;
    this->x = x;
    this->y = y;
    this->width = charW;
    this->height = charH;

    // animation[0] — hover/idle loop
    LoopAnimation* idle = new LoopAnimation(4, 1.0f / 6.0f, false);
    idle->loadTexture("Sprites/enemysub-move.png"); 
    idle->setScale(charW / 168.f,charH / 71.f);
    idle->setActive(true);
    animations.add(idle);

    // animation[1] — destruction oneshot 
    OneShotAnimation* destroy = new OneShotAnimation(4, 1.0f /6.0f, true);
    destroy->loadTexture("Sprites/enemysub-die.png"); 
    destroy->setActive(false);
    IntRect dieFrames[4] = {
        IntRect(0 , 42 , 171 , 85),
        IntRect(204 , 42 , 163 , 96),
        IntRect(381 , 12, 177 , 164),
        IntRect(575 , 21, 195 , 174)
    };
    destroy->setCustomFrames(dieFrames, 4);
    destroy->setScale(charW / 195.f , charH/ 164.f);
    animations.add(destroy);

    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[0]->getRect());
    this->sprite.setScale(animations[0]->getScaleX(), animations[0]->getScaleY());

}

void EnemySub::movement()
{
    PlayerSoldier* player = level->getCurrentPlayer();
    if (player == nullptr) return;

    float dist = this->calculateDistance(player);
    alerted = (dist <= detectionRadius);

    if (!alerted)
    {
        velocityX = 0;
        velocityY = 0;
        return;
    }

    float dx = player->getX() - this->x;
    float dy = player->getY() - this->y;
    facingRight = (dx > 0);

    float vx = (dx > 0) ? max_speed : -max_speed;
    float vy = (dy > 0) ? max_speed : -max_speed;

    updateTileChecks(this->x + vx, this->y + vy);
    bool submerged = (getTopMid() == '~' && getMidLeft() == '~' && getMidRight() == '~' && getBottomMid() == '~');

    bool atBottomBoundary = getBottomMid() == '~' && vy > 0;
    bool atTopBoundary = getTopMid() == '~' && vy < 0;
    if (submerged)
    {
        this->x += vx;
        if (!atTopBoundary && !atBottomBoundary) {
            this->y += vy;
        }
        velocityX = vx;
        velocityY = vy;
    }
    else
    {
        velocityX = 0;
        velocityY = 5;
        this->x += velocityX;
        this->y += velocityY;
    }
    clampToMap();
}

void EnemySub::attack()
{
    if (!alerted) return;
    if (weapon == nullptr) return;
    weapon->fire();
}

void EnemySub::updateAnimations()
{
    // destruction takes priority
    if (animations[1]->isActive())
    {
        animations[1]->update();
        this->Tex = animations[1]->getTexture();
        this->sprite.setTexture(this->Tex);
        this->sprite.setTextureRect(animations[1]->getRect());
        float sx = animations[1]->getScaleX();
        float sy = animations[1]->getScaleY();
        this->sprite.setScale(facingRight ? sx : -sx, sy);

        if (animations[1]->getIsComplete())
            this->dead = true;

        return;
    }

    // idle loop
    animations[0]->update();
    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[0]->getRect());
    float sx = animations[0]->getScaleX();
    float sy = animations[0]->getScaleY();
    this->sprite.setScale(facingRight ? sx : -sx, sy);
}

void EnemySub::behaviour()
{
    if (!dying)
    {
        movement();
        attack();
    }

    if (weapon != nullptr)
        weapon->update();

    updateAnimations();
    setAndDraw();
}

EnemySub::~EnemySub()
{
}


// player vehicles=============================
float PlayerVehicle::getShootAngle() const {
    float mouseWorldX = (float)Mouse::getPosition(*window).x + level->getScreenLeft();
    float mouseWorldY = (float)Mouse::getPosition(*window).y + level->getScreenTop();
    float dx = mouseWorldX - (this->x + this->width / 2.0f);
    float dy = mouseWorldY - (this->y + this->height / 2.0f);
    return atan2(dy, dx) * 180.0f / 3.14159265f;
}

// ============= PLAYER VEHICLE =============

PlayerVehicle::PlayerVehicle(float x, float y, Level* level) : Vehicle(x, y, level) {
    this->occupant = nullptr;
    this->isOccupied = false;
    this->currentWeapon = 0;
    this->xWasPressed = false;
}

PlayerVehicle::~PlayerVehicle() {
    for (int i = 0; i < weapons.getSize(); i++) {
        if (weapons[i] != nullptr) {
            delete weapons[i];
            weapons[i] = nullptr;
        }
    }
}

void PlayerVehicle::enter(PlayerSoldier* player) {
    if (player == nullptr) return;
    this->occupant = player;
    this->isOccupied = true;
    player->setVehicle(this);
}

void PlayerVehicle::exit() {
    if (occupant == nullptr) return;
    occupant->setX(this->x);
    occupant->setY(this->y - this->height);
    occupant->setVelocityY(-12.0f);
    occupant->setVehicle(nullptr);
    occupant = nullptr;
    isOccupied = false;
}

bool PlayerVehicle::getIsOccupied() const {
    return isOccupied;
}

PlayerSoldier* PlayerVehicle::getOccupant() const {
    return occupant;
}

Weapon* PlayerVehicle::getWeapon() {
    if (currentWeapon < 0 || currentWeapon >= weapons.getSize()) return nullptr;
    return weapons[currentWeapon];
}

void PlayerVehicle::changeHP(int change) {
    HP += change;
    if (HP <= 0 && !dead) {
        HP = 0;
        if (isOccupied) exit();
        dead = true;
    }
}


// ============= METAL SLUG =============
MetalSlug::MetalSlug(float x, float y, Level* level) : GroundVehicle(x, y, level) {
    this->HP = 100;
    this->max_speed = 6.0f;
    this->acceleration = 0.4f;

    float charW = 192.0f;
    float charH = 128.0f;
    this->x = x;
    this->y = y;
    this->width = charW;
    this->height = charH;

    // animation[0] — idle
    LoopAnimation* idle = new LoopAnimation(1, 1.0f / 1.0f, false);
    idle->loadTexture("Sprites/metalslug-idle.png");
    idle->setScale(charW / 254.f,charH / 238.f);
    idle->setActive(true);
    animations.add(idle);

    // animation[1] — move
    LoopAnimation* move = new LoopAnimation(5, 1.0f / 8.0f, true);
    move->loadTexture("Sprites/metalslug-move.png");
    move->setActive(false);
    IntRect moveFrames[5] = {
        IntRect(0,5,117,112),
        IntRect(123,7,120,110),
        IntRect(249,9,122,108),
        IntRect(377,7,122,110),
        IntRect(505,5,118,112),
    };
    move->setCustomFrames(moveFrames, 5);
    move->setScale(charW / 122.f,charH / 112.f);
    animations.add(move);

    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
    this->initializeScale();
    this->initializeDimensions();

    mode = new GroundForm(this);
}


MetalSlug::~MetalSlug() {
    delete mode;
}

void MetalSlug::movement() {
    mode->movement(this);
}

void MetalSlug::attack() {
    mode->attack(this);
}

void MetalSlug::updateAnimations() {
    if (animations.getSize() < 2) return;

    bool moving = (velocityX != 0);

    if (moving) {
        animations[0]->setActive(false);
        animations[0]->reset();
        animations[1]->setActive(true);
    }
    else {
        animations[1]->setActive(false);
        animations[1]->reset();
        animations[0]->setActive(true);
    }

    int i = moving ? 1 : 0;
    animations[i]->update();
    
    this->Tex = animations[i]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[i]->getRect());
    this->sprite.setScale(facingRight ? animations[i]->getScaleX() : -animations[i]->getScaleX(),
        animations[i]->getScaleY());
}

Weapon* MetalSlug::getWeapon() {
    return mode->getCurrentWeapon();
}

void MetalSlug::behaviour() {
    if (!isOccupied) {
        PlayerSoldier* p = level->getCurrentPlayer();
        bool xPressed = Keyboard::isKeyPressed(Keyboard::X);
        if (p != nullptr && p->getVehicle() == nullptr && checkCollision(p)) {
            if (xPressed && !xWasPressed) {
                enter(p);
                xWasPressed = xPressed;
                updateAnimations();
                setAndDraw();
                return;
            }
        }
        xWasPressed = xPressed;

        movement();
        updateAnimations();
        setAndDraw();
        return;
    }

    bool xPressed = Keyboard::isKeyPressed(Keyboard::X);
    if (xPressed && !xWasPressed) {
        exit();
        xWasPressed = xPressed;
        updateAnimations();
        setAndDraw();
        return;
    }
    xWasPressed = xPressed;

    movement();
    attack();
    updateAnimations();
    setAndDraw();
}

// ============= SLUG FLYER =============

SlugFlyer::SlugFlyer(float x, float y, Level* level) : AerialVehicle(x, y, level) {
    this->HP = 100;
    this->max_speed = 5.0f;
    this->acceleration = 0.4f;

    float charW = 192.0f;
    float charH = 128.0f;
    this->x = x;
    this->y = y;
    this->width = charW;
    this->height = charH;

    LoopAnimation* idle = new LoopAnimation(1, 1.0f / 1.0f, false);
    idle->loadTexture("Sprites/slugflyer-idle.png");
    idle->setScale(charW / 246.f ,charH / 140.f);
    idle->setActive(true);
    animations.add(idle);

    LoopAnimation* move = new LoopAnimation(5, 1.0f / 8.0f, true);
    move->loadTexture("Sprites/slugflyer-move.png");
    move->setActive(false);
    IntRect moveFrames[5] = {
        IntRect(0,7,155,92),
        IntRect(161,5,160,96),
        IntRect(327,3,160,100),
        IntRect(493,1,160,104),
        IntRect(659,1,160,104),

    };
    move->setCustomFrames(moveFrames, 5);
    move->setScale(charW / 160.f,charH /104.f);
    animations.add(move);

    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
    this->initializeScale();
    this->initializeDimensions();

    mode = new AerialForm(this);
}

SlugFlyer::~SlugFlyer() {
    delete mode;
}

void SlugFlyer::movement() {
    mode->movement(this);
}

void SlugFlyer::attack() {
    mode->attack(this);
}

void SlugFlyer::updateAnimations() {
    if (animations.getSize() < 2) return;

    bool moving = (velocityX != 0 || velocityY != 0);

    if (moving) {
        animations[0]->setActive(false);
        animations[0]->reset();
        animations[1]->setActive(true);
    }
    else {
        animations[1]->setActive(false);
        animations[1]->reset();
        animations[0]->setActive(true);
    }

    int i = moving ? 1 : 0;
    animations[i]->update();
    this->Tex = animations[i]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[i]->getRect());
    this->sprite.setScale(facingRight ? animations[i]->getScaleX() : -animations[i]->getScaleX(),
        animations[i]->getScaleY());
}
Weapon* SlugFlyer::getWeapon() {
    return mode->getCurrentWeapon();
}

void SlugFlyer::behaviour() {
    if (!isOccupied) {
        PlayerSoldier* p = level->getCurrentPlayer();
        bool xPressed = Keyboard::isKeyPressed(Keyboard::X);
        if (p != nullptr && p->getVehicle() == nullptr && checkCollision(p)) {
            if (xPressed && !xWasPressed) {
                enter(p);
                xWasPressed = xPressed;
                setAndDraw();
                return;
            }
        }
        xWasPressed = xPressed;
        movement();
        setAndDraw();
        return;
    }

    bool xPressed = Keyboard::isKeyPressed(Keyboard::X);
    if (xPressed && !xWasPressed) {
        exit();
        xWasPressed = xPressed;
        setAndDraw();
        return;
    }
    xWasPressed = xPressed;

    movement();
    attack();
    updateAnimations();
    setAndDraw();
}


// ============= SLUG MARINER =============

SlugMariner::SlugMariner(float x, float y, Level* level) : AquaticVehicle(x, y, level) {
    this->HP = 100;
    this->max_speed = 4.0f;
    this->acceleration = 0.3f;

    float charW = 192.0f;
    float charH = 128.0f;
    this->x = x;
    this->y = y;
    this->width = charW;
    this->height = charH;

    LoopAnimation* idle = new LoopAnimation(1, 1.0f / 1.0f, false);
    idle->loadTexture("Sprites/slugmariner-idle.png");
    idle->setScale(charW / 469.f, charH / 430.f);
    idle->setActive(true);
    animations.add(idle);

    LoopAnimation* move = new LoopAnimation(5, 1.0f / 8.0f, true);
    move->loadTexture("Sprites/slugmariner-move.png");
    move->setActive(false);
    IntRect moveFrames[5] = {
        IntRect(0,4,166,153),
        IntRect(175,1,171,156),
        IntRect(355,1,174,156),
        IntRect(538,1,174,159),
        IntRect(721,1,174,159),
    };
    move->setCustomFrames(moveFrames, 5);
    move->setScale(charW / 174.f,charH / 159.f);
    animations.add(move);

    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
    this->initializeScale();
    this->initializeDimensions();

    mode = new AquaticForm(this);
}


SlugMariner::~SlugMariner() {
    delete mode;
}

void SlugMariner::movement() {
    mode->movement(this);
}

void SlugMariner::attack() {
    mode->attack(this);
}

void SlugMariner::updateAnimations() {
    if (animations.getSize() < 2) return;

    bool moving = (velocityX != 0 || velocityY != 0);

    if (moving) {
        animations[0]->setActive(false);
        animations[0]->reset();
        animations[1]->setActive(true);
    }
    else {
        animations[1]->setActive(false);
        animations[1]->reset();
        animations[0]->setActive(true);
    }

    int i = moving ? 1 : 0;
    animations[i]->update();
    this->Tex = animations[i]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[i]->getRect());
    this->sprite.setScale(facingRight ? animations[i]->getScaleX() : -animations[i]->getScaleX(),
        animations[i]->getScaleY());
}

Weapon* SlugMariner::getWeapon() {
    return mode->getCurrentWeapon();
}

void SlugMariner::behaviour() {
    if (!isOccupied) {
        PlayerSoldier* p = level->getCurrentPlayer();
        bool xPressed = Keyboard::isKeyPressed(Keyboard::X);
        if (p != nullptr && p->getVehicle() == nullptr && checkCollision(p)) {
            if (xPressed && !xWasPressed) {
                enter(p);
                xWasPressed = xPressed;
                setAndDraw();
                return;
            }
        }
        xWasPressed = xPressed;
        movement();
        setAndDraw();
        return;
    }

    bool xPressed = Keyboard::isKeyPressed(Keyboard::X);
    if (xPressed && !xWasPressed) {
        exit();
        xWasPressed = xPressed;
        setAndDraw();
        return;
    }
    xWasPressed = xPressed;

    movement();
    attack();
    updateAnimations();
    setAndDraw();
}


// ============= GROUND FORM =============

GroundForm::GroundForm(PlayerVehicle* host) {
     weapons.add(new HeavyMachineGun(host));
    currentWeapon = 0;
}


GroundForm::~GroundForm() {
    for (int i = 0; i < weapons.getSize(); i++) {
        if (weapons[i] != nullptr) delete weapons[i];
    }
}

void GroundForm::movement(PlayerVehicle* host) {
    if (!host->getIsOccupied()) {
        host->setVelocityX(0);
        host->entityGravity();
        return;
    }

    host->updateTileChecks(host->getX(), host->getY());

    bool blockedRight = host->horizontalCollisionCheck(' ', host->getMidRight(), host->getBottomRight());
    bool blockedLeft = host->horizontalCollisionCheck(' ', host->getMidLeft(), host->getBottomLeft());

    bool dHeld = Keyboard::isKeyPressed(Keyboard::D);
    bool aHeld = Keyboard::isKeyPressed(Keyboard::A);

    float vx = host->getVelocityX();
    float accel = host->getAcceleration();
    float maxSpd = host->getMaxSpeed();

    if (dHeld && !blockedRight) {
        vx += accel;
        if (vx > maxSpd) vx = maxSpd;
        host->setFacingRight(true);
    }
    else if (aHeld && !blockedLeft) {
        vx -= accel;
        if (vx < -maxSpd) vx = -maxSpd;
        host->setFacingRight(false);
    }
    else {
        vx = 0;
    }
    host->setVelocityX(vx);

    bool walkingIntoWall = (dHeld && blockedRight) || (aHeld && blockedLeft);
    if (walkingIntoWall && host->OnGround()) {
        host->setVelocityY(-16.0f);
    }

    if (Keyboard::isKeyPressed(Keyboard::W) && host->OnGround()) {
        host->setVelocityY(-16.0f);
    }

    host->entityGravity();
    host->setX(host->getX() + vx);
    host->clampToMap();
}

void GroundForm::attack(PlayerVehicle* host) {
    if (!host->getIsOccupied()) return;

    if (Keyboard::isKeyPressed(Keyboard::Num1) && 0 < weapons.getSize()) currentWeapon = 0;
    if (Keyboard::isKeyPressed(Keyboard::Num2) && 1 < weapons.getSize()) currentWeapon = 1;
    if (Keyboard::isKeyPressed(Keyboard::Num3) && 2 < weapons.getSize()) currentWeapon = 2;

    if (Mouse::isButtonPressed(Mouse::Left)) {
        if (weapons[currentWeapon] != nullptr) weapons[currentWeapon]->fire();
    }

    for (int i = 0; i < weapons.getSize(); i++) {
        if (weapons[i] != nullptr) weapons[i]->update();
    }
}

void GroundForm::updateAnimations(PlayerVehicle* host) {
}

void GroundForm::applySprite(PlayerVehicle* host) {
}

Weapon* GroundForm::getCurrentWeapon() {
    if (currentWeapon < 0 || currentWeapon >= weapons.getSize()) return nullptr;
    return weapons[currentWeapon];
}

void GroundForm::swapWeapon(int slot) {
    if (slot >= 0 && slot < weapons.getSize()) currentWeapon = slot;
}


// ============= AERIAL FORM =============

AerialForm::AerialForm(PlayerVehicle* host) {
    weapons.add(new RocketLauncher(host));
    weapons.add(new Pistol(host));
    currentWeapon = 0;
}

AerialForm::~AerialForm() {
    for (int i = 0; i < weapons.getSize(); i++) {
        if (weapons[i] != nullptr) delete weapons[i];
    }
}

void AerialForm::movement(PlayerVehicle* host) {
    if (!host->getIsOccupied()) {
        host->setVelocityX(0);
        host->entityGravity();
        return;
    }

    host->updateTileChecks(host->getX(), host->getY());

    bool blockedRight = host->horizontalCollisionCheck(' ', host->getMidRight(), host->getBottomRight());
    bool blockedLeft = host->horizontalCollisionCheck(' ', host->getMidLeft(), host->getBottomLeft());
    bool blockedUp = (host->getTopMid() == '#' || host->getTopMid() == 'D' || host->getTopMid() == '~');
    bool blockedDown = (host->getBottomMid() == '#' || host->getBottomMid() == 'D' || host->getBottomMid() == '~');

    bool dHeld = Keyboard::isKeyPressed(Keyboard::D);
    bool aHeld = Keyboard::isKeyPressed(Keyboard::A);
    bool wHeld = Keyboard::isKeyPressed(Keyboard::W);
    bool sHeld = Keyboard::isKeyPressed(Keyboard::S);

    float vx = host->getVelocityX();
    float vy = host->getVelocityY();
    float accel = host->getAcceleration();
    float maxSpd = host->getMaxSpeed();

    if (dHeld && !blockedRight) {
        vx += accel;
        if (vx > maxSpd) vx = maxSpd;
        host->setFacingRight(true);
    }
    else if (aHeld && !blockedLeft) {
        vx -= accel;
        if (vx < -maxSpd) vx = -maxSpd;
        host->setFacingRight(false);
    }
    else {
        vx = 0;
    }

    if (wHeld && !blockedUp) {
        vy -= accel;
        if (vy < -maxSpd) vy = -maxSpd;
    }
    else if (sHeld && !blockedDown) {
        vy += accel;
        if (vy > maxSpd) vy = maxSpd;
    }
    else {
        vy = 0;
    }

    host->setVelocityX(vx);
    host->setVelocityY(vy);
    host->setX(host->getX() + vx);
    host->setY(host->getY() + vy);
    host->clampToMap();
}

void AerialForm::attack(PlayerVehicle* host) {
    if (!host->getIsOccupied()) return;

    if (Keyboard::isKeyPressed(Keyboard::Num1) && 0 < weapons.getSize()) currentWeapon = 0;
    if (Keyboard::isKeyPressed(Keyboard::Num2) && 1 < weapons.getSize()) currentWeapon = 1;

    if (Mouse::isButtonPressed(Mouse::Left)) {
        if (weapons[currentWeapon] != nullptr) weapons[currentWeapon]->fire();
    }

    for (int i = 0; i < weapons.getSize(); i++) {
        if (weapons[i] != nullptr) weapons[i]->update();
    }
}

void AerialForm::updateAnimations(PlayerVehicle* host) {
}

void AerialForm::applySprite(PlayerVehicle* host) {
}

Weapon* AerialForm::getCurrentWeapon() {
    if (currentWeapon < 0 || currentWeapon >= weapons.getSize()) return nullptr;
    return weapons[currentWeapon];
}

void AerialForm::swapWeapon(int slot) {
    if (slot >= 0 && slot < weapons.getSize()) currentWeapon = slot;
}
// ============= AQUATIC FORM =============

AquaticForm::AquaticForm(PlayerVehicle* host) {
    weapons.add(new RocketLauncher(host));
    currentWeapon = 0;
}

AquaticForm::~AquaticForm() {
    for (int i = 0; i < weapons.getSize(); i++) {
        if (weapons[i] != nullptr) delete weapons[i];
    }
}

void AquaticForm::movement(PlayerVehicle* host) {
    if (!host->getIsOccupied()) {
        host->setVelocityX(0);
        host->setVelocityY(0);
        return;
    }

    bool dHeld = Keyboard::isKeyPressed(Keyboard::D);
    bool aHeld = Keyboard::isKeyPressed(Keyboard::A);
    bool wHeld = Keyboard::isKeyPressed(Keyboard::W);
    bool sHeld = Keyboard::isKeyPressed(Keyboard::S);

    float maxSpd = host->getMaxSpeed();
    float vx = 0;
    float vy = 0;

    if (dHeld) {
        vx = maxSpd;
        host->setFacingRight(true);
    }
    else if (aHeld) {
        vx = -maxSpd;
        host->setFacingRight(false);
    }

    if (wHeld) vy = -maxSpd;
    else if (sHeld) vy = maxSpd;

    host->updateTileChecks(host->getX() + vx, host->getY() + vy);
    bool stillSubmerged = (host->getTopMid() == '~' && host->getMidLeft() == '~' && host->getMidRight() == '~' && host->getBottomMid() == '~');

    host->setX(host->getX() + vx);
    host->setY(host->getY() + vy);
    host->setVelocityX(vx);
    host->setVelocityY(vy);
    host->clampToMap();
}

void AquaticForm::attack(PlayerVehicle* host) {
    if (!host->getIsOccupied()) return;

    if (Keyboard::isKeyPressed(Keyboard::Num1) && 0 < weapons.getSize()) currentWeapon = 0;

    if (Mouse::isButtonPressed(Mouse::Left)) {
        if (weapons[currentWeapon] != nullptr) weapons[currentWeapon]->fire();
    }

    for (int i = 0; i < weapons.getSize(); i++) {
        if (weapons[i] != nullptr) weapons[i]->update();
    }
}

void AquaticForm::updateAnimations(PlayerVehicle* host) {
}

void AquaticForm::applySprite(PlayerVehicle* host) {
}

Weapon* AquaticForm::getCurrentWeapon() {
    if (currentWeapon < 0 || currentWeapon >= weapons.getSize()) return nullptr;
    return weapons[currentWeapon];
}

void AquaticForm::swapWeapon(int slot) {
    if (slot >= 0 && slot < weapons.getSize()) currentWeapon = slot;
}
// ============= AMPHIBIOUS SLUG =============

AmphibiousSlug::AmphibiousSlug(float x, float y, Level* level) : PlayerVehicle(x, y, level) {
    this->HP = 100;
    this->max_speed = 5.0f;
    this->acceleration = 0.4f;
    float charW = 192.0f;
    float charH = 128.0f;
    this->x = x;
    this->y = y;
    this->width = charW;
    this->height = charH;


    // ground: [0] idle, [1] move
    LoopAnimation* gIdle = new LoopAnimation(1, 1.0f / 1.0f, false);
    gIdle->loadTexture("Sprites/metalslug-idle.png");
    gIdle->setScale(charW / 254.f, charH / 238.f);
    gIdle->setActive(true);
    animations.add(gIdle);

    LoopAnimation* gMove = new LoopAnimation(5, 1.0f / 8.0f, true);
    gMove->loadTexture("Sprites/metalslug-move.png");
    gMove->setActive(false);
    IntRect gMoveFrames[5] = {
        IntRect(0,5,117,112),
        IntRect(123,7,120,110),
        IntRect(249,9,122,108),
        IntRect(377,7,122,110),
        IntRect(505,5,118,112),
    };
    gMove->setCustomFrames(gMoveFrames, 5);
    gMove->setScale(charW / 122.f, charH / 112.f);
    animations.add(gMove);

    // aerial: [2] idle, [3] move
    LoopAnimation* aIdle = new LoopAnimation(1, 1.0f / 1.0f, false);
    aIdle->loadTexture("Sprites/slugflyer-idle.png");
    aIdle->setScale(charW / 246.f, charH / 140.f);
    aIdle->setActive(false);
    animations.add(aIdle);

    LoopAnimation* aMove = new LoopAnimation(5, 1.0f / 8.0f, true);
    aMove->loadTexture("Sprites/slugflyer-move.png");
    aMove->setActive(false);
    IntRect aMoveFrames[5] = {
        IntRect(0,7,155,92),
        IntRect(161,5,160,96),
        IntRect(327,3,160,100),
        IntRect(493,1,160,104),
        IntRect(659,1,160,104),
    };
    aMove->setCustomFrames(aMoveFrames, 5);
    aMove->setScale(charW / 160.f, charH / 104.f);
    animations.add(aMove);

    // aquatic: [4] idle, [5] move
    LoopAnimation* qIdle = new LoopAnimation(5, 1.0f / 5.0f, true);
    qIdle->loadTexture("Sprites/slugmariner-move.png");
    qIdle->setActive(false);
    IntRect qIdleFrames[5] = {
        IntRect(0,4,166,153),
        IntRect(175,1,171,156),
        IntRect(355,1,174,156),
        IntRect(538,1,174,159),
        IntRect(721,1,174,159),
    };
    qIdle->setCustomFrames(qIdleFrames, 5);
    qIdle->setScale(charW / 174.f, charH / 159.f);
    animations.add(qIdle);

    LoopAnimation* qMove = new LoopAnimation(5, 1.0f / 8.0f, true);
    qMove->loadTexture("Sprites/slugmariner-move.png");
    qMove->setActive(false);
    IntRect qMoveFrames[5] = {
        IntRect(0,4,166,153),
        IntRect(175,1,171,156),
        IntRect(355,1,174,156),
        IntRect(538,1,174,159),
        IntRect(721,1,174,159),
    };
    qMove->setCustomFrames(qMoveFrames, 5);
    qMove->setScale(charW / 174.f, charH / 159.f);
    animations.add(qMove);

    // sync initial sprite to ground idle
    this->Tex = animations[0]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->setScale(animations[0]->getScaleX(), animations[0]->getScaleY());
    this->initializeScale();
    this->initializeDimensions();

    modes[0] = new GroundForm(this);
    modes[1] = new AerialForm(this);
    modes[2] = new AquaticForm(this);
    currentForm = 0;
}

AmphibiousSlug::~AmphibiousSlug() {
    for (int i = 0; i < 3; i++) {
        if (modes[i] != nullptr) delete modes[i];
    }
}

void AmphibiousSlug::updateForm() {
    updateTileChecks(this->x, this->y);
    bool inWater = (bottomLeft == '~' || bottomRight == '~' || topLeft == '~' || topRight == '~');

    char oneBelow = level->tileAtWorld(this->x + this->width / 2.0f, this->y + this->height + 1 * cell_size);
    char twoBelow = level->tileAtWorld(this->x + this->width / 2.0f, this->y + this->height + 2 * cell_size);
    bool oneBlockAir = !OnGround() && (twoBelow != '#') && (twoBelow != 'D') && (oneBelow != '#') && (oneBelow != 'D');

    int newForm = currentForm;
    if (inWater) newForm = 2;
    else if (oneBlockAir) newForm = 1;
    else if (oneBelow == '#' || oneBelow == 'D') newForm = 0;

    if (newForm != currentForm) {
        currentForm = newForm;
        modes[currentForm]->applySprite(this);
    }
}

void AmphibiousSlug::movement() {
    modes[currentForm]->movement(this);
}

void AmphibiousSlug::attack() {
    modes[currentForm]->attack(this);
}

void AmphibiousSlug::updateAnimations() {
    if (animations.getSize() < 6) return;

    // base index for current form: ground=0, aerial=2, aquatic=4
    int base = currentForm * 2;
    bool moving = (velocityX != 0 || velocityY != 0);
    int activeIndex = base + (moving ? 1 : 0);
    int inactiveIndex = base + (moving ? 0 : 1);

    // only deactivate/reset the one we're switching away from
    if (animations[inactiveIndex]->isActive()) {
        animations[inactiveIndex]->setActive(false);
        animations[inactiveIndex]->reset();
    }

    // activate the target if not already active
    if (!animations[activeIndex]->isActive()) {
        animations[activeIndex]->setActive(true);
    }

    animations[activeIndex]->update();

    this->Tex = animations[activeIndex]->getTexture();
    this->sprite.setTexture(this->Tex);
    this->sprite.setTextureRect(animations[activeIndex]->getRect());
    this->sprite.setScale(
        facingRight ? animations[activeIndex]->getScaleX() : -animations[activeIndex]->getScaleX(),
        animations[activeIndex]->getScaleY()
    );
}

Weapon* AmphibiousSlug::getWeapon() {
    return modes[currentForm]->getCurrentWeapon();
}

void AmphibiousSlug::behaviour() {
    if (!isOccupied) {
        PlayerSoldier* p = level->getCurrentPlayer();
        bool xPressed = Keyboard::isKeyPressed(Keyboard::X);
        if (p != nullptr && p->getVehicle() == nullptr && checkCollision(p)) {
            if (xPressed && !xWasPressed) {
                enter(p);
                xWasPressed = xPressed;
                setAndDraw();
                return;
            }
        }
        xWasPressed = xPressed;

        // idle: just gravity if grounded
        entityGravity();
        setAndDraw();
        return;
    }

    bool xPressed = Keyboard::isKeyPressed(Keyboard::X);
    if (xPressed && !xWasPressed) {
        exit();
        xWasPressed = xPressed;
        setAndDraw();
        return;
    }
    xWasPressed = xPressed;

    updateForm();
    movement();
    attack();
    updateAnimations();
    setAndDraw();
}
