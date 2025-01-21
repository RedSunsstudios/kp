//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include "math.h"

// секция данных игры
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
sprite splittedEnemy[3][3];// поменять значения

struct {
    float x;
    HBITMAP hBitmap;//хэндл к спрайту ракетки противника
} enemy;

struct {
    float x, y, rad, dx, dy, speed;
    HBITMAP hBitmap;//хэндл к спрайту шарика
} ball;

struct {
    int score, balls;//количество набранных очков и оставшихся "жизней"
    bool action = false;//состояние - ожидание (игрок должен нажать пробел) или игра
} game;

struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;


POINT p;

enum class GameMode { map, battle, loot, terminal };
GameMode game_mode = GameMode::map;


typedef struct {
    float x, y, width, height;
    HBITMAP hBitmap;//хэндл к спрайту шарика
    HBITMAP hBitmapGlow;
} button;


button PW_butt, SW_butt, DW_butt, Exit_butt;
HBITMAP hBack;// хэндл для фонового изображения
HBITMAP hBattleBack;


bool L_Mouse, R_Mouse;

//cекция кода
void LoadButton(button &b, const char* imagename, const char* imagenameglow, float x, float y, float w, float h) {
    b.hBitmap = (HBITMAP)LoadImageA(NULL, imagename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    b.hBitmapGlow = (HBITMAP)LoadImageA(NULL, imagenameglow, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    b.height = w*window.height;
    b.width = h*window.width;
    b.x = window.width / 2 - b.width * x;
    b.y = window.height / 2 + b.height * y;
 }

void InitGame()
{
    //в этой секции загружаем спрайты с помощью функций gdi
    //пути относительные - файлы должны лежать рядом с .exe 
    //результат работы LoadImageA сохраняет в хэндлах битмапов, рисование спрайтов будет произовдиться с помощью этих хэндлов
    ball.hBitmap = (HBITMAP)LoadImageA(NULL, "shar.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    racket.hBitmap = (HBITMAP)LoadImageA(NULL, "raketka.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    // доделать
    
    LoadButton(PW_butt, "pw_butt.bmp", "pw_butt_glow.bmp", 1.45, 4.13, .09, .11);
    LoadButton(SW_butt, "sw_butt.bmp", "sw_butt_glow.bmp", 0.45, 4.13, .09, .11);
    LoadButton(DW_butt, "dw_butt.bmp", "dw_butt_glow.bmp", -0.55, 4.15, .09, .11);
    LoadButton(Exit_butt, "Exit_butt.bmp", "Exit_butt_glow.bmp", 12, -24, .02, .04);


   

    //splittedEnemy[1][1].hBitmap = (HBITMAP)LoadImageA(NULL, "enemy1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
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

   // splittedEnemy[3][3].width = 150;
   // splittedEnemy[3][3].height = 100;

    racket.width = 50;
    racket.height = 50;
    racket.speed = 17;//скорость перемещения ракетки
    racket.x = window.width / 2.;//ракетка посередине окна
    racket.y = window.height - racket.height;//чуть выше низа экрана - на высоту ракетки

    enemy.x = racket.x;//х координату оппонета ставим в ту же точку что и игрока

    ball.dy = (rand() % 65 + 35) / 100.;//формируем вектор полета шарика
    ball.dx = -(1 - ball.dy);//формируем вектор полета шарика
    ball.speed = 30;
    ball.rad = 40;
    ball.x = racket.x;//x координата шарика - на середие ракетки
    ball.y = racket.y - ball.rad;//шарик лежит сверху ракетки

    game.score = 0;
    game.balls = 9;

}

void ProcessSound(const char* name)//проигрывание аудиофайла в формате .wav, файл должен лежать в той же папке где и программа
{
    PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//переменная name содежрит имя файла. флаг ASYNC позволяет проигрывать звук паралельно с исполнением программы
}

void ShowScore()
{
    return;
    //поиграем шрифтами и цветами
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "GEORGIA");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//буфер для текста
    _itoa_s(game.score, txt, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
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
    if (GetAsyncKeyState(VK_LBUTTON)) {
        L_Mouse = true;
    }
    else {
        L_Mouse = false;
    }
    if (GetAsyncKeyState(VK_RBUTTON)) {
        R_Mouse = true;
    }
    else {
        R_Mouse = false;
    }
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

    hMemDC = CreateCompatibleDC(hDC); // Создаем контекст памяти, совместимый с контекстом отображения
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// Выбираем изображение bitmap в контекст памяти

    if (hOldbm) // Если не было ошибок, продолжаем работу
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // Определяем размеры изображения

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//все пиксели черного цвета будут интепретированы как прозрачные
        }
        else
        {
            StretchBlt(hDC, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // Рисуем изображение bitmap
        }

        SelectObject(hMemDC, hOldbm);// Восстанавливаем контекст памяти
    }

    DeleteDC(hMemDC); // Удаляем контекст памяти
}


void ShowLoot() {
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//задний фон
    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);
}

void ShowTerminal() {
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//задний фон
    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);
}

