#include "main.h"
#include "fichier.h"
#include "repertoire.h"

void creerRepertoire() {
  char nom[LONGUEUR_MAX + 1];
  Repertoire* repCourant;
  int index;

  obtenirString(chemin);

  if (strlen(chemin) == LONGUEUR_MAX + 1) {
    printf("%s\n", "Le chemin absolu est trop long.");

    return;
  }

  repCourant = obtenirRepRacine();
  index = validerChemin(chemin, nom, repCourant, R);

  if (index >= 0) {
    printf("%s\n", "Le répertoire existe déjà.");
  } else if (index == -1) {
    ajouterRepertoire(repCourant, nom);
  }

  free(repCourant);
  fclose(repertoire);
}

void supprimerRepertoire() {
  Repertoire* repCourant;
  char nom[LONGUEUR_MAX + 1];
  int i = 0;
  int index;
  boolean trouve = FALSE;

  obtenirString(chemin);

  repCourant = obtenirRepRacine();
  index = validerChemin(chemin, nom, repCourant, R);

  if (index >= 0) {
    do {
      if (repCourant -> sousRep[i] == index) {
        repCourant -> sousRep[i] = -1;

        fseek(repertoire, repCourant -> sousRep[index] * sizeof (Repertoire), SEEK_SET);
        fwrite(repCourant, sizeof (Repertoire), 1, repertoire);

        trouve = TRUE;
      }

      i++;
    } while (trouve == FALSE);
  } else if (index == -1) {
    printf("%s\n", "Le répertoire n'existe pas.");
  }

  free(repCourant);
  fclose(repertoire);
}

int nouveauRepertoire(char * nom) {
  Repertoire* rep;
  int i;
  int index;

  rep = malloc(sizeof (Repertoire));

  strcpy(rep -> nom, nom);

  for (i = 0; i < MAX_REPERTOIRE; i++) {
    rep -> sousRep[i] = -1;
    rep -> fichier[i] = -1;
  }

  rep -> suivant = -1;

  fseek(repertoire, 0, SEEK_END);

  index = ftell(repertoire) / sizeof (Repertoire);
  rep -> index = index;

  fwrite(rep, sizeof (Repertoire), 1, repertoire);
  free(rep);

  return index;
}

int repertoireExiste(char* sousRepertoire, Repertoire* repCourant) {
  Repertoire* repSuivant;
  boolean trouve = FALSE;
  boolean encore;
  int i = 0;

  repSuivant = malloc(sizeof (Repertoire));

  do {
    encore = FALSE;

    do {
      if (repCourant -> sousRep[i] != -1) {
        fseek(repertoire, (repCourant -> sousRep[i] * sizeof (Repertoire)), SEEK_SET);
        fread(repSuivant, sizeof (Repertoire), 1, repertoire);

        if (strcmp(repSuivant -> nom, sousRepertoire) == 0) {
          trouve = TRUE;
        }
      }

      if (!trouve) {
        i++;
      }
    } while ((trouve == FALSE) && (i < MAX_REPERTOIRE));

    if ((trouve == FALSE) && (repCourant -> suivant != -1)) {
      fseek(repertoire, (repCourant -> suivant * sizeof (Repertoire)), SEEK_SET);
      fread(repCourant, sizeof (Repertoire), 1, repertoire);

      encore = TRUE;
      i = 0;
    }
  } while (encore == TRUE);

  free(repSuivant);

  if (trouve == TRUE) {
    return i;
  } else {
    return -1;
  }
}

int validerChemin(char* chemin, char* nom, Repertoire* repCourant, type type) {
  boolean finString;
  boolean finChemin = FALSE;
  int i = 0;
  int j;
  int index;

  do {
    finString = FALSE;
    j = 0;

    do {
      nom[j] = chemin[i];

      i++;
      j++;

      if ((chemin[i] == '/') || (chemin[i] == '\0')) {
        finString = TRUE;
        nom[j] = '\0';
      }
    } while (finString == FALSE);

    if (chemin[i] == '/') {
      index = repertoireExiste(nom, repCourant);

      if (index == -1) {
        printf("%s\n", "Répertoire parent inexistant.");

        index = -2;
        finChemin = TRUE;
      } else {
        fseek(repertoire, repCourant -> sousRep[index] * sizeof (Repertoire), SEEK_SET);
        fread(repCourant, sizeof (Repertoire), 1, repertoire);
      }
    } else {
      if (type == R) {
        index = repertoireExiste(nom, repCourant);
      } else {
        index = fichierExiste(nom, repCourant);
      }

      finChemin = TRUE;
    }
  } while (finChemin == FALSE);

  return index;
}

