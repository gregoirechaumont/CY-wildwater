/*
 * usine.h - Header pour le traitement des histogrammes d'usines
 * Projet C-Wildwater
 * 
 * Ce fichier définit les fonctions pour générer les histogrammes
 * des capacités, volumes captés et volumes traités des usines
 */

#ifndef USINE_H
#define USINE_H

#include "avl.h"
#include "fichier.h"

// ============================================================================
// CONSTANTES
// ============================================================================

// Modes de traitement
#define MODE_MAX "max"              // Capacité maximale
#define MODE_SRC "src"              // Volume capté
#define MODE_REAL "real"            // Volume traité

// Noms de fichiers de sortie
#define FICHIER_MAX "output/vol_max.dat"
#define FICHIER_SRC "output/vol_src.dat"
#define FICHIER_REAL "output/vol_real.dat"

// ============================================================================
// FONCTION PRINCIPALE
// ============================================================================

/**
 * Traite la commande histogramme
 * 
 * Workflow :
 * 1. Crée un AVL vide
 * 2. Lit le fichier CSV et remplit l'AVL
 * 3. Écrit le fichier .dat trié
 * 
 * @param mode : Mode de traitement ("max", "src", "real")
 * @return 0 si succès, code d'erreur sinon
 */
int traiterHistogramme(const char* mode);

// ============================================================================
// LECTURE ET REMPLISSAGE AVL
// ============================================================================

/**
 * Lit le fichier CSV et remplit l'AVL selon le mode
 * 
 * Traitement par type de ligne :
 * - SOURCE -> USINE : ajoute volumeCapte et volumeTraite
 * - USINE (nœud) : définit volumeMax
 * - Autres lignes : ignorées pour les histogrammes
 * 
 * @param avl : AVL à remplir
 * @param mode : Mode de traitement
 * @return 0 si succès, code d'erreur sinon
 */
int lireEtRemplirAVL(AVL* avl, const char* mode);

/**
 * Traite une ligne SOURCE -> USINE pour l'AVL
 * @param avl : AVL à mettre à jour
 * @param ligne : Ligne CSV à traiter
 * @param mode : Mode de traitement
 * @return 0 si succès, code d'erreur sinon
 */
int traiterLigneSourceUsine(AVL* avl, const LigneCSV* ligne, const char* mode);

/**
 * Traite une ligne USINE (nœud) pour l'AVL
 * @param avl : AVL à mettre à jour
 * @param ligne : Ligne CSV à traiter
 * @return 0 si succès, code d'erreur sinon
 */
int traiterLigneUsineNoeud(AVL* avl, const LigneCSV* ligne);

// ============================================================================
// ÉCRITURE FICHIER .DAT
// ============================================================================

/**
 * Écrit le fichier .dat avec les données de l'AVL
 * 
 * Format du fichier :
 * - Ligne 1 : En-têtes selon le mode
 * - Lignes suivantes : identifiant;volume
 * - Ordre : alphabétique inverse (requis par le projet)
 * 
 * @param avl : AVL contenant les données
 * @param mode : Mode de traitement
 * @return 0 si succès, code d'erreur sinon
 */
int ecrireFichierDat(AVL* avl, const char* mode);

/**
 * Obtient le nom du fichier de sortie selon le mode
 * @param mode : Mode de traitement
 * @return Chemin du fichier de sortie
 */
const char* obtenirNomFichierSortie(const char* mode);

/**
 * Obtient l'en-tête du fichier selon le mode
 * @param mode : Mode de traitement
 * @return Chaîne d'en-tête
 */
const char* obtenirEnteteFichier(const char* mode);

// ============================================================================
// VALIDATION
// ============================================================================

/**
 * Vérifie si le mode est valide
 * @param mode : Mode à vérifier
 * @return 1 si valide, 0 sinon
 */
int estModeValide(const char* mode);

/**
 * Vérifie si l'AVL contient des données
 * @param avl : AVL à vérifier
 * @return 1 si non vide, 0 si vide
 */
int avlNonVide(AVL* avl);

// ============================================================================
// CALCUL DES VOLUMES
// ============================================================================

/**
 * Calcule le volume traité depuis le volume capté et le taux de fuite
 * 
 * Formule : volumeTraité = volumeCapté * (1 - tauxFuite/100)
 * 
 * @param volumeCapte : Volume capté en k.m³
 * @param tauxFuite : Taux de fuite en %
 * @return Volume traité en k.m³
 */
float calculerVolumeTraite(float volumeCapte, float tauxFuite);

// ============================================================================
// DEBUG ET AFFICHAGE
// ============================================================================

/**
 * Affiche un résumé du traitement
 * @param avl : AVL contenant les données
 * @param mode : Mode de traitement
 */
void afficherResume(AVL* avl, const char* mode);

#endif /* USINE_H */
