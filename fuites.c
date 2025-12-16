/*
 * fuites.c - Implémentation du calcul des fuites
 * Projet C-Wildwater
 * 
 * Construit l'arbre de distribution et calcule les fuites totales
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fuites.h"
#include "arbre.h"
#include "fichier.h"

// ============================================================================
// UTILITAIRES
// ============================================================================

static int max(int a, int b) {
    return (a > b) ? a : b;
}

static int min(int a, int b) {
    return (a < b) ? a : b;
}

// ============================================================================
// FONCTION PRINCIPALE
// ============================================================================

/**
 * Traite la commande de calcul de fuites
 */
int traiterFuites(const char* idUsine) {
    if (idUsine == NULL || !estIDUsineValide(idUsine)) {
        fprintf(stderr, "Erreur: identifiant d'usine invalide\n");
        return 1;
    }
    
    printf("Calcul des fuites pour l'usine : %s\n", idUsine);
    
    // Construire l'arbre de distribution
    AVLRecherche* avlRecherche = NULL;
    Noeud* racine = construireArbreDistribution(idUsine, &avlRecherche);
    
    if (racine == NULL) {
        fprintf(stderr, "Erreur: usine non trouvée ou arbre non construit\n");
        if (avlRecherche != NULL) {
            libererAVLRecherche(avlRecherche);
        }
        // Écrire -1 dans le fichier pour indiquer usine non trouvée
        ecrireFichierRendement(idUsine, -1.0);
        return 0;  // Pas une erreur du programme, juste usine introuvable
    }
    
    printf("Arbre de distribution construit (%d nœuds)\n", 
           compterNoeudsArbre(racine));
    
    // Calculer les fuites totales
    double fuitesM = calculerFuitesTotales(racine, idUsine);
    
    if (fuitesM < 0) {
        fprintf(stderr, "Erreur lors du calcul des fuites\n");
        libererNoeud(racine);
        libererAVLRecherche(avlRecherche);
        return 2;
    }
    
    // Écrire le résultat dans le fichier
    if (ecrireFichierRendement(idUsine, fuitesM) != 0) {
        fprintf(stderr, "Erreur lors de l'écriture du fichier\n");
        libererNoeud(racine);
        libererAVLRecherche(avlRecherche);
        return 3;
    }
    
    // Afficher résumé
    afficherResumeFuites(idUsine, fuitesM);
    
    // Libérer la mémoire
    libererNoeud(racine);
    libererAVLRecherche(avlRecherche);
    
    printf("Traitement terminé avec succès\n");
    return 0;
}

// ============================================================================
// CONSTRUCTION DE L'ARBRE
// ============================================================================

/**
 * Construit l'arbre de distribution pour une usine donnée
 */
Noeud* construireArbreDistribution(const char* idUsine, AVLRecherche** avlRecherche) {
    if (idUsine == NULL || avlRecherche == NULL) {
        return NULL;
    }
    
    // Créer l'AVL de recherche
    *avlRecherche = creerAVLRecherche();
    if (*avlRecherche == NULL) {
        return NULL;
    }
    
    // Ouvrir le fichier CSV
    FILE* fichier = ouvrirFichierCSV(NULL);
    if (fichier == NULL) {
        return NULL;
    }
    
    Noeud* racine = NULL;
    LigneCSV ligne;
    int nbLignes = 0;
    
    // Lire ligne par ligne
    while (lireLigneCSV(fichier, &ligne)) {
        nbLignes++;
        
        // Identifier le type de ligne
        TypeLigneCSV type = identifierTypeLigne(&ligne);
        
        // Traiter la ligne
        traiterLigneArbre(&ligne, type, idUsine, &racine, *avlRecherche);
    }
    
    fermerFichierCSV(fichier);
    
    printf("Lignes CSV traitées : %d\n", nbLignes);
    
    return racine;
}

/**
 * Traite une ligne du CSV pour construire l'arbre
 */
