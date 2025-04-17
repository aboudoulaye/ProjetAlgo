#include <stdio.h>
#include <stdlib.h>
#include "../include/graph.h"
#include "../include/algorithms/optimisation/approche_genetic.h"
#include "../include/algorithms/optimisation/approche_dynamique.h"
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <float.h>

#define MAX_NOM 50
#define MAX_PATH_LENGTH 20

int main() {
    char nomFichier[100];
    printf("Veuillez entrer le nom du fichier de données du graphe : ");
    if (scanf("%99s", nomFichier) != 1) {
        fprintf(stderr, "Erreur lors de la lecture du nom de fichier.\n");
        return 1;
    }
    while (getchar() != '\n'); // Nettoyer le buffer d'entrée

    FILE* fichier = fopen(nomFichier, "r");
    if (!fichier) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", nomFichier);
        exit(EXIT_FAILURE);
    }

    char ligne[256];
    int nbSommets = 0;
    int sommetsLus = 0;
    int nbArretes = 0;
    int arretesLues = 0;
    Graph* monGraphe = NULL;

    // Lecture du nombre de sommets
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (sscanf(ligne, "%d", &nbSommets) == 1) {
            printf("Nombre de sommets lu : %d\n", nbSommets);
            monGraphe = creerGraphe(nbSommets);
            if (!monGraphe) {
                fprintf(stderr, "Erreur lors de la création du graphe\n");
                fclose(fichier);
                return 1;
            }
        } else {
            fprintf(stderr, "Erreur: Format incorrect pour le nombre de sommets: %s\n", ligne);
            fclose(fichier);
            libererGraphe(monGraphe);
            return 1;
        }
    } else {
        fprintf(stderr, "Erreur: Impossible de lire le nombre de sommets.\n");
        fclose(fichier);
        return 1;
    }

    // Lecture des informations des sommets
    for (int i = 0; i < nbSommets; i++) {
        if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
            if (ligne[0] == '#') {
                i--; // Ignorer les commentaires et relire la prochaine ligne
                continue;
            }
            int id;
            char typeStr[10];
            char nom[MAX_NOM];
            float capacite, lat, lon;
            int ouverture, fermeture;
            if (sscanf(ligne, "%d %9s %49s %f %d %d %f %f", &id, typeStr, nom, &capacite, &ouverture, &fermeture, &lat, &lon) == 8) {
                NodeType type;
                if (strcmp(typeStr, "HUB") == 0) type = HUB;
                else if (strcmp(typeStr, "RELAIS") == 0) type = RELAIS;
                else if (strcmp(typeStr, "CLIENT") == 0) type = CLIENT;
                else {
                    fprintf(stderr, "Type de sommet invalide pour le sommet %d: %s\n", id, typeStr);
                    fclose(fichier);
                    libererGraphe(monGraphe);
                    return 1;
                }
                ajouterVertex(monGraphe, id, type, nom, capacite, ouverture, fermeture, lat, lon);
            } else {
                fprintf(stderr, "Erreur de lecture des informations du sommet %d: %s\n", i, ligne);
                fclose(fichier);
                libererGraphe(monGraphe);
                return 1;
            }
        } else {
            fprintf(stderr, "Erreur: Impossible de lire les informations du sommet %d.\n", i);
            fclose(fichier);
            libererGraphe(monGraphe);
            return 1;
        }
    }

    // Lecture du nombre d'arêtes
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (sscanf(ligne, "%d", &nbArretes) == 1) {
            printf("Nombre d'arêtes lu : %d\n", nbArretes);
            if (nbArretes <= 0) {
                fprintf(stderr, "Erreur: Nombre d'arêtes invalide.\n");
                fclose(fichier);
                libererGraphe(monGraphe);
                return 1;
            }
        } else {
            fprintf(stderr, "Erreur: Format incorrect pour le nombre d'arêtes: %s\n", ligne);
            fclose(fichier);
            libererGraphe(monGraphe);
            return 1;
        }
    } else {
        fprintf(stderr, "Erreur: Impossible de lire le nombre d'arêtes.\n");
        fclose(fichier);
        libererGraphe(monGraphe);
        return 1;
    }

    // Lecture des informations des arêtes
    for (int i = 0; i < nbArretes; i++) {
        if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
            if (ligne[0] == '#') {
                i--; // Ignorer les commentaires et relire la prochaine ligne
                continue;
            }
            int src_id, dest_id, restrictions;
            float distance, temps_base, cout, fiabilite;
            int typeInt;
            int typeConnexionInt; // Nouvelle variable pour le type de connexion
            if (sscanf(ligne, "%d %d %d %f %f %f %d %f %d", &src_id, &dest_id, &restrictions, &distance, &temps_base, &cout, &typeInt, &fiabilite, &typeConnexionInt) == 9) {
                RoadType type = (RoadType)typeInt;
                ConnectionType typeConnexion;
                if (typeConnexionInt == 0) {
                    typeConnexion = UNDIRECTED;
                } else if (typeConnexionInt == 1) {
                    typeConnexion = DIRECTED;
                } else {
                    fprintf(stderr, "Erreur: Type de connexion invalide: %d\n", typeConnexionInt);
                    fclose(fichier);
                    libererGraphe(monGraphe);
                    return 1;
                }
                bool actif = (restrictions >= 0);
                ajouterEdge(monGraphe, src_id, dest_id, actif, distance, temps_base, cout, type, fiabilite, restrictions, typeConnexion);
            } else {
                fprintf(stderr, "Erreur de lecture des informations de l'arête %d: %s\n", i, ligne);
                fclose(fichier);
                libererGraphe(monGraphe);
                return 1;
            }
        } else {
            fprintf(stderr, "Erreur: Impossible de lire les informations de l'arête %d.\n", i);
            fclose(fichier);
            libererGraphe(monGraphe);
            return 1;
        }
    }

    fclose(fichier);

    printf("Affichage du graphe chargé depuis %s:\n", nomFichier);
    afficherGraphe(monGraphe);

    // Exemples d'utilisation des fonctions
    if (monGraphe != NULL) {
        bool* visited = (bool*)malloc(monGraphe->V * sizeof(bool));
        if (!visited) {
            fprintf(stderr, "Erreur d'allocation mémoire pour le tableau visited.\n");
            libererGraphe(monGraphe);
            return 1;
        }

        char nomDepart[MAX_NOM];
        printf("\nVeuillez entrer le nom du sommet de départ pour les parcours : ");
        if (scanf("%49s", nomDepart) != 1) {
            fprintf(stderr, "Erreur lors de la lecture du nom du sommet.\n");
            free(visited);
            libererGraphe(monGraphe);
            return 1;
        }
        while (getchar() != '\n');

        int startVertex = trouverIdSommetParNom(monGraphe, nomDepart);
        if (startVertex == -1) {
            fprintf(stderr, "Erreur : Sommet '%s' non trouvé dans le graphe.\n", nomDepart);
            free(visited);
            libererGraphe(monGraphe);
            return 1;
        }

        printf("\n--- Parcours en profondeur (DFS) depuis le sommet '%s' (ID: %d) ---\n", monGraphe->vertices[startVertex].nom, startVertex);
        DFS(monGraphe, startVertex, visited);

        printf("\n--- Parcours en largeur (BFS) depuis le sommet '%s' (ID: %d) ---\n", monGraphe->vertices[startVertex].nom, startVertex);
        BFS(monGraphe, startVertex, visited);

        free(visited);

        if (detecterCycle(monGraphe)) {
            printf("\nLe graphe contient un cycle.\n");
        } else {
            printf("\nLe graphe ne contient pas de cycle.\n");
        }

        trouverComposantesConnexes(monGraphe);
        trouverPointsArticulation(monGraphe);

        char nomDepartAccessibilite[MAX_NOM], nomArriveeAccessibilite[MAX_NOM];
        printf("\nVeuillez entrer le sommet de départ et d'arrivée pour vérifier l'accessibilité (noms): ");
        if (scanf("%49s %49s", nomDepartAccessibilite, nomArriveeAccessibilite) == 2) {
            int departAccessibilite = trouverIdSommetParNom(monGraphe, nomDepartAccessibilite);
            int arriveeAccessibilite = trouverIdSommetParNom(monGraphe, nomArriveeAccessibilite);
            if (departAccessibilite != -1 && arriveeAccessibilite != -1) {
                if (estAccessible(monGraphe, departAccessibilite, arriveeAccessibilite)) {
                    printf("Le sommet '%s' est accessible depuis le sommet '%s'.\n", nomArriveeAccessibilite, nomDepartAccessibilite);
                } else {
                    printf("Le sommet '%s' n'est pas accessible depuis le sommet '%s'.\n", nomArriveeAccessibilite, nomDepartAccessibilite);
                }
            } else {
                printf("Erreur : Sommet(s) non trouvé(s) pour l'accessibilité.\n");
            }
        } else {
            printf("Erreur de saisie pour l'accessibilité.\n");
        }

        printf("\nDegré moyen du graphe : %.2f\n", calculerDegreMoyen(monGraphe));
        printf("Densité du graphe : %.2f\n", calculerDensite(monGraphe));
    }

     Graph* reseau = creerReseauLogistique();
    algorithmeGenetique(reseau);

     // After loading the graph, calculate Floyd-Warshall
     printf("\n--- Floyd-Warshall Algorithm ---\n");

       float** dist = floyd_warshall(monGraphe);

        if (dist != NULL) {
            printf("Distance Abidjan -> Korhogo: %.2f km\n", dist[0][4]);

            // Libérer la mémoire allouée pour la matrice des distances
            for (int i = 0; i < monGraphe->V; i++) {
                free(dist[i]);
            }
            free(dist);
        } else {
            printf("Erreur dans l'appel de floyd warshall");
        }


    if (monGraphe) libererGraphe(monGraphe);

    return 0;
}