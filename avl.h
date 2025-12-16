#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NoeudAVL {    // Structure d'un noeud AVL représentant une usine de traitement d'eau  
    char identifiant[100];           // ID de l'usine
    double volumeMax;                // Capacité max (k.m³)
    double volumeCapte;              // Volume total capté
    double volumeTraite;             // Volume réellement traité
    int equilibre;                   // Facteur d'équilibre
    struct NoeudAVL* gauche;        // Pointeur vers le sous-arbre gauche
    struct NoeudAVL* milieu;        // Pointeur vers le sous-arbre du milieu 
    struct NoeudAVL* droit;         // Pointeur vers le sous-arbre droit
} NoeudAVL;             

typedef struct {        // Structure de l'arbre AVL
    NoeudAVL* racine;
} AVL;


typedef struct {      // Version optimisée pour réduire la taille mémoire
    char identifiant[80];      // Au lieu de 200
    float volumeMax;           // float au lieu de double (4 vs 8 octets)
    float volumeCapte;         // Précision suffisante
    float volumeTraite;
    signed char equilibre;     // Au lieu de int (-128 à 127 suffit)
    struct NoeudAVL* gauche;
    struct NoeudAVL* droit;
} NoeudAVL_Optimise;

typedef struct {        // Structure de l'arbre AVL optimisé
    NoeudAVL_Optimise* racine;
} AVL_Optimise;


typedef struct Noeud {    // Structure d'un noeud dans l'arbre de distribution d'eau
    char identifiant[100];
    char type;                    // 'U'sine, 'S'tockage, 'J'onction, 'R'accordement
    double volumeEntrant;         // Volume qui entre dans ce nœud
    double tauxFuite;             // Pourcentage de fuite (%)
    struct Noeud** enfants;       // Tableau dynamique de pointeurs
    int nbEnfants;
    int capaciteEnfants;          // Capacité allouée
} Noeud;

typedef struct {
    Noeud* racine;                // L'usine recherchée
} ArbreDistribution;


typedef struct ListeNoeud {   // Liste chaînée pour les enfants dans l'arbre de distribution
    Noeud* noeud;
    struct ListeNoeud* suivant;
} ListeNoeud;

typedef struct Noeud {       // Structure d'un noeud dans l'arbre de distribution d'eau avec liste chaînée
    char identifiant[100];
    char type;
    double volumeEntrant;
    double tauxFuite;
    ListeNoeud* enfants;          // Liste chaînée d'enfants
} Noeud;


// Structure pour l'AVL de recherche
typedef struct NoeudAVLRecherche {
    char identifiant[100];
    Noeud* adresseNoeud;          // Pointeur vers le nœud dans l'arbre
    int equilibre;
    struct NoeudAVLRecherche* gauche;
    struct NoeudAVLRecherche* droit;
} NoeudAVLRecherche;

NoeudAVL* creerNoeudAVl(const char* identifiant);

AVL* creerAVL();

NoeudAVL* rotationGAuche(NoeudAVL* a);

NoeudAVL* rotationDroite(NoeudAVL* a);

NoeudAVL* doubleRotationGauche(NoeudAVL* a);

NoeudAVL* doubleRotationDroite(NoeudAVL* a);

NoeudAVL* equilibrerAVL(NoeudAVL* a);

NoeudAVL* insererAVL_rec(NoeudAVL* racine, const char* identifiant, int* h);

int insererAVL(AVL* avl, const char* identifiant);

NoeudAVL* rechercherAVL_rec(NoeudAVL* racine, const char* identifiant);

NoeudAVL* rechercherAVL(AVL* avl, const char* identifiant);

NoeudAVL* rechercherUsine(AVL* avl, const char* identifiant);

void ajouterVolumeCapte(NoeudAVL* noeud, float volume);

void ajouterVolumeTraite(NoeudAVL* noeud, float volume);

void definirVolumeMax(NoeudAVL* noeud, float volume);

void parcoursInfixe(NoeudAVL* racine, void (*callback)(NoeudAVL*, void*), void* data);
    
void parcoursInfixeInverse(NoeudAVL* racine, void (*callback)(NoeudAVL*, void*), void* data);

//Callback pour écrire un nœud dans un fichier (format mode max)
static void ecrireNoeudMax(NoeudAVL* noeud, void* data);

//Callback pour écrire un nœud dans un fichier (format mode src)
static void ecrireNoeudSrc(NoeudAVL* noeud, void* data);


//Callback pour écrire un nœud dans un fichier (format mode real)
static void ecrireNoeudReal(NoeudAVL* noeud, void* data);

int ecrireAVLDansFichier(AVL* avl, FILE* fichier, const char* mode);

int compterNoeuds(NoeudAVL* racine);

int hauteurAVL(NoeudAVL* racine);

void afficherStatistiquesAVL(AVL* avl);

void libererAVL_rec(NoeudAVL* racine);

void libererAVL(AVL* avl);

static void afficherNoeud(NoeudAVL* noeud, void* data);

void afficherAVL(AVL* avl)
