#define _CRT_SECURE_NO_WARNINGS
#define MAXELEMENT 2048
#define MAXROW 256
#define MAXCOL 256
#define TRUE 1
#define FALSE 0

#include<stdio.h>

//방향 오프셋 관련
typedef struct {
    short int vert;
    short int horiz;
} offsets;
offsets move[8];
void setOffsets() {
    move[0].vert = -1;  move[0].horiz = 0;
    move[1].vert = -1;  move[1].horiz = 1;
    move[2].vert = 0;   move[2].horiz = 1;
    move[3].vert = 1;   move[3].horiz = 1;
    move[4].vert = 1;   move[4].horiz = 0;
    move[5].vert = 1;   move[5].horiz = -1;
    move[6].vert = 0;   move[6].horiz = -1;
    move[7].vert = -1;  move[7].horiz = -1;
}
char dir_str[8][3] = { "N","NE","E","SE","S", "SW", "W", "NW" };

//스택 관련
typedef struct {
    short int row;
    short int col;
    short int dir;
}element;
element stack[MAXELEMENT];
element zero;
int top = -1;
element pop();
void push(element e);
int isStackFull();
int isStackEmpty();
void resetStack();

//배열
short int maze[MAXROW + 1][MAXCOL + 1];
short int mark[MAXROW + 1][MAXCOL + 1];
short int backtrackRecord[MAXROW + 1][MAXCOL + 1];
short int row_size, col_size; //입력 받을 메이즈의 사이즈.
short int row, col; //현재 위치 - 프린트 과정을 따로 빼기 위해서 전역변수로 선언함
short int nowDir;
short int setDir = 0; //처음 살펴볼 방향 설정을 위한 변수
short int found = FALSE; //찾았는지 체크
short int numOfBackTrack = -1;

//예제
//복잡하고 긴 maze 예시a
#define EG1_ROW 8
#define EG1_COL 10
short int mazeEg1[EG1_ROW][EG1_COL] = { {0,0,0,0,0,0,1,0,1,0},
                                          {1,0,1,1,1,1,0,1,1,0},
                                          {0,1,0,1,0,1,1,1,0,0},
                                          {0,1,1,1,0,0,1,1,0,0},
                                          {0,1,1,0,1,0,0,1,0,0},
                                          {1,0,1,0,1,0,1,1,0,1},
                                          {0,1,1,0,1,0,1,0,1,1},
                                          {0,0,0,1,1,1,0,1,0,0} };
//중간복잡도의 중간 길이 maze 예시

#define EG2_ROW 7
#define EG2_COL 6
short int mazeEg2[EG2_ROW][EG2_COL] = { {0,0,0,1,0,0},
                                          {1,1,1,0,0,0},
                                          {1,0,0,1,1,1},
                                          {1,0,1,1,1,1},
                                          {0,1,0,0,0,1},
                                          {0,1,1,1,1,0},
                                          {0,1,1,1,0,0} };

//입출력 함수 + 유틸리티
void menu();
void setMaze(int r, int c,short int *m);
void playMaze_Random();
void playMaze_Custom();
void playMaze_Example(int a);
void printMazeWithPath();
void resetAll();

int returnDir(int a, int b) {
    int i;

    if(row == a && col == b){
        return nowDir;
    }

    for (i = 0; i <= top; i++) {
        if (stack[i].row == a && stack[i].col ==b ) {
            return stack[i].dir-1;
        }
    }
    return -1;
}

//길찾기 함수
void path();

//메인
int main() {
    setOffsets();
    menu();

    return 0;
}

element pop() {
    if (isStackEmpty()) {
        printf("스택이 비었음");
        element temp;
        temp.row = -1;
        temp.col = -1;
        temp.dir = -1;
        return temp;
    }
    else return stack[top--];
}
void push(element e) {
    if (isStackFull()) {
        printf("스택이 가득 찼음");
    }
    else stack[++top] = e;
}
int isStackFull() {
    if (top >= MAXELEMENT) return 1;
    else return 0;
}
int isStackEmpty() {
    if (top == -1) return 1;
    else return 0;
}
void resetStack() {
    for (int i = 0; i <= 0; i++) {
        stack[i] = zero;
    }
    top = -1;
}

