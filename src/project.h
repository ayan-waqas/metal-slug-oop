#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <ctime>
using namespace sf;
using namespace std;

extern int screen_x;
extern int screen_y;
extern int cell_size;
extern int height;
extern int width;
extern float gravity;
extern RenderWindow* window;
extern float terminal_Velocity;
extern Font hudFont;
extern Font scoreFont;
extern float deltaTime;

//sound system added after submission
extern sf::Music bgMusic;
extern sf::SoundBuffer sfxBuffers[20];
extern sf::Sound sfxSounds[20];

enum SFX {
    SFX_SHOOT = 0,
    SFX_EXPLOSION,
    SFX_PICKUP,
    SFX_PLAYER_HIT,
    SFX_ENEMY_DIE,
    SFX_KNIFE,
    SFX_RELOAD,
    SFX_GRENADE_BOUNCE,
    SFX_MENU_SELECT,
    SFX_MENU_MOVE,
    SFX_FIRE,
    SFX_SLASH
};

void playSFX(int id);       // declare here, define in Engines.cpp
void playMusic(const char* path, bool loop = true);


class SfmlObject;
class Entity;
class Block;
class DamagableEntity;
class Projectile;
class StraightProjectile;
class BallisticProjectile;
class TransformationState;
class Soldier;
class PlayerSoldier;
struct Inventory;
class Marco;
class Tarma;
class Eri;
class Germi;
class Mahoraga;
class MahoragaAIState;
class EnemyAIState;
class RebelAIState;
class ShieldedAIState;
class BazookaAIState;
class GrenadeAIState;
class ParatrooperAIState;
class MummyAIState;
class ZombieAIState;
class PodAIState;
class MartianGroundedAIState;
class GrudgefulAIState;
class Enemy;
class Batch;
class RebelSoldier;
class ShieldedSoldier;
class BazookaSoldier;
class GrenadeSoldier;
class ParaTrooper;
class MummyWarrior;
class Zombie;
class Martian;
class Vehicle;
class EnemyVehicle;
class PlayerVehicle;
class GroundVehicle;
class AerialVehicle;
class AquaticVehicle;
class FlyingTara;
class Bradley;
class EnemySub;
class MetalSlug;
class SlugFlyer;
class SlugMariner;
class AmphibiousSlug;
class VehicleMode;
class GroundForm;
class AerialForm;
class AquaticForm;
class Weapon;
class ProjectileWeapon;
class Knife;
class Bullet;
class Pistol;
class Slash;
class SlashGun;
class HeavyMachineGun;
class Explosion;
class RocketLauncher;
class FireEffect;
class FlameShot;
class LaserGun;
class PodLaserGun;
class HandGrenade;
class FireBombGrenade;
class Collectible;
class InteractableObject;
class Food;
class Prisonor;
class SupplyBox;
class Boss;
class IronNokana;
class HairBuster;
class SeaSatan;
class UltimateBoss;
class BossMode;
class GroundMode;
class AerialMode;
class AquaticMode;
class Map;
class Level;
class SurvivalLevel;
class InfiniteLevel;
class HashValue;
class PerlinGenerator;
class FractalNoiseGenerator;
class NoiseProfileFactory;
class NoiseProfile;
class AmplifiedProfile;
class FlatProfile;
class NormalProfile;
class NodeGene;
class ConnectionBase;
class ConnectionGene;
class Genome;
class InnovationRegistry;
class Species;
class Population;
class NeuralNetwork;
class NEATtrackerHelper;
class NEATController;
class FileHandler;
class ProfileFileHandler;
class RageBaitFileHandler;
class ForksOfFateFileHandler;
class Parameter;
class Profile;
class Recorder;
class GhostController;
class Autopilot;
class SessionManager;
struct NearbyEnemyInfo;
class DeathFingerPrint;
class DeathCompositor;
struct WavHeader;
class Whisper;
class Complex;
class Spectrum;
class Filter;
class SampleAudio;
class AudioChecker;
class EchoChamber;
class AudioManager;
class RageBait;
struct States;
struct WeaponState;
struct InventoryState;
struct PlayerState;
struct EnemyState;
struct BossState;
struct VehicleState;
struct BulletState;
struct FireEffectState;
struct ExplosionState;
struct collectibleState;
struct BlockState;
struct ChunkState;
struct CurrentState;
class GrudgefulSystem;
class CowardiceSystem;
class StatesBuffer;
class ForksOfFateManager;
class PlayerManager;
class EnemyManager;
class BossManager;
class VehicleManager;
class WeaponManager;
class CollectibleManager;
class FireEffectManager;
class ExplosionManager;
class ScoreManager;
class BulletManager;
class BlockManager;
class UrduTerminal;
class Timer;
class Queue;
class MemoryModule;
class RegisterBank;
struct Cache;
struct SignalVector;
class Processor;
class PhosphorDisplay;
class GraphicsAdapter;
class MyKeyboard;
class PSU;
class Mainboard;
class StateManager;
class StateTable;
class PressingState;
class ConsolidatingState;
class RetreatingState;
class BurningResourcesState;
class StallingState;
class SavingState;
class AFKState;
class GameManager;
class EntityManager;
class LevelManager;
class GameState;
class PlayState;
class Survival;
class Campaign;
class SelfPlay;
class MainMenu;
class GameStateManager;
class Game;
class Animation;
class DamageZone;
class OneShotZone;
class TimedZone;
class ExplosionZone;
class Rocket;
class Grenade;
class Fire;
class FireZone;
class FireBomb;
class ZombieHitZone;
class MummyCurseZone;
class DroppedWeapon;


extern ScoreManager g_scoreManager;
// ==========DAMAGE ZONE SYSTEM=============

class DamageZone {
protected:
    float x;
    float y;
    float width;
    float height;
    int damage;
    bool damagesPlayersFlag;
    bool piercesShield;
    bool consumed;
    bool* deadFlag;
    bool wasProcessed;

public:
    DamageZone();
    DamageZone(float x, float y, float w, float h, int dmg, bool damagesPlayers, bool pierces, bool* deadFlag = nullptr);
    virtual ~DamageZone();

    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;
    int getDamage() const;
    bool damagesPlayers() const;
    bool getPiercesShield() const;
    virtual void igniteEntity(DamagableEntity* victim) {}
    virtual void applySpecialEffect(PlayerSoldier* player) {} //for zombie and mummy 

    virtual void update();
    virtual void onHit();
    virtual bool shouldBeRemoved() const;
};

class OneShotZone : public DamageZone {
public:
    OneShotZone(float x, float y, float w, float h, int dmg, bool damagesPlayers, bool pierces, bool* deadFlag = nullptr);
    void onHit();
    bool shouldBeRemoved() const;
};


class TimedZone : public DamageZone {
protected:
    float remainingLife;
public:
    TimedZone(float x, float y, float w, float h, int dmg, bool damagesPlayers, bool pierces, float life, bool* deadFlag = nullptr);
    void update();
    bool shouldBeRemoved() const;
};

class ExplosionZone : public DamageZone {
protected:
    int zoneId;
public:
    ExplosionZone(float x, float y, float w, float h, int dmg, bool damagesPlayers, bool pierces, bool* deadFlag = nullptr);
    bool shouldBeRemoved() const;
    int getZoneId() const { return zoneId; }
};

class FireZone : public DamageZone {
public:
    FireZone(float x, float y, float w, float h, int dmg, bool damagesPlayers);
    bool shouldBeRemoved() const;
    void igniteEntity(DamagableEntity* victim);
};

class ZombieHitZone : public OneShotZone {
public:
    ZombieHitZone(float x, float y, float w, float h, int dmg) : OneShotZone(x, y, w, h, dmg, true, false, nullptr) {
    }
    void applySpecialEffect(PlayerSoldier* player);
};

class MummyCurseZone : public OneShotZone {
public:
    MummyCurseZone(float x, float y, float w, float h, int dmg) : OneShotZone(x, y, w, h, dmg, true, false, nullptr) {
    }
    void applySpecialEffect(PlayerSoldier* player);
};



class MyString {
private:
    char* data;
    int len;

public:
    MyString();
    MyString(const char* str);
    MyString(const MyString& other);
    ~MyString();

    MyString& operator=(const MyString& other);
    MyString& operator=(const char* str);

    MyString operator+(const MyString& other) const;
    MyString operator+(const char* str) const;
    MyString& operator+=(const MyString& other);
    MyString& operator+=(const char* str);
    MyString& operator+=(char c);

    bool operator==(const MyString& other) const;
    bool operator==(const char* str) const;
    bool operator!=(const MyString& other) const;
    bool operator!=(const char* str) const;

    char& operator[](int index);
    char operator[](int index) const;

    int length() const;
    bool isEmpty() const;
    const char* c_str() const;

    void clear();
    void toUpper();
    void toLower();

    int toInt() const;
    float toFloat() const;
    static MyString fromInt(int n);
    MyString fromFloat(float f, int decimals);

    MyString substring(int start, int count) const;
    int indexOf(char c) const;
};

template<class T>

class MyVector {
private:
    int cap;
    T* arr;
    int current;

    void resize() {
        int newCap = cap + 10;
        T* newArr = new T[newCap];
        for (int i = 0; i < current; i++)
            newArr[i] = arr[i];
        delete[] arr;
        arr = newArr;
        cap = newCap;
    }
public:
    MyVector() : cap(2), arr(new T[cap]), current(0) {}
    ~MyVector() {
        delete[] arr;
    }
    MyVector(const MyVector& other) : cap(other.cap), current(other.current), arr(new T[other.cap]) {
        for (int i = 0; i < current; i++)
            arr[i] = other.arr[i];
    }
    void operator=(const MyVector& other) {
        if (this == &other)
            return;
        delete[] arr;
        cap = other.cap;
        current = other.current;
        arr = new T[cap];
        for (int i = 0; i < current; i++)
            arr[i] = other.arr[i];
    }
    void add(T element) {
        if (current == cap)
            this->resize();
        arr[current++] = element;
    }
    void addByIndex(T element, int index) {
        if (current == cap) resize();
        for (int i = current; i > index; i--)
            arr[i] = arr[i - 1];          // shift right to make room
        arr[index] = element;
        current++;
    }
    T& operator[](int index) {
        if (index < 0 || index >= current) {
            if (window != nullptr) window->close();
            return arr[0];
        }
        return arr[index];
    }

    void remove(int index) {
        if (index < current) {
            for (int i = index; i < current - 1; i++) {
                arr[i] = arr[i + 1];
            }
            current--;
        }
    }
    void removeObject(T element) {
        for (int i = 0; i < current; i++) {
            if (arr[i] == element) {
                remove(i);
                i--; //to recheck the new element
            }
        }
    }

    int getSize() const {
        return current;
    }
};
//------------------animation------------------
class Animation {
protected:
    int totalFrames;
    int currentFrame;
    float frameTime;
    float frameInterval;
    bool isComplete;
    bool active;
    Texture tex;
    float scaleX;
    float scaleY;
    IntRect frames[30]; //for custom animations(which are not evenly distributed in the sheet), max 30 frames per animation
    bool useCustomFrames;
public:
    Animation(int totalFrames, float frameInterval, bool);
    virtual void update() = 0;
    void reset();
    void loadTexture(const char* path);
    Texture& getTexture();
    void setActive(bool val);
    bool isActive() const;
    IntRect getRect();
    bool getIsComplete();
    void setScale(float x, float y);
    float getScaleX() const;
    float getScaleY() const;
    void setCustomFrames(IntRect*, int);
    int getTotalFrames()const { return totalFrames; }

};

