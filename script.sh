#!/bin/bash

# c-wire.sh - Script principal du projet C-Wildwater

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Début du chronomètre
START_TIME=$(date +%s)

# Répertoires
DATA_DIR="data"
OUTPUT_DIR="output"
TMP_DIR="tmp"
SRC_DIR="src"

# Fichiers
CSV_FILE="$DATA_DIR/c-wire_v00.csv"
EXECUTABLE="./c-wire"

# Créer les répertoires nécessaires
mkdir -p "$OUTPUT_DIR" "$TMP_DIR"

# ============================================================================
# FONCTION : Afficher l'aide
# ============================================================================
function afficher_aide() {
    echo "Usage: $0 <mode> <parametre>"
    echo ""
    echo "Modes disponibles:"
    echo "  histo max   - Histogramme des capacités maximales"
    echo "  histo src   - Histogramme des volumes captés"
    echo "  histo real  - Histogramme des volumes traités"
    echo "  leaks <ID>  - Calculer les fuites d'une usine"
    echo ""
    echo "Exemples:"
    echo "  $0 histo max"
    echo "  $0 leaks \"Facility complex #RH400057F\""
}

# ============================================================================
# FONCTION : Vérifier la compilation
# ============================================================================
function verifier_compilation() {
    if [ ! -f "$EXECUTABLE" ]; then
        echo -e "${YELLOW}L'exécutable n'existe pas. Compilation...${NC}"
        make
        
        if [ $? -ne 0 ]; then
            echo -e "${RED}Erreur lors de la compilation${NC}"
            exit 1
        fi
        echo -e "${GREEN}✓ Compilation réussie${NC}"
    fi
}

