//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include "math.h"


// ������ ������ ����
typedef struct {
    float x, y, width, height, speed;
    HBITMAP hBitmap;
} racket_type;


racket_type racket;
enum class entity { empty, enemy, lootchest, terminal };

typedef struct {
    float x, y, width, height;
    HBITMAP hBitmap;
    entity type;
} enemycco;
const int enemycout = 22;
enemycco enemy1[enemycout];

typedef struct {
    float x, y, width, height;
    HBITMAP hBitmap;
} sprite;
const int splittedEnemyI = 3;
const int splittedEnemyj = 3;
sprite splittedEnemy[3][3];// �������� ��������








struct {
    float x;
    HBITMAP hBitmap;//����� � ������� ������� ����������
} enemy;

struct {
    float x, y, rad, dx, dy, speed;
    HBITMAP hBitmap;//����� � ������� ������
} ball;

struct {
    int score, balls;//���������� ��������� ����� � ���������� "������"
    bool action = false;//��������� - �������� (����� ������ ������ ������) ��� ����
} game;

struct {
    HWND hWnd;//����� ����
    HDC device_context, context;// ��� ��������� ���������� (��� �����������)
    int width, height;//���� �������� ������� ���� ������� ������� ���������
} window;

struct {
    HWND hWnd_battle;//����� ����
    HDC device_context_battle, context_battle;// ��� ��������� ���������� (��� �����������)
    int width_battle, height_battle;//���� �������� ������� ���� ������� ������� ���������
} window_battle;

POINT p;

enum class GameMode {map,battle,loot, terminal};
GameMode game_mode = GameMode::map;


HBITMAP hBack;// ����� ��� �������� �����������
HBITMAP hBattleBack, hPW_butt, hSW_butt, hDW_butt;
bool L_Mouse, R_Mouse;
//c����� ����

