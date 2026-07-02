#include "project.h"


// ===================== MAP =====================

Map::Map(int height, int width) : cell_size(::cell_size), height(height), width(width) {
    this->lvl = new char* [height];
    for (int i = 0; i < height; i++) {
        this->lvl[i] = new char[width];
        for (int j = 0; j < width; j++) {
            this->lvl[i][j] = ' ';
        }
    }
    this->blocks = nullptr;
    this->chunkOriginX = 0;
}

char**& Map::getLvl() {
    return this->lvl;
}

void Map::setTile(int x, int y, char tile) {
    this->lvl[y][x] = tile;
}

int Map::getCellSize() {
    return this->cell_size;
}

int Map::getHeight() {
    return this->height;
}

int Map::getWidth() {
    return this->width;
}

long long Map::getChunkOriginX() const { return chunkOriginX; }
void Map::setChunkOriginX(long long x) { chunkOriginX = x; }

Map::~Map() {
    for (int i = 0; i < this->height; i++) {
        delete[] this->lvl[i];
    }
    delete[] this->lvl;
    delete[] this->blocks;
}


// ===================== LEVEL =====================


Level::Level() : currentPlayer(nullptr), grassBlock(new SfmlObject), bedrock(new SfmlObject),
dirtBlock(new SfmlObject), water(new SfmlObject), background(new SfmlObject),
levelComplete(false), screenLeft(0), screenRight(screen_x), screenTop(0),
screenBottom(screen_y), isRaining(false), damageZones(nullptr), pendingSpawns(nullptr) {
    this->map[0] = nullptr;
    this->map[1] = nullptr;
    this->map[2] = nullptr;
    this->timeTillRain = (float)(rand() % 60 + 30);
    this->rainDuration = (float)(rand() % 30 + 10);
}

Level::Level(PlayerSoldier* currentPlayer) : currentPlayer(currentPlayer), grassBlock(new SfmlObject),
bedrock(new SfmlObject), dirtBlock(new SfmlObject), water(new SfmlObject), background(new SfmlObject),
levelComplete(false), screenLeft(0), screenRight(screen_x), screenTop(0),
screenBottom(screen_y), isRaining(false), damageZones(nullptr), pendingSpawns(nullptr) {
    this->map[0] = nullptr;
    this->map[1] = nullptr;
    this->map[2] = nullptr;
    this->timeTillRain = (float)(rand() % 60 + 30);
    this->rainDuration = (float)(rand() % 30 + 10);
}

MyString Level::getDisplayName() const {
    return MyString("LEVEL");
}

MyString SurvivalLevel::getDisplayName() const {
    if (isBossLevel)
    {
        return MyString("BOSS");
    }
    MyString result = "LEVEL ";
    char buffer[2];
    int n = currentLevel;
    buffer[0] = '0' + n;
    buffer[1] = '\0';
    result += buffer;
    return result;
}

MyString InfiniteLevel::getDisplayName() const {
    return MyString("CAMPAIGN");
}


Map* Level::getMap() {
    return map[1];
}

int Level::getMapCount() {
    return 1;
}

Map* Level::getMapAt(int i) {
    return map[1];
}

void Level::updateRain() {
    // fill later
}

bool Level::getIsRaining() {
    return this->isRaining;
}

void Level::setCurrentPlayer(PlayerSoldier* player) {
    this->currentPlayer = player;
}

void Level::scrollLevel() {
	if (currentPlayer == nullptr) {
		return;
	}


	// horizontal
	if (currentPlayer->getX() < screenLeft + (screen_x / 2.5)) {
		screenLeft = currentPlayer->getX() - (float)screen_x/2.5;
		screenRight = screenLeft + screen_x;
	}
	else if (currentPlayer->getX() > screenRight - (screen_x / 2.5)) {
		screenRight = currentPlayer->getX() + (float)screen_x / 2.5;
		screenLeft = screenRight - screen_x;
	}

	// vertical
	if (currentPlayer->getY() < screenTop + (screen_y / 2)) {
		screenTop = currentPlayer->getY() - (float)screen_y / 2;
		screenBottom = screenTop + screen_y;
	}
	else if (currentPlayer->getY() > screenBottom - (screen_y / 2)) {
		screenBottom = currentPlayer->getY() + (float)screen_y / 2;
		screenTop = screenBottom - screen_y;
	}

    // clamp to map bounds
    if (screenLeft < 0) { screenLeft = 0; screenRight = screen_x; }
    if (screenRight > map[1]->getWidth() * map[1]->getCellSize()) {
        screenRight = map[1]->getWidth() * map[1]->getCellSize();
        screenLeft = screenRight - screen_x;
    }
    if (screenTop < 0) { screenTop = 0; screenBottom = screen_y; }
    if (screenBottom > map[1]->getHeight() * map[1]->getCellSize()) {
        screenBottom = map[1]->getHeight() * map[1]->getCellSize();
        screenTop = screenBottom - screen_y;
    }
}

void Level::addDamageZone(DamageZone* zone) {
    if (damageZones != nullptr) damageZones->add(zone);
}

void Level::queueSpawn(Entity* e) {
    if (pendingSpawns != nullptr) {
        pendingSpawns->add(e);
    }
}

Level::~Level() {
    delete this->grassBlock;
    delete this->background;
    delete this->dirtBlock;
    delete this->bedrock;
    delete this->water;
    for (int i = 0; i < 3; i++) {
        delete this->map[i];
    }
}

void Level::placeVehicleMarkers(char**& lvl, int W, int H) {
    int runStart = 0;
    int runType = -1; // 0 = plains, 1 = aerial, 2 = aquatic

    for (int c = 0; c <= W; c++) {
        int colType = -1;
        int srow = -1;

        if (c < W) {
            bool hasWater = false;
            for (int r = 0; r < H; r++) {
                if (lvl[r][c] == '~') { hasWater = true; break; }
            }
            if (hasWater) colType = 2;
            else {
                int sr = surfaceRow(lvl, c);
                int height = H - sr;
                srow = sr;
                if (height > 20) colType = 1;
                else colType = 0;
            }
        }

        bool boundary = (c == W) || (colType != runType);
        if (boundary && runType != -1) {
            int runWidth = c - runStart;
            if (runWidth >= 20) {
                int mid = (runStart + c - 1) / 5;
                bool amphigousSlug = rand() % 5;
                amphigousSlug = !amphigousSlug;
                if (runType == 0) {
                    int sr = surfaceRow(lvl, mid);
                    if (sr - 1 >= 0) lvl[sr - 1][mid] = 'L';
                    if (amphigousSlug) lvl[sr - 1][mid] = 'C';
                }
                else if (runType == 1) {
                    int sr = surfaceRow(lvl, mid);
                    if (sr - 2 >= 0) lvl[sr - 2][mid] = 'Y';
                    if (amphigousSlug) lvl[sr - 2][mid] = 'C';
                }
                else if (runType == 2) {
                    int waterTop = -1;
                    for (int r = 0; r < H; r++) {
                        if (lvl[r][mid] == '~') { waterTop = r; break; }
                    }
                    if (waterTop >= 0) lvl[waterTop][mid] = 'Q';
                    if ((waterTop >= 0)&&amphigousSlug) lvl[waterTop][mid] = 'C';
                }
            }
            runStart = c;
            runType = colType;
        }
        else if (runType == -1) {
            runStart = c;
            runType = colType;
        }
    }
}

