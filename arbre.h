#ifndef ARBRE_H
#define ARBRE_H

// ============================================================================
// CONSTANTES
// ============================================================================

#define CAPACITE_INITIALE_ENFANTS 4    // Capacité initiale du tableau d'enfants

// Types de nœuds dans le réseau
#define TYPE_USINE         'U'
#define TYPE_STOCKAGE      'S'
#define TYPE_JONCTION      'J'
#define TYPE_RACCORDEMENT  'R'
#define TYPE_USAGER        'C'  // Customer

typedef struct Noeud {
    char identifiant[80];           // Identifiant unique du nœud
    char type;                      // Type de nœud (U/S/J/R/C)
    double volumeEntrant;           // Volume d'eau entrant (k.m³)
    float tauxFuite;                // Taux de fuite en % (0.0 à 100.0)
    struct Noeud** enfants;         // Tableau dynamique de pointeurs vers enfants
    int nbEnfants;                  // Nombre actuel d'enfants
    int capaciteEnfants;            // Capacité allouée pour les enfants
} Noeud;

typedef struct {
    Noeud* racine;                  // Racine (usine recherchée)
} ArbreDistribution;

Noeud* creerNoeud(const char* identifiant, char type, float tauxFuite);

ArbreDistribution* creerArbreDistribution();

void libererNoeud(Noeud* noeud);

void libererArbre(ArbreDistribution* arbre);

int ajouterEnfant(Noeud* parent, Noeud* enfant);

int agrandirCapaciteEnfants(Noeud* noeud);

double calculerFuitesRecursif(Noeud* noeud, double volumeEntrant);

void trouverFuiteMaximale(Noeud* noeud, double volumeEntrant, double* maxFuite, char* idAmont, char* idAval);

int compterNoeudsArbre(Noeud* noeud);

int hauteurArbre(Noeud* noeud);

void afficherArbre(Noeud* noeud, int niveau);

void afficherStatistiquesArbre(ArbreDistribution* arbre);

const char* obtenirNomType(char type);

#endif /* ARBRE_H */
