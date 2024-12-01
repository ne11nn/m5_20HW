#include "battleCity.h"
#include "powerup.h"

MyTank myTank;

EnemyTank tank1;
EnemyTank tank2;
EnemyTank tank3;

Bullet myBullet;
Bullet bullet1;
Bullet bullet2;
Bullet bullet3;

int lives;

int myReplaceWater = 0;
int oneReplaceWater = 0;
int twoReplaceWater = 0;
int threeReplaceWater = 0;

int shieldLost = 0;

double myPowerupTime = -1;
int myPowerupActive = -1;

int waterBoard[2][500];
int treeBoard[2][500];

int waterDetected = 0;
int treeDetected = 0;

void getHighScore()
{
    FILE *fp = NULL;
    fp = fopen("D:/Coding/C/cClass/battleCity/scoreinfo.txt", "r");

    fscanf(fp, "%d", &highscore);
    fclose(fp);
}

void updateHighScore()
{
    highscore = score;
    GoToxy(COL + 2, 10);
    printf("Highscore: %d", highscore);

    FILE *fp = NULL;

    fp = fopen("D:/Coding/C/cClass/battleCity/scoreinfo.txt", "w+");
    fprintf(fp, "%d", highscore);
    fclose(fp);
}

void hideCursor(int on)
{
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   if (on == 0)
   {
        info.bVisible = TRUE;
   }
   else
   {
        info.bVisible = FALSE;
   }
   SetConsoleCursorInfo(consoleHandle, &info);
}

void GoToxy(int x, int y)
{
	COORD pos;
	pos.X = x+1;
	pos.Y = y+1;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(handle, pos);
}

void cleanIntArray(int array[ROW][COL]) {
    for (int y=0; y<ROW; y++) {
        for (int x=0; x<COL; x++) {
            array[y][x] = 0;
        }
    }
}

void cleanDoubleArray(double array[ROW][COL]) {
    for (int y=0; y<ROW; y++) {
        for (int x=0; x<COL; x++) {
            array[y][x] = 0;
        }
    }
}

void cleanParentArray(int array[ROW][COL][2]) {
    for (int y=0; y<ROW; y++) {
        for (int x=0; x<COL; x++) {
            array[y][x][0] = -1;
            array[y][x][1] = -1;
        }
    }
}

void calculateHeuristics(double h[ROW][COL], double g[ROW][COL], double f[ROW][COL], int end[2]) {
    for (int y=0; y<ROW; y++) {
        for (int x=0; x<COL; x++) {
            h[y][x] = sqrt(pow(end[1] - x, 2) + pow(end[0] - y, 2));
            f[y][x] = g[y][x] + h[y][x];
        }
    }
}

int* getNewCurrent(double f[ROW][COL], int array[ROW][COL], int open[ROW][COL]) {
    static int lowest[2] = {-1, -1};
    int lowestValue = 99999;
    for (int y=0; y<ROW; y++) {
        for (int x=0; x<COL; x++) {
            if (f[y][x] < lowestValue && open[y][x] == 1) {
                lowest[0] = y;
                lowest[1] = x;
                lowestValue = f[lowest[0]][lowest[1]];
            }
        }
    }
    return lowest;
}

void reconstruct(int* current, int parent[ROW][COL][2], int array[ROW][COL]) {
    while (parent[current[0]][current[1]][0] != -1) {
        array[parent[current[0]][current[1]][0]][parent[current[0]][current[1]][1]] = 20;
        current[0] = parent[current[0]][current[1]][0];
        current[1] = parent[current[0]][current[1]][1];
    }
}

int pathFind(int startX, int startY, int endX, int endY)
{
    int parent[ROW][COL][2];

    double f[ROW][COL];
    double g[ROW][COL];
    double h[ROW][COL];

    int* current;
    int done = 0;
    int numOpen = 0;
    int open[ROW][COL];
    int closed[ROW][COL];

    int start[2] = {startX, startY};
    int end[2] = {endX, endY};

    pathFindBoard[start[0]][start[1]] = 20;
    pathFindBoard[end[0]][end[1]] = 20;

    cleanParentArray(parent);
    cleanDoubleArray(f);
    cleanDoubleArray(g);
    cleanDoubleArray(h);
    calculateHeuristics(h, g, f, end);
    cleanIntArray(open);
    cleanIntArray(closed);

    open[start[0]][start[1]] = 1;
    numOpen++;

    while (numOpen > 0 && done == 0) {
        current = getNewCurrent(f, pathFindBoard, open);

        if (current[0] == end[0] && current[1] == end[1]) {
            done = 1;
            reconstruct(current, parent, pathFindBoard);
            return 0;
        }

        open[current[0]][current[1]] = 0;
        numOpen--;
        closed[current[0]][current[1]] = 1;

        for (int y=current[0]-1; y<current[0]+2; y++) {
            for (int x=current[1]-1; x<current[1]+2; x++) {
                if (x < 0 || y < 0 || x > COL-1 || y > ROW-1 ||
                    abs(current[0] - y) == abs(current[1] - x) ||
                    closed[y][x] == 1 || pathFindBoard[y][x] == 1) {
                    continue;
                }

                double tempG = g[current[0]][current[1]] + 1;
                if (open[y][x] == 1) {
                    if (tempG < g[y][x]) {
                        g[y][x] = tempG;
                        f[y][x] = g[y][x] + h[y][x];
                        parent[y][x][0] = current[0];
                        parent[y][x][1] = current[1];
                    }
                }
                else {
                    g[y][x] = tempG;
                    open[y][x] = 1;
                    numOpen++;
                    f[y][x] = g[y][x] + h[y][x];
                    parent[y][x][0] = current[0];
                    parent[y][x][1] = current[1];
                }
            }
        }
    }

    return 0;
}

void printTank(MyTank tank)
{
    char *(*tankF)[4] = tank_figure[tank.shape];
    for (int i = 0; i < 3; i++)
    {
        GoToxy((tank.x-1), tank.y-1+i);
        printf("%s", tankF[i][tank.direction]);
        for (int j = 0; j < 3; j++)
        {
            gameBoard[tank.y+j-1][tank.x+i-1] = PTANK;
            pathFindBoard[tank.y+j-1][tank.x+i-1] = PTANK;
        }
    }
    GoToxy(0,ROW+5);
}

