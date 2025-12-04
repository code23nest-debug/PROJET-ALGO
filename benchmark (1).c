// Programme de benchmark pour comparer les performances
// des algorithmes recursif et iteratif des Tours de Hanoi
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Variables globales pour le comptage des deplacements
long long compteur_deplacements = 0;

// Structure pour la pile (version iterative)
typedef struct {
    int n;
    char source;
    char destination;
    char auxiliaire;
    int phase;
} Frame;

// Fonction de deplacement silencieuse pour le benchmark
void deplacement_silent(int n, char source, char destination) {
    (void)n; (void)source; (void)destination;
    compteur_deplacements++;
}

// Fonction recursive pour les Tours de Hanoi
void hanoi(int n, char source, char destination, char auxiliaire) {
    if (n > 0) {
        hanoi(n - 1, source, auxiliaire, destination);
        deplacement_silent(n, source, destination);
        hanoi(n - 1, auxiliaire, destination, source);
    }
}

// Fonction iterative pour les Tours de Hanoi
void hanoiIteratif(int n, char source, char destination, char auxiliaire) {
    if (n <= 0) return;
    
    // Taille correcte : 2^n au lieu de n*3
    size_t max_stack_size = (1 << n);
    Frame* stack = (Frame*)malloc(max_stack_size * sizeof(Frame));
    if (stack == NULL) {
        printf("Erreur d'allocation memoire (besoin de %zu frames)\n", max_stack_size);
        return;
    }
    
    int top = 0;
    stack[top].n = n;
    stack[top].source = source;
    stack[top].destination = destination;
    stack[top].auxiliaire = auxiliaire;
    stack[top].phase = 0;
    top++;
    
    while (top > 0) {
        top--;
        Frame current = stack[top];
        
        if (current.n == 0) {
            continue;
        }
        
        if (current.phase == 0) {
            current.phase = 1;
            stack[top] = current;
            top++;
            
            if (current.n - 1 > 0) {
                stack[top].n = current.n - 1;
                stack[top].source = current.source;
                stack[top].destination = current.auxiliaire;
                stack[top].auxiliaire = current.destination;
                stack[top].phase = 0;
                top++;
            }
        } else if (current.phase == 1) {
            deplacement_silent(current.n, current.source, current.destination);
            
            current.phase = 2;
            stack[top] = current;
            top++;
            
            if (current.n - 1 > 0) {
                stack[top].n = current.n - 1;
                stack[top].source = current.auxiliaire;
                stack[top].destination = current.destination;
                stack[top].auxiliaire = current.source;
                stack[top].phase = 0;
                top++;
            }
        }
    }
    
    free(stack);
}

// Fonction pour executer un test et retourner les statistiques
void executer_test(int n, const char* methode, double* avg_time, double* std_dev) {
    const int repetitions = 5;
    double temps[repetitions];
    double temps_total = 0.0;
    double temps_min = 1e9;
    double temps_max = 0.0;
    
    for (int i = 0; i < repetitions; i++) {
        compteur_deplacements = 0;
        
        clock_t debut = clock();
        
        if (strcmp(methode, "recursive") == 0) {
            hanoi(n, 'A', 'C', 'B');
        } else if (strcmp(methode, "iterative") == 0) {
            hanoiIteratif(n, 'A', 'C', 'B');
        }
        
        clock_t fin = clock();
        
        temps[i] = (double)(fin - debut) / CLOCKS_PER_SEC;
        temps_total += temps[i];
        if (temps[i] < temps_min) temps_min = temps[i];
        if (temps[i] > temps_max) temps_max = temps[i];
        
        long long deplacements_attendus = (1LL << n) - 1;
        if (compteur_deplacements != deplacements_attendus) {
            printf("Erreur: Nombre de deplacements incorrect pour n=%d (%lld au lieu de %lld)\n",
                   n, compteur_deplacements, deplacements_attendus);
            *avg_time = -1.0;
            *std_dev = 0.0;
            return;
        }
    }
    
    // Moyenne sans valeurs extremes
    double moyenne;
    int nb_valeurs = repetitions;
    if (repetitions > 2) {
        moyenne = (temps_total - temps_min - temps_max) / (repetitions - 2);
        nb_valeurs -= 2;
    } else {
        moyenne = temps_total / repetitions;
    }
    
    // Ecart-type
    double somme_carres = 0.0;
    for (int i = 0; i < repetitions; i++) {
        if ((repetitions > 2) && 
            ((temps[i] == temps_min) || (temps[i] == temps_max))) {
            continue;
        }
        double ecart = temps[i] - moyenne;
        somme_carres += ecart * ecart;
    }
    double ecart_type = sqrt(somme_carres / nb_valeurs);
    
    *avg_time = moyenne;
    *std_dev = ecart_type;
}