int traiterLigneArbre(const LigneCSV* ligne, TypeLigneCSV type,
                      const char* idUsine, Noeud** racine,
                      AVLRecherche* avlRecherche) {
    if (ligne == NULL || idUsine == NULL || racine == NULL || avlRecherche == NULL) {
        return -1;
    }
    
    char idCol1[TAILLE_CHAMP];
    char idAmont[TAILLE_CHAMP];
    char idAval[TAILLE_CHAMP];
    float tauxFuite = 0.0f;
    
    // Extraire les données communes
    extraireIDAmont(ligne, idAmont);
    extraireTauxFuite(ligne, &tauxFuite);
    
    switch (type) {
        case TYPE_USINE_STOCKAGE:
            // Vérifier si c'est notre usine
            if (strcmp(idAmont, idUsine) != 0) {
                return 0;  // Pas notre usine, ignorer
            }
            
            // Créer la racine (l'usine)
            if (*racine == NULL) {
                *racine = creerNoeud(idAmont, TYPE_USINE, 0.0f);
                if (*racine == NULL) return -1;
                insererRecherche(avlRecherche, idAmont, *racine);
            }
            
            // Créer le nœud stockage
            if (extraireIDAval(ligne, idAval)) {
                Noeud* stockage = creerNoeud(idAval, TYPE_STOCKAGE, tauxFuite);
                if (stockage == NULL) return -1;
                
                ajouterEnfant(*racine, stockage);
                insererRecherche(avlRecherche, idAval, stockage);
            }
            break;
            
        case TYPE_STOCKAGE_JONCTION:
            // Vérifier si la ligne concerne notre usine
            if (!extraireIDUsine(ligne, idCol1) || strcmp(idCol1, idUsine) != 0) {
                return 0;
            }
            
            // Rechercher le parent (stockage)
            Noeud* parent = rechercherNoeudPublic(avlRecherche, idAmont);
            if (parent == NULL) return 0;
            
            // Créer le nœud jonction
            if (extraireIDAval(ligne, idAval)) {
                Noeud* jonction = creerNoeud(idAval, TYPE_JONCTION, tauxFuite);
                if (jonction == NULL) return -1;
                
                ajouterEnfant(parent, jonction);
                insererRecherche(avlRecherche, idAval, jonction);
            }
            break;
            
        case TYPE_JONCTION_RACCORDEMENT:
            // Vérifier si la ligne concerne notre usine
            if (!extraireIDUsine(ligne, idCol1) || strcmp(idCol1, idUsine) != 0) {
                return 0;
            }
            
            // Rechercher le parent (jonction)
            parent = rechercherNoeudPublic(avlRecherche, idAmont);
            if (parent == NULL) return 0;
            
            // Créer le nœud raccordement
            if (extraireIDAval(ligne, idAval)) {
                Noeud* raccordement = creerNoeud(idAval, TYPE_RACCORDEMENT, tauxFuite);
                if (raccordement == NULL) return -1;
                
                ajouterEnfant(parent, raccordement);
                insererRecherche(avlRecherche, idAval, raccordement);
            }
            break;
            
        case TYPE_RACCORDEMENT_USAGER:
            // Vérifier si la ligne concerne notre usine
            if (!extraireIDUsine(ligne, idCol1) || strcmp(idCol1, idUsine) != 0) {
                return 0;
            }
            
            // Rechercher le parent (raccordement)
            parent = rechercherNoeudPublic(avlRecherche, idAmont);
            if (parent == NULL) return 0;
            
            // Créer le nœud usager
            if (extraireIDAval(ligne, idAval)) {
                Noeud* usager = creerNoeud(idAval, TYPE_USAGER, tauxFuite);
                if (usager == NULL) return -1;
                
                ajouterEnfant(parent, usager);
                insererRecherche(avlRecherche, idAval, usager);
            }
            break;
            
        default:
            return 0;  // Type non géré pour les fuites
    }
    
    return 1;
}

// ============================================================================
// AVL DE RECHERCHE - CRÉATION
// ============================================================================

/**
 * Crée un AVL de recherche vide
 */
AVLRecherche* creerAVLRecherche() {
    AVLRecherche* avl = (AVLRecherche*)malloc(sizeof(AVLRecherche));
    if (avl == NULL) {
        fprintf(stderr, "Erreur: allocation AVLRecherche\n");
        return NULL;
    }
    
    avl->racine = NULL;
    return avl;
}

/**
 * Crée un nœud AVL de recherche
 */
