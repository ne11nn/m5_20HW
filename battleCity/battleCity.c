#include "battleCity.h"

MyTank myTank;

EnemyTank tank1;
EnemyTank tank2;
EnemyTank tank3;

Bullet myBullet;
Bullet bullet1;
Bullet bullet2;
Bullet bullet3;

int lives;

void getHighScore()
{
    FILE *fp = NULL;
    fp = fopen("D:/Coding/C/cClass/battleCity/scoreinfo.txt", "r");

    fscanf(fp, "%d", &highscore);
    fclose(fp);
}

void updateHighScore()
{
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

void printBullet(Bullet bullet)
{
    GoToxy(bullet.x, bullet.y);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN);
    printf("O");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
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
            myTank.speed = 3;
            myTank.bulletPower = 1;
            myTank.bulletSpeed = 2;
            myTank.health = 3;
            myTank.lives = 4;
            break;

        case 2:
            myTank.speed = 1;
            myTank.bulletPower = 1;
            myTank.bulletSpeed = 2;
            myTank.health = 3;
            myTank.lives = 3;
            break;

        case 3:
            myTank.speed = 2;
            myTank.bulletPower = 2;
            myTank.bulletSpeed = 2;
            myTank.health = 2;
            myTank.lives = 3;
            break;

        case 4:
            myTank.speed = 4;
            myTank.bulletPower = 2;
            myTank.bulletSpeed = 3;
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
            //printf("%d ", gameBoard[i][j]);
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
    printf("Lives left: %d", myTank.lives);
    GoToxy(COL + 2, 19);
    printf("Controls:");
    GoToxy(COL + 2, 21);
    printf("↑: Up");
    GoToxy(COL + 2, 22);
    printf("↓: Down");
    GoToxy(COL + 2, 23);
    printf("←: Left");
    GoToxy(COL + 2, 24);
    printf("→: Right");
    GoToxy(COL + 2, 25);
    printf("F: Shoot");
    GoToxy(COL + 2, 27);
    printf("Kill all tanks before you");
    GoToxy(COL + 2, 28);
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

void endScreen(int levelNum)
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

void moveTank(int x, int y)
{
    char empty = ' ';

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            GoToxy((myTank.x-1)+j , myTank.y-1+i);
            printf("%c",empty);
            gameBoard[myTank.y+j-1][myTank.x+i-1] = EMPTY;
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
    if (judgeEnemyMovement(*tank,x,y) == 0)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                GoToxy((tank->x-1)+j, tank->y-1+i);
                printf("%c",empty);
                gameBoard[tank->y+j-1][tank->x+i-1] = EMPTY;
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
            GoToxy(x,y);
            printf("%c", clear);
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
    if (gameBoard[y][x] == EMPTY || gameBoard[y][x] == WATER)
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
    if (tank.number == 1)
    {
        bulletNum = 11;
    }
    else if (tank.number == 2)
    {
        bulletNum = 12;
    }
    else if (tank.number == 3)
    {
        bulletNum = 13;
    }

    GoToxy(bullet->x, bullet->y);
    printf("%c",clear);
    gameBoard[bullet->y][bullet->x] = EMPTY;

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
        GoToxy(x,y);
        printf("%c", clear);
        bullet->available = 1;
    }
    else if (judgeBulletMovement(x, y) == -2)
    {
        myTank.health -= bullet->power;
        if (myTank.health <= 0)
        {
            lives -= 1;
            clearSpawnTank();
            updateLives();
        }
        bullet->available = 1;
    }
    else if (judgeBulletMovement(x, y) == -3)
    {
        lives = 0;
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
        GoToxy(x,y);
        printf("%c", clear);
        myBullet.available = 1;
    }
    else if (judgeBulletMovement(x, y) == -1)
    {
        int result = judgeTank(x,y);
        if (result == 1)
        {
            tank1.health -= myTank.bulletPower;
            if (tank1.health <= 0)
            {
                tanksRemaining -= 1;
                tanksOnField -= 1;
                score += tank1.scoreGiven;
                clearEnemyTank(tank1);
            }
        }
        else if (result == 2)
        {
            tank2.health -= myTank.bulletPower;
            if (tank2.health <= 0)
            {
                tanksRemaining -= 1;
                tanksOnField -= 1;
                score += tank2.scoreGiven;
                clearEnemyTank(tank2);
            }
        }
        else
        {
            tank3.health -= myTank.bulletPower;
            if (tank3.health <= 0)
            {
                tanksRemaining -= 1;
                tanksOnField -= 1;
                score += tank3.scoreGiven;
                clearEnemyTank(tank3);
            }
        }

        myBullet.available = 1;
        updateTanksLeft();
        updateScore();
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
            tank->speed = 4;
            tank->bulletPower = 2;
            tank->bulletSpeed = 4;
            tank->health = 2;
            tank->scoreGiven = 500;
            tank->shape = 0;
            break;
        
        case 1:
            tank->speed = 1;
            tank->bulletPower = 1;
            tank->bulletSpeed = 1;
            tank->health = 1;
            tank->scoreGiven = 650;
            tank->shape = 1;
            break;
    }

    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            gameBoard[tank->y+j][tank->x+i] = ETANK;
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
            else if (tank2.health <= 0)
            {
                tankSpawning(&tank2);
                bullet2.power = tank2.bulletPower;
                bullet2.speed = tank2.speed;
            }
            else if (tank3.health <= 0)
            {
                tankSpawning(&tank3);
                bullet3.power = tank3.bulletPower;
                bullet3.speed = tank3.speed;
            }
        }     // -1,0|0,0  up,none 

        if (cycle % myTank.speed == 0)
        {
            getInput(&x, &y, &xG, &yG);

            if (judgeMovement(xG, yG) == 0)
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

        // if (bullet1.available == 0 && cycle % bullet1.speed == 0) // bullet1.available = 0 means bullet is in the air
        // {
        //     moveEnemyBullet(tank1, &bullet1);
        // }

        if (tank2.health > 0 && cycle % tank2.speed == 0)
        {
            moveEnemyTank(&tank2);
            enemyBulletSpawning(tank2, &bullet2);
        }

        // if (bullet2.available == 0 && cycle % bullet2.speed == 0)
        // {
        //     moveEnemyBullet(tank2, &bullet2);
        // }

        if (tank3.health > 0 && cycle % tank3.speed == 0)
        {
            moveEnemyTank(&tank3);
            enemyBulletSpawning(tank3, &bullet3);
        }

        // if (bullet3.available == 0 && cycle % bullet3.speed == 0)
        // {
        //     moveEnemyBullet(tank3, &bullet3);
        // }

        
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

        Sleep(20);

        if (tanksRemaining <= 0)
        {
            break;
        }

        if (lives <= 0)
        {
            exit(1); // losing
        }
        
    }
    endScreen(1);
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
    srand(time(NULL));
    getHighScore();
    system("cls");
    game();
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    GoToxy(COL*2,ROW);
    hideCursor(0);
}