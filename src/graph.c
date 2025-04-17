#include "../include/graph.h"
#include <float.h> // Pour FLT_MAX

Graph* creerGraphe(int V) {
    Graph* graphe = (Graph*)malloc(sizeof(Graph));
    if (!graphe) return NULL;

    graphe->V = V;
    graphe->vertices = (Vertex*)malloc(V * sizeof(Vertex));
    if (!graphe->vertices) {
        free(graphe);
        return NULL;
    }

    for (int i = 0; i < V; i++) {
        graphe->vertices[i].id = -1; // Initialiser à une valeur invalide
        graphe->vertices[i].edges = NULL;
    }

    return graphe;
}

void ajouterVertex(Graph* graphe, int id, NodeType type, const char* nom,
                    float capacite, int ouverture, int fermeture,
                    float lat, float lon) {
    if (!graphe || id < 0 || id >= graphe->V) return;

    strcpy(graphe->vertices[id].nom, nom);
    graphe->vertices[id].id = id;
    graphe->vertices[id].type = type;
    graphe->vertices[id].capacite = capacite;
    graphe->vertices[id].ouverture = ouverture;
    graphe->vertices[id].fermeture = fermeture;
    graphe->vertices[id].coord[0] = lat;
    graphe->vertices[id].coord[1] = lon;
    graphe->vertices[id].edges = NULL;
}

void ajouterEdge(Graph* graphe, int src_id, int dest_id, bool actif, float distance,
                    float temps_base, float cout, RoadType type,
                    float fiabilite, int restrictions, ConnectionType type_connexion) {
    if (!graphe || src_id < 0 || src_id >= graphe->V || dest_id < 0 || dest_id >= graphe->V) return;

    Edge* nouvelleEdge = (Edge*)malloc(sizeof(Edge));
    if (!nouvelleEdge) return;

    nouvelleEdge->dest_id = dest_id;
    nouvelleEdge->actif = actif;
    nouvelleEdge->distance = distance;
    nouvelleEdge->temps_base = temps_base;
    nouvelleEdge->cout = cout;
    nouvelleEdge->type_route = type;
    nouvelleEdge->fiabilite = fiabilite;
    nouvelleEdge->restrictions = restrictions;
    nouvelleEdge->variations = NULL;
    nouvelleEdge->next = graphe->vertices[src_id].edges;
    nouvelleEdge->type_connexion = type_connexion;
    graphe->vertices[src_id].edges = nouvelleEdge;

    // Si le graphe est non orienté, ajouter l'arête dans les deux sens
    if (type_connexion == UNDIRECTED && src_id != dest_id) {
        Edge* nouvelleEdgeRetour = (Edge*)malloc(sizeof(Edge));
        if (!nouvelleEdgeRetour) return;

        nouvelleEdgeRetour->dest_id = src_id;
        nouvelleEdgeRetour->actif = actif;
        nouvelleEdgeRetour->distance = distance;
        nouvelleEdgeRetour->temps_base = temps_base;
        nouvelleEdgeRetour->cout = cout;
        nouvelleEdgeRetour->type_route = type;
        nouvelleEdgeRetour->fiabilite = fiabilite;
        nouvelleEdgeRetour->restrictions = restrictions;
        nouvelleEdgeRetour->variations = NULL;
        nouvelleEdgeRetour->next = graphe->vertices[dest_id].edges;
        nouvelleEdgeRetour->type_connexion = type_connexion; // Important de garder le même type
        graphe->vertices[dest_id].edges = nouvelleEdgeRetour;
    }
}

void ajouterVariation(Edge* edge, const char* periode, float coefficient) {
    if (!edge) return;

    DynamicAttr* nouvelleVariation = (DynamicAttr*)malloc(sizeof(DynamicAttr));
    if (!nouvelleVariation) return;

    nouvelleVariation->periode = strdup(periode);
    nouvelleVariation->coefficient_temps = coefficient;
    nouvelleVariation->next = edge->variations;
    edge->variations = nouvelleVariation;
}