class LoopAnimation : public Animation {
public:
    LoopAnimation(int totalFrames, float frameInterval, bool);
    void update();
};
class OneShotAnimation : public Animation {
public:
    OneShotAnimation(int totalFrames, float frameInterval, bool);
    void update();
};
//======================================
class SfmlObject {
protected:
    float x;
    float y;
    Texture Tex;
    Sprite sprite;
    float width;
    float height;
    float scaleX;
    float scaleY;

public:
    SfmlObject();
    SfmlObject(Texture& texture, float x, float y, float scale_x, float scale_y);
    void centreSprite();
    void initializeScale();
    void initializeDimensions();

    // Getters
    float getX() const;
    float getY() const;
    Texture& getTex();
    const Texture& getTex() const;
    Sprite& getSprite();
    const Sprite& getSprite() const;
    float getWidth() const;
    float getHeight() const;
    float getScaleX() const;
    float getScaleY() const;

    // Setters
    void setX(float x);
    void setY(float y);
    void setPosition(float x, float y);
    void setTex(Texture& texture);
    void setSprite(const Sprite& sprite);
    void setWidth(float width);
    void setHeight(float height);
    void setScaleX(float scaleX);
    void setScaleY(float scaleY);
    void setScale(float scaleX, float scaleY);
};
// ============= ENTITY ROOT =============

class Entity : public SfmlObject {  // this our entity + body + physicsBody
protected:

    Level* level;
    static int nextId;
    int entityId;

    char topLeft;
    char topMid;
    char topRight;
    char midLeft;
    char midRight;
    char bottomLeft;
    char bottomMid;
    char bottomRight;
    char bottomLeftAbove;
    char bottomMidAbove;
    char bottomRightAbove;
    char belowLeft;
    char belowMid;
    char belowRight;

    float velocityX;
    float velocityY;
    float max_speed;
    float acceleration;
    bool onGround;
    MyVector<Animation*> animations;

    virtual void movement() {}
    virtual void updateAnimations();



public:
    Entity();
    Entity(Level* level);
    bool checkCollision(Entity* other) const;
    bool checkCollisionCentre(Entity* other) const;
    float calculateDistance(Entity* other) const;
    bool OnGround();
    float getVelocityX();
    float getVelocityY();
    float getMaxSpeed();
    void setMaxSpeed(float speed);
    void setAcceleration(float accel);
    float getAcceleration();
    bool inAir();
    virtual ~Entity() = 0;
    Level* getLevel();
    void updateTileChecks(float check_x, float check_y);
    void entityGravity();
    bool horizontalCollisionCheck(char top = ' ', char mid = ' ', char bottom = ' ') const;
    char getMidRight() const;
    char getMidLeft() const;
    char getBottomRight() const;
    char getBottomLeft() const;
    char getTopMid() const;
    char getBottomMid() const;
    char getBelowMid() const;
    char getTopLeft() const;
    char getTopRight() const;
    void setVelocityX(float val);
    void setVelocityY(float val) { this->velocityY = val; }
    void setLevel(Level* level);
    virtual void setDevMode(bool on) {}

    virtual void receiveDamageZone(DamageZone& zone) {}
    virtual bool shouldBeRemoved() const { return false; }
    virtual bool canBeKilledWhenFar() const { return false; }
    virtual void onKilledFromFar() {}
    void updateXY();
    virtual void behaviour() {} // the goat
    virtual bool isOffScreen() const;
    virtual void clampToMap();
    virtual bool isFinalBoss() const { return false; }
    virtual bool inVehicle() { return false; };
    int getId() const { return entityId; }

    virtual void writeStateTo(CurrentState& s) const {}
    virtual void applyStateFrom(const CurrentState& s) {}
    virtual void setAndDraw();
    virtual bool shouldDrawDuringRewind() const { return true; }   // most entities always draw; players override

    virtual bool isHostileToPlayers() const { return false; }
};

class Block : public Entity {
protected:
    int HP;
    Map* map;
    void breakBlock();
    void movement();
    void updateAnimations();
public:
    Block();
    Block(Level* level);
    void behaviour();
};

class DamagableEntity : public Entity {
protected:
    int HP;
    bool isAttacking;
    bool facingRight;
    bool dead;
public:
    DamagableEntity();
    DamagableEntity(Level* level);
    int getHP();
    bool getFacingRight() const;
    void setFacingRight(bool val);
    bool isBlockingHit(float attackX) const;
    virtual void attack() = 0;
    virtual void onMeleeStart() {}
    virtual void onMeleeEnd() {}
    virtual void onIgnite() {}

    virtual void changeHP(int change);
    virtual bool absorbsHit(const DamageZone& zone) { return false; }
    bool isDead() const { return dead; }
    virtual float getShootAngle() const { return facingRight ? 0.0f : 180.0f; } // default shoot angle is straight in the direction the entity is facing

    void receiveDamageZone(DamageZone& zone);
    bool shouldBeRemoved() const;
    virtual bool isHostileToPlayers() const { return false; }

    void onKilledFromFar();

    void setAndDraw(); //overriding for the collision box fixes and uhhh facing left and right
};

// ============= PROJECTILE SYSTEM =============

class Projectile : public Entity {
protected:
    bool damagesPlayers;
    int damage;
    bool* dead;
    bool removeMe;
public:
    Projectile();
    Projectile(bool damagesPlayers, int damage, Level* level);
    virtual ~Projectile();
    virtual void movement() = 0;
};

class StraightProjectile : public Projectile {
protected:
    float angle;
    float distanceTraveled;
    float range;
public:
    StraightProjectile();
    StraightProjectile(bool damagesPlayers, int damage, float range, float angle, Level* level);
    float getAngle();
    float setAngle(float angle);
    virtual void movement();
    virtual ~StraightProjectile();
};

class BallisticProjectile : public Projectile {
protected:

public:
    BallisticProjectile();
    virtual void movement();
};



// ============= CHARACTER SYSTEM =============

class TransformationState {
protected:
    int currentState; // 0 = BASE, 1 = UNDEAD, 2 = MUMMY
    float stateTimer; // counts down from 10
public:
    TransformationState();
    void setCurrentState(int state);
    int getCurrentState();
    float getStateTimer();
    virtual void applyEffects(PlayerSoldier* player) = 0; // applies stat changes
    virtual void update(PlayerSoldier* player) = 0;       // called every frame
    virtual void removeEffects(PlayerSoldier* player) = 0; // removes stat changes when state ends
};

class BaseState : public TransformationState {
public:
    BaseState();
    void applyEffects(PlayerSoldier* player);
    void update(PlayerSoldier* player);
    void removeEffects(PlayerSoldier* player);
};

class UndeadState : public TransformationState {
    // 50% decrease in walking speed for 10 seconds
public:
    UndeadState();
    void applyEffects(PlayerSoldier* player);
    void update(PlayerSoldier* player);
    void removeEffects(PlayerSoldier* player);
};

class MummyState : public TransformationState {
    // only knife available for 10 seconds
public:
    MummyState();
    void applyEffects(PlayerSoldier* player);
    void update(PlayerSoldier* player);
    void removeEffects(PlayerSoldier* player);
};


class Soldier : public DamagableEntity {
protected:
    TransformationState* transformationState;
public:
    Soldier();
    Soldier(Level* level);
    ~Soldier();

};



struct Inventory {
    PlayerSoldier* owner;
    MyVector<Weapon*> weapons;
    int currentWeapon;
    HandGrenade* handGrenade;
    HandGrenade* fireGrenade;
    bool melee; // eri (false)
    bool mummyState;

    Inventory(PlayerSoldier* owner);
    ~Inventory();
};


class PlayerSoldier : public Soldier {
protected:
    // type 0 = MARCO , 1 = TARMA , 2 = ERI , 3 = GERMI
    Inventory inventory;
    int lives; // 2
    int state; // 0 = BASE , 1 = UNDEAD, 2 = MUMMY
    Vehicle* vehicle; // null if not in vehicle
    int damageState; // 0 = BASE , 1 = INJURY , 2 = CRITICAL , 3 = DEAD
    int saturation;
    bool fat; // this is a seperate state from state since both can be active at the same time (for example you can be in undead state and also be fat)
    bool isJumping;
    bool immortal;
    float jumpForce;
    bool isCrouching;
    bool usingMelee;
    bool xWasPressed;
    float respawnTimer; // counts down from 1 sec during respawn (player hidden meanwhile)
    bool respawning;    // true while respawn timer is ticking; player is hidden + frozen

    Texture savedTransformTex;
    float savedTransformScaleX;
    float savedTransformScaleY;
    float postRespawnInvincibilityTimer; // counts down from 2 sec after respawn; player is invincible + flickers

    float damageStateTimer; // counts down from 1 sec
    float spawnX; //for respawning and shi 
    float spawnY;
    SfmlObject* redHue;// for that cool red hue thingy 
    SfmlObject* weaponSprite;        // overlay
    Texture weaponTextures[5];       // one per weapon slot
    float handOffsetX;               // offset from player x to draw weapon
    float handOffsetY;               // offset from player y to draw weapon
    int lastWeapon;                  // track weapon switches
    bool powerupUsed;
    bool powerupActive;
    float powerupTimer;
    SfmlObject* redTop;
    SfmlObject* redBottom;
    SfmlObject* redLeft;
    SfmlObject* redRight;

    bool fusionUsedThisLevel;
    float fusionCooldown;

    virtual void movement();
    virtual void attack();
    virtual void applyFireRate();
    virtual void slugBonus();
    virtual void vehicleFireRateMultiplier(); // tarma 
    virtual void vehicleDuribilityMultiplier(); // tarma
    virtual void extraLife();
    virtual void movementMultiplier();
    virtual void blastRadiusMultiplier(); // eri
    virtual void ammoMultiplier(); // germi
    virtual void meleePierce(); // works for marco 
    virtual bool hasMelee()const { return true; } //override to false for eri
    virtual void powerupEnd() {} //every character's powerup cleanup
    virtual void meleeDamageMultipler();
    virtual void assignHP(int hp);
    virtual void powerup() = 0;
    void fusionPowerup();
    void makeImmortal();
    void makeMortal();

    void receiveDamageZone(DamageZone& zone);
    virtual int getMaxHP() const = 0;
    void drawRedHue();
    void updateDamageState();
    void handleDeath();
    void updateWeaponSprite();       // checks lastWeapon, swaps texture if changed

    void checkPowerup();
    void drawWeaponOverlay();
    virtual void clampToMap();

public:
    PlayerSoldier();
    PlayerSoldier(float x, float y, Level* level);
    virtual ~PlayerSoldier();
    virtual HandGrenade* getActiveGrenade();
    void respawn();
    Inventory& getAndSetInventory();
    void replenishOnBatchKill();
    void onMeleeStart();
    void onMeleeEnd();
    int getLives();
    void setLives(int lives);
    void setIsAttacking(bool val);
    void setUsingMelee(bool val);
    void setKnifeSprite();
    void resetSprite();
    void setShootSprite();           // replaces setPistolSprite and setHMGSprite
    void drawHUD();
    void updateAnimations();
    void setDevMode(bool on);

    void setAndDraw();               // overrides Entity::setAndDraw

    float getShootAngle() const;
    virtual void switchTransformationState(TransformationState* newState);
    virtual void onHit();
    void setSpawn(float x, float y);

    void applyTransformSprite(const char* path);
    void restoreTransformSprite();
    void setWeaponTexture(int slot, const char* path);


    bool isHostileToPlayers() const { return false; }
    void setVehicle(Vehicle* v);
    Vehicle* getVehicle() const;
    void increaseSaturation(int amount);
    void behaviour(); //  every player soldier HAS a behaviour(which is same , (moves , attacks etc..))
    virtual bool inVehicle() { return vehicle != nullptr; }

    void writeStateTo(CurrentState& s) const;
    void applyStateFrom(const CurrentState& s);

    bool shouldDrawDuringRewind() const;
    bool canSpawnFusion() const;
    void consumeFusion();
};