// Getters
int Level::getScreenLeft() const {
    return screenLeft;
}

int Level::getScreenRight() const {
    return screenRight;
}

int Level::getScreenTop() const {
    return screenTop;
}

int Level::getScreenBottom() const {
    return screenBottom;
}

// Setters
void Level::setScreenLeft(int value) {
    screenLeft = value;
}

void Level::setScreenRight(int value) {
    screenRight = value;
}

void Level::setScreenTop(int value) {
    screenTop = value;
}

void Level::setScreenBottom(int value) {
    screenBottom = value;
}

PlayerSoldier* Level::getCurrentPlayer()
{
    return this->currentPlayer;
}

int Level::getHeight(long long x) {
    return 35;
}

char Level::tileAtWorld(float worldX, float worldY) {
    Map* m = map[1];
    if (m == nullptr) return ' ';
    int cs = m->getCellSize();
    int W = m->getWidth();
    int H = m->getHeight();
    int col = (int)(worldX / cs);
    int row = (int)(worldY / cs);
    if (col < 0) col = 0;
    if (col >= W) col = W - 1;
    if (row < 0) row = 0;
    if (row >= H) row = H - 1;
    return m->getLvl()[row][col];
}

void Level::fillMapFromSeed(char**& lvl, int seed, int profileChoice) {
    NoiseProfileFactory factory;
    factory.selectProfile(profileChoice);
    FractalNoiseGenerator gen(seed);

    int heights[200];
    gen.generateSummedHeights(0, heights, factory.getProfile());

    for (int r = 0; r < 40; r++)
        for (int c = 0; c < 200; c++)
            lvl[r][c] = ' ';

    for (int c = 0; c < 200; c++) {
        int h = heights[c];
        int surfaceRow = 40 - h;
        if (surfaceRow < 0) surfaceRow = 0;
        if (surfaceRow > 39) surfaceRow = 39;

        lvl[39][c] = 'X';
        for (int r = 38; r > surfaceRow; r--) lvl[r][c] = 'D';

        if (h <= 5) {
            for (int r = 35; r <= 38; r++) lvl[r][c] = '~';
        }
        else {
            lvl[surfaceRow][c] = '#';
        }
    }
}

int Level::surfaceRow(char**& lvl, int col) {
    for (int r = 0; r < 40; r++) {
        if (lvl[r][col] != ' ') return r;
    }
    return 39;
}


// ===================== SURVIVAL LEVEL =====================                                      ####
//                                                                       ########   #######    #############
//                                                                     ###########################################
//                                                                  ###                                           #####
//                                                              ####                                                    ###
//                                                         #####                                                           ##
//                        ########      ######      #######                                                                  ##
//     #######      ######        ######      ######                                                                           ##
//#####       ######                                                                                                             ##
//                                                                                                                                 ##
//                                                                                                                                   WWWWWWWWWWWWWWWWWWWWWWWWWWWWW
//
//
//

SurvivalLevel::SurvivalLevel(bool isBossLevel, int level) : Level(), isBossLevel(isBossLevel), currentLevel(level) {
    this->map[1] = (level == 4) ? new Map(40, 250) : new Map(40, 200);
	this->dirtBlock->getTex().loadFromFile("Sprites/blocks/dirt.png");
	this->bedrock->getTex().loadFromFile("Sprites/blocks/andesite.png");
	this->grassBlock->getTex().loadFromFile("Sprites/blocks/grass_block_side.png");
	this->water->getTex().loadFromFile("Sprites/blocks/water.png");
	this->background->getTex().loadFromFile("Sprites/backGround2.png");
	this->background->setScaleX((float)(screen_x) / this->background->getTex().getSize().x);
	this->background->setScaleY((float)(screen_y) / this->background->getTex().getSize().y);

	this->background->initializeScale();
    this->background->initializeDimensions();

    this->dirtBlock->initializeScale();
    this->dirtBlock->initializeDimensions();

    this->grassBlock->initializeScale();
    this->grassBlock->initializeDimensions();

    this->water->initializeScale();
    this->water->initializeDimensions();

    this->bedrock->initializeScale();
    this->bedrock->initializeDimensions();

    screenTop = (35*64 - 128) - (screen_y / 2);   // 2112 - 450 = 1662   
    screenBottom = screenTop + screen_y;     // 1662 + 900 = 2562
}


SurvivalLevel::SurvivalLevel(bool isBossLevel, PlayerSoldier* currentPlayer, int level) : Level(currentPlayer), isBossLevel(isBossLevel), currentLevel(level) {
    this->map[1] = (level == 4) ? new Map(40, 250) : new Map(40, 200);
    this->dirtBlock->getTex().loadFromFile("Sprites/blocks/dirt.png");
    this->bedrock->getTex().loadFromFile("Sprites/blocks/andesite.png");
    this->grassBlock->getTex().loadFromFile("Sprites/blocks/grass_block_side.png");
    this->water->getTex().loadFromFile("Sprites/blocks/water.png");
    this->water->setScaleX(64.0f / 304.0f);
    this->water->setScaleY(64.0f / 303.0f);
    this->background->getTex().loadFromFile("Sprites/backGround2.png");
    this->background->setScaleX((float)(screen_x) / this->background->getTex().getSize().x);
    this->background->setScaleY((float)(screen_y) / this->background->getTex().getSize().y);

    this->background->initializeScale();
    this->background->initializeDimensions();

    this->dirtBlock->initializeScale();
    this->dirtBlock->initializeDimensions();

    this->grassBlock->initializeScale();
    this->grassBlock->initializeDimensions();

    this->water->initializeScale();
    this->water->initializeDimensions();

    this->bedrock->initializeScale();
    this->bedrock->initializeDimensions();

    screenTop = (35 * 64 - 128) - (screen_y / 2);   // 2112 - 450 = 1662
    screenBottom = screenTop + screen_y;     // 1662 + 900 = 2562
}


void SurvivalLevel::checkCompletion(EntityManager* em) {
    if (levelComplete) return;
    if (currentPlayer == nullptr) return;

    if (isBossLevel) {
        if (em->finalBossDefeated()) levelComplete = true;
    }
    else {
        float mapRightPx = (float)(map[1]->getWidth() * map[1]->getCellSize());
        float threshold = (float)map[1]->getCellSize();
        if (currentPlayer->getX() >= mapRightPx - threshold) {
            levelComplete = true;
        }
    }
}