void printEnemyTank(EnemyTank tank)
{
    char *(*tankF)[4] = tank_figure[tank.shape];
    for (int i = 0; i < 3; i++)
    {
        GoToxy((tank.x-1), tank.y-1+i);
        printf("%s", tankF[i][tank.direction]);
        for (int j = 0; j < 3; j++)
        {
            gameBoard[tank.y+j-1][tank.x+i-1] = ETANK;
            pathFindBoard[tank.y+j-1][tank.x+i-1] = ETANK;
        }
    }
    GoToxy(0,ROW+5);
}

void clearEnemyTank(EnemyTank tank)
{
    char clear = ' ';
    for (int i = 0; i < 3; i++)
    {
        GoToxy((tank.x-1), tank.y-1+i);
        printf("%c%c%c", clear, clear, clear);
        for (int j = 0; j < 3; j++)
        {
            gameBoard[tank.y+j-1][tank.x+i-1] = EMPTY;
            pathFindBoard[tank.y+j-1][tank.x+i-1] = EMPTY;
        }
    }
}

void clearSpawnTank()
{
    char clear = ' ';
    for (int i = 0; i < 3; i++)
    {
        GoToxy((myTank.x-1), myTank.y-1+i);
        printf("%c%c%c", clear, clear, clear);
        for (int j = 0; j < 3; j++)
        {
            gameBoard[myTank.y+j-1][myTank.x+i-1] = EMPTY;
            pathFindBoard[myTank.y+j-1][myTank.x+i-1] = EMPTY;
        }
    }

    initiateTank();
    printTank(myTank);
}

void updateScore()
{
    GoToxy(COL + 2, 7);
    printf("Score: %d", score);
}

void updateTanksLeft()
{
    GoToxy(COL + 2, 13);
    printf("Tanks Left: %d/%d", tanksRemaining,originalTanks);
}

void updateLives()
{
    GoToxy(COL + 2, 16);
    printf("Lives left: %d", lives);
}

void updateHealth()
{
    GoToxy(COL + 2, 19);
    printf("Health left: %d  ", myTank.health);
}

void printBullet(Bullet bullet)
{
    GoToxy(bullet.x, bullet.y);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN);
    printf("O");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void defensePowerup(int rein)
{
    for (int i = 18; i < 25; i++)
    {
        for (int j = 35; j < 40; j++)
        {
            if (i < 23 && i > 19)
            {
                if (j < 40 && j > 36)
                {
                    break;
                }
            }
            if (rein == 1)
            {
                gameBoard[j][i] = REINWALL;
                pathFindBoard[j][i] = REINWALL;
                GoToxy(i,j);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED
                            |BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED);
                printf("■");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
            else
            {
                gameBoard[j][i] = REGWALL;
                pathFindBoard[j][i] = REGWALL;
                GoToxy(i,j);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED|BACKGROUND_GREEN|BACKGROUND_RED);
				printf("▓"); 
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
        }
    }
}

void enemyDefensePowerup(int rein)
{
    char clear = ' ';
    for (int i = 18; i < 25; i++)
    {
        for (int j = 35; j < 40; j++)
        {
            if (i < 23 && i > 19)
            {
                if (j < 40 && j > 36)
                {
                    break;
                }
            }
            if (rein == 1)
            {
                gameBoard[j][i] = EMPTY;
                pathFindBoard[j][i] = EMPTY;
                GoToxy(i,j);
                printf("%c", clear);
            }
            else
            {
                gameBoard[j][i] = REGWALL;
                pathFindBoard[j][i] = REGWALL;
                GoToxy(i,j);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED|BACKGROUND_GREEN|BACKGROUND_RED);
				printf("▓"); 
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
        }
    }
}

void initiateTank()
{
    myTank.x = COL/2 - 5;
    myTank.y = 38;
    myTank.shape = myTank.type - 1;
    myTank.direction = 0;

    switch (myTank.type)
    {
        case 1:
            myTank.speed = 6;
            myTank.bulletPower = 1;
            myTank.bulletSpeed = 4;
            myTank.health = 3;
            myTank.lives = 4;
            break;

        case 2:
            myTank.speed = 2;
            myTank.bulletPower = 1;
            myTank.bulletSpeed = 4;
            myTank.health = 1000; //3
            myTank.lives = 3;
            break;

        case 3:
            myTank.speed = 4;
            myTank.bulletPower = 2;
            myTank.bulletSpeed = 4;
            myTank.health = 2;
            myTank.lives = 3;
            break;

        case 4:
            myTank.speed = 8;
            myTank.bulletPower = 2;
            myTank.bulletSpeed = 6;
            myTank.health = 5;
            myTank.lives = 3;
            break;
    }
}

void initiateMap(int mapNumber)
{
    FILE *fp = NULL;

    char finalFileName[100];
    snprintf(finalFileName, sizeof(finalFileName), "D:/Coding/C/cClass/battleCity/mapInfo/map%d.txt", mapNumber);

    fp = fopen(finalFileName, "r");
    

    for (int i = 0; i < 6; i++)
    {
        fscanf(fp, "%d", &numTanks[i]);
        //printf("%d\n", numTanks[i]);
    }

    int length = sizeof(numTanks)/sizeof(numTanks[0]);    
    
    for (int i = 0; i < length; i++)
    {
        tanksRemaining += numTanks[i];
    }

    originalTanks = tanksRemaining;

    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            fscanf(fp, "%d", &gameBoard[i][j]);

            pathFindBoard[i][j] = gameBoard[i][j];
            //printf("%d ", gameBoard[i][j]);
            if (gameBoard[i][j] == WATER)
            {
                waterBoard[0][waterDetected] = i;
                waterBoard[1][waterDetected] = j;
                waterDetected += 1;
            }
            else if (gameBoard[i][j] == TREE)
            {
                treeBoard[0][treeDetected] = i;
                treeBoard[1][treeDetected] = j;
                treeDetected += 1;
            }
        }
        //printf("\n");
    }

    fclose(fp);

    myTank.x = COL/2 - 5;
    myTank.y = 38;
    myTank.shape = myTank.type - 1;
    myTank.direction = 0;

    initiateTank();
}