void InitGame()
{
    //� ���� ������ ��������� ������� � ������� ������� gdi
    //���� ������������� - ����� ������ ������ ����� � .exe 
    //��������� ������ LoadImageA ��������� � ������� ��������, ��������� �������� ����� ������������� � ������� ���� �������
    ball.hBitmap = (HBITMAP)LoadImageA(NULL, "shar.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    racket.hBitmap = (HBITMAP)LoadImageA(NULL, "raketka.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hPW_butt = (HBITMAP)LoadImageA(NULL, "pw_butt.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hSW_butt = (HBITMAP)LoadImageA(NULL, "sw_butt.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hDW_butt = (HBITMAP)LoadImageA(NULL, "dw_butt.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    splittedEnemy[1][1].hBitmap = (HBITMAP)LoadImageA(NULL, "enemy1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    srand(0);

   
    int i = 0;
    float cellsize = 50;
    for (int x = 0; x < window.width / cellsize; x++) {
        for (int y = 0; y < window.height / cellsize; y++) {
            if (i >= enemycout) {
                continue;
            }
            entity Etype;
            auto rnd = (rand() % 100); 
            if (rnd < 97) {
                Etype = entity::empty;
            }
            else {
                rnd = 100 - rnd;
                Etype = (entity)rnd;
            }
            if (Etype == entity::empty) {
                continue;
            }
           enemy1[i].type = Etype;
            if (enemy1[i].type == entity::enemy)
            {
                enemy1[i].hBitmap = (HBITMAP)LoadImageA(NULL, "enemycco.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            }
            if (enemy1[i].type == entity::lootchest)
            {
                enemy1[i].hBitmap = (HBITMAP)LoadImageA(NULL, "lootchest.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            }
            if (enemy1[i].type == entity::terminal)
            {
                enemy1[i].hBitmap = (HBITMAP)LoadImageA(NULL, "terminal.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            }
          
            enemy1[i].width = cellsize;
            enemy1[i].height = cellsize;
            float screen_X = enemy1[i].width * x;
            float screen_Y = enemy1[i].height * y;
            enemy1[i].x = screen_X;
            enemy1[i].y = screen_Y;
            i++;

        }
    }



    enemy.hBitmap = (HBITMAP)LoadImageA(NULL, "raketka.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBack = (HBITMAP)LoadImageA(NULL, "phon1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBattleBack = (HBITMAP)LoadImageA(NULL, "Battlephon1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //------------------------------------------------------
   
    splittedEnemy[3][3].width = 150;
    splittedEnemy[3][3].height = 100;

    racket.width = 50;
    racket.height = 50;
    racket.speed = 17;//�������� ����������� �������
    racket.x = window.width / 2.;//������� ���������� ����
    racket.y = window.height - racket.height;//���� ���� ���� ������ - �� ������ �������

    enemy.x = racket.x;//� ���������� �������� ������ � �� �� ����� ��� � ������

    ball.dy = (rand() % 65 + 35) / 100.;//��������� ������ ������ ������
    ball.dx = -(1 - ball.dy);//��������� ������ ������ ������
    ball.speed = 30;
    ball.rad = 40;
    ball.x = racket.x;//x ���������� ������ - �� ������� �������
    ball.y = racket.y - ball.rad;//����� ����� ������ �������

    game.score = 0;
    game.balls = 9;

}

void ProcessSound(const char* name)//������������ ���������� � ������� .wav, ���� ������ ������ � ��� �� ����� ��� � ���������
{
    PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//���������� name �������� ��� �����. ���� ASYNC ��������� ����������� ���� ���������� � ����������� ���������
}

void ShowScore()
{
    return;
    //�������� �������� � �������
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "GEORGIA");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//����� ��� ������
    _itoa_s(game.score, txt, 10);//�������������� �������� ���������� � �����. ����� �������� � ���������� txt
    TextOutA(window.context, 10, 10, "Score", 5);
    TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));

    _itoa_s(game.balls, txt, 10);
    TextOutA(window.context, 10, 100, "Balls", 5);
    TextOutA(window.context, 200, 100, (LPCSTR)txt, strlen(txt));
}

void ProcessInput()
{
    if (GetAsyncKeyState(VK_LEFT)) racket.x -= racket.speed;
    if (GetAsyncKeyState(VK_RIGHT)) racket.x += racket.speed;
    if (GetAsyncKeyState(VK_UP)) racket.y -= racket.speed;
    if (GetAsyncKeyState(VK_DOWN)) racket.y += racket.speed;
    if (GetAsyncKeyState(VK_LBUTTON)) L_Mouse = true;
    if (GetAsyncKeyState(VK_RBUTTON)) R_Mouse = true;

    if (!game.action && GetAsyncKeyState(VK_SPACE))
    {
        game.action = true;
        ProcessSound("knopka-voda-vyisokii-rezkii.wav");
    }
}

void ShowBitmap(HDC hDC, int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha = false)
{
    HBITMAP hbm, hOldbm;
    HDC hMemDC;
    BITMAP bm;

    hMemDC = CreateCompatibleDC(hDC); // ������� �������� ������, ����������� � ���������� �����������
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// �������� ����������� bitmap � �������� ������

    if (hOldbm) // ���� �� ���� ������, ���������� ������
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // ���������� ������� �����������

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//��� ������� ������� ����� ����� ��������������� ��� ����������
        }
        else
        {
            StretchBlt(hDC, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // ������ ����������� bitmap
        }

        SelectObject(hMemDC, hOldbm);// ��������������� �������� ������
    }

    DeleteDC(hMemDC); // ������� �������� ������
}


void ShowLoot() {
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//������ ���
    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);
}

void ShowTerminal() {
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//������ ���
    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);
}


void ShowBattle() {
    ShowBitmap(window.context, 0, 0, window.width , window.height, hBattleBack);//������ ���
       
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ShowBitmap(window.context, splittedEnemy[i][j].x, splittedEnemy[i][j].y, splittedEnemy[i][j].height, splittedEnemy[i][j].width, splittedEnemy[i][j].hBitmap);
        }
  
    }

    ShowBitmap(window.context, 650, 950, window.width / 9.5, window.height / 13, hPW_butt);
    ShowBitmap(window.context, 850, 950, window.width / 9.5, window.height / 13, hSW_butt);
    ShowBitmap(window.context, 1050, 955, window.width / 9.5, window.height / 13, hDW_butt);


   
}




void ShowRacketAndBall()
{
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//������ ���
    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);// ������� ������
    for (int i = 0; i < enemycout; i++) {
        ShowBitmap(window.context, enemy1[i].x - enemy1[i].width / 2., enemy1[i].y, enemy1[i].width, enemy1[i].height, enemy1[i].hBitmap);
    }
    if (ball.dy < 0 && (enemy.x - racket.width / 4 > ball.x || ball.x > enemy.x + racket.width / 4))
    {
        //��������� ���������� ���������. �� ����� ����, ��������� ������� �� �����������, � �� �� ������� �������� �� ��� ������� �� ������
        //������ �����, �� ������ ������ ������ �� �������, � ������� ���������� ������� - ����������� ��� �����
        //�������� ����� ������ ���� ����� ����� �����, � ������ ���� ����� �� ��� X ������� �� ������� �������� ����� �������
        //� ���� ������, �� ��������� ���������� ������� � ������ � ��������� 9 � 1
        enemy.x = ball.x * .1 + enemy.x * .9;
    }
    ShowBitmap(window.context, p.x, p.y, ball.rad, ball.rad, ball.hBitmap);
  //  ShowBitmap(window.context, enemy.x - racket.width / 2, 0, racket.width, racket.height, enemy.hBitmap);//������� ���������
   // ShowBitmap(window.context, ball.x - ball.rad, ball.y - ball.rad, 2 * ball.rad, 2 * ball.rad, ball.hBitmap, true);// �����
}

void LimitRacket()
{
    racket.x = max(racket.x, racket.width / 2.);//���� ��������� ������ ���� ������� ������ ����, �������� �� ����
    racket.x = min(racket.x, window.width - racket.width / 2.);//���������� ��� ������� ����
    racket.y = max(racket.y, 0.);
    racket.y = min(racket.y, window.height - racket.height);
}
   

void CheckWalls()
{
    if (ball.x < ball.rad || ball.x > window.width - ball.rad)
    {
        ball.dx *= -1;
        ProcessSound("knopka-voda-vyisokii-rezkii.wav");
    }
}

void CheckRoof()
{
    if (ball.y < ball.rad + racket.height)
    {
        ball.dy *= -1;
        ProcessSound("knopka-voda-vyisokii-rezkii.wav");
    }
}

bool tail = false;

bool CheckCollision(racket_type r, enemycco e)
{
    auto dx = r.x - e.x;
    auto dy = r.y - e.y;
    auto dxy = sqrt(dx * dx + dy * dy);
   
    if (dxy < (r.width + e.height) / 2.)
    {
        return true;
    }
    return false;
}

bool CheckCollisionMouse(enemycco e)
{
    auto dx = p.x - e.x;
    auto dy = p.y - e.y;
    auto dxy = sqrt(dx * dx + dy * dy);

    if (dxy < (dxy + e.height) / 2.)
    {
        return true;
    }
    return false;
}

bool CheckCollisionMouseBattle(sprite s)
{
    auto dx = p.x - s.x;
    auto dy = p.y - s.y;
    auto dxy = sqrt(dx * dx + dy * dy);

    if (dxy < (dxy + s.height) / 2.)
    {
        return true;
    }
    return false;
}

void CheckFloor()
{
    if (ball.y > window.height - ball.rad - racket.height)//����� ������� ����� ������� - ����������� �������
    {
        if (!tail && ball.x >= racket.x - racket.width / 2. - ball.rad && ball.x <= racket.x + racket.width / 2. + ball.rad)//����� �����, � �� �� � ������ ��������� ������
        {
            game.score++;//�� ������ ������� ���� ���� ����
            ball.speed += 5. / game.score;//�� ����������� ��������� - ���������� �������� ������
            ball.dy *= -1;//������
            racket.width -= 10. / game.score;//������������� ��������� ������ ������� - ��� ���������
            ProcessSound("knopka-voda-vyisokii-rezkii.wav");//������ ���� �������
        }
        else
        {//����� �� �����

            tail = true;//����� ������ ������ ���� �������

            if (ball.y - ball.rad > window.height)//���� ����� ���� �� ������� ����
            {
                game.balls--;//��������� ���������� "������"

                ProcessSound("fail.wav");//������ ����

                if (game.balls < 0) { //�������� ������� ��������� "������"

                    MessageBoxA(window.hWnd, "game over", "", MB_OK);//������� ��������� � ���������
                    InitGame();//������������������ ����
                }

                ball.dy = (rand() % 65 + 35) / 100.;//������ ����� ��������� ������ ��� ������
                ball.dx = -(1 - ball.dy);
                ball.x = racket.x;//�������������� ���������� ������ - ������ ��� �� �������
                ball.y = racket.y - ball.rad;
                game.action = false;//���������������� ����, ���� ����� �� ������ ������
                tail = false;
            }
        }
    }
}





void ProcessRoom()
{
    //������������ �����, ������� � ���. ������� - ���� ������� ����� ���� ���������, � ������, ��� ������� �� ����� ������ ������������� ����� ������� �������� ������
    CheckWalls();
    CheckRoof();
    CheckFloor();
    
    for (int i = 0; i < enemycout; i++) {
        if (CheckCollision(racket, enemy1[i]) == true) {
            if (enemy1[i].type == entity::lootchest) {
                game_mode = GameMode::loot;
            }
            if (enemy1[i].type == entity::enemy) {
                game_mode = GameMode::battle;
            }                    
               
            if (enemy1[i].type == entity::terminal) {
                game_mode = GameMode::terminal;
            }
        }
    }
}

void ProcessBall()
{
    if (game.action)
    {
        //���� ���� � �������� ������ - ���������� �����
        ball.x += ball.dx * ball.speed;
        ball.y += ball.dy * ball.speed;
    }
    else
    {
        //����� - ����� "��������" � �������
        ball.x = racket.x;
    }
}

void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//�� ������ ���� ������� ����� ��������� ���������� ��� ���������
    window.width = r.right - r.left;//���������� ������� � ���������
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//������ �����
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//����������� ���� � ���������
    GetClientRect(window.hWnd, &r);

}
void BattleGame() {
  




    ShowBattle();//������ ��� 
   
    BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//�������� ����� � ����
    Sleep(20);//���� 16 ���������� (1/���������� ������ � �������)
    for (int i = 0; i < enemycout; i++) {
        for (int j = 0; j < enemycout; j++) {
            if (CheckCollisionMouseBattle(splittedEnemy[3][3])) {
                if (enemy1[i].type == entity::lootchest) {
                    game_mode = GameMode::loot;
                }
                if (enemy1[i].type == entity::enemy) {
                    game_mode = GameMode::battle;
                }

                if (enemy1[i].type == entity::terminal) {
                    game_mode = GameMode::terminal;
                }
            }

        }
       
    }


    ProcessInput();//����� ����������
    LimitRacket();
   
}

void TerminalGame() {
    ShowTerminal();
    BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);
    Sleep(20);
    ProcessInput();
}

