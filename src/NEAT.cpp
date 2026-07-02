#include "project.h"

NEATController::NEATController(): population(nullptr), currentNetwork(nullptr),currentPlayer(nullptr), tracker(nullptr), activeLevel(nullptr),episodeTimer(0.0f), generation(1), genomeFitness(0.0f) {
}

NEATController::~NEATController() {
    if (population != nullptr) { 
	delete population; population = nullptr; 
	}
    if (currentNetwork != nullptr) { 
	delete currentNetwork; currentNetwork = nullptr; 
	 }
    if (tracker != nullptr) {
	 delete tracker; tracker = nullptr;
	 }
}


float* NEATController::gatherInputs() {
    float inputs[10];
    if (currentPlayer == nullptr) return inputs;

    inputs[0] = currentPlayer->getX() / (200.0f * cell_size);
    inputs[1] = currentPlayer->getY() / (40.0f * cell_size);
    inputs[2] = currentPlayer->getVelocityX() / currentPlayer->getMaxSpeed();
    inputs[3] = currentPlayer->OnGround() ? 1.0f : 0.0f;
    inputs[4] = (float)currentPlayer->getHP() / 100.0f;
    inputs[5] = currentPlayer->getFacingRight() ? 1.0f : 0.0f;
    inputs[6] = currentPlayer->inAir() ? 1.0f : 0.0f;
    inputs[7] = (float)currentPlayer->getAndSetInventory().currentWeapon / 5.0f;
    inputs[8] = 0.5f;
    inputs[9] = 0.5f;
    return inputs;
}

void NEATController::applyOutput(float* output) {
    if (currentPlayer == nullptr) return;

    float moveX = 0;
    if (output[0] > 0.6f)       
	moveX = 1.0f;   // move right
    else if (output[0] < 0.4f)  
	moveX = -1.0f;  // move left

    bool jump = output[1] > 0.7f;
    bool shoot = output[2] > 0.6f;
    bool grenade = output[3] > 0.85f;

    currentPlayer->setAIInputs(moveX, jump, shoot, grenade);
}


float NEATController::calculateFitness() {
    if (currentPlayer == nullptr) return 0;
    return currentPlayer->getX() / (float)cell_size;
}

void NEATController::randomizeSeed(int seed) {
    srand(seed);
    for (int i = 0; i < 4; i++)
        weights[i] = (float)(rand() % 100) / 100.0f;
}

SelfPlay::SelfPlay() : bestPlayerActive(false), currentLevel(nullptr), neatController(nullptr) {
    levelManager->setLevel(new SurvivalLevel(false, 1));
    levelManager->generateLevel();
    levelManager->getCurrentLevel()->setDamageZones(levelManager->getDamageZones());
    entityManager->setDamageZones(levelManager->getDamageZones());
    levelManager->getCurrentLevel()->setPendingSpawns(levelManager->getPendingSpawns());
    entityManager->setPendingSpawns(levelManager->getPendingSpawns());
    entityManager->spawnPlayers(levelManager->getCurrentLevel());
    levelManager->getCurrentLevel()->setCurrentPlayer(entityManager->getCurrentPlayer());

    neatController = new NEATController();
    neatController->randomizeSeed(rand());
}

SelfPlay::~SelfPlay() {
    if (neatController != nullptr) {
        delete neatController;
        neatController = nullptr;
    }
}

void SelfPlay::toggleBestPlayer() {
    bestPlayerActive = !bestPlayerActive;
}

void SelfPlay::runGame() {
    levelManager->getCurrentLevel()->scrollLevel();
    levelManager->RenderLevel();
    entityManager->scanAndSpawnEnemies(levelManager->getCurrentLevel());
    entityManager->killFarAwayEnemies(
        entityManager->getCurrentPlayer()->getX(),
        50.0f * levelManager->getCurrentLevel()->getMapAt(0)->getCellSize()
    );

    // drive player with NEAT
    PlayerSoldier* p = entityManager->getCurrentPlayer();
    if (p != nullptr) {
        p->setAIControlled(true);
        neatController->currentPlayer = p;
        neatController->activeLevel = levelManager->getCurrentLevel();
        neatController->runGeneration();
    }

    entityManager->runEntities();
    entityManager->drawHUD();
    entityManager->drawDebug();

    // simple top left overlay
    if (p != nullptr) {
        // right side info panel
        RectangleShape panel;
        panel.setSize(Vector2f(260, 130));
        panel.setFillColor(Color(0, 0, 0, 170));
        panel.setOutlineColor(Color::Yellow);
        panel.setOutlineThickness(1.5f);
        panel.setPosition(screen_x - 275, 10);
        window->draw(panel);

        Text t;
        t.setFont(hudFont);
        t.setCharacterSize(17);
        t.setFillColor(Color::Yellow);
        t.setString(
            "  NEAT SELF-PLAY\n"
            "  Generation : " + to_string(neatController->generation) + "\n" +
            "  Episode    : " + to_string((int)neatController->episodeTimer) + "s / 15s\n" +
            "  Fitness    : " + to_string((int)neatController->genomeFitness) + "\n" +
            "  HP         : " + to_string(p->getHP()) + "\n" +
            "  Lives      : " + to_string(p->getLives())
        );
        t.setPosition(screen_x - 268, 16);
        window->draw(t);
    }
}