void displayMap(int mapNumber)
{
    char strVal = ' ';
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            switch (gameBoard[i][j])
            {
                case REGWALL:
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED|BACKGROUND_GREEN|BACKGROUND_RED);
				    GoToxy(j,i);
				    printf("▓"); 
                    break;

                case OUTERWALL:
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN
					    |FOREGROUND_RED|FOREGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE);
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN
					    |FOREGROUND_RED|FOREGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE);
				    GoToxy(j,i);
				    printf("■");
                    break;

                case REINWALL:
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED
                        |BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED);
                    GoToxy(j,i);
				    printf("■");
                    break;

                case HOME:
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED
                        |BACKGROUND_RED);
                    GoToxy(j,i);
                    printf("■");
                    break;

                case POWERWALL:
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN
                        |BACKGROUND_RED);
                    GoToxy(j,i);
                    printf("■");
                    break;

                case WATER:
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|BACKGROUND_BLUE);
                    GoToxy(j,i);
                    printf("■");
                    break;

                case EMPTY:
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN
			        |FOREGROUND_RED|FOREGROUND_BLUE);
                    GoToxy(j,i);
                    printf("%c",strVal);
                    break;
            }
        }
    }

    GoToxy(COL + 2,3);
    printf("Level: %d", mapNumber);
    GoToxy(COL + 2, 7);
    printf("Score: %d", score);
    GoToxy(COL + 2, 10);
    printf("Highscore: %d", highscore);
    GoToxy(COL + 2, 13);
    printf("Tanks Left: %d/%d", tanksRemaining,originalTanks);
    GoToxy(COL + 2, 16);
    printf("Lives left: %d", lives);
    GoToxy(COL + 2, 19);
    printf("Health left: %d", myTank.health);
    GoToxy(COL + 2, 22);
    printf("Controls:");
    GoToxy(COL + 2, 23);
    printf("-------------");
    GoToxy(COL + 2, 24);
    printf("↑: Up");
    GoToxy(COL + 2, 25);
    printf("↓: Down");
    GoToxy(COL + 2, 26);
    printf("←: Left");
    GoToxy(COL + 2, 27);
    printf("→: Right");
    GoToxy(COL + 2, 28);
    printf("F: Shoot");
    GoToxy(COL + 2, 29);
    printf("Kill all tanks before you");
    GoToxy(COL + 2, 30);
    printf("lose all your lives or they reach your home");

    GoToxy(myTank.x,myTank.y);
    printTank(myTank);
}

void startScreen()
{
    GoToxy(30,15);
    printf("Battle City");
    GoToxy(28,16);
    printf("Choose your tank");
    GoToxy(15,20);
    printf("Normal: A regular tank that has no special traits - 1");
    for (int i = 0; i < 3; i++)
    {
        GoToxy(15,22+i);
        printf("%s", tank_figure[0][i][0]);
    }
    GoToxy(15,26);
    printf("Agility: A tank that has faster speed but less lives - 2");
    for (int i = 0; i < 3; i++)
    {
        GoToxy(15,28+i);
        printf("%s", tank_figure[1][i][0]);
    }
    GoToxy(15,32);
    printf("Attack: A tank that has a little more speed, more damage but less health and lives - 3");
    for (int i = 0; i < 3; i++)
    {
        GoToxy(15,34+i);
        printf("%s", tank_figure[2][i][0]);
    }
    GoToxy(15,38);
    printf("Defense: A tank that's slow and has more damage and has more health - 4");
    for (int i = 0; i < 3; i++)
    {
        GoToxy(15,40+i);
        printf("%s", tank_figure[3][i][0]);
    }

    myTank.type = -1;

    int n = 0; // how many times player fails to type a number 1 - 4
    while (myTank.type != 1 && myTank.type != 2 && myTank.type != 3 && myTank.type != 4)
    {
        GoToxy(15,44 + n);
        printf("Pick your tank by typing in a number (1 - 4): ");
        scanf("%d",&myTank.type);
        n += 1;
    }

    Sleep(500);
    system("cls");
}

void endWinScreen(int levelNum)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED|BACKGROUND_BLUE);
    GoToxy(15,19);
    printf("End of Level %d", levelNum);
    Sleep(500);
    GoToxy(17,20);
    printf("Score: %d", score);
    Sleep(500);
    GoToxy(13,21);
    printf("Starting Level %d...", levelNum+1);
}

void endLoseScreen()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED|BACKGROUND_BLUE);
    GoToxy(17,19);
    printf("You lost!");
    Sleep(500);
    GoToxy(16,20);
    printf("Score: %d", score);
    Sleep(500);
    GoToxy(13,21);
    printf("High Score: %d", highscore);
    GoToxy(13,40);
}

void selfPowerupEffect(int powerup)
{
    switch (powerup)
    {
        case POWERUPSCORE:
            myPowerupTime = 15;
            myPowerupActive = POWERUPSCORE;
            break;

        case POWERUPSPEED:
            myTank.speed = myTank.speed / 2;
            myPowerupTime = 15;
            myPowerupActive = POWERUPSPEED;
            break;

        case POWERUPWATER: 
            myPowerupTime = 20;
            myPowerupActive = POWERUPWATER;
            break;

        case POWERUPEXPLOSIVE:
            myPowerupTime = 12;
            myPowerupActive = POWERUPEXPLOSIVE;
            break;

        case POWERUPPOWER:
            myTank.bulletPower = myTank.bulletPower + 2;
            myPowerupTime = 15;
            myPowerupActive = POWERUPPOWER;
            break;

        case POWERUPHEAL:
            myTank.health += 2;
            updateHealth();
            break;

        case POWERUPDEFENSE:
            myPowerupTime = 18;
            myPowerupActive = POWERUPDEFENSE;
            defensePowerup(1);
            break;

        case POWERUPLIFE:
            lives += 1;
            updateLives();
            break;

        case POWERUPSHIELD:
            myPowerupTime = 30;
            myPowerupActive = POWERUPSHIELD;
            myTank.health += 5;
            updateHealth();
            break;

        case POWERUPVISION:
            myPowerupTime = 25;
            myPowerupActive = POWERUPVISION;
            break;
    }
}

void checkETankPowerup(EnemyTank *tank)
{
    if (tank->powerupTime <= 0.0 && tank->powerupTime > -0.9)
    {
        switch (tank->powerupActive)
        {
            case POWERUPSPEED:
                tank->speed = tank->speed * 2;
                tank->powerupActive = -1;
                tank->powerupTime = -1;
                break;

            case POWERUPWATER:
                tank->powerupActive = -1;
                tank->powerupTime = -1;
                break;

            case POWERUPEXPLOSIVE: //
                tank->powerupActive = -1;
                tank->powerupTime = -1;
                break;

            case POWERUPPOWER:
                tank->bulletPower -= 2;
                tank->powerupActive = -1;
                tank->powerupTime = -1;
                break;

            case POWERUPSHIELD:
                tank->powerupActive = -1;
                tank->powerupTime = -1;
                tank->health -= 5 - tank->shieldLost;
                break;

            case POWERUPDEFENSE:
                tank->powerupTime = -1;
                tank->powerupActive = -1;
                enemyDefensePowerup(0);
                break;

            case POWERUPVISION:
                tank->powerupActive = -1;
                tank->powerupTime = -1;
                break;
        }
    }
}