//  I AM ASSUMING THAT ONLY THE STUFF THAT WAS INITIALLY WRITTEN IN MARCO WAS MEANT TO BE OVERRIDDEN , NONE ELSE
class Marco : public PlayerSoldier {
    void meleePierce();
    void powerup();
    void applyFireRate();
    void attack() override;
    int getMaxHP() const { return 100; }
    void powerupEnd() {} //no cleanup for this niga's powerup
public:
    Marco(float x, float y, Level* level);
};

class Tarma : public PlayerSoldier {
    void assignHP();
    void vehicleFireRateMultiplier();
    void vehicleDuribilityMultiplier();
    void extraLife();
    void powerup();
    void powerupEnd();
    void movementMultiplier();
    int getMaxHP() const { return 80; }
public:
    Tarma(float x, float y, Level* level);
};

class Eri : public PlayerSoldier {
    bool fWasPressed; //guard for double grenade
    bool eWasPressed;
    void blastRadiusMultiplier();
    void powerup();
    void applyFireRate();
    bool hasMelee()const { return false; }
    void attack();    // override to block knife and double grenade
    int getMaxHP() const { return 100; }
public:
    Eri(float x, float y, Level* level);
    ~Eri() {}
};

class Germi : public PlayerSoldier {
    void meleeDamageMultipler();
    void ammoMultiplier(int, int);
    void powerup();
    void applyFireRate();
    void powerupEnd();
    void attack();
    int getMaxHP() const { return 100; }
public:
    Germi(float x, float y, Level* level);
    ~Germi() {}
};


class MahoragaAIState {
public:
    virtual void movement(Mahoraga* m);
    virtual void attack(Mahoraga* m);
    virtual ~MahoragaAIState() {}
};

class Mahoraga : public DamagableEntity {
protected:
    MahoragaAIState* aiState;
    Entity* nearestTarget;
    SlashGun* weapon;
    FireBombGrenade* firebomb;
    EntityManager* em;
    float lifeTimer;

    void updateAnimations();
public:
    Mahoraga(float x, float y, EntityManager* em, Level* level);
    ~Mahoraga();

    void setNearestTarget(Entity* e) { nearestTarget = e; }
    SlashGun* getWeapon() { return weapon; }
    FireBombGrenade* getFireBomb() { return firebomb; }
    EntityManager* getEntityManager() { return em; }
    void attack();
    void behaviour();
    void receiveDamageZone(DamageZone& zone) {}
    bool isHostileToPlayers() const { return false; }
    bool canBeKilledWhenFar() const { return false; }
    float getShootAngle() const;
};



// ========== ENEMY SYSTEM ============

class EnemyAIState {
protected:
public:
    virtual void movement(Enemy* enemy);
    virtual void attack(Enemy* enemy);
};

class RebelAIState : public EnemyAIState {
public:
    void movement(Enemy* enemy);
    void attack(Enemy* enemy);
};

class ShieldedAIState : public EnemyAIState {
public:
    void movement(Enemy* enemy);
    void attack(Enemy* enemy);
};

class BazookaAIState : public EnemyAIState {
public:
    void movement(Enemy* enemy);
    void attack(Enemy* enemy);
};

class GrenadeAIState : public EnemyAIState {
public:
    void movement(Enemy* enemy);
    void attack(Enemy* enemy);
};

class ParatrooperAIState : public EnemyAIState {
public:
    void movement(Enemy* enemy);
    void attack(Enemy* enemy);
};

class MummyAIState : public EnemyAIState {
public:
    void movement(Enemy* enemy);
    void attack(Enemy* enemy);
};

class ZombieAIState : public EnemyAIState {
public:
    void movement(Enemy* enemy);
    void attack(Enemy* enemy);
};

class PodAIState : public EnemyAIState {
public:
    void movement(Enemy* enemy);
    void attack(Enemy* enemy);
};

class MartianGroundedAIState : public EnemyAIState {
public:
    void movement(Enemy* enemy);
    void attack(Enemy* enemy);
};


class Enemy : public DamagableEntity {
protected:
    EnemyAIState* aiState;
    float aggression; // default is 1 multiplies stats by this speed etc
    //type is as follows:- 12 = REBEL SOLDIER , 13 = SHIELDED SOLDIER , 14 = BAZOOKA SOLDIER , 15 = GRENADE SOLDIER , 16 = PARA TROOPER , 17 = MUMMY WARRIOR , 18 = ZOMBIE , 19 = MARTIAN
    Weapon* weapon;
    bool alerted;
    float patrolDirection;
    float detectionRadius;
    float patrolPauseTimer;

    virtual void movement();
    void updateAnimations();
    virtual void attack();
    void setAggression(float val);

public:
    void setAlerted(bool val);
    bool isAlerted();
    float getDetectionRadius();
    float getPatrolDirection();
    void setPatrolDirection(float val);

    Enemy(float x, float y, Level* levelSkeleton);
    float getAggression();
    void setWeapon(Weapon& weapon);
    void removeWeapon();
    Weapon* getWeapon();
    float getPatrolPauseTimer();
    void setPatrolPauseTimer(float val);
    virtual int getBatchSize() const = 0;
    virtual void behaviour() = 0;
    virtual ~Enemy();
    bool canBeKilledWhenFar() const { return true; }
    bool isHostileToPlayers() const { return true; }

    void writeStateTo(CurrentState& s) const;
    void applyStateFrom(const CurrentState& s);
};


class RebelSoldier : public Enemy {
protected:
    // HP = 2 
    // Weapon = Pistol 
    //type = 0 
    bool dying;
    bool killedByBlast;
    void receiveDamageZone(DamageZone& zone);
    void changeHP(int change);
    void updateAnimations();
public:
    RebelSoldier(float x, float y, Level* level);
    int getBatchSize() const { return rand() % 3 + 2; } // 2-4
    void behaviour();
    ~RebelSoldier();
};

class ShieldedSoldier : public Enemy {
protected:
    // HP = 5 
    // Weapon = Pistol 
    // type = 1 
    // shield blocks frontal bullet hits — must be hit from behind, above, or with explosives
    bool dying;
    bool killedByBlast;
    void movement();
    void updateAnimations();
    void attack();
    void receiveDamageZone(DamageZone& zone);
    void changeHP(int change);
public:
    ShieldedSoldier(float x, float y, Level* level);
    ~ShieldedSoldier();
    int getBatchSize() const { return rand() % 2 + 1; } // 1-2
    void behaviour();
    bool absorbsHit(const DamageZone& zone) const;
};

class BazookaSoldier : public Enemy {
protected:
    // HP = 2 
    // Weapon = RocketLauncher 
    // type = 2 
    // batchSize = rand()%2 + 1
    bool dying;
    bool killedByBlast;
    void receiveDamageZone(DamageZone& zone);
    void changeHP(int change);
    void movement();
    void updateAnimations();
    void attack();
public:
    BazookaSoldier(float x, float y, Level* level);
    ~BazookaSoldier();
    int getBatchSize() const { return rand() % 2 + 1; } // 1-2
    void behaviour();
};

class GrenadeSoldier : public Enemy {
protected:
    // HP = 2 
    // Weapon = HandGrenade 
    // type = 3 
    // batchSize = rand()%2 + 1
    bool dying;
    bool killedByBlast;
    void receiveDamageZone(DamageZone& zone);
    void changeHP(int change);
    void movement();
    void updateAnimations();
    void attack();
public:
    int getBatchSize() const { return rand() % 2 + 1; } // 1-2
    GrenadeSoldier(float x, float y, Level* levelSkeleton);
    void behaviour();
    ~GrenadeSoldier();
};

class ParaTrooper : public Enemy {
protected:
    Enemy* landingEnemy;
    bool scoredAerial;
    void movement();
    void updateAnimations();
    void attack();
public:
    ParaTrooper(float x, float y, Level* level, int typeAfterLanding);
    bool isLanded();
    void behaviour();
    int getBatchSize() const { return landingEnemy->getBatchSize(); }
    void changeHP(int change);
    ~ParaTrooper();
};

class MummyWarrior : public Enemy {
protected:
    bool isCrumbled;
    float resurrectionTimer;
    float meleeCooldown;
    bool isDying;
    bool killedByFire;

    void movement();
    void updateAnimations();
    void attack();
    void updateResurrectionTimer();
    void startCrumble();
    void applyAnimation(int index);

    void receiveDamageZone(DamageZone& zone);
    void changeHP(int change);

public:
    MummyWarrior(float x, float y, Level* levelSkeleton);
    bool IsCrumbled() const;
    int getBatchSize() const { return 1; }
    void behaviour();
    bool absorbsHit(const DamageZone& zone) const;
    void onIgnite();
    ~MummyWarrior();
    bool isOffScreen()const;
};

class Zombie : public Enemy {
protected:
    float meleeCooldown;
    bool dying;
    bool killedByBlast;
    void movement();
    void updateAnimations();
    void attack();
    void receiveDamageZone(DamageZone& zone);
    void changeHP(int change);
public:
    Zombie(float x, float y, Level* levelSkeleton);
    int getBatchSize() const { return rand() % 3 + 3; } // 3-5
    void behaviour();
    ~Zombie();
};

class Martian : public Enemy {
protected:
    int phase;
    int podHP;
    bool transitioning;     //for pod destroy animation and shi
    bool killedByBlast;
    bool dying;             //true when death anim is playing
    void changePhase();
    void finalizePhase();
    void receiveDamageZone(DamageZone& zone);
    void movement();
    void updateAnimations();
    void attack();
public:
    Martian(float x, float y, Level* levelSkeleton);
    int getPhase();
    bool getTransitioning() const;
    void behaviour();
    void changeHP(int change);
    int getBatchSize() const { return 1; }
    ~Martian();
};

// ============= VEHICLE SYSTEM =============

class Vehicle : public DamagableEntity {
protected:
    Weapon* weapon;
    virtual void movement() = 0;
    virtual void updateAnimations() = 0;
    virtual void attack() = 0;
public:
    Vehicle(float x, float y, Level* levelSkeleton);
    virtual Weapon* getWeapon();
    virtual void behaviour() = 0;
    void writeStateTo(CurrentState& s) const;
    void applyStateFrom(const CurrentState& s);
    ~Vehicle();
};

// ============= ENEMY VEHICLE =============

class EnemyVehicle : public Vehicle {
protected:
    bool alerted;
    float detectionRadius;
    bool dying;
public:
    EnemyVehicle(float x, float y, Level* level);
    virtual int getBatchSize() const = 0;
    void changeHP(int change);
    virtual bool isHostileToPlayers() const { return true; }
};

class FlyingTara : public EnemyVehicle {
protected:
    void movement();
    void attack();
    void updateAnimations();
public:
    FlyingTara(float x, float y, Level* level);
    int getBatchSize() const { return 1; }
    void behaviour();
    ~FlyingTara();
};

class Bradley : public EnemyVehicle {
protected:
    void movement();
    void attack();
    void updateAnimations();
public:
    Bradley(float x, float y, Level* level);
    int getBatchSize() const { return 1; }
    void behaviour();
    ~Bradley();
};

class EnemySub : public EnemyVehicle {
protected:
    void movement();
    void attack();
    void updateAnimations();
public:
    EnemySub(float x, float y, Level* level);
    int getBatchSize() const { return 1; }
    void behaviour();
    ~EnemySub();
};

// ============= PLAYER VEHICLE =============

class PlayerVehicle : public Vehicle {
protected:
    PlayerSoldier* occupant;
    bool isOccupied;
    MyVector<Weapon*> weapons;
    int currentWeapon;
    bool xWasPressed;
public:
    PlayerVehicle(float x, float y, Level* level);
    virtual ~PlayerVehicle();
    virtual void enter(PlayerSoldier* player);
    virtual void exit();
    bool getIsOccupied() const;
    PlayerSoldier* getOccupant() const;
    Weapon* getWeapon();
    void changeHP(int change);
    float getShootAngle() const;
    virtual bool isHostileToPlayers() const { return false; }
};

