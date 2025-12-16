/*
 * arbre.c - Implémentation des arbres n-aires
 * Projet C-Wildwater
 * 
 * Gestion des arbres de distribution avec nombre variable d'enfants
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arbre.h"

// ============================================================================
// FONCTIONS UTILITAIRES PRIVÉES
// ============================================================================

static int max(int a, int b) {
    return (a > b) ? a : b;
}

// ============================================================================
// CRÉATION ET DESTRUCTION
// ============================================================================

/**
 * Crée un nouveau nœud
 */
Noeud* creerNoeud(const char* identifiant, char type, float tauxFuite) {
    Noeud* nouveau = (Noeud*)malloc(sizeof(Noeud));
    if (nouveau == NULL) {
        fprintf(stderr, "Erreur: allocation mémoire pour Noeud\n");
        return NULL;
    }
    
    // Copier l'identifiant
    strncpy(nouveau->identifiant, identifiant, sizeof(nouveau->identifiant) - 1);
    nouveau->identifiant[sizeof(nouveau->identifiant) - 1] = '\0';
    
    // Initialiser les champs
    nouveau->type = type;
    nouveau->volumeEntrant = 0.0;
    nouveau->tauxFuite = tauxFuite;
    nouveau->nbEnfants = 0;
    nouveau->capaciteEnfants = CAPACITE_INITIALE_ENFANTS;
    
    // Allouer le tableau d'enfants
    nouveau->enfants = (Noeud**)malloc(CAPACITE_INITIALE_ENFANTS * sizeof(Noeud*));
    if (nouveau->enfants == NULL) {
        fprintf(stderr, "Erreur: allocation mémoire pour enfants\n");
        free(nouveau);
        return NULL;
    }
    
    return nouveau;
}

/**
 * Crée un arbre de distribution vide
 */
ArbreDistribution* creerArbreDistribution() {
    ArbreDistribution* arbre = (ArbreDistribution*)malloc(sizeof(ArbreDistribution));
    if (arbre == NULL) {
        fprintf(stderr, "Erreur: allocation mémoire pour ArbreDistribution\n");
        return NULL;
    }
    
    arbre->racine = NULL;
    return arbre;
}

/**
 * Libère récursivement un nœud et tous ses enfants
 */
void libererNoeud(Noeud* noeud) {
    if (noeud == NULL) {
        return;
    }
    
    // Libérer tous les enfants récursivement
    for (int i = 0; i < noeud->nbEnfants; i++) {
        libererNoeud(noeud->enfants[i]);
    }
    
    // Libérer le tableau d'enfants
    free(noeud->enfants);
    
    // Libérer le nœud lui-même
    free(noeud);
}

/**
 * Libère un arbre de distribution complet
 */
void libererArbre(ArbreDistribution* arbre) {
    if (arbre == NULL) {
        return;
    }
    
    libererNoeud(arbre->racine);
    free(arbre);
}

// ============================================================================
// GESTION DES ENFANTS
// ============================================================================

/**
 * Agrandit la capacité du tableau d'enfants
 */
int agrandirCapaciteEnfants(Noeud* noeud) {
    if (noeud == NULL) {
        return 0;
    }
    
    int nouvelleCapacite = noeud->capaciteEnfants * 2;
    
    Noeud** nouveauTableau = (Noeud**)realloc(noeud->enfants, 
                                               nouvelleCapacite * sizeof(Noeud*));
    if (nouveauTableau == NULL) {
        fprintf(stderr, "Erreur: realloc pour enfants\n");
        return 0;
    }
    
    noeud->enfants = nouveauTableau;
    noeud->capaciteEnfants = nouvelleCapacite;
    
    return 1;
}

/**
 * Ajoute un enfant à un nœud parent
 */
int ajouterEnfant(Noeud* parent, Noeud* enfant) {
    if (parent == NULL || enfant == NULL) {
        return 0;
    }
    
    // Vérifier si on doit agrandir le tableau
    if (parent->nbEnfants >= parent->capaciteEnfants) {
        if (!agrandirCapaciteEnfants(parent)) {
            return 0;
        }
    }
    
    // Ajouter l'enfant
    parent->enfants[parent->nbEnfants] = enfant;
    parent->nbEnfants++;
    
    return 1;
}

// ============================================================================
// CALCUL DES FUITES
// ============================================================================

/**
 * Calcule récursivement les fuites dans tout le sous-arbre
 * 
 * Principe de répartition équitable :
 * - volumeSortant = volumeEntrant * (1 - tauxFuite/100)
 * - Chaque enfant reçoit : volumeSortant / nbEnfants
 */
