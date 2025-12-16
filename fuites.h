/*
 * fuites.h - Header pour le calcul des fuites
 * Projet C-Wildwater
 * 
 * Ce fichier définit les fonctions pour calculer les fuites
 * dans le réseau de distribution en aval d'une usine
 */

#ifndef FUITES_H
#define FUITES_H

#include "arbre.h"
#include "fichier.h"

// ============================================================================
// CONSTANTES
// ============================================================================

#define FICHIER_RENDEMENT "output/rendement.dat"
#define VOLUME_NON_TROUVE -1.0      // Valeur retournée si usine non trouvée

// ============================================================================
// STRUCTURE AVL DE RECHERCHE
// ============================================================================

/**
 * AVL auxiliaire pour retrouver rapidement les nœuds
 * 
 * Problème : Lors de la lecture du CSV, on doit retrouver le nœud parent
 * pour y ajouter un enfant. Sans AVL, ce serait O(n) par ligne.
 * Avec AVL : O(log n) par ligne.
 * 
 * Cette structure stocke la correspondance :
 * identifiant -> adresse du Noeud dans l'arbre de distribution
 */
typedef struct NoeudAVLRecherche {
    char identifiant[80];           // Identifiant du nœud
    Noeud* adresse;                 // Pointeur vers le nœud dans l'arbre
    signed char equilibre;          // Facteur d'équilibre
    struct NoeudAVLRecherche* gauche;
    struct NoeudAVLRecherche* droit;
} NoeudAVLRecherche;

/**
 * Structure AVL de recherche
 */
typedef struct {
    NoeudAVLRecherche* racine;
} AVLRecherche;

// ============================================================================
// FONCTION PRINCIPALE
// ============================================================================

/**
 * Traite la commande de calcul de fuites
 * 
 * Workflow :
 * 1. Construit l'arbre de distribution pour l'usine
 * 2. Calcule les fuites totales récursivement
 * 3. Écrit le résultat dans le fichier rendement.dat
 * 
 * @param idUsine : Identifiant de l'usine (avec guillemets si espaces)
 * @return 0 si succès, code d'erreur sinon
 */
int traiterFuites(const char* idUsine);

// ============================================================================
// CONSTRUCTION DE L'ARBRE
// ============================================================================

/**
 * Construit l'arbre de distribution pour une usine donnée
 * 
 * Lit le CSV et crée un arbre n-aire représentant tout le réseau
 * en aval de l'usine spécifiée
 * 
 * @param idUsine : Identifiant de l'usine recherchée
 * @param avlRecherche : AVL auxiliaire pour retrouver les nœuds (créé et rempli)
 * @return Racine de l'arbre (l'usine), NULL si non trouvée
 */
Noeud* construireArbreDistribution(const char* idUsine, AVLRecherche** avlRecherche);

/**
 * Traite une ligne du CSV pour construire l'arbre
 * @param ligne : Ligne CSV à traiter
 * @param type : Type de la ligne
 * @param idUsine : ID de l'usine recherchée
 * @param racine : Pointeur vers la racine (peut être modifié)
 * @param avlRecherche : AVL de recherche
 * @return 1 si ligne traitée, 0 si ignorée, -1 si erreur
 */
int traiterLigneArbre(const LigneCSV* ligne, TypeLigneCSV type,
                      const char* idUsine, Noeud** racine,
                      AVLRecherche* avlRecherche);

// ============================================================================
// AVL DE RECHERCHE - CRÉATION
// ============================================================================

/**
 * Crée un AVL de recherche vide
 * @return Pointeur vers l'AVL
 */
AVLRecherche* creerAVLRecherche();

/**
 * Crée un nœud AVL de recherche
 * @param identifiant : Identifiant du nœud
 * @param adresse : Adresse du nœud dans l'arbre
 * @return Pointeur vers le nouveau nœud AVL
 */
NoeudAVLRecherche* creerNoeudAVLRecherche(const char* identifiant, Noeud* adresse);

// ============================================================================
// AVL DE RECHERCHE - INSERTION
// ============================================================================

/**
 * Insère un nœud dans l'AVL de recherche (récursif)
 * @param racine : Racine du sous-arbre
 * @param identifiant : Identifiant à insérer
 * @param adresse : Adresse du nœud
 * @param h : Pointeur sur variable de hauteur
 * @return Nouvelle racine du sous-arbre
 */
NoeudAVLRecherche* insererRecherche_rec(NoeudAVLRecherche* racine,
                                        const char* identifiant,
                                        Noeud* adresse, int* h);

/**
 * Insère un nœud dans l'AVL de recherche (fonction publique)
 * @param avl : AVL de recherche
 * @param identifiant : Identifiant à insérer
 * @param adresse : Adresse du nœud
 * @return 1 si succès, 0 si erreur
 */
int insererRecherche(AVLRecherche* avl, const char* identifiant, Noeud* adresse);

// ============================================================================
// AVL DE RECHERCHE - RECHERCHE
// ============================================================================