NoeudAVLRecherche* creerNoeudAVLRecherche(const char* identifiant, Noeud* adresse) {
    NoeudAVLRecherche* nouveau = (NoeudAVLRecherche*)malloc(sizeof(NoeudAVLRecherche));
    if (nouveau == NULL) {
        fprintf(stderr, "Erreur: allocation NoeudAVLRecherche\n");
        return NULL;
    }
    
    strncpy(nouveau->identifiant, identifiant, sizeof(nouveau->identifiant) - 1);
    nouveau->identifiant[sizeof(nouveau->identifiant) - 1] = '\0';
    
    nouveau->adresse = adresse;
    nouveau->equilibre = 0;
    nouveau->gauche = NULL;
    nouveau->droit = NULL;
    
    return nouveau;
}

// ============================================================================
// AVL DE RECHERCHE - ROTATIONS
// ============================================================================

/**
 * Rotation gauche pour AVL de recherche
 */
NoeudAVLRecherche* rotationGaucheRecherche(NoeudAVLRecherche* a) {
    if (a == NULL || a->droit == NULL) {
        return a;
    }
    
    NoeudAVLRecherche* pivot = a->droit;
    int eq_a = a->equilibre;
    int eq_p = pivot->equilibre;
    
    a->droit = pivot->gauche;
    pivot->gauche = a;
    
    a->equilibre = eq_a - max(eq_p, 0) - 1;
    pivot->equilibre = min(eq_a - 2, min(eq_a + eq_p - 2, eq_p - 1));
    
    return pivot;
}

/**
 * Rotation droite pour AVL de recherche
 */
NoeudAVLRecherche* rotationDroiteRecherche(NoeudAVLRecherche* a) {
    if (a == NULL || a->gauche == NULL) {
        return a;
    }
    
    NoeudAVLRecherche* pivot = a->gauche;
    int eq_a = a->equilibre;
    int eq_p = pivot->equilibre;
    
    a->gauche = pivot->droit;
    pivot->droit = a;
    
    a->equilibre = eq_a - min(eq_p, 0) + 1;
    pivot->equilibre = max(eq_a + 2, max(eq_a + eq_p + 2, eq_p + 1));
    
    return pivot;
}

/**
 * Double rotation gauche pour AVL de recherche
 */
NoeudAVLRecherche* doubleRotationGaucheRecherche(NoeudAVLRecherche* a) {
    if (a == NULL || a->droit == NULL) {
        return a;
    }
    
    a->droit = rotationDroiteRecherche(a->droit);
    return rotationGaucheRecherche(a);
}

/**
 * Double rotation droite pour AVL de recherche
 */
NoeudAVLRecherche* doubleRotationDroiteRecherche(NoeudAVLRecherche* a) {
    if (a == NULL || a->gauche == NULL) {
        return a;
    }
    
    a->gauche = rotationGaucheRecherche(a->gauche);
    return rotationDroiteRecherche(a);
}

/**
 * Équilibre un nœud AVL de recherche
 */
NoeudAVLRecherche* equilibrerAVLRecherche(NoeudAVLRecherche* a) {
    if (a == NULL) {
        return NULL;
    }
    
    if (a->equilibre >= 2) {
        if (a->droit != NULL && a->droit->equilibre >= 0) {
            return rotationGaucheRecherche(a);
        } else {
            return doubleRotationGaucheRecherche(a);
        }
    } else if (a->equilibre <= -2) {
        if (a->gauche != NULL && a->gauche->equilibre <= 0) {
            return rotationDroiteRecherche(a);
        } else {
            return doubleRotationDroiteRecherche(a);
        }
    }
    
    return a;
}

// ============================================================================
// AVL DE RECHERCHE - INSERTION
// ============================================================================

/**
 * Insère un nœud dans l'AVL de recherche (récursif)
 */