Population::Population(int size, int inputCount, int outputCount): speciesCount(0), speciesCapacity(0), allSpecies(nullptr),
    genomeCount(0), allGenomes(nullptr), currentGeneration(0),
    globallyBest(inputCount, outputCount), registry(100) {
}

Population::~Population() {}

NEATtrackerHelper::NEATtrackerHelper(int total)
    : currentIndex(0), totalGenomes(total), episodeTime(0),
    maxEpisodeTime(30.0f), episodeRunning(false) {
}

// ------Genome-------
Genome::Genome(int inputCount, int outputCount): nodes(nullptr), connections(nullptr),
    nodeCount(0), connectionsCount(0), fitness(0.0f) {
}

Genome::Genome(const Genome& copy) : nodes(nullptr), connections(nullptr),nodeCount(copy.nodeCount), connectionsCount(copy.connectionsCount),
    fitness(copy.fitness) {
}

Genome::~Genome() {
    if (nodes != nullptr) { 
	delete[] nodes; 
	nodes = nullptr; 
	}
    if (connections != nullptr) { 
	delete[] connections; 
	connections = nullptr; 
	}
}

float Genome::getFitness() const { 
	return fitness; 
	}
int Genome::getNodeCount() const {
	 return nodeCount; 
	}
int Genome::getConnectionCount() const { 
	return connectionsCount;
	 }
NodeGene* Genome::getNodes() { 
	return nodes; 
	}
ConnectionGene* Genome::getConnections() { 
	return connections; 
	}
void Genome::addNode(InnovationRegistry& registry) {}
void Genome::addConnection(InnovationRegistry& registry) {}
void Genome::tuneWeights() {}
Genome Genome::crossOver(Genome& other) {
	 return Genome(*this); 
	}
float Genome::returnCompatibilityDistance(const Genome& other) const { 
	return 0.0f; 
	}

// ----Innovation Reg--------
InnovationRegistry::InnovationRegistry(int count)
    : globalInnovations(nullptr), count(0) {
}

InnovationRegistry::~InnovationRegistry() {
    if (globalInnovations != nullptr) {
        delete[] globalInnovations;
        globalInnovations = nullptr;
    }
}

int InnovationRegistry::getInnovationCount(int from, int to) { 
	return 0;
	 }
void InnovationRegistry::reset() {}

// ============= NEURAL NETWORK =============
NeuralNetwork::NeuralNetwork(Genome* other) : source(other) {}

NeuralNetwork::~NeuralNetwork() {}

void NeuralNetwork::takeInputs(float* inputs, int count) {}
void NeuralNetwork::propagate() {}
float* NeuralNetwork::getOutput() {
    float out[4] = { 0.5f, 0.0f, 0.0f, 0.0f };
    return out;
}

// ---Node Gene-----
NodeGene::NodeGene(int id, int type) : id(id), type(type), value(0.0f) {}
float NodeGene::getValue() const { return value; }
void NodeGene::setValue(float v) { value = v; }

// ---Connection gene-----
ConnectionGene::ConnectionGene(int from, int to, float weight, int innovation): weight(weight), isEnabled(true) {
    data.setFromNode(from);
    data.setToNode(to);
    data.setInnovatioNum(innovation);
}

int ConnectionGene::getFromNode() const { 
	return data.getfromNode(); 
	}
int ConnectionGene::getToNode() const { 
	return data.getToNode(); 
	}
float ConnectionGene::getWeight() const { 
	return weight; 
	}
bool ConnectionGene::getIsEnabled() const {
	 return isEnabled;
	 }

// ---Connection Base------
int ConnectionBase::getfromNode() const {
	 return fromNode; 
	}
int ConnectionBase::getToNode() const {
	 return toNode;
	 }
int ConnectionBase::getInnovationNumber() const { 
	return innovationNumber; 
	}
void ConnectionBase::setFromNode(int f) { 
	fromNode = f;
	 }
void ConnectionBase::setToNode(int t) { 
	toNode = t; 
	}