void menu() {
    while (1) {
        resetAll();

        printf("\n\n\n");
        printf("\t * 미로 입력 모드를 선택하세요. (default - Random)\n");
        printf("\t| A : Example1(예제1번) \t| B : Example2(예제2번) \t| U : User Input(수동 입력) \t| R : Random(랜덤 - 구현X) \t| Q : 나가기 \t \n");
        printf("\t: ");
        char mode;
        scanf("%c", &mode);
        printf("\n");

        if (mode == 'U' || mode == 'u') {
            printf("\t! 미로를 수동으로 입력합니다.\n");
            playMaze_Custom();
        }
        else if (mode == 'Q' || mode == 'q') {
            printf("\t! 종료합니다.\n");
            break;
        }
        else if (mode == 'A' || mode == 'a') {
            printf("\t! 미로 데이터가 예제1번으로 입력됩니다.\n");
            playMaze_Example(1);
        }
        else if (mode == 'B' || mode == 'b') {
            printf("\t! 미로 데이터가 예제2번으로 입력됩니다.\n");
            playMaze_Example(2);
        }
        else {
            printf("\t! 미로 데이터가 랜덤으로 입력됩니다.\n");
            playMaze_Random();
        }


    }
}


void setMaze(int r, int c,short int *m) {
    row_size = r;
    col_size = c;
    int i, j;
    //printf("%d, %d", r, c);
    for (i = 0; i < row_size; i++) {
        for (j = 0; j < col_size; j++) {
            maze[i + 1][j + 1] = *(m+i*col_size+j);
        }
        //printf("\n");
    }
}

void playMaze_Random() {
    //구현 안함
}
void playMaze_Custom() {
    printf("\t * Row와 Col 사이즈를 입력하세요\n\t:");
    scanf("%hd %hd", &row_size, &col_size);

    printf("\t * 미로를 입력하세요\n");
    int i, j;
    for (i = 1; i <= row_size; i++) {
        for (j = 1; j <= col_size; j++) {
            scanf("%hd", &maze[i][j]);
        }
    }

    printf("\n\n\n");
    printf("\t * 기본 탐색 방향을 입력하세요 (default - 0(North))\n");
    printf("\t| 7 : NW  \t| 0 : N  \t| 1 : NE \n");
    printf("\t| 6 : W   \t         \t| 2 : E \n");
    printf("\t| 5 : SW  \t| 4 : S  \t| 3 : SE \n");
    printf("\t: ");
    short int setDir_i;
    scanf("%hd", &setDir_i);
    if (setDir_i >= 0 && setDir_i <= 7) {
        setDir = setDir_i;
    }

    printf("\n\t **** 예제 1번, 최초 탐색 방향 : %s(%d번 방향) ****", dir_str[setDir], setDir);
    path();
    printMazeWithPath();
}

void playMaze_Example(int a) {
    short int i;
    if (a == 1) {
        //첫 번째 예제
        while(1){
            resetAll();
            printf("\n");
            printf("\t * 기본 탐색 방향을 입력하세요 (default - 9(Back))\n");
            printf("\t| 7 : NW  \t| 0 : N  \t| 1 : NE \n");
            printf("\t| 6 : W   \t| 9 : Back\t| 2 : E \n");
            printf("\t| 5 : SW  \t| 4 : S  \t| 3 : SE \n");
            printf("\t: ");
            short int setDir_i;
            scanf("%hd", &setDir_i);
            if (setDir_i >= 0 && setDir_i <= 7) {
                setDir = setDir_i;
            } else{
                printf(" * 메뉴로 돌아갑니다.");
                break;
            }
            setMaze(EG1_ROW, EG1_COL, (short *) (int *) mazeEg1);
            printf("\n\t **** 예제 1번, 최초 탐색 방향 : %s(%d번 방향) ****", dir_str[setDir], setDir);
            path();
            printMazeWithPath();
        }
    }
    if (a == 2) {
        //두 번째 예제
        while(1) {
            resetAll();
            printf("\n");
            printf("\t * 기본 탐색 방향을 입력하세요 (default - 9(Back))\n");
            printf("\t| 7 : NW  \t| 0 : N  \t| 1 : NE \n");
            printf("\t| 6 : W   \t| 9 : Back\t| 2 : E \n");
            printf("\t| 5 : SW  \t| 4 : S  \t| 3 : SE \n");
            printf("\t: ");
            short int setDir_i;
            scanf("%hd", &setDir_i);
            if (setDir_i >= 0 && setDir_i <= 7) {
                setDir = setDir_i;
            } else{
                printf(" * 메뉴로 돌아갑니다.");
                break;
            }
            setMaze(EG2_ROW, EG2_COL, (short *) (int *) mazeEg2);
            printf("\n\t **** 예제 2번, 최초 탐색 방향 : %s(%d번 방향) ****", dir_str[setDir], setDir);
            path();
            printMazeWithPath();
        }
    }
}

