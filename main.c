#include "main.h"
#include "fichier.h"
#include "repertoire.h"

FILE* disque;
FILE* ficBlocs;
FILE* repertoire;
FILE* fichier;
char chemin[LONGUEUR_MAX + 1];
char carac;

int main(int argc, char** argv) {
  char commande[LONGUEUR_MAX + 1];

  verifierFichiers();

  do {
    printf("%s\n", "Disque prêt, veuillez entrer votre commande:");
    obtenirString(commande);

    if (strcmp(commande, "creation_repertoire") == 0) {
      creerRepertoire();
    } else if (strcmp(commande, "suppression_repertoire") == 0) {
      supprimerRepertoire();
    } else if (strcmp(commande, "creation_fichier") == 0) {
      creerFichier();
    } else if (strcmp(commande, "suppression_fichier") == 0) {
      supprimerFichier();
    } else if (strcmp(commande, "lire_fichier") == 0) {
      lireFichier();
    } else if (strcmp(commande, "fin") != 0) {
      printf("%s\n", "Commande erronée.");
    }
    if (carac != '\n') {
      while (fgetc(stdin) != '\n');
    }
  } while (strcmp(commande, "fin") != 0);

  return 0;
}

void obtenirString(char* string) {
  int compteur = 0;
  boolean finString = FALSE;

  do {
    carac = fgetc(stdin);
    compteur++;
    if (carac == ' ' || carac == '\n' || compteur > LONGUEUR_MAX + 1) {
      *string = '\0';
      finString = TRUE;
    } else {
      *string = carac;
      string++;
    }
  } while (!finString);
}

void verifierFichiers() {
  int i;
  int bloc[1];

  disque = fopen("disque", "rb+");
  if (disque == NULL) {
    disque = fopen("disque", "wb+");
    fseek(disque, ESPACE_DISQUE - 1, SEEK_SET);
    fwrite("", sizeof (char), 1, disque);
  }
  fclose(disque);

  ficBlocs = fopen("blocs", "rb+");
  if (ficBlocs == NULL) {
    bloc[0] = 0;
    ficBlocs = fopen("blocs", "wb+");
    for (i = 0; i < ESPACE_DISQUE / GROSSEUR_BLOC; i++) {
      fwrite(bloc, sizeof (int), 1, ficBlocs);
    }
  }
  fclose(ficBlocs);

  repertoire = fopen("repertoire", "rb+");
  if (repertoire == NULL) {
    repertoire = fopen("repertoire", "wb+");
    nouveauRepertoire("/");
  }
  fclose(repertoire);

  fichier = fopen("fichier", "rb+");
  if (fichier == NULL) {
    fichier = fopen("fichier", "wb+");
  }
  fclose(fichier);
}