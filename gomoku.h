#include <stdbool.h>

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
    Coordinate move; // Le coup qui a mené à ce noeud (inutile pour le noeud racine)
    Case playerTurn; // Le joueur qui a joué ce coup (Player ou Bot)
    
    int nbTest;  //Nombre de fois que ce noeud a été visité
    float score; // Nombre de victoires (on utilise un float pour gérer les matchs nuls éventuels)
    
    Possibilities untriedMoves; // Les coups légaux qu'il reste à explorer depuis cette position
    
    struct node **children;// Tableau dynamique de pointeurs vers les noeuds enfants
    int numChildren; //Le nombre d'enfants actuellement dans le tableau
    int maxChildren; // La capacité actuelle du tableau
    
    struct node *parent;// Pointeur vers le parent
};

typedef struct node *Tree;

bool hasNeighbor(Grid grid, int r, int c);
Coordinate getUrgentMove(Grid g, Case playerToMove);
bool isWinningMove(Grid g, Coordinate c, Case player);