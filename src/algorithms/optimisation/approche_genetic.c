#include "../../../include/algorithms/optimisation/approche_genetic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <float.h>



// Déclarations anticipées des fonctions internes
static void swap(int *a, int *b);
static int contains(const int* genes, int start, int end, int value);
static int compareChromosomes(const void* a, const void* b);
static Chromosome selectionTournoi(const Chromosome* population);

// Fonction utilitaire pour échanger deux entiers
static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Vérifie la présence d'une valeur dans un segment de gènes
static int contains(const int* genes, int start, int end, int value) {
    for(int i = start; i <= end; i++) {
        if(genes[i] == value) return 1;
    }
    return 0;
}

// Comparateur pour qsort
static int compareChromosomes(const void* a, const void* b) {
    const Chromosome* c1 = (const Chromosome*)a;
    const Chromosome* c2 = (const Chromosome*)b;
    if(c1->fitness > c2->fitness) return -1;
    if(c1->fitness < c2->fitness) return 1;
    return 0;
}

// Sélection par tournoi améliorée
static Chromosome selectionTournoi(const Chromosome* population) {
    const int TOURNAMENT_SIZE = 5;
    Chromosome best = population[rand() % POPULATION_SIZE];
    
    for(int i = 1; i < TOURNAMENT_SIZE; i++) {
        int idx = rand() % POPULATION_SIZE;
        if(population[idx].fitness > best.fitness) {
            best = population[idx];
        }
    }
    return best;
}

// Initialisation de la population
void initialiserPopulation(Graph* graphe, Chromosome* population) {
    srand(time(NULL));
    const int numVertices = graphe->V;
    
    for(int i = 0; i < POPULATION_SIZE; i++) {
        population[i].gene_length = numVertices;
        population[i].genes = (int*)malloc(numVertices * sizeof(int));
        
        // Initialisation linéaire
        for(int j = 0; j < numVertices; j++) {
            population[i].genes[j] = j;
        }
        
        // Mélange de Fisher-Yates
        for(int j = numVertices - 1; j > 0; j--) {
            int k = rand() % (j + 1);
            swap(&population[i].genes[j], &population[i].genes[k]);
        }
    }
}

void evaluerPopulation(Graph* graphe, Chromosome* population) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        float totalDistance = 0.0f; // Déclarer AVANT les boucles internes
        int valid = 1;

        // Calcul de la distance
        for (int j = 0; j < population[i].gene_length; j++) {
            int from = population[i].genes[j];
            int to = population[i].genes[(j+1) % population[i].gene_length];
            
            Edge* edge = graphe->vertices[from].edges;
            while (edge && edge->dest_id != to) edge = edge->next;
            
            if (edge) {
                totalDistance += edge->distance;
            } else {
                valid = 0;
                break; // Sortir si connexion manquante
            }
        }

        population[i].fitness = (valid && totalDistance > 0.0f) 
                               ? (1.0f / totalDistance) 
                               : 0.0f;
    }
}

// Croisement OX1 optimisé
void croisement(const Chromosome parent1, const Chromosome parent2, 
               Chromosome* enfant1, Chromosome* enfant2) {
    const int length = parent1.gene_length;
    int start = rand() % length;
    int end = rand() % length;
    if(start > end) swap(&start, &end);
    
    // Allocation mémoire
    enfant1->genes = (int*)malloc(length * sizeof(int));
    enfant2->genes = (int*)malloc(length * sizeof(int));
    enfant1->gene_length = enfant2->gene_length = length;
    
    // Initialisation à -1
    memset(enfant1->genes, -1, length * sizeof(int));
    memset(enfant2->genes, -1, length * sizeof(int));
    
    // Copie des segments
    for(int i = start; i <= end; i++) {
        enfant1->genes[i] = parent1.genes[i];
        enfant2->genes[i] = parent2.genes[i];
    }
    
    // Remplissage des enfants
    for(int p = 0; p < 2; p++) {
        Chromosome* enfant = (p == 0) ? enfant1 : enfant2;
        const Chromosome* parent = (p == 0) ? &parent2 : &parent1;
        
        int idx = 0;
        for(int i = 0; i < length; i++) {
            const int pos = (end + 1 + i) % length;
            const int gene = parent->genes[pos];
            
            if(!contains(enfant->genes, start, end, gene)) {
                while(idx >= start && idx <= end) idx++;
                enfant->genes[idx++] = gene;
            }
        }
    }
}

// Mutation adaptative
void mutation(Chromosome* chromosome) {
    if((float)rand()/RAND_MAX < MUTATION_RATE) {
        const int mutations = 1 + rand() % 3; // 1-3 mutations
        for(int i = 0; i < mutations; i++) {
            int a = rand() % chromosome->gene_length;
            int b = rand() % chromosome->gene_length;
            swap(&chromosome->genes[a], &chromosome->genes[b]);
        }
    }
}

// Algorithme principal optimisé
void algorithmeGenetique(Graph* graphe) {
    Chromosome population[POPULATION_SIZE];
    Chromosome newPopulation[POPULATION_SIZE];
    
    initialiserPopulation(graphe, population);
    
    for(int gen = 0; gen < MAX_GENERATIONS; gen++) {
        evaluerPopulation(graphe, population);
        
        // Tri par fitness
        qsort(population, POPULATION_SIZE, sizeof(Chromosome), compareChromosomes);
        
        // Élitisme
        newPopulation[0] = population[0];
        
        // Génération de nouvelle population
        for(int i = 1; i < POPULATION_SIZE; i += 2) {
            if((float)rand()/RAND_MAX < CROSSOVER_RATE && (i+1) < POPULATION_SIZE) {
                Chromosome parent1 = selectionTournoi(population);
                Chromosome parent2 = selectionTournoi(population);
                Chromosome enfants[2];
                
                croisement(parent1, parent2, &enfants[0], &enfants[1]);
                mutation(&enfants[0]);
                mutation(&enfants[1]);
                
                newPopulation[i] = enfants[0];
                newPopulation[i+1] = enfants[1];
            }
        }
        
        // Remplacement de population
        memcpy(population, newPopulation, sizeof(population));
        
        // Affichage des statistiques
        if(gen % 100 == 0) {
            printf("Generation %4d | Fitness: %.4f | Distance: %.2f km\n", 
                   gen, 
                   population[0].fitness, 
                   1.0f / population[0].fitness);
        }
    }
    
    // Affichage final
    printf("\nSolution optimale :\n");
    for(int i = 0; i < population[0].gene_length; i++) {
        printf("%s -> ", graphe->vertices[population[0].genes[i]].nom);
    }
    printf("%s\n", graphe->vertices[population[0].genes[0]].nom);
    if (population[0].fitness > 0.0f) {
    printf("\nDistance totale : %.2f km\n", 1.0f / population[0].fitness);
        } else {
    printf("\nAucune solution valide trouvée\n");
            }
    
    // Nettoyage mémoire
    for(int i = 0; i < POPULATION_SIZE; i++) {
        free(population[i].genes);
    }
}