void enemyPowerupEffect(int powerup, EnemyTank *tank)
{
    if (tank->powerupActive != -1)
    {
        tank->powerupTime = 0;
        checkETankPowerup(tank);
    }
    
    powerup = POWERUPWATER;

    switch (powerup)
    {
        case POWERUPSCORE:
            score -= 500;
            updateScore();
            break;

        case POWERUPSPEED:
            tank->speed = tank->speed / 2;
            tank->powerupTime = 15;
            tank->powerupActive = POWERUPSPEED;
            break;

        case POWERUPWATER: 
            tank->powerupTime = 20;
            tank->powerupActive = POWERUPWATER;
            break;

        case POWERUPEXPLOSIVE:
            tank->powerupTime = 12;
            tank->powerupActive = POWERUPEXPLOSIVE;
            break;

        case POWERUPPOWER:
            tank->bulletPower = tank->bulletPower + 2;
            tank->powerupTime = 15;
            tank->powerupActive = POWERUPPOWER;
            break;

        case POWERUPHEAL:
            tank->health += 2;
            break;

        case POWERUPDEFENSE:
            tank->powerupTime = 18;
            tank->powerupActive = POWERUPDEFENSE;
            enemyDefensePowerup(1);
            break;

        case POWERUPLIFE:
            lives -= 1;
            break;

        case POWERUPSHIELD:
            tank->powerupTime = 60; //30
            tank->powerupActive = POWERUPSHIELD;
            tank->health += 5;
            break;

        case POWERUPVISION:
            tank->powerupTime = 25;
            tank->powerupActive = POWERUPVISION;
            break;
    }
    GoToxy(51,51);
    printf("powerup: %d      speed: %d",powerup,tank->speed);
}

void replaceWater()
{
    int x, y;
    if (myTank.direction == UP)
    {
        y = -1;
        x = 0;
    }
    else if (myTank.direction == DOWN)
    {
        y = 1;
        x = 0;
    }
    else if (myTank.direction == LEFT)
    {
        y = 0;
        x = -1;
    }
    else if (myTank.direction == RIGHT)
    {
        y = 0;
        x = 1;
    }

    if (x == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            for (int j = 0; j < waterDetected; j++)
            {
                if (waterBoard[0][j] == myTank.y-2*y && waterBoard[1][j] == myTank.x-2*x+i)
                {
                    gameBoard[myTank.y-2*y][myTank.x-2*x+i] = WATER;
                    pathFindBoard[myTank.y-2*y][myTank.x-2*x+i] = WATER;
                    GoToxy(myTank.x-2*x+i, myTank.y-2*y);
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|BACKGROUND_BLUE);
                    printf("■");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
            }
        }
    }

    if (y == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            for (int j = 0; j < waterDetected; j++)
            {
                if (waterBoard[0][j] == myTank.y-2*y+i && waterBoard[1][j] == myTank.x-2*x)
                {
                    gameBoard[myTank.y-2*y+i][myTank.x-2*x] = WATER;
                    pathFindBoard[myTank.y-2*y+i][myTank.x-2*x] = WATER;
                    GoToxy(myTank.x-2*x, myTank.y-2*y+i);
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|BACKGROUND_BLUE);
                    printf("■");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
            }
        }
    }

    GoToxy(58,58);
    printf("%d", gameBoard[27][15]);
}

void enemyReplaceWater(EnemyTank *tank)
{
    int x, y;
    if (tank->direction == UP)
    {
        y = -1;
        x = 0;
    }
    else if (tank->direction == DOWN)
    {
        y = 1;
        x = 0;
    }
    else if (tank->direction == LEFT)
    {
        y = 0;
        x = -1;
    }
    else if (tank->direction == RIGHT)
    {
        y = 0;
        x = 1;
    }

    if (x == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            for (int j = 0; j < waterDetected; j++)
            {
                if (waterBoard[0][j] == tank->y-2*y && waterBoard[1][j] == tank->x-2*x+i)
                {
                    gameBoard[tank->y-2*y][tank->x-2*x+i] = WATER;
                    pathFindBoard[tank->y-2*y][tank->x-2*x+i] = WATER;
                    GoToxy(tank->x-2*x+i, tank->y-2*y);
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|BACKGROUND_BLUE);
                    printf("■");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
            }
        }
    }

    if (y == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            for (int j = 0; j < waterDetected; j++)
            {
                if (waterBoard[0][j] == tank->y-2*y+i && waterBoard[1][j] == tank->x-2*x)
                {
                    gameBoard[tank->y-2*y+i][tank->x-2*x] = WATER;
                    pathFindBoard[tank->y-2*y+i][tank->x-2*x] = WATER;
                    GoToxy(tank->x-2*x, tank->y-2*y+i);
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|BACKGROUND_BLUE);
                    printf("■");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
            }
        }
    }

    GoToxy(58,58);
    printf("%d", gameBoard[27][15]);
}

void moveTank(int x, int y)
{
    char empty = ' ';
    if (x == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            if (gameBoard[myTank.y+2*y][myTank.x+2*x+i] >= 14)
            {
                selfPowerupEffect(gameBoard[myTank.y+2*y][myTank.x+2*x+i]); // if moves into powerup then eat powerup
            }
        }
    }

    if (y == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            if (gameBoard[myTank.y+2*y+i][myTank.x+2*x] >= 14)
            {
                selfPowerupEffect(gameBoard[myTank.y+2*y+i][myTank.x+2*x]);
            }
        }
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            GoToxy((myTank.x-1)+j , myTank.y-1+i);
            printf("%c",empty);
            gameBoard[myTank.y+j-1][myTank.x+i-1] = EMPTY;
            pathFindBoard[myTank.y+j-1][myTank.x+i-1] = EMPTY;
        } 
    }

    myTank.y += y;
    myTank.x += x;

    printTank(myTank);
}

int judgeEnemyMovement(EnemyTank tank, int x, int y)
{
    // if (gameBoard[myTank.x + 2*x][myTank.y - 2*y] != EMPTY)
    // {
    //     return 1;
    // }

    // GoToxy(myTank.x+2*x,myTank.y+2*y);
    // printf("h");

    if (x == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            if (gameBoard[tank.y+2*y][tank.x+2*x+i] >= 14)
            {
                return 2;
            }
            if (gameBoard[tank.y+2*y][tank.x+2*x+i] == WATER)
            {
                return WATER;
            }
            if (gameBoard[tank.y+2*y][tank.x+2*x+i])
            {
                return 1;
            }
        }
    }

    if (y == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            if (gameBoard[tank.y+2*y][tank.x+2*x+i] >= 14)
            {
                return 2;
            }
            if (gameBoard[tank.y+2*y][tank.x+2*x+i] == WATER)
            {
                return WATER;
            }
            if (gameBoard[tank.y+2*y+i][tank.x+2*x])
            {
                return 1;
            }
        }
    }

    return 0;
}