void SurvivalLevel::generateMap() {
    char**& lvl = this->map[1]->getLvl();
    int W = this->map[1]->getWidth();
    int H = this->map[1]->getHeight();
    if (currentLevel == 1) {
        // --- fill everything with air ---
        for (int r = 0; r < H; r++)
            for (int c = 0; c < W; c++)
                lvl[r][c] = ' ';



        for (int c = 0; c <= 66; c++)
        {
            lvl[39][c] = 'X';
            lvl[38][c] = 'D';
            lvl[37][c] = 'D';
            lvl[36][c] = 'D';
            lvl[35][c] = '#';
        }

        // --- raised ground sections (every ~7 cols, 1-2 blocks high) ---

        // raise 1: cols 8-12,  1 block high (row 34)
        for (int c = 8; c <= 12; c++) lvl[34][c] = '#';

        // raise 2: cols 20-25, 2 blocks high (rows 33-34)
        for (int c = 20; c <= 25; c++) { lvl[34][c] = '#'; lvl[33][c] = '#'; }

        // raise 3: cols 33-37, 1 block high
        for (int c = 33; c <= 37; c++) lvl[34][c] = '#';

        // raise 4: cols 46-51, 2 blocks high
        for (int c = 46; c <= 51; c++) { lvl[34][c] = '#'; lvl[33][c] = '#'; }

        // raise 5: cols 58-62, 1 block high
        for (int c = 58; c <= 62; c++) lvl[34][c] = '#';

        // FIX 1 — in-air platforms are now 4 rows above ground (row 31)
        // instead of rows 33-34 which was only 1-2 blocks up
        // row 31 = 4 clear air rows above ground row 35 = visibly floating

        // air platform 1: cols 15-18, row 31
        for (int c = 15; c <= 18; c++) lvl[31][c] = '#';

        // air platform 2: cols 40-43, row 31
        for (int c = 40; c <= 43; c++) lvl[31][c] = '#';

        // air platform 3: cols 54-57, row 31
        for (int c = 54; c <= 57; c++) lvl[31][c] = '#';



        int groundRow = 35;  // matches plains ground

        // -- upward slant cols 67-88 --
        // pattern per group of 3: '/' drops groundRow by 1, then 2 flat '#'

        // group 1: col 67 = '/', cols 68-69 = '#'
        lvl[groundRow][67] = '#';
        for (int r = groundRow + 1; r <= 38; r++) lvl[r][67] = 'D'; lvl[39][67] = 'X';
        groundRow--;
        lvl[groundRow][68] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][68] = 'D'; lvl[39][68] = 'X';
        lvl[groundRow][69] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][69] = 'D'; lvl[39][69] = 'X';

        // group 2
        lvl[groundRow][70] = '#';
        for (int r = groundRow + 1; r <= 38; r++) lvl[r][70] = 'D'; lvl[39][70] = 'X';
        groundRow--;
        lvl[groundRow][71] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][71] = 'D'; lvl[39][71] = 'X';
        lvl[groundRow][72] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][72] = 'D'; lvl[39][72] = 'X';

        // group 3
        lvl[groundRow][73] = '#';
        for (int r = groundRow + 1; r <= 38; r++) lvl[r][73] = 'D'; lvl[39][73] = 'X';
        groundRow--;
        lvl[groundRow][74] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][74] = 'D'; lvl[39][74] = 'X';
        lvl[groundRow][75] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][75] = 'D'; lvl[39][75] = 'X';

        // group 4
        lvl[groundRow][76] = '#';
        for (int r = groundRow + 1; r <= 38; r++) lvl[r][76] = 'D'; lvl[39][76] = 'X';
        groundRow--;
        lvl[groundRow][77] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][77] = 'D'; lvl[39][77] = 'X';
        lvl[groundRow][78] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][78] = 'D'; lvl[39][78] = 'X';

        // group 5
        lvl[groundRow][79] = '#';
        for (int r = groundRow + 1; r <= 38; r++) lvl[r][79] = 'D'; lvl[39][79] = 'X';
        groundRow--;
        lvl[groundRow][80] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][80] = 'D'; lvl[39][80] = 'X';
        lvl[groundRow][81] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][81] = 'D'; lvl[39][81] = 'X';

        // group 6
        lvl[groundRow][82] = '#';
        for (int r = groundRow + 1; r <= 38; r++) lvl[r][82] = 'D'; lvl[39][82] = 'X';
        groundRow--;
        lvl[groundRow][83] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][83] = 'D'; lvl[39][83] = 'X';
        lvl[groundRow][84] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][84] = 'D'; lvl[39][84] = 'X';

        // group 7
        lvl[groundRow][85] = '#';
        for (int r = groundRow + 1; r <= 38; r++) lvl[r][85] = 'D'; lvl[39][85] = 'X';
        groundRow--;
        lvl[groundRow][86] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][86] = 'D'; lvl[39][86] = 'X';
        lvl[groundRow][87] = '#'; for (int r = groundRow + 1; r <= 38; r++) lvl[r][87] = 'D'; lvl[39][87] = 'X';

        // final slant col 88, then bridge remaining rise to row 20
        lvl[groundRow][88] = '#';
        for (int r = groundRow + 1; r <= 38; r++) lvl[r][88] = 'D'; lvl[39][88] = 'X';
        groundRow--;

        // bridge cols 89-95: pure '/' to finish climbing to row 20
        for (int c = 89; c <= 95; c++)
        {
            lvl[groundRow][c] = '#';
            for (int r = groundRow + 1; r <= 38; r++) lvl[r][c] = 'D';
            lvl[39][c] = 'X';
            groundRow--;
        }

        int mountRow = groundRow;   // = 20

        // FIX 2 — flat top is now cols 96-118 (23 cols of room to move)
        for (int c = 96; c <= 118; c++)
        {
            lvl[mountRow][c] = '#';
            for (int r = mountRow + 1; r <= 38; r++) lvl[r][c] = 'D';
            lvl[39][c] = 'X';
        }

        // raised sections on mountain top (small, 1 block)
        for (int c = 101; c <= 104; c++) lvl[mountRow - 1][c] = '#';
        for (int c = 112; c <= 115; c++) lvl[mountRow - 1][c] = '#';

        // in-air platforms on mountain — 4 rows above mountRow (row 16)
        for (int c = 98; c <= 101; c++) lvl[mountRow - 4][c] = '#';
        for (int c = 109; c <= 113; c++) lvl[mountRow - 4][c] = '#';

        // -- downward slant cols 119-133 --
        // must end at row 34 (one above water surface 35)
        // mountRow = 20, target = 34, difference = 14 rows over 15 cols (119-133)
        groundRow = mountRow;  // = 20

        for (int c = 119; c <= 133; c++)
        {
            lvl[groundRow][c] = '#';
            for (int r = groundRow + 1; r <= 38; r++) lvl[r][c] = 'D';
            lvl[39][c] = 'X';
            groundRow++;
        }
        // groundRow is now exactly 35 after 15 increments (20+15=35)
        // shore sits at row 34 = one above water
        int shoreRow = 34;



        // --- left shore (cols 134-140) ---
        for (int c = 134; c <= 140; c++)
        {
            lvl[39][c] = 'X';
            for (int r = shoreRow + 1; r <= 38; r++) lvl[r][c] = 'D';
            lvl[shoreRow][c] = '#';
        }

        // --- water body (cols 141-191) ---
        for (int c = 141; c <= 191; c++)
        {
            lvl[39][c] = 'X';
            lvl[38][c] = '~';
            lvl[37][c] = '~';
            lvl[36][c] = '~';
            lvl[35][c] = '~';
        }

        // platforms ON water surface (row 34 = one above water surface row 35)
        for (int c = 148; c <= 153; c++) lvl[34][c] = '#';
        for (int c = 161; c <= 166; c++) lvl[34][c] = '#';
        for (int c = 174; c <= 179; c++) lvl[34][c] = '#';
        for (int c = 183; c <= 188; c++) lvl[34][c] = '#';

        // --- right shore (cols 192-199) ---
        for (int c = 192; c <= 199; c++)
        {
            lvl[39][c] = 'X';
            for (int r = shoreRow + 1; r <= 38; r++) lvl[r][c] = 'D';
            lvl[shoreRow][c] = '#';
        }
        //some error fixed 
        for (int i = 20; i <= 25; i++)
            lvl[33][i] = ' ';

        for (int i = 15; i <= 18; i++) {
            lvl[31][i] = ' ';
            lvl[33][i] = '#';
        }
        lvl[34][45] = '#';
        for (int i = 40; i <= 43; i++) {
            lvl[31][i] = ' ';
            lvl[32][i] = '#';
        }
        for (int i = 54; i <= 57; i++) {
            lvl[31][i] = ' ';
            lvl[32][i] = '#';
        }
        lvl[34][133] = '#';
        for (int i = 98; i <= 113; i++) {
            lvl[16][i] = ' ';
            lvl[17][i] = ' ';
        }
        for (int i = 105; i <= 110; i++)
            lvl[17][i] = '#';

        lvl[34][52] = '#';

        lvl[32][15] = '#';
        lvl[32][16] = '#';
        lvl[32][17] = '#';
        lvl[32][18] = '#';

        lvl[33][15] = ' ';
        lvl[33][16] = ' ';
        lvl[33][17] = ' ';
        lvl[33][18] = ' ';


        // allocate temp grid
        char** temp = new char* [H];
        for (int i = 0; i < H; i++) {
            temp[i] = new char[W];
        }

        // copy original → temp
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                temp[i][j] = lvl[i][j];
            }
        }

        // process
        for (int i = 0; i < H - 1; i++) {
            for (int j = 0; j < W; j++) {
                if (lvl[i][j] == '#' && lvl[i + 1][j] == '#') {
                    temp[i + 1][j] = 'D';
                }
            }
        }

        // copy back temp → lvl
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                lvl[i][j] = temp[i][j];
            }
        }

        // free memory
        for (int i = 0; i < H; i++) {
            delete[] temp[i];
        }
        delete[] temp;



        // FLAT
        lvl[25][16] = 'P';
        lvl[31][16] = 'R';
        lvl[32][47] = 'R';
        lvl[34][17] = 'S';
        lvl[34][40] = 'S';
        lvl[34][29] = 'B';
        lvl[34][54] = 'B';
        lvl[33][35] = 'G';
        lvl[33][60] = 'G';

        lvl[33][23] = 'Z'; // ZOMBIE
        lvl[31][56] = 'Z'; // ZOMBIE

        lvl[33][12] = 'M'; // MUMMY
        lvl[31][42] = 'M';

        lvl[29][28] = 'F'; // FLYING TARA

        lvl[33][37] = '2'; //prisoner


        // AERIAL
        lvl[27][87] = 'R';    // REBEL SOLDIER
        lvl[19][98] = 'R';
        lvl[19][108] = 'S';
        lvl[28][83] = 'S';   // SHIELDED SOLDIER
        lvl[18][102] = 'B';
        lvl[18][114] = 'B';  // BAZOOKA SOLDIER
        lvl[16][108] = 'G';
        lvl[19][117] = 'G'; // GRENADE SOLDIER

        lvl[13][110] = 'P'; //PARATROOPER
        lvl[19][110] = 'A'; //ALIEN/ MARTIAN

        lvl[13][108] = 'F'; // FLYING TARA

        lvl[18][112] = '2'; //prisoner

        //AQUATIC
        lvl[33][135] = 'R';
        lvl[33][193] = 'R';
        lvl[33][137] = 'S';
        lvl[33][186] = 'S';
        lvl[33][150] = 'B';
        lvl[33][177] = 'B';
        lvl[33][139] = 'G';
        lvl[33][164] = 'G';

        lvl[33][162] = 'Z'; // ZOMBIE
        lvl[33][175] = 'Z'; // ZOMBIE

        lvl[34][157] = 'N'; // enemy submarine

        lvl[33][152] = '2'; //prisoner
        lvl[31][15] = 'C';

    }

    else if (currentLevel == 2) {
        this->background->getTex().loadFromFile("Sprites/backGround3.png");
        this->background->setScaleX((float)(screen_x) / this->background->getTex().getSize().x);
        this->background->setScaleY((float)(screen_y) / this->background->getTex().getSize().y);

        this->background->initializeScale();
        this->background->initializeDimensions();


        fillMapFromSeed(lvl, 28482, 1);

        lvl[34][159] = '#';
        for (int i = 35; i < 39; i++) {
            lvl[i][159] = 'D';
        }

        // ===== AERIAL (cols 16-53, the slope - skip very tall left peak) =====
        lvl[surfaceRow(lvl, 18) - 1][18] = 'R';
        lvl[surfaceRow(lvl, 30) - 1][30] = 'R';
        lvl[surfaceRow(lvl, 42) - 1][42] = 'R';
        lvl[surfaceRow(lvl, 22) - 1][22] = 'S';
        lvl[surfaceRow(lvl, 34) - 1][34] = 'S';
        lvl[surfaceRow(lvl, 46) - 1][46] = 'S';
        lvl[surfaceRow(lvl, 20) - 1][20] = 'B';
        lvl[surfaceRow(lvl, 32) - 1][32] = 'B';
        lvl[surfaceRow(lvl, 44) - 1][44] = 'B';
        lvl[surfaceRow(lvl, 26) - 1][26] = 'G';
        lvl[surfaceRow(lvl, 38) - 1][38] = 'G';
        lvl[surfaceRow(lvl, 50) - 1][50] = 'G';

        lvl[surfaceRow(lvl, 24) - 5][24] = 'P';
        lvl[surfaceRow(lvl, 40) - 5][40] = 'P';
        lvl[surfaceRow(lvl, 28) - 5][28] = 'A';
        lvl[surfaceRow(lvl, 48) - 5][48] = 'A';

        lvl[surfaceRow(lvl, 36) - 1][36] = '2';

        // ===== PLAINS (cols 58-155) =====
        lvl[surfaceRow(lvl, 58) - 1][58] = 'R';
        lvl[surfaceRow(lvl, 90) - 1][90] = 'R';
        lvl[surfaceRow(lvl, 130) - 1][130] = 'R';
        lvl[surfaceRow(lvl, 65) - 1][65] = 'S';
        lvl[surfaceRow(lvl, 100) - 1][100] = 'S';
        lvl[surfaceRow(lvl, 140) - 1][140] = 'S';
        lvl[surfaceRow(lvl, 72) - 1][72] = 'B';
        lvl[surfaceRow(lvl, 110) - 1][110] = 'B';
        lvl[surfaceRow(lvl, 150) - 1][150] = 'B';
        lvl[surfaceRow(lvl, 80) - 1][80] = 'G';
        lvl[surfaceRow(lvl, 105) - 1][105] = 'G';
        lvl[surfaceRow(lvl, 125) - 1][125] = 'G';

        lvl[surfaceRow(lvl, 62) - 1][62] = 'Z';
        lvl[surfaceRow(lvl, 95) - 1][95] = 'Z';
        lvl[surfaceRow(lvl, 135) - 1][135] = 'Z';
        lvl[surfaceRow(lvl, 70) - 1][70] = 'M';
        lvl[surfaceRow(lvl, 115) - 1][115] = 'M';
        lvl[surfaceRow(lvl, 145) - 1][145] = 'M';

        lvl[surfaceRow(lvl, 75) - 1][75] = 'V';
        lvl[surfaceRow(lvl, 120) - 1][120] = 'V';

        lvl[surfaceRow(lvl, 85) - 1][85] = '2';
        lvl[surfaceRow(lvl, 108) - 1][108] = '2';

        // ===== AQUATIC (cols 162-199) =====
        lvl[surfaceRow(lvl, 162) - 1][162] = 'R';
        lvl[surfaceRow(lvl, 180) - 1][180] = 'R';
        lvl[surfaceRow(lvl, 195) - 1][195] = 'R';
        lvl[surfaceRow(lvl, 165) - 1][165] = 'S';
        lvl[surfaceRow(lvl, 183) - 1][183] = 'S';
        lvl[surfaceRow(lvl, 197) - 1][197] = 'S';
        lvl[surfaceRow(lvl, 170) - 1][170] = 'B';
        lvl[surfaceRow(lvl, 186) - 1][186] = 'B';
        lvl[surfaceRow(lvl, 192) - 1][192] = 'B';
        lvl[surfaceRow(lvl, 175) - 1][175] = 'G';
        lvl[surfaceRow(lvl, 189) - 1][189] = 'G';
        lvl[surfaceRow(lvl, 199) - 1][199] = 'G';

        lvl[surfaceRow(lvl, 168) - 1][168] = 'Z';
        lvl[surfaceRow(lvl, 178) - 1][178] = 'Z';
        lvl[surfaceRow(lvl, 190) - 1][190] = 'Z';

        lvl[surfaceRow(lvl, 172) - 1][172] = 'N';

        lvl[surfaceRow(lvl, 185) - 1][185] = '2';

        // ===== FLYING TARA =====
        lvl[surfaceRow(lvl, 45) - 5][45] = 'F';
        lvl[surfaceRow(lvl, 100) - 5][100] = 'F';
        lvl[surfaceRow(lvl, 170) - 5][170] = 'F';
    }

    else if (currentLevel == 3) {
        this->background->getTex().loadFromFile("Sprites/backGround4.png");
        this->background->setScaleX((float)(screen_x * 1.3) / this->background->getTex().getSize().x);
        this->background->setScaleY((float)(screen_y*1.5) / this->background->getTex().getSize().y);

        this->background->initializeScale();
        this->background->initializeDimensions();

        fillMapFromSeed(lvl, 28482, 1);
        
        for (int c = 0; c < W / 2; c++) {
            for (int i = 0; i < H; i++) {
                char temp = lvl[i][c];
                lvl[i][c] = lvl[i][W - c - 1];
                lvl[i][W - c - 1] = temp;
            }
        }
        
        lvl[34][40] = '#';
        for (int i = 35; i < 39; i++) {
            lvl[i][40] = 'D';
        }


        // ===== AQUATIC (cols 0-40, water on left) =====
        // infantry: 3 batches each
        lvl[surfaceRow(lvl, 4) - 1][11] = 'R';
        lvl[surfaceRow(lvl, 18) - 1][18] = 'R';
        lvl[surfaceRow(lvl, 32) - 1][32] = 'R';
        lvl[surfaceRow(lvl, 7) - 1][9] = 'S';
        lvl[surfaceRow(lvl, 21) - 1][21] = 'S';
        lvl[surfaceRow(lvl, 35) - 1][35] = 'S';
        lvl[surfaceRow(lvl, 10) - 1][10] = 'B';
        lvl[surfaceRow(lvl, 24) - 1][24] = 'B';
        lvl[surfaceRow(lvl, 38) - 1][38] = 'B';
        lvl[surfaceRow(lvl, 13) - 1][13] = 'G';
        lvl[surfaceRow(lvl, 27) - 1][27] = 'G';
        lvl[surfaceRow(lvl, 40) - 1][40] = 'G';

        // zombies in aquatic: 4 batches
        lvl[surfaceRow(lvl, 6) - 1][14] = 'Z';
        lvl[surfaceRow(lvl, 16) - 1][16] = 'Z';
        lvl[surfaceRow(lvl, 26) - 1][26] = 'Z';
        lvl[surfaceRow(lvl, 36) - 1][36] = 'Z';

        // 2 subs in aquatic
        lvl[surfaceRow(lvl, 12) - 1][12] = 'N';
        lvl[surfaceRow(lvl, 30) - 1][30] = 'N';

        // POW in aquatic: 2
        lvl[surfaceRow(lvl, 9) - 1][15] = '2';
        lvl[surfaceRow(lvl, 22) - 1][22] = '2';

        // ===== PLAINS (cols 44-141) =====
        // infantry: 3 batches each
        lvl[surfaceRow(lvl, 48) - 1][48] = 'R';
        lvl[surfaceRow(lvl, 80) - 1][80] = 'R';
        lvl[surfaceRow(lvl, 120) - 1][120] = 'R';
        lvl[surfaceRow(lvl, 55) - 1][55] = 'S';
        lvl[surfaceRow(lvl, 88) - 1][88] = 'S';
        lvl[surfaceRow(lvl, 128) - 1][128] = 'S';
        lvl[surfaceRow(lvl, 62) - 1][62] = 'B';
        lvl[surfaceRow(lvl, 96) - 1][96] = 'B';
        lvl[surfaceRow(lvl, 135) - 1][135] = 'B';
        lvl[surfaceRow(lvl, 70) - 1][70] = 'G';
        lvl[surfaceRow(lvl, 105) - 1][105] = 'G';
        lvl[surfaceRow(lvl, 140) - 1][140] = 'G';

        // undead: 4 batches each (Z, M)
        lvl[surfaceRow(lvl, 50) - 1][50] = 'Z';
        lvl[surfaceRow(lvl, 75) - 1][75] = 'Z';
        lvl[surfaceRow(lvl, 100) - 1][100] = 'Z';
        lvl[surfaceRow(lvl, 130) - 1][130] = 'Z';
        lvl[surfaceRow(lvl, 58) - 1][58] = 'M';
        lvl[surfaceRow(lvl, 85) - 1][85] = 'M';
        lvl[surfaceRow(lvl, 112) - 1][112] = 'M';
        lvl[surfaceRow(lvl, 138) - 1][138] = 'M';

        // bradleys: 2
        lvl[surfaceRow(lvl, 65) - 1][65] = 'V';
        lvl[surfaceRow(lvl, 115) - 1][115] = 'V';

        // POW in plains: 2
        lvl[surfaceRow(lvl, 78) - 1][78] = '2';
        lvl[surfaceRow(lvl, 122) - 1][122] = '2';

        // ===== AERIAL (cols 146-183, slope on right) =====
        // infantry: 3 batches each
        lvl[surfaceRow(lvl, 149) - 1][149] = 'R';
        lvl[surfaceRow(lvl, 161) - 1][161] = 'R';
        lvl[surfaceRow(lvl, 173) - 1][173] = 'R';
        lvl[surfaceRow(lvl, 153) - 1][153] = 'S';
        lvl[surfaceRow(lvl, 165) - 1][165] = 'S';
        lvl[surfaceRow(lvl, 177) - 1][177] = 'S';
        lvl[surfaceRow(lvl, 155) - 1][155] = 'B';
        lvl[surfaceRow(lvl, 167) - 1][167] = 'B';
        lvl[surfaceRow(lvl, 179) - 1][179] = 'B';
        lvl[surfaceRow(lvl, 159) - 1][159] = 'G';
        lvl[surfaceRow(lvl, 171) - 1][171] = 'G';
        lvl[surfaceRow(lvl, 183) - 1][183] = 'G';

        // aerial+alien: 3 batches each
        lvl[surfaceRow(lvl, 151) - 5][151] = 'P';
        lvl[surfaceRow(lvl, 163) - 5][163] = 'P';
        lvl[surfaceRow(lvl, 175) - 5][175] = 'P';
        lvl[surfaceRow(lvl, 157) - 5][157] = 'A';
        lvl[surfaceRow(lvl, 169) - 5][169] = 'A';
        lvl[surfaceRow(lvl, 181) - 5][181] = 'A';

        // POW in aerial: 2
        lvl[surfaceRow(lvl, 162) - 1][162] = '2';
        lvl[surfaceRow(lvl, 178) - 1][178] = '2';

        // ===== FLYING TARA: 3 batches, distributed =====
        lvl[surfaceRow(lvl, 30) - 5][30] = 'F';
        lvl[surfaceRow(lvl, 100) - 5][100] = 'F';
        lvl[surfaceRow(lvl, 170) - 5][170] = 'F';
    }

    else if (currentLevel == 4) {
        for (int r = 0; r < H; r++)
            for (int c = 0; c < W; c++)
                lvl[r][c] = ' ';

        // bedrock floor across whole map
        for (int c = 0; c < W; c++) lvl[39][c] = 'X';

        // ----- plains: cols 0-50, surface row 30 -----
        for (int c = 0; c <= 50; c++) {
            lvl[30][c] = '#';
            for (int r = 31; r <= 38; r++) lvl[r][c] = 'D';
        }

        // ----- ramp up: cols 51-65, surface row 29 -> 15 -----
        int sr = 29;
        for (int c = 51; c <= 65; c++) {
            lvl[sr][c] = '#';
            for (int r = sr + 1; r <= 38; r++) lvl[r][c] = 'D';
            if (sr > 15) sr--;
        }

        // ----- aerial peak: cols 66-115, surface row 15 -----
        for (int c = 66; c <= 115; c++) {
            lvl[15][c] = '#';
            for (int r = 16; r <= 38; r++) lvl[r][c] = 'D';
        }

        // ----- ramp down: cols 116-134, surface row 16 -> 30 -----
        sr = 16;
        for (int c = 116; c <= 134; c++) {
            lvl[sr][c] = '#';
            for (int r = sr + 1; r <= 38; r++) lvl[r][c] = 'D';
            if (sr < 30) sr++;
        }

        // ----- aquatic basin: cols 135-184, water rows 30-38 -----
        for (int c = 135; c <= 184; c++) {
            for (int r = 30; r <= 38; r++) lvl[r][c] = '~';
        }

        // ----- coast: col 185 -----
        lvl[30][185] = '#';
        for (int r = 31; r <= 38; r++) lvl[r][185] = 'D';

        // ----- blended phase 4: cols 186-249 -----
        // pattern: plain (10) - water (10) - plain (10) - water (10) - plain (10) - water (10) - plain (4)
        int c = 186;
        bool plain = true;
        for (int col = c; col < c + 10; col++) {
            lvl[29][col] = '#';
            for (int r = 30; r <= 38; r++) lvl[r][col] = 'D';
        }
        c += 10;
        while (c <= 249) {
            int segEnd = c + 9;
            if (segEnd > 249) segEnd = 249;
            if (plain) {
                for (int col = c; col <= segEnd; col++) {
                    lvl[29][col] = '#';
                    for (int r = 30; r <= 38; r++) lvl[r][col] = '~';
                }
            }
            else {
                for (int col = c; col <= segEnd; col++) {
                    for (int r = 30; r <= 38; r++) lvl[r][col] = '~';
                }
            }
            c = segEnd + 1;
            plain = !plain;
        }

        // ----- boss spawn tiles -----
        lvl[26][30] = 'I';   // IronNokana - on plains
        lvl[10][90] = 'H';   // HairBuster - above aerial peak
        lvl[31][160] = 'E';   // SeaSatan - in water basin
        lvl[20][215] = 'U';   // UltimateBoss - blended phase 4

        //// ----- minion spawn tiles per phase -----
        //// plains/IronNokana zone: ground infantry
        //lvl[29][20] = 'R';
        //lvl[29][40] = 'B';

        //// aerial/HairBuster zone: paratroopers, flying tara
        //lvl[10][80] = 'P';
        //lvl[10][100] = 'F';

        //// aquatic/SeaSatan zone: zombies on coast (closest match for water-adjacent enemies)
        //lvl[29][130] = 'Z';

        //// blended/UltimateBoss zone: scattered around
        //lvl[29][195] = 'R';
        //lvl[29][225] = 'Z';
    }

    placeVehicleMarkers(this->map[1]->getLvl(), this->map[1]->getWidth(), this->map[1]->getHeight());
}



