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

  if (S->nrMese == 0){
    fprintf(out, "Sala este inchisa!\n");
  }

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

void delPlayer(Masa* M, Jucator J){
  TLista L = (*M)->jucatori;

  for (; ; L = L->urm){
    if (strcmp(((Jucator)(L->urm->info))->nume, J->nume) == 0){
      L->urm = L->urm->urm;
      break;
    }
  }
}

void delTable(Sala* S, Masa M){
  TLista L = (*S)->masa;

  if (strcmp(((Masa)(L->info))->numeMasa, M->numeMasa) == 0){
    (*S)->masa = L->urm;
    return;
  }

  for (; ; L = L->urm){
    if (strcmp(((Masa)(L->urm->info))->numeMasa, M->numeMasa) == 0){
      L->urm = L->urm->urm;
      break;
    }
  }
}

void noroc(Sala S, char* table, char* name, int value, FILE* out){
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    return;
  }

  Jucator J = findPlayer(M, name);
  if (J == NULL){
    fprintf(out, "Jucatorul %s nu exista la masa %s!\n", name, table);
    return;
  }

  J->nrMaini += value;
}

void ghinion(Sala S, char* table, char* name, int value, FILE* out){
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    return;
  }

  Jucator J = findPlayer(M, name);
  if (J == NULL){
    fprintf(out, "Jucatorul %s nu exista la masa %s!\n", name, table);
    return;
  }

  J->nrMaini -= value;

  if (J->nrMaini <= 0){
    M->nrCrtJucatori--;
    S->nrLocCrt--;
    delPlayer(&M, J);
  }

  if (M->nrCrtJucatori <= 0){
    S->nrMese--;
    S->nrLocMax -= M->nrMaxJucatori;
    delTable(&S, M);
  }
}

void tura(Sala S, char* table, FILE* out){
  Jucator J = NULL;
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    return;
  }

  TLista L = M->jucatori;
  TLista u = NULL;
  TLista n = L->urm;

  for (u = L->urm; u->urm != L; u = u->urm);

  u->urm = n;
  L->urm = n->urm;
  n->urm = L;

  for (u = L->urm; u != L; u = u->urm){
    J = ((Jucator)(u->info));
    J->nrMaini--;

    if (J->nrMaini <= 0){
      M->nrCrtJucatori--;
      S->nrLocCrt--;
      delPlayer(&M, J);

      if (M->nrCrtJucatori <= 0){
        S->nrLocMax -= M->nrMaxJucatori;
        S->nrMese--;
        delTable(&S, M);
      }
    }

  }
}

void turaCompleta(Sala S, FILE* out){
  TLista L = S->masa;
  TLista p = NULL;
  Masa M = NULL;

  for (p = L; p != NULL; p = p->urm){
    M = ((Masa)(p->info));
    tura(S, M->numeMasa, out);
  }
}

void inchide(Sala S, char* table, FILE* out){
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    return;
  }

  //Masa mL = NULL;
  TLista L = NULL, u = NULL;
  TLista p = NULL, aux = NULL, q = NULL;
  Jucator J = NULL;
  int x, i;

  if (M->nrCrtJucatori > (S->nrLocMax - S->nrLocCrt)){
    fprintf(out, "Nu exista suficiente locuri in sala!\n");
    return;
  }

  S->nrLocMax -= M->nrMaxJucatori;
  p = M->jucatori;

  for (L = S->masa; L != NULL; L = L->urm){
    if (strcmp(((Masa)(L->info))->numeMasa, table) == 0){
      continue;
    }

    x = ((Masa)L->info)->nrMaxJucatori - ((Masa)L->info)->nrCrtJucatori;

    if (x > 0){

      //parcurgere ca sa aflu ultimul jucator de la masa la care vreau sa inserez
      //p = M->jucatori; //era ((Masa)(M->info))->jucatori inainte
      q = ((Masa)(L->info))->jucatori;
      for (u = q->urm; u->urm != q; u = u->urm);

      for (i = 0; x > 0 && M->nrCrtJucatori > 0; i++){
        //printf("OHYEAH!%d",M->nrCrtJucatori);
        J = (Jucator)(p->urm->info);

        aux = AlocCelula((void*) J, sizeof(struct jucator));
        if (!aux) printf("Alocare nereusita");
        u->urm = aux;
        aux->urm = q;
        u = u->urm;

        //delPlayer(&M, J);

        M->nrCrtJucatori--;
        S->nrLocCrt--;
        x--;
        ((Masa)(L->info))->nrCrtJucatori++;
        //printf("%d ", ((Masa)L->info)->nrCrtJucatori);
        p = p->urm;
      }
    }
  }

  S->nrLocMax -= M->nrMaxJucatori;
  S->nrMese--;
  delTable(&S, M);
}

