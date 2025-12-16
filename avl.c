NoeudAVL* creerNoeudAVl(const char* identifiant){
    NoeudAVL* nouveau = (NoeudAVL*)malloc(sizeof(NoeudAVL));
    if(nouveau == NULL){
        fprintf(stderr,"Erreur d'allocation mémoire pour NoeudAVL\n");
        return NULL;
    }
    
    nouveau->volumeMax = 0.0;
    nouveau->volumeCapte = 0.0;
    nouveau->volumeTraite = 0.0;
    nouveau->equilibre = 0;
    nouveau->gauche = NULL;
    nouveau->droit = NULL;
    
    return nouveau;
}

AVL* creerAVL(){
    AVL* avl = (AVL*)malloc(sizeof(AVL));
    if(AVL == NULL){
        fprintf(stderr,"Erreur d'allocation mémoire pour AVL\n");
        return NULL;
    }
    avl->racine = NULL;
    return avl;
}

NoeudAVL* rotationGAuche(NoeudAVL* a){
    if(a == NULL || a->droit == NULL){
        return a; // Pas de rotation possible
    }

    NoeudAVL* b = a->droit;   // b devient la nouvelle racine du sous-arbre (b=pivot)
    int eq_a = a->equilibre;       // Mettre à jour les facteurs d'équilibre
    int eq_b = b->equilibre;
    a->droit = b->gauche;     // Le sous-arbre gauche de b devient le sous-arbre droit de a
    b->gauche = a;            // a devient le sous-arbre gauche de b
    a->equilibre = eq_a -max(eq_b, 0)-1; // Mettre à jour les facteurs d'équilibre
    b->equilibre = min(min(eq_a -2, eq_a + eq_b -2), eq_b -1); // Mettre à jour les facteurs d'équilibre
    return b;                 // Retourner la nouvelle racine du sous-arbre
}


//claude
NoeudAVL* rotationDroite(NoeudAVL* a){
    if (a == NULL || a->gauche == NULL) {
        return a;
    }
    
    // Sauvegarder les pointeurs
    NoeudAVL* b = a->gauche;
    int eq_a = a->equilibre;
    int eq_b = b->equilibre;
    
    // Effectuer la rotation
    a->gauche = b->droit;
    b->droit = a;
    
    // Mise à jour des facteurs d'équilibre
    a->equilibre = eq_a - min(eq_b, 0) + 1;
    b->equilibre = max(eq_a + 2, max(eq_a + eq_b + 2, eq_b + 1));
    
    return b;
}

NoeudAVL* doubleRotationGauche(NoeudAVL* a){
    if (a == NULL || a->droit == NULL) {
        return a;
    }
    // Rotation droite sur le fils droit
    a->droit = rotationDroite(a->droit);
    
    // Puis rotation gauche sur le nœud
    return rotationGauche(a);
}


NoeudAVL* doubleRotationDroite(NoeudAVL* a){
    if(a == NULL || a->gauche == NULL){
        return a;
    }
    // Rotation gauche sur le fils gauche
    a->gauche = rotationGauche(a->gauche);

    // Puis rotation droite sur le nœud
    return rotationDroite(a);
}

NoeudAVL* equilibrerAVL(NoeudAVL* a){
    if(a == NULL){
        return a;
    }
    if(a->equilibre >= 2){
        if(a->droit != NULL && a->droit->equilibre >= 0){
            // Rotation gauche simple
            return rotationGauche(a);
        } else {
            // Double rotation gauche
            return doubleRotationGauche(a);
        }
    }

    else if(a->equilibre <= -2){
        if(a->gauche != NULL && a->gauche->equilibre <= 0){
            // Rotation droite simple
            return rotationDroite(a);
        } else {
            // Double rotation droite
            return doubleRotationDroite(a);
        }
    }
    
    return a; // Déjà équilibré
}


