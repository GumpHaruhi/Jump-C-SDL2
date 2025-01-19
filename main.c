#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <math.h>

typedef struct{
    int state;                     // 状态，0-静止  1-蓄力  2-跳跃
    SDL_Rect* rect;
    int velocity;                  // 跳跃初速度
    Uint32 preparTimer;
} Player;

typedef struct MyPlate{
    int type;
    int centerX;
    SDL_Rect* rect;
    int lenth;
} Plate;

typedef struct MyMap{
    Plate* pre;
    Plate* mid;
    Plate* lat;
} Map;

// 全局变量
SDL_Window* window;                // 主窗口
SDL_Surface* screenSurface;        // 屏幕
SDL_Renderer* renderer;            // 画笔
TTF_Font* font_0 = NULL;           // 字体
TTF_Font* font_1 = NULL;
TTF_Font* font_2 = NULL;
TTF_Font* font_3 = NULL;
TTF_Font* font_4 = NULL;
const int speed = 12;              // 速度
int score;                         // 得分
const int grity = 4;               // 重力
const int playerHigh = 70;         // 玩家体积数据
const int playerY = 600;
int weight_1, weight_2;            // 难度权重

// 函数声明
void initSDL();
void renderText(const char* text, int x, int y, char flag);
void cleanUp();
Player* initPlayer();
void runGame();
bool updatePlayer(Player* player);
int getPargraph(Uint32 x, Uint32 y);
void drawPlayer(Player* player);
void drawPlate(Plate* plate);
Map* initMap();
void updateMap(Map* map, bool move);
Plate* generatePlate();
bool checkAlive(Player* player, Plate* plate);
void updateWeight();

// 主函数
int main(int argc,char *argv[]) {
    // 程序初始化
    initSDL();
    srand((unsigned int)time(NULL));   // 随机数种子

    bool quit = false;          // 程序退出
    bool gameRuning = false;    // 游戏正在运行
    bool tag = false;           // 标记是否已经进行过游戏
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    if (!gameRuning) {
                        // 空格键按下，开始游戏
                        gameRuning = true;
                    }
                }
            }
        }

        // 清空渲染器
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 设置颜色为白色
        //SDL_RenderClear(renderer);

        if (gameRuning) {
            SDL_RenderClear(renderer);
            runGame();
            gameRuning = false;
            tag = true;
        }

        if(tag){
            // 游戏已输，等待重开
            char numberText[20];  // 足够大以容纳整数的字符串表示
            sprintf(numberText, "LOSE");
            renderText(numberText, 160, 300, 'r');
        }
        else{
            char title0Text[25];
            sprintf(title0Text, "JUMP");
            renderText(title0Text, 12, 100, 'x');
            char title1Text[25];
            sprintf(title1Text, "Jump");
            renderText(title1Text, 180, 180, 'y');
            char numberText[25];  // 足够大以容纳整数的字符串表示
            sprintf(numberText, "TAP SPASE");
            renderText(numberText, 160, 500, 's');
        }

        // 刷新屏幕
        SDL_UpdateWindowSurface(window);
        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    cleanUp();
    return 0;
}


/************        游戏主逻辑         *************/
void runGame(){
    score = 0;
    weight_1 = 70;
    weight_2 = 90;
    Player* player = initPlayer();
    Map* map = initMap();

    bool over = false;
    SDL_Event event;

    while(!over){
        while (SDL_PollEvent(&event) != 0) {
            // 退出游戏
            if (event.type == SDL_QUIT) {
                over = true;
            }
            // 按下蓄力
            else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE){
                if(player->state == 0){
                    player->state = 1;
                    player->preparTimer = SDL_GetTicks();
                }
            }
            // 释放跳跃
            else if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE){
                if(player->state == 1){
                    player->state = 2;
                    player->rect->h = playerHigh;
                    player->rect->y = playerY;
                    player->velocity = -24 - getPargraph(SDL_GetTicks(), player->preparTimer)*6;
                }
            }
        }

        // 更新游戏状态
        if(updatePlayer(player)){
            if(checkAlive(player, map->mid) || checkAlive(player, map->pre) || checkAlive(player, map->lat)){
                score++;
            }
            else{
                over = true;
            }
        }
        updateMap(map, player->state == 2);
        updateWeight();

        // 清空渲染器
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 设置颜色为白色
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // 绘图
        drawPlate(map->pre);
        drawPlate(map->mid);
        drawPlate(map->lat);
        drawPlayer(player);
        char scoreText[100];  // 足够大以容纳整数的字符串表示
        sprintf(scoreText, "Score: %d", score);
        renderText(scoreText, 400, 100, 'b');

        // 更新屏幕
        SDL_UpdateWindowSurface(window);
        SDL_RenderPresent(renderer);
        SDL_Delay(30);
    }
}