void SurvivalLevel::renderScreen() {
    char**& lvl = this->map[1]->getLvl();
    int cellSize = this->map[1]->getCellSize();
    int mapH = this->map[1]->getHeight();
    int mapW = this->map[1]->getWidth();

    int colStart = this->screenLeft / cellSize;
    int colEnd = this->screenRight / cellSize;
    int rowStart = this->screenTop / cellSize;
    int rowEnd = this->screenBottom / cellSize;

    // clamp
    if (colStart < 0) colStart = 0;
    if (colEnd >= mapW) colEnd = mapW - 1;
    if (rowStart < 0) rowStart = 0;
    if (rowEnd >= mapH) rowEnd = mapH - 1;

    // draw background first
    float bgX = -(screenLeft);
    float bgY = -(screenTop);
    this->background->getSprite().setPosition(bgX, bgY);
    this->background->centreSprite();
    window->draw(this->background->getSprite());

    for (int row = rowStart; row <= rowEnd; row++) {
        for (int col = colStart; col <= colEnd; col++) {
            if (lvl[row][col] == ' ') continue;
            float screenX = (float)(col * cellSize) - this->screenLeft;
            float screenY = (float)(row * cellSize) - this->screenTop;
            if (lvl[row][col] == 'D') {
                if (row < 24) {
                    this->bedrock->getSprite().setPosition(screenX, screenY);
                    window->draw(this->bedrock->getSprite());
                }
                else {
                    this->dirtBlock->getSprite().setPosition(screenX, screenY);
                    window->draw(this->dirtBlock->getSprite());
                }
            }
            else if (lvl[row][col] == 'X') {
                this->bedrock->getSprite().setPosition(screenX, screenY);
                window->draw(this->bedrock->getSprite());
            }
            else if (lvl[row][col] == '#') {
                if (row < 24) {
                    this->bedrock->getSprite().setPosition(screenX, screenY);
                    window->draw(this->bedrock->getSprite());
                }
                else {
                    this->grassBlock->getSprite().setPosition(screenX, screenY);
                    window->draw(this->grassBlock->getSprite());
                }
            }
            else if (lvl[row][col] == '~') {
                this->water->getSprite().setPosition(screenX, screenY);
                window->draw(this->water->getSprite());
            }
        }
    }
}