# ============================================================================
# FONCTION : Vérifier les arguments
# ============================================================================
function verifier_arguments() {
    # Vérifier le nombre d'arguments
    if [ $# -lt 2 ]; then
        echo -e "${RED}Erreur: nombre d'arguments insuffisant${NC}"
        afficher_aide
        exit 1
    fi
    
    # Vérifier le fichier CSV
    if [ ! -f "$CSV_FILE" ]; then
        echo -e "${RED}Erreur: fichier CSV introuvable: $CSV_FILE${NC}"
        exit 1
    fi
    
    MODE=$1
    PARAM=$2
    
    # Vérifier le mode
    if [ "$MODE" != "histo" ] && [ "$MODE" != "leaks" ]; then
        echo -e "${RED}Erreur: mode inconnu '$MODE'${NC}"
        afficher_aide
        exit 1
    fi
    
    # Vérifier le paramètre selon le mode
    if [ "$MODE" == "histo" ]; then
        if [ "$PARAM" != "max" ] && [ "$PARAM" != "src" ] && [ "$PARAM" != "real" ]; then
            echo -e "${RED}Erreur: paramètre histogramme invalide '$PARAM'${NC}"
            afficher_aide
            exit 1
        fi
    elif [ "$MODE" == "leaks" ]; then
        if [ -z "$PARAM" ]; then
            echo -e "${RED}Erreur: identifiant d'usine requis${NC}"
            afficher_aide
            exit 1
        fi
    fi
    
    # Vérifier les arguments supplémentaires
    if [ $# -gt 2 ]; then
        echo -e "${RED}Erreur: trop d'arguments${NC}"
        afficher_aide
        exit 1
    fi
}

# ============================================================================
# FONCTION : Filtrer le CSV (optionnel)
# ============================================================================
function filtrer_csv() {
    local mode=$1
    local param=$2
    local fichier_filtre="$TMP_DIR/filtered.csv"
    
    # Exemple de filtrage (à adapter selon vos besoins)
    # Pour l'instant, on copie juste le fichier
    cp "$CSV_FILE" "$fichier_filtre"
    
    echo "$fichier_filtre"
}

# ============================================================================
# FONCTION : Traiter histogramme
# ============================================================================
function traiter_histogramme() {
    local param=$1
    
    echo -e "${GREEN}Traitement de l'histogramme ($param)...${NC}"
    
    # Appeler le programme C
    "$EXECUTABLE" histo "$param" < "$CSV_FILE"
    
    local code_retour=$?
    
    if [ $code_retour -ne 0 ]; then
        echo -e "${RED}Erreur lors du traitement (code: $code_retour)${NC}"
        exit $code_retour
    fi
    
    # Générer l'histogramme avec gnuplot
    local fichier_dat="$OUTPUT_DIR/vol_${param}.dat"
    local fichier_png="$OUTPUT_DIR/vol_${param}.png"
    
    if [ -f "$fichier_dat" ]; then
        generer_histogramme "$fichier_dat" "$fichier_png" "$param"
    else
        echo -e "${RED}Erreur: fichier .dat non généré${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ Histogramme généré: $fichier_png${NC}"
}

# ============================================================================
# FONCTION : Traiter fuites
# ============================================================================
function traiter_fuites() {
    local id_usine=$1
    
    echo -e "${GREEN}Calcul des fuites pour: $id_usine${NC}"
    
    # Appeler le programme C
    "$EXECUTABLE" leaks "$id_usine" < "$CSV_FILE"
    
    local code_retour=$?
    
    if [ $code_retour -ne 0 ]; then
        echo -e "${RED}Erreur lors du traitement (code: $code_retour)${NC}"
        exit $code_retour
    fi
    
    echo -e "${GREEN}✓ Fuites calculées et enregistrées${NC}"
}

# ============================================================================
# FONCTION : Générer histogramme avec gnuplot
# ============================================================================
function generer_histogramme() {
    local fichier_dat=$1
    local fichier_png=$2
    local mode=$3
    
    # Titre selon le mode
    case $mode in
        max)
            titre="Capacité maximale des usines"
            ylabel="Volume (k.m³/an)"
            ;;
        src)
            titre="Volume capté par les usines"
            ylabel="Volume (k.m³/an)"
            ;;
        real)
            titre="Volume traité par les usines"
            ylabel="Volume (k.m³/an)"
            ;;
    esac
    
    # Extraire top 5 et bottom 5
    local fichier_top5="$TMP_DIR/top5_${mode}.dat"
    local fichier_bottom5="$TMP_DIR/bottom5_${mode}.dat"
    local fichier_final="$TMP_DIR/final_${mode}.dat"
    
    # Top 5 (en ignorant la ligne d'en-tête)
    tail -n +2 "$fichier_dat" | sort -t';' -k2 -n -r | head -n 5 > "$fichier_top5"
    
    # Bottom 5
    tail -n +2 "$fichier_dat" | sort -t';' -k2 -n | head -n 5 > "$fichier_bottom5"
    
    # Combiner
    cat "$fichier_bottom5" "$fichier_top5" > "$fichier_final"
    
    # Générer avec gnuplot
    gnuplot <<EOF
set terminal png size 1200,800
set output "$fichier_png"
set title "$titre"
set ylabel "$ylabel"
set xlabel "Usines"
set style fill solid
set boxwidth 0.8
set datafile separator ";"
set xtics rotate by -45
plot "$fichier_final" using 2:xtic(1) with boxes notitle
EOF
}

# ============================================================================
# MAIN
# ============================================================================

# Vérifier les arguments
verifier_arguments "$@"

# Vérifier la compilation
verifier_compilation

# Traiter selon le mode
if [ "$MODE" == "histo" ]; then
    traiter_histogramme "$PARAM"
elif [ "$MODE" == "leaks" ]; then
    traiter_fuites "$PARAM"
fi

# Fin du chronomètre
END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

echo ""
echo -e "${GREEN}═══════════════════════════════════════${NC}"
echo -e "${GREEN}Durée totale: ${DURATION} secondes${NC}"
echo -e "${GREEN}═══════════════════════════════════════${NC}"

exit 0
