#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <math.h>

#define MAX_X 30
#define MAX_Y 30
#define scale 20

RECT r,border,level_r,ii_level_r,name_r,pause_r;
HDC hdc;
PAINTSTRUCT ps;
HBRUSH background,border_b,font_brush;
int delay=500,my_color=0,pause=1,pointer=0,flag=1,game_over=0;

char name[80]="player";
char level_str[20]="Your Level: ";
char ii_level_str[20]="II Level: ";
char pause_str[10]="PAUSE";
char game_str[10]="GAME OVER";


typedef struct tail_t{
	int x;
	int y;
	HBRUSH color;
} tail_t;

typedef struct snake_t{
	int x;
	int y;
	HBRUSH h_color;
	HBRUSH color;
	int level;
	int dir;
	struct tail_t * tail;
	size_t tsize;
}  snake_t;

snake_t*snake;
snake_t*snake2;
tail_t*snakeIITrajectory;
tail_t*food;

tail_t* getTrajectory(tail_t*arr,snake_t* snake,tail_t*food){
    double ax=food->x-snake->x;
    double ay=food->y-snake->y;
    double k=ay/ax;
    double b=snake->y-k*snake->x;

    int i=0,j=0,tempx=0,tempy=0;

    for(j=0;j<(MAX_X+MAX_Y);j++){
        arr[j].x=0;
        arr[j].y=0;
    }

    if(abs(ax)>abs(ay)){
        if(ax>0){
            for(j=0,i=snake->x+1;i<=food->x;i++,j++){
                tempx=i;
                tempy=round(k*i+b);
                if(j>0){
                    if((tempx!=arr[j-1].x)&&(tempy!=arr[j-1].y)){
                        arr[j].x=tempx;
                        arr[j].y=arr[j-1].y;
                        j++;
                    }
                }
                arr[j].x=tempx;
                arr[j].y=tempy;
            }
            //size=j;
        }else{
            for(j=0,i=snake->x-1;i>=food->x;i--,j++){
                tempx=i;
                tempy=round(k*i+b);
                if(j>0){
                    if((tempx!=arr[j-1].x)&&(tempy!=arr[j-1].y)){
                        arr[j].x=tempx;
                        arr[j].y=arr[j-1].y;
                        j++;
                    }
                }
                arr[j].x=tempx;
                arr[j].y=tempy;
            }
            //size=j;
        }
    }else{
        if(ay>0){
            for(j=0,i=snake->y+1;i<=food->y;i++,j++){
                tempy=i;
                tempx=round((i-b)/k);
                if(j>0){
                    if((tempx!=arr[j-1].x)&&(tempy!=arr[j-1].y)){
                        arr[j].x=tempx;
                        arr[j].y=arr[j-1].y;
                        j++;
                    }
                }
                arr[j].x=tempx;
                arr[j].y=tempy;
            }
            //size=j;
        }else{
            for(j=0,i=snake->y-1;i>=food->y;i--,j++){
                tempy=i;
                tempx=round((i-b)/k);
                if(j>0){
                    if((tempx!=arr[j-1].x)&&(tempy!=arr[j-1].y)){
                        arr[j].x=tempx;
                        arr[j].y=arr[j-1].y;
                        j++;
                    }
                }
                arr[j].x=tempx;
                arr[j].y=tempy;
            }
            //size=j;
        }
    }

    arr[j].x=food->x;
    arr[j].y=food->y;

    for(i=0;i<j;i++){
        if((arr[i].x!=arr[i+1].x)&&(arr[i].y!=arr[i+1].y)){
            arr[i].x=0;
            arr[i].y=0;
        }
        printf("X=%d,Y=%d\n",arr[i].x,arr[i].y);
    }
    flag=1;
    return arr;
}

tail_t*initFood(){
    tail_t*food=(tail_t*)malloc(sizeof(tail_t));
    food->x=rand()%MAX_X;
    food->y=rand()%MAX_Y;
    food->color=CreateSolidBrush(RGB(255,0,255));
    snakeIITrajectory=getTrajectory(snakeIITrajectory,snake2,food);
    pointer=0;
    return food;
}

snake_t*init(int x,int y,HBRUSH color,HBRUSH h_color,int size){
    snake_t*snake=(snake_t*)malloc(sizeof(snake_t));
    snake->color=color;
    snake->h_color=h_color;
    snake->x=x;
    snake->y=y;
    snake->tsize=size;
    snake->tail=(tail_t*)malloc(sizeof(tail_t)*100);
    snake->dir=2;
    for (int i =0; i < snake->tsize; ++i){
		snake->tail[i].x = x + i +1;
		snake->tail[i].y = y;
	};
	snake->level=1;
    return snake;
}