SurvivalLevel::~SurvivalLevel() {

}

// =========== BLOCK ============
Block::Block() : Entity(), HP(0), map(nullptr) {
}

Block::Block(Level* level) : Entity(level), HP(20), map(level->getMap()) {

}
void Block::breakBlock() {
    int row = (int)(y) / cell_size;
    int col = (int)(x) / cell_size;
    if (row < 0) { return; }
    if (row >= map->getHeight()) { return; }
    if (col < 0) { return; }
    if (col >= map->getWidth()) { return; }
    map->setTile(col, row, ' ');
}
void Block::movement(){}
void Block::updateAnimations() {}
void Block::behaviour() {}







// ============ HASHVALUE ============
HashValue::HashValue() {
    grad = 0;
    val = 0;
}

float HashValue::getGrad() { return grad; }
float HashValue::getVal() { return val; }

void HashValue::seed(long long x, int random) {
    srand((long long)(random + x));
    grad = (float)((rand() % 2001) - 1000) / 1000.0f;
    val = (float)(rand() % 2001 - 1000) / 1000.0f;
    srand(time(0));
}


// ============ PERLINGENERATOR ============
PerlinGenerator::PerlinGenerator(int seed) {
    worldSeed = seed;
}

float PerlinGenerator::noiseAt(long long x, int spacing) {
    long long leftInfluence;
    if (x >= 0) {
        leftInfluence = (x / spacing) * spacing;
    }
    else {
        leftInfluence = ((x - spacing)/ spacing) * spacing;
    }
    long long rightInfluence = leftInfluence + spacing;

    value[0].seed(leftInfluence, worldSeed);
    value[1].seed(rightInfluence, worldSeed);

    float t = (float)(x - leftInfluence) / (float)spacing;
    float leftInf = value[0].getGrad() * t + value[0].getVal();
    float rightInf = value[1].getGrad() * (t - 1.0f) + value[1].getVal();
    float fade = 6 * t * t * t * t * t - 15 * t * t * t * t + 10 * t * t * t;

    return leftInf + fade * (rightInf - leftInf);
}


