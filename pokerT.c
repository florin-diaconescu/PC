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
  TLista L = S->masa; //

  for (i = 0; i < S->nrMese; i++){
    M = (Masa)L->info; //M = (Masa)S->masa->info;
    fprintf(out, "%s: ", M->numeMasa);
    AfiLista(M->jucatori, AfiJucator, out);
    fprintf(out, ".\n");
    L = L->urm;
  }
}

//returneaza adresa mesei cautate sau NULL in caz ca nu o gaseste
Masa findTable(Sala S, char* x){
  Masa aux = NULL;
  TLista L = S->masa;
  char* tableName = NULL;

  for(; L != NULL; L = L->urm){
    tableName = ((Masa)(L->info))->numeMasa;
    if (strcmp(tableName, x) == 0){
      aux = (Masa)(L->info);
      break;
    }
  }

  return aux;
}

Jucator findPlayer(Masa M, char* x){
  Jucator aux = NULL;
  TLista L = M->jucatori;
  TLista inc = L;
  char* name = NULL;

  for (L = L->urm; L != inc; L = L->urm){
    name = ((Jucator)(L->info))->nume;
    if (strcmp(name, x) == 0){
      aux = (Jucator)(L->info);
      break;
    }
  }

  return aux;
}

void noroc(Sala S, char* table, char* name, int value, FILE* out){
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    exit(1);
  }

  Jucator J = findPlayer(M, name);
  if (J == NULL){
    fprintf(out, "Jucatorul %s nu exista la masa %s!\n", name, table);
    exit(1);
  }

  J->nrMaini += value;
}

void ghinion(Sala S, char* table, char* name, int value, FILE* out){
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    exit(1);
  }

  Jucator J = findPlayer(M, name);
  if (J == NULL){
    fprintf(out, "Jucatorul %s nu exista la masa %s!\n", name, table);
    exit(1);
  }

  J->nrMaini -= value;
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
  else if (strcmp(cmd_name, "noroc") == 0){
    char* table_str = strtok(NULL, delims);
    if (!table_str){
      goto invalid_command;
    }
    char* name_str = strtok(NULL, delims);
    if (!name_str){
      goto invalid_command;
    }
    char* value_str = strtok(NULL, delims);
    if (!value_str){
      goto invalid_command;
    }
    int value = atoi(value_str);
    noroc(S, table_str, name_str, value, out);
  }
  else if (strcmp(cmd_name, "ghinion") == 0){
    char* table_str = strtok(NULL, delims);
    if (!table_str){
      goto invalid_command;
    }
    char* name_str = strtok(NULL, delims);
    if (!name_str){
      goto invalid_command;
    }
    char* value_str = strtok(NULL, delims);
    if (!value_str){
      goto invalid_command;
    }
    int value = atoi(value_str);
    ghinion(S, table_str, name_str, value, out);
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
