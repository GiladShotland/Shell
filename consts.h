char lastCommand[1024];
#define IFOK 0
#define IFERR -1
enum ifelsestates{
    REGULAR,
    THEN,
    ELSE,
    ELSE_BLOCK,
    THEN_BLOCK
};
int state = REGULAR, result = IFOK,prevState = 0;

typedef struct args{
    char* command[10];
    struct args *next;
    struct args *prev;
}args;