class GroundVehicle : public PlayerVehicle {
public:
    GroundVehicle(float x, float y, Level* level) : PlayerVehicle(x, y, level) {}
    virtual void movement() = 0;
    virtual void attack() = 0;
};

class AerialVehicle : public PlayerVehicle {
public:
    AerialVehicle(float x, float y, Level* level) : PlayerVehicle(x, y, level) {}
    virtual void movement() = 0;
    virtual void attack() = 0;
};

class AquaticVehicle : public PlayerVehicle {
public:
    AquaticVehicle(float x, float y, Level* level) : PlayerVehicle(x, y, level) {}
    virtual void movement() = 0;
    virtual void attack() = 0;
};

class MetalSlug : public GroundVehicle {
protected:
    GroundForm* mode;
    void movement();
    void attack();
    void updateAnimations();
public:
    MetalSlug(float x, float y, Level* level);
    ~MetalSlug();
    int getBatchSize() const { return 1; }
    void behaviour();
    Weapon* getWeapon();
};

class SlugFlyer : public AerialVehicle {
protected:
    AerialForm* mode;
    void movement();
    void attack();
    void updateAnimations();
public:
    SlugFlyer(float x, float y, Level* level);
    ~SlugFlyer();
    int getBatchSize() const { return 1; }
    void behaviour();
    Weapon* getWeapon();
};

class SlugMariner : public AquaticVehicle {
protected:
    AquaticForm* mode;
    void movement();
    void attack();
    void updateAnimations();
public:
    SlugMariner(float x, float y, Level* level);
    ~SlugMariner();
    int getBatchSize() const { return 1; }
    void behaviour();
    Weapon* getWeapon();
};


class VehicleMode {
public:
    virtual ~VehicleMode() {}
    virtual void movement(PlayerVehicle* host) = 0;
    virtual void attack(PlayerVehicle* host) = 0;
    virtual void updateAnimations(PlayerVehicle* host) = 0;
    virtual void applySprite(PlayerVehicle* host) = 0;
    virtual Weapon* getCurrentWeapon() = 0;
    virtual void swapWeapon(int slot) = 0;
};

class GroundForm : public VehicleMode {
protected:
    MyVector<Weapon*> weapons;
    int currentWeapon;
    Texture sprite;
public:
    GroundForm(PlayerVehicle* host);
    ~GroundForm();
    void movement(PlayerVehicle* host);
    void attack(PlayerVehicle* host);
    void updateAnimations(PlayerVehicle* host);
    void applySprite(PlayerVehicle* host);
    Weapon* getCurrentWeapon();
    void swapWeapon(int slot);
};

class AerialForm : public VehicleMode {
protected:
    MyVector<Weapon*> weapons;
    int currentWeapon;
    Texture sprite;
public:
    AerialForm(PlayerVehicle* host);
    ~AerialForm();
    void movement(PlayerVehicle* host);
    void attack(PlayerVehicle* host);
    void updateAnimations(PlayerVehicle* host);
    void applySprite(PlayerVehicle* host);
    Weapon* getCurrentWeapon();
    void swapWeapon(int slot);
};

class AquaticForm : public VehicleMode {
protected:
    MyVector<Weapon*> weapons;
    int currentWeapon;
    Texture sprite;
public:
    AquaticForm(PlayerVehicle* host);
    ~AquaticForm();
    void movement(PlayerVehicle* host);
    void attack(PlayerVehicle* host);
    void updateAnimations(PlayerVehicle* host);
    void applySprite(PlayerVehicle* host);
    Weapon* getCurrentWeapon();
    void swapWeapon(int slot);
};

class AmphibiousSlug : public PlayerVehicle {
protected:
    int currentForm;
    VehicleMode* modes[3];
    void movement();
    void attack();
    void updateAnimations();
    void updateForm();
public:
    AmphibiousSlug(float x, float y, Level* level);
    ~AmphibiousSlug();
    int getBatchSize() const { return 1; }
    void behaviour();
    Weapon* getWeapon();
};





// ============ COLLECTIBLE & INTERACTABLE SYSTEM =============

class Collectible : public Entity {
protected:
    bool collected;
public:
    Collectible(float x, float y, Level* levelSkeleton);
    bool isCollected() const;
    virtual void collect();
    virtual ~Collectible();
};

class InteractableObject : public Entity {
protected:
    bool interacted;
public:
    InteractableObject();
    virtual void collect(DamagableEntity* owner) = 0;
};

class Food : public Collectible {
protected:
    void movement();
    void updateAnimations();
    virtual void collect();
    virtual int getSaturationAmount() const = 0;
public:
    Food(float x, float y, Level* levelSkeleton);
    void increaseSaturation(PlayerSoldier* player);
    void behaviour();
    bool shouldBeRemoved() const { return collected; }
};

class Turkey : public Food {
protected:
    int getSaturationAmount() const { return 3; }
public:
    Turkey(float x, float y, Level* levelSkeleton);
};

class Fruit : public Food {
protected:
    int getSaturationAmount() const { return 2; }
public:
    Fruit(float x, float y, Level* levelSkeleton);
};

class DroppedWeapon : public Collectible {
protected:
    Weapon* weapon;
    void movement();
    void updateAnimations();
public:
    DroppedWeapon(float x, float y, Level* levelSkeleton, Weapon* weapon);
    void collect();
    void behaviour();
    bool shouldBeRemoved() const { return collected; }
    ~DroppedWeapon();
};

class Prisonor : public InteractableObject {
protected:
    // type = 29
    void movement();
    void updateAnimations();
    void collect(DamagableEntity* owner); // this will free the prisonor and drop a supply box at the prisonors location which the player can then collect  ALSO ADDed by ayan
    void dropSupplyBox();

public:
    Prisonor(float x, float y, Level* levelSkeleton);
    void behaviour();
};

class SupplyBox : public InteractableObject {
protected:
    Weapon* containedWeapon;
    int grenadeCount;
    int fireBombCount;
    bool hasFireBombs;
    bool opened;

    void replenishAmmo(PlayerSoldier* player);
    void dropWeapon(PlayerSoldier* player);
    void giveGrenades(PlayerSoldier* player);
    void openSupplyBox(PlayerSoldier* player);

    void movement();
    void updateAnimations();
    void collect(DamagableEntity* owner);
public:
    SupplyBox(float x, float y, Level* levelSkeleton);
    void behaviour();
    bool shouldBeRemoved() const { return interacted; }
    ~SupplyBox();
};




// ============= WEAPON SYSTEM =============

class Weapon : public InteractableObject {
protected:
    DamagableEntity* owner;
    float damageDealt;
    float range;
    float angle;
    float fireRate;
    bool fireInBothDirections;
    float cooldownTimer;
    Animation* fireAnim;
    float muzzleOffsetX;
    float muzzleOffsetY;
    float baseFireRate; //for reverting

    bool canFire();
public:
    Weapon();
    Weapon(DamagableEntity* owner);
    virtual void fire() = 0;
    virtual void update();
    virtual void reload() {}
    virtual void addAmmo(int delta) {}
    float getDamage();
    void setDamage(float);
    float getRange();
    virtual bool isOutOfAmmo() { return false; }
    virtual void setPierceShield() {}
    virtual void boostFireRate(float multiplier);
    float getFireRate();
    void resetFireRate();
    bool isInRange(Entity* E);
    float getAngle();
    void collect(DamagableEntity* owner);

    virtual void fireAtAngle(float angle) {}
    float getCooldownTimer() const { return cooldownTimer; }
    virtual void scaleBlastRadius(float multiplier) {}

    Animation* getFireAnim() { return fireAnim; }
    float getMuzzleOffsetX() { return muzzleOffsetX; }
    float getMuzzleOffsetY() { return muzzleOffsetY; }

    virtual int getAmmo() const { return -1; }
    virtual MyString getDisplayName() const { return MyString("WEAPON"); }
    ~Weapon();
};

class ProjectileWeapon : public Weapon {
protected:
    int totalAmmo;
    int currentMag;
    int magazineSize;
    float reloadTime;
    float reloadTimer;
    bool reloading;
public:
    ProjectileWeapon();
    virtual void fire() = 0;
    virtual void reload();
    virtual void update();
    virtual bool hasInfiniteAmmo() { return false; }
    virtual void addAmmo(int amount) { totalAmmo += amount; }
    virtual void applyExplosionEffect(int x, int y, int damage);
    int getTotalAmmo();
    int getCurrentMag();
    bool isReloading();
    void fireAtAngle(float angle);
    virtual bool isOutOfAmmo();
};


class Knife : public Weapon {
protected:
    //damage dealt = 2Hp
    bool pierce;
    bool attacking;

    virtual void movement();
    virtual void updateAnimations();
    virtual void fire();
    virtual void setPierceShield();

public:
    Knife(DamagableEntity* owner);
    virtual void behaviour();
    virtual void update();
    virtual void drop();
    virtual void collect(DamagableEntity* owner);
    virtual ~Knife();
    bool canPierceShield();
    MyString getDisplayName() const;
};


class Bullet : public StraightProjectile {
protected:
    void fatPlayerBulletMovement();
    void movement();
    void updateAnimations();
public:
    Bullet(bool damagesPlayers, int damage, float range, float spawnX, float spawnY, float angle, Level* level);
    void behaviour();
    bool shouldBeRemoved() const;
    void writeStateTo(CurrentState& s) const;
    void applyStateFrom(const CurrentState& s);
};

class Rocket : public StraightProjectile {
protected:
    float blastRadius;

    void movement();
    void updateAnimations();
public:
    Rocket(bool damagesPlayers, int damage, float range, float blastRadius, float spawnX, float spawnY, float angle, Level* level);
    void behaviour();
    bool shouldBeRemoved() const;
};

class Laser : public StraightProjectile {
protected:
    int lifetime;
    void movement();
    void updateAnimations();
public:
    Laser(bool damagesPlayers, int damage, float spawnX, float spawnY, float beamLength, float angle, Level* level);
    void behaviour();
    bool shouldBeRemoved() const;
};

class Grenade : public BallisticProjectile {
protected:
    float blastRadius;

    void movement();
    void updateAnimations();
    void explode();
    virtual void onExplode() {}
public:
    Grenade(bool damagesPlayers, int damage, float blastRadius, float spawnX, float spawnY, float angle, float speed, Level* level);
    void behaviour();
    bool shouldBeRemoved() const;
};

class FireBomb : public Grenade {
public:
    FireBomb(bool damagesPlayers, int damage, float blastRadius, float spawnX, float spawnY, float angle, float speed, Level* level);
    void onExplode();
};

class Pistol : public ProjectileWeapon {
public:
    Pistol(DamagableEntity* owner);
    Pistol(float x, float y, Level* level);
    void fire();
    void reload();
    bool hasInfiniteAmmo();
    MyString getDisplayName() const;
};

class Slash : public StraightProjectile {
protected:
    void fatPlayerBulletMovement();
    void movement();
    void updateAnimations();
public:
    Slash(bool damagesPlayers, int damage, float range, float spawnX, float spawnY, float angle, Level* level);
    void behaviour();
    bool shouldBeRemoved() const;
    void writeStateTo(CurrentState& s) const;
    void applyStateFrom(const CurrentState& s);
};

class SlashGun : public ProjectileWeapon {
public:
    SlashGun(DamagableEntity* owner);
    SlashGun(float x, float y, Level* level);
    void fire();
    void reload();
    bool hasInfiniteAmmo();
    MyString getDisplayName() const;
};

class HeavyMachineGun : public ProjectileWeapon {
protected:
    //fire rate double than pistol (8 shots / sec)
    //same damage as pistol (3HP)
public:
    HeavyMachineGun(DamagableEntity* owner);
    HeavyMachineGun(float x, float y, Level* level);
    void fire();
    MyString getDisplayName() const;
};