NoeudAVL* insererAVL_rec(NoeudAVL* racine, const char* identifiant, int* h){
    if(racine == NULL){
        *h = 1;
        return creerNoeudAVl(identifiant);
    }      

    // Comparer avec la racine
    int cmp = strcmp(identifiant, racine->identifiant);
    
    if (cmp < 0) {
        // Insérer à gauche
        racine->gauche = insererAVL_rec(racine->gauche, identifiant, h);
        *h = -*h;  // Inverser le signe pour la gauche
    } 
    else if (cmp > 0) {
        // Insérer à droite
        racine->droit = insererAVL_rec(racine->droit, identifiant, h);
        // *h reste positif pour la droite
    } 
    else {
        // Élément déjà présent, pas d'insertion
        *h = 0;
        return racine;
    }
    
    // Mise à jour du facteur d'équilibre si la hauteur a changé
    if (*h != 0) {
        racine->equilibre += *h;
        racine = equilibrerAVL(racine);
        
        // Si l'équilibre est 0, la hauteur du sous-arbre n'a pas changé
        if (racine->equilibre == 0) {
            *h = 0;
        } else {
            *h = 1;
        }
    }
    
    return racine;
}


int insererAVL(AVL* avl, const char* identifiant){
    if (avl == NULL || identifiant == NULL) {
        return 0;
    }
    
    int h = 0;
    NoeudAVL* nouvelle_racine = insererAVL_rec(avl->racine, identifiant, &h);
    
    if (nouvelle_racine == NULL) {
        return 0;  // Échec de l'insertion
    }
    
    avl->racine = nouvelle_racine;
    return 1;  // Succès
}

//copilot
NoeudAVL* rechercherAVL_rec(NoeudAVL* racine, const char* identifiant){
    if (racine == NULL ) {
        return NULL;  // Élément non trouvé
    }
    int cmp = strcmp(identifiant, racine->identifiant);
    if (cmp == 0) {
        return racine;  // Élément trouvé
    } 
    else if (cmp < 0) {
        return rechercherAVL_rec(racine->gauche, identifiant);  // Chercher à gauche
    } 
    else {
        return rechercherAVL_rec(racine->droit, identifiant);   // Chercher à droite
    }
}   


NoeudAVL* rechercherAVL(NoeudAVL* racine, const char* identifiant){
    if(racine == NULL || identifiant == NULL){
        return NULL;
    }
    int cmp = strcmp(identifiant, racine->identifiant);  // comparer avec la racine
    if(cmp == 0){ //trouvé
        return racine;
    }
    else if (cmp < 0) {
        // Chercher à gauche
        return rechercherAVL(racine->gauche, identifiant);
    } 
    else {
        // Chercher à droite
        return rechercherAVL(racine->droit, identifiant);
    }
}


NoeudAVL* rechercherUsine(AVL* avl, const char* identifiant) {
    if (avl == NULL) {
        return NULL;
    }
    
    return rechercherAVL(avl->racine, identifiant);
}


void ajouterVolumeCapte(NoeudAVL* noeud, float volume) {
    if (noeud != NULL) {
        noeud->volumeCapte += volume;
    }
}


void ajouterVolumeTraite(NoeudAVL* noeud, float volume) {
    if (noeud != NULL) {
        noeud->volumeTraite += volume;
    }
}


void definirVolumeMax(NoeudAVL* noeud, float volume) {
    if (noeud != NULL) {
        noeud->volumeMax = volume;
    }
}



void parcoursInfixe(NoeudAVL* racine, void (*callback)(NoeudAVL*, void*), void* data) {
    if (racine == NULL) {
        return;
    }
    
    // Parcourir le sous-arbre gauche
    parcoursInfixe(racine->gauche, callback, data);
    
    // Traiter la racine
    callback(racine, data);
    
    // Parcourir le sous-arbre droit
    parcoursInfixe(racine->droit, callback, data);
}


void parcoursInfixeInverse(NoeudAVL* racine, void (*callback)(NoeudAVL*, void*), void* data) {
    if (racine == NULL) {
        return;
    }
    
    // Parcourir le sous-arbre droit d'abord
    parcoursInfixeInverse(racine->droit, callback, data);
    
    // Traiter la racine
    callback(racine, data);
    
    // Parcourir le sous-arbre gauche
    parcoursInfixeInverse(racine->gauche, callback, data);
}

