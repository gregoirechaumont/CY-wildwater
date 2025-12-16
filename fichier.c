/*
 * fichier.c - Implémentation de la lecture et parsing CSV
 * Projet C-Wildwater
 * 
 * Gestion complète du fichier CSV avec parsing robuste
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fichier.h"

// ============================================================================
// OUVERTURE ET FERMETURE
// ============================================================================

/**
 * Ouvre un fichier CSV en lecture depuis stdin
 */
FILE* ouvrirFichierCSV(const char* chemin) {
    // Pour ce projet, on lit depuis stdin (pipe depuis shell)
    // Le script shell redirige le CSV vers stdin
    (void)chemin;  // Paramètre non utilisé mais gardé pour l'interface
    
    return stdin;
}

/**
 * Ferme un fichier CSV
 */
void fermerFichierCSV(FILE* fichier) {
    // Ne pas fermer stdin
    if (fichier != NULL && fichier != stdin) {
        fclose(fichier);
    }
}

// ============================================================================
// NETTOYAGE DE CHAÎNES
// ============================================================================

/**
 * Supprime les espaces en début et fin de chaîne
 */
void supprimerEspaces(char* str) {
    if (str == NULL || *str == '\0') {
        return;
    }
    
    // Supprimer espaces au début
    char* debut = str;
    while (isspace((unsigned char)*debut)) {
        debut++;
    }
    
    // Décaler la chaîne
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
 * Supprime les retours à la ligne et caractères invisibles
 */
void nettoyerChaine(char* str) {
    if (str == NULL) {
        return;
    }
    
    // Enlever \n, \r, \t
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == '\n' || str[i] == '\r' || str[i] == '\t') {
            str[i] = '\0';
            break;
        }
    }
    
    supprimerEspaces(str);
}

// ============================================================================
// SÉPARATION DES CHAMPS
// ============================================================================

/**
 * Sépare une ligne CSV en champs
 */
int separerChamps(const char* ligneTexte, char champs[][TAILLE_CHAMP], int maxChamps) {
    if (ligneTexte == NULL || champs == NULL) {
        return 0;
    }
    
    int nbChamps = 0;
    int posChamp = 0;
    int longueur = strlen(ligneTexte);
    
    for (int i = 0; i <= longueur && nbChamps < maxChamps; i++) {
        // Si on trouve un séparateur ou la fin de ligne
        if (ligneTexte[i] == SEPARATEUR || ligneTexte[i] == '\0') {
            champs[nbChamps][posChamp] = '\0';
            nettoyerChaine(champs[nbChamps]);
            nbChamps++;
            posChamp = 0;
        } else {
            // Copier le caractère
            if (posChamp < TAILLE_CHAMP - 1) {
                champs[nbChamps][posChamp] = ligneTexte[i];
                posChamp++;
            }
        }
    }
    
    return nbChamps;
}

// ============================================================================
// PARSING
// ============================================================================

/**
 * Parse une ligne brute en remplissant la structure LigneCSV
 */
int parserLigneCSV(const char* ligneTexte, LigneCSV* ligne) {
    if (ligneTexte == NULL || ligne == NULL) {
        return 0;
    }
    
    // Tableau temporaire pour stocker les champs
    char champs[5][TAILLE_CHAMP];
    
    // Séparer la ligne en champs
    int nbChamps = separerChamps(ligneTexte, champs, 5);
    
    if (nbChamps < 5) {
        // Ligne incomplète, remplir avec des tirets
        for (int i = nbChamps; i < 5; i++) {
            strcpy(champs[i], "-");
        }
    }
    
    // Copier dans la structure
    strcpy(ligne->col1, champs[0]);
    strcpy(ligne->col2, champs[1]);
    strcpy(ligne->col3, champs[2]);
    strcpy(ligne->col4, champs[3]);
    strcpy(ligne->col5, champs[4]);
    
    return 1;
}

/**
 * Lit une ligne du fichier CSV et remplit la structure
 */
int lireLigneCSV(FILE* fichier, LigneCSV* ligne) {
    if (fichier == NULL || ligne == NULL) {
        return 0;
    }
    
    char buffer[TAILLE_LIGNE];
    
    // Lire une ligne
    if (fgets(buffer, TAILLE_LIGNE, fichier) == NULL) {
        return 0;  // EOF ou erreur
    }
    
    // Parser la ligne
    return parserLigneCSV(buffer, ligne);
}