class Explosion : public Entity {
protected:
    float blastRadius;
    bool done;
public:
    Explosion(float cx, float cy, float radius, Level* level);
    ~Explosion();
    void behaviour();
    bool shouldBeRemoved() const { return done; }
};

class RocketLauncher : public ProjectileWeapon {
protected:
    float blastRadius;
public:
    RocketLauncher(DamagableEntity* owner);
    RocketLauncher(float x, float y, Level* level);
    void setBlastRadius(float r) { blastRadius = r; }
    float getBlastRadius() { return blastRadius; }
    void scaleBlastRadius(float multiplier) { blastRadius *= multiplier; }
    void fire();
    MyString getDisplayName() const;
};

class Fire : public Entity {
protected:
    float lifetime;
    float tickTimer;
    int damagePerTick;
    bool damagesPlayers;
public:
    Fire(float x, float y, float w, float h, float lifetime, int damagePerTick, bool damagesPlayers, Level* level);
    void behaviour();
    bool shouldBeRemoved() const;
};

class FlameShot : public ProjectileWeapon {
protected:
    // works like a stream 
    // range = 5Bodys
    // damage = 2hp per second
    // only way to kills mummies
public:
    FlameShot(DamagableEntity* owner);
    FlameShot(float x, float y, Level* levelSkeleton);
    void fire();
    void applyEffect();
    MyString getDisplayName() const;
};


class LaserGun : public ProjectileWeapon {
protected:
public:
    LaserGun(DamagableEntity* owner);
    LaserGun(float x, float y, Level* levelSkeleton);
    void fire();
    MyString getDisplayName() const;
};

class PodLaserGun : public LaserGun {
public:
    PodLaserGun(DamagableEntity* owner);
    void fire();
};

class HandGrenade : public Weapon {
protected:
    float throwOffset;
    float blastRadius;
    int ammo;
    float throwSpeed;
    float gravity;
public:
    HandGrenade(DamagableEntity* owner);
    HandGrenade(float x, float y, Level* levelSkeleton);
    void fire();
    virtual void onImpact();
    void applyExplosionEffect(int x, int y, int damage);
    void addAmmo(int amount);
    int getAmmo();
    void setBlastRadius(float r) { blastRadius = r; }
    float getBlastRadius() { return blastRadius; }
    void scaleBlastRadius(float multiplier) { blastRadius *= multiplier; }
    void setOffset(float x) { this->throwOffset = x; }
    virtual bool isOutOfAmmo();
    MyString getDisplayName() const;
};

class FireBombGrenade : public HandGrenade {
protected:
public:
    FireBombGrenade(DamagableEntity* owner);
    FireBombGrenade(float x, float y, Level* levelSkeleton);
    void fire();
    void onImpact();
    void applyFireEffect(int x, int y);
    MyString getDisplayName() const;
};




// ============ BOSS =================


class Boss : public DamagableEntity {
protected:
    Weapon* weapon[3];
    int currentWeapon;
    MyVector<DamagableEntity*> currentBatch;
    float batchCooldown;
    bool retreating;
    int maxHP;
    float detectionRadius;

    virtual void attack() = 0;
    virtual void movement() = 0;
    virtual void updateAnimations() = 0;
    virtual void spawnNextBatch() {}
    void pruneDeadMinions();
public:
    bool playerInRange() const;
    Boss(float x, float y, Level* levelSkeleton);
    virtual ~Boss();
    virtual void behaviour() = 0;
    int getMaxHP() const;
    bool isRetreating() const;
    bool isHostileToPlayers() const { return true; }
    float getShootAngle() const;
    void writeStateTo(CurrentState& s) const;
    void applyStateFrom(const CurrentState& s);
};
class IronNokana : public Boss {
protected:
    float weaponRotateTimer;
    void movement();
    void updateAnimations();
    void attack();
    void spawnNextBatch();
public:
    IronNokana(float x, float y, Level* levelSkeleton);
    ~IronNokana();
    void changeHP(int change);
    void behaviour();
};

class HairBuster : public Boss {
protected:
    float weaponRotateTimer;
    void movement();
    void updateAnimations();
    void attack();
    void spawnNextBatch();
public:
    HairBuster(float x, float y, Level* levelSkeleton);
    ~HairBuster();
    void changeHP(int change);
    void behaviour();
};

class SeaSatan : public Boss {
protected:
    float weaponRotateTimer;
    void movement();
    void updateAnimations();
    void attack();
    void spawnNextBatch();
public:
    SeaSatan(float x, float y, Level* levelSkeleton);
    ~SeaSatan();
    void changeHP(int change);
    void behaviour();
};

class BossMode {
protected:
    int hp;
    float minionTimer;
public:
    BossMode(int hp);
    virtual ~BossMode();
    int getHP() const;
    void changeHP(int delta);
    bool isDead() const;
    virtual void attack(UltimateBoss* host) = 0;
    virtual void movement(UltimateBoss* host) = 0;
    virtual void updateAnimations(UltimateBoss* host) = 0;
    virtual void spawnMinionBatch(UltimateBoss* host) = 0;
    virtual void tickMinions(UltimateBoss* host);
    virtual void applySprite(UltimateBoss* host) = 0;
};

class GroundMode : public BossMode {
protected:
    Weapon* rocket;
    Weapon* flame;
    Weapon* firebomb;
    int currentWeapon;
    float weaponRotateTimer;
    Texture sprite;

    LoopAnimation* anim;

public:
    GroundMode(int hp, UltimateBoss* host);
    ~GroundMode();
    void attack(UltimateBoss* host);
    void movement(UltimateBoss* host);
    void updateAnimations(UltimateBoss* host);
    void spawnMinionBatch(UltimateBoss* host);
    void applySprite(UltimateBoss* host);
};

class AerialMode : public BossMode {
protected:
    Weapon* rocket;
    Weapon* firebomb;
    int currentWeapon;
    float weaponRotateTimer;
    Texture sprite;
    LoopAnimation* anim;

public:
    AerialMode(int hp, UltimateBoss* host);
    ~AerialMode();
    void attack(UltimateBoss* host);
    void movement(UltimateBoss* host);
    void updateAnimations(UltimateBoss* host);
    void spawnMinionBatch(UltimateBoss* host);
    void applySprite(UltimateBoss* host);
};

class AquaticMode : public BossMode {
protected:
    Weapon* hmg;
    Weapon* rocket;
    int currentWeapon;
    float weaponRotateTimer;
    Texture sprite;
    LoopAnimation* anim;

public:
    AquaticMode(int hp, UltimateBoss* host);
    ~AquaticMode();
    void attack(UltimateBoss* host);
    void movement(UltimateBoss* host);
    void updateAnimations(UltimateBoss* host);
    void spawnMinionBatch(UltimateBoss* host);
    void applySprite(UltimateBoss* host);
};

class UltimateBoss : public Boss {
protected:
    BossMode* modes[3];
    int currentModeIndex;
    float modeTimer;
    float modeDuration;

    bool transitioning;
    float transitionTimer;
    float transitionDuration;
    int pendingModeIndex;
    float startY;
    float targetY;

    bool introPlaying;
    int introPhase;
    float introTimer;
    float ironIntroX, ironIntroY;
    float satanIntroX, satanIntroY;
    float hairIntroX, hairIntroY;
    float collisionX, collisionY;
    bool explosionSpawned;

    // one LoopAnimation per intro sprite
    LoopAnimation* introIronAnim;
    LoopAnimation* introSatanAnim;
    LoopAnimation* introHairAnim;

    void runIntro();
    void drawIntroSprite(LoopAnimation* anim, float worldX, float worldY, bool flipX);

    void attack();
    void movement();
    void updateAnimations();
    void cycleMode();
    void startTransition(int newIdx);
    float computeTargetY(int idx);
    int pickModeFromTerrain();
public:
    UltimateBoss(float x, float y, Level* levelSkeleton);
    ~UltimateBoss();
    void behaviour();
    void changeHP(int delta);
    bool shouldBeRemoved() const;
    MyVector<DamagableEntity*>& getBatch();
    BossMode* getCurrentMode();
    bool isFinalBoss() const { return true; }
};

// ============= LEVEL & ENVIRONMENT =============
class Map {
    char** lvl;
    int cell_size;
    int height;
    int width;
    long long chunkOriginX;
    Block** blocks;
public:
    Map(int height, int width);
    char**& getLvl();
    void setTile(int x, int y, char tile);
    int getCellSize();
    int getHeight();
    int getWidth();
    long long getChunkOriginX() const;
    void setChunkOriginX(long long x);
    ~Map();
};
class Level {
protected:
    MyVector<Entity*>* pendingSpawns;
    MyVector<DamageZone*>* damageZones;
    PlayerSoldier* currentPlayer; // WE HOLD A POINTER TO PLAYER SO WE CAN SCROLL LEVEL
    SfmlObject* background;
    SfmlObject* grassBlock;
    SfmlObject* dirtBlock;
    SfmlObject* bedrock;
    SfmlObject* water;
    Map* map[3]; // for survival 0 and 2 are null and in infinite 1 is current, 0 is prev, 2 is next
    bool levelComplete;
    int screenLeft;
    int screenRight;
    int screenTop;
    int screenBottom;
    float timeTillRain; // random value assigned in constructor
    float rainDuration;
    bool isRaining;
public:
    Level();
    Level(PlayerSoldier* currentPlayer);
    virtual ~Level();
    void fillMapFromSeed(char**& lvl, int seed, int profileChoice);
    PlayerSoldier* getCurrentPlayer();
    virtual Map* getMap();
    virtual int getMapCount();
    virtual Map* getMapAt(int i);
    virtual int getHeight(long long x);
    virtual char tileAtWorld(float worldX, float worldY);
    virtual void generateMap() = 0;
    virtual void renderScreen() = 0;
    void setDamageZones(MyVector<DamageZone*>* dz) { damageZones = dz; }
    void addDamageZone(DamageZone* zone);
    void setPendingSpawns(MyVector<Entity*>* ps) { pendingSpawns = ps; }
    void queueSpawn(Entity* e);
    virtual void checkCompletion(EntityManager* em) {}
    bool isLevelComplete() const { return levelComplete; }
    void setLevelComplete(bool v) { levelComplete = v; }
    virtual bool hasFiniteBounds() const { return true; }

    // Getters
    int getScreenLeft() const;
    int getScreenRight() const;
    int getScreenTop() const;
    int getScreenBottom() const;

    // Setters
    void setScreenLeft(int value);
    void setScreenRight(int value);
    void setScreenTop(int value);
    void setScreenBottom(int value);
    void setCurrentPlayer(PlayerSoldier* player);

    void updateRain();    // ticks timeTillRain, triggers rain, counts down rainDuration
    bool getIsRaining();

    virtual void scrollLevel();
    int surfaceRow(char**& lvl, int col);
    void placeVehicleMarkers(char**& lvl, int W, int H);
    MyString getDisplayName() const;
};


class SurvivalLevel : public Level {
protected:
    bool isBossLevel;
    int currentLevel;
public:
    SurvivalLevel(bool isBossLevel, int currentLevel);
    SurvivalLevel(bool isBossLevel, PlayerSoldier* currentPlayer, int currentLevel);
    virtual ~SurvivalLevel();
    void generateMap();
    void renderScreen();
    void checkCompletion(EntityManager* em);
    MyString getDisplayName() const;

};

class InfiniteLevel : public Level {
protected:
    int profileChoice;
    int worldSeed;
    NoiseProfileFactory* noiseProfileFactory;
    FractalNoiseGenerator* fractalGen;
public:
    InfiniteLevel(PlayerSoldier* currentPlayer, int profileChoice);
    virtual ~InfiniteLevel();
    int getHeight(long long x);
    int getMapCount();
    Map* getMapAt(int i);
    void generateMap();
    Map* generateChunk(long long anyXInChunk);
    void shiftChunksIfNeeded();
    void renderScreen();
    void scrollLevel();
    char tileAtWorld(float worldX, float worldY);
    bool hasFiniteBounds() const { return false; }
    MyString getDisplayName() const;
};

