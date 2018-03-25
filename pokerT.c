#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "info.h"

#define NMAX 50

TLista InitL(){
  TLista aux;
  aux = (TLista) malloc(sizeof(Celula));
  if (!aux) return NULL;
  aux->info = NULL;
  aux->urm = aux;
  return aux;
}

TLista AlocCelula(void* x, size_t d){
  TLista aux;
  aux = (TLista) malloc(sizeof(Celula));
  if (!aux) return NULL;
  aux->info = malloc(d);
  if (!aux->info){
    free(aux);
    return NULL;
  }
  memcpy(aux->info, x, d);
  aux->urm = NULL;
  return aux;
}

Sala ReadCFG(FILE* config){
    Sala S = (Sala) calloc(1, sizeof(struct sala));
    if (!S) return NULL;

    TLista p = NULL, aux = NULL, inc = NULL;
    Masa M = NULL;
    Jucator J = NULL;
    int i, j;

    fscanf(config, "%d", &S->nrMese);
    S->masa = NULL;
    S->nrLocCrt = 0;
    S->nrLocMax = 0;

    for (i = 0; i < S->nrMese; i++){
      M = (Masa) calloc(1, sizeof(struct masa));
      if (!M) return NULL;

      M->numeMasa = (char*) calloc(1, NMAX);
      if (!M->numeMasa){
        free(M);
        return NULL;
      }

      fscanf(config, "%s %d %d", M->numeMasa, &M->nrCrtJucatori, &M->nrMaxJucatori);
      S->nrLocCrt += M->nrCrtJucatori;
      S->nrLocMax += M->nrMaxJucatori;
      M->jucatori = InitL();

      if (i == 0){
        S->masa = AlocCelula((void*) M, sizeof(struct masa));
        p = S->masa;
      }
      else{
        aux = AlocCelula((void*) M, sizeof(struct masa));
        p->urm = aux;
        p = p->urm;
      }

      inc = M->jucatori;

      for (j = 0; j < M->nrCrtJucatori; j++){
        J = (Jucator) calloc(1, sizeof(struct jucator));
        if (!J) return NULL;

        J->nume = (char*) calloc(1, NMAX);
//elibereaza in caz de nereusire alocare !!!

        fscanf(config, "%s %d", J->nume, &J->nrMaini);
        aux = AlocCelula((void*) J, sizeof(struct jucator));
        M->jucatori->urm = aux;
        aux->urm = inc;
        M->jucatori = M->jucatori->urm;
      }
    }

    return S;
}

void AfiJucator(void* x, FILE* out){
  Jucator J;
  J = (Jucator)x;
  fprintf(out, "%s - %d", J->nume, J->nrMaini);
}

void AfiLista(TLista L, void (*fafi)(void*, FILE*), FILE* out){
  TLista inc = L->urm;

  while (inc != L){
    fafi(inc->info, out);
    if (inc->urm != L)
      fprintf(out,"; ");
    inc = inc->urm;
  }
}

void afisareSala(Sala S, FILE* out){
  int i;
  Masa M;

  for (i = 0; i < S->nrMese; i++){
    M = (Masa)S->masa->info;
    fprintf(out, "%s: ", M->numeMasa);
    AfiLista(M->jucatori, AfiJucator, out);
    fprintf(out, ".\n");
    S->masa = S->masa->urm;
  }
}

void parseCommand(char* cmd, Sala S, FILE* out){
  const char* delims = " \n";

  char* cmd_name = strtok(cmd, delims);
  if (!cmd_name){
    goto invalid_command;
  }

  if (strcmp(cmd_name, "print") == 0){
    afisareSala(S, out);
  }
  else{
    goto invalid_command;
  }

  return;

  invalid_command:
    printf("Invalid command: %s\n", cmd);
    exit(1);
}

int main(int argc, char* argv[]){
  FILE* config = fopen(argv[1], "r");
  if (!config) return 0;

  FILE* events = fopen(argv[2], "r");
  if (!events){
    free(config);
    return 0;
  }

  FILE* out = fopen(argv[3], "w");
  if (!out){
    free(events);
    free(config);
    return 0;
  }

  ssize_t read;
  char* line = NULL;
  size_t len;

  Sala S = (Sala) malloc(sizeof(Sala));

  S = ReadCFG(config);

  while ((read = getline(&line, &len, events)) != -1){
    parseCommand(line, S, out);
  }

  free(S);
  free(line);
  fclose(config);
  fclose(events);
  fclose(out);
  return 0;
}