int checkCollision(snake_t* snake){//Моя функция проверки на контакт головы с элементом хвоста - более универсальная чем checkDirection.Чтобы попробовать: в main **закоментировать, ***раскоментировать
    for (int i = 0;i<snake->tsize;i++){
		if((snake->x==snake->tail[i].x)&&(snake->y==snake->tail[i].y)){
            return 0;
		}
    }
    return 1;
}

void moveLeft(snake_t* snake){//Влево
    //printf("Left");
	for (int i = snake->tsize - 1; i > 0; i--){
		snake->tail[i] = snake->tail[i-1];
    }
	snake->tail[0].x = snake->x;
	snake->tail[0].y = snake->y;

	snake->x = snake->x - 1;
	if (snake->x < 0){
		snake->x = MAX_X - 1;
    }
}

void moveRight(snake_t* snake){//Вправо
	for (int i = snake->tsize - 1; i > 0; i--){
		snake->tail[i] = snake->tail[i-1];
    }
	snake->tail[0].x = snake->x;
	snake->tail[0].y = snake->y;

	snake->x = snake->x + 1;
	if (snake->x > (MAX_X-1)){
		snake->x = 0;
    }
}

void moveUp(snake_t* snake){//Вверх
	for (int i = snake->tsize - 1; i > 0; i--){
		snake->tail[i] = snake->tail[i-1];
    }
	snake->tail[0].x = snake->x;
	snake->tail[0].y = snake->y;

	snake->y = snake->y - 1;
	if (snake->y < 0){
		snake->y = MAX_Y-1;
    }
}

void moveDown(snake_t* snake){//Вниз
	for (int i = snake->tsize - 1; i > 0; i--){
		snake->tail[i] = snake->tail[i-1];
    }
	snake->tail[0].x = snake->x;
	snake->tail[0].y = snake->y;

	snake->y = snake->y + 1;
	if (snake->y > (MAX_Y-1)){
		snake->y = 0;
    }
}

int ifInSnake(snake_t*snake,tail_t*kub){
    if((snake->x==kub->x)&&(snake->y==kub->y)){
        return 1;
    }else{
        for (int i = 0;i<snake->tsize;i++){
            if((kub->x==snake->tail[i].x)&&(kub->y==snake->tail[i].y)){
                return 1;
            }
        }
    }
    return 0;
}

void getNewFood(){
    flag=0;
    food->x=rand()%(MAX_X-1);
    food->y=rand()%(MAX_Y-1);
    while(ifInSnake(snake,food)||ifInSnake(snake2,food)){
        food->x=rand()%(MAX_X-1);
        food->y=rand()%(MAX_Y-1);
    }

    snakeIITrajectory=getTrajectory(snakeIITrajectory,snake2,food);
    pointer=0;
    //printf("New trajectory %d\n",pointer);
}

int ifEat(snake_t*snake,tail_t*food){ //проверим пересеклись лы мы с едой
    if((snake->x==food->x)&&(snake->y==food->y)){
        snake->tsize++;
        getNewFood();
        return 1;
    }
    return 0;
}

typedef struct line{
    double k;
    double b;
} line;

//y1=kx1+b
//y2=kx2+b
//b=y1-kx1
//y2=kx2+y1-kx1
//y2-y1=k(x2-x1)
//k=(y2-y1)/(x2-x1)
//x=(y-b)/k
void snakeIIMove(snake_t* snake){
    if(flag){
        if(snake->x<snakeIITrajectory[pointer].x){
            snake->dir=1;
        }else{
            if(snake->x>snakeIITrajectory[pointer].x){
                snake->dir=2;
            }else{
                if(snake->y<snakeIITrajectory[pointer].y){
                    snake->dir=3;
                }
                else{
                    snake->dir=4;
                }
            }
        }
        pointer++;
    }


    switch(snake->dir){
        case 1:moveRight(snake);break;
        case 2:moveLeft(snake);break;
        case 3:moveDown(snake);break;
        case 4:moveUp(snake);break;
    }
}


LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wp,LPARAM lp)
{
    char str[10]={0};
    HBRUSH my_snake_brush;
    switch(msg)
    {
        case WM_CREATE:

            snakeIITrajectory=(tail_t*)malloc(sizeof(tail_t)*(MAX_X+MAX_Y));
            //printf("COLOR %d\n",my_color);
            switch(my_color){
                case 1:my_snake_brush=CreateSolidBrush(RGB(0,0,0));break;
                case 2:my_snake_brush=CreateSolidBrush(RGB(255,0,0));break;
                case 3:my_snake_brush=CreateSolidBrush(RGB(0,0,255));break;
                case 4:my_snake_brush=CreateSolidBrush(RGB(0,255,0));break;
                case 5:my_snake_brush=CreateSolidBrush(RGB(255,255,0));break;
                case 6:my_snake_brush=CreateSolidBrush(RGB(100,100,100));break;
                case 7:my_snake_brush=CreateSolidBrush(RGB(0,255,255));break;
                case 8:my_snake_brush=CreateSolidBrush(RGB(255,255,255));break;
            }
            snake=init(10,10,my_snake_brush,my_snake_brush,3);
            snake2=init(5,5,CreateSolidBrush(RGB(165,42,42)),CreateSolidBrush(RGB(255,167,0)),3);
            food=initFood();

            r.bottom=(MAX_Y)*scale+3;
            r.left=3;
            r.right=(MAX_X)*scale+3;
            r.top=3;

            level_r.bottom=(MAX_Y-1)*scale;
            level_r.top=(MAX_Y-3)*scale;
            level_r.left=1*scale;
            level_r.right=8*scale;

            ii_level_r.bottom=(MAX_Y-1)*scale;
            ii_level_r.top=(MAX_Y-3)*scale;
            ii_level_r.left=9*scale;
            ii_level_r.right=14*scale;

            pause_r.bottom=(MAX_Y/2)*scale;
            pause_r.top=(MAX_Y/2-2)*scale;
            pause_r.left=(MAX_X/2-2)*scale;
            pause_r.right=(MAX_X/2+2)*scale;

            name_r.bottom=(MAX_Y/2)*scale;
            name_r.top=(MAX_Y/2-2)*scale;
            name_r.left=(MAX_X+1)*scale;
            name_r.right=(MAX_X+6)*scale;

            itoa(snake->level,str,10);
            strcat(level_str,str);
            str[0]='\0';
            itoa(snake2->level,str,10);
            strcat(ii_level_str,str);

            background=CreateSolidBrush(RGB(0,255,0));
            border_b=CreateSolidBrush(RGB(0,0,0));
            font_brush=CreateSolidBrush(RGB(0,255,255));
            SetTimer(wnd,0,delay,NULL);
            return TRUE;
        case WM_TIMER:
            //cout<<"Timer"<<endl;
            if((pause==1)&&(game_over!=1)){
                if(ifEat(snake,food)){

                    if(delay>0){
                        delay-=50;
                    }

                    KillTimer(wnd,0);
                    SetTimer(wnd,0,delay,NULL);
                    if(delay<200){
                        game_over=1;
                    }

                    snake->level++;
                    itoa(snake->level,str,10);
                    level_str[12]='\0';
                    strcat(level_str,str);
                    str[0]='\0';
                    //printf("%d %s\n",snake->level,level_str);
                }

                switch(snake->dir){
                    case 1:moveRight(snake);break;
                    case 2:moveLeft(snake);break;
                    case 3:moveDown(snake);break;
                    case 4:moveUp(snake);break;
                }

                if(checkCollision(snake)==0){
                    printf("CRASH");
                }

                snakeIIMove(snake2);
                //printf("Food:X=%d,Y=%d\n",food->x,food->y);
                //printf("Snake:X=%d,Y=%d\n",snake2->x,snake2->y);
                if(ifEat(snake2,food)){
                    snake2->level++;
                    itoa(snake2->level,str,10);
                    ii_level_str[10]='\0';
                    strcat(ii_level_str,str);
                    str[0]='\0';
                    //printf("%d %s\n",snake2->level,ii_level_str);
                }

            }
            InvalidateRect(wnd,&r,FALSE);

            break;
        case WM_PAINT:

            hdc=BeginPaint(wnd,&ps);

            SelectObject(hdc,background);
            Rectangle(hdc,0,0,MAX_X*scale,MAX_Y*scale);


            SelectObject(hdc,food->color);
            Rectangle(hdc,food->x*scale,food->y*scale,(food->x+1)*scale,(food->y+1)*scale);


            SelectObject(hdc,snake->h_color);
            Rectangle(hdc,snake->x*scale,snake->y*scale,(snake->x+1)*scale,(snake->y+1)*scale);
            SelectObject(hdc,snake->color);
            for(int i=0;i<snake->tsize;i++)
            {
                Rectangle(hdc,snake->tail[i].x*scale,snake->tail[i].y*scale,(snake->tail[i].x+1)*scale,(snake->tail[i].y+1)*scale);
            }


//            SelectObject(hdc,snake2->color);
//            Ellipse(hdc,snake2->x*scale,snake2->y*scale,(snake2->x+1)*scale,(snake2->y+1)*scale);
//            for(int i=0;i<snake2->tsize;i++)
//            {
//                Ellipse(hdc,snake2->tail[i].x*scale,snake2->tail[i].y*scale,(snake2->tail[i].x+1)*scale,(snake2->tail[i].y+1)*scale);
//            }

            SelectObject(hdc,snake2->h_color);
            RoundRect(hdc,snake2->x*scale,snake2->y*scale,(snake2->x+1)*scale,(snake2->y+1)*scale,8,15);
            SelectObject(hdc,snake2->color);
            for(int i=0;i<snake2->tsize;i++)
            {
                if(i%2==0){
                    SelectObject(hdc,snake2->color);
                    RoundRect(hdc,snake2->tail[i].x*scale,snake2->tail[i].y*scale,(snake2->tail[i].x+1)*scale,(snake2->tail[i].y+1)*scale,8,15);
                }else{
                    SelectObject(hdc,snake2->h_color);
                    RoundRect(hdc,snake2->tail[i].x*scale,snake2->tail[i].y*scale,(snake2->tail[i].x+1)*scale,(snake2->tail[i].y+1)*scale,8,15);
                }

            }

            SetBkMode(hdc, TRANSPARENT);
            DrawText(hdc,name,strlen(name),&name_r,DT_CENTER);
            DrawText(hdc,level_str,strlen(level_str),&level_r,DT_CENTER);
            DrawText(hdc,ii_level_str,strlen(ii_level_str),&ii_level_r,DT_CENTER);

            if(pause==0){
                SetBkMode(hdc, TRANSPARENT);
                DrawText(hdc,pause_str,strlen(pause_str),&pause_r,DT_CENTER);
            }
            if(game_over==1){
                SetBkMode(hdc, TRANSPARENT);
                DrawText(hdc,game_str,strlen(game_str),&pause_r,DT_CENTER);
            }

            EndPaint(wnd,&ps);
            break;
        case WM_CHAR:
            switch((char)wp)
            {
                case 'D':
                case 'd':
                    snake->dir=1;
                    break;
                case 'A':
                case 'a':
                    snake->dir=2;
                    break;
                case 'S':
                case 's':
                    snake->dir=3;
                    break;
                case 'W':
                case 'w':
                    snake->dir=4;
                    break;
                case 'P':
                case 'p':
                    pause^=1;
                    break;
                default:;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default: DefWindowProc(wnd,msg,wp,lp);
    }
}


int main()
{
    char c=0;
    int num=0;
    FILE*f=NULL;
    while(c!='e'){
        system("cls");
        printf("       Menu        \nCreate new player->n\nChoose snake color->c\nLoad saved games->l\nShow prev records->s\nStart the game->e\n");
        c=getch();
        switch(c){
            case 'n':
                system("cls");
                printf("Enter player name\n");
                name[0]='\0';
                scanf("%s",name);
                break;
            case 'c':
                system("cls");
                printf("Choose snake color\nBLACK ->1\nRED   ->2\nBLUE  ->3\nGREEN ->4\nYELLOW->5\nGREY  ->6\nPINK  ->7\nWHITE ->8\n");
                scanf("%d",&my_color);

                break;
            case 'l':
                system("cls");
                printf("Load saved games\nEnter number of the game you want to load\n");
                printf("For future use only");
//                f=fopen("games.txt","r");
//                if(f!=NULL){
//                    printf("Open");
//                }
//                num=0;
//                fscanf("%d",&num);
                break;
            case 'r':
                system("cls");
                printf("prev records\n");
                printf("For future use only");
//                f=fopen("records.txt","r");
//                if(f!=NULL){
//                    printf("Open");
//                }
                break;
            case 'e':
                system("cls");
                break;
            default:;
        }
    }


    srand(time(NULL));
    WNDCLASS wnd;
    char*ClassName="WndClass";
    HINSTANCE hinst=GetModuleHandle(NULL);
    HBRUSH hb=CreateSolidBrush(RGB(0,255,0));
    HCURSOR hc=LoadCursor(NULL,IDC_CROSS);
    ZeroMemory(&wnd,sizeof(wnd));
    wnd.hInstance=hinst;
    wnd.lpszClassName=ClassName;
    wnd.lpfnWndProc=WndProc;
    wnd.hbrBackground=hb;
    wnd.hCursor=hc;
    RegisterClass(&wnd);
    HWND hwnd=CreateWindow(ClassName,"MyWindow",WS_OVERLAPPEDWINDOW,200,50,(MAX_X+8)*scale,(MAX_Y+8)*scale,NULL,NULL,hinst,NULL);


    ShowWindow(hwnd,SW_SHOW);
    MSG msg;
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }





    return 0;
}