class HashValue {
protected:
    float grad;
    float val;
public:
    HashValue();
    void seed(long long x, int globalRandomConst);
    float getGrad();
    float getVal();
};


class PerlinGenerator {
protected:
    int worldSeed;
    HashValue value[2];
public:
    PerlinGenerator(int worldSeed);
    float noiseAt(long long x, int spacing);
};


class NoiseProfile {
protected:
    int octaves;
    int baseSpacing;
    float baseAmplitude;
    int heightMultiplier;
    int heightOffset;
public:
    NoiseProfile();
    virtual ~NoiseProfile() {}
    int getOctaves() const;
    int getBaseSpacing() const;
    float getBaseAmplitude() const;
    int getHeightMultiplier() const;
    int getHeightOffset() const;
    virtual float shape(float n) const;
};

class AmplifiedProfile : public NoiseProfile {
public:
    AmplifiedProfile();
    float shape(float n) const;
};

class FlatProfile : public NoiseProfile {
public:
    FlatProfile();
    float shape(float n) const;
};

class NormalProfile : public NoiseProfile {
public:
    NormalProfile();
};


class FractalNoiseGenerator {
protected:
    PerlinGenerator* perlinGenerator;
    int worldSeed;
public:
    FractalNoiseGenerator(int worldSeed);
    ~FractalNoiseGenerator();
    void generateSummedHeights(long long anyXInChunk, int* outBuffer, NoiseProfile* profile);
};


class NoiseProfileFactory {
protected:
    NoiseProfile* noiseProfile;
public:
    NoiseProfileFactory();
    ~NoiseProfileFactory();
    void selectProfile(int selectedProfile);
    NoiseProfile* getProfile();
};

///////////////////////////////////////////////////
////////////NEAT ALGORITHM START///////////////////////////////////////
class NodeGene {
protected:
    int id;
    int type; // 0 -> input , 1 -> hidden(beech me mtlb) , 2 -> ouput 
    float value;
    //neural network type shi
    // inputs           hidden              output
    // Node 1 ->     Nodex           ->       ...
    // Node 2 ->     ...             ->       ....  
    // Node 3 ->      ..             ->     ....
    // Node 4 ->        ...         ->         ...
public:
    NodeGene(int id, int type);
    float getValue() const;
    void setValue(float v);
    float sigmoid(float x) { //this squashes to 0 - 1
        return  1.0f / (1.0f + exp(-4.9f * x)); // steepened sigmoid from paper
    }
};
class ConnectionBase {
protected:
    int fromNode;
    int toNode;
    int innovationNumber;
public:
    int getfromNode()const;
    int getToNode() const;
    int getInnovationNumber() const;
    void setFromNode(int);
    void setToNode(int);
    void setInnovatioNum(int);
};
class ConnectionGene { //the "wire"(->) between 2 nodes (basically the tracker)
protected:
    ConnectionBase data;
    float weight;
    bool isEnabled;
public:
    ConnectionGene(int from, int to, float weight, int innovation);
    int getFromNode()const;
    int getToNode()const;
    float getWeight()const;
    bool getIsEnabled() const;
};
class Genome { // a single player's blueprint
protected:
    NodeGene* nodes;
    ConnectionGene* connections;
    int nodeCount;
    int connectionsCount;
    float fitness; //how good the genome is
public:
    Genome(int inputCount, int outputCount); //starts minimal without any hidden nodes with random weights assigned
    Genome(const Genome& copy);
    ~Genome();
    void addNode(InnovationRegistry& registry);
    void addConnection(InnovationRegistry& registry);
    void tuneWeights();
    Genome crossOver(Genome& other);
    float returnCompatibilityDistance(const Genome& other)const;
    float getFitness() const;
    int getNodeCount() const;
    int getConnectionCount() const;
    NodeGene* getNodes();
    ConnectionGene* getConnections();
};
class InnovationRegistry {
protected:
    ConnectionBase* globalInnovations;
    int count;
public:
    InnovationRegistry(int count);
    ~InnovationRegistry();
    int getInnovationCount(int from, int to); //returns existing if the pair already happened OR assigns and return a new number
    void reset(); // to start new generation
};
class Species {
protected:
    Genome* similarGenomes;
    int genomeCount;
    int capacity;
    Genome tester; //to check against k same type he or wha
    float bestFitness;
    int generationsWithNoImprovement; //if this goes to 15 , we kill the species
public:
    Species(Genome& first); //the 1st will be the tester for the species to be tested against
    ~Species();
    bool isCompatible(const Genome& other);
    void adjustFitness();
    void reproduce();
};
class Population {
protected:
    Species* allSpecies;
    int speciesCount;
    int speciesCapacity;
    Genome* allGenomes;
    int genomeCount;
    Genome globallyBest;
    int currentGeneration; //for tracking purposes
    InnovationRegistry registry;
public:
    Population(int size, int inputCount, int outputCount); //creates 'size' identical genomes
    ~Population();
    void speciate();// assign genomes to species
    void evaluate();// let each genome play, get fitness
    void reproduce();// breed next generation , basically calls each species reproduce 
    void generationLoop();
};
class NeuralNetwork {
protected:
    Genome* source;
public:
    NeuralNetwork(Genome* other);
    ~NeuralNetwork();
    void takeInputs(float* inputs, int count);
    void propagate(); //running the signal 
    float* getOutput(); //feeds output to the applyOutput of the NEATController
};
class NEATtrackerHelper {
protected:
    int currentIndex;    // which genome is playing right now
    int totalGenomes;    // how many in this generation
    float episodeTime;   // how long current genome has been playing
    float maxEpisodeTime; // kill it after this long 
    bool episodeRunning;
public:
    NEATtrackerHelper(int total);
    bool hasNext(); // is there another genome to run?
    int getCurrentIndex(); // which genome is playing
    void startEpisode();// begin timing
    void endEpisode(); // record, move to next
    bool shouldTerminate();// time up OR player died
};
class NEATController {
protected:
    Population population;
    NeuralNetwork* currentNetwork;
    PlayerSoldier* currentPlayer;
    NEATtrackerHelper tracker;
    Level* activeLevel;
    float* gatherInputs(); //reads the game  , return array , goes to the takeInputs of NeuralNetwork for processing
    void applyOutput(float* output); //calls player methods 
    float calculateFitness(); //maybe itll go like score + kills + survival time , could add more things as well but these are enough for now ig
public:
    NEATController();
    ~NEATController();
    void runGeneration();
    void saveBestGenome();
    void loadBestGenome();
    void activateBestPlayer(); // the 'B' key feature 
};
/// 
////////////////NEAT ALGORITHM END /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

// ============ file handler ============== 
class FileHandler {
protected:

public:
    virtual void loadFile(const char* name) = 0;
    virtual void readFile(const char* name) = 0;
    virtual void writeFile(const char* name) = 0;
    virtual void appendFile(const char* name) = 0;
    virtual void saveFile(const char* name) = 0;
};

class ProfileFileHandler : public FileHandler {
protected:

public:
    void loadFile(const char* name);
    void readFile(const char* name);
    void writeFile(const char* name);
    void appendFile(const char* name);
    void saveFile(const char* name);
};

class RageBaitFileHandler : public FileHandler {
public:
    void loadFile(const char* name);
    void readFile(const char* name);
    void writeFile(const char* name);
    void appendFile(const char* name);
    void saveFile(const char* name);
};

class ForksOfFateFileHandler {
protected:
    ofstream out;
    bool open;
public:
    ForksOfFateFileHandler();
    ~ForksOfFateFileHandler();
    void openTimeline(const char* name);
    void closeTimeline();
    void appendSnapshot(const CurrentState& s);
    bool readSnapshotAt(const char* name, int index, CurrentState& dst);
    int snapshotCount(const char* name);
};

// ==============================================


/////////////////////////////////////////////////////////////////
///////////////FINGERPRINTS START//////////////////////////////////


class Parameter {
protected:
    char* id;
    char* name;
    float value;
    float weight;
    float max;
    float min;
    //not adding datatype(everything is float anyways) ,unit(not needed) , description (not needed) , category ( ...)
public:
    Parameter(char* id, char* name, float value, float weight, float min, float max);
    float getValue() const;
    void setValue(float v);
    char* getId() const;
    char* getName() const;
    float getWeight() const;
    float getMin() const;
    float getMax() const;
};

class Profile {
protected:
    Parameter* parameters;
    const int count; //260
    ProfileFileHandler* profileFileHandler;

public:
    Profile();
    ~Profile();
    Parameter* getById(const char* id);
    Parameter* getByName(const char* name);
    void setValue(const char* id, float value);
    float getValue(const char* id) const;
};

class Recorder {
protected:
    Profile* profile;
    PlayerSoldier* trackedPlayer;
    EntityManager* enemies; //it needs this for some parts like checking enemy positions, type and shi
    int frameCount;
public:
    Recorder(Profile* profile);
    void setTrackedPlayer(PlayerSoldier* player);
    void update();  // called every frame by GameManager
};


class GhostController {
protected:
    Profile* profile;
    PlayerSoldier* controlledPlayer;
    EntityManager* enemies; // basically to see enemies in order to shoot 
    PlayerSoldier* actualPlayer; // to know where the real player is
public:
    GhostController(Profile* profile, PlayerSoldier* controlledPlayer, EntityManager* enemies, PlayerSoldier* actualPlayer);
    ~GhostController();
    virtual void update(); // called every frame to make decisions
};


class Autopilot : public GhostController {
protected:
    float timer;    // counts down from 15
    const float maxSeconds;   // 15.0f   -> can also be just setted in the constructor without making it a variable
    bool active;
public:
    Autopilot(Profile* profile, PlayerSoldier* controlledPlayer, EntityManager* enemies, PlayerSoldier* actualPlayer);
    ~Autopilot();
    void activate();    // starts the 15 second timer
    bool isActive() const;
    void update();    // override, adds timer logic
};


class SessionManager {
protected:
    Profile profile;
    int sessionCount;
public:
    SessionManager();
    ~SessionManager();
    void onGameLaunch();  // loads CSV, increments session_count
    void onGameQuit();   // saves CSV
    Profile* getProfile();  // hands profile to whoever needs it
};


////////////////FINGERPRINTS END /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


//===================================== DEATH COMPOSITOR ================================

struct NearbyEnemyInfo {
    int enemyID;
    int enemyType;
    float distanceFromPlayer;
};

class DeathFingerPrint {
protected:
    int playerType;
    float x;
    float y;
    int weaponType;
    int ammoLeft;
    bool wasInVehicle;
    int damageState;
    NearbyEnemyInfo nearbyEnemies[10];
    int nearbyEnemyCount;
    int killerEnemyType; // what actually killed you
    int timeSinceCheckPoint;
public:
    DeathFingerPrint();
    void recordDeath(GameState& currentState, int killerID); // uses takeSnapshot() of forksoffate to make a gamestate
    float compareToSelf(DeathFingerPrint& other);   // returns similarity score 0 to 100
    float compareToCurrent(GameState& gamestate);
    int getKillerType() const;
    float getDeathX() const;
    float getDeathY() const;
    int getWeaponType() const;
    bool getWasInVehicle() const;
};

