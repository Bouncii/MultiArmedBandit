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
    Case playerToMove = (node->playerTurn == Bot) ? Player : Bot;
    Coordinate urgent = getUrgentMove(grid, playerToMove);
    
    if (urgent.ligne != -1) {
        addPossibility(&(node->untriedMoves), urgent);
        return; 
    }

    bool isBoardEmpty = true;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == empty) {
                if (hasNeighbor(grid, i, j)) {
                    Coordinate co = {i, j};
                    addPossibility(&(node->untriedMoves), co);
                }
            } else {
                isBoardEmpty = false;
            }
        }
    }
    if (isBoardEmpty) {
        Coordinate center = {SIZE/2, SIZE/2};
        addPossibility(&(node->untriedMoves), center);
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
    
    Coordinate emptyCases[SIZE * SIZE];
    int nbEmpty = 0;
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (tempGrid[i][j] == empty) {
                emptyCases[nbEmpty].ligne = i;
                emptyCases[nbEmpty].colonne = j;
                nbEmpty++;
            }
        }
    }

    for (int i = nbEmpty - 1; i > 0; i--) {
        int j = randint(0, i);
        Coordinate temp = emptyCases[i];
        emptyCases[i] = emptyCases[j];
        emptyCases[j] = temp;
    }

    Case winner = isWinner(tempGrid);
    Case currentPlayer = nextToPlay;
    int index = 0;

    while (winner == empty && index < nbEmpty) {
        Coordinate move = emptyCases[index];
        placePawn(tempGrid, currentPlayer, move);
        
        winner = isWinner(tempGrid);
        currentPlayer = (currentPlayer == Bot) ? Player : Bot;
        
        index++; 
    }
    return winner;
}

//Fonction qui sert à remonter dans l'arbre après une simulation
void backpropagate(Tree node, Case winner) {
    Tree current = node;
    while (current != NULL) {
        current->nbTest++;
        
        if (winner == empty) {
            current->score += 0.5;
        } 
        else if (winner == current->playerTurn) {
            current->score += 1.0; 
        } 
        else {
            if (current->playerTurn == Bot && winner == Player) {
                current->score -= 10000.0;
            } else {
                current->score -= 1.0;
            }
        }
        
        current = current->parent;
    }
}

// algo UCB
Tree selectBestChildUCB(Tree node, float explorationParameter) {
    Tree bestChild = NULL;
    float bestScore = -1000000.0;

    for (int i = 0; i < node->numChildren; i++) {
        Tree child = node->children[i];
        
        float exploitation = child->score / (float)child->nbTest;
        float exploration = explorationParameter * sqrt(log((float)node->nbTest) / (float)child->nbTest);
        float ucbValue = exploitation + exploration;

        if (bestChild == NULL || ucbValue > bestScore) {
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


// Conserve uniquement le nœud correspondant au coup joué et libère tout le reste
Tree keepChildAndFreeRest(Tree root, Coordinate move) {
    if (root == NULL) return NULL;
    Tree nextRoot = NULL;

    for (int i = 0; i < root->numChildren; i++) {
        if (coordinatesEquals(root->children[i]->move, move)) {
            nextRoot = root->children[i];
        } else {
            freeTree(root->children[i]); 
        }
    }
    
    free(root->children);
    Possibilities p = root->untriedMoves;
    while (p != NULL) {
        struct location *temp = p;
        p = p->suiv;
        free(temp);
    }
    free(root);

    if (nextRoot != NULL) {
        nextRoot->parent = NULL;
    }

    return nextRoot;
}

//Bouvle princiaple lors du tour de l'ia
Coordinate mcts_ai_turn(Grid grid, int iterations, Tree root) {
    for (int i = 0; i < iterations; i++) {
        Tree current = root;

        // 1. Sélection (Descente)
        while (current->untriedMoves == NULL && current->numChildren > 0) {
            current = selectBestChildUCB(current, 1.41);
            placePawn(grid, current->playerTurn, current->move);
        }

        Case winner = isWinner(grid);

        if (winner == empty && current->untriedMoves == NULL && current->numChildren == 0) {
            fillUntriedMoves(current, grid);
        }

        // 2. Expansion
        if (winner == empty && current->untriedMoves != NULL) {
            Tree child = expand(current, grid);
            current = child;
            placePawn(grid, current->playerTurn, current->move);
            winner = isWinner(grid);
        }

        // 3. Simulation (Un seul appel suffit !)
        if (winner == empty) {
            Case nextToPlay = (current->playerTurn == Bot) ? Player : Bot;
            winner = simulate(grid, nextToPlay);
        }

        // 4. Rétropropagation et Nettoyage de la grille
        backpropagate(current, winner);

        while (current != NULL) {
            if (current->parent != NULL) {
                removePawn(grid, current->move);
            }
            current = current->parent;
        }
    }

    return getBestMove(root);
}

bool hasNeighbor(Grid grid, int r, int c) {
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            if (i == 0 && j == 0) continue;
            int nr = r + i;
            int nc = c + j;
            if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE) {
                if (grid[nr][nc] != empty) {
                    return true;
                }
            }
        }
    }
    return false;
}

Coordinate getUrgentMove(Grid g, Case playerToMove) {
    Case opponent = (playerToMove == Bot) ? Player : Bot;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (g[i][j] == empty) {
                g[i][j] = playerToMove;
                if (isWinner(g) == playerToMove) {
                    g[i][j] = empty;
                    return (Coordinate){i, j};
                }
                g[i][j] = empty;
            }
        }
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (g[i][j] == empty) {
                g[i][j] = opponent;
                if (isWinner(g) == opponent) {
                    g[i][j] = empty;
                    return (Coordinate){i, j};
                }
                g[i][j] = empty;
            }
        }
    }

    return (Coordinate){-1, -1};
}

int main() {
    Grid g;
    Case winner = empty;
    int moves = 0;
    int maxMoves = SIZE * SIZE;
    Coordinate coord;
    Tree root = NULL;

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

        if (root != NULL) {
            root = keepChildAndFreeRest(root, coord);
        }

        winner = isWinner(g);

        if (winner == empty && moves < maxMoves){
            printf("Le Bot reflechit...\n");

            if (root == NULL) {
                root = createNode((Coordinate){-1, -1}, Player, NULL);
                fillUntriedMoves(root, g);
            }
            coord = mcts_ai_turn(g, 50000, root);

            printf("Le Bot joue en : %d %d\n", coord.ligne, coord.colonne);
            placePawn(g, Bot, coord);
            moves++;

            if (root != NULL) {
                root = keepChildAndFreeRest(root, coord); 
            }

            winner = isWinner(g);
        }

    }

    displayGrid(g);

    if (winner == Player) printf("FELICITATIONS ! Vous avez battu l'IA.\n");
    else if (winner == Bot) printf("DOMMAGE... L'IA a gagne.\n");
    else printf("MATCH NUL ! Le plateau est plein.\n");

    return 0;
}