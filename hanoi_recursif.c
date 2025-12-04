// Programme des Tours de Hanoi - Version Recursive
#include <stdio.h>
#include <time.h>
#include <math.h>

// Compte le nombre de deplacements
int compteur_deplacements = 0;

// Fonction de deplacement par defaut (peut etre surchargee)
void (*deplacement_ptr)(int, char, char) = NULL;

// Affiche un deplacement et compte
void deplacement(int n, char source, char destination) {
    compteur_deplacements++;
    if (deplacement_ptr != NULL) {
        deplacement_ptr(n, source, destination);
    } else {
        printf("Deplacement %d : Disque %d de %c vers %c\n", 
               compteur_deplacements, n, source, destination);
    }
}

// Fonction recursive pour les Tours de Hanoï
void hanoi(int n, char source, char destination, char auxiliaire) {
    if (n > 0) {
        hanoi(n - 1, source, auxiliaire, destination);
        deplacement(n, source, destination);
        hanoi(n - 1, auxiliaire, destination, source);
    }
}

int main() {
    int n;
    printf("Entrez le nombre de disques : ");
    scanf("%d", &n);
    
    // Réinitialisation du compteur
    compteur_deplacements = 0;
    
    clock_t debut = clock();
    hanoi(n, 'A', 'C', 'B');
    clock_t fin = clock();
    
    double temps = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n=== RESUME ===\n");
    printf("Deplacements : %d\n", compteur_deplacements);
    printf("Temps : %.6f s\n", temps);
    printf("Theorique (2^%d - 1) : %.0f\n", n, pow(2, n) - 1);
    
    return 0;
}