// Structure pour stocker les resultats
typedef struct {
    int n;
    long long mouvements;
    double temps_rec_moy;
    double temps_rec_std;
    double temps_iter_moy;
    double temps_iter_std;
} ResultatLigne;

ResultatLigne resultats[100];
int nb_lignes = 0;

// Fonction pour ajouter ou mettre a jour un resultat
void ajouter_resultat(int n, const char* methode, double temps_moy, double temps_std) {
    long long mouvements = (1LL << n) - 1;
    
    // Chercher si la ligne existe deja
    int index = -1;
    for (int i = 0; i < nb_lignes; i++) {
        if (resultats[i].n == n) {
            index = i;
            break;
        }
    }
    
    // Si pas trouve, creer nouvelle ligne
    if (index == -1) {
        index = nb_lignes;
        resultats[index].n = n;
        resultats[index].mouvements = mouvements;
        resultats[index].temps_rec_moy = 0.0;
        resultats[index].temps_rec_std = 0.0;
        resultats[index].temps_iter_moy = 0.0;
        resultats[index].temps_iter_std = 0.0;
        nb_lignes++;
    }
    
    // Mettre a jour les valeurs
    if (strcmp(methode, "recursive") == 0) {
        resultats[index].temps_rec_moy = temps_moy;
        resultats[index].temps_rec_std = temps_std;
    } else {
        resultats[index].temps_iter_moy = temps_moy;
        resultats[index].temps_iter_std = temps_std;
    }
}

// Fonction pour ecrire le CSV final bien organise
void ecrire_csv_final(const char* nom_fichier) {
    FILE* fichier = fopen(nom_fichier, "w");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }
    
    // Entete
    fprintf(fichier, "n,mouvements,temps_rec_moy,temps_rec_std,temps_iter_moy,temps_iter_std\n");
    
    // Lignes
    for (int i = 0; i < nb_lignes; i++) {
        fprintf(fichier, "%d,%lld,%.9f,%.9f,%.9f,%.9f\n",
                resultats[i].n,
                resultats[i].mouvements,
                resultats[i].temps_rec_moy,
                resultats[i].temps_rec_std,
                resultats[i].temps_iter_moy,
                resultats[i].temps_iter_std);
    }
    
    fclose(fichier);
}

// Fonction pour executer les tests sur une plage de n
void ecrire_resultats(int n_min, int n_max, const char* methode) {
    for (int n = n_min; n <= n_max; n++) {
        printf("Test en cours: n=%d (%s)...\n", n, methode);
        
        double temps_moyen, ecart_type;
        executer_test(n, methode, &temps_moyen, &ecart_type);
        
        if (temps_moyen < 0) {
            printf("Erreur lors de l'execution du test. Arret.\n");
            break;
        }
        
        // Stocker le resultat
        ajouter_resultat(n, methode, temps_moyen, ecart_type);
        
        printf("  Termine: temps = %.6f s (ecart-type = %.6f)\n", 
               temps_moyen, ecart_type);
        
        // Arreter si ca prend trop de temps
        if (temps_moyen > 5.0) {
            printf("Temps d'execution trop long, arret des tests pour cette methode.\n");
            break;
        }
    }
}

int main() {
    const char* fichier_sortie = "hanoi_benchmark.csv";
    
    // Valeurs de n a tester
    int n_min = 10;
    int n_max = 26;
    
    printf("========================================================================\n");
    printf("           BENCHMARK TOURS DE HANOI - RECURSIF VS ITERATIF\n");
    printf("========================================================================\n");
    printf("Config: 5 repetitions par test, moyenne sans min/max\n");
    printf("Fichier: %s\n", fichier_sortie);
    printf("Valeurs de n: %d a %d\n", n_min, n_max);
    printf("========================================================================\n\n");
    
    printf("=== Version recursive ===\n");
    ecrire_resultats(n_min, n_max, "recursive");
    
    compteur_deplacements = 0;
    
    printf("\n=== Version iterative ===\n");
    ecrire_resultats(n_min, n_max, "iterative");
    
    // Ecrire le CSV final bien organise
    printf("\nEcriture du fichier CSV...\n");
    ecrire_csv_final(fichier_sortie);
    
    printf("\n========================================================================\n");
    printf("Benchmark termine!\n");
    printf("Resultats enregistres dans: %s\n", fichier_sortie);
    printf("========================================================================\n");
    
    return 0;
}