#ifndef ALGORITHMS_OPTIMISATION_APPROCHE_DYNAMIQUE_H_
#define ALGORITHMS_OPTIMISATION_APPROCHE_DYNAMIQUE_H_

#include "../../graph.h"
#include "../../../include/algorithms/optimisation/approche_dynamique.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <time.h>
#include <math.h>

float** floyd_warshall(Graph* graph);
float bellman_ford(Graph* graph, int start, int end, time_t arrival_time, float* total_cost);
void solve_tsp_dynamic_programming(Graph* graph, int start_node);
void plan_multi_day_delivery_dynamic(Graph* graph, int num_days);

// Add these prototypes
float calculate_distance(Graph* graph, int node1, int node2);
float minimum(float a, float b);

#endif