int fichierExiste(char* nom, Repertoire* repCourant) {
  Inode* inode;
  boolean trouve = FALSE;
  boolean encore;
  int i = 0;

  inode = malloc(sizeof (Inode));
  fichier = fopen("fichier", "rb+");

  do {
    encore = FALSE;

    do {
      if (repCourant -> fichier[i] != -1) {
        fseek(fichier, (repCourant -> fichier[i] * sizeof (Inode)), SEEK_SET);
        fread(inode, sizeof (Inode), 1, fichier);

        if (strcmp(inode -> nom, nom) == 0) {
          trouve = TRUE;
        }
      }

      if (!trouve) {
        i++;
      }
    } while ((trouve == FALSE) && (i < MAX_REPERTOIRE));

    if ((trouve == FALSE) && (repCourant -> suivant != -1)) {
      fseek(repertoire, (repCourant -> suivant * sizeof (Repertoire)), SEEK_SET);
      fread(repCourant, sizeof (Repertoire), 1, repertoire);

      encore = TRUE;
      i = 0;
    }
  } while (encore == TRUE);

  free(inode);

  if (trouve == TRUE) {
    return i;
  } else {
    return -1;
  }
}

void ajouterFichier(Inode* inode, Repertoire* repCourant) {
  boolean indexTrouve = FALSE;
  int i = 0;

  do {
    if (repCourant -> fichier[i] == -1) {
      indexTrouve = TRUE;
    } else {
      i++;

      if (i == MAX_REPERTOIRE) {
        creerRepertoireSuivant(repCourant);

        i = 0;
        indexTrouve = TRUE;
      }
    }
  } while (indexTrouve == FALSE);

  fseek(fichier, 0, SEEK_END);

  repCourant -> fichier[i] = ftell(fichier) / sizeof (Inode);

  fwrite(inode, sizeof (Inode), 1, fichier);
  fseek(repertoire, repCourant -> index * sizeof (Repertoire), SEEK_SET);
  fwrite(repCourant, sizeof (Repertoire), 1, repertoire);
}

void ajouterRepertoire(Repertoire* repCourant, char* nom) {
  boolean indexTrouve = FALSE;
  int i = 0;

  do {
    if (repCourant -> sousRep[i] == -1) {
      indexTrouve = TRUE;
    } else {
      i++;

      if (i == MAX_REPERTOIRE) {
        creerRepertoireSuivant(repCourant);

        i = 0;
        indexTrouve = TRUE;
      }
    }
  } while (indexTrouve == FALSE);

  repCourant -> sousRep[i] = nouveauRepertoire(nom);

  fseek(repertoire, repCourant -> index * sizeof (Repertoire), SEEK_SET);
  fwrite(repCourant, sizeof (Repertoire), 1, repertoire);
}

void creerRepertoireSuivant(Repertoire* repCourant) {
  repCourant -> suivant = nouveauRepertoire(repCourant -> nom);
  fseek(repertoire, repCourant -> index * sizeof (Repertoire), SEEK_SET);
  fwrite(repCourant, sizeof (Repertoire), 1, repertoire);
  fseek(repertoire, repCourant -> suivant * sizeof (Repertoire), SEEK_SET);
  fread(repCourant, sizeof (Repertoire), 1, repertoire);
}

Repertoire* obtenirRepRacine() {
  Repertoire* repCourant;

  repertoire = fopen("repertoire", "rb+");
  repCourant = malloc(sizeof (Repertoire));
  fread(repCourant, sizeof (Repertoire), 1, repertoire);
  return repCourant;
}

