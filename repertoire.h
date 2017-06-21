typedef struct {
  char nom[LONGUEUR_MAX + 1];
  int sousRep[MAX_REPERTOIRE];
  int fichier[MAX_REPERTOIRE];
  int index;
  int suivant;
} Repertoire;

void creerRepertoire();
void supprimerRepertoire();
int nouveauRepertoire(char*);
int validerChemin(char*, char*, Repertoire*, type);
int repertoireExiste(char*, Repertoire*);
int fichierExiste(char*, Repertoire*);
void ajouterFichier(Inode*, Repertoire*);
void ajouterRepertoire(Repertoire*, char*);
void creerRepertoireSuivant(Repertoire*);
Repertoire* obtenirRepRacine();
