// #ifndef APPROCHE_GENETIC_H
// #define APPROCHE_GENETIC_H

// #include "../../graph.h" // Inclure graph.h pour utiliser les structures de graphe

// // Définitions pour l'algorithme génétique
// #define POPULATION_SIZE 100
// #define MAX_GENERATIONS 1000
// #define MUTATION_RATE 0.01
// #define CROSSOVER_RATE 0.9

// // Structure pour représenter un chromosome (solution)
// typedef struct {
//     int* genes;          // Séquence de sommets représentant la tournée
//     float fitness;       // Valeur de la fonction de fitness
//     int gene_length;     // Nombre de gènes (sommets) dans le chromosome
// } Chromosome;

// // Prototypes des fonctions pour l'approche génétique
// void initialiserPopulation(Graph* graphe, Chromosome* population);
// void evaluerPopulation(Graph* graphe, Chromosome* population);
// void croisement(Chromosome parent1, Chromosome parent2, Chromosome* enfant1, Chromosome* enfant2);
// void mutation(Chromosome* chromosome);
// void selection(Chromosome* population, Chromosome* nouvellePopulation);
// void algorithmeGenetique(Graph* graphe);

// #endif // APPROCHE_GENETIC_H

#ifndef APPROCHE_GENETIC_H
#define APPROCHE_GENETIC_H

#include "../../graph.h"

// Configuration de l'algorithme
#define POPULATION_SIZE 500
#define MAX_GENERATIONS 5000
#define MUTATION_RATE 0.05
#define CROSSOVER_RATE 0.9

// Structure représentant un chromosome
typedef struct {
    int* genes;          // Séquence de sommets
    float fitness;       // Valeur d'adaptation
    int gene_length;     // Longueur du chromosome
} Chromosome;

// Interface publique principale
void algorithmeGenetique(Graph* graphe);

// Fonctions utilitaires publiques (optionnel)
float calculerFitness(const Chromosome* chromosome, const Graph* graphe);
void afficherSolution(const Chromosome* best, const Graph* graphe);

#endif