// ============================================================================
// VALIDATION ET CONVERSION
// ============================================================================

/**
 * Vérifie si un champ est vide (contient '-')
 */
int estChampVide(const char* champ) {
    if (champ == NULL || strlen(champ) == 0) {
        return 1;
    }
    
    // Vérifier si c'est un tiret
    if (strcmp(champ, "-") == 0) {
        return 1;
    }
    
    return 0;
}

/**
 * Convertit un champ texte en float
 */
int convertirEnFloat(const char* champ, float* valeur) {
    if (champ == NULL || valeur == NULL) {
        return 0;
    }
    
    // Vérifier si le champ est vide
    if (estChampVide(champ)) {
        return 0;
    }
    
    // Convertir
    char* endptr;
    *valeur = strtof(champ, &endptr);
    
    // Vérifier si la conversion a réussi
    if (endptr == champ || *endptr != '\0') {
        return 0;
    }
    
    return 1;
}

/**
 * Convertit un champ texte en double
 */
int convertirEnDouble(const char* champ, double* valeur) {
    if (champ == NULL || valeur == NULL) {
        return 0;
    }
    
    // Vérifier si le champ est vide
    if (estChampVide(champ)) {
        return 0;
    }
    
    // Convertir
    char* endptr;
    *valeur = strtod(champ, &endptr);
    
    // Vérifier si la conversion a réussi
    if (endptr == champ || *endptr != '\0') {
        return 0;
    }
    
    return 1;
}

/**
 * Vérifie si une ligne est valide
 */
int estLigneValide(const LigneCSV* ligne) {
    if (ligne == NULL) {
        return 0;
    }
    
    // Une ligne est valide si col2 n'est pas vide
    if (estChampVide(ligne->col2)) {
        return 0;
    }
    
    return 1;
}

// ============================================================================
// IDENTIFICATION DU TYPE DE LIGNE
// ============================================================================

/**
 * Vérifie si une chaîne contient un motif
 */
static int contient(const char* chaine, const char* motif) {
    return strstr(chaine, motif) != NULL;
}

/**
 * Identifie le type d'une ligne CSV
 */
TypeLigneCSV identifierTypeLigne(const LigneCSV* ligne) {
    if (ligne == NULL || !estLigneValide(ligne)) {
        return TYPE_INCONNU;
    }
    
    int col1Vide = estChampVide(ligne->col1);
    int col3Vide = estChampVide(ligne->col3);
    int col4Vide = estChampVide(ligne->col4);
    int col5Vide = estChampVide(ligne->col5);
    
    // SOURCE -> USINE : col1='-', col2=Spring, col3=Facility, col4=volume, col5=taux
    if (col1Vide && contient(ligne->col2, "Spring") && 
        contient(ligne->col3, "Facility") && !col4Vide && !col5Vide) {
        return TYPE_SOURCE_USINE;
    }
    
    // USINE (nœud) : col1='-', col2=Facility, col3='-', col4=volume, col5='-'
    if (col1Vide && contient(ligne->col2, "Facility") && 
        col3Vide && !col4Vide && col5Vide) {
        return TYPE_USINE_NOEUD;
    }
    
    // USINE -> STOCKAGE : col1='-', col2=Facility, col3=Storage, col4='-', col5=taux
    if (col1Vide && contient(ligne->col2, "Facility") && 
        contient(ligne->col3, "Storage") && col4Vide && !col5Vide) {
        return TYPE_USINE_STOCKAGE;
    }
    
    // STOCKAGE -> JONCTION : col1=Facility, col2=Storage, col3=Junction, col4='-', col5=taux
    if (!col1Vide && contient(ligne->col2, "Storage") && 
        contient(ligne->col3, "Junction") && col4Vide && !col5Vide) {
        return TYPE_STOCKAGE_JONCTION;
    }
    
    // JONCTION -> RACCORDEMENT : col1=Facility, col2=Junction, col3=Service, col4='-', col5=taux
    if (!col1Vide && contient(ligne->col2, "Junction") && 
        contient(ligne->col3, "Service") && col4Vide && !col5Vide) {
        return TYPE_JONCTION_RACCORDEMENT;
    }
    
    // RACCORDEMENT -> USAGER : col1=Facility, col2=Service, col3=Cust, col4='-', col5=taux
    if (!col1Vide && contient(ligne->col2, "Service") && 
        contient(ligne->col3, "Cust") && col4Vide && !col5Vide) {
        return TYPE_RACCORDEMENT_USAGER;
    }
    
    return TYPE_INCONNU;
}