void moveEnemyTank(EnemyTank *tank)
{
    char empty = ' ';
    int y, x;
    int waterThrough = 0;

    int direction = rand() % 4;
    (*tank).direction = direction;

    switch (direction)
    {
        case DOWN:
            y = 1;
            x = 0;
            break;

        case UP:
            y = -1;
            x = 0;
            break;

        case RIGHT:
            y = 0;
            x = 1;
            break;

        case LEFT:
            y = 0;
            x = -1; 
            break;
    }

    if (x == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            if (gameBoard[tank->y+2*y][tank->x+2*x+i] >= 14)
            {
                enemyPowerupEffect(gameBoard[tank->y+2*y][tank->x+2*x+i], tank);
            }
        }
    }

    if (y == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            if (gameBoard[tank->y+2*y+i][tank->x+2*x] >= 14)
            {
                enemyPowerupEffect(gameBoard[tank->y+2*y+i][tank->x+2*x], tank);
            }
        }
    }
    
    if (judgeEnemyMovement(*tank, x, y) == WATER && tank->powerupActive == POWERUPWATER)
    {
        waterThrough = 1;
    }

    GoToxy(20, 50);
    printf("water through: %d", waterThrough);

    if (judgeEnemyMovement(*tank, x, y) == 0 || judgeEnemyMovement(*tank, x, y) == 2 || waterThrough == 1)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                GoToxy((tank->x-1)+j, tank->y-1+i);
                printf("%c",empty);
                gameBoard[tank->y+j-1][tank->x+i-1] = EMPTY;
                pathFindBoard[tank->y+j-1][tank->x+i-1] = EMPTY;
            }
        }

        (*tank).y += y;
        (*tank).x += x;

        printEnemyTank(*tank);
    }
}

void myBulletSpawning(MyTank tank)
{
    char clear = ' ';
    if (myBullet.available == 1)
    {
        int x, y;
        if (tank.direction == UP)
        {
            x = tank.x;
            y = tank.y - 2;
        }
        else if (tank.direction == DOWN)
        {
            x = tank.x;
            y = tank.y + 2;
        }
        else if (tank.direction == LEFT)
        {
            x = tank.x - 2;
            y = tank.y;
        }
        else if (tank.direction == RIGHT)
        {
            x = tank.x + 2;
            y = tank.y;
        }

        if (gameBoard[y][x] == REGWALL)
        {
            gameBoard[y][x] = EMPTY;
            pathFindBoard[y][x] = EMPTY;
            GoToxy(x,y);
            printf("%c", clear);
            myBullet.available = 1;
        }
        else if (gameBoard[y][x] == POWERWALL)
        {
            gameBoard[y][x] = EMPTY;
            pathFindBoard[y][x] = EMPTY;
            GoToxy(x,y);
            printf("%c", clear);
            int powerupType = rand() % 10;
            gameBoard[y][x] = powerupType + 14;
            pathFindBoard[y][x] = powerupType + 14;
            initiatePowerup(x, y, powerupType);
            myBullet.available = 1;
        }
        else if (gameBoard[y][x] == EMPTY)
        {
            gameBoard[y][x] = MYBULLET;
            myBullet.x = x;
            myBullet.y = y;
            myBullet.speed = tank.bulletSpeed;
            myBullet.power = tank.bulletPower;
            myBullet.direction = tank.direction;
            printBullet(myBullet);
            myBullet.available = 0;
        }
    }
}

void enemyBulletSpawning(EnemyTank tank, Bullet* bullet)
{
    char clear = ' ';

    if (bullet->available == 1)
    {
        int x, y;
        if (tank.direction == UP)
        {
            x = tank.x;
            y = tank.y - 2;
        }
        else if (tank.direction == DOWN)
        {
            x = tank.x;
            y = tank.y + 2;
        }
        else if (tank.direction == LEFT)
        {
            x = tank.x - 2;
            y = tank.y;
        }
        else if (tank.direction == RIGHT)
        {
            x = tank.x + 2;
            y = tank.y;
        }

        if (gameBoard[y][x] == REGWALL)
        {
            gameBoard[y][x] = EMPTY;
            pathFindBoard[y][x] = EMPTY;
            GoToxy(x,y);
            printf("%c", clear);
            bullet->available = 1;
        }
        else if (gameBoard[y][x] == EMPTY)
        {
            gameBoard[y][x] = MYBULLET;
            bullet->x = x;
            bullet->y = y;
            bullet->speed = tank.bulletSpeed;
            bullet->power = tank.bulletPower;
            bullet->direction = tank.direction;
            printBullet(*bullet);
            bullet->available = 0;
        }
    }
}

int judgeBulletMovement(int x, int y)
{
    if (gameBoard[y][x] == EMPTY)
    {
        return 2;
    }
    else if (gameBoard[y][x] == REGWALL)
    {
        return 1;
    }
    else if (gameBoard[y][x] == ETANK)
    {
        return -1;
    }
    else if (gameBoard[y][x] == PTANK)
    {
        return -2;
    }
    else if (gameBoard[y][x] == HOME)
    {
        return -3;
    }
    else if (gameBoard[y][x]== POWERWALL) 
    {
        return -4;
    }
    else if (gameBoard[y][x] == WATER)
    {
        return -5;
    }
    else
    {
        return 0;
    }
}

