#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h> 

// Définition des types de nœuds
typedef enum {
    HUB,
    RELAIS,
    CLIENT
} NodeType;

// Définition des types de routes
typedef enum {
    ROUTE_NORMALE,
    ROUTE_EXPRESSE,
    ROUTE_PIETONNE
} RoadType;

// Définition du type de connexion
typedef enum {
    UNDIRECTED, // Arête non orientée
    DIRECTED    // Arc orienté
} ConnectionType;




// Structure pour représenter les attributs dynamiques (variations) d'une arête
typedef struct DynamicAttr {
    char* periode;          // Période de la variation (ex: "matin", "soir")
    float coefficient_temps; // Coefficient multiplicateur pour le temps de trajet
    struct DynamicAttr* next;
} DynamicAttr;

// Structure pour représenter une arête (connexion entre deux nœuds)
typedef struct Edge {
    int dest_id;            // ID du nœud de destination
    bool actif;             // Indique si l'arête est active
    float distance;         // Distance de l'arête (en km)
    float temps_base;       // Temps de trajet de base (en minutes)
    float cout;             // Coût de l'arête (en XOF)
    RoadType type_route;    // Type de route
    float fiabilite;        // Fiabilité de l'arête (pourcentage ou autre échelle)
    int restrictions;       // Éventuelles restrictions (ex: poids max, véhicules autorisés)
    DynamicAttr* variations; // Liste des variations dynamiques
    struct Edge* next;      // Pointeur vers l'arête suivante pour le même nœud source
    ConnectionType type_connexion; // Type de connexion (orientée ou non)
} Edge;

// Structure pour représenter un nœud (sommet) du graphe
typedef struct Vertex {
    int id;                 // Identifiant unique du nœud
    NodeType type;          // Type de nœud (HUB, RELAIS, CLIENT)
    char nom[50];           // Nom du nœud
    float capacite;         // Capacité du nœud (si applicable)
    int ouverture;         // Heure d'ouverture (si applicable)
    int fermeture;        // Heure de fermeture (si applicable)
    float coord[2];         // Coordonnées [latitude, longitude]
    Edge* edges;            // Liste des arêtes sortantes de ce nœud
} Vertex;

// Structure principale pour représenter le graphe
typedef struct Graph {
    int V;              // Nombre total de nœuds dans le graphe
    Vertex* vertices;   // Tableau des nœuds du graphe
} Graph;

// Forward declaration of Chromosome
struct Chromosome;

void initialize_population(struct Chromosome* population[], int population_size, int num_nodes);
void calculate_fitness(struct Chromosome* chromosome, struct Graph* graph);
void crossover(const struct Chromosome* parent1, const struct Chromosome* parent2, struct Chromosome* child, int num_nodes);
void mutate(struct Chromosome* chromosome, float mutation_rate, int num_nodes);
struct Chromosome* select_parent(struct Chromosome* population[], int population_size);
struct Chromosome* genetic_algorithm(struct Graph* graph, int start_node, int num_generations, int population_size, float mutation_rate);
void free_chromosome(struct Chromosome* chromosome);

// Prototypes des fonctions
Graph* creerGraphe(int V);
void ajouterVertex(Graph* graphe, int id, NodeType type, const char* nom,
                    float capacite, int ouverture, int fermeture,
                    float lat, float lon);
void ajouterEdge(Graph* graphe, int src_id, int dest_id, bool actif, float distance,
                    float temps_base, float cout, RoadType type,
                    float fiabilite, int restrictions, ConnectionType type_connexion);
void ajouterVariation(Edge* edge, const char* periode, float coefficient);
void afficherGraphe(Graph* graphe);
void libererGraphe(Graph* graphe);

// Fonction pour trouver l'ID d'un sommet à partir de son nom
int trouverIdSommetParNom(Graph* graphe, const char* nomSommet);

// Déclaration de la fonction getCoefficientTemps
float getCoefficientTemps(Edge* edge);

// Prototypes pour DFS et BFS
void DFS(Graph* graphe, int startVertex, bool* visited);
void BFS(Graph* graphe, int startVertex, bool* visited);

// Fonctions d'application des parcours
bool detecterCycle(Graph* graphe);
void trouverComposantesConnexes(Graph* graphe);
void trouverPointsArticulation(Graph* graphe);
bool estAccessible(Graph* graphe, int depart, int arrivee);
float calculerDegreMoyen(Graph* graphe);
float calculerDensite(Graph* graphe);

// Dans graph.h, ajouter :
Graph* creerReseauLogistique(void);

int* find_connected_components(Graph* graph);


#endif // GRAPH_H