void afficherGraphe(Graph* graphe) {
    if (!graphe) return;

    for (int i = 0; i < graphe->V; i++) {
        printf("[%d] %s (Type: ", i, graphe->vertices[i].nom);
        switch (graphe->vertices[i].type) {
            case HUB: printf("HUB"); break;
            case RELAIS: printf("RELAIS"); break;
            case CLIENT: printf("CLIENT"); break;
        }
        printf(") -> ");

        Edge* courant = graphe->vertices[i].edges;
        while (courant) {
            printf("%d (%.1f km, %.0f min, %.1f XOF, Type: ", courant->dest_id, courant->distance, courant->temps_base, courant->cout);
            switch (courant->type_route) {
                case ROUTE_NORMALE: printf("NORMALE"); break;
                case ROUTE_EXPRESSE: printf("EXPRESSE"); break;
                case ROUTE_PIETONNE: printf("PIETONNE"); break;
            }
            printf(", Connexion: ");
            if (courant->type_connexion == UNDIRECTED) {
                printf("UNDIRECTED");
            } else {
                printf("DIRECTED");
            }
            printf(")");

            if (courant->variations) {
                printf(" (Variations: ");
                DynamicAttr* varCourant = courant->variations;
                while (varCourant) {
                    printf("%s: %.1f ", varCourant->periode, varCourant->coefficient_temps);
                    varCourant = varCourant->next;
                }
                printf(")");
            }

            printf(", ");
            courant = courant->next;
        }
        printf("\n");
    }
}

void libererGraphe(Graph* graphe) {
    if (!graphe) return;

    for (int i = 0; i < graphe->V; i++) {
        Edge* courant = graphe->vertices[i].edges;
        while (courant) {
            Edge* temp = courant;
            courant = courant->next;

            DynamicAttr* varCourant = temp->variations;
            while (varCourant) {
                DynamicAttr* varTemp = varCourant;
                varCourant = varCourant->next;
                free(varTemp->periode);
                free(varTemp);
            }
            free(temp);
        }
    }
    free(graphe->vertices);
    free(graphe);
}

float getCoefficientTemps(Edge* edge) {
    if (!edge || !edge->variations) return 1.0f; // Valeur par défaut si pas de variation

    // Implémentation simplifiée : retourne toujours le premier coefficient
    // Vous devrez adapter cela selon votre logique (heure actuelle, etc.)
    return edge->variations->coefficient_temps;
}

int trouverIdSommetParNom(Graph* graphe, const char* nomSommet) {
    if (!graphe) return -1; // Graphe invalide

    for (int i = 0; i < graphe->V; i++) {
        if (strcmp(graphe->vertices[i].nom, nomSommet) == 0) {
            return graphe->vertices[i].id;
        }
    }
    return -1; // Sommet non trouvé
}

void DFS(Graph* graphe, int startVertex, bool* visited) {
    if (!graphe || startVertex < 0 || startVertex >= graphe->V) return;

    int stack[graphe->V];
    int top = -1;

    for (int i = 0; i < graphe->V; i++) {
        visited[i] = false;
    }

    visited[startVertex] = true;
    stack[++top] = startVertex;

    while (top >= 0) {
        int currentVertex = stack[top--];
        printf("Visité : %d (%s)\n", currentVertex, graphe->vertices[currentVertex].nom);

        Edge* currentEdge = graphe->vertices[currentVertex].edges;
        while (currentEdge) {
            int nextVertex = currentEdge->dest_id;
            if (!visited[nextVertex]) {
                visited[nextVertex] = true;
                stack[++top] = nextVertex;
            }
            currentEdge = currentEdge->next;
        }
    }
}