/**
 * Recherche un nœud dans l'AVL de recherche
 * @param racine : Racine de l'AVL
 * @param identifiant : Identifiant recherché
 * @return Adresse du nœud dans l'arbre, NULL si non trouvé
 */
Noeud* rechercherNoeud(NoeudAVLRecherche* racine, const char* identifiant);

/**
 * Recherche un nœud (version publique)
 * @param avl : AVL de recherche
 * @param identifiant : Identifiant recherché
 * @return Adresse du nœud dans l'arbre, NULL si non trouvé
 */
Noeud* rechercherNoeudPublic(AVLRecherche* avl, const char* identifiant);

// ============================================================================
// AVL DE RECHERCHE - ROTATIONS
// ============================================================================

/**
 * Rotation gauche pour AVL de recherche
 * @param a : Nœud déséquilibré
 * @return Nouvelle racine
 */
NoeudAVLRecherche* rotationGaucheRecherche(NoeudAVLRecherche* a);

/**
 * Rotation droite pour AVL de recherche
 * @param a : Nœud déséquilibré
 * @return Nouvelle racine
 */
NoeudAVLRecherche* rotationDroiteRecherche(NoeudAVLRecherche* a);

/**
 * Double rotation gauche pour AVL de recherche
 * @param a : Nœud déséquilibré
 * @return Nouvelle racine
 */
NoeudAVLRecherche* doubleRotationGaucheRecherche(NoeudAVLRecherche* a);

/**
 * Double rotation droite pour AVL de recherche
 * @param a : Nœud déséquilibré
 * @return Nouvelle racine
 */
NoeudAVLRecherche* doubleRotationDroiteRecherche(NoeudAVLRecherche* a);

/**
 * Équilibre un nœud AVL de recherche
 * @param a : Nœud à équilibrer
 * @return Nœud équilibré
 */
NoeudAVLRecherche* equilibrerAVLRecherche(NoeudAVLRecherche* a);

// ============================================================================
// AVL DE RECHERCHE - LIBÉRATION
// ============================================================================

/**
 * Libère un AVL de recherche (récursif)
 * @param racine : Racine du sous-arbre
 */
void libererAVLRecherche_rec(NoeudAVLRecherche* racine);

/**
 * Libère un AVL de recherche complet
 * @param avl : AVL à libérer
 */
void libererAVLRecherche(AVLRecherche* avl);

// ============================================================================
// CALCUL DES FUITES
// ============================================================================

/**
 * Calcule les fuites totales pour une usine
 * 
 * Détermine d'abord le volume initial en lisant le CSV
 * puis calcule récursivement les fuites dans tout l'arbre
 * 
 * @param racine : Racine de l'arbre (l'usine)
 * @param idUsine : ID de l'usine (pour retrouver le volume initial)
 * @return Volume total des fuites en M.m³, -1 si erreur
 */
double calculerFuitesTotales(Noeud* racine, const char* idUsine);

/**
 * Détermine le volume initial qui entre dans l'usine
 * en sommant tous les volumes captés par les sources
 * 
 * @param idUsine : ID de l'usine
 * @return Volume initial en k.m³
 */
double determinerVolumeInitial(const char* idUsine);

// ============================================================================
// ÉCRITURE RÉSULTATS
// ============================================================================

/**
 * Écrit le résultat dans le fichier rendement.dat
 * 
 * Format :
 * - Si fichier n'existe pas : créer avec en-tête
 * - Ajouter ligne : identifiant;volume_fuites (M.m³)
 * 
 * @param idUsine : Identifiant de l'usine
 * @param fuitesM : Volume de fuites en M.m³
 * @return 0 si succès, code d'erreur sinon
 */
int ecrireFichierRendement(const char* idUsine, double fuitesM);

/**
 * Écrit le résultat avec le tronçon de fuite maximale (BONUS)
 * @param idUsine : Identifiant de l'usine
 * @param fuitesM : Volume de fuites en M.m³
 * @param idAmont : ID amont du tronçon à fuite max
 * @param idAval : ID aval du tronçon à fuite max
 * @param fuiteMax : Valeur de la fuite max en M.m³
 * @return 0 si succès, code d'erreur sinon
 */
int ecrireFichierRendementAvecMax(const char* idUsine, double fuitesM,
                                   const char* idAmont, const char* idAval,
                                   double fuiteMax);

// ============================================================================
// VALIDATION
// ============================================================================

/**
 * Vérifie si un identifiant d'usine est valide
 * @param idUsine : ID à vérifier
 * @return 1 si valide, 0 sinon
 */
int estIDUsineValide(const char* idUsine);

// ============================================================================
// DEBUG
// ============================================================================

/**
 * Affiche un résumé des fuites calculées
 * @param idUsine : ID de l'usine
 * @param fuitesM : Fuites en M.m³
 */
void afficherResumeFuites(const char* idUsine, double fuitesM);

/**
 * Affiche les statistiques de l'AVL de recherche
 * @param avl : AVL de recherche
 */
void afficherStatistiquesAVLRecherche(AVLRecherche* avl);

#endif /* FUITES_H */
