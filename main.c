// EarthDefender version: 2
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<termios.h>
#include<unistd.h>

struct termios original_settings;

void enable_raw_mode(){
    struct termios new_settings;
    tcgetattr(STDIN_FILENO,&original_settings);
    new_settings = original_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    new_settings.c_cc[VMIN] = 0;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings);
}

int kbhit() {
    char ch;
    if (read(STDIN_FILENO, &ch, 1) == 1) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

struct player {
    int x,y;
    int life;
};

struct bullet{
    int x,y;
    int active;
};

struct enemy{
    int x;
    float y;
    int active;
};

void field(char grid[21][51]){
    for(int i=0;i<21;i++){
        for(int j=0;j<51;j++){
            if ((i == 0 || i == 20) && (j == 0 || j == 50)) grid[i][j] = '@';
            else if (i == 0 || i == 20) grid[i][j] = '_';
            else if (j == 0 || j == 50) grid[i][j] = '|';
            else grid[i][j] = ' ';
        }
    }
}

void printer(char grid[21][51]){
    for(int i=0;i<21;i++){
        for(int j=0;j<51;j++){
            if(grid[i][j] == 'E') {
                printf("\033[32mE\033[0m");
            }else if(grid[i][j] == 'A'){
                printf("\033[34mA\033[0m");
            }else {
                printf("%c", grid[i][j]);
            }
        }
        printf("\n");
    }
}

void stop_bgm() {
    system("pkill mpg123");
}

int main() {
    srand(time(NULL));
    printf("Do you want to play BGM?(you need mpg123) (y/n): ");
    int c = getchar();
    if (c == 'y'){
        system("mpg123 -q ./BGM.mp3 >/dev/null 2>&1 &");
        atexit(stop_bgm);
    }
    while (getchar() != '\n');
    printf("Choose difficulty 1=Easy, 2=Medium, 3=Hard 4=ENDLESS: ");
    int difficulty;
    while(difficulty < 1 || difficulty > 4) {
        if (scanf("%d", &difficulty) != 1) {
            printf("Invalid input. Please enter a number between 1 and 3: ");
            while(getchar() != '\n');
        }
    }
    enable_raw_mode();
    atexit(disable_raw_mode);
    
    struct player p = {25, 19, 3};
    struct bullet b = {};
    int e_num;
    if (difficulty == 1) e_num = 4;
    else if (difficulty == 2) e_num = 8;
    else if (difficulty == 3) e_num = 12;
    else if (difficulty == 4) e_num = 10;
    struct enemy e[e_num];
    for (int i = 0; i < e_num; i++) {
        e[i].x = rand() % 49 + 1;
        e[i].y = 1;
        e[i].active = 1;
    }
    int kill = 0;
    char grid[21][51];
    char command = '\0';
    system("clear");
    printf(
" o            o       _____           _   _                   \n"
"  \\          /       |  ___|         | | | |                  \n"
"   \\        /        | |__  __ _ _ __| |_| |__                \n"
"    :-'\"\"'-:         |  __|/ _` | '__| __| '_ \\               \n"
" .-'  ____  `-.      | |__| (_| | |  | |_| | | |              \n"
"( (  (_()_)  ) )     \\____/\\__,_|_|   \\__|_| |_|              \n"
" `-.   ^^   .-'                                                \n"
"    `._==_.'         ______      __               _           \n"
"    __)(___         |  _  \\    / _|             | |          \n"
"                    | | | |___| |_ ___ _ __   __| | ___ _ __ \n"
"                    | | | / _ \\  _/ _ \\ '_ \\ / _` |/ _ \\ '__|\n"
"                    | |/ /  __/ ||  __/ | | | (_| |  __/ |   \n"
"                    |___/ \\___|_| \\___|_| |_|\\__,_|\\___|_|   \n"
"\n");

    sleep(2);

    while (1) {
        field(grid);
        
        if (kbhit()) {
            command = getchar();
            if (command == 'a' && p.x > 1) p.x--;
            else if (command == 'd' && p.x < 49) p.x++;
            else if (command == 'w' && p.y > 1) p.y--;
            else if (command == 's' && p.y < 19) p.y++;
            else if (command == ' ' && !b.active) {
                b.x = p.x;
                b.y = p.y - 1;
                b.active = 1;
            }else if (command == 'q') {
                system("clear");
                break;
            }
        }
        if (b.active) {
            b.y -= 0.1;            
            if (b.y <= 1) b.active = 0;
            else grid[b.y][b.x] = '|';
        }
        for (int i = 0; i < e_num; i++) {
            if (e[i].active) {
                e[i].y += 0.02;
                if (b.active && (int)b.x == (int)e[i].x && abs((int)b.y - (int)e[i].y) <= 1){
                    e[i].active = 0;
                    b.active = 0;
                    grid[(int)e[i].y][e[i].x] = '*';
                    kill++;
                }else if (e[i].y >= 20) {
                    p.life -= 3;
                    e[i].active = 0;
                }else if((int)e[i].x == p.x && (int)e[i].y == p.y) {
                    e[i].active = 0;
                    p.life--;
                    kill++;
                    grid[(int)e[i].y][e[i].x] = 'X';
                }else {
                    grid[(int)e[i].y][e[i].x] = 'E';
                }
            }

            if (!e[i].active) {
                e[i].x = rand() % 49 + 1;
                e[i].y = 1;
                e[i].active = 1;
            }
        }

        system("clear");
        printf("Life: %d | Pos: (%d,%d) | Kill: %d/%d |Mode: %d enum: %d\n", p.life, p.x, p.y, kill, difficulty * 5+5,difficulty, e_num);
        grid[p.y][p.x] = 'A';
        printer(grid);
        usleep(10000);
        if(!(difficulty == 4) && kill == difficulty * 5 + 5){
            system("clear");
            printf(
" __   _____ ___ _____ ___  _____   __ \n"
" \\ \\ / /_ _/ __|_   _/ _ \\| _ \\ \\ / / \n"
"  \\ V / | | (__  | || (_) |   /\\ V /  \n"
"   \\_/ |___\\___| |_| \\___/|_|_\\ |_|   \n"
"\n            The earth is saved\n");
            sleep(2);
            break;
        } else if(!(difficulty == 4) && p.life < 1){
            system("clear");
            printf(
"  ____                         ___                 _ \n"
" / ___| __ _ _ __ ___   ___   / _ \\__   _____ _ __| |\n"
"| |  _ / _` | '_ ` _ \\ / _ \\ | | | \\ \\ / / _ \\ '__| |\n"
"| |_| | (_| | | | | | |  __/ | |_| |\\ V /  __/ |  |_|\n"
" \\____|\\__,_|_| |_| |_|\\___|  \\___/  \\_/ \\___|_|  (_)\n"
"\n              GAME OVER - YOU DEAD\n\n");
            sleep(2);
            break;
        }
    }

    system("clear");
    printf(
"  _______ _    _ ______   ______ _   _ _____  \n"
" |__   __| |  | |  ____| |  ____| \\ | |  __ \\ \n"
"    | |  | |__| | |__    | |__  |  \\| | |  | |\n"
"    | |  |  __  |  __|   |  __| | . ` | |  | |\n"
"    | |  | |  | | |____  | |____| |\\  | |__| |\n"
"    |_|  |_|  |_|______| |______|_| \\_|_____/\n\n");
}
