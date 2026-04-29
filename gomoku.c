#include "gomoku.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>



int randint(int min,int max)
{
    int random = rand()%(max+1-min) +min;
    return random;
}

void initGrille(Grid grid)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            grid[i][j] = empty;
        }
        
    }
    
}

void displayGrid(Grid grid) {
    printf("\n   ");
    for (int j = 0; j < SIZE; j++){
        printf("%d ", j);
    }
    printf("\n");

    for (int i = 0; i < SIZE; i++) {
        printf("%d |", i);
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == Player) {
                printf("X ");
            }
            else if (grid[i][j] == Bot) {
                printf("O ");
            }
            else {
                printf(". ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void placePawn(Grid grid,Case pawn,Coordinate co)
{
    grid[co.ligne][co.colonne] = pawn;
}

void removePawn(Grid grid, Coordinate co) {
    grid[co.ligne][co.colonne] = empty;
}

Possibilities initPossibilities()
{
    Possibilities p = NULL;
    return p;
}

void addPossibility(Possibilities *p,Coordinate c)
{
    struct location *temp = malloc(sizeof(struct location));
    temp -> info = c;
    temp -> suiv = *p;
    *p = temp;
}

bool coordinatesEquals (Coordinate c1,Coordinate c2)
{
    return c1.colonne == c2.colonne && c1.ligne == c2.ligne;
}

void removePossibility (Possibilities *p, Coordinate c)
{
    struct location *ptint = *p;
    bool found = false;
    if (*p != NULL)
    {
        if (coordinatesEquals((*p)->info, c))
        {
            struct location *temp = *p;
            *p = (*p)->suiv;
            free(temp);
        }else
        {
            while (!found && ptint -> suiv != NULL)
            {
                if (coordinatesEquals(ptint -> suiv -> info, c))
                {
                    struct location *temp = ptint->suiv;
                    ptint->suiv = temp -> suiv;
                    free(temp);
                    found = true;
                }else
                {
                    ptint = ptint ->suiv;
                }
            }
        }
    }
}

Case isWinner(Grid g)
{
    Case winner = empty;
    int i = 0;
    int j;

    while (i < SIZE && winner == empty)
    {
        j = 0;
        while (j < SIZE && winner == empty)
        {
            Case current_pawn = g[i][j];

            if (current_pawn != empty)
            {
                if (j <= SIZE - VICTORY && winner == empty)
                {
                    bool win = true;
                    int k = 1;
                    while (k < VICTORY && win)
                    {
                        if (g[i][j+k] != current_pawn) { 
                            win = false; 
                        }
                        k++;
                    }
                    if (win) {
                        winner = current_pawn;
                    }
                }

                if (i <= SIZE - VICTORY && winner == empty)
                {
                    bool win = true;
                    int k = 1;
                    while (k < VICTORY && win)
                    {
                        if (g[i+k][j] != current_pawn) { 
                            win = false; 
                        }
                        k++;
                    }
                    if (win) {
                        winner = current_pawn;
                    }
                }

                if (i <= SIZE - VICTORY && j <= SIZE - VICTORY && winner == empty)
                {
                    bool win = true;
                    int k = 1;
                    while (k < VICTORY && win)
                    {
                        if (g[i+k][j+k] != current_pawn) { 
                            win = false; 
                        }
                        k++;
                    }
                    if (win) {
                        winner = current_pawn;
                    }
                }

                if (i >= VICTORY - 1 && j <= SIZE - VICTORY && winner == empty)
                {
                    bool win = true;
                    int k = 1;
                    while (k < VICTORY && win)
                    {
                        if (g[i-k][j+k] != current_pawn) { 
                            win = false; 
                        }
                        k++;
                    }
                    if (win) {
                        winner = current_pawn;
                    }
                }
            }
            j++;
        }
        i++;
    }
    return winner;
}



Tree createNode(Coordinate move, Case player, Tree parent) {
    Tree newNode = malloc(sizeof(struct node));
    newNode->move = move;
    newNode->playerTurn = player;

    newNode->nbTest = 0;
    newNode->score = 0.0;

    newNode->parent = parent;
    newNode->children = NULL;
    newNode->numChildren = 0;
    newNode->maxChildren = 0;
    
    newNode->untriedMoves = initPossibilities();
    return newNode;
}

void fillUntriedMoves(Tree node, Grid grid) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == empty) {
                Coordinate co = {i, j};
                addPossibility(&(node->untriedMoves), co);
            }
        }
    }
}

Tree expand(Tree node, Grid grid) {
    if (node->untriedMoves == NULL) return NULL;

    Coordinate move = node->untriedMoves->info;
    Case nextPlayer = (node->playerTurn == Bot) ? Player : Bot;
    Tree child = createNode(move, nextPlayer, node);
    if (node->children == NULL) {
        node->maxChildren = 10;
        node->children = malloc(node->maxChildren * sizeof(struct node*));
    }
    
    if (node->numChildren == node->maxChildren) {
        node->maxChildren *= 2;
        node->children = realloc(node->children, node->maxChildren * sizeof(struct node*));
    }

    node->children[node->numChildren] = child;
    node->numChildren++;

    removePossibility(&(node->untriedMoves), move);

    return child;
}

void copyGrid(Grid src, Grid dest) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

