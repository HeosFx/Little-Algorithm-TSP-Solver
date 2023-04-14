/**
 * Projec : gtsp (voyageur de commerce)
 *
 * Date   : 07/04/2014
 * Author : Olivier Grunder
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NBR_TOWNS 10
#define INF 99999999999999

/* Distance matrix */
double dist[NBR_TOWNS][NBR_TOWNS];

/* Each edge has a starting and ending node */
int starting_town[NBR_TOWNS];
int ending_town[NBR_TOWNS];

/* no comment */
int best_solution[NBR_TOWNS];
double best_eval = -1.0;


/**
 * Berlin52 :
 *  6 towns : Best solution (2315.15): 0 1 2 3 5 4
 * 10 towns : Best solution (2826.50): 0 1 6 2 7 8 9 3 5 4
 */
float coord[NBR_TOWNS][2] =
        {
                {565.0,  575.0},
                {25.0,   185.0},
                {345.0,  750.0},
                {945.0,  685.0},
                {845.0,  655.0},
                {880.0,  660.0},
                {25.0,   230.0},
                {525.0,  1000.0},
                {580.0,  1175.0},
                {650.0,  1130.0}
        };


/**
 * print a matrix
 */
void print_matrix(double d[NBR_TOWNS][NBR_TOWNS]) {
    int i, j;
    for (i = 0; i < NBR_TOWNS; i++) {
        printf("%d:", i);
        for (j = 0; j < NBR_TOWNS; j++) {
            printf("%6.1f ", d[i][j]);
        }
        printf("\n");
    }
}


/**
 * print a solution
 */
void print_solution(int *sol, double eval) {
    int i;
    printf("(%.2f): ", eval);
    for (i = 0; i < NBR_TOWNS; i++)
        printf("%d ", sol[i]);
    printf("\n");
}


/**
 * evaluation of a solution
 */
double evaluation_solution(int *sol) {
    double eval = 0;
    int i;
    for (i = 0; i < NBR_TOWNS - 1; i++) {
        eval += dist[sol[i]][sol[i + 1]];
    }
    eval += dist[sol[NBR_TOWNS - 1]][sol[0]];

    return eval;

}


/**
 * nearest neighbour solution
 */
double build_nearest_neighbour() {
    /* solution of the nearest neighbour */
    int sol[NBR_TOWNS];

    /* evaluation of the solution */
    double eval = 0;
    int isUsed;

    sol[0] = 0;
    /*  Build an heuristic solution : the nearest neighbour */
    for (int i = 1; i < NBR_TOWNS; ++i) { // Loop to select the N-1 other cities' rank in the solution
        double min_dist = INF; // Set the minimum distance to +inf
        for (int j = 0; j < NBR_TOWNS; ++j) { // Find the nearest city for each step
            isUsed = 0; // Check if the city is already used
            for (int k = 0; k < i; ++k) {
                if (sol[k] == j) isUsed = 1;
            }
            if (!isUsed) { // If not -> check if the city is the nearest
                if (dist[sol[i - 1]][j] >= 0 && dist[sol[i - 1]][j] < min_dist) {
                    min_dist = dist[sol[i - 1]][j];
                    sol[i] = j;
                }
            }
        }
    }

    eval = evaluation_solution(sol);
    printf("Nearest neighbour ");
    print_solution(sol, eval);
    for (int i = 0; i < NBR_TOWNS; i++) best_solution[i] = sol[i];
    best_eval = eval;

    return eval;
}


/**
 *  Build final solution
 */
void build_solution() {
    int i, solution[NBR_TOWNS];

    int indiceCour = 0;
    int villeCour = 0;

    while (indiceCour < NBR_TOWNS) {

        solution[indiceCour] = villeCour;

        // Test si le cycle est hamiltonien
        for (i = 0; i < indiceCour; i++) {
            if (solution[i] == villeCour) {
                /* printf ("cycle non hamiltonien\n") ; */
                return;
            }
        }
        // Recherche de la ville suivante
        int trouve = 0;
        i = 0;
        while ((!trouve) && (i < NBR_TOWNS)) {
            if (starting_town[i] == villeCour) {
                trouve = 1;
                villeCour = ending_town[i];
            }
            i++;
        }
        indiceCour++;
    }

    double eval = evaluation_solution(solution);

    if (best_eval < 0 || eval < best_eval) {
        best_eval = eval;
        for (i = 0; i < NBR_TOWNS; i++)
            best_solution[i] = solution[i];
        printf("New best solution: ");
        print_solution(solution, best_eval);
    }
    return;
}

