/*
 * utils.c - Implémentation des fonctions utilitaires
 * Projet C-Wildwater
 * 
 * Bibliothèque de fonctions génériques
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include "utils.h"

// ============================================================================
// MANIPULATION DE CHAÎNES
// ============================================================================

/**
 * Supprime les espaces en début et fin de chaîne (trim)
 */
void trim(char* str) {
    if (str == NULL || *str == '\0') {
        return;
    }
    
    // Supprimer espaces au début
    char* debut = str;
    while (isspace((unsigned char)*debut)) {
        debut++;
    }
    
    // Décaler la chaîne si nécessaire
    if (debut != str) {
        memmove(str, debut, strlen(debut) + 1);
    }
    
    // Supprimer espaces à la fin
    char* fin = str + strlen(str) - 1;
    while (fin > str && isspace((unsigned char)*fin)) {
        *fin = '\0';
        fin--;
    }
}

/**
 * Copie une chaîne de manière sécurisée
 */
void copierChaineSafe(char* dest, const char* src, int taille) {
    if (dest == NULL || src == NULL || taille <= 0) {
        return;
    }
    
    strncpy(dest, src, taille - 1);
    dest[taille - 1] = '\0';
}

/**
 * Convertit une chaîne en minuscules
 */
void toLower(char* str) {
    if (str == NULL) {
        return;
    }
    
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

/**
 * Convertit une chaîne en majuscules
 */
void toUpper(char* str) {
    if (str == NULL) {
        return;
    }
    
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

/**
 * Vérifie si une chaîne commence par un préfixe
 */
int commencePar(const char* str, const char* prefix) {
    if (str == NULL || prefix == NULL) {
        return 0;
    }
    
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

/**
 * Vérifie si une chaîne se termine par un suffixe
 */
int terminePar(const char* str, const char* suffix) {
    if (str == NULL || suffix == NULL) {
        return 0;
    }
    
    int lenStr = strlen(str);
    int lenSuffix = strlen(suffix);
    
    if (lenStr < lenSuffix) {
        return 0;
    }
    
    return strcmp(str + lenStr - lenSuffix, suffix) == 0;
}

// ============================================================================
// VALIDATION
// ============================================================================

/**
 * Vérifie si une chaîne représente un nombre entier valide
 */
int estNombreEntier(const char* str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }
    
    // Permettre un signe au début
    if (*str == '-' || *str == '+') {
        str++;
    }
    
    // Vérifier que tous les caractères sont des chiffres
    while (*str != '\0') {
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    
    return 1;
}

/**
 * Vérifie si une chaîne représente un nombre flottant valide
 */
int estNombreFlottant(const char* str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }
    
    char* endptr;
    strtod(str, &endptr);
    
    // Vérifier qu'on a lu toute la chaîne
    return (*endptr == '\0');
}

/**
 * Vérifie si une chaîne est vide
 */
int estChaineVide(const char* str) {
    return (str == NULL || *str == '\0');
}

/**
 * Vérifie si un identifiant est valide
 */
int estIdentifiantValide(const char* id) {
    if (estChaineVide(id)) {
        return 0;
    }
    
    // Un identifiant valide ne contient que des caractères alphanumériques,
    // espaces, tirets, dièse et underscores
    for (int i = 0; id[i] != '\0'; i++) {
        char c = id[i];
        if (!isalnum((unsigned char)c) && c != ' ' && c != '-' && 
            c != '_' && c != '#') {
            return 0;
        }
    }
    
    return 1;
}

// ============================================================================
// GESTION DE FICHIERS
// ============================================================================

/**
 * Vérifie si un fichier existe
 */
int fichierExiste(const char* chemin) {
    if (chemin == NULL) {
        return 0;
    }
    
    FILE* fichier = fopen(chemin, "r");
    if (fichier != NULL) {
        fclose(fichier);
        return 1;
    }
    
    return 0;
}

/**
 * Vérifie si un répertoire existe
 */
int repertoireExiste(const char* chemin) {
    if (chemin == NULL) {
        return 0;
    }
    
    struct stat st;
    if (stat(chemin, &st) == 0 && S_ISDIR(st.st_mode)) {
        return 1;
    }
    
    return 0;
}

/**
 * Crée un répertoire s'il n'existe pas
 */
int creerRepertoire(const char* chemin) {
    if (chemin == NULL) {
        return 0;
    }
    
    if (repertoireExiste(chemin)) {
        return 1;  // Déjà existe
    }
    
#ifdef _WIN32
    return mkdir(chemin) == 0;
#else
    return mkdir(chemin, 0755) == 0;
#endif
}

/**
 * Obtient la taille d'un fichier en octets
 */
long obtenirTailleFichier(const char* chemin) {
    if (chemin == NULL) {
        return -1;
    }
    
    struct stat st;
    if (stat(chemin, &st) == 0) {
        return st.st_size;
    }
    
    return -1;
}

// ============================================================================
// GESTION D'ERREURS
// ============================================================================

/**
 * Affiche un message d'erreur formaté sur stderr
 */
void afficherErreur(const char* fonction, const char* message) {
    if (fonction != NULL && message != NULL) {
        fprintf(stderr, "[ERREUR] %s: %s\n", fonction, message);
    } else if (message != NULL) {
        fprintf(stderr, "[ERREUR] %s\n", message);
    }
}

/**
 * Affiche un avertissement formaté sur stderr
 */
void afficherAvertissement(const char* fonction, const char* message) {
    if (fonction != NULL && message != NULL) {
        fprintf(stderr, "[AVERTISSEMENT] %s: %s\n", fonction, message);
    } else if (message != NULL) {
        fprintf(stderr, "[AVERTISSEMENT] %s\n", message);
    }
}

/**
 * Affiche un message d'information formaté sur stdout
 */
void afficherInfo(const char* fonction, const char* message) {
    if (fonction != NULL && message != NULL) {
        printf("[INFO] %s: %s\n", fonction, message);
    } else if (message != NULL) {
        printf("[INFO] %s\n", message);
    }
}

// ============================================================================
// MATHÉMATIQUES
// ============================================================================

/**
 * Retourne le minimum de deux entiers
 */
int minInt(int a, int b) {
    return (a < b) ? a : b;
}

/**
 * Retourne le maximum de deux entiers
 */
int maxInt(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * Retourne le minimum de deux flottants
 */
float minFloat(float a, float b) {
    return (a < b) ? a : b;
}

/**
 * Retourne le maximum de deux flottants
 */
float maxFloat(float a, float b) {
    return (a > b) ? a : b;
}

/**
 * Arrondit un double à n décimales
 */
double arrondir(double valeur, int decimales) {
    double multiplicateur = pow(10.0, decimales);
    return round(valeur * multiplicateur) / multiplicateur;
}

// ============================================================================
// CHRONOMÈTRE ET PERFORMANCE
// ============================================================================

/**
 * Démarre un chronomètre
 */
long long demarrerChronometre() {
    return (long long)time(NULL);
}

/**
 * Arrête le chronomètre et retourne le temps écoulé en millisecondes
 */
long long arreterChronometre(long long debut) {
    long long fin = (long long)time(NULL);
    return (fin - debut) * 1000;  // Convertir en ms
}

/**
 * Affiche le temps écoulé de manière formatée
 */
void afficherTempsEcoule(long long tempsMs) {
    if (tempsMs < 1000) {
        printf("Temps écoulé : %lld ms\n", tempsMs);
    } else if (tempsMs < 60000) {
        printf("Temps écoulé : %.2f secondes\n", tempsMs / 1000.0);
    } else {
        int minutes = tempsMs / 60000;
        int secondes = (tempsMs % 60000) / 1000;
        printf("Temps écoulé : %d min %d sec\n", minutes, secondes);
    }
}

// ============================================================================
// CONVERSION D'UNITÉS
// ============================================================================

/**
 * Convertit des k.m³ en M.m³
 */
double km3VersM(double km3) {
    return km3 / 1000.0;
}

/**
 * Convertit des M.m³ en k.m³
 */
double MVerskm3(double Mm3) {
    return Mm3 * 1000.0;
}

/**
 * Formate un volume avec l'unité appropriée
 */
void formaterVolume(double volume, char* buffer, int tailleBuffer) {
    if (buffer == NULL || tailleBuffer <= 0) {
        return;
    }
    
    if (volume < 1.0) {
        snprintf(buffer, tailleBuffer, "%.3f k.m³", volume);
    } else if (volume < 1000.0) {
        snprintf(buffer, tailleBuffer, "%.2f k.m³", volume);
    } else {
        snprintf(buffer, tailleBuffer, "%.2f M.m³", volume / 1000.0);
    }
}

// ============================================================================
// AFFICHAGE ET FORMATAGE
// ============================================================================

/**
 * Affiche une barre de progression
 */
void afficherBarreProgression(int actuel, int total, int largeur) {
    if (total <= 0) {
        return;
    }
    
    float pourcentage = (float)actuel / total;
    int nbCaracteres = (int)(pourcentage * largeur);
    
    printf("\r[");
    for (int i = 0; i < largeur; i++) {
        if (i < nbCaracteres) {
            printf("=");
        } else if (i == nbCaracteres) {
            printf(">");
        } else {
            printf(" ");
        }
    }
    printf("] %3.0f%% (%d/%d)", pourcentage * 100, actuel, total);
    fflush(stdout);
    
    if (actuel >= total) {
        printf("\n");
    }
}

/**
 * Affiche un séparateur formaté
 */
void afficherSeparateur(char caractere, int largeur) {
    for (int i = 0; i < largeur; i++) {
        printf("%c", caractere);
    }
    printf("\n");
}

/**
 * Affiche un titre centré
 */
void afficherTitreCentre(const char* titre, int largeur) {
    if (titre == NULL) {
        return;
    }
    
    int lenTitre = strlen(titre);
    int padding = (largeur - lenTitre) / 2;
    
    if (padding > 0) {
        for (int i = 0; i < padding; i++) {
            printf(" ");
        }
    }
    
    printf("%s\n", titre);
}

// ============================================================================
// MÉMOIRE ET DEBUG
// ============================================================================

/**
 * Affiche la quantité de mémoire utilisée (estimation basique)
 */
void afficherUtilisationMemoire() {
    // Cette fonction est une estimation très basique
    // Pour un vrai monitoring mémoire, utiliser des outils spécialisés
    printf("Note: Utiliser valgrind ou un outil similaire pour le monitoring mémoire précis\n");
}

/**
 * Vérifie si un pointeur est NULL
 */
int verifierPointeurNull(void* ptr, const char* nom) {
    if (ptr == NULL) {
        if (nom != NULL) {
            afficherErreur("verifierPointeurNull", nom);
        } else {
            afficherErreur("verifierPointeurNull", "Pointeur NULL détecté");
        }
        return 1;
    }
    return 0;
}