//Callback pour écrire un nœud dans un fichier (format mode max)
static void ecrireNoeudMax(NoeudAVL* noeud, void* data) {
    FILE* fichier = (FILE*)data;
    fprintf(fichier, "%s;%.2f\n", noeud->identifiant, noeud->volumeMax);
}

//Callback pour écrire un nœud dans un fichier (format mode src)
static void ecrireNoeudSrc(NoeudAVL* noeud, void* data) {
    FILE* fichier = (FILE*)data;
    fprintf(fichier, "%s;%.2f\n", noeud->identifiant, noeud->volumeCapte);
}


//Callback pour écrire un nœud dans un fichier (format mode real)
static void ecrireNoeudReal(NoeudAVL* noeud, void* data) {
    FILE* fichier = (FILE*)data;
    fprintf(fichier, "%s;%.2f\n", noeud->identifiant, noeud->volumeTraite);
}


int ecrireAVLDansFichier(AVL* avl, FILE* fichier, const char* mode) {
    if (avl == NULL || fichier == NULL || mode == NULL) {
        return 0;
    }
    
    // Écrire l'en-tête selon le mode
    if (strcmp(mode, "max") == 0) {
        fprintf(fichier, "identifier;max volume (k.m3.year-1)\n");
        parcoursInfixeInverse(avl->racine, ecrireNoeudMax, fichier);
    } 
    else if (strcmp(mode, "src") == 0) {
        fprintf(fichier, "identifier;source volume (k.m3.year-1)\n");
        parcoursInfixeInverse(avl->racine, ecrireNoeudSrc, fichier);
    } 
    else if (strcmp(mode, "real") == 0) {
        fprintf(fichier, "identifier;real volume (k.m3.year-1)\n");
        parcoursInfixeInverse(avl->racine, ecrireNoeudReal, fichier);
    } 
    else {
        fprintf(stderr, "Erreur: mode inconnu '%s'\n", mode);
        return 0;
    }
    
    return 1;
}


int compterNoeuds(NoeudAVL* racine) {
    if (racine == NULL) {
        return 0;
    }
    
    return 1 + compterNoeuds(racine->gauche) + compterNoeuds(racine->droit);
}


int hauteurAVL(NoeudAVL* racine) {
    if (racine == NULL) {
        return 0;
    }
    
    int hauteurGauche = hauteurAVL(racine->gauche);
    int hauteurDroite = hauteurAVL(racine->droit);
    
    return 1 + max(hauteurGauche, hauteurDroite);
}


void afficherStatistiquesAVL(AVL* avl) {
    if (avl == NULL) {
        printf("AVL vide ou invalide\n");
        return;
    }
    
    int nbNoeuds = compterNoeuds(avl->racine);
    int hauteur = hauteurAVL(avl->racine);
    
    printf("=== Statistiques AVL ===\n");
    printf("Nombre de nœuds : %d\n", nbNoeuds);
    printf("Hauteur : %d\n", hauteur);
    printf("========================\n");
}


void libererAVL_rec(NoeudAVL* racine) {
    if (racine == NULL) {
        return;
    }
    
    // Libérer les sous-arbres
    libererAVL_rec(racine->gauche);
    libererAVL_rec(racine->droit);
    
    // Libérer le nœud
    free(racine);
}


void libererAVL(AVL* avl) {
    if (avl == NULL) {
        return;
    }
    
    // Libérer tous les nœuds
    libererAVL_rec(avl->racine);
    
    // Libérer la structure AVL elle-même
    free(avl);
}


static void afficherNoeud(NoeudAVL* noeud, void* data) {
    (void)data;  // Paramètre non utilisé
    
    if (noeud == NULL) {
        return;
    }
    
    printf("ID: %s | Max: %.2f | Capté: %.2f | Traité: %.2f | Eq: %d\n",
           noeud->identifiant,
           noeud->volumeMax,
           noeud->volumeCapte,
           noeud->volumeTraite,
           noeud->equilibre);
}


void afficherAVL(AVL* avl) {
    if (avl == NULL) {
        printf("AVL vide ou invalide\n");
        return;
    }
    
    printf("\n=== Contenu de l'AVL (ordre alphabétique) ===\n");
    parcoursInfixe(avl->racine, afficherNoeud, NULL);
    printf("=============================================\n\n");
}
