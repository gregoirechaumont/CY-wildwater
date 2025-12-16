/*
 * utils.h - Header pour les fonctions utilitaires
 * Projet C-Wildwater
 * 
 * Fonctions génériques et helpers utilisés dans tout le projet
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

// ============================================================================
// CONSTANTES
// ============================================================================

#define MAX_CHEMIN 256
#define MAX_LIGNE 1024
#define MAX_MESSAGE 512

// ============================================================================
// MANIPULATION DE CHAÎNES
// ============================================================================

/**
 * Supprime les espaces en début et fin de chaîne (trim)
 * @param str : Chaîne à nettoyer (modifiée sur place)
 */
void trim(char* str);

/**
 * Copie une chaîne de manière sécurisée avec limite de taille
 * @param dest : Destination
 * @param src : Source
 * @param taille : Taille max de dest
 */
void copierChaineSafe(char* dest, const char* src, int taille);

/**
 * Convertit une chaîne en minuscules
 * @param str : Chaîne à convertir (modifiée sur place)
 */
void toLower(char* str);

/**
 * Convertit une chaîne en majuscules
 * @param str : Chaîne à convertir (modifiée sur place)
 */
void toUpper(char* str);

/**
 * Vérifie si une chaîne commence par un préfixe
 * @param str : Chaîne à tester
 * @param prefix : Préfixe recherché
 * @return 1 si str commence par prefix, 0 sinon
 */
int commencePar(const char* str, const char* prefix);

/**
 * Vérifie si une chaîne se termine par un suffixe
 * @param str : Chaîne à tester
 * @param suffix : Suffixe recherché
 * @return 1 si str se termine par suffix, 0 sinon
 */
int terminePar(const char* str, const char* suffix);

// ============================================================================
// VALIDATION
// ============================================================================

/**
 * Vérifie si une chaîne représente un nombre entier valide
 * @param str : Chaîne à vérifier
 * @return 1 si nombre valide, 0 sinon
 */
int estNombreEntier(const char* str);

/**
 * Vérifie si une chaîne représente un nombre flottant valide
 * @param str : Chaîne à vérifier
 * @return 1 si nombre valide, 0 sinon
 */
int estNombreFlottant(const char* str);

/**
 * Vérifie si une chaîne est vide (NULL ou longueur 0)
 * @param str : Chaîne à vérifier
 * @return 1 si vide, 0 sinon
 */
int estChaineVide(const char* str);

/**
 * Vérifie si un identifiant est valide (non vide, sans caractères spéciaux)
 * @param id : Identifiant à vérifier
 * @return 1 si valide, 0 sinon
 */
int estIdentifiantValide(const char* id);

// ============================================================================
// GESTION DE FICHIERS
// ============================================================================

/**
 * Vérifie si un fichier existe
 * @param chemin : Chemin du fichier
 * @return 1 si existe, 0 sinon
 */
int fichierExiste(const char* chemin);

/**
 * Vérifie si un répertoire existe
 * @param chemin : Chemin du répertoire
 * @return 1 si existe, 0 sinon
 */
int repertoireExiste(const char* chemin);

/**
 * Crée un répertoire s'il n'existe pas
 * @param chemin : Chemin du répertoire
 * @return 1 si succès, 0 si erreur
 */
int creerRepertoire(const char* chemin);

/**
 * Obtient la taille d'un fichier en octets
 * @param chemin : Chemin du fichier
 * @return Taille en octets, -1 si erreur
 */
long obtenirTailleFichier(const char* chemin);

// ============================================================================
// GESTION D'ERREURS
// ============================================================================

/**
 * Affiche un message d'erreur formaté sur stderr
 * @param fonction : Nom de la fonction où l'erreur s'est produite
 * @param message : Message d'erreur
 */
void afficherErreur(const char* fonction, const char* message);

/**
 * Affiche un avertissement formaté sur stderr
 * @param fonction : Nom de la fonction
 * @param message : Message d'avertissement
 */