bool CheckCollisionMouseBattle(button b);

bool ShowButton(button &bt)
{
    bool pw_collision = CheckCollisionMouseBattle(bt);
    float offset = (float)pw_collision * sinf(timeGetTime() * .01);
    ShowBitmap(window.context, bt.x, bt.y + offset, bt.width, bt.height, pw_collision ? bt.hBitmapGlow : bt.hBitmap);
    return pw_collision;
}

void ShowBattle() {
  
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBattleBack);//задний фон

    bool pw = ShowButton(PW_butt);
    bool sw = ShowButton(SW_butt);
    bool dw = ShowButton(DW_butt);
    bool exit = ShowButton(Exit_butt);

    if (L_Mouse)
    {
        if (pw)
        {

        }
    }

 }

void ShowRacketAndBall()
{
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//задний фон

    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);// ракетка игрока
    for (int i = 0; i < enemycout; i++) {
        ShowBitmap(window.context, enemy1[i].x - enemy1[i].width / 2., enemy1[i].y, enemy1[i].width, enemy1[i].height, enemy1[i].hBitmap);
    }
    if (ball.dy < 0 && (enemy.x - racket.width / 4 > ball.x || ball.x > enemy.x + racket.width / 4))
    {
        //имитируем разумность оппонента. на самом деле, компьютер никогда не проигрывает, и мы не считаем попадает ли его ракетка по шарику
        //вместо этого, мы всегда делаем отскок от потолка, а раектку противника двигаем - подставляем под шарик
        //движение будет только если шарик летит вверх, и только если шарик по оси X выходит за пределы половины длины ракетки
        //в этом случае, мы смешиваем координаты ракетки и шарика в пропорции 9 к 1
        enemy.x = ball.x * .1 + enemy.x * .9;
    }
    ShowBitmap(window.context, p.x, p.y, ball.rad, ball.rad, ball.hBitmap);
    //  ShowBitmap(window.context, enemy.x - racket.width / 2, 0, racket.width, racket.height, enemy.hBitmap);//ракетка оппонента
     // ShowBitmap(window.context, ball.x - ball.rad, ball.y - ball.rad, 2 * ball.rad, 2 * ball.rad, ball.hBitmap, true);// шарик
}

