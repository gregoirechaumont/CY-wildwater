/*
 * fichier.h - Header pour la gestion des fichiers CSV
 * Projet C-Wildwater
 * 
 * Ce fichier définit les structures et fonctions pour lire et parser
 * le fichier CSV contenant les données du réseau de distribution
 */

#ifndef FICHIER_H
#define FICHIER_H

#include <stdio.h>

// ============================================================================
// CONSTANTES
// ============================================================================

#define TAILLE_CHAMP 100           // Taille maximale d'un champ CSV
#define TAILLE_LIGNE 512           // Taille maximale d'une ligne CSV
#define SEPARATEUR ';'             // Séparateur de colonnes
#define MARQUEUR_VIDE '-'          // Marqueur pour champ vide

// ============================================================================
// STRUCTURES
// ============================================================================

/**
 * Structure représentant une ligne du fichier CSV
 * 
 * Format du CSV :
 * Col1 ; Col2 ; Col3 ; Col4 ; Col5
 * 
 * - Col1 : ID usine (ou '-')
 * - Col2 : ID amont
 * - Col3 : ID aval (ou '-')
 * - Col4 : Volume (ou '-')
 * - Col5 : Taux de fuite en % (ou '-')
 */
typedef struct {
    char col1[TAILLE_CHAMP];      // ID usine ou '-'
    char col2[TAILLE_CHAMP];      // ID amont
    char col3[TAILLE_CHAMP];      // ID aval ou '-'
    char col4[TAILLE_CHAMP];      // Volume ou '-'
    char col5[TAILLE_CHAMP];      // Taux fuite ou '-'
} LigneCSV;

/**
 * Structure pour les statistiques de lecture
 */
typedef struct {
    int lignesLues;               // Nombre total de lignes lues
    int lignesValides;            // Nombre de lignes valides
    int erreursParsing;           // Nombre d'erreurs de parsing
} StatistiquesLecture;

// ============================================================================
// OUVERTURE ET FERMETURE
// ============================================================================

/**
 * Ouvre un fichier CSV
 * @param chemin : Chemin vers le fichier
 * @return Pointeur FILE ouvert en lecture, NULL si erreur
 */
FILE* ouvrirFichierCSV(const char* chemin);

/**
 * Ferme un fichier
 * @param fichier : Pointeur FILE à fermer
 */
void fermerFichierCSV(FILE* fichier);

// ============================================================================
// LECTURE ET PARSING
// ============================================================================

/**
 * Lit une ligne du fichier CSV et remplit la structure LigneCSV
 * 
 * @param fichier : Fichier ouvert en lecture
 * @param ligne : Structure à remplir
 * @return 1 si ligne lue avec succès, 0 si EOF ou erreur
 */
int lireLigneCSV(FILE* fichier, LigneCSV* ligne);

/**
 * Parse une ligne brute en remplissant la structure LigneCSV
 * @param ligneTexte : Ligne de texte à parser
 * @param ligne : Structure à remplir
 * @return 1 si parsing réussi, 0 si erreur
 */
int parserLigneCSV(const char* ligneTexte, LigneCSV* ligne);

/**
 * Sépare une ligne CSV en champs
 * @param ligneTexte : Ligne à découper
 * @param champs : Tableau de chaînes pour stocker les champs
 * @param maxChamps : Nombre maximum de champs
 * @return Nombre de champs trouvés
 */
int separerChamps(const char* ligneTexte, char champs[][TAILLE_CHAMP], int maxChamps);

// ============================================================================
// VALIDATION ET CONVERSION
// ============================================================================

/**
 * Vérifie si un champ est vide (contient '-')
 * @param champ : Champ à vérifier
 * @return 1 si vide, 0 sinon
 */
int estChampVide(const char* champ);

/**
 * Convertit un champ texte en nombre flottant (volume ou taux)
 * @param champ : Champ texte à convertir
 * @param valeur : Pointeur où stocker la valeur convertie
 * @return 1 si conversion réussie, 0 si erreur ou champ vide
 */
int convertirEnFloat(const char* champ, float* valeur);

/**
 * Convertit un champ texte en nombre double (pour volumes précis)
 * @param champ : Champ texte à convertir
 * @param valeur : Pointeur où stocker la valeur convertie
 * @return 1 si conversion réussie, 0 si erreur ou champ vide
 */