void BFS(Graph* graphe, int startVertex, bool* visited) {
    if (!graphe || startVertex < 0 || startVertex >= graphe->V) return;

    int queue[graphe->V];
    int front = 0, rear = 0;

    for (int i = 0; i < graphe->V; i++) {
        visited[i] = false;
    }

    visited[startVertex] = true;
    queue[rear++] = startVertex;

    while (front < rear) {
        int currentVertex = queue[front++];
        printf("Visité : %d (%s)\n", currentVertex, graphe->vertices[currentVertex].nom);

        Edge* currentEdge = graphe->vertices[currentVertex].edges;
        while (currentEdge) {
            int nextVertex = currentEdge->dest_id;
            if (!visited[nextVertex]) {
                visited[nextVertex] = true;
                queue[rear++] = nextVertex;
            }
            currentEdge = currentEdge->next;
        }
    }
}

bool DFS_DetecterCycle(Graph* graphe, int sommet, bool* visited, bool* recursionStack) {
    visited[sommet] = true;
    recursionStack[sommet] = true;

    Edge* courant = graphe->vertices[sommet].edges;
    while (courant) {
        int voisin = courant->dest_id;
        if (!visited[voisin]) {
            if (DFS_DetecterCycle(graphe, voisin, visited, recursionStack)) {
                return true;
            }
        } else if (recursionStack[voisin]) {
            return true;
        }
        courant = courant->next;
    }

    recursionStack[sommet] = false;
    return false;
}

bool detecterCycle(Graph* graphe) {
    if (!graphe) return false;

    bool* visited = (bool*)malloc(graphe->V * sizeof(bool));
    bool* recursionStack = (bool*)malloc(graphe->V * sizeof(bool));
    for (int i = 0; i < graphe->V; i++) {
        visited[i] = false;
        recursionStack[i] = false;
    }

    for (int i = 0; i < graphe->V; i++) {
        if (!visited[i]) {
            if (DFS_DetecterCycle(graphe, i, visited, recursionStack)) {
                free(visited);
                free(recursionStack);
                return true;
            }
        }
    }

    free(visited);
    free(recursionStack);
    return false;
}

void DFS_ComposanteConnexe(Graph* graphe, int sommet, bool* visited) {
    visited[sommet] = true;
    printf("%d (%s) ", sommet, graphe->vertices[sommet].nom);

    Edge* courant = graphe->vertices[sommet].edges;
    while (courant) {
        int voisin = courant->dest_id;
        if (!visited[voisin] && courant->type_connexion == UNDIRECTED) {
            DFS_ComposanteConnexe(graphe, voisin, visited);
        }
        courant = courant->next;
    }
}

void trouverComposantesConnexes(Graph* graphe) {
    if (!graphe) return;

    bool* visited = (bool*)malloc(graphe->V * sizeof(bool));
    for (int i = 0; i < graphe->V; i++) {
        visited[i] = false;
    }

    printf("Composantes Connexes :\n");
    for (int i = 0; i < graphe->V; i++) {
        if (!visited[i]) {
            printf("Composante : ");
            DFS_ComposanteConnexe(graphe, i, visited);
            printf("\n");
        }
    }
    free(visited);
}

void DFS_PointsArticulation(Graph* graphe, int sommet, bool* visited, int* disc, int* low, int parent, bool* isArticulation, int* time) {
    visited[sommet] = true;
    disc[sommet] = low[sommet] = ++(*time);
    int enfants = 0;

    if (graphe->vertices[sommet].type == HUB) return;

    Edge* courant = graphe->vertices[sommet].edges;
    while (courant) {
        int voisin = courant->dest_id;
        if (!visited[voisin] && courant->type_connexion == UNDIRECTED) {
            enfants++;
            DFS_PointsArticulation(graphe, voisin, visited, disc, low, sommet, isArticulation, time);
            low[sommet] = fmin(low[sommet], low[voisin]);

            if (parent != -1 && low[voisin] >= disc[sommet]) {
                isArticulation[sommet] = true;
            }
        } else if (voisin != parent && courant->type_connexion == UNDIRECTED) {
            low[sommet] = fmin(low[sommet], disc[voisin]);
        }
        courant = courant->next;
    }

    if (parent == -1 && enfants > 1) {
        isArticulation[sommet] = true;
    }
}