// Fonction qui simule une partie à partir d'une position
Case simulate(Grid grid, Case nextToPlay) {
    Grid tempGrid;
    copyGrid(grid, tempGrid);
    
    Case winner = isWinner(tempGrid);
    Case currentPlayer = nextToPlay;
    int movesCount = 0;
    int maxMoves = SIZE * SIZE;

    while (winner == empty && movesCount < maxMoves) {
        int r = randint(0, SIZE - 1);
        int c = randint(0, SIZE - 1);

        if (tempGrid[r][c] == empty) {
            Coordinate move = {r, c};
            placePawn(tempGrid, currentPlayer, move);
            
            winner = isWinner(tempGrid);
            currentPlayer = (currentPlayer == Bot) ? Player : Bot;
        }
        movesCount++; 
    }
    return winner;
}

//Fonction qui sert à remonter dans l'arbre après une simulation
void backpropagate(Tree node, Case winner) {
    Tree current = node;
    while (current != NULL) {
        current->nbTest++;
        
        if (winner == Bot) {
            current->score += 1.0;
        } else if (winner == empty) {
            current->score += 0.5;
        }
        current = current->parent;
    }
}

// algo UCB
Tree selectBestChildUCB(Tree node, float explorationParameter) {
    Tree bestChild = NULL;
    float bestScore = -1.0;

    for (int i = 0; i < node->numChildren; i++) {
        Tree child = node->children[i];
        
        float exploitation = child->score / (float)child->nbTest;
        float exploration = explorationParameter * sqrt(log((float)node->nbTest) / (float)child->nbTest);
        float ucbValue = exploitation + exploration;

        if (ucbValue > bestScore) {
            bestScore = ucbValue;
            bestChild = child;
        }
    }
    return bestChild;
}

// Fonction qui renvoi le meilleure coup à partir d'un arbre representant une position
// le meilleur coup est celui le plus visité par UCB
Coordinate getBestMove(Tree root) {
    int maxVisits = -1;
    Coordinate bestMove = {-1, -1};

    for (int i = 0; i < root->numChildren; i++) {
        if (root->children[i]->nbTest > maxVisits) {
            maxVisits = root->children[i]->nbTest;
            bestMove = root->children[i]->move;
        }
    }
    return bestMove;
}

void freeTree(Tree node) {
    if (node == NULL) return;
    for (int i = 0; i < node->numChildren; i++) {
        freeTree(node->children[i]);
    }
    free(node->children);
    Possibilities p = node->untriedMoves;
    while (p != NULL) {
        struct location *temp = p;
        p = p->suiv;
        free(temp);
    }
    free(node);
}

//Bouvle princiaple lors du tour de l'ia
Coordinate mcts_ai_turn(Grid grid, int iterations) {
    // La racine représente l'état actuel après le coup du joueur
    Tree root = createNode((Coordinate){-1, -1}, Player, NULL);
    fillUntriedMoves(root, grid);

    for (int i = 0; i < iterations; i++) {
        Tree current = root;

        while (current->untriedMoves == NULL && current->numChildren > 0) {
            current = selectBestChildUCB(current, 1.41);
            placePawn(grid, current->playerTurn, current->move);
        }

        Case winner = isWinner(grid);
        if (winner == empty && current->untriedMoves != NULL) {
            Tree child = expand(current, grid);
            current = child;
            placePawn(grid, current->playerTurn, current->move);
        }

        Case nextToPlay;
        if(current->playerTurn == Bot){
            nextToPlay = Player;
        }else{
            nextToPlay = Bot;
        }
        winner = simulate(grid, nextToPlay);

        while (current != NULL) {
            backpropagate(current, winner);
            if (current->parent != NULL) {
                removePawn(grid, current->move);
            }
            current = current->parent;
        }
    }

    Coordinate finalMove = getBestMove(root);
    freeTree(root); 
    return finalMove;
}

int main() {
    Grid g;
    Case winner = empty;
    int moves = 0;
    int maxMoves = SIZE * SIZE;
    Coordinate coord;

    srand(time(NULL));
    initGrille(g);

    printf("------------------------------------\n");
    printf("Vous jouez les X, le Bot joue les O.\n");

    while (winner == empty && moves < maxMoves) {
        displayGrid(g);
        int r, c;
        do {
            printf("A vous (ligne colonne) : ");
            scanf("%d %d", &r, &c);
        } while (r < 0 || r >= SIZE || c < 0 || c >= SIZE || g[r][c] != empty);

        coord.ligne = r;
        coord.colonne = c;
        placePawn(g, Player, coord);
        moves++;

        winner = isWinner(g);

        if (winner == empty && moves < maxMoves){
            printf("Le Bot reflechit...\n");
            coord = mcts_ai_turn(g, 100000);
            printf("Le Bot joue en : %d %d\n", coord.ligne, coord.colonne);
            placePawn(g, Bot, coord);
            moves++;

            winner = isWinner(g);
        }

    }

    displayGrid(g);

    if (winner == Player) printf("FELICITATIONS ! Vous avez battu l'IA.\n");
    else if (winner == Bot) printf("DOMMAGE... L'IA a gagne.\n");
    else printf("MATCH NUL ! Le plateau est plein.\n");

    return 0;
}