// ============ NOISEPROFILE ============
NoiseProfile::NoiseProfile() {
    octaves = 4;
    baseSpacing = 40;
    baseAmplitude = 1.0f;
    heightMultiplier = 800;   
    heightOffset = 22;
}
int NoiseProfile::getOctaves() const { return octaves; }
int NoiseProfile::getBaseSpacing() const { return baseSpacing; }
float NoiseProfile::getBaseAmplitude() const { return baseAmplitude; }
int NoiseProfile::getHeightMultiplier() const { return heightMultiplier; }
int NoiseProfile::getHeightOffset() const { return heightOffset; }
float NoiseProfile::shape(float n) const { return n; }


AmplifiedProfile::AmplifiedProfile() {
    octaves = 4;
    baseSpacing = 80;
    baseAmplitude = 2.0f;
    heightMultiplier = 300;
    heightOffset = 24;
}
float AmplifiedProfile::shape(float n) const {
    if (n >= 0) return n * n * 0.8f;
    return -(n * n) * 0.8f;
}


NormalProfile::NormalProfile() {
    octaves = 2;
    baseSpacing = 60;
    baseAmplitude = 2.0f;
    heightMultiplier = 600;
    heightOffset = 22;
}

FlatProfile::FlatProfile() {
    octaves = 1;
    baseSpacing = 40;
    baseAmplitude = 2.0f;
    heightMultiplier = 400;
    heightOffset = 20;
}
float FlatProfile::shape(float n) const {
    return n * 0.8f;
}





