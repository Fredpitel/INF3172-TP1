typedef struct {
  int ptr[8];
} IndexationSimple;

typedef struct {
  IndexationSimple indexSimple[8];
} IndexationDouble;

typedef struct {
  IndexationDouble indexDouble[8];
} IndexationTriple;

typedef struct {
  char nom[LONGUEUR_MAX + 1];
  int ptr[8];
  IndexationSimple indexSimple;
  IndexationDouble indexDouble;
  IndexationTriple indexTriple;
} Inode;

void creerFichier();
void supprimerFichier();
void lireFichier();
Inode* creerInode(char*);
boolean verifierErreur();
boolean remplirBloc(char*);
void modifierFichierBlocs(Inode*, int);
boolean lireContenu(char*, int*);