void min_col_dbl(double matrix[NBR_TOWNS][NBR_TOWNS], double *min) {
    for (int i = 0; i < NBR_TOWNS; ++i) {
        for (int j = 0; j < NBR_TOWNS; ++j) {
            if (matrix[j][i] < min[i])
                min[i] = matrix[j][i];
        }
    }
}

void min_row_dbl(double matrix[NBR_TOWNS][NBR_TOWNS], double *min) {
    for (int i = 0; i < NBR_TOWNS; ++i) {
        for (int j = 0; j < NBR_TOWNS; ++j) {
            if (matrix[i][j] < min[i])
                min[i] = matrix[i][j];
        }
    }
}

/**
 *  Little Algorithm
 */
void little_algorithm(double d0[NBR_TOWNS][NBR_TOWNS], int iteration, double eval_node_parent) {

    if (iteration == NBR_TOWNS) {
        build_solution();
        return;
    }

    /* Do the modification on a copy of the distance matrix */
    double d[NBR_TOWNS][NBR_TOWNS];
    memcpy(d, d0, NBR_TOWNS * NBR_TOWNS * sizeof(double));

    double eval_node_child = eval_node_parent;

    /**
     * substract the min of the rows and the min of the columns
     * and update the evaluation of the current node
     */
    double changes = 0;

    /* Check that there is at least one 0 in each row */
    for (int i = 0; i < NBR_TOWNS; i++) {
        double min = INF;
        for (int j = 0; j < NBR_TOWNS; j++) {
            if (d[i][j] < min) {
                if (d[i][j] != -1)
                    min = d[i][j];
            }
        }
        for (int j = 0; j < NBR_TOWNS; j++) {
            if (d[i][j] != -1)
                d[i][j] -= min;
        }
        if (min != INF)
            eval_node_child += min;
    }

    /* Check that there is at least one 0 in each column */
    for (int j = 0; j < NBR_TOWNS; j++) {
        double min = INFINITY;
        for (int i = 0; i < NBR_TOWNS; i++) {
            if (d[i][j] < min) {
                if (d[i][j] != -1)
                    min = d[i][j];
            }
        }
        for (int i = 0; i < NBR_TOWNS; i++) {
            if (d[i][j] != -1)
                d[i][j] -= min;
        }
        if (min != INFINITY)
            eval_node_child += min;
    }

    /* Cut : stop the exploration of this node */
    if (best_eval >= 0 && eval_node_child >= best_eval)
        return;

    /**
     *  Compute the penalities to identify the zero with max penalty
     *  If no zero in the matrix, then return, solution infeasible
     */



    /*
    min_row_dbl(d, min_row);
    min_col_dbl(d, min_col);

    double penalties[NBR_TOWNS][NBR_TOWNS];
    for (int i = 0; i < NBR_TOWNS; ++i) {
        for (int j = 0; j < NBR_TOWNS; ++j) {
            if (d[i][j] == 0) {
                if (min_row[i] + min_col[j] < 0) {
                    penalties[i][j] = INF;
                } else {
                    penalties[i][j] = min_row[i] + min_col[j];
                }
            } else {
                penalties[i][j] = -INF; // If the matrix value isn't 0 then penalty is -INF
            }
        }
    }

     */

    /* row and column of the zero with the max penalty */
    int izero = -1, jzero = -1;
    double max_penalty = 0;

    /**
     *  Store the row and column of the zero with max penalty in
     *  starting_town and ending_town
     */


    /*for (int i = 0; i < NBR_TOWNS; ++i) {
        for (int j = 0; j < NBR_TOWNS; ++j) {
            if (penalties[i][j] > max_penalty) {
                izero = i;
                jzero = j;
                max_penalty = penalties[i][j];
            }
        }
    }*/
    for (int i = 0; i < NBR_TOWNS; ++i) {
        for (int j = 0; j < NBR_TOWNS; ++j) {
            /* Check penalty only if it's a 0 */
            if (d[i][j] == 0) {
                double penalty_row = INF, penalty_col = INF;
                double selected_row = 0, selected_col = 0;
                for (int k = 0; k < NBR_TOWNS; ++k) {
                    /* Select the lowest value in the j column */
                    if (k != i && d[k][j] < penalty_row && d[k][j] >= 0) {
                        penalty_row = d[k][j];
                        selected_row = 1;
                    }
                    /* Select the lowest value in the i row */
                    if (k != j && d[i][k] < penalty_row && d[i][k] >= 0) {
                        penalty_col = d[i][k];
                        selected_col = 1;
                    }
                }

                /* Compute the total penalty of the current 0 */
                double penalty_total = penalty_row + penalty_col;

                /* Update the values */
                if (penalty_total > max_penalty && (selected_row || selected_col)) {
                    max_penalty = penalty_total;
                    izero = i;
                    jzero = j;
                } else if (!selected_row && !selected_col) {
                    max_penalty = INF;
                    izero = i;
                    jzero = j;
                }
            }
        }
    }

    /* End if there is no 0 eligible */
    if (izero == -1 || jzero == -1)
        return;

    /* Save the starting and ending town */
    starting_town[iteration] = izero;
    ending_town[iteration] = jzero;

    /* Do the modification on a copy of the distance matrix */
    double d2[NBR_TOWNS][NBR_TOWNS];
    memcpy(d2, d, NBR_TOWNS * NBR_TOWNS * sizeof(double));

    /**
     *  Modify the matrix d2 according to the choice of the zero with the max penalty
     *  Set to -1 both row and column containing the selected 0
     */
    for (int i = 0; i < NBR_TOWNS; ++i) {
        d2[starting_town[iteration]][i] = -1;
        d2[i][ending_town[iteration]] = -1;
    }

    /* Explore left child node according to given choice */
    little_algorithm(d2, iteration + 1, eval_node_child);

    /* Do the modification on a copy of the distance matrix */
    memcpy(d2, d, NBR_TOWNS * NBR_TOWNS * sizeof(double));

    /**
     *  Modify the dist matrix to explore the other possibility : the non-choice
     *  of the zero with the max penalty
     */
    d2[starting_town[iteration]][ending_town[iteration]] = -1;

    /* Explore right child node according to non-choice */
    little_algorithm(d2, iteration, eval_node_child);

}


/**
 *
 */
int main(int argc, char *argv[]) {

    best_eval = -1;

    /* Print problem information */
    printf("Points coordinates:\n");
    for (int i = 0; i < NBR_TOWNS; ++i) {
        printf("Node %d: x=%f, y=%f\n", i, coord[i][0], coord[i][1]);
    }
    printf("\n");


    /* Calcul de la matrice des distances */
    for (int i = 0; i < NBR_TOWNS; ++i) {
        for (int j = 0; j < NBR_TOWNS; ++j) {
            if (i == j) {
                dist[i][j] = -1;
            } else {
                dist[i][j] = sqrt(pow((coord[j][0] - coord[i][0]), 2) + pow((coord[j][1] - coord[i][1]), 2));
            }
        }
    }

    printf("Distance Matrix:\n");
    print_matrix(dist);
    printf("\n");

    /* Nearest Neighbour heuristic : uncomment when needed */
    double nearest_neighbour = build_nearest_neighbour();


    /** Little : uncomment when needed
     */
    int iteration = 0;
    double lowerbound = 0.0;

    little_algorithm(dist, iteration, lowerbound);

    printf("Best solution:");
    print_solution(best_solution, best_eval);


    printf("Hit RETURN!\n");
    getchar();

    return 0;
}
