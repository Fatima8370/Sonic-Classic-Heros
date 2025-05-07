

/*


MY CHANGES FOR CHARACTER CLASSES

    float getPositionX() { return position[0]; }



    OFFSET ADJUSTMENT
    playerSprite.setPosition(position[0] - offset, position[1]);

    X Y CONDITIONS TO PREVENT GETTING OFFSCREEN

           
           if (position[1] > 1200) position[1] = 1200 - 64;

           if (position[0] < 64) position[0] = 64;



           class EnemyFactory {
private:
    static const int MAX_ENEMIES = 50;
    Enemies* enemies[MAX_ENEMIES];
    float usedX[MAX_ENEMIES];
    int count;

    char** levelGrid;
    int gridHeight, gridWidth;
    int levelNum;

    // Minimum distance between enemies
    const float MIN_ENEMY_DISTANCE = 150.0f;

    bool isFarFromOthers(float x) {
        for (int i = 0; i < count; ++i) {
            if (abs(x - usedX[i]) < MIN_ENEMY_DISTANCE)
                return false;
        }
        return true;
    }

    Enemies* createEnemy(const string& type, float spawnX, float spawnY) {
        Enemies* enemy = nullptr;

        if (type == "batbrain") enemy = new BatBrain();
        else if (type == "motobug") enemy = new Motobug();
        else if (type == "beebot") enemy = new BeeBot();
        else if (type == "crabmeat") enemy = new CrabMeat();
        else if (type == "eggstinger") enemy = new EggStinger();

        if (enemy) {
            enemy->setPosition(spawnX, spawnY);
            cout << "Created " << type << " at (" << spawnX << ", " << spawnY << ")" << endl;
        }
        return enemy;
    }

public:
    EnemyFactory(char** grid, int height, int width, int lvl)
        : gridHeight(height), gridWidth(width), levelNum(lvl), count(0), levelGrid(grid)
    {
        for (int i = 0; i < MAX_ENEMIES; ++i)
            enemies[i] = nullptr;

        cout << "In Enemy Factory\n";
    }

    Enemies* createWithSpawn(const string& type) {
        if (count >= MAX_ENEMIES) return nullptr;

        cout << "Creating with spawn: " << type << endl;

        int tries = 0;
        const int MAX_TRIES = 100; // Increased attempts

        while (tries < MAX_TRIES) {
            int col = rand() % gridWidth;
            int row;
            float spawnX = static_cast<float>(col * 64);  // tile size
            float spawnY;
            bool validSpawn = false;

            // Different spawn algorithms based on enemy type
            if (type == "batbrain" || type == "beebot") {
                // FLYING ENEMIES: Spawn in open air
                // Pick a random row in the upper 2/3 of the level
                row = rand() % (int)(gridHeight * 0.66);

                // Check for an open space (no wall/spike/pit)
                if (levelGrid[row][col] == ' ') {
                    // For flying enemies, adjust height to be higher off the ground
                    // Find distance to ground
                    int groundRow = row;
                    while (groundRow < gridHeight && levelGrid[groundRow][col] == ' ') {
                        groundRow++;
                    }

                    // If we're at least 2 tiles from ground/ceiling, this is a good spot
                    if (row > 1 && (groundRow - row) >= 2) {
                        spawnY = static_cast<float>(row * 64);
                        validSpawn = true;

                        // Adjust heights for specific flying enemies
                        if (type == "beebot") {
                            spawnY -= 30; // Lift BeeBots higher
                        }
                    }
                }
            }
            else if (type == "motobug" || type == "crabmeat") {
                // GROUND ENEMIES: Find solid ground to spawn on

                // Start from a random position in the lower 2/3 of the level
                row = (int)(gridHeight * 0.33) + rand() % (int)(gridHeight * 0.66);

                // Look for a platform or ground
                if (row + 1 < gridHeight &&
                    (levelGrid[row][col] == ' ' &&
                        (levelGrid[row + 1][col] == 'w' || levelGrid[row + 1][col] == 'p'))) {

                    // Place directly on top of the ground
                    spawnY = static_cast<float>(row * 64);
                    validSpawn = true;

                    // Adjust for specific ground enemies
                    if (type == "motobug") {
                        spawnY += 10; // Place motobugs slightly lower to touch ground
                    }
                }
            }
            else if (type == "eggstinger") {
                // BOSS: Special spawn in center-top
                spawnX = (gridWidth / 2) * 64.0f;
                spawnY = 150.0f;
                validSpawn = true;
            }

            if (validSpawn && isFarFromOthers(spawnX)) {
                Enemies* e = createEnemy(type, spawnX, spawnY);
                if (e) {
                    enemies[count] = e;
                    usedX[count] = spawnX;
                    ++count;
                    return e;
                }
            }

            ++tries;
        }

        cout << "Failed to spawn " << type << " after " << MAX_TRIES << " attempts." << endl;
        return nullptr;
    }

    // More flexible enemy creation for specific patterns or manual placement
    Enemies* createAt(const string& type, float spawnX, float spawnY) {
        if (count >= MAX_ENEMIES) return nullptr;

        if (isFarFromOthers(spawnX)) {
            Enemies* e = createEnemy(type, spawnX, spawnY);
            if (e) {
                enemies[count] = e;
                usedX[count] = spawnX;
                ++count;
                return e;
            }
        }
        return nullptr;
    }

    // Spawn a specific number of each enemy type
    void populateLevel() {
        // Clear any existing enemies
        for (int i = 0; i < count; i++) {
            if (enemies[i]) {
                delete enemies[i];
                enemies[i] = nullptr;
            }
        }
        count = 0;

        // Spawn enemies based on level number
        int batbrains = 3 + levelNum;
        int motobugs = 4 + levelNum;
        int beebots = (levelNum > 1) ? 2 + levelNum : 0; // No beebots in level 1
        int crabmeats = (levelNum > 2) ? 2 + levelNum : 0; // No crabmeats in levels 1-2

        // Boss only in certain levels
        if (levelNum % 3 == 0) { // Every 3rd level has a boss
            createWithSpawn("eggstinger");
        }

        // Spawn regular enemies
        for (int i = 0; i < batbrains; i++) createWithSpawn("batbrain");
        for (int i = 0; i < motobugs; i++) createWithSpawn("motobug");
        for (int i = 0; i < beebots; i++) createWithSpawn("beebot");
        for (int i = 0; i < crabmeats; i++) createWithSpawn("crabmeat");

        cout << "Level " << levelNum << " populated with " << count << " enemies" << endl;
    }

    void updateEnemies(RenderWindow& window, Enemies* enemy[], int count, float offsetX, float deltaTime, float playerX, float playerY, bool attack)
    {
        // Only print debug info occasionally to avoid console spam
        static int frameCounter = 0;
        frameCounter++;

        if (frameCounter % 60 == 0 && count > 0) { // Print debug every 60 frames
            cout << "Updating " << count << " enemies. Player at (" << playerX << ", " << playerY << ")" << endl;
            frameCounter = 0;
        }

        for (int i = 0; i < count; i++)
        {
            if (enemy[i])
            {
                // Update all enemies using the unified update method
                enemy[i]->update(playerX, playerY, deltaTime);

                // Draw with scrolling offset
                enemy[i]->draw(window, offsetX);

                // Handle attack/die logic
                if (attack) {
                    enemy[i]->die(playerX, playerY);
                }
            }
        }
    }

    // Clear all enemies - useful when changing levels
    void clearEnemies() {
        for (int i = 0; i < count; i++) {
            if (enemies[i]) {
                delete enemies[i];
                enemies[i] = nullptr;
            }
        }
        count = 0;
        cout << "All enemies cleared" << endl;
    }

    int getCount() const { return count; }
    Enemies** getAllEnemies() { return enemies; }
};




*/