void afficherAvertissement(const char* fonction, const char* message);

/**
 * Affiche un message d'information formaté sur stdout
 * @param fonction : Nom de la fonction
 * @param message : Message d'information
 */
void afficherInfo(const char* fonction, const char* message);

// ============================================================================
// MATHÉMATIQUES
// ============================================================================

/**
 * Retourne le minimum de deux entiers
 * @param a : Premier entier
 * @param b : Deuxième entier
 * @return Le plus petit des deux
 */
int minInt(int a, int b);

/**
 * Retourne le maximum de deux entiers
 * @param a : Premier entier
 * @param b : Deuxième entier
 * @return Le plus grand des deux
 */
int maxInt(int a, int b);

/**
 * Retourne le minimum de deux flottants
 * @param a : Premier flottant
 * @param b : Deuxième flottant
 * @return Le plus petit des deux
 */
float minFloat(float a, float b);

/**
 * Retourne le maximum de deux flottants
 * @param a : Premier flottant
 * @param b : Deuxième flottant
 * @return Le plus grand des deux
 */
float maxFloat(float a, float b);

/**
 * Arrondit un double à n décimales
 * @param valeur : Valeur à arrondir
 * @param decimales : Nombre de décimales
 * @return Valeur arrondie
 */
double arrondir(double valeur, int decimales);

// ============================================================================
// CHRONOMÈTRE ET PERFORMANCE
// ============================================================================

/**
 * Démarre un chronomètre
 * @return Timestamp de départ
 */
long long demarrerChronometre();

/**
 * Arrête le chronomètre et retourne le temps écoulé en millisecondes
 * @param debut : Timestamp de départ
 * @return Temps écoulé en ms
 */
long long arreterChronometre(long long debut);

/**
 * Affiche le temps écoulé de manière formatée
 * @param tempsMs : Temps en millisecondes
 */
void afficherTempsEcoule(long long tempsMs);

// ============================================================================
// CONVERSION D'UNITÉS
// ============================================================================

/**
 * Convertit des k.m³ en M.m³
 * @param km3 : Valeur en k.m³
 * @return Valeur en M.m³
 */
double km3VersM(double km3);

/**
 * Convertit des M.m³ en k.m³
 * @param Mm3 : Valeur en M.m³
 * @return Valeur en k.m³
 */
double MVerskm3(double Mm3);

/**
 * Formate un volume avec l'unité appropriée
 * @param volume : Volume à formater
 * @param buffer : Buffer pour le résultat
 * @param tailleBuffer : Taille du buffer
 */
void formaterVolume(double volume, char* buffer, int tailleBuffer);

// ============================================================================
// AFFICHAGE ET FORMATAGE
// ============================================================================

/**
 * Affiche une barre de progression
 * @param actuel : Valeur actuelle
 * @param total : Valeur totale
 * @param largeur : Largeur de la barre (en caractères)
 */
void afficherBarreProgression(int actuel, int total, int largeur);

/**
 * Affiche un séparateur formaté
 * @param caractere : Caractère à utiliser ('-', '=', etc.)
 * @param largeur : Largeur du séparateur
 */
void afficherSeparateur(char caractere, int largeur);

/**
 * Affiche un titre centré
 * @param titre : Titre à afficher
 * @param largeur : Largeur totale
 */
void afficherTitreCentre(const char* titre, int largeur);

// ============================================================================
// MÉMOIRE ET DEBUG
// ============================================================================

/**
 * Affiche la quantité de mémoire utilisée par le programme (estimation)
 */
void afficherUtilisationMemoire();

/**
 * Vérifie si un pointeur est NULL et affiche une erreur si c'est le cas
 * @param ptr : Pointeur à vérifier
 * @param nom : Nom de la variable (pour le message)
 * @return 1 si NULL (erreur), 0 si OK
 */
int verifierPointeurNull(void* ptr, const char* nom);

#endif /* UTILS_H */