void trouverPointsArticulation(Graph* graphe) {
    if (!graphe) return;

    bool* visited = (bool*)malloc(graphe->V * sizeof(bool));
    int* disc = (int*)malloc(graphe->V * sizeof(int));
    int* low = (int*)malloc(graphe->V * sizeof(int));
    bool* isArticulation = (bool*)malloc(graphe->V * sizeof(bool));
    int time = 0;

    for (int i = 0; i < graphe->V; i++) {
        visited[i] = false;
        isArticulation[i] = false;
    }

    for (int i = 0; i < graphe->V; i++) {
        if (!visited[i]) {
            DFS_PointsArticulation(graphe, i, visited, disc, low, -1, isArticulation, &time);
        }
    }

    printf("Points d'Articulation :\n");
    for (int i = 0; i < graphe->V; i++) {
        if (isArticulation[i]) {
            printf("%d (%s) ", i, graphe->vertices[i].nom);
        }
    }
    printf("\n");

    free(visited);
    free(disc);
    free(low);
    free(isArticulation);
}

bool estAccessible(Graph* graphe, int depart, int arrivee) {
    if (!graphe || depart < 0 || depart >= graphe->V || arrivee < 0 || arrivee >= graphe->V) return false;

    bool* visited = (bool*)malloc(graphe->V * sizeof(bool));
    for (int i = 0; i < graphe->V; i++) {
        visited[i] = false;
    }

    int queue[graphe->V];
    int front = 0, rear = 0;

    visited[depart] = true;
    queue[rear++] = depart;

    while (front < rear) {
        int courant = queue[front++];
        if (courant == arrivee) {
            free(visited);
            return true; // Sommet d'arrivée trouvé
        }

        Edge* courantEdge = graphe->vertices[courant].edges;
        while (courantEdge) {
            int voisin = courantEdge->dest_id;
            if (!visited[voisin]) {
                visited[voisin] = true;
                queue[rear++] = voisin;
            }
            courantEdge = courantEdge->next;
        }
    }

    free(visited);
    return false; // Sommet d'arrivée non trouvé
}

float calculerDegreMoyen(Graph* graphe) {
    if (!graphe || graphe->V == 0) return 0.0f;

    float totalDegre = 0.0f;
    for (int i = 0; i < graphe->V; i++) {
        Edge* courant = graphe->vertices[i].edges;
        while (courant) {
            totalDegre++;
            courant = courant->next;
        }
    }
    if (graphe->vertices[0].edges->type_connexion == UNDIRECTED)
    {
        return totalDegre / graphe->V;
    }
    else
    {return totalDegre / graphe->V;
    }
}

// Corrigez la fonction calculerDensite
float calculerDensite(Graph* graphe) {
    if (!graphe || graphe->V == 0) return 0.0f;

    float nbAretes = 0.0f;
    for (int i = 0; i < graphe->V; i++) {
        Edge* courant = graphe->vertices[i].edges;
        while (courant) {
            nbAretes++;
            courant = courant->next;
        }
    }

    float nbSommets = (float)graphe->V;
    return nbAretes / (nbSommets * (nbSommets - 1.0f)); // Formule unifiée
}

// Déplacez et corrigez la fonction creerReseauLogistique
Graph* creerReseauLogistique(void) {
    // Création d'un réseau de test avec 3 sommets
    Graph* reseau = creerGraphe(3);
    
    // Ajout des sommets
    ajouterVertex(reseau, 0, HUB,    "HubPrincipal", 1000.0, 0, 24, 14.7167, -17.4677);
    ajouterVertex(reseau, 1, RELAIS, "RelaisNord",    200.0, 6, 22, 14.7234, -17.4543);
    ajouterVertex(reseau, 2, CLIENT, "ClientA",        0.0,  8, 18, 14.7281, -17.4629);

    // Ajout des arêtes
    ajouterEdge(reseau, 0, 1, true, 5.2, 15.0, 500.0, ROUTE_EXPRESSE, 0.95, 0, UNDIRECTED);
    ajouterEdge(reseau, 1, 2, true, 2.1, 8.0,  200.0, ROUTE_NORMALE,  0.85, 0, DIRECTED);

    return reseau;
}