int convertirEnDouble(const char* champ, double* valeur);

/**
 * Vérifie si une ligne est valide
 * @param ligne : Ligne à vérifier
 * @return 1 si valide, 0 sinon
 */
int estLigneValide(const LigneCSV* ligne);

// ============================================================================
// IDENTIFICATION DU TYPE DE LIGNE
// ============================================================================

/**
 * Identifie le type de ligne CSV
 * 
 * Types possibles :
 * - SOURCE_USINE : Spring -> Facility (captage)
 * - USINE : Facility -> - (nœud usine)
 * - USINE_STOCKAGE : Facility -> Storage
 * - STOCKAGE_JONCTION : Storage -> Junction
 * - JONCTION_RACCORDEMENT : Junction -> Service
 * - RACCORDEMENT_USAGER : Service -> Cust (branchement)
 * - INCONNU : Type non reconnu
 */
typedef enum {
    TYPE_SOURCE_USINE,
    TYPE_USINE_NOEUD,
    TYPE_USINE_STOCKAGE,
    TYPE_STOCKAGE_JONCTION,
    TYPE_JONCTION_RACCORDEMENT,
    TYPE_RACCORDEMENT_USAGER,
    TYPE_INCONNU
} TypeLigneCSV;

/**
 * Identifie le type d'une ligne CSV
 * @param ligne : Ligne à analyser
 * @return Type de la ligne
 */
TypeLigneCSV identifierTypeLigne(const LigneCSV* ligne);

/**
 * Retourne une description du type de ligne
 * @param type : Type de ligne
 * @return Chaîne décrivant le type
 */
const char* obtenirNomTypeLigne(TypeLigneCSV type);

// ============================================================================
// EXTRACTION DE DONNÉES
// ============================================================================

/**
 * Extrait l'ID de l'usine depuis col1
 * @param ligne : Ligne CSV
 * @param buffer : Buffer pour stocker l'ID (min 100 caractères)
 * @return 1 si succès, 0 si champ vide
 */
int extraireIDUsine(const LigneCSV* ligne, char* buffer);

/**
 * Extrait l'ID amont depuis col2
 * @param ligne : Ligne CSV
 * @param buffer : Buffer pour stocker l'ID
 * @return 1 si succès, 0 si erreur
 */
int extraireIDAmont(const LigneCSV* ligne, char* buffer);

/**
 * Extrait l'ID aval depuis col3
 * @param ligne : Ligne CSV
 * @param buffer : Buffer pour stocker l'ID
 * @return 1 si succès, 0 si champ vide
 */
int extraireIDAval(const LigneCSV* ligne, char* buffer);

/**
 * Extrait le volume depuis col4
 * @param ligne : Ligne CSV
 * @param volume : Pointeur où stocker le volume
 * @return 1 si succès, 0 si champ vide
 */
int extraireVolume(const LigneCSV* ligne, float* volume);

/**
 * Extrait le taux de fuite depuis col5
 * @param ligne : Ligne CSV
 * @param taux : Pointeur où stocker le taux
 * @return 1 si succès, 0 si champ vide
 */
int extraireTauxFuite(const LigneCSV* ligne, float* taux);

// ============================================================================
// NETTOYAGE DE CHAÎNES
// ============================================================================

/**
 * Supprime les espaces en début et fin de chaîne
 * @param str : Chaîne à nettoyer (modifiée sur place)
 */
void supprimerEspaces(char* str);

/**
 * Supprime les retours à la ligne et caractères invisibles
 * @param str : Chaîne à nettoyer (modifiée sur place)
 */
void nettoyerChaine(char* str);

// ============================================================================
// STATISTIQUES
// ============================================================================

/**
 * Initialise les statistiques de lecture
 * @param stats : Structure à initialiser
 */
void initialiserStatistiques(StatistiquesLecture* stats);

/**
 * Affiche les statistiques de lecture
 * @param stats : Statistiques à afficher
 */
void afficherStatistiques(const StatistiquesLecture* stats);

// ============================================================================
// DEBUG
// ============================================================================

/**
 * Affiche une ligne CSV (pour debug)
 * @param ligne : Ligne à afficher
 */
void afficherLigneCSV(const LigneCSV* ligne);

#endif /* FICHIER_H */




