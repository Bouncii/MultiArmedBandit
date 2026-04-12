#define SIZE 10
#define VICTORY 5

typedef enum{
    empty,
    Player,
    Bot
} Case;

typedef struct
{
    int ligne;
    int colonne;
} Coordinate;


typedef Case Grid[SIZE][SIZE];

struct location
{
    Coordinate info;
    struct location *suiv;
};

typedef struct location *Possibilities;

struct tree
{
    float info;
    struct tree *fg;
    struct tree *fd;
};

typedef struct tree *tree;