NoeudAVLRecherche* insererRecherche_rec(NoeudAVLRecherche* racine,
                                        const char* identifiant,
                                        Noeud* adresse, int* h) {
    if (racine == NULL) {
        *h = 1;
        return creerNoeudAVLRecherche(identifiant, adresse);
    }
    
    int cmp = strcmp(identifiant, racine->identifiant);
    
    if (cmp < 0) {
        racine->gauche = insererRecherche_rec(racine->gauche, identifiant, adresse, h);
        *h = -*h;
    } else if (cmp > 0) {
        racine->droit = insererRecherche_rec(racine->droit, identifiant, adresse, h);
    } else {
        *h = 0;
        return racine;
    }
    
    if (*h != 0) {
        racine->equilibre += *h;
        racine = equilibrerAVLRecherche(racine);
        
        if (racine->equilibre == 0) {
            *h = 0;
        } else {
            *h = 1;
        }
    }
    
    return racine;
}

/**
 * Insère un nœud dans l'AVL de recherche (fonction publique)
 */
int insererRecherche(AVLRecherche* avl, const char* identifiant, Noeud* adresse) {
    if (avl == NULL || identifiant == NULL || adresse == NULL) {
        return 0;
    }
    
    int h = 0;
    avl->racine = insererRecherche_rec(avl->racine, identifiant, adresse, &h);
    
    return 1;
}

// ============================================================================
// AVL DE RECHERCHE - RECHERCHE
// ============================================================================

/**
 * Recherche un nœud dans l'AVL de recherche
 */
Noeud* rechercherNoeud(NoeudAVLRecherche* racine, const char* identifiant) {
    if (racine == NULL || identifiant == NULL) {
        return NULL;
    }
    
    int cmp = strcmp(identifiant, racine->identifiant);
    
    if (cmp == 0) {
        return racine->adresse;
    } else if (cmp < 0) {
        return rechercherNoeud(racine->gauche, identifiant);
    } else {
        return rechercherNoeud(racine->droit, identifiant);
    }
}

/**
 * Recherche un nœud (version publique)
 */
Noeud* rechercherNoeudPublic(AVLRecherche* avl, const char* identifiant) {
    if (avl == NULL) {
        return NULL;
    }
    
    return rechercherNoeud(avl->racine, identifiant);
}

// ============================================================================
// AVL DE RECHERCHE - LIBÉRATION
// ============================================================================

/**
 * Libère un AVL de recherche (récursif)
 */
void libererAVLRecherche_rec(NoeudAVLRecherche* racine) {
    if (racine == NULL) {
        return;
    }
    
    libererAVLRecherche_rec(racine->gauche);
    libererAVLRecherche_rec(racine->droit);
    free(racine);
}

/**
 * Libère un AVL de recherche complet
 */
void libererAVLRecherche(AVLRecherche* avl) {
    if (avl == NULL) {
        return;
    }
    
    libererAVLRecherche_rec(avl->racine);
    free(avl);
}

// ============================================================================
// CALCUL DES FUITES
// ============================================================================

/**
 * Détermine le volume initial qui entre dans l'usine
 */
double determinerVolumeInitial(const char* idUsine) {
    if (idUsine == NULL) {
        return 0.0;
    }
    
    FILE* fichier = ouvrirFichierCSV(NULL);
    if (fichier == NULL) {
        return 0.0;
    }
    
    double volumeTotal = 0.0;
    LigneCSV ligne;
    
    // Parcourir le CSV pour trouver toutes les sources de cette usine
    while (lireLigneCSV(fichier, &ligne)) {
        TypeLigneCSV type = identifierTypeLigne(&ligne);
        
        if (type == TYPE_SOURCE_USINE) {
            char idAval[TAILLE_CHAMP];
            float volume, taux;
            
            if (extraireIDAval(&ligne, idAval) && strcmp(idAval, idUsine) == 0) {
                if (extraireVolume(&ligne, &volume) && extraireTauxFuite(&ligne, &taux)) {
                    // Volume traité = volume * (1 - taux/100)
                    double volumeTraite = volume * (1.0 - taux / 100.0);
                    volumeTotal += volumeTraite;
                }
            }
        }
    }
    
    fermerFichierCSV(fichier);
    
    return volumeTotal;
}

/**
 * Calcule les fuites totales pour une usine
 */