void clasament(Sala S, char* table, FILE* out){
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    return;
  }

  fprintf(out, "Clasament %s:\n", table);

  TLista rez = InitL();
  TLista p = NULL; //lista de jucatori
  TLista u = NULL, aux = NULL, inc = NULL;
  Jucator ins = (Jucator) calloc(1, sizeof(struct jucator));
  if (!ins) return;

  int count = 0, lastMaxVal = 0, curMaxValue = 0, value;
  char* last = calloc(1, NMAX);
  if (!last) return;

  inc = rez; //salvez inceputul listei
  ins->nume = (char*) calloc(1, NMAX);

  p = M->jucatori;
  curMaxValue = ((Jucator)(p->urm->info))->nrMaini;
  ins->nrMaini = curMaxValue;
  ins->nume = ((Jucator)(p->urm->info))->nume;

//fac o parcurgere pentru a determina valoarea maxima si a o insera in lista rez
  for (u = p->urm; u != p; u = u->urm){
    value = ((Jucator)(u->info))->nrMaini;
    if (value > curMaxValue){
      ins->nrMaini = value;
      ins->nume = ((Jucator)(u->info))->nume;
      curMaxValue = value;
    }
    else if (value == curMaxValue){
      if (strcmp(((Jucator)(u->info))->nume, ins->nume) > 0){
        ins->nume = ((Jucator)(u->info))->nume;
      }
    }
  }
  //printf("%s %d\n", ins->nume, ins->nrMaini);

  aux = AlocCelula((void*) ins, sizeof(struct jucator));
  rez->urm = aux;
  aux->urm = inc;
  rez = rez->urm;
  count++;

  while (count < M->nrCrtJucatori){
    lastMaxVal = curMaxValue;
    curMaxValue = 0;
    last = ins->nume;

    for (u = p->urm; u != p; u = u->urm){
      value = ((Jucator)(u->info))->nrMaini;
      //printf("%d ", M->nrCrtJucatori);
      if ((value < lastMaxVal) && (value > curMaxValue)){
        curMaxValue = value;
        ins->nrMaini = value;
        ins->nume = ((Jucator)(u->info))->nume;
      }
      else if ((value == curMaxValue) && (curMaxValue != lastMaxVal)){
        if ((strcmp(((Jucator)(u->info))->nume, ins->nume)) > 0){
          //printf("%s %s\n", table, ins->nume);
          ins->nume = ((Jucator)(u->info))->nume;
          ins->nrMaini = value;
        }
      }
      else if (value == lastMaxVal){
        if ((strcmp(((Jucator)(u->info))->nume, last)) < 0){
          if (value == curMaxValue){
            if ((strcmp(((Jucator)(u->info))->nume, ins->nume)) > 0){
              //printf("%s %s\n", table, ins->nume);
              //printf("%s %s\n", ((Jucator)(u->info))->nume, ins->nume);
              ins->nume = ((Jucator)(u->info))->nume;
              ins->nrMaini = value;
            }
          }
          else{
            ins->nume = ((Jucator)(u->info))->nume;
            ins->nrMaini = value;
            curMaxValue = value;
          }
        }
      }
    }
    //printf("%s %s\n", table, ins->nume);
    aux = AlocCelula((void*) ins, sizeof(struct jucator));
    rez->urm = aux;
    aux->urm = inc;
    rez = rez->urm;
    count++;
  }

  for (u = inc->urm; u != inc; u = u->urm){
    fprintf(out, "%s %d\n", ((Jucator)(u->info))->nume, ((Jucator)(u->info))->nrMaini);
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
  else if (strcmp(cmd_name, "noroc") == 0){
    char* table_str = strtok(NULL, delims);
    if (!table_str){
      goto invalid_command;
    }
    char* name_str = strtok(NULL, delims);
    if (!name_str){
      goto invalid_command;
    }
    char* value_str = strtok(NULL, "\n");
    if (!value_str){
      value_str = "0";
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
  else if (strcmp(cmd_name, "tura") == 0){
    char* table_str = strtok(NULL, delims);
    if (!table_str){
      goto invalid_command;
    }
    tura(S, table_str, out);
  }
  else if (strcmp(cmd_name, "tura_completa") == 0){
    turaCompleta(S, out);
  }
  else if (strcmp(cmd_name, "inchide") == 0){
    char* table_str = strtok(NULL, delims);
    if (!table_str){
      goto invalid_command;
    }
    inchide(S, table_str, out);
  }
  else if (strcmp(cmd_name, "clasament") == 0){
    char* table_str = strtok(NULL, delims);
    if (!table_str){
      goto invalid_command;
    }
    clasament(S, table_str, out);
  }
  else{
    goto invalid_command;
  }

  return;

  invalid_command:
    printf("Invalid command: %s\n", cmd);
    return;
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
