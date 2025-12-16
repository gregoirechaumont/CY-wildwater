# Nom du programme (doit correspondre à ce qui est attendu par script.sh)
PROG = c-wire

# Liste des fichiers sources
SRC = main.c avl.c arbre.c fichier.c fuites.c usine.c utils.c

# Liste des headers (pour que make détecte les changements)
HEADERS = avl.h arbre.h fichier.h fuites.h usine.h utils.h

# Règle par défaut : compile uniquement
# (J'ai retiré l'exécution automatique car le programme nécessite des arguments)
all: $(PROG)

# Compilation du programme
# Ajout de -lm car utils.c utilise math.h (pow, round)
$(PROG): $(SRC) $(HEADERS)
	gcc -Wall -Wextra -g -o $(PROG) $(SRC) -lm

# Nettoyage
clean:
	rm -f $(PROG)