class DeathCompositor {
protected:
    DeathFingerPrint* fingerPrintsHistory;
    int FPcount;
    EnemyManager* enemyManager;
public:
    DeathCompositor();
    void addFingerPrint();
    int compareCurrentToHistory(GameState& gamestate); // compares current situation with all of history and returns index of the most similar fingerprint with at 
    // least similarityscore of above 70 else returns -1
    void increaseAggression(); // called only if similarityscore reaches above 80 uses enemyManager calls once more if above 90 and again if above 95
    void spawnKillerEnemies(); // spawns enemies from NearbyEnemyInfo through enemyManager if score>=80
};

// ==========================================================================================












//========================= VOICE OF BENE GESSERIT ====================== ECHO CHAMBERS ================================


struct WavHeader {
    char chunkId[4];
    int chunkSize;
    char format[4];
    char subchunk1Id[4];
    int subchunk1Size;
    short audioFormat;
    short numChannels;
    int sampleRate;
    int byteRate;
    short blockAlign;
    short bitsPerSample;
    char subchunk2Id[4];
    int subchunk2Size;
    WavHeader();
    void load(ifstream& file);
};

class Whisper {
private:
    WavHeader header;
    short* data;
    int size;
public:
    // Default constructor
    Whisper();
    // Parameterized constructor
    Whisper(WavHeader h, short* samples, int numSamples);
    // Copy constructor
    Whisper(const Whisper& other);
    void convertto16bit();
    WavHeader getheader() const;
    void Normalization();
    void operator*=(float factor);
    void Reversal();
    Whisper Splicing(float starttime, float endtime);
    //phase 3 math engine
    int nextPowerOf2(int n);
    Spectrum decoupling();
    void doubleAndCopy(Complex**& ptr, int currentCount);
    void load(ifstream& file);
    Whisper operator+(Whisper& other);
    Whisper& operator+=(Whisper& other);
    Whisper& operator=(const Whisper& other);
    void display()const;
    // Destructor
    ~Whisper();
};

class Complex {
private:
    float real, imaginary;
public:
    Complex();
    Complex(float r, float i);
    void appendvalues(int x);
    float getreal() const;
    Complex operator+(const Complex& other);
    Complex operator-(const Complex& other);
    Complex operator*(const Complex& other);
    Complex operator*(float weight);
    void display()const;
};

class Spectrum {
private:
    Complex* frequencyBins;
    int size;
    int originalSize;
    int sampleRate;
public:
    Spectrum();
    Spectrum(Complex* bins, int s, int original, int sr);
    Spectrum(const Spectrum& other);
    Spectrum& operator=(const Spectrum& other);
    ~Spectrum();
    void applyFilter(Filter& filter);
    Whisper coupling();
    void doubleAndCopy(Complex**& ptr, int currentCount);
    int getSampleRate() const;
    int getSize() const;
    void display() const;
};


class Filter {
private:
    float* weights;
    int size;
public:
    Filter();
    Filter(float* w, int s);
    Filter(const Filter& other);
    ~Filter();
    void lowPass(float cutoff, int sampleRate, int s);
    void highPass(float cutoff, int sampleRate, int s);
    void bandPass(float lower, float upper, int sampleRate, int s);
    float* getWeights();
};

class SampleAudio {
protected:
    Whisper sample;
    Spectrum decomposed;
public:
    SampleAudio();
    void load(const char* filename); // for reference audio from file
    void loadFromMic(short* data, int size, WavHeader header); // for mic capture
    void applyFFT(); // calls sample.decoupling() stores result in decomposed
    Spectrum& getSpectrum();
};

class AudioChecker {
protected:
    SampleAudio* reference;
    SampleAudio* test;
    float magnitude(Spectrum& s);
    float dotProduct(Spectrum& a, Spectrum& b);
public:
    AudioChecker();
    void loadReference(const char* filename);
    void captureMic();  // grabs mic audio puts it in test
    float cosineSimilarity();  // compares reference and test
    bool isMatch();  // returns true if cosineSimilarity >= 0.4
};

//============echo chamber=============
class EchoChamber {
protected:
    bool isSubmerged;
    Whisper* originalAudio;   // non-owning
    Whisper muffledAudio;     // owns this
    Filter lowPassFilter;
    void process();
public:
    EchoChamber();
    void setAudio(Whisper* audio);
    void update(bool playerInWater);
    Whisper* getActiveAudio();
    bool getIsSubmerged() const;
};
//=====================================
class AudioManager {
protected:
    EchoChamber echoChamber;
    AudioChecker audioChecker;
public:
    AudioManager();
    EchoChamber& getEchoChamber();
    void setAudio(Whisper* audio);
    bool match(const char* filename); // loads sample and starts listening then loads test and checks for audio
};
//=============RageBait====================
class RageBait {
protected:
    int* midScores;
    int* finalScores;
    int runCount;
    int currentMidScore;
    bool midPointPassed;
    EnemyManager* enemyManager;
    RageBaitFileHandler ragebaitFileHandler;
    void adjustSpawns(); // tells enemyManager to spawn low value enemies
public:
    RageBait(EnemyManager*);
    void onMidPoint(int currentScore);
    void onRunEnd(int finalScore);
    ~RageBait();
};

//====================================== FORKS OF FATE ========================================




struct States {
    int id;
    float x;
    float y;
    States();
};

struct PlayerState : public States {
    int lives;
    int activeWeaponIndex;
    int activeWeaponAmmo;
    int vehicleID;
    bool immortal;
    bool dead;
    int HP;
    PlayerState();
};

struct EnemyState : public States {
    int HP;
    bool dead;
    EnemyState();
};

struct BossState : public States {
    int HP;
    int currentWeapon;
    bool dead;
    BossState();
};

struct VehicleState : public States {
    int HP;
    int occupiedByPlayerID;
    bool dead;
    VehicleState();
};

struct BulletState : public States {
    float angle;
    bool active;
    BulletState();
};

struct CurrentState {
    float timestamp;
    PlayerState players[4];
    int playerCount;
    int activePlayerIndex;
    EnemyState enemies[100];
    int enemyCount;
    BossState bosses[4];
    int bossCount;
    VehicleState vehicles[10];
    int vehicleCount;
    BulletState bullets[200];
    int bulletCount;
    int screenLeft;
    int screenRight;
    int screenTop;
    int screenBottom;
    CurrentState();
};



//==================================================================




class GrudgefulSystem {   // <-- added to GameManager 
    bool checkIfPassed(Enemy* enemy, int screenLeft);
    void mark(Enemy* enemy);
    void boost(Enemy* enemy);
    void move(Enemy* enemy, float playerX, float playerY);
protected:
    EnemyManager* enemyManager;
public:
    void update(int screeLeft, float playerX, float playerY);
};

class CowardiceSystem { // <---gameManager 
    bool checkIfIgnored(Enemy* enemy, int screenLeft);
    void addToReserve(Enemy* enemy);
    bool shouldCounterattack();
    void triggerCounterattack();
protected:
    EnemyManager* enemyManager;
    int reserveCount;
public:
    void update(int screenLeft, float playerX, float playerY);
};


class StatesBuffer {
protected:
    CurrentState ring[600];                  // ring buffer holds the last 10 seconds (60fps * 10)
    int writeHead;                           // next slot to write into
    int filled;                              // how many slots have valid data, caps at 600
    ForksOfFateFileHandler* fileHandler;     // also forwards each snapshot to disk
public:
    StatesBuffer(ForksOfFateFileHandler* fh);
    void push(const CurrentState& s);                       // adds to ring + writes file
    const CurrentState& peekRelative(int framesAgo) const;  // 0 = newest, 1 = one frame before, etc
    int available() const { return filled; }               // how many frames we can rewind through
};


class ForksOfFateManager {
protected:
    ForksOfFateFileHandler fileHandler;   // owns the file
    StatesBuffer* buffer;                 // RAM ring buffer
    EntityManager* entityManager;         // who we ask for entities
    bool isReversing;                     // true when R was pressed and rewind is playing
    int rewindCursor;                     // how many frames we've gone back so far during current rewind
    float runTimer;                       // total time elapsed in this run, written to snapshot timestamps
public:
    ForksOfFateManager();
    ~ForksOfFateManager();
    void setEntityManager(EntityManager* em);
    void beginSession(const char* timelineFile);   // opens timeline file at start of run
    void endSession();                             // closes file
    void takeSnapshot();                           // called every frame in normal play
    void startRewind();                            // R pressed, begin going backward
    void stepRewind();                             // called every frame during rewind
    void stopRewind();                             // R pressed again or buffer exhausted
    bool reversing() const { return isReversing; }
};

//================================ Urdu Terminal =====================================

class UrduTerminal {
protected:
    Mainboard* mainboard;
    bool active;
public:
    void RenderTerminal();
    bool getActive();
    void Activate();
    void Deactivate();
};

class Timer {
protected:
    int seconds;
    int minutes;
    int hours;
    bool paused;
public:
    Timer();
    void setTime(int hours, int mins, int seconds);
    int getSec();
    int getMin();
    int getHours();
    void startTimer();
    void pauseTimer();
    void updateTime();
    bool getPaused();
    void resetTimer();
};



class Queue {
private:
    char* buffer;
    int count;
public:
    Queue();
    Queue(int size);
    ~Queue();
    char* getBuffer() const;
    void setBuffer(char* newbuffer);
    int getCount() const;
    void addChar(char* c);
    void pop();
};

class MemoryModule {
private:
    unsigned char memory[3840];
    float powerDraw;
public:
    MemoryModule();
    void loadRawBinary(int index, unsigned short segment);
    unsigned char read(int index);
    void write(int index, unsigned char value);
    float getPowerDraw();
};


class RegisterBank {
private:
    unsigned short registers[10];
    unsigned char Flags;
public:
    RegisterBank();
    unsigned short read(int index) const;
    void write(int index, unsigned short value);
};

struct Cache {
    unsigned char block[16];   //16 bytes
    unsigned short start;
    bool valid;
};

struct SignalVector {
    bool IS_VALID;
    bool USES_AEC;
    int  AEC_OPERATION;
    bool IS_MEMORY_READ;
    bool IS_MEMORY_WRITE;
    bool IS_BRANCH;
    bool BRANCH_ON_ZERO;
    bool BRANCH_ON_NOT_ZERO;
    bool WRITES_TO_REGISTER;
    bool IS_4BIT_IMMEDIATE;
    bool IS_16BIT_IMMEDIATE;
};

class Processor {
private:
    RegisterBank registerBank;
    Cache cache;
    SignalVector signals[256];
    bool plugged;
    Mainboard* mainboard;
    unsigned short* addressBus;
    unsigned char* dataBus;
    unsigned char* controlBus;
    float temperature;
    unsigned short immediate16;
public:
    Processor();
    bool End();
    bool checkFlags();
    unsigned short getCacheStart();
    void invalidateCache();
    void fetch();
    void incrementPC();
    int decode();
    void execute();
    void checkTemperature();
    void plugIn(unsigned short* ab, unsigned char* db, unsigned char* cb, Mainboard* mb);
    void plugOut();
    void loadCache(unsigned short start, unsigned char* block);
    int ALU(unsigned char opcode, unsigned short operandA, unsigned short operandB, int destReg);
    unsigned char readByte(int address);
    unsigned short readRegister(int index) const;
    float getTemperature() const;
};

class PhosphorDisplay {
private:
    char screen[16][32];
    int row;
    int col;
public:
    PhosphorDisplay();
    void insertChar(char c);
    void render();
};


class GraphicsAdapter {
    float powerDraw;
public:
    GraphicsAdapter();
    char CHAR_PAYLOAD(unsigned char byte);
    char* INT_PAYLOAD(unsigned char byte);
    float getPowerDraw();
};


class MyKeyboard {
private:
    Queue buffer;
public:
    bool PENDING_INPUT();
    int READ_CHAR();
    int READ_INT();
};

class PSU {
private:
    float maxWattage;
    Mainboard* mainboard;
public:
    PSU(float maxWattage, Mainboard* m);
    void checkPower(float currentDraw);
};

