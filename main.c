#include <stdio.h>
#include <stdlib.h>

typedef enum {
    false, true
} bool;

typedef struct {
    int width, height;
    int **shape;
} Maze;
typedef struct Point {
    int row, column;
} *Point;
typedef struct Node {
    Point point;
    struct Node *after;
} *Stack;

void init(){
#ifdef WIN32
#ifdef ON_CLION_CONSOLE
    printf("run on Clion for windows.\n");
#else
    system("chcp 65001 & cls");
#endif
#endif
}
void readFile(Maze *maze);

void printMaze(Maze *maze, bool raw);

void pushIntoStack(Stack *stack, Point point);

void popFromStack(Stack *stack);

Maze *mazeDeepCopy(Maze *maze);

void depthFirstSearch(Maze *maze);
void breadthFirstSearch(Maze *maze);

int main() {
    init();
    Maze *maze = malloc(sizeof(Maze));
    readFile(maze);
    printMaze(maze, false);
    depthFirstSearch(maze);
    printf("----------------BREADTH FIRST SEARCH----------------\n");
    breadthFirstSearch(maze);
    return 0;
}

void readFile(Maze *maze) {
    char *path = malloc(sizeof(char) * 128);
    printf("Please input the file path:");
    scanf("%s", path);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("Bad file path, can not read file \033[31m%s\033[0m, ensure file exist and have privilege to read.",
               path);
        exit(1);
    }
    // Read the first line from the file to determine the line wrapping method
    int crlf = 1;
    while (true) {
        char ch = fgetc(file);
        if (ch == EOF) {
            break;
        }
        if (ch == '\r') {
            ch = fgetc(file);
            if (ch == '\n') {
                crlf = 2;
            }
            break;
        }
        if (ch == '\n') {
            break;
        }
    }
    fseek(file, 0, SEEK_SET);
    // Read the width of the first row as the requirement for the width of all subsequent rows,
    // and check whether the format meets the requirements
    int width = 0;
    while (true) {
        char ch = fgetc(file);
        if (ch == '\r' || ch == '\n' || ch == EOF) {
            fseek(file, 0, SEEK_SET);
            break;
        } else {
            if (ch == '0' || ch == '1') {
                width = width + 1;
            } else {
                fseek(file, 0, SEEK_SET);
                char firstLine[width + 1 + 1];
                fgets(firstLine, width + 1 + 1, file);
                printf("Bad characters in first line, column %d:\n%s\n", width + 1, firstLine);
                for (int i = 0; i < width; i += 1) {
                    printf(" ");
                }
                printf("^\nEnsure just 0 or 1 appears in the table.");
                fclose(file);
                exit(1);
            }
        }
    }
    if (width < 3) {
        printf("Bad shape of this file, ensure each line width >= 3");
        exit(1);
    }
    // The read height also detects whether each row is as wide as the first row
    int height = 0;
    while (true) {
        char row[width + crlf];
        char *status = fgets(row, width + crlf, file);
        if (status == NULL) {
            fseek(file, 0, SEEK_SET);
            break;
        }
        char c = fgetc(file);
        if (
                (crlf == 1 && row[width] == '\0' && (c == '\r' || c == '\n' || c == EOF)) ||
                (crlf == 2 && (row[width] == '\r' || row[width] == '\0') && row[width + 1] == '\0' &&
                 (c == '\n' || c == EOF))
                ) {
            height += 1;
        } else {
            printf("Bad shape of this file in line %d", height + 1);
            printf("\nEnsure each line width equals the first line width %d", width);
            exit(1);
        }
    }
    if (height < 3) {
        printf("Bad shape of this file, ensure each line height >= 3");
        exit(1);
    }
    maze->width = width;
    maze->height = height;
    maze->shape = malloc(sizeof(int *) * height);
    height = 0;
    // Read in the elements and check each for compliance
    while (true) {
        char line[width + crlf];
        char *status = fgets(line, width + crlf, file);
        if (status == NULL) {
            break;
        }
        int *row = malloc(sizeof(int) * width);
        for (int i = 0; i < width; i += 1) {
            // check if first line and last line is all 1
            if ((height == 0 || height == maze->height - 1) && line[i] != '1') {
                printf("Bad content of this file in line %d, column %d:\n%s\n", height + 1, i + 1, line);
                for (int j = 0; j < i; j += 1) {
                    printf(" ");
                }
                printf("^\nEnsure the border must be wall (value must be 1).");
                exit(1);
            }
            // check if first column and last column is all 1
            if ((i == 0 || i == width - 1) && line[i] != '1') {
                printf("Bad content of this file in line %d, column %d:\n%s\n", height + 1, i + 1, line);
                for (int j = 0; j < i; j += 1) {
                    printf(" ");
                }
                printf("^\nEnsure the border must be wall (value must be 1).");
                exit(1);
            }
            // check if other line is all 0 or 1
            if (line[i] != '1' && line[i] != '0') {
                printf("Bad content of this file in line %d, column %d:\n%s\n", height + 1, i + 1, line);
                for (int j = 0; j < i; j += 1) {
                    printf(" ");
                }
                printf("^\nEnsure each line width equals the first line width %d", width);
                exit(1);
            } else {
                row[i] = line[i] - '0';
            }
        }
        fgetc(file);
        maze->shape[height] = row;
        height += 1;
    }
}