// ============ FRACTALNOISEGENERATOR ============
FractalNoiseGenerator::FractalNoiseGenerator(int seed) {
    worldSeed = seed;
    perlinGenerator = new PerlinGenerator(seed);
}

FractalNoiseGenerator::~FractalNoiseGenerator() {
    delete perlinGenerator;
}

void FractalNoiseGenerator::generateSummedHeights(long long anyXInChunk, int* outBuffer, NoiseProfile* profile) {
    long long chunkStart;
    if (anyXInChunk >= 0) {
        chunkStart = (anyXInChunk / 200) * 200;
    }
    else {
        long long mod = anyXInChunk % 200;
        if (mod < 0) mod += 200;
        chunkStart = anyXInChunk - mod;
    }

    int octaves = profile->getOctaves();
    int baseSpacing = profile->getBaseSpacing();
    float baseAmp = profile->getBaseAmplitude();
    int mult = profile->getHeightMultiplier();
    int offset = profile->getHeightOffset();

    for (int i = 0; i < 200; i++) {
        long long worldX = chunkStart + i;
        float total = 0.0f;
        float amp = baseAmp;
        int spacing = baseSpacing;

        for (int o = 0; o < octaves; o++) {
            float n = perlinGenerator->noiseAt(worldX, spacing);
            total += n * amp;
            amp *= 0.5f;
            spacing /= 2;
            if (spacing < 2) spacing = 2;
        }

        float shaped = profile->shape(total);
        int h = (int)(shaped * mult / 100.0f) + offset;
        if (h < 4) h = 4;
        if (h > 38) h = 38;
        outBuffer[i] = h;
    }
}


// ============ NOISEPROFILEFACTORY ============
NoiseProfileFactory::NoiseProfileFactory() {
    noiseProfile = nullptr;
}

NoiseProfileFactory::~NoiseProfileFactory() {
    if (noiseProfile) delete noiseProfile;
}

void NoiseProfileFactory::selectProfile(int selectedProfile) {
    if (noiseProfile) { delete noiseProfile; noiseProfile = nullptr; }
    if (selectedProfile == 1) noiseProfile = new AmplifiedProfile();
    else if (selectedProfile == 2) noiseProfile = new NormalProfile();
    else noiseProfile = new FlatProfile();
}

NoiseProfile* NoiseProfileFactory::getProfile() {
    return noiseProfile;
}


// ============ INFINITELEVEL ============
InfiniteLevel::InfiniteLevel(PlayerSoldier* currentPlayer, int profileChoice) : Level(currentPlayer)
{
    this->profileChoice = profileChoice;
    this->worldSeed = rand() % 100000000 + 1;

    noiseProfileFactory = new NoiseProfileFactory();
    noiseProfileFactory->selectProfile(profileChoice);
    fractalGen = new FractalNoiseGenerator(worldSeed);

    map[0] = generateChunk(-200);
    map[1] = generateChunk(0);
    map[2] = generateChunk(200);

    dirtBlock->getTex().loadFromFile("Sprites/blocks/dirt.png");
    bedrock->getTex().loadFromFile("Sprites/blocks/andesite.png");
    grassBlock->getTex().loadFromFile("Sprites/blocks/grass_block_side.png");
    water->getTex().loadFromFile("Sprites/blocks/water.png");
    water->setScaleX(64.0f / 304.0f);
    water->setScaleY(64.0f / 303.0f);
    background->getTex().loadFromFile("Sprites/backGround2.png");
    background->setScaleX((float)(screen_x) / background->getTex().getSize().x);
    background->setScaleY((float)(screen_y) / background->getTex().getSize().y);

    background->initializeScale();
    background->initializeDimensions();

    dirtBlock->initializeScale();
    dirtBlock->initializeDimensions();

    grassBlock->initializeScale();
    grassBlock->initializeDimensions();

    water->initializeScale();
    water->initializeDimensions();

    bedrock->initializeScale();
    bedrock->initializeDimensions();

    screenTop = (35 * 64 - 128) - (screen_y / 2);
    screenBottom = screenTop + screen_y;
}

InfiniteLevel::~InfiniteLevel() {
    delete fractalGen;
    delete noiseProfileFactory;
}

int InfiniteLevel::getMapCount() { return 3; }

Map* InfiniteLevel::getMapAt(int i) {
    if (i < 0 || i > 2) return nullptr;
    return map[i];
}

int InfiniteLevel::getHeight(long long x) {
    int buffer[200];
    fractalGen->generateSummedHeights(x, buffer, noiseProfileFactory->getProfile());

    long long chunkStart;
    if (x >= 0) chunkStart = (x / 200) * 200;
    else {
        long long mod = x % 200;
        if (mod < 0) mod += 200;
        chunkStart = x - mod;
    }
    return buffer[x - chunkStart];
}