int judgeTank(int x, int y)
{
    double tank1dis = sqrt(pow((tank1.x - x), 2) + pow((tank1.y - y), 2));
    double tank2dis = sqrt(pow((tank2.x - x), 2) + pow((tank2.y - y), 2));
    double tank3dis = sqrt(pow((tank3.x - x), 2) + pow((tank3.y - y), 2));

    if (tank1dis < tank2dis && tank1dis < tank3dis)
	{
		return 1;
	}
	else if (tank2dis < tank3dis)
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

void moveEnemyBullet(EnemyTank tank, Bullet* bullet)
{
    char clear = ' ';

    int bulletNum;
    int tempReplaceWater;
    if (tank.number == 1)
    {
        bulletNum = 11;
        tempReplaceWater = oneReplaceWater;
    }
    else if (tank.number == 2)
    {
        bulletNum = 12;
        tempReplaceWater = twoReplaceWater;
    }
    else if (tank.number == 3)
    {
        bulletNum = 13;
        tempReplaceWater = threeReplaceWater;
    }

    GoToxy(bullet->x, bullet->y);
    printf("%c",clear);
    gameBoard[bullet->y][bullet->x] = EMPTY;
    pathFindBoard[bullet->y][bullet->x] = EMPTY;

    if (tempReplaceWater == 1)
    {
        GoToxy(bullet->x, bullet->y);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|BACKGROUND_BLUE);
        printf("■");
        gameBoard[bullet->y][bullet->x] = WATER;
        pathFindBoard[bullet->y][bullet->x] = WATER;
        if (tank.number == 1)
        {
            oneReplaceWater = 0;
        }
        else if (tank.number == 2)
        {
            twoReplaceWater = 0;
        }
        else if (tank.number == 3)
        {
            threeReplaceWater = 0;
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    }

    int x, y;
    if (bullet->direction == UP)
    {
        x = bullet->x;
        y = bullet->y - 1;
    }
    else if (bullet->direction == DOWN)
    {
        x = bullet->x;
        y = bullet->y + 1;
    }
    else if (bullet->direction == LEFT)
    {
        x = bullet->x - 1;
        y = bullet->y;
    }
    else if (bullet->direction == RIGHT)
    {
        x = bullet->x + 1;
        y = bullet->y;
    }

    if (judgeBulletMovement(x, y) == 2)
    {
        gameBoard[y][x] = MYBULLET;
        bullet->x = x;
        bullet->y = y;
        printBullet(*bullet);
    }
    else if (judgeBulletMovement(x, y) == 1)
    {
        gameBoard[y][x] = EMPTY;
        pathFindBoard[y][x] = EMPTY;
        GoToxy(x,y);
        printf("%c", clear);
        bullet->available = 1;
        if (tank.powerupActive == POWERUPEXPLOSIVE)
        {
            gameBoard[y-1][x] = EMPTY;
            pathFindBoard[y-1][x] = EMPTY;
            gameBoard[y+1][x] = EMPTY;
            pathFindBoard[y+1][x] = EMPTY;
            gameBoard[y][x-1] = EMPTY;
            pathFindBoard[y][x-1] = EMPTY;
            gameBoard[y][x+1] = EMPTY;
            pathFindBoard[y][x+1] = EMPTY;
            GoToxy(x+1,y);
            printf("%c", clear);
            GoToxy(x-1,y);
            printf("%c", clear);
            GoToxy(x,y+1);
            printf("%c", clear);
            GoToxy(x,y-1);
            printf("%c", clear);
        }
    }
    else if (judgeBulletMovement(x, y) == -2)
    {
        myTank.health -= bullet->power;
        if (myPowerupActive == POWERUPSHIELD)
        {
            shieldLost += bullet->power;
        }
        if (shieldLost >= 5)
        {
            myPowerupTime = -1;
            myPowerupActive = -1;
            shieldLost = 0;
        }
        if (myTank.health <= 0)
        {
            myTank.health = 0;
            lives -= 1;
            clearSpawnTank();
            updateLives();
        }
        updateHealth();
        bullet->available = 1;
    }
    else if (judgeBulletMovement(x, y) == -3)
    {
        lives = 0;
    }
    else if (judgeBulletMovement(x, y) == -4)
    {
        gameBoard[y][x] = EMPTY;
        pathFindBoard[y][x] = EMPTY;
        GoToxy(x,y);
        printf("%c", clear);
        int powerupType = rand() % 10;
        gameBoard[y][x] = powerupType + 14;
        pathFindBoard[y][x] = powerupType + 14;
        initiatePowerup(x, y, powerupType);
        bullet->available = 1;
    }
    else if (judgeBulletMovement(x, y) == -5)
    {
        // gameBoard[y][x] = MYBULLET;
        bullet->x = x;
        bullet->y = y;
        // printBullet(myBullet);
        if (tank.number == 1)
        {
            oneReplaceWater = 1;
        }
        else if (tank.number == 2)
        {
            twoReplaceWater = 1;
        }
        else
        {
            threeReplaceWater = 1;
        }
    }
    else
    {
        bullet->available = 1;
    }
}

void moveSelfBullet()
{
    char clear = ' ';

    GoToxy(myBullet.x, myBullet.y);
    printf("%c",clear);
    gameBoard[myBullet.y][myBullet.x] = EMPTY;
    pathFindBoard[myBullet.y][myBullet.x] = EMPTY;

    if (myReplaceWater == 1)
    {
        GoToxy(myBullet.x, myBullet.y);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|BACKGROUND_BLUE);
        printf("■");
        gameBoard[myBullet.y][myBullet.x] = WATER;
        pathFindBoard[myBullet.y][myBullet.x] = WATER;
        myReplaceWater = 0;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    }

    int x, y;
    if (myBullet.direction == UP)
    {
        x = myBullet.x;
        y = myBullet.y - 1;
    }
    else if (myBullet.direction == DOWN)
    {
        x = myBullet.x;
        y = myBullet.y + 1;
    }
    else if (myBullet.direction == LEFT)
    {
        x = myBullet.x - 1;
        y = myBullet.y;
    }
    else if (myBullet.direction == RIGHT)
    {
        x = myBullet.x + 1;
        y = myBullet.y;
    }

    if (judgeBulletMovement(x, y) == 2)
    {
        gameBoard[y][x] = MYBULLET;
        myBullet.x = x;
        myBullet.y = y;
        printBullet(myBullet);
    }
    else if (judgeBulletMovement(x, y) == 1)
    {
        gameBoard[y][x] = EMPTY;
        pathFindBoard[y][x] = EMPTY;
        GoToxy(x,y);
        printf("%c", clear);
        myBullet.available = 1;
        if (myPowerupActive == POWERUPEXPLOSIVE)
        {
            gameBoard[y-1][x] = EMPTY;
            pathFindBoard[y-1][x] = EMPTY;
            gameBoard[y+1][x] = EMPTY;
            pathFindBoard[y+1][x] = EMPTY;
            gameBoard[y][x-1] = EMPTY;
            pathFindBoard[y][x-1] = EMPTY;
            gameBoard[y][x+1] = EMPTY;
            pathFindBoard[y][x+1] = EMPTY;
            GoToxy(x+1,y);
            printf("%c", clear);
            GoToxy(x-1,y);
            printf("%c", clear);
            GoToxy(x,y+1);
            printf("%c", clear);
            GoToxy(x,y-1);
            printf("%c", clear);
        }
    }
    else if (judgeBulletMovement(x, y) == -1)
    {
        int result = judgeTank(x,y);
        if (result == 1)
        {
            tank1.health -= myTank.bulletPower;
            if (tank1.powerupActive == POWERUPSHIELD)
            {
                tank1.shieldLost += myTank.bulletPower;
            }
            if (tank1.shieldLost >= 5)
            {
                tank1.powerupTime = -1;
                tank1.powerupActive = -1;
                tank1.shieldLost = 0;
            }
            if (tank1.health <= 0)
            {
                tanksRemaining -= 1;
                tanksOnField -= 1;
                score += tank1.scoreGiven;
                if (myPowerupActive == POWERUPSCORE)
                {
                    score += tank1.scoreGiven;
                }
                clearEnemyTank(tank1);
            }
        }
        else if (result == 2)
        {
            tank2.health -= myTank.bulletPower;
            if (tank2.powerupActive == POWERUPSHIELD)
            {
                tank2.shieldLost += myTank.bulletPower;
            }
            if (tank2.shieldLost >= 5)
            {
                tank2.powerupTime = -1;
                tank2.powerupActive = -1;
                tank2.shieldLost = 0;
            }
            if (tank2.health <= 0)
            {
                tanksRemaining -= 1;
                tanksOnField -= 1;
                score += tank2.scoreGiven;
                if (myPowerupActive == POWERUPSCORE)
                {
                    score += tank2.scoreGiven;
                }
                clearEnemyTank(tank2);
            }
        }
        else
        {
            tank3.health -= myTank.bulletPower;
            if (tank3.powerupActive == POWERUPSHIELD)
            {
                tank3.shieldLost += myTank.bulletPower;
            }
            if (tank3.shieldLost >= 5)
            {
                tank3.powerupTime = -1;
                tank3.powerupActive = -1;
                tank3.shieldLost = 0;
            }
            if (tank3.health <= 0)
            {
                tanksRemaining -= 1;
                tanksOnField -= 1;
                score += tank3.scoreGiven;
                if (myPowerupActive == POWERUPSCORE)
                {
                    score += tank3.scoreGiven;
                }
                clearEnemyTank(tank3);
            }
        }

        myBullet.available = 1;
        updateTanksLeft();
        updateScore();
    }
    else if (judgeBulletMovement(x, y) == -4)
    {
        gameBoard[y][x] = EMPTY;
        pathFindBoard[y][x] = EMPTY;
        GoToxy(x,y);
        printf("%c", clear);
        int powerupType = rand() % 10;
        gameBoard[y][x] = powerupType + 14;
        pathFindBoard[y][x] = powerupType + 14;
        initiatePowerup(x, y, powerupType);
        myBullet.available = 1;
    }
    else if (judgeBulletMovement(x, y) == -5)
    {
        // gameBoard[y][x] = MYBULLET;
        myBullet.x = x;
        myBullet.y = y;
        // printBullet(myBullet);
        myReplaceWater = 1;
    }
    else
    {
        myBullet.available = 1;
    }
}