void printMaze(Maze *maze, bool raw) {
    if (raw) {
        for (int i = 0; i < maze->height; i += 1) {
            for (int j = 0; j < maze->width; j += 1) {
                printf("%d", maze->shape[i][j]);
            }
            printf("\n");
        }
        return;
    }
    printf("Maze graph with cosmetic:\nThe symbol meanings: \n  □: available, ■: wall, ↓: down, →: right, ←: left, ↑: up,"
           " ○: start, ●: end, ◇: rollback, *: scanned\n");
    for (int i = 0; i < maze->height; i += 1) {
        for (int j = 0; j < maze->width; j += 1) {
            if (i == 1 && j == 1) {
                printf("○");
                continue;
            }
            if (i == maze->height - 2 && j == maze->width - 2) {
                printf("●");
                continue;
            }
            char *direction;
            switch (maze->shape[i][j]) {
                case 0: {
                    direction = "□";
                    break;
                }
                case 1: {
                    direction = "■";
                    break;
                }
                case 2: {
                    direction = "↓";
                    break;
                }
                case 3: {
                    direction = "→";
                    break;
                }
                case 4: {
                    direction = "←";
                    break;
                }
                case 5: {
                    direction = "↑";
                    break;
                }
                case 6: {
                    direction = "◇";
                    break;
                }
                case 7:{
                    direction = "*";
                    break;
                }
            }
            printf("%s", direction);
        }
        printf("\n");
    }
}

void pushIntoStack(Stack *stack, Point point) {
    Stack node = malloc(sizeof(struct Node));
    node->point = point;
    node->after = *stack;
    *stack = node;
}

void popFromStack(Stack *stack) {
    Stack node = *stack;
    *stack = node->after;
    free(node);
}

Maze *mazeDeepCopy(Maze *maze) {
    Maze *newMaze = malloc(sizeof(Maze));
    newMaze->width = maze->width;
    newMaze->height = maze->height;
    newMaze->shape = malloc(sizeof(int *) * maze->height);
    for (int i = 0; i < maze->height; i += 1) {
        int *row = malloc(sizeof(int) * maze->width);
        for (int j = 0; j < maze->width; j += 1) {
            row[j] = maze->shape[i][j];
        }
        newMaze->shape[i] = row;
    }
    return newMaze;
}