Map* InfiniteLevel::generateChunk(long long anyXInChunk) {
    long long chunkStart;
    if (anyXInChunk >= 0) chunkStart = (anyXInChunk / 200) * 200;
    else {
        long long mod = anyXInChunk % 200;
        if (mod < 0) mod += 200;
        chunkStart = anyXInChunk - mod;
    }

    Map* m = new Map(40, 200);
    m->setChunkOriginX(chunkStart);

    int heights[200];
    fractalGen->generateSummedHeights(chunkStart, heights, noiseProfileFactory->getProfile());

    char**& lvl = m->getLvl();
    for (int r = 0; r < 40; r++) {
        for (int c = 0; c < 200; c++) {
            lvl[r][c] = ' ';
        }
    }


    for (int c = 0; c < 200; c++) {
        int h = heights[c];
        int surfaceRow = 40 - h;
        if (surfaceRow < 0) surfaceRow = 0;
        if (surfaceRow > 39) surfaceRow = 39;

        lvl[39][c] = 'X';
        for (int r = 38; r > surfaceRow; r--) lvl[r][c] = 'D';

        if (h <= 5) {
            for (int r = 35; r <= 38; r++) {
                lvl[r][c] = '~';
            }

        }
        else {
            lvl[surfaceRow][c] = '#';
        }
    }

    // sparse spawn markers - 2 per chunk, only on grass surface
    const char enemyPool[8] = { 'R', 'S', 'B', 'G', 'M', 'Z', 'P', 'A' };
    for (int attempt = 0; attempt < 20; attempt++) {
        int c = rand() % 200;
        int tries = 0;
        while (tries < 10) {
            int sr = -1;
            for (int r = 0; r < 40; r++) {
                if (lvl[r][c] == '#') { sr = r; break; }
            }
            if (sr > 0) {
                char enemyChoosen = enemyPool[rand() % 8];
                if (enemyChoosen == 'P') 
                {
                    if (sr > 5) {
                        lvl[sr - 5][c] = enemyChoosen;
                    }

                }
                else {
                    lvl[sr - 1][c] = enemyChoosen;
                }

                break;
            }
            c = (c + 17) % 200;
            tries++;
        }
    }

    placeVehicleMarkers(lvl, 200, 40);
    return m;
}

void InfiniteLevel::generateMap() {
    // chunks built in constructor
}

void InfiniteLevel::shiftChunksIfNeeded() {
    if (currentPlayer == nullptr) return;
    int cs = map[1]->getCellSize();

    while (true) {
        long long midOrigin = map[1]->getChunkOriginX();
        long long playerCol = (long long)(currentPlayer->getX() / cs);

        if (playerCol - 1 >= midOrigin + 200) {
            delete map[0];
            map[0] = map[1];
            map[1] = map[2];
            map[2] = generateChunk(map[1]->getChunkOriginX() + 200);
        }
        else if (playerCol + 1 < midOrigin) {
            delete map[2];
            map[2] = map[1];
            map[1] = map[0];
            map[0] = generateChunk(map[1]->getChunkOriginX() - 200);
        }
        else break;
    }
}

void InfiniteLevel::renderScreen() {
    shiftChunksIfNeeded();

    int cs = map[1]->getCellSize();
    background->getSprite().setPosition(0, 0);
    window->draw(background->getSprite());

    for (int i = 0; i < 3; i++) {
        Map* m = map[i];
        if (m == nullptr) continue;
        char** lvl = m->getLvl();
        long long originPx = m->getChunkOriginX() * cs;

        int colStart = (int)((screenLeft - originPx) / cs) - 1;
        int colEnd = (int)((screenRight - originPx) / cs) + 1;
        if (colStart < 0) colStart = 0;
        if (colEnd > 199) colEnd = 199;
        if (colEnd < 0 || colStart > 199) continue;

        for (int r = 0; r < 40; r++) {
            for (int c = colStart; c <= colEnd; c++) {
                char t = lvl[r][c];
                if (t == ' ') continue;

                float screenX = (float)(originPx + c * cs - screenLeft);
                float screenY = (float)(r * cs - screenTop);

                //SfmlObject* obj = nullptr;
                //if (t == 'X') obj = bedrock;
                //else if (t == 'D') obj = dirtBlock;
                //else if (t == '#') obj = grassBlock;
                //else if (t == '~') obj = water;

                //if (obj != nullptr) {
                //    obj->getSprite().setPosition(screenX, screenY);
                //    window->draw(obj->getSprite());
                //}
                if (t == 'D') {
                    if (r < 24) {
                        this->bedrock->getSprite().setPosition(screenX, screenY);
                        window->draw(this->bedrock->getSprite());
                    }
                    else {
                        this->dirtBlock->getSprite().setPosition(screenX, screenY);
                        window->draw(this->dirtBlock->getSprite());
                    }
                }
                else if (t == 'X') {
                    this->bedrock->getSprite().setPosition(screenX, screenY);
                    window->draw(this->bedrock->getSprite());
                }
                else if (t == '#') {
                    if (r < 24) {
                        this->bedrock->getSprite().setPosition(screenX, screenY);
                        window->draw(this->bedrock->getSprite());
                    }
                    else {
                        this->grassBlock->getSprite().setPosition(screenX, screenY);
                        window->draw(this->grassBlock->getSprite());
                    }
                }
                else if (t == '~') {
                    this->water->getSprite().setPosition(screenX, screenY);
                    window->draw(this->water->getSprite());
                }
            }
        }
    }
}


void InfiniteLevel::scrollLevel() {
    if (currentPlayer == nullptr) return;

    // horizontal
    if (currentPlayer->getX() < screenLeft + (screen_x / 2.5)) {
        screenLeft = currentPlayer->getX() - (float)screen_x / 2.5;
        screenRight = screenLeft + screen_x;
    }
    else if (currentPlayer->getX() > screenRight - (screen_x / 2.5)) {
        screenRight = currentPlayer->getX() + (float)screen_x / 2.5;
        screenLeft = screenRight - screen_x;
    }

    // vertical
    if (currentPlayer->getY() < screenTop + (screen_y / 2.5)) {
        screenTop = currentPlayer->getY() - (float)screen_y / 2.5;
        screenBottom = screenTop + screen_y;
    }
    else if (currentPlayer->getY() > screenBottom - (screen_y / 2.5)) {
        screenBottom = currentPlayer->getY() + (float)screen_y / 2.5;
        screenTop = screenBottom - screen_y;
    }
}

char InfiniteLevel::tileAtWorld(float worldX, float worldY) {
    int cs = this->map[1]->getCellSize();
    int H = this->map[1]->getHeight();
    int row = (int)(worldY / cs);
    if (row < 0) return ' ';
    if (row >= H) row = H - 1;

    long long worldCol;
    if (worldX >= 0) {
        worldCol = (long long)(worldX / cs);
    }
    else {
        worldCol = (long long)((worldX - cs) / cs);
    }

    for (int i = 0; i < 3; i++) {
        Map* m = this->map[i];
        if (m == nullptr) continue;
        long long origin = m->getChunkOriginX();
        if (worldCol >= origin && worldCol < origin + 200) {
            int localCol = (int)(worldCol - origin);
            return m->getLvl()[row][localCol];
        }
    }
    return ' ';
}