/**
 * Retourne une description du type de ligne
 */
const char* obtenirNomTypeLigne(TypeLigneCSV type) {
    switch (type) {
        case TYPE_SOURCE_USINE:
            return "Source -> Usine";
        case TYPE_USINE_NOEUD:
            return "Usine (noeud)";
        case TYPE_USINE_STOCKAGE:
            return "Usine -> Stockage";
        case TYPE_STOCKAGE_JONCTION:
            return "Stockage -> Jonction";
        case TYPE_JONCTION_RACCORDEMENT:
            return "Jonction -> Raccordement";
        case TYPE_RACCORDEMENT_USAGER:
            return "Raccordement -> Usager";
        default:
            return "Inconnu";
    }
}

// ============================================================================
// EXTRACTION DE DONNÉES
// ============================================================================

/**
 * Extrait l'ID de l'usine depuis col1
 */
int extraireIDUsine(const LigneCSV* ligne, char* buffer) {
    if (ligne == NULL || buffer == NULL) {
        return 0;
    }
    
    if (estChampVide(ligne->col1)) {
        return 0;
    }
    
    strcpy(buffer, ligne->col1);
    return 1;
}

/**
 * Extrait l'ID amont depuis col2
 */
int extraireIDAmont(const LigneCSV* ligne, char* buffer) {
    if (ligne == NULL || buffer == NULL) {
        return 0;
    }
    
    strcpy(buffer, ligne->col2);
    return 1;
}

/**
 * Extrait l'ID aval depuis col3
 */
int extraireIDAval(const LigneCSV* ligne, char* buffer) {
    if (ligne == NULL || buffer == NULL) {
        return 0;
    }
    
    if (estChampVide(ligne->col3)) {
        return 0;
    }
    
    strcpy(buffer, ligne->col3);
    return 1;
}

/**
 * Extrait le volume depuis col4
 */
int extraireVolume(const LigneCSV* ligne, float* volume) {
    if (ligne == NULL || volume == NULL) {
        return 0;
    }
    
    return convertirEnFloat(ligne->col4, volume);
}

/**
 * Extrait le taux de fuite depuis col5
 */
int extraireTauxFuite(const LigneCSV* ligne, float* taux) {
    if (ligne == NULL || taux == NULL) {
        return 0;
    }
    
    return convertirEnFloat(ligne->col5, taux);
}

// ============================================================================
// STATISTIQUES
// ============================================================================

/**
 * Initialise les statistiques de lecture
 */
void initialiserStatistiques(StatistiquesLecture* stats) {
    if (stats == NULL) {
        return;
    }
    
    stats->lignesLues = 0;
    stats->lignesValides = 0;
    stats->erreursParsing = 0;
}

/**
 * Affiche les statistiques de lecture
 */
void afficherStatistiques(const StatistiquesLecture* stats) {
    if (stats == NULL) {
        return;
    }
    
    printf("=== Statistiques de lecture CSV ===\n");
    printf("Lignes lues : %d\n", stats->lignesLues);
    printf("Lignes valides : %d\n", stats->lignesValides);
    printf("Erreurs de parsing : %d\n", stats->erreursParsing);
    printf("===================================\n");
}

// ============================================================================
// DEBUG
// ============================================================================

/**
 * Affiche une ligne CSV (pour debug)
 */
void afficherLigneCSV(const LigneCSV* ligne) {
    if (ligne == NULL) {
        printf("Ligne NULL\n");
        return;
    }
    
    printf("Col1: [%s]\n", ligne->col1);
    printf("Col2: [%s]\n", ligne->col2);
    printf("Col3: [%s]\n", ligne->col3);
    printf("Col4: [%s]\n", ligne->col4);
    printf("Col5: [%s]\n", ligne->col5);
    printf("Type: %s\n", obtenirNomTypeLigne(identifierTypeLigne(ligne)));
    printf("---\n");
}
