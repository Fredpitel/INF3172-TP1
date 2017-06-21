#include "main.h"
#include "fichier.h"
#include "repertoire.h"

void creerFichier() {
  char nom[LONGUEUR_MAX + 1];
  char bloc[GROSSEUR_BLOC];
  Inode* inode;
  Repertoire* repCourant;
  boolean tropGros = FALSE;
  int index;

  obtenirString(chemin);

  if (verifierErreur()) {
    return;
  }

  repCourant = obtenirRepRacine();
  index = validerChemin(chemin, nom, repCourant, F);

  if (index >= 0) {
    printf("%s\n", "Un fichier avec ce nom existe déjà dans ce répertoire.");
  } else if (index == -1) {
    ficBlocs = fopen("blocs", "rb+");
    disque = fopen("disque", "rb+");
    inode = creerInode(nom);
    tropGros = lireContenu(bloc, inode -> ptr);

    if (tropGros == FALSE) {
      ajouterFichier(inode, repCourant);
      modifierFichierBlocs(inode, 1);
    }

    free(inode);
    fclose(disque);
    fclose(ficBlocs);
  }

  free(repCourant);
  fclose(repertoire);
  fclose(fichier);
}

void supprimerFichier() {
  Repertoire* repCourant;
  Inode* inode;
  char nom[LONGUEUR_MAX + 1];
  int index;

  obtenirString(chemin);

  repCourant = obtenirRepRacine();
  index = validerChemin(chemin, nom, repCourant, F);

  if (index >= 0) {
    ficBlocs = fopen("blocs", "rb+");
    inode = malloc(sizeof (Inode));

    fseek(fichier, repCourant -> fichier[index] * sizeof (Inode), SEEK_SET);
    fread(inode, sizeof (Inode), 1, fichier);
    modifierFichierBlocs(inode, 0);

    repCourant -> fichier[index] = -1;

    fseek(repertoire, repCourant -> index * sizeof (Repertoire), SEEK_SET);
    fwrite(repCourant, sizeof (Repertoire), 1, repertoire);
    free(inode);
    fclose(ficBlocs);
  } else {
    printf("%s\n", "Le fichier n'existe pas.");
  }

  free(repCourant);
  fclose(fichier);
  fclose(repertoire);
}

void lireFichier() {
  Repertoire* repCourant;
  Inode* inode;
  char nom[LONGUEUR_MAX + 1];
  char bloc[GROSSEUR_BLOC + 1];
  int* ptrIndex;
  int index;
  int i = 0;

  obtenirString(chemin);

  repCourant = obtenirRepRacine();
  index = validerChemin(chemin, nom, repCourant, F);

  if (index >= 0) {
    disque = fopen("disque", "rb+");
    inode = malloc(sizeof (Inode));
    fseek(fichier, repCourant -> fichier[index] * sizeof (Inode), SEEK_SET);
    fread(inode, sizeof (Inode), 1, fichier);
    ptrIndex = inode -> ptr;

    do {
      fseek(disque, *ptrIndex * sizeof (char[GROSSEUR_BLOC]), SEEK_SET);
      fread(bloc, sizeof (char[GROSSEUR_BLOC]), 1, disque);
      bloc[GROSSEUR_BLOC] = '\0';
      printf("%s", bloc);
      ptrIndex++;
      i++;
    } while ((*ptrIndex != -1) && (i < 592));

    printf("%s", "\n");
    free(inode);
    fclose(disque);
  } else if (index == -1) {
    printf("%s\n", "Le fichier n'existe pas.");
  }

  free(repCourant);
  fclose(repertoire);
  fclose(fichier);
}

Inode* creerInode(char* nom) {
  int i, j, k;
  Inode* inode;

  inode = malloc(sizeof (Inode));
  strcpy(inode -> nom, nom);

  for (i = 0; i < GROSSEUR_BLOC; i++) {
    inode -> ptr[i] = -1;
    inode -> indexSimple.ptr[i] = -1;
    for (j = 0; j < GROSSEUR_BLOC; j++) {
      inode -> indexDouble.indexSimple[i].ptr[j] = -1;
      for (k = 0; k < GROSSEUR_BLOC; k++) {
        inode -> indexTriple.indexDouble[i].indexSimple[j].ptr[k] = -1;
      }
    }
  }

  return inode;
}

boolean verifierErreur() {
  if (strlen(chemin) == LONGUEUR_MAX + 1) {
    printf("%s\n", "Le chemin absolu est trop long.");
    return TRUE;
  }
  if (carac == '\n') {
    printf("%s\n", "Le fichier ne peut pas être vide");
    return TRUE;
  } else {
    carac = fgetc(stdin);
    if (carac == '\n') {
      printf("%s\n", "Le fichier ne peut pas être vide");
      return TRUE;
    }
  }
  return FALSE;
}

boolean remplirBloc(char* bloc) {
  int i = 0;

  while ((carac != '\n') && (i < GROSSEUR_BLOC)) {
    bloc[i] = carac;
    i++;
    carac = fgetc(stdin);
  }

  if (i != GROSSEUR_BLOC) {
    do {
      bloc[i] = '\0';
      i++;
    } while (i < GROSSEUR_BLOC);
    return TRUE;
  }

  return FALSE;
}

void modifierFichierBlocs(Inode* inode, int modif) {
  int* ptrIndex;
  int blocCourant[1];
  int i = 0;

  ptrIndex = inode -> ptr;
  *blocCourant = modif;

  do {
    fseek(ficBlocs, *ptrIndex * sizeof (int), SEEK_SET);
    fwrite(blocCourant, sizeof (int), 1, ficBlocs);
    ptrIndex++;
    i++;
  } while ((i < 592) && (*ptrIndex != -1));
}

boolean lireContenu(char* bloc, int* ptrIndex) {
  boolean finFichier;
  int blocCourant[1];
  int indexBloc = -1;
  int i = 0;

  do {
    finFichier = remplirBloc(bloc);

    if (bloc[0] != '\0') {
      do {
        fread(blocCourant, sizeof (int), 1, ficBlocs);
        indexBloc++;

        if (indexBloc == ESPACE_DISQUE / GROSSEUR_BLOC) {
          printf("%s\n", "Le disque est plein.");
          return TRUE;
        }

        if (*blocCourant == 0) {
          *ptrIndex = indexBloc;
          fseek(disque, indexBloc * sizeof (bloc), SEEK_SET);
          fwrite(bloc, sizeof (bloc), 1, disque);
          ptrIndex++;
          i++;

          if (i == 592) {
            printf("%s\n", "Le fichier est trop gros.");

            return TRUE;
          }
        }
      } while (*blocCourant == 1);
    }
  } while (finFichier == FALSE);

  return FALSE;
}