void getInput(int *x, int *y, int *xG, int *yG)
{
    if (kbhit())
    {
        int ch = getch();
        
        switch (ch)
        {
            case UPKEY:
                myTank.direction = UP;
                *x = 0;
                *y = -1;
                *xG = 0;
                *yG = -1;
                break;

            case DOWNKEY:
                myTank.direction = DOWN;
                *x = 0;
                *y = 1;
                *xG = 0;
                *yG = 1;
                break;

            case LEFTKEY:
                myTank.direction = LEFT;
                *x = -1;
                *y = 0;
                *xG = -1;
                *yG = 0;
                break;

            case RIGHTKEY:
                myTank.direction = RIGHT;
                *x = 1;
                *y = 0;
                *xG = 1;
                *yG = 0;
                break;  
                
            case SMALLFKEY:
                myBulletSpawning(myTank);
                break;

            case BIGFKEY:
                myBulletSpawning(myTank);
                break;

            default:
                *x = 0;
                *y = 0;
        }
    }

    else
    {
        *x = 0;
        *y = 0;
    }
}

int judgeEnemyTankSpawning (EnemyTank *tank)
{
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (gameBoard[tank->y+j][tank->x+i] != 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

void tankSpawning(EnemyTank *tank)
{
    int tankType;

    tankType = rand() % 2;
    while (numTanks[tankType] < 0)
    {
        tankType = rand() % 2;
    }

    numTanks[tankType] -= 1;

    // create enemytank object

    tank->y = 2;
    tank->x = rand() % (COL + 1);
    
    while (judgeEnemyTankSpawning(tank) == 0)
    {
        tank->x = rand() % (COL + 1);
    }

    tank->direction = DOWN;
    tank->type = tankType;
    
    switch (tank->type)
    {
        case 0:
            tank->speed = 10; //8
            tank->bulletPower = 3; //2
            tank->bulletSpeed = 8;
            tank->health = 3; // 2
            tank->scoreGiven = 500;
            tank->shape = 0;
            break;
        
        case 1:
            tank->speed = 10; //2
            tank->bulletPower = 3; //1
            tank->bulletSpeed = 2;
            tank->health = 3; // 1
            tank->scoreGiven = 650;
            tank->shape = 1;
            break;
    }

    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            gameBoard[tank->y+j][tank->x+i] = ETANK;
            pathFindBoard[tank->y+j][tank->x+i] = ETANK;
        }
    }

    printEnemyTank(*tank);
    tanksOnField += 1;
}


int judgeMovement(int x, int y)
{
    if (x == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            if (gameBoard[myTank.y+2*y][myTank.x+2*x+i] >= 14)
            {
                return 2;
            }
            if (gameBoard[myTank.y+2*y][myTank.x+2*x+i] == WATER)
            {
                return 3;
            }
            if (gameBoard[myTank.y+2*y][myTank.x+2*x+i])
            {
                return 1;
            }
        }
    }

    if (y == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            if (gameBoard[myTank.y+2*y+i][myTank.x+2*x] >= 14)
            {
                return 2;
            }
            if (gameBoard[myTank.y+2*y+i][myTank.x+2*x] == WATER)
            {
                return 3;
            }
            if (gameBoard[myTank.y+2*y+i][myTank.x+2*x])
            {
                return 1;
            }
        }
    }

    return 0;
}

// 60 

