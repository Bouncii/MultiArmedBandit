#include "gomoku.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



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

void placePawn(Grid grid,Case pawn,Coordinate co)
{
    grid[co.ligne][co.colonne] = pawn;
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
                // 1. Horizontale
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


Tree createNode(Coordinate move, Possibilities p)
{
    Tree new_node = malloc(sizeof(struct node));
    
    if (new_node != NULL)
    {
        new_node->move = move;
        new_node->visits = 0;
        new_node->score = 0.0;
        new_node->possibles = p;
        new_node->child = NULL;
        new_node->sibling = NULL;
    }
    
    return new_node;
}

void addChild(Tree parent, Tree new_child)
{
    if (parent != NULL && new_child != NULL)
    {
        if (parent->child == NULL)
        {
            parent->child = new_child;
        }
        else
        {
            Tree current = parent->child;
            while (current->sibling != NULL)
            {
                current = current->sibling;
            }
            current->sibling = new_child;
        }
    }
}


Possibilities copyList(Possibilities p)
{
    Possibilities head = NULL;
    Possibilities last = NULL;
    Possibilities current = p;

    while (current != NULL)
    {
        struct location *new_loc = malloc(sizeof(struct location));
        new_loc->info = current->info;
        new_loc->suiv = NULL;

        if (head == NULL) {
            head = new_loc;
        } else {
            last->suiv = new_loc;
        }
        last = new_loc;
        current = current->suiv;
    }
    return head;
}

Tree expand(Tree parent)
{
    Tree new_leaf = NULL;

    if (parent->possibles != NULL)
    {
        Coordinate move_to_try = parent->possibles->info;

        Possibilities child_p = copyList(parent->possibles);
        removePossibility(&child_p, move_to_try);

        new_leaf = createNode(move_to_try, child_p);
        addChild(parent, new_leaf);

        removePossibility(&(parent->possibles), move_to_try);
    }

    return new_leaf;
}