void depthFirstSearch(Maze *maze) {
    Maze* workspace = mazeDeepCopy(maze);
    Stack stack = NULL;
    Point point = malloc(sizeof(struct Point));
    point->row = 1;
    point->column = 1;
    pushIntoStack(&stack, point);
    int steps = 0;
    // 0 available, 1 wall, 2 down, 3 left, 4 right, 5 up , 6 returned
    printf("Explore started.\n");
    while (stack != NULL) {
        steps += 1;
        point = stack->point;
        int row = point->row;
        int column = point->column;
        if (row == maze->height - 2 && column == maze->width - 2) {
            printf("Explore finished.\n");
            break;
        }
        // down
        if (workspace->shape[row + 1][column] == 0) {
            workspace->shape[row][column] = 2;
            Point newPoint = malloc(sizeof(struct Point));
            newPoint->row = row + 1;
            newPoint->column = column;
            printf("  [Explore %d] (%d, %d) -> (%d, %d), towards DOWN\n", steps, row + 1, column + 1, row + 1 + 1, column + 1);
            pushIntoStack(&stack, newPoint);
        } else {
            // right
            if (workspace->shape[row][column + 1] == 0) {
                workspace->shape[row][column] = 3;
                Point newPoint = malloc(sizeof(struct Point));
                newPoint->row = row;
                newPoint->column = column + 1;
                printf("  [Explore %d] (%d, %d) -> (%d, %d), towards RIGHT\n", steps, row + 1, column + 1, row + 1, column + 1 + 1);
                pushIntoStack(&stack, newPoint);
            } else {
                // left
                if (workspace->shape[row][column - 1] == 0) {
                    workspace->shape[row][column] = 4;
                    Point newPoint = malloc(sizeof(struct Point));
                    newPoint->row = row;
                    newPoint->column = column - 1;
                    printf("  [Explore %d] (%d, %d) -> (%d, %d), towards LEFT\n", steps, row + 1, column + 1, row + 1, column - 1 + 1);
                    pushIntoStack(&stack, newPoint);
                } else {
                    // up
                    if (workspace->shape[row - 1][column] == 0) {
                        workspace->shape[row][column] = 5;
                        Point newPoint = malloc(sizeof(struct Point));
                        newPoint->row = row - 1;
                        newPoint->column = column;
                        printf("  [Explore %d] (%d, %d) -> (%d, %d), towards UP\n", steps, row + 1, column + 1, row + 1 - 1, column + 1);
                        pushIntoStack(&stack, newPoint);
                    } else {
                        workspace->shape[row][column] = 6;
                        popFromStack(&stack);
                        if(stack != NULL)
                        printf("  [Backtrack %d] (%d, %d) -> (%d, %d)\n", steps, row + 1, column + 1, stack->point->row + 1,
                               stack->point->column + 1);
                    }
                }
            }
        }
    }
    if (stack == NULL) {
        printf("No solution found.\nSearch path graph:\n");
        printMaze(workspace, false);
    } else {
        printf("Solution found in %d steps:\n", steps);
        printMaze(workspace, false);
    }
}
// additional search method realize
typedef struct SuperPoint{
    int row, column, direction;
    struct SuperPoint* before, * after;
}* SuperPoint;
typedef SuperPoint Queue;
void popFromQueue(Queue *queue){
    if(queue == NULL){
        return;
    }
    SuperPoint head = (*queue)->after;
    *queue = head;
    // should not free head, cause the pointer `before` will use to show path in the end
}
void pushIntoQueue(Queue *queue, SuperPoint superPoint){
    if(*queue == NULL){
        *queue = superPoint;
        return;
    }
    SuperPoint node = *queue;
    while (node->after != NULL){
        node = node->after;
    }
    node->after = superPoint;
}
bool pointIsInQueue(Queue *queue, SuperPoint superPoint){
    SuperPoint node = *queue;
    while (node != NULL){
        if(node->row == superPoint->row && node->column == superPoint->column){
            return true;
        }
        node = node->after;
    }
    return false;
}
void breadthFirstSearch(Maze *maze) {
    Maze *workspace = mazeDeepCopy(maze);
    Queue queue = NULL;
    SuperPoint superPoint = malloc(sizeof(struct SuperPoint));
    superPoint->row = 1;
    superPoint->column = 1;
    superPoint->direction = 0;
    superPoint->before = NULL;
    superPoint->after = NULL;
    pushIntoQueue(&queue, superPoint);
    int steps = 0;
    // 0 available, 1 wall, 7 visited
    int VISITED_FLAG = 7;
    printf("Scan started.\n");
    while (queue != NULL){
        steps += 1;
        workspace->shape[queue->row][queue->column] = VISITED_FLAG;
        // right
        if(workspace->shape[queue->row][queue->column + 1] == 0){
            superPoint = malloc(sizeof(struct SuperPoint));
            superPoint->row = queue->row;
            superPoint->column = queue->column + 1;
            superPoint->before = queue;
            superPoint->direction = 3;
            superPoint->after = NULL;
            if(!pointIsInQueue(&queue, superPoint)){
                pushIntoQueue(&queue, superPoint);
                workspace->shape[queue->row][queue->column + 1] = VISITED_FLAG;
                printf("  [Scan %d] (%d, %d) -> (%d, %d), towards RIGHT\n", steps, queue->row + 1, queue->column + 1, queue->row + 1, queue->column + 1 + 1);
            }
            if(queue->row == maze->height - 2 && queue->column + 1 == maze->width - 2){
                printf("Scan finished.\n");
                break;
            }
        }
        // down
        if(workspace->shape[queue->row + 1][queue->column] == 0){
            superPoint = malloc(sizeof(struct SuperPoint));
            superPoint->row = queue->row + 1;
            superPoint->column = queue->column;
            superPoint->before = queue;
            superPoint->direction = 2;
            superPoint->after = NULL;
            if(!pointIsInQueue(&queue, superPoint)){
                pushIntoQueue(&queue, superPoint);
                workspace->shape[queue->row + 1][queue->column] = VISITED_FLAG;
                printf("  [Scan %d] (%d, %d) -> (%d, %d), towards DOWN\n", steps, queue->row + 1, queue->column + 1, queue->row + 1 + 1, queue->column + 1);
            }
            if(queue->row + 1 == maze->height - 2 && queue->column == maze->width - 2){
                printf("Scan finished.\n");
                break;
            }
        }
        // left
        if(workspace->shape[queue->row][queue->column - 1] == 0){
            superPoint = malloc(sizeof(struct SuperPoint));
            superPoint->row = queue->row;
            superPoint->column = queue->column - 1;
            superPoint->before = queue;
            superPoint->direction = 4;
            superPoint->after = NULL;
            if(!pointIsInQueue(&queue, superPoint)){
                pushIntoQueue(&queue, superPoint);
                workspace->shape[queue->row][queue->column - 1] = VISITED_FLAG;
                printf("  [Scan %d] (%d, %d) -> (%d, %d), towards LEFT\n", steps, queue->row + 1, queue->column + 1, queue->row + 1, queue->column - 1 + 1);
            }
            if(queue->row == maze->height - 2 && queue->column - 1 == maze->width - 2){
                printf("Scan finished.\n");
                break;
            }
        }
        // up
        if(workspace->shape[queue->row - 1][queue->column] == 0){
            superPoint = malloc(sizeof(struct SuperPoint));
            superPoint->row = queue->row - 1;
            superPoint->column = queue->column;
            superPoint->before = queue;
            superPoint->direction = 5;
            superPoint->after = NULL;
            if(!pointIsInQueue(&queue, superPoint)){
                pushIntoQueue(&queue, superPoint);
                workspace->shape[queue->row - 1][queue->column] = VISITED_FLAG;
                printf("  [Scan %d] (%d, %d) -> (%d, %d), towards UP\n", steps, queue->row + 1, queue->column + 1, queue->row + 1 - 1, queue->column + 1);
            }
            if(queue->row - 1 == maze->height - 2 && queue->column == maze->width - 2){
                printf("Scan finished.\n");
                break;
            }
        }
        popFromQueue(&queue);
    }
    if(queue == NULL){
        printf("No solution found.\nSearch path graph:\n");
        printMaze(workspace, false);
        return;
    }
    int n = 0;
    while(superPoint->before != NULL){
        n += 1;
        (*(superPoint->before)).after = superPoint;
        superPoint = superPoint->before;
    }
    printf("Total %d steps, ", n);
    printf("The shortest path is:\n");
    while(superPoint != NULL){
        printf("(%d, %d)", superPoint->row + 1, superPoint->column + 1);
        workspace->shape[superPoint->row][superPoint->column] = superPoint->direction;
        superPoint = superPoint->after;
        if(superPoint != NULL){
            printf(" -> ");
        } else{
            printf("\n");
        }
    }
    printMaze(workspace, false);
}