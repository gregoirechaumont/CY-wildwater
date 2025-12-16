  /*
 * usine.c - Implémentation du traitement des histogrammes
 * Projet C-Wildwater
 * 
 * Génère les histogrammes des capacités, volumes captés et traités
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usine.h"
#include "avl.h"
#include "fichier.h"

// ============================================================================
// FONCTION PRINCIPALE
// ============================================================================

/**
 * Traite la commande histogramme
 */
int traiterHistogramme(const char* mode) {
    if (mode == NULL || !estModeValide(mode)) {
        fprintf(stderr, "Erreur: mode invalide\n");
        return 1;
    }
    
    printf("Traitement de l'histogramme mode '%s'...\n", mode);
    
    // Créer un AVL vide
    AVL* avl = creerAVL();
    if (avl == NULL) {
        fprintf(stderr, "Erreur: création AVL\n");
        return 2;
    }
    
    // Lire le CSV et remplir l'AVL
    int resultat = lireEtRemplirAVL(avl, mode);
    if (resultat != 0) {
        fprintf(stderr, "Erreur lors de la lecture du CSV\n");
        libererAVL(avl);
        return 3;
    }
    
    // Vérifier que l'AVL n'est pas vide
    if (!avlNonVide(avl)) {
        fprintf(stderr, "Erreur: aucune donnée trouvée\n");
        libererAVL(avl);
        return 4;
    }
    
    // Écrire le fichier .dat
    resultat = ecrireFichierDat(avl, mode);
    if (resultat != 0) {
        fprintf(stderr, "Erreur lors de l'écriture du fichier\n");
        libererAVL(avl);
        return 5;
    }
    
    // Afficher résumé
    afficherResume(avl, mode);
    
    // Libérer la mémoire
    libererAVL(avl);
    
    printf("Traitement terminé avec succès\n");
    return 0;
}

// ============================================================================
// LECTURE ET REMPLISSAGE AVL
// ============================================================================

/**
 * Lit le fichier CSV et remplit l'AVL
 */
int lireEtRemplirAVL(AVL* avl, const char* mode) {
    if (avl == NULL || mode == NULL) {
        return 1;
    }
    
    FILE* fichier = ouvrirFichierCSV(NULL);
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: ouverture du fichier CSV\n");
        return 2;
    }
    
    LigneCSV ligne;
    int nbLignes = 0;
    int nbTraitees = 0;
    
    // Lire ligne par ligne
    while (lireLigneCSV(fichier, &ligne)) {
        nbLignes++;
        
        // Identifier le type de ligne
        TypeLigneCSV type = identifierTypeLigne(&ligne);
        
        // Traiter selon le type
        switch (type) {
            case TYPE_SOURCE_USINE:
                // Ligne importante : ajoute volumeCapte et volumeTraite
                if (traiterLigneSourceUsine(avl, &ligne, mode) == 0) {
                    nbTraitees++;
                }
                break;
                
            case TYPE_USINE_NOEUD:
                // Ligne importante : définit volumeMax
                if (traiterLigneUsineNoeud(avl, &ligne) == 0) {
                    nbTraitees++;
                }
                break;
                
            default:
                // Autres lignes ignorées pour les histogrammes
                break;
        }
    }
    
    fermerFichierCSV(fichier);
    
    printf("Lignes lues : %d, lignes traitées : %d\n", nbLignes, nbTraitees);
    
    return 0;
}

/**
 * Traite une ligne SOURCE -> USINE
 */
int traiterLigneSourceUsine(AVL* avl, const LigneCSV* ligne, const char* mode) {
    if (avl == NULL || ligne == NULL || mode == NULL) {
        return 1;
    }
    
    // Extraire les données
    char idUsine[TAILLE_CHAMP];
    float volumeCapte, tauxFuite;
    
    if (!extraireIDAval(ligne, idUsine)) {
        return 2;
    }
    
    if (!extraireVolume(ligne, &volumeCapte)) {
        return 3;
    }
    
    if (!extraireTauxFuite(ligne, &tauxFuite)) {
        tauxFuite = 0.0f;  // Par défaut si pas de taux
    }
    
    // Rechercher ou créer le nœud usine dans l'AVL
    NoeudAVL* noeud = rechercherUsine(avl, idUsine);
    if (noeud == NULL) {
        // Créer un nouveau nœud
        if (!insererAVL(avl, idUsine)) {
            return 4;
        }
        noeud = rechercherUsine(avl, idUsine);
        if (noeud == NULL) {
            return 5;
        }
    }
    
    // Ajouter le volume capté
    ajouterVolumeCapte(noeud, volumeCapte);
    
    // Calculer et ajouter le volume traité
    float volumeTraite = calculerVolumeTraite(volumeCapte, tauxFuite);
    ajouterVolumeTraite(noeud, volumeTraite);
    
    return 0;
}

/**
 * Traite une ligne USINE (nœud)
 */