double calculerFuitesRecursif(Noeud* noeud, double volumeEntrant) {
    if (noeud == NULL) {
        return 0.0;
    }
    
    // Stocker le volume entrant dans le nœud
    noeud->volumeEntrant = volumeEntrant;
    
    // Calculer les fuites de ce tronçon
    double fuitesNoeud = volumeEntrant * (noeud->tauxFuite / 100.0);
    
    // Volume qui sort de ce nœud
    double volumeSortant = volumeEntrant - fuitesNoeud;
    
    // Fuites totales = fuites de ce nœud + fuites des enfants
    double fuitesTotales = fuitesNoeud;
    
    // Si le nœud a des enfants, répartir équitablement
    if (noeud->nbEnfants > 0) {
        double volumeParEnfant = volumeSortant / noeud->nbEnfants;
        
        for (int i = 0; i < noeud->nbEnfants; i++) {
            fuitesTotales += calculerFuitesRecursif(noeud->enfants[i], volumeParEnfant);
        }
    }
    
    return fuitesTotales;
}

/**
 * Trouve le tronçon avec les fuites maximales en valeur absolue
 */
void trouverFuiteMaximale(Noeud* noeud, double volumeEntrant,
                          double* maxFuite, char* idAmont, char* idAval) {
    if (noeud == NULL) {
        return;
    }
    
    // Calculer la fuite de ce tronçon
    double fuiteNoeud = volumeEntrant * (noeud->tauxFuite / 100.0);
    
    // Si c'est la plus grande fuite trouvée jusqu'ici
    if (fuiteNoeud > *maxFuite) {
        *maxFuite = fuiteNoeud;
        // idAmont serait le parent (à gérer avec un paramètre supplémentaire)
        strcpy(idAval, noeud->identifiant);
    }
    
    // Volume sortant
    double volumeSortant = volumeEntrant - fuiteNoeud;
    
    // Continuer récursivement sur les enfants
    if (noeud->nbEnfants > 0) {
        double volumeParEnfant = volumeSortant / noeud->nbEnfants;
        
        for (int i = 0; i < noeud->nbEnfants; i++) {
            trouverFuiteMaximale(noeud->enfants[i], volumeParEnfant,
                                maxFuite, idAmont, idAval);
        }
    }
}

// ============================================================================
// STATISTIQUES ET DEBUG
// ============================================================================

/**
 * Compte le nombre total de nœuds dans l'arbre
 */
int compterNoeudsArbre(Noeud* noeud) {
    if (noeud == NULL) {
        return 0;
    }
    
    int total = 1;  // Ce nœud
    
    for (int i = 0; i < noeud->nbEnfants; i++) {
        total += compterNoeudsArbre(noeud->enfants[i]);
    }
    
    return total;
}

/**
 * Calcule la hauteur de l'arbre
 */
int hauteurArbre(Noeud* noeud) {
    if (noeud == NULL) {
        return 0;
    }
    
    if (noeud->nbEnfants == 0) {
        return 1;
    }
    
    int hauteurMax = 0;
    for (int i = 0; i < noeud->nbEnfants; i++) {
        int h = hauteurArbre(noeud->enfants[i]);
        hauteurMax = max(hauteurMax, h);
    }
    
    return 1 + hauteurMax;
}

/**
 * Affiche l'arbre de manière hiérarchique (pour debug)
 */
void afficherArbre(Noeud* noeud, int niveau) {
    if (noeud == NULL) {
        return;
    }
    
    // Indentation selon le niveau
    for (int i = 0; i < niveau; i++) {
        printf("  ");
    }
    
    // Afficher le nœud
    printf("[%c] %s (%.2f k.m³, fuite: %.2f%%)\n",
           noeud->type,
           noeud->identifiant,
           noeud->volumeEntrant,
           noeud->tauxFuite);
    
    // Afficher les enfants récursivement
    for (int i = 0; i < noeud->nbEnfants; i++) {
        afficherArbre(noeud->enfants[i], niveau + 1);
    }
}

/**
 * Affiche les statistiques de l'arbre de distribution
 */
void afficherStatistiquesArbre(ArbreDistribution* arbre) {
    if (arbre == NULL || arbre->racine == NULL) {
        printf("Arbre vide\n");
        return;
    }
    
    int nbNoeuds = compterNoeudsArbre(arbre->racine);
    int hauteur = hauteurArbre(arbre->racine);
    
    printf("=== Statistiques de l'arbre ===\n");
    printf("Nombre de nœuds : %d\n", nbNoeuds);
    printf("Hauteur : %d\n", hauteur);
    printf("Racine : %s\n", arbre->racine->identifiant);
    printf("===============================\n");
}

// ============================================================================
// UTILITAIRES
// ============================================================================

/**
 * Retourne le nom du type de nœud
 */
const char* obtenirNomType(char type) {
    switch (type) {
        case TYPE_USINE:
            return "Usine";
        case TYPE_STOCKAGE:
            return "Stockage";
        case TYPE_JONCTION:
            return "Jonction";
        case TYPE_RACCORDEMENT:
            return "Raccordement";
        case TYPE_USAGER:
            return "Usager";
        default:
            return "Inconnu";
    }
}
