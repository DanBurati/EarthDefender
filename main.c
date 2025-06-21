// EarthDefender version: 1.0
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


void field(char grid[11][41]){
    for(int i=0;i<11;i++){
        for(int j=0;j<41;j++){
            if ((i == 0 || i == 10) && (j == 0 || j == 40)) grid[i][j] = '@';
            else if (i == 0 || i == 10) grid[i][j] = '_';
            else if (j == 0 || j == 40) grid[i][j] = '|';
            else grid[i][j] = ' ';
        }

    }
}

void printer(char grid[11][41]){
    for(int i=0;i<11;i++){
        for(int j=0;j<41;j++){
            printf("%c",grid[i][j]);
        }
        printf("\n");
    }
}

void stop_bgm() {
    system("pkill mpg123");
}

void main() {
    srand(time(NULL));
    printf("Do you want to play BGM?(you need mpg123) (y/n): ");
    int c = getchar();
    if (c == 'y'){
        system("mpg123 -q /home/dan/code/C/EarthDefender/BGM.mp3 >/dev/null 2>&1 &");
        atexit(stop_bgm);
    }
    enable_raw_mode();
    atexit(disable_raw_mode);
    
    struct player p = {20, 9, 3};
    struct bullet b = {};
    struct enemy e = {(rand()%40)+1,1,1};
    int kill = 0;
    char grid[11][41];
    char command = '\0';
    system("clear");
    printf(
" _____           _   _                   \n"
"|  ___|         | | | |                  \n"
"| |__  __ _ _ __| |_| |__                \n"
"|  __|/ _` | '__| __| '_ \\               \n"
"| |__| (_| | |  | |_| | | |              \n"
"\\____/\\__,_|_|   \\__|_| |_|              \n"
"                                        \n"
"                                        \n"
"______      __               _          \n"
"|  _  \\    / _|             | |         \n"
"| | | |___| |_ ___ _ __   __| | ___ _ __\n"
"| | | / _ \\  _/ _ \\ '_ \\ / _` |/ _ \\ '__|\n"
"| |/ /  __/ ||  __/ | | | (_| |  __/ |   \n"
"|___/ \\___|_| \\___|_| |_|\\__,_|\\___|_|   \n"
"                                        \n"
"                                        \n"
);
    sleep(3);

    while (1) {
        field(grid);
        
        if (kbhit()) {
            command = getchar();
            if (command == 'a' && p.x > 1) p.x--;
            else if (command == 'd' && p.x < 39) p.x++;
            else if (command == 's' && !b.active) {
                b.x = p.x;
                b.y = p.y - 1;
                b.active = 1;
            }
        }

        if (b.active) {
            b.y--;
            if (b.y <= 1) b.active = 0;
            else grid[b.y][b.x] = '.';
        }
        if (e.active) {
            e.y += 0.2;
            if (b.active && b.x == e.x && abs(b.y - (int)e.y) <= 1){
                e.active = 0;
                b.active = 0;
                grid[(int)e.y][e.x] = '*';
                kill++;
            }else if (e.y >= 10) {
                e.active = 0;
                p.life--;
            }else {
                grid[(int)e.y][e.x] = 'E';
            }
        }

        if (!e.active) {
            e.x = rand() % 39 + 1;
            e.y = 1;
            e.active = 1;
        }

        system("clear");
        printf("Life: %d | Pos: (%d,%d) | Kill: %d \n", p.life, p.x, p.y, kill);
        grid[p.y][p.x] = 'A';
        printer(grid);
        usleep(100000);
        if (kill == 3){
            system("clear");
            printf(
" __   _____ ___ _____ ___  _____   __ \n"
" \\ \\ / /_ _/ __|_   _/ _ \\| _ \\ \\ / / \n"
"  \\ V / | | (__  | || (_) |   /\\ V /  \n"
"   \\_/ |___\\___| |_| \\___/|_|_\\ |_|   \n"
"                                     \n"
"            The earth is saved\n");


            sleep(2);
            break;
        } else if(p.life < 1){
            system("clear");
            printf(
"  ____                         ___                 _ \n"
" / ___| __ _ _ __ ___   ___   / _ \\__   _____ _ __| |\n"
"| |  _ / _` | '_ ` _ \\ / _ \\ | | | \\ \\ / / _ \\ '__| |\n"
"| |_| | (_| | | | | | |  __/ | |_| |\\ V /  __/ |  |_|\n"
" \\____|\\__,_|_| |_| |_|\\___|  \\___/  \\_/ \\___|_|  (_)\n"
"\n"
"              GAME OVER - YOU DEAD\n\n"
);

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
"    |_|  |_|  |_|______| |______|_| \\_|_____/\n"
"\n"
);
}