int traiterLigneUsineNoeud(AVL* avl, const LigneCSV* ligne) {
    if (avl == NULL || ligne == NULL) {
        return 1;
    }
    
    // Extraire les données
    char idUsine[TAILLE_CHAMP];
    float volumeMax;
    
    if (!extraireIDAmont(ligne, idUsine)) {
        return 2;
    }
    
    if (!extraireVolume(ligne, &volumeMax)) {
        return 3;
    }
    
    // Rechercher ou créer le nœud usine dans l'AVL
    NoeudAVL* noeud = rechercherUsine(avl, idUsine);
    if (noeud == NULL) {
        // Créer un nouveau nœud
        if (!insererAVL(avl, idUsine)) {
            return 4;
        }
        noeud = rechercherUsine(avl, idUsine);
        if (noeud == NULL) {
            return 5;
        }
    }
    
    // Définir le volume maximum
    definirVolumeMax(noeud, volumeMax);
    
    return 0;
}

// ============================================================================
// ÉCRITURE FICHIER .DAT
// ============================================================================

/**
 * Écrit le fichier .dat avec les données de l'AVL
 */
int ecrireFichierDat(AVL* avl, const char* mode) {
    if (avl == NULL || mode == NULL) {
        return 1;
    }
    
    // Obtenir le nom du fichier de sortie
    const char* nomFichier = obtenirNomFichierSortie(mode);
    
    // Ouvrir le fichier en écriture
    FILE* fichier = fopen(nomFichier, "w");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: ouverture fichier '%s'\n", nomFichier);
        return 2;
    }
    
    // Écrire les données avec l'en-tête approprié
    if (!ecrireAVLDansFichier(avl, fichier, mode)) {
        fprintf(stderr, "Erreur: écriture des données\n");
        fclose(fichier);
        return 3;
    }
    
    fclose(fichier);
    
    printf("Fichier généré : %s\n", nomFichier);
    
    return 0;
}

/**
 * Obtient le nom du fichier de sortie selon le mode
 */
const char* obtenirNomFichierSortie(const char* mode) {
    if (strcmp(mode, MODE_MAX) == 0) {
        return FICHIER_MAX;
    } else if (strcmp(mode, MODE_SRC) == 0) {
        return FICHIER_SRC;
    } else if (strcmp(mode, MODE_REAL) == 0) {
        return FICHIER_REAL;
    }
    
    return "output/inconnu.dat";
}

/**
 * Obtient l'en-tête du fichier selon le mode
 */
const char* obtenirEnteteFichier(const char* mode) {
    if (strcmp(mode, MODE_MAX) == 0) {
        return "identifier;max volume (k.m3.year-1)";
    } else if (strcmp(mode, MODE_SRC) == 0) {
        return "identifier;source volume (k.m3.year-1)";
    } else if (strcmp(mode, MODE_REAL) == 0) {
        return "identifier;real volume (k.m3.year-1)";
    }
    
    return "identifier;volume";
}

// ============================================================================
// VALIDATION
// ============================================================================

/**
 * Vérifie si le mode est valide
 */
int estModeValide(const char* mode) {
    if (mode == NULL) {
        return 0;
    }
    
    if (strcmp(mode, MODE_MAX) == 0 ||
        strcmp(mode, MODE_SRC) == 0 ||
        strcmp(mode, MODE_REAL) == 0) {
        return 1;
    }
    
    return 0;
}

/**
 * Vérifie si l'AVL contient des données
 */
int avlNonVide(AVL* avl) {
    if (avl == NULL || avl->racine == NULL) {
        return 0;
    }
    
    return 1;
}

// ============================================================================
// CALCUL DES VOLUMES
// ============================================================================

/**
 * Calcule le volume traité depuis le volume capté et le taux de fuite
 */
float calculerVolumeTraite(float volumeCapte, float tauxFuite) {
    // Formule : volumeTraité = volumeCapté * (1 - tauxFuite/100)
    
    // Vérifier que le taux est valide
    if (tauxFuite < 0.0f) {
        tauxFuite = 0.0f;
    }
    if (tauxFuite > 100.0f) {
        tauxFuite = 100.0f;
    }
    
    float pourcentageRestant = 1.0f - (tauxFuite / 100.0f);
    return volumeCapte * pourcentageRestant;
}

// ============================================================================
// DEBUG ET AFFICHAGE
// ============================================================================

/**
 * Affiche un résumé du traitement
 */
void afficherResume(AVL* avl, const char* mode) {
    if (avl == NULL || mode == NULL) {
        return;
    }
    
    int nbUsines = compterNoeuds(avl->racine);
    
    printf("\n=== Résumé du traitement ===\n");
    printf("Mode : %s\n", mode);
    printf("Nombre d'usines : %d\n", nbUsines);
    printf("Fichier : %s\n", obtenirNomFichierSortie(mode));
    printf("===========================\n\n");
}
