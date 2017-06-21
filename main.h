#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ESPACE_DISQUE 512000
#define LONGUEUR_MAX 40
#define MAX_REPERTOIRE 50
#define GROSSEUR_BLOC 8
extern FILE* repertoire;
extern FILE* disque;
extern FILE* ficBlocs;
extern FILE* fichier;
extern char chemin[LONGUEUR_MAX + 1];
extern char carac;

typedef enum {FALSE, TRUE} boolean;

typedef enum {F, R} type;

void obtenirString(char*);
void verifierFichiers();
