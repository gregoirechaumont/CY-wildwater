// main.c - Point d'entrée
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "arbre.h"
#include "fichier.h"
#include "usine.h"
#include "fuites.h"

// version courte 
int main(int argc, char* argv[]) {
    // Vérifier arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <mode> <parametre>\n", argv[0]);
        return 1;
    }
    
    char* mode = argv[1];        // "histo" ou "leaks"
    char* param = argv[2];       // "max", "src", "real" ou ID usine
    
    if (strcmp(mode, "histo") == 0) {
        // Traiter histogramme
        return traiterHistogramme(param);
    } 
    else if (strcmp(mode, "leaks") == 0) {
        // Traiter fuites
        return traiterFuites(param);
    }
    else {
        fprintf(stderr, "Erreur: mode inconnu '%s'\n", mode);
        return 1;
    }
    
    return 0;
}

//version longue
#define CODE_SUCCES 0
#define CODE_ERREUR_ARGUMENTS 1
#define CODE_ERREUR_MODE 2
#define CODE_ERREUR_TRAITEMENT 3

// ============================================================================
// PROTOTYPES
// ============================================================================

void afficherUsage(const char* nomProgramme);
int validerArguments(int argc, char* argv[]);

// ============================================================================
// FONCTION PRINCIPALE
// ============================================================================

/**
 * Point d'entrée du programme
 * 
 * Usage :
 *   ./c-wire histo max|src|real
 *   ./c-wire leaks "Facility complex #..."
 * 
 * @param argc : Nombre d'arguments
 * @param argv : Tableau d'arguments
 * @return 0 si succès, code d'erreur sinon
 */
int main(int argc, char* argv[]) {
    // Vérifier le nombre d'arguments
    if (argc < 3) {
        fprintf(stderr, "Erreur: nombre d'arguments insuffisant\n\n");
        afficherUsage(argv[0]);
        return CODE_ERREUR_ARGUMENTS;
    }
    
    // Récupérer les arguments
    char* mode = argv[1];           // "histo" ou "leaks"
    char* parametre = argv[2];      // "max/src/real" ou ID usine
    
    // Vérifier qu'il n'y a pas d'arguments supplémentaires
    if (argc > 3) {
        fprintf(stderr, "Erreur: trop d'arguments\n\n");
        afficherUsage(argv[0]);
        return CODE_ERREUR_ARGUMENTS;
    }
    
    // ========================================================================
    // TRAITEMENT HISTOGRAMME
    // ========================================================================
    if (strcmp(mode, "histo") == 0) {
        // Vérifier que le paramètre est valide
        if (!estModeValide(parametre)) {
            fprintf(stderr, "Erreur: mode histogramme invalide '%s'\n", parametre);
            fprintf(stderr, "Modes valides: max, src, real\n\n");
            afficherUsage(argv[0]);
            return CODE_ERREUR_MODE;
        }
        
        // Traiter l'histogramme
        int resultat = traiterHistogramme(parametre);
        
        if (resultat != CODE_SUCCES) {
            fprintf(stderr, "Erreur lors du traitement de l'histogramme\n");
            return CODE_ERREUR_TRAITEMENT;
        }
        
        return CODE_SUCCES;
    }
    
    // ========================================================================
    // TRAITEMENT FUITES
    // ========================================================================
    else if (strcmp(mode, "leaks") == 0) {
        // Vérifier que l'ID usine n'est pas vide
        if (strlen(parametre) == 0) {
            fprintf(stderr, "Erreur: identifiant d'usine requis\n\n");
            afficherUsage(argv[0]);
            return CODE_ERREUR_ARGUMENTS;
        }
        
        // Traiter les fuites
        int resultat = traiterFuites(parametre);
        
        if (resultat != CODE_SUCCES) {
            fprintf(stderr, "Erreur lors du calcul des fuites\n");
            return CODE_ERREUR_TRAITEMENT;
        }
        
        return CODE_SUCCES;
    }
    
    // ========================================================================
    // MODE INCONNU
    // ========================================================================
    else {
        fprintf(stderr, "Erreur: mode inconnu '%s'\n", mode);
        fprintf(stderr, "Modes valides: histo, leaks\n\n");
        afficherUsage(argv[0]);
        return CODE_ERREUR_MODE;
    }
    
    return CODE_SUCCES;
}

// ============================================================================
// FONCTIONS UTILITAIRES
// ============================================================================

/**
 * Affiche l'usage du programme
 * @param nomProgramme : Nom de l'exécutable (argv[0])
 */
void afficherUsage(const char* nomProgramme) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║           C-WILDWATER - Système de Distribution d'Eau      ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("USAGE:\n");
    printf("  %s <mode> <parametre>\n", nomProgramme);
    printf("\n");
    printf("MODES DISPONIBLES:\n");
    printf("\n");
    printf("  1. Histogrammes des usines\n");
    printf("     %s histo <type>\n", nomProgramme);
    printf("\n");
    printf("     Types:\n");
    printf("       max   - Capacité maximale de traitement (k.m³/an)\n");
    printf("       src   - Volume total capté depuis les sources (k.m³/an)\n");
    printf("       real  - Volume réellement traité (k.m³/an)\n");
    printf("\n");
    printf("  2. Calcul des fuites pour une usine\n");
    printf("     %s leaks \"<ID_usine>\"\n", nomProgramme);
    printf("\n");
    printf("     ID_usine: Identifiant complet de l'usine (avec guillemets si espaces)\n");
    printf("\n");
    printf("EXEMPLES:\n");
    printf("  %s histo max\n", nomProgramme);
    printf("  %s histo src\n", nomProgramme);
    printf("  %s leaks \"Facility complex #RH400057F\"\n", nomProgramme);
    printf("\n");
    printf("FICHIERS DE SORTIE:\n");
    printf("  - Histogrammes: output/vol_<type>.dat\n");
    printf("  - Fuites:       output/rendement.dat\n");
    printf("\n");
    printf("CODES DE RETOUR:\n");
    printf("  0 - Succès\n");
    printf("  1 - Erreur d'arguments\n");
    printf("  2 - Mode invalide\n");
    printf("  3 - Erreur de traitement\n");
    printf("\n");
}

/**
 * Valide les arguments (fonction auxiliaire non utilisée mais utile)
 * @param argc : Nombre d'arguments
 * @param argv : Tableau d'arguments
 * @return 1 si valides, 0 sinon
 */
int validerArguments(int argc, char* argv[]) {
    // Vérifier le nombre d'arguments
    if (argc != 3) {
        return 0;
    }
    
    // Vérifier que les arguments ne sont pas NULL
    if (argv[1] == NULL || argv[2] == NULL) {
        return 0;
    }
    
    // Vérifier que les arguments ne sont pas vides
    if (strlen(argv[1]) == 0 || strlen(argv[2]) == 0) {
        return 0;
    }
    
    return 1;
}
