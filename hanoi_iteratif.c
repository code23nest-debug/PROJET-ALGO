// Programme des Tours de Hanoi - Version Iterative
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Compte les deplacements
int compteur_deplacements = 0;

// Fonction de deplacement par defaut (peut etre surchargee)
void (*deplacement_ptr)(int, char, char) = NULL;

// Structure pour un disque
typedef struct {
    int taille;
    char source;
    char destination;
    char auxiliaire;
} Disque;

// Structure de pile
typedef struct {
    int capacite;
    int sommet;
    Disque *tableau;
} Pile;

// Crée une pile
static Pile* creerPile(int capacite) {
    Pile* pile = (Pile*)malloc(sizeof(Pile));
    if (pile == NULL) {
        printf("Erreur d'allocation memoire\n");
        exit(EXIT_FAILURE);
    }
    pile->capacite = capacite;
    pile->sommet = -1;
    pile->tableau = (Disque*)malloc(capacite * sizeof(Disque));
    if (pile->tableau == NULL) {
        free(pile);
        printf("Erreur d'allocation memoire\n");
        exit(EXIT_FAILURE);
    }
    return pile;
}

// Vérifie si la pile est pleine
static int estPleine(Pile* pile) {
    return pile->sommet == pile->capacite - 1;
}

// Vérifie si la pile est vide
static int estVide(Pile* pile) {
    return pile->sommet == -1;
}

// Ajoute un élément à la pile
static void empiler(Pile* pile, Disque disque) {
    if (estPleine(pile)) {
        printf("Erreur: pile pleine\n");
        return;
    }
    pile->tableau[++pile->sommet] = disque;
}

// Retire et retourne l'élément du sommet
static Disque depiler(Pile* pile) {
    Disque vide = {0};
    if (estVide(pile)) return vide;
    return pile->tableau[pile->sommet--];
}

// Affiche et compte un deplacement
static void afficherDeplacement(int n, char source, char destination) {
    compteur_deplacements++;
    if (deplacement_ptr != NULL) {
        deplacement_ptr(n, source, destination);
    } else {
        printf("Deplacement %d : Disque %d de %c vers %c\n", 
               compteur_deplacements, n, source, destination);
    }
}

// Version itérative de l'algorithme
void hanoiIteratif(int n, char source, char destination, char auxiliaire) {
    if (n <= 0) return;
    
    Pile* pile = creerPile(2 * n);
    Disque disque = {n, source, destination, auxiliaire};
    empiler(pile, disque);
    
    while (!estVide(pile)) {
        Disque courant = depiler(pile);
        
        if (courant.taille == 1) {
            afficherDeplacement(1, courant.source, courant.destination);
        } else {
            // Simule les appels récursifs (ordre inverse car on utilise une pile)
            Disque troisieme = {
                courant.taille - 1,
                courant.auxiliaire,
                courant.destination,
                courant.source
            };
            empiler(pile, troisieme);
            
            Disque deuxieme = {
                1,
                courant.source,
                courant.destination,
                courant.auxiliaire
            };
            empiler(pile, deuxieme);
            
            Disque premier = {
                courant.taille - 1,
                courant.source,
                courant.auxiliaire,
                courant.destination
            };
            empiler(pile, premier);
        }
    }
    
    // Libération de la mémoire
    free(pile->tableau);
    free(pile);
}

int main() {
    int n;
    printf("Entrez le nombre de disques : ");
    if (scanf("%d", &n) != 1 || n < 1) {
        printf("Nombre de disques invalide. Utilisation d'une valeur par defaut (3).\n");
        n = 3;
    }
    
    // Réinitialisation du compteur
    compteur_deplacements = 0;
    
    clock_t debut = clock();
    hanoiIteratif(n, 'A', 'C', 'B');
    clock_t fin = clock();
    
    double temps = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n=== RESUME ===\n");
    printf("Deplacements : %d\n", compteur_deplacements);
    printf("Temps : %.6f s\n", temps);
    printf("Theorique (2^%d - 1) : %.0f\n", n, pow(2, n) - 1);
    
    return 0;
}