double calculerFuitesTotales(Noeud* racine, const char* idUsine) {
    if (racine == NULL || idUsine == NULL) {
        return -1.0;
    }
    
    // Déterminer le volume initial
    double volumeInitial = determinerVolumeInitial(idUsine);
    
    if (volumeInitial <= 0.0) {
        fprintf(stderr, "Avertissement: volume initial nul ou invalide\n");
        volumeInitial = 0.0;
    }
    
    printf("Volume initial entrant dans l'usine : %.2f k.m³\n", volumeInitial);
    
    // Calculer les fuites récursivement
    double fuitesKm3 = calculerFuitesRecursif(racine, volumeInitial);
    
    // Convertir de k.m³ en M.m³ (diviser par 1000)
    double fuitesM = fuitesKm3 / 1000.0;
    
    return fuitesM;
}

// ============================================================================
// ÉCRITURE RÉSULTATS
// ============================================================================

/**
 * Écrit le résultat dans le fichier rendement.dat
 */
int ecrireFichierRendement(const char* idUsine, double fuitesM) {
    if (idUsine == NULL) {
        return 1;
    }
    
    FILE* fichier = fopen(FICHIER_RENDEMENT, "a");
    if (fichier == NULL) {
        // Créer le fichier avec en-tête
        fichier = fopen(FICHIER_RENDEMENT, "w");
        if (fichier == NULL) {
            fprintf(stderr, "Erreur: création fichier rendement\n");
            return 2;
        }
        
        fprintf(fichier, "identifier;Leak volume (M.m3.year-1)\n");
    }
    
    // Écrire la ligne
    fprintf(fichier, "%s;%.6f\n", idUsine, fuitesM);
    
    fclose(fichier);
    
    return 0;
}

/**
 * Écrit le résultat avec le tronçon de fuite maximale (BONUS)
 */
int ecrireFichierRendementAvecMax(const char* idUsine, double fuitesM,
                                   const char* idAmont, const char* idAval,
                                   double fuiteMax) {
    if (idUsine == NULL) {
        return 1;
    }
    
    FILE* fichier = fopen(FICHIER_RENDEMENT, "a");
    if (fichier == NULL) {
        fichier = fopen(FICHIER_RENDEMENT, "w");
        if (fichier == NULL) {
            return 2;
        }
        
        fprintf(fichier, "identifier;Leak volume (M.m3.year-1);Max leak section (amont);Max leak section (aval);Max leak value (M.m3.year-1)\n");
    }
    
    fprintf(fichier, "%s;%.6f;%s;%s;%.6f\n", idUsine, fuitesM, idAmont, idAval, fuiteMax);
    
    fclose(fichier);
    
    return 0;
}

// ============================================================================
// VALIDATION
// ============================================================================

/**
 * Vérifie si un identifiant d'usine est valide
 */
int estIDUsineValide(const char* idUsine) {
    if (idUsine == NULL || strlen(idUsine) == 0) {
        return 0;
    }
    
    return 1;
}

// ============================================================================
// DEBUG
// ============================================================================

/**
 * Affiche un résumé des fuites calculées
 */
void afficherResumeFuites(const char* idUsine, double fuitesM) {
    if (idUsine == NULL) {
        return;
    }
    
    printf("\n=== Résumé des fuites ===\n");
    printf("Usine : %s\n", idUsine);
    printf("Fuites totales : %.6f M.m³/an\n", fuitesM);
    printf("Fichier : %s\n", FICHIER_RENDEMENT);
    printf("========================\n\n");
}

/**
 * Compte les nœuds dans l'AVL de recherche
 */
static int compterNoeudsAVLRecherche_rec(NoeudAVLRecherche* racine) {
    if (racine == NULL) {
        return 0;
    }
    return 1 + compterNoeudsAVLRecherche_rec(racine->gauche) + 
           compterNoeudsAVLRecherche_rec(racine->droit);
}

/**
 * Affiche les statistiques de l'AVL de recherche
 */
void afficherStatistiquesAVLRecherche(AVLRecherche* avl) {
    if (avl == NULL) {
        printf("AVL de recherche vide\n");
        return;
    }
    
    int nbNoeuds = compterNoeudsAVLRecherche_rec(avl->racine);
    
    printf("=== AVL de recherche ===\n");
    printf("Nombre de nœuds indexés : %d\n", nbNoeuds);
    printf("=======================\n");
}