class Mainboard {
private:
    MemoryModule* memMod;
    Processor* processor;
    GraphicsAdapter* graphicsAdap;
    PhosphorDisplay* display;
    MyKeyboard* keyboard;
    PSU* psu;
    unsigned short addressBus;
    unsigned char dataBus;
    unsigned char controlBus; //0 = idle, 1 = read, 2 = write
    bool powerOn;
public:
    Mainboard();
    void READ(int address);
    void WRITE(int address, int data);
    void loadCache(int address);
    void plugInProcessor(Processor* p);
    void plugOutProcessor();
    void plugInMemory(MemoryModule* m);
    void plugInKeyboard(MyKeyboard* k);
    void plugInDisplay(PhosphorDisplay* d);
    void plugInGraphics(GraphicsAdapter* g);
    void plugInPSU(PSU* p);
    void pulseClock();
    void killPower();
    bool isPowerOn();
    void renderDisplay();
    void displayRegisters();
};

//=========================================================================


//================================= CHAIN OF DEATHS AHAHAHAHAHAH ========================================

class StateManager {
protected:
    StateTable* playerStates[7];
    int ActiveStateIndex;
    EnemyManager* enemyManager;
    VehicleManager* vehicleManager;
    BossManager* bossManager;
    ForksOfFateFileHandler* fileHandler;
public:
    StateManager();
    void updateStates();
    void ActivateState(); // IMPORTANT. this checks previous 4 seconds through files and accordingly sets a state
    void manipulate(); // core of chains of death. checks current state and accordingly manipulates enemies and vehicles and bosses accordingly
};

class StateTable {
protected:
    StateTable* previousState;
    int type; // 0 = press , 1 = consolidate, 2 = Retreat, 3 = burn, 4 = stall, 5 = saving, 6 = AFK
    int totalTransitions; // number of times we have transitioned from this state to another. Used to calculte percentage we increment this whenever theres a transition into this
    int Table[7];
    int scoreGainedInNextState[7]; // this records the score gained in a specific state to determine what the best chain would be --- more score means better chain 
    int scoreGainedInThisState;
    float timeSpendInThisState;
    bool Active;
public:
    StateTable();
    void ActivateState();
    void DeactivateState();
    void changeState(int type);
    bool isActive();
    int getType();
    void incrementTransitions();
    int highestProbability(); // just sees highest from table and returns index
    int bestChain();   // sees highest from score table and returns index
};

class PressingState : public StateTable {
protected:
    // if player has killed 2 enemies in the past 4 seconds and total displacement in the last 4 seconds is > 5 blocks (320 pixels) then this activates
public:
    PressingState();
};

class ConsolidatingState : public StateTable {
protected:
    // not getting damaged and obtaining collectibles
public:
    ConsolidatingState();
};

class RetreatingState : public StateTable {
protected:
    // displacement is negative and damaged
public:
    RetreatingState();
};

class BurningResourcesState : public StateTable {
protected:
    // used 1 grenade and 10+ ammo but no kills 
public:
    BurningResourcesState();
};

class StallingState : public StateTable {
protected:
    //very little displacement and no kills and little utility usages
public:
    StallingState();
};

class SavingState : public StateTable {
protected:
    // not using utilty grenadecount, ammo isnt being used even when enemies are near
public:
    SavingState();
};

class AFKState : public StateTable {
protected:
    // no inputs at all
public:
    AFKState();
};


//======================================================================================================





class GameManager {
protected:
    Recorder fingerprintrecorder;    // ---------added these
    Autopilot* autoPilot;                // pointers because of update() to be called polymorphically
    GhostController* ghostController;        //
    ForksOfFateManager forksOfFateManager;
    DeathCompositor deathCompositor;
    UrduTerminal urduTerminal; // if active then rungamefunction opens a seperate window (a while loop of another window inside the main windows while loop) and there we do the 
    // injection and then when this loop is exited urduTerminal is deavtivated too and timer is unpaused and game continues
    GrudgefulSystem grudgefulSystem;
    CowardiceSystem cowardiceSystem;
    RageBait rageBait;
    StateManager stateManager;
public:
    GameManager();
    // returning by reference (allow read & write)
    Recorder& getFingerprintRecorder(); // these too
    Autopilot& getAutopilot();
    GhostController& getGhostController();
    ForksOfFateManager& getForksOfFateManager();
    DeathCompositor& getDeathCompositor();
    UrduTerminal& getUrduTerminal();
    AudioManager& getAudioManager();
    GrudgefulSystem& getGrudgefulSystem();
    CowardiceSystem& getCowardiceSystem();
    RageBait& getRageBait();
    StateManager& getStateManager();
    ~GameManager();
};


// ================== SCORE MANAGER ==================
class ScoreManager {
protected:
    int score;
    int killCount;
    int campaignKillQuota;
    bool campaignClearAwarded;
    bool survivalClearAwarded;
    int nextExplosionZoneId;
    int  explKillIds[128];
    int  explKillCount[128];
    bool explFlawlessGiven[128];
    int  explSlotsUsed;
    int  currentExplosionZoneId;

    int findOrAllocSlot(int zoneId);
public:
    static const int SCORE_REBEL = 50;
    static const int SCORE_SHIELDED = 75;
    static const int SCORE_BAZOOKA = 100;
    static const int SCORE_GRENADE = 100;
    static const int SCORE_PARA_BONUS = 25;
    static const int SCORE_MUMMY = 150;
    static const int SCORE_ZOMBIE = 100;
    static const int SCORE_MARTIAN = 200;
    static const int SCORE_NORMAL_BOSS = 500;
    static const int SCORE_ULTIMATE_BOSS = 1500;
    static const int SCORE_SURVIVAL_CLEAR = 1000;
    static const int SCORE_CAMPAIGN_CLEAR = 3000;
    static const int SCORE_FLAWLESS = 5000;

    ScoreManager();
    static ScoreManager& instance();

    int  getScore() const { return score; }
    int  getKillCount() const { return killCount; }
    int  getCampaignKillQuota() const { return campaignKillQuota; }

    void reset();
    void addScore(int amount);
    void registerKill(int amount);
    void registerExplosionKill(int zoneId, int amount);
    int  allocateExplosionZoneId();

    void awardSurvivalClear();
    void awardCampaignClear();
    bool campaignQuotaReached() const { return killCount >= campaignKillQuota; }

    void setCurrentExplosionContext(int zoneId) { currentExplosionZoneId = zoneId; }
    void clearCurrentExplosionContext() { currentExplosionZoneId = 0; }
    void noteEnemyKill(int amount);

    void draw();
};

class EntityManager {
protected:
    MyVector<DamageZone*>* damageZones;
    MyVector<Entity*>* pendingSpawns;
    MyVector<Entity*> entities;
    int totalPlayers;
    int totalEnemies;
    int totalVehicles;
    int totalWeapons;
    int totalBullets;
    int totalCollectibles;
    int totalBosses;

    int currentPlayer;
    int playerCount;
    bool playerAlive[4];
    bool zWasPressed;
    bool finalBossWasSpawned;

    bool devMode;
    float devToggleCooldown;
    bool opWasPressed;
    bool vWasPressed;
    bool externalRewindActive;   // when true, runEntities skips everything, ForksOfFateManager drives the frame
public:
    EntityManager();
    void addEntity(Entity* entity);
    void runEntities();
    void removeEntity(int index);
    PlayerSoldier* getCurrentPlayer();
    void setCurrentPlayer(int index);
    void scanAndSpawnEnemies(Level* level);
    void spawnPlayers(Level* level);
    void killFarAwayEnemies(float centerX, float radiusPx);
    void setDamageZones(MyVector<DamageZone*>* dz) { damageZones = dz; }
    void setPendingSpawns(MyVector<Entity*>* ps) { pendingSpawns = ps; }
    void drainPendingSpawns();
    void processDamageZones();
    void sweepDead();
void drawHUD();
    void drawDebug();
    void drawMahoragaOverlay();   // NEW
    ~EntityManager();

    bool isDevMode() const { return devMode; }
    void enableDevMode();
    void checkSwitchingAndBehaviour();
    void switchToNextPlayer(Level* level, bool forced = false);
    bool allPlayersDead() const;
    bool finalBossDefeated();
    void updateFinalBossTracker();
    MyVector<Entity*>& getEntities() { return entities; }   // ForksOfFate iterates over this for snapshots
    void setRewindActive(bool on) { externalRewindActive = on; }
};
class LevelManager {
protected:
    Level* currentLevel;
    MyVector<DamageZone*> damageZones;
    MyVector<Entity*> pendingSpawns;
public:
    LevelManager();
    LevelManager(Level* level);
    void setLevel(Level* level);
    Level* getCurrentLevel();
    void generateLevel();
    void RenderLevel();
    MyVector<DamageZone*>* getDamageZones() { return &damageZones; }
    MyVector<Entity*>* getPendingSpawns() { return &pendingSpawns; }
    ~LevelManager();
};



class GameState {
protected:
    bool modeComplete;
    bool developerMode;
    int currentGameMode;
public:
    GameState();
    void activateDeveloperMode();
    void deactivateDeveloperMode();
    bool getDeveloperMode();
    void setModeComplete();
    bool getModeComplete();
    virtual void runGame() = 0;
    virtual ~GameState();
};

class PlayState : public GameState {
protected:
    EntityManager* entityManager;
    LevelManager* levelManager;
    ForksOfFateManager* forks;   // owns the rewind system for this play session
    bool rWasPressed;            // for reversal

public:
    PlayState();
    virtual ~PlayState();
};

class Survival : public PlayState {
protected:
    int currentLevel;
    void loadLevel(int n);
    void transitionToNextLevel();
public:
    Survival(int level);
    virtual ~Survival();
    virtual void runGame();
};



class Campaign : public PlayState {
protected:
    bool profileSelected;
    int chosenProfile;
    SfmlObject* profileMenu;
public:
    Campaign();
    virtual ~Campaign();
    virtual void runGame();
    void selectProfileScreen();
    void buildLevel();
};

class SelfPlay : public PlayState {
protected:
    NEATController neatController;
    Level* currentLevel; // this'll be the basic gauntlent level
    bool bestPlayerActive; // 'B' toggle
public:
    SelfPlay();
    virtual ~SelfPlay();
    void toggleBestPlayer(); //neatController.activateBestPlayer()
    virtual void runGame();
};


class MainMenu {
    int selectedOption; // 0 = Gauntlet , 1 = Campaign  , 2 = self-play
    bool detectedPopup; // if active displayMenu function displays popup
    SfmlObject* menu;
    SfmlObject* popup;
public:
    MainMenu();
    void displayMenu();
    void selectOption(int option); // this will set the selectedOption variable and will be used in the main game loop to determine which game mode to run
    int returnSelectedOption(); // this will return the selectedOption variable and will be used in the main game loop to determine which game mode to run
    ~MainMenu();
};


class GameStateManager {
protected:
    GameState* currentState; // gamemode
    MainMenu mainMenu;
    int currentGameMode; // 0 = game startup, 1 = survival level 1, 2 = survival level 2 , 3 = survival level 3 , 4 = boss level, 5 = campaign , 6 = selfPlay(?)
    //   SessionManager sessionManager;   //// <-- added this  , ye manager class me isliye nhi he cuz it needs to start at the start of game
public:
    GameStateManager();
    ~GameStateManager();
    void setGameState(int gameMode); // this will set the currentState pointer to the corresponding game mode class based on the gameMode parameter
    void run(); // checks the currentGameMode variable and runs the corresponding game mode by calling the runGame function of GameState
    // class if its mainmenu it will call the displayMenu function of the MainMenu class and then call returnSelectedOption to 
    // determine which game mode to run
};


class Game { // MetalSlugGame
protected:
    GameStateManager* stateManager;
    RenderWindow window;
public:
    Game();
    void run();
    ~Game();
};



