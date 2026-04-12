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

struct node
{
    Coordinate move;
    int visits;
    float score;
    
    Possibilities possibles;
    
    struct node *child;
    struct node *sibling;
};

typedef struct node *Tree;