void gameLoop()
{
    int x, y, xG, yG;
    int tankType;
    int powerupConsumed;
    xG = 0;
    yG = -1;
    x = 0;
    y = 0;
    while (1)
    {
        cycle += 1;

        if (score > highscore)
        {
            updateHighScore();
        }

        if (tanksOnField < 3 && tanksRemaining > 2)
        {
            if (tank1.health <= 0)
            {   
                tankSpawning(&tank1);
                bullet1.power = tank1.bulletPower;
                bullet1.speed = tank1.speed;
            }
            // else if (tank2.health <= 0)
            // {
            //     tankSpawning(&tank2);
            //     bullet2.power = tank2.bulletPower;
            //     bullet2.speed = tank2.speed;
            // }
            // else if (tank3.health <= 0)
            // {
            //     tankSpawning(&tank3);
            //     bullet3.power = tank3.bulletPower;
            //     bullet3.speed = tank3.speed;
            // }
        }     // -1,0|0,0  up,none 

        if (cycle % myTank.speed == 0)
        {
            getInput(&x, &y, &xG, &yG);

            if (judgeMovement(xG, yG) == 0)
            {
                moveTank(x, y);
            }
            else if (judgeMovement(xG, yG) == 3)
            {
                if (myPowerupActive == POWERUPWATER)
                {
                    moveTank(x, y);
                }
                else
                {
                    printTank(myTank);
                }
            }
            else if (judgeMovement(xG, yG) == 2)
            {   
                moveTank(x, y);
            }
            else
            {
                printTank(myTank);
            }
        }

        if (tank1.health > 0 && cycle % tank1.speed == 0)
        {
            moveEnemyTank(&tank1);
            enemyBulletSpawning(tank1, &bullet1);
        }

        if (bullet1.available == 0 && cycle % bullet1.speed == 0) // bullet1.available = 0 means bullet is in the air
        {
            moveEnemyBullet(tank1, &bullet1);
        }

        if (tank2.health > 0 && cycle % tank2.speed == 0)
        {
            moveEnemyTank(&tank2);
            enemyBulletSpawning(tank2, &bullet2);
        }

        if (bullet2.available == 0 && cycle % bullet2.speed == 0)
        {
            moveEnemyBullet(tank2, &bullet2);
        }

        if (tank3.health > 0 && cycle % tank3.speed == 0)
        {
            moveEnemyTank(&tank3);
            enemyBulletSpawning(tank3, &bullet3);
        }

        if (bullet3.available == 0 && cycle % bullet3.speed == 0)
        {
            moveEnemyBullet(tank3, &bullet3);
        }

        
        // GoToxy(0,ROW+5);
        // printf("%d|-----|%d\n%d|-----|%d\n", xG, yG,myTank.x,myTank.y);
        // for (int i = 0; i < ROW; i++)
        // {
        //     for (int j = 0; j < COL; j++)
        //     {
        //         printf("%d", gameBoard[i][j]);
        //     }
        //     printf("\n");
        // }

        if (myBullet.available == 0 && cycle % myTank.bulletSpeed == 0)
        {
            moveSelfBullet();
        }

        //Sleep(20);

        if (tanksRemaining <= 0)
        {
            break;
        }

        if (lives <= 0)
        {
            endLoseScreen();
            Sleep(500);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            exit(1); // losing
        }
        
        if (myPowerupTime > -1)
        {
            myPowerupTime -= 0.035;
        }

        if (tank1.powerupTime > -1)
        {
            tank1.powerupTime -= 0.035;
        }

        if (tank2.powerupTime > -1)
        {
            tank2.powerupTime -= 0.035;
        }

        if (tank3.powerupTime > -1)
        {
            tank3.powerupTime -= 0.035;
        }

        if (myPowerupTime <= 0.0 && myPowerupTime > -0.9)
        {
            GoToxy(60, 60);
            printf("end");
            switch (myPowerupActive)
            {
                case POWERUPSCORE:
                    myPowerupActive = -1;
                    myPowerupTime = -1;
                    break;

                case POWERUPSPEED:
                    myTank.speed = myTank.speed * 2;
                    myPowerupActive = -1;
                    myPowerupTime = -1;
                    break;

                case POWERUPWATER:
                    myPowerupActive = -1;
                    myPowerupTime = -1;
                    break;

                case POWERUPEXPLOSIVE:
                    myPowerupActive = -1;
                    myPowerupTime = -1;
                    break;

                case POWERUPPOWER:
                    myTank.bulletPower -= 2;
                    myPowerupActive = -1;
                    myPowerupTime = -1;
                    break;

                case POWERUPDEFENSE:
                    myPowerupActive = -1;
                    myPowerupTime = -1;
                    defensePowerup(0);
                    break;

                case POWERUPSHIELD:
                    myPowerupActive = -1;
                    myPowerupTime = -1;
                    myTank.health -= 5 - shieldLost;
                    updateHealth();
                    break;

                case POWERUPVISION:
                    myPowerupActive = -1;
                    myPowerupTime = -1;
                    break;
            }
        }

        checkETankPowerup(&tank1);
        checkETankPowerup(&tank2);
        checkETankPowerup(&tank3);

        replaceWater();
        enemyReplaceWater(&tank1);
        enemyReplaceWater(&tank2);
        enemyReplaceWater(&tank3);

        Sleep(5); //25

        GoToxy(53,53);
        printf("time: %lf       health: %d", tank1.powerupTime, tank1.powerupActive);
        GoToxy(54,54);
        printf("time: %lf       health: %d", tank2.powerupTime, tank2.powerupActive);
        GoToxy(55,55);
        printf("time: %lf       health: %d", tank3.powerupTime, tank3.powerupActive);
    }
    endWinScreen(1);
}

void game()
{
    tank1.health = 0;
    tank2.health = 0;
    tank3.health = 0;
    startScreen();
    hideCursor(1);
    initiateMap(1);
    lives = myTank.lives;
    displayMap(1);
    gameLoop();
}

void main()
{
    myBullet.available = 1;
    bullet1.available = 1;
    bullet2.available = 1;
    bullet3.available = 1;
    tank1.number = 1;
    tank2.number = 2;
    tank3.number = 3;
    tank1.powerupTime = -1;
    tank2.powerupTime = -1;
    tank3.powerupTime = -1;
    tank1.powerupActive = -1;
    tank2.powerupActive = -1;
    tank3.powerupActive = -1;
    tank1.shieldLost = 0;
    tank2.shieldLost = 0;
    tank3.shieldLost = 0;
    srand(time(NULL));
    getHighScore();
    system("cls");
    game();
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    GoToxy(COL*2,ROW);
    hideCursor(0);
}