void ConnectionBase::setInnovatioNum(int n) {
	 innovationNumber = n; 
	}

// ============= SPECIES =============
Species::Species(Genome& first): similarGenomes(nullptr), genomeCount(0),capacity(0), tester(first), bestFitness(0.0f),
    generationsWithNoImprovement(0) {
}

Species::~Species() {
    if (similarGenomes != nullptr) {
        delete[] similarGenomes;
        similarGenomes = nullptr;
    }
}

bool Species::isCompatible(const Genome& other) { return false; }
void Species::adjustFitness() {}
void Species::reproduce() {}

// ----Population // stubbed temporarily
//Population::Population(int size, int inputCount, int outputCount)
//    : speciesCount(0), speciesCapacity(0), allSpecies(nullptr),
//    genomeCount(0), allGenomes(nullptr), currentGeneration(0),
//    globallyBest(inputCount, outputCount), registry(100) {
//}
//
//Population::~Population() {
//    if (allSpecies != nullptr) { 
	delete[] allSpecies; 
	allSpecies = nullptr; 
	}
//    if (allGenomes != nullptr) { 
	delete[] allGenomes; 
	allGenomes = nullptr;
	 }
//}

void Population::speciate() {}
void Population::evaluate() {}
void Population::reproduce() {}
void Population::generationLoop() {}

// ============= NEAT TRACKER =============
//NEATtrackerHelper::NEATtrackerHelper(int total)
//    : currentIndex(0), totalGenomes(total), episodeTime(0),
//    maxEpisodeTime(30.0f), episodeRunning(false) {
//}

bool NEATtrackerHelper::hasNext() { return false; }
int NEATtrackerHelper::getCurrentIndex() { return currentIndex; }
void NEATtrackerHelper::startEpisode() { episodeRunning = true; episodeTime = 0; }
void NEATtrackerHelper::endEpisode() { episodeRunning = false; currentIndex++; }
bool NEATtrackerHelper::shouldTerminate() { return episodeTime >= maxEpisodeTime; }

// ---controller----------
//NEATController::NEATController()
//    : population(nullptr), currentNetwork(nullptr),
//    currentPlayer(nullptr), tracker(nullptr), activeLevel(nullptr) {
//}
//
//NEATController::~NEATController() {
//    if (population != nullptr) { 
//	delete population; population = nullptr;
//	 }
//    if (currentNetwork != nullptr) { 
//	delete currentNetwork; 
//	currentNetwork = nullptr;
//	 }
//    if (tracker != nullptr) { 
//	delete tracker; 
//	tracker = nullptr;
//	 }
//}

void NEATController::runGeneration() {
    if (currentPlayer == nullptr) return;

    episodeTimer += deltaTime;
    genomeFitness = calculateFitness();

    PlayerSoldier* p = currentPlayer;
    Level* lvl = activeLevel;

    // find nearest enemy by scanning level entities
    float nearestEnemyX = -1;
    float nearestEnemyY = -1;
    float nearestDist = 9999;

    // scan pending + existing entities via level
    // simplest: just use player position heuristics

    float playerX = p->getX();
    float playerY = p->getY();

    float moveX = 0;
    bool jump = false;
    bool shoot = false;
    bool grenade = false;

    // always move right (fitness = distance traveled)
    moveX = 1.0f;

    // check wall ahead — if blocked, jump
    p->updateTileChecks(p->getX(), p->getY());
    if (p->horizontalCollisionCheck(' ', p->getMidRight(), p->getBottomRight())) {
        jump = true;
    }

    // shoot every 0.5 seconds
    static float shootTimer = 0;
    shootTimer += deltaTime;
    if (shootTimer >= 0.5f) {
        shoot = true;
        shootTimer = 0;
    }

    // grenade occasionally
    static float grenadeTimer = 0;
    grenadeTimer += deltaTime;
    if (grenadeTimer >= 5.0f) {
        grenade = true;
        grenadeTimer = 0;
    }

    // if falling into pit, jump
    if (p->inAir() && p->getVelocityY() > 5.0f) {
        jump = true;
    }

    p->setAIInputs(moveX, jump, shoot, grenade);

    // reset every 15 seconds
    if (episodeTimer >= 15.0f) {
        episodeTimer = 0.0f;
        generation++;
        generationJustReset = true;
    }
    else {
        generationJustReset = false;
    }
}
void NEATController::saveBestGenome() {}
void NEATController::loadBestGenome() {}
void NEATController::activateBestPlayer() {}
//float* NEATController::gatherInputs() { return nullptr; }
//void NEATController::applyOutput(float* output) {}
//float NEATController::calculateFitness() { return 0.0f; }