/***********         初始化         ************/
// 初始化SDL
void initSDL(){
    SDL_Init(SDL_INIT_VIDEO);

    // 加载字体
    TTF_Init();
    // 获取可执行文件的基本路径
    char basePath[256];
    SDL_strlcpy(basePath, SDL_GetBasePath(), sizeof(basePath));
    // 构建相对路径
    char relativePath0[256];
    SDL_snprintf(relativePath0, sizeof(relativePath0), "%s%s", basePath, "font_0.ttf");
    font_0 = TTF_OpenFont(relativePath0, 28);

    char relativePath1[256];
    SDL_snprintf(relativePath1, sizeof(relativePath1), "%s%s", basePath, "font_1.ttf");
    font_1 = TTF_OpenFont(relativePath1, 100);

    char relativePath2[256];
    SDL_snprintf(relativePath2, sizeof(relativePath2), "%s%s", basePath, "font_0.ttf");
    font_2 = TTF_OpenFont(relativePath2, 50);

    char relativePath3[256];
    SDL_snprintf(relativePath3, sizeof(relativePath3), "%s%s", basePath, "font_1.ttf");
    font_3 = TTF_OpenFont(relativePath3, 180);

    char relativePath4[256];
    SDL_snprintf(relativePath4, sizeof(relativePath4), "%s%s", basePath, "font_0.ttf");
    font_4 = TTF_OpenFont(relativePath4, 100);

    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 800, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    screenSurface = SDL_GetWindowSurface( window );
}

// 释放资源
void cleanUp() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

/**********         渲染         ***********/
// 渲染文本
void renderText(const char* text, int x, int y, char flag) {
    SDL_Color textColor;
    SDL_Surface *textSurface;
    if (flag == 'r') {   // 红色大字
        textColor.r = 255;
        textColor.g = 0;
        textColor.b = 0;
        textColor.a = 255;
        textSurface = TTF_RenderText_Solid(font_1, text, textColor);
    } else if (flag == 's') {       // 开始界面
        textColor.r = 255;
        textColor.g = 255;
        textColor.b = 255;
        textColor.a = 255;
        textSurface = TTF_RenderText_Solid(font_2, text, textColor);
    } else if (flag == 'b') {       // 计分板
        textColor.r = 255;
        textColor.g = 0;
        textColor.b = 0;
        textColor.a = 255;
        textSurface = TTF_RenderText_Solid(font_0, text, textColor);
    } else if (flag == 'x') {
        textColor.r = 255;
        textColor.g = 255;
        textColor.b = 255;
        textColor.a = 255;
        textSurface = TTF_RenderText_Solid(font_3, text, textColor);
    } else if (flag == 'y') {
        textColor.r = 127;
        textColor.g = 127;
        textColor.b = 127;
        textColor.a = 255;
        textSurface = TTF_RenderText_Solid(font_4, text, textColor);
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect destRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &destRect);
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}


// 初始化玩家
Player* initPlayer(){
    Player* hero = (Player*)malloc(sizeof(Player));
    hero->rect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
    hero->rect->w = 40;
    hero->rect->h = playerHigh;
    hero->rect->x = 100;
    hero->rect->y = playerY;
    hero->state = 0;
    hero->velocity = 0;
    return hero;
}

// 迭代玩家状态
bool updatePlayer(Player* player){
    // 蓄力状态
    if(player->state == 1){
        int temp = getPargraph(SDL_GetTicks(), player->preparTimer)*5;
        player->rect->y = playerY + temp;
        player->rect->h = playerHigh - temp;
    }
    // 跳跃状态
    else if(player->state == 2){
        player->rect->y += player->velocity;
        if(player->rect->y >= playerY){
            player->state = 0;
            player->velocity = 0;
            return true;
        }
        else{
            player->velocity += grity;
        }
    }
    return false;
}

// 画出玩家
void drawPlayer(Player* player){
    if(player == NULL) { return; }
    switch (score % 3){
        case 0:
            SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            break;
        case 1:
            SDL_SetRenderDrawColor(renderer, 127, 255, 0, 255);
            break;
        case 2:
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            break;
    }
    SDL_RenderFillRect(renderer, player->rect);
}