void printMazeWithPath() {
    int i,j;
    //printf("%d, %d\n", row_size, col_size);
    /*
    for(i=0;i<row_size+2;i++){
        for(j=0;j<col_size+2;j++){
            printf("%d ", maze[i][j]);
        }
        printf("\n");
    }*/


    for(i=0;i<=top;i++){
        maze[stack[i].row][stack[i].col] = 3;
    }
    maze[1][1] = 2;
    maze[row][col] =3;
    maze[row_size][col_size] =4;

    printf("\n\n");
    printf(" * 미로 경로와 backtrack 위치\t\t\t \n");
    if(found){ // 찾았을 때
        for(i=0;i<row_size+2;i++){
            for(j=0;j<col_size+2;j++){
                if(maze[i][j] == 2 || maze[i][j] == 3){
                    maze[i][j] = 0;
                    printf("\033[34m%d\033[0m ", returnDir(i,j)%8); // 경로 파란색
                }else if(maze[i][j] == 4){
                    maze[i][j] = 0;
                    printf("\033[31m○\033[0m "); // 도착점 빨간색
                }else if(backtrackRecord[i][j] == 1){
                    backtrackRecord[i][j] = 0;
                    printf("\033[95m%d\033[0m ", backtrackRecord[i][j]); // backtrack 위치 핑크색
                }else{
                    printf("%d ", maze[i][j]);
                }
            }

            printf("\n");
        }
        printf(" * BackTrack 횟수 : %d\n\n", numOfBackTrack);
    } else{
        //길을 찾지 못했습니다.
    }
}
void resetAll() {
    resetStack();
    setDir = 0;
    row_size = 0;
    row = 0;
    col = 0;
    col_size = 0;
    found =0;
    numOfBackTrack = -1;

    int i, j;
    for (i = 0; i < MAXROW + 1; i++) {
        for (j = 0; j < MAXCOL + 1; j++) {
            maze[i][j] = 1;
            mark[i][j] = 0;
            backtrackRecord[i][j] = 0;
        }
    }
}


void path() {
    short int i, nextRow, nextCol, dir;
    element position;
    mark[1][1] = 1; top =0;
    stack[0].row = 1; stack[0].col = 1; stack[0].dir = setDir;
    while (top>-1 && !found){
        position = pop();
        numOfBackTrack++;
        row = position.row; col = position.col;
        dir = position.dir;
        while(dir<8+setDir && !found){
            nextRow = row + move[dir%8].vert;
            nextCol = col + move[dir%8].horiz;
            if(nextRow == row_size && nextCol == col_size) {
                nowDir = dir;
                found = TRUE;
            }else if(!maze[nextRow][nextCol] && !mark[nextRow][nextCol]){
                mark[nextRow][nextCol] = 1;
                position.row = row; position.col = col;
                position.dir = ++dir;
                push(position);
                row = nextRow; col = nextCol; dir = setDir;
            }else{
                ++dir;
            }
        }
        if(!found) {
            backtrackRecord[row][col] = 1;
        }
    }

    if (found) {
        printf("\nThe path is:\n");
        printf("row col\n");
        for (i = 0; i <= top; i++)
            printf(" %2d  to %3d\n", stack[i].row, stack[i].col);
        printf(" %2d  to %3d\n", row, col);
        printf(" %2d  to %3d\n", row_size, col_size);
    }else{
        printf("The maze does not have a path\n");
    }
}