void LootGame() {
    ShowLoot();
    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);
    BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//�������� ����� � ����
    Sleep(20);//���� 16 ���������� (1/���������� ������ � �������)
    ProcessInput();//����� ����������

}


void MapGame() {
    ShowRacketAndBall();//������ ���, ������� � �����
    ShowScore();
    BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//�������� ����� � ����
    Sleep(20);//���� 16 ���������� (1/���������� ������ � �������)
    for (int i = 0; i < enemycout; i++) {
       if (CheckCollisionMouse(enemy1[i]))
        {
            if (enemy1[i].type == entity::lootchest) {
                game_mode = GameMode::loot;
            }
            if (enemy1[i].type == entity::enemy) {
                game_mode = GameMode::battle;
            }

            if (enemy1[i].type == entity::terminal) {
                game_mode = GameMode::terminal;
            }
        }
    }
    
        ProcessInput();//����� ����������
    LimitRacket();//���������, ����� ������� �� ������� �� �����
    // ProcessBall();//���������� �����
    ProcessRoom();//������������ ������� �� ���� � �������, ��������� ������ � ��������
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    InitWindow();//����� �������������� ��� ��� ����� ��� ��������� � ����
    InitGame();//����� �������������� ���������� ����

    while (!GetAsyncKeyState(VK_ESCAPE))
    {

        GetCursorPos(&p);

        ScreenToClient(window.hWnd, &p);
        switch (game_mode) {
        case GameMode::map: MapGame(); break;
        case GameMode::battle: BattleGame(); break;
        case GameMode::loot: LootGame(); break;
        case GameMode::terminal : TerminalGame(); break;
        }
            

    }

}