// 获得蓄力层级
int getPargraph(Uint32 x, Uint32 y){
    int n = (x - y) / 200 + 1;
    return n > 7 ? 7: n;
}

// 迭代地图
void updateMap(Map* map, bool move){
    // 平台移动
    if(move){
        map->pre->rect->x -= speed;
        map->pre->centerX -= speed;
        map->mid->rect->x -= speed;
        map->mid->centerX -= speed;
        map->lat->rect->x -= speed;
        map->lat->centerX -= speed;
    }

    // 维护地图
    if(map->pre->rect->x + map->pre->rect->w <= 0){
        free(map->pre->rect);
        free(map->pre);
        map->pre = map->mid;
        map->mid = map->lat;
        map->lat = generatePlate();
    }
}

// 产生新平台
Plate* generatePlate(){
    Plate* sp = (Plate*)malloc(sizeof(Plate));
    sp->type = rand() % 3 ;
    sp->rect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
    sp->centerX = 720;

    // 平台的难度随机数(带权重)
    int randomNum = rand() % 100;
    if(randomNum >= 0 && randomNum < weight_1){
        sp->lenth = 60;
    }
    else if(randomNum >= weight_1 && randomNum < weight_2){
        sp->lenth = 50;
    }
    else if(randomNum >= weight_2){
        sp->lenth = 30;
    }

    switch (sp->type) {
        case 0:
            sp->rect->x = sp->centerX - sp->lenth;
            sp->rect->y = 670 - sp->lenth;
            sp->rect->w = 2* sp->lenth;
            sp->rect->h = 2* sp->lenth;
            break;
        case 1:
            sp->rect->x = sp->centerX - sp->lenth;
            sp->rect->y = 685 - sp->lenth;
            sp->rect->w = 2* sp->lenth;
            sp->rect->h = 2* sp->lenth - 30;
            break;
        case 2:
            sp->rect->x = sp->centerX - sp->lenth;
            sp->rect->y = 670 - sp->lenth;
            sp->rect->w = 2* sp->lenth;
            sp->rect->h = 2* sp->lenth;
            break;
    }

    return sp;
}

// 初始化地图
Map* initMap(){
    Map* map = (Map*)malloc(sizeof(Map));
    map->pre = generatePlate();
    map->mid = generatePlate();
    map->lat = generatePlate();
    map->pre->centerX = 120;
    map->pre->rect->x = map->pre->centerX - map->pre->lenth;
    map->mid->centerX = 420;
    map->mid->rect->x = map->mid->centerX - map->mid->lenth;

    return map;
}

// 画出平台
void drawPlate(Plate* plate){
    if(plate == NULL) { return; }
    switch(plate->type){
        case 0:
            SDL_SetRenderDrawColor(renderer, 0, 127, 127, 255);
            SDL_RenderFillRect(renderer, plate->rect);
            SDL_RenderDrawLine(renderer, plate->centerX, 0, plate->centerX, plate->rect->y+2);
            break;
        case 1:
            SDL_SetRenderDrawColor(renderer, 0, 180, 0, 255);
            for (int i = 0; i < 360; ++i) {
                double angle = i * M_PI / 180.0;
                int centerX = plate->centerX, centerY = plate->rect->y + plate->lenth, ll = plate->lenth;
                int circleX = centerX + ll * cos(angle);
                int circleY = centerY + ll * sin(angle);
                SDL_RenderDrawPoint(renderer, circleX, circleY);
            }
            SDL_RenderDrawLine(renderer, plate->centerX, plate->rect->y+2*plate->lenth, plate->centerX, 800);
            break;
        case 2:
            SDL_SetRenderDrawColor(renderer, 127, 64, 0, 255);
            SDL_RenderFillRect(renderer, plate->rect);
            SDL_RenderDrawLine(renderer, plate->centerX, 0, plate->centerX, plate->rect->y+2);
            break;
    }
}

// 检测碰撞
bool checkAlive(Player* player, Plate* plate){
    int distance = plate->centerX - 120;
    distance = distance >= 0 ? distance: -1*distance;
    return distance <= plate->lenth;
}

// 难度升级
void updateWeight(){
    int level = score / 10;
    switch (level) {
        case 0:
            break;
        case 1:
            weight_1 = 60;
            break;
        case 2:
            weight_1 = 50;
            weight_2 = 80;
            break;
        case 3:
            weight_1 = 40;
            weight_2 = 70;
            break;
        case 4:
            weight_1 = 20;
            weight_2 = 50;
            break;
        default:
            break;
    }
}