void LimitRacket()
{
    racket.x = max(racket.x, racket.width / 2.);//если коодината левого угла ракетки меньше нуля, присвоим ей ноль
    racket.x = min(racket.x, window.width - racket.width / 2.);//аналогично для правого угла
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

bool CheckCollisionMouseExit(button Exit_butt) {
    if (p.x < Exit_butt.x + Exit_butt.width && p.x > Exit_butt.x && p.y < Exit_butt.y + Exit_butt.height && p.y > Exit_butt.y)
    {
        return true;
    }
    return false;
}

bool CheckCollisionMouseBattle(button b)
{
    if (p.x < b.x + b.width && p.x > b.x && p.y < b.y + b.height && p.y > b.y)
    {
        return true;
    }
    return false;
}

void CheckFloor()
{
    if (ball.y > window.height - ball.rad - racket.height)//шарик пересек линию отскока - горизонталь ракетки
    {
        if (!tail && ball.x >= racket.x - racket.width / 2. - ball.rad && ball.x <= racket.x + racket.width / 2. + ball.rad)//шарик отбит, и мы не в режиме обработки хвоста
        {
            game.score++;//за каждое отбитие даем одно очко
            ball.speed += 5. / game.score;//но увеличиваем сложность - прибавляем скорости шарику
            ball.dy *= -1;//отскок
            racket.width -= 10. / game.score;//дополнительно уменьшаем ширину ракетки - для сложности
            ProcessSound("knopka-voda-vyisokii-rezkii.wav");//играем звук отскока
        }
        else
        {//шарик не отбит

            tail = true;//дадим шарику упасть ниже ракетки

            if (ball.y - ball.rad > window.height)//если шарик ушел за пределы окна
            {
                game.balls--;//уменьшаем количество "жизней"

                ProcessSound("fail.wav");//играем звук

                if (game.balls < 0) { //проверка условия окончания "жизней"

                    MessageBoxA(window.hWnd, "game over", "", MB_OK);//выводим сообщение о проигрыше
                    InitGame();//переинициализируем игру
                }

                ball.dy = (rand() % 65 + 35) / 100.;//задаем новый случайный вектор для шарика
                ball.dx = -(1 - ball.dy);
                ball.x = racket.x;//инициализируем координаты шарика - ставим его на ракетку
                ball.y = racket.y - ball.rad;
                game.action = false;//приостанавливаем игру, пока игрок не нажмет пробел
                tail = false;
            }
        }
    }
}





void ProcessRoom()
{
    //обрабатываем стены, потолок и пол. принцип - угол падения равен углу отражения, а значит, для отскока мы можем просто инвертировать часть вектора движения шарика
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
        //если игра в активном режиме - перемещаем шарик
        ball.x += ball.dx * ball.speed;
        ball.y += ball.dy * ball.speed;
    }
    else
    {
        //иначе - шарик "приклеен" к ракетке
        ball.x = racket.x;
    }
}

void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//из хэндла окна достаем хэндл контекста устройства для рисования
    window.width = r.right - r.left;//определяем размеры и сохраняем
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//второй буфер
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//привязываем окно к контексту
    GetClientRect(window.hWnd, &r);

}
void BattleGame() {

    ShowBattle();//рисуем фон 

    BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
    Sleep(20);//ждем 16 милисекунд (1/количество кадров в секунду)
    if (L_Mouse)
    {
        if (CheckCollisionMouseBattle(PW_butt)) {

            game_mode = GameMode::map;

        }
        if (CheckCollisionMouseBattle(SW_butt)) {
            game_mode = GameMode::map;
        }

        if (CheckCollisionMouseBattle(DW_butt)) {
            game_mode = GameMode::map;
        }
        if (CheckCollisionMouseBattle(Exit_butt)) {
            game_mode = GameMode::map;
        }

    }
    ProcessInput();//опрос клавиатуры
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
    BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
    Sleep(20);//ждем 16 милисекунд (1/количество кадров в секунду)
    ProcessInput();//опрос клавиатуры

}


void MapGame() {
    ShowRacketAndBall();//рисуем фон, ракетку и шарик
    ShowScore();
    BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
    Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)
    for (int i = 0; i < enemycout; i++) {
        if (L_Mouse == true) {
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
    }
    ProcessInput();//опрос клавиатуры
    LimitRacket();//проверяем, чтобы ракетка не убежала за экран
    // ProcessBall();//перемещаем шарик
    ProcessRoom();//обрабатываем отскоки от стен и каретки, попадание шарика в картетку
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    InitWindow();//здесь инициализируем все что нужно для рисования в окне
    InitGame();//здесь инициализируем переменные игры

    while (!GetAsyncKeyState(VK_ESCAPE))
    {

        GetCursorPos(&p);

        ScreenToClient(window.hWnd, &p);
        switch (game_mode) {
        case GameMode::map: MapGame(); break;
        case GameMode::battle: BattleGame(); break;
        case GameMode::loot: LootGame(); break;
        case GameMode::terminal: TerminalGame(); break;
        }


    }

}
