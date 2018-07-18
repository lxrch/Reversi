#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#define size 8    ///  size为棋盘规格  格子数：size*size
struct chess{
    int f;        ///  f  = 1 : 存在黑棋     f = 0 : 存在白旗        f = -1:  这里没有棋子
    int w;        /// w = 1: 可以放置白旗   w = 0 ： 不可放白旗
    int b;
    char s[6];
}chesses[size*2 + 2][size*2 + 2];
struct xulie{          /// 储存电脑的合法落子点
    int x;             /// x,y 为 位置坐标
    int y;
    int kill_num;      /// 电脑落在此处可翻转对方棋子的数目
}p[60];
time_t time1;
time_t time2;
int hard_config = 1;   /// 1为困难模式   0为简单模式
int cursor_x = 0;      /// 黑棋的合法落脚点数目
int cursor = 0;        /// 白棋的合法落脚点数目
int ex_rebot = 1;      /// 人机模式难度: 1为困难  0为简单
int exit_status = 0;   /// 退出游戏标志
int x=1,y=1 ;          /// 光标所在位置
int count_x = 0;       /// 黑棋数目
int count_y = 0;       /// 白棋数目
int turn = 1;          /// turn为1是黑棋下子   0时白棋下子
int fan = -1;          /// 用来控制交替下棋
void init();           /// 初始化函数  初始化各项参数
void display();        /// 棋盘刷新
void move();           /// 白棋的移动
int make_d();          /// 落子
void kill();           /// 寻找能消灭对方棋子的方向
void real_kill();      /// 消灭对方棋子
void start();          /// 游戏入口
void game_over();      /// 游戏结束(判断胜负)
int judge_all();       /// 判断哪些位置可以放白棋或黑棋
void left_move(char key);    /// 黑棋的移动
int rebot();                 /// 机器人模块
void p_c();      /// 人机对战
void p_p();      /// 人人对战
struct chess * get_xy();   /// 获取棋子地址
void change_player();      /// 交替
int get_kill(int x,int y); /// 获取某个位置机器人能消灭的棋子数
int main() {
    SetConsoleTitle("黑白棋游戏");
    while(exit_status == 0){
        start();
    }
    return 0;
}
int get_kill(int x,int y){
    int a,b,i,j;
    int step = 0;
    int result = 0;
    for(a = -1; a <= 1; a++){
        for(b = -1; b <= 1; b++){
            if(a==0 && b==0) continue;
            step = 0;
            for(i=x,j=y; i>=1 && j>=1 && i<=size && j<=size; i+=a,j+=b){
                if(i==x && j==y) continue;
                if(get_xy(i,j)->f == 1){
                    step++;           /// 累加某个方向吃子数
                }
                else if(get_xy(i,j)->f == 0){
                    break;
                }
                else if(get_xy(i,j)->f == -1){
                    step = 0;          /// 此方向不能吃子
                    break;
                }

            }
            result += step;       /// 累积每个方向的吃子数
        }
    }
    return result;
}
void help(){
    system("cls");
    //system("color B5");
    printf("\n                        游戏玩法\n");
    printf("\n\n\
    棋盘为8×8的方格布局，开局时在棋盘正中有摆好的四枚棋子，\n\
    黑白各2枚，交叉放置，由执黑棋的一方先落子, 双方交替下子\n\
    棋子落在方格内。\n\
    下子方式：把自己颜色的棋子放在棋盘的空格上，而当自己放\n\
    下的棋子在横、竖、斜八个方向内有一个自己的棋子，则被夹\n\
    在中间的对方棋子全部翻转会成为自己的棋子。夹住的位置上\n\
    必须全部是对手的棋子，不能有空格。并且，只有在可以翻转\n\
    棋子的地方才可以下子。\n\
    翻转棋子时,有一个棋子的翻转动画大概显示1秒左右每次下子\n\
    最少必须翻转对方一个棋子，若棋局中下子都不能翻转对方棋\n\
    子，则自动pass轮空，您无子可下由对方继续下子，若两个玩\n\
    家都不能下子翻转对方棋子，游戏结束。\n\n\
    具体玩法：玩家一 使用 W A S D 控制方向，空格放棋子。\n\
              玩家二 使用 ↑ ← ↓ → 控制方向，enter键放置棋子\n\
              注：单机模式只有玩家一\n\
    \n\n                     按任意键返回");
    getch();

}
void change_player(){
    turn = turn + fan;          /// 使turn在0和1中循环
    fan = -fan;
}
void p_p(){
    char ch;
    init();
    while(ch =  getch()) {
        if(cursor == 0 && turn == 0 && cursor_x != 0){
            change_player();
            continue;
        }
        else if(cursor_x == 0 && turn == 1 && cursor != 0){
            change_player();
            continue;
        }
        else if(cursor == 0 && cursor_x == 0){
            game_over();
            return;
        }
        if(ch == -32) {
            move();
        }
        else if(ch == 'a' || ch == 's' || ch == 'd' || ch == 'w') {
            left_move(ch);
        }
        else if((ch == ' ' && turn == 1 )||( ch == 13 && turn == 0)) {
            int ff = make_d(x,y,turn);
            if(ff){
                turn = turn + fan;
                fan = -fan;
                judge_all();
            }
        }
        else if(ch == 'q'){
            break;
        }
        display();
        if(count_x == 0 || count_y == 0 || count_x+count_y == size*size){
            game_over();
        }

    }
}
void p_c(){
    char ch;
    printf("    请选择先手或后手(输入后回车)\n    1.先手    2.后手\n    ");
    int order = 1;                       /// order 为出手顺序
    scanf("%d",&order);
    init();
    if(order == 2) rebot();        /// 机器人先手
    judge_all();
    while(ch =  getch()) {
        if(cursor_x == 0 && turn == 1 && cursor != 0) {
                rebot();
                judge_all();
        }
        else if(cursor == 0 && cursor_x == 0){
            game_over();
            return;
        }
        if(ch == 'a' || ch == 's' || ch == 'd' || ch == 'w') {
            left_move(ch);
        }
        else if((ch == ' ' && turn == 1 )||( ch == 13 && turn == 0)) {
            display();
            int ff = make_d(x,y,turn);
            if(ff){
                rebot();
                judge_all();
            }
        }
        else if(ch == 'q'){
            break;
        }
        if(count_x == 0 || count_y == 0 || count_x+count_y == size*size){
            game_over();
        }
    }
}
void start(){
    system("cls");
    system("color f0");
    printf("\n\
    @@@@@@                                             @#\n\
    @    @@\n\
    @     @\n\
    @     @     @@@   @@     @    @@@    @@ @@  ~@@@   @\n\
    @     @    @   @   @     @   @   @   @@@   $@   ~  @\n\
    @~~~@@    @     @  @-   @=  @     @  @@    @       @\n\
    @@@@$     @     @  !@   @   @     @  @@    @@      @\n\
    @   @@    @@@@@@@   @  -@   @@@@@@@  @@     @@@    @\n\
    @    @    @         @  @   ~@        @@       @@~  @\n\
    @    @@   @          @ @    @        @@        $@  @\n\
    @     @!  @@    -    @ =    @@       @@        $@  @\n\
    @     -@   @@@@@@    @@      @@@@@*  @@    @@@@@   @      \n\n\n\n\
    a.单人游戏\t b.双人游戏\th.查看帮助\tq.退出游戏\n");
    char st = getch();
    int dii;
    switch(st){
        case 'a':
            turn = 1;
            printf("    请选择难度：1.简单  2.困难 (输入后回车)\n    ");
            scanf("%d",&dii);
            if(dii == 1){
                hard_config = 0;
            }
            else{
                hard_config = 1;
            }
            time1 = time(NULL);
            p_c();
            break;
        case 'b': time1 = time(NULL); turn = 1;p_p();break;    /// 记录开始时间
        case 'q': exit_status = 1;return;
        case 'h': help();break;
        default:return;
    }
    return ;
}
void real_kill(int x,int y,int a,int b){
    int i,j;
    for(i=x,j=y; i>=1 && j>=1 && i<=size && j<=size; i+=a,j+=b){
        if(i==x && j==y) continue;
        if(turn){
            if(get_xy(i,j)->f == 0){           /// 消灭白棋
                strcpy(get_xy(i,j)->s,"●");
                count_x++;
                count_y--;
                get_xy(i,j)->f = 1;
            }
            else break;
        }
        else {                                 /// 消灭黑棋
            if(get_xy(i,j)->f == 1){
                count_y++;
                count_x--;
                strcpy(get_xy(i,j)->s,"○");
                get_xy(i,j)->f = 0;
            }
            else break;
        }
    }
}
int rebot(){
    if(ex_rebot == 0) return 0;
    cursor = 0;
    judge_all();
    change_player();
    if(cursor == 0){
        change_player();
        return 0;
    }
    srand(time(NULL));
    int max_kill,i;
    int rand_p = rand()%(cursor);
    if(hard_config == 1){           /// 困难模式下,寻找能吃子最多的位置
        max_kill = 0;
        for(i = 1; i <= cursor-1; i++){
            if(max_kill < p[i].kill_num){
                max_kill = p[i].kill_num;
                rand_p = i;
            }
        }

    }
    x = p[rand_p].x;
    y = p[rand_p].y;
    Sleep(1000);                /// 机器人等待时间
    make_d(p[rand_p].x,p[rand_p].y,0);
    change_player();
    display();
    return 1;
}
void game_over(){
    system("cls");
    time_t time_over;
    time_over = time(NULL);      /// 记录结束时间
    int ov_t = time_over - time1;
    int h = ov_t/3600;
    int m = ov_t%3600/60;
    int s = ov_t%60;
    int i,j;
    for(i = 1; i <= size; i++){
        for(j = 1; j <= size; j++){
            printf("%s",get_xy(i,j)->s);
        }
        if(i == size/2 -1){
            printf("          黑棋:%d  白旗：%d",count_x,count_y);
        }

        if(i == size/2){
            if(count_x < count_y) {
                printf("          白棋胜利");
            }
            else if(count_x > count_y) {
                printf("          黑棋胜利");
            }
            else printf("          平局");
        }
        if(i == size/2+1){
                printf("          所用时间： %02d:%02d:%02d",h,m,s);
        }
        printf("\n");
    }
    printf("\n输入任意键返回主菜单");
    //getch();

}
void kill(int x,int y){
    int i,j,a,b;
    for(a = -1; a <= 1; a++){
        for(b = -1; b <= 1; b++){
            if(a==0 && b==0) continue;
            for(i=x,j=y; i>=1 && j>=1 && i<=size && j<=size; i+=a,j+=b){
                if(i==x && j==y) continue;
                ///printf("\nget->f:%d\nturn:%d",get_xy(i,j)->f,turn);
                if(get_xy(i,j)->f == turn) {
                    real_kill(x,y,a,b);            /// 消灭某个方向的棋子
                    ///printf("real_kill(%d,%d)\n",a,b);
                    break;
                }
                else if(get_xy(i,j)->f == -1){
                    break;
                }
            }
        }
    }
}
void left_move(char key) {
    if(turn == 0) return ;
    ///recover_high(x,y);
    ///high_light(x,y);
    char di = key;
    switch(di) {
        case 'd':
            y++;    ///right
            break;
        case 'a':
            y--;    ///left
            break;
        case 's':
            x++;     ///down
            break;
        case 'w':
            x--;   ///up
            break;
    }
    if(x > size) x = size;
    else if(x < 1) x = 1;
    if(y > size) y = size;
    else if(y < 1) y = 1;
    display();

}
void move(){
    if(turn == 1) return ;
    ///recover_high(x,y);
    char di;
    ///high_light(x,y);
    switch(di = getch()) {
        case 77:
            y++;    ///right
            break;
        case 75:
            y--;    ///left
            break;
        case 80:
            x++;     ///down
            break;
        case 72:
            x--;    ///up
            break;
    }
    if(x > size) x = size;           /// 防止光标越界
    else if(x < 1) x = 1;
    if(y > size) y = size;
    else if(y < 1) y = 1;
    display();
}
int judge_all(){
    int r,t,a,b,i,j;
    cursor_x = 0;
    cursor = 0;
    for(r = 1; r <= size; r++){                /// 重置能否放置的数据
        for(t = 1; t <= size; t++){
                get_xy(r,t)->b = 0;
                get_xy(r,t)->w = 0;
        }
    }
    for(i = 1; i <= 50; i++){
        p[i].kill_num = 0;
    }
    for(r = 1; r <= size; r++){
        for(t = 1; t <= size; t++){
            if(get_xy(r,t)->f == 1){
                int x = r;
                int y = t;
                for(a = -1; a <= 1; a++){
                    for(b = -1; b <= 1; b++){
                        if(a==0 && b==0) continue;
                        for(i=x,j=y; i>=1 && j>=1 && i<=size && j<=size; i+=a,j+=b){
                            if(i==x && j==y) continue;
                            if(get_xy(i,j)->f == 1) break;
                            else if(get_xy(i,j)->f == -1) {
                                if(i-a!=x || j-b!=y){
                                    get_xy(i,j)->b = 1;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            else if(get_xy(r,t)->f == 0){
                int x = r;
                int y = t;
                for(a = -1; a <= 1; a++){
                    for(b = -1; b <= 1; b++){
                        if(a==0 && b==0) continue;
                        for(i=x,j=y; i>=1 && j>=1 && i<=size && j<=size; i+=a,j+=b){
                            if(i==x && j==y) continue;
                            if(get_xy(i,j)->f == 0) break;
                            else if(get_xy(i,j)->f == -1) {
                                if(i-a!=x || j-b!=y){
                                    get_xy(i,j)->w = 1;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    for(r = 1; r <= size; r++){
        for(t = 1; t <= size; t++){
            if((get_xy(r,t)->w == 1 && get_xy(r,t)->f == -1)){
                p[cursor].x = r;
                p[cursor].y = t;
                p[cursor].kill_num = get_kill(r,t);
                cursor++;
            }
            else if(get_xy(r,t)->b == 1 && get_xy(r,t)->f == -1){
                cursor_x ++;
            }
        }
    }
    return 1;

}
int make_d(int x, int y, int flag){
    if(get_xy(x,y)->f != -1 || (get_xy(x,y)->w ==0 && flag == 0)|| (get_xy(x,y)->b ==0 && flag == 1))
        return 0;
    if(flag){
        count_x++;
        strcpy(get_xy(x,y)->s, "●");
        get_xy(x,y)->f = 1;
    }
    else {
        count_y++;
        strcpy(get_xy(x,y)->s, "○");
        get_xy(x,y)->f = 0;
    }
    kill(x,y);
    display();
    return 1;
}
struct chess * get_xy(int x, int y) {
    if(!(x&&y)) return NULL;
    x *= 2;
    y *= 2;
    if(x < size*2+1 && y < size*2+1) {
        return &chesses[x][y];
    }
    else return NULL;
}
void init() {
    system("color f0");
    int i,j;
    for(j = 1; j <= size*2+1; j += 2){
        strcpy(chesses[j][1].s,"├");
        for(i = 2; i < size*2+1; i += 2) {
            strcpy(chesses[j][i].s,"─");
            strcpy(chesses[j][i+1].s,"┼");

        }
        strcpy(chesses[j][size*2+1].s,"┤");
    }
    for(i = 3; i < size*2+1; i += 2) {
        strcpy(chesses[1][i].s,"┬");
        strcpy(chesses[size*2+1][i].s,"┴");
    }
    for(i = 2; i < size*2+1; i += 2) {
        for(j = 1; j <= size*2+1; j += 2) {
            strcpy(chesses[i][j].s,"│");
            strcpy(chesses[i][j-1].s,"  ");
        }
    }
    strcpy(chesses[1][1].s,"┌");
    strcpy(chesses[size*2+1][1].s,"└");
    strcpy(chesses[1][size*2+1].s,"┐");
    strcpy(chesses[size*2+1][size*2+1].s,"┘");

    for(i = 1; i <= size; i++){
        for(j = 1; j <= size; j++){
            get_xy(i,j)->f = -1;
            get_xy(i,j)->w = 0;
            get_xy(i,j)->b = 0;
        }
    }
    get_xy(size/2, size/2)->w = 1;
    get_xy(size/2, size/2+1)->b = 1;
    get_xy(size/2+1, size/2+1)->w = 1;
    get_xy(size/2+1, size/2)->b = 1;
    make_d(size/2, size/2,0);
    make_d(size/2, size/2+1,1);
    make_d(size/2+1, size/2+1,0);
    make_d(size/2+1, size/2,1);

    count_x = 2;
    count_y = 2;

    x = 0;
    y = 0;

    judge_all();
}
void display() {
    system("cls");
    int i,j;
    for(i = 1; i <= size*2+1; i++){
        for(j = 1; j <= size*2+1; j++){
            if(i == x*2-1 && j == y*2) printf("╦");
            else if(i == x*2+1 && j == y*2) printf("╩");
            else if(i == x*2 && j == y*2-1) printf("╠");
            else if(i == x*2 && j == y*2+1) printf("╣");
            else if(i == x*2 && j == y*2 && ((turn == 1 && get_xy(x,y)->b == 0) || (turn == 0 && get_xy(x,y)->w == 0)) && get_xy(x,y)->f == -1) printf("╬");
            else printf("%s",chesses[i][j].s);
        }
        if(i == size)
            printf("    ●:%d",count_x);
        else if(i == size + 2)
            printf("    ○:%d",count_y);
       printf("\n");
    }
    //printf("x:%d  y:%d\n",x,y);
    if(turn == 1)
        printf("   轮到 ● 棋\n   W A S D 控制方向  space 放置棋子\n   按 q 返回主菜单\n");
    else
        printf("   轮到 ○ 棋\n   ↑ ↓ ← → 控制方向  enter 放置棋子\n   按 q 返回主菜单\n");
    time2 = time(NULL);              /// 记录当前时间
    int e_time = time2 - time1;
    int h = e_time/3600;
    int m = e_time%3600/60;
    int s = e_time%60;
    printf("   时间： %02d:%02d:%02d\n",h,m,s);
}
