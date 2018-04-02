/* DIACONESCU Florin - 312CB */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "info.h"

//definesc lungimea maxima pentru numele jucatorilor citite din fisier
#define NMAX 50

/*functia distruge o masa, primind ca parametru adresa listei respective
corespunzatoare */
void DistrugeMasa(TLista L){
  Masa M = ((Masa)((L)->info));

  /*variabile auxiliare, folosite pentru a face codul mai readable (mai putine
  casturi */
  TLista J = M->jucatori;
  TLista p = J->urm, aux = NULL;
  //eliberez char* asociat numelui mesei
  free(M->numeMasa);

  /*pornind de la primul jucator (cel dupa santinela), eliberez memoria celulei
  din lista corespnzatoare*/
  while (p != J){
    aux = p;
    p = p->urm;
    free(((Jucator)(aux->info))->nume);
    free (aux->info);
    free(aux);
  }
  //eliberez santinela
  free(J);
}

//functie pentru initializarea listei de jucatori (circulara cu santinela)
TLista InitL(){
  TLista aux;
  aux = (TLista) malloc(sizeof(Celula));
  if (!aux) return NULL;
  aux->info = NULL;
  aux->urm = aux;
  return aux;
}

/*functie pentru alocarea unei celule (generica), atat pentru elemente de tip
Jucator, cat si de tip Masa*/
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

/*functie pentru initializarea salii de joc, citind informatiile din configuratie
si alocand memoria, iar mai apoi eliberand-o pe cea pentru variabile auxiliare*/
Sala ReadCFG(FILE* config){
    Sala S = (Sala) calloc(1, sizeof(struct sala));
    if (!S) return NULL;

    //variabile auxiliare
    TLista p = NULL, aux = NULL, inc = NULL;
    Masa M = NULL;
    Jucator J = NULL;
    int i, j;

    //initializarea campurilor pentru gestiunea numarului de jucatori din sala
    fscanf(config, "%d", &S->nrMese);
    S->masa = NULL;
    S->nrLocCrt = 0;
    S->nrLocMax = 0;

    //citirea pe rand a informatiilor din fiecare masa din sala
    for (i = 0; i < S->nrMese; i++){
      M = (Masa) calloc(1, sizeof(struct masa));
      if (!M) return NULL;

      M->numeMasa = (char*) calloc(1, NMAX);
      if (!M->numeMasa){
        free(M);
        return NULL;
      }

      /*actualizez numarul curent si maxim de jucatori prin numarul corespunzator
      asociat fiecarei mese*/
      fscanf(config, "%s %d %d", M->numeMasa, &M->nrCrtJucatori, &M->nrMaxJucatori);
      S->nrLocCrt += M->nrCrtJucatori;
      S->nrLocMax += M->nrMaxJucatori;

      //initializez lista de jucatori (atasandu-i santinela)
      M->jucatori = InitL();

      /*leg celulele din lista de mese, luand separat cazurile in care masa
      citita este prima din sala sau oricare alta*/
      if (i == 0){
        S->masa = AlocCelula((void*) M, sizeof(struct masa));
        p = S->masa;
      }
      else{
        aux = AlocCelula((void*) M, sizeof(struct masa));
        p->urm = aux;
        p = p->urm;
      }

      //salvez santinela din lista de jucatori in variabila inc
      inc = M->jucatori;

      //realizez citirea listei de jucatori corespunzatoare fiecarei mese
      for (j = 0; j < M->nrCrtJucatori; j++){
        J = (Jucator) calloc(1, sizeof(struct jucator));
        if (!J) return NULL;

        J->nume = (char*) calloc(1, NMAX);
        if (!J->nume){
          free(J);
          free(J->nume);
        }

        fscanf(config, "%s %d", J->nume, &J->nrMaini);

        //aloc o celula cu elemente de tip Jucator si fac legaturile in lista
        aux = AlocCelula((void*) J, sizeof(struct jucator));
        M->jucatori->urm = aux;
        aux->urm = inc;
        M->jucatori = M->jucatori->urm;

        //eliberez memoria alocata pentru variabilele alocate la fiecare iteratie
        free(J);
      }
      free(M);
    }

    //intorc adresa salii formate pe parcursul functiei
    return S;
}

//functie generica pentru afisarea unui jucator conform cerintei
void AfiJucator(void* x, FILE* out){
  Jucator J;
  J = (Jucator)x;
  fprintf(out, "%s - %d", J->nume, J->nrMaini);
}

//functie pentru afisarea unei liste (folosita pentru jucatori)
void AfiLista(TLista L, void (*fafi)(void*, FILE*), FILE* out){
  TLista inc = L->urm;

  while (inc != L){
    fafi(inc->info, out);
    if (inc->urm != L)
      fprintf(out,"; ");
    inc = inc->urm;
  }
}

//functie pentru afisarea configuratiei salii, corespunzatoare comenzii print
void afisareSala(Sala S, FILE* out){
  int i;
  Masa M;
  TLista L = S->masa; //

  if (S->nrMese == 0){
    fprintf(out, "Sala este inchisa!\n");
  }

  /*parcurg lista de mese si afisez informatiile conform cerintei, mai intai
  prin afisarea numelui si mai apoi apeland functia de afisare lista*/
  for (i = 0; i < S->nrMese; i++){
    M = (Masa)L->info;
    fprintf(out, "%s: ", M->numeMasa);
    AfiLista(M->jucatori, AfiJucator, out);
    fprintf(out, ".\n");
    L = L->urm;
  }
}

//functie care returneaza adresa mesei cautate sau NULL in caz ca nu o gaseste
Masa findTable(Sala S, char* x){
  Masa aux = NULL;
  TLista L = S->masa;
  char* tableName = NULL;

  /*parcurg lista de mese pana gasesc masa cu campul numeMasa identic cu x,
  trimis ca parametru (adica numele mesei cautate)*/
  for(; L != NULL; L = L->urm){
    tableName = ((Masa)(L->info))->numeMasa;
    if (strcmp(tableName, x) == 0){
      aux = (Masa)(L->info);
      break;
    }
  }

  return aux;
}

/*functie similara findTable, dar in care returnez fie adresa jucatorului cautat,
in cazul in care este gasit la masa respectiva, fie NULL in cazul in care nu
exista*/
Jucator findPlayer(Masa M, char* x){
  Jucator aux = NULL;
  TLista L = M->jucatori;
  TLista inc = L;
  char* name = NULL;

  /*parcurg lista de jucatori pana gasesc jucatorul cu campul nume identic cu x,
  trimis ca parametru (adica numele jucatorului cautat)*/
  for (L = L->urm; L != inc; L = L->urm){
    name = ((Jucator)(L->info))->nume;
    if (strcmp(name, x) == 0){
      aux = (Jucator)(L->info);
      break;
    }
  }

  return aux;
}

/*functie care sterge un jucator de la masa, eliberand memoria aferenta si
refacand legaturile din lista de jucatori de la masa trimisa ca parametru*/
void delPlayer(Masa* M, Jucator J){
  TLista L = (*M)->jucatori;
  TLista aux;

  for (; ; L = L->urm){
    if (strcmp(((Jucator)(L->urm->info))->nume, J->nume) == 0){
      free(((Jucator)(L->urm->info))->nume);
      free((Jucator)(L->urm->info));
      aux = L->urm;
      L->urm = L->urm->urm;
      free(aux);
      break;
    }
  }
}

/*functie similara delPlayer, care sterge o masa din sala, eliberand memoria
aferenta si refacand legaturile din lista de mese de la sala, trimisa ca
parametru*/
void delTable(Sala* S, Masa M){
  TLista L = (*S)->masa;
  TLista aux;

  //cazul in care masa care se doreste a fi stearsa este chiar prima din sala
  if (strcmp(((Masa)(L->info))->numeMasa, M->numeMasa) == 0){
    //eliberez prima data santinela din lista de jucatori
    free(((Masa)(L->info))->jucatori);

    aux = (*S)->masa;
    (*S)->masa = aux->urm;

    free(((Masa)(aux->info))->numeMasa);
    free((Masa)(aux->info));
    free(aux);

    return;
  }

  /*in cazul in care masa care urmeaza sa fie stearsa este oricare alta decat
  prima*/
  for (; ; L = L->urm){
    if (strcmp(((Masa)(L->urm->info))->numeMasa, M->numeMasa) == 0){
      //ca si in celalalt caz, eliberez prima data santinela din lista de jucatori
      free(((Masa)(L->urm->info))->jucatori);

      free(((Masa)(L->urm->info))->numeMasa);
      free((Masa)(L->urm->info));

      aux = L->urm;
      L->urm = L->urm->urm;
      free(aux);

      break;
    }
  }
}

/*functie corespunzatoare comenzii noroc, prin care se incrementeaza valoarea
de la campul nrMaini cu valoarea "value" trimisa ca parametru*/
void noroc(Sala S, char* table, char* name, int value, FILE* out){
  //gasesc masa in lista de mese
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    return;
  }

  //gasesc jucatorul in cadrul mesei identificate anterior
  Jucator J = findPlayer(M, name);
  if (J == NULL){
    fprintf(out, "Jucatorul %s nu exista la masa %s!\n", name, table);
    return;
  }

  J->nrMaini += value;
}

/*functie corespnzatoare comenzii ghinion, prin care se decrementeaza valoarea
de la campul nrMaini cu valoarea "value" trimisa ca parametru, iar mai apoi
trateaza cazurile extreme, in care trebuie sters un jucator sau o masa*/
void ghinion(Sala S, char* table, char* name, int value, FILE* out){
  //gasesc masa in lista de mese
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    return;
  }

  //gasesc jucatorul in cadrul mesei identificate anterior
  Jucator J = findPlayer(M, name);
  if (J == NULL){
    fprintf(out, "Jucatorul %s nu exista la masa %s!\n", name, table);
    return;
  }

  J->nrMaini -= value;

  /*daca nrMaini al unui jucator a ajuns la 0 (deci ramane fara maini),
   acesta trebuie scos de la masa, iar gestiunea salii actualizata*/
  if (J->nrMaini <= 0){
    M->nrCrtJucatori--;
    S->nrLocCrt--;
    delPlayer(&M, J);

    /*suplimentar, daca masa a ramas fara jucatori (mai e doar santinela),
    acea masa trebuie stearsa si gestiunea numarului de locuri din sala trebuie
    actualizata corespunzator*/
    if (M->nrCrtJucatori <= 0){
      S->nrMese--;
      S->nrLocMax -= M->nrMaxJucatori;
      delTable(&S, M);
    }
  }
}

/*functie corespunzatoare comenzii tura, prin care decrementez cu 1 campul
nrMaini al fiecarui jucator de la acea masa si tratez cazurile extreme*/
void tura(Sala S, char* table, FILE* out){
  Jucator J = NULL;
  //gasesc masa la care trebuie efectuata tura
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    return;
  }

  TLista L = M->jucatori;
  TLista u = NULL;
  TLista n = L->urm;

  /*parcurg lista de jucatori pentru identificarea ultimului jucator de la masa,
  stocat in variabila u*/
  for (u = L->urm; u->urm != L; u = u->urm);

  //mut santinela, refacand legaturile din lista de jucatori
  u->urm = n;
  L->urm = n->urm;
  n->urm = L;

  /*realizez tura efectiva, aplicand o logica similara celei de la ghinion,
  tratand cazurile extreme in care un jucator ramane fara maini sau sala ramane
  fara mese*/
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

//functie corespunzatoare comenzii tura_completa
void turaCompleta(Sala S, FILE* out){
  TLista L = S->masa;
  TLista p = NULL;
  Masa M = NULL;

  //parcurg lista de mese si efectuez tura pentru fiecare dintre ele
  for (p = L; p != NULL; p = p->urm){
    M = ((Masa)(p->info));
    tura(S, M->numeMasa, out);
  }
}

//functie corespunzatoare comenzii inchide
void inchide(Sala S, char* table, FILE* out){
  /*gasec masa pe care doresc sa o inchid, pentru a ma asigura ca nu inchid
  o masa care nu exista*/
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    return;
  }

  TLista L = NULL, u = NULL;
  TLista p = NULL, aux = NULL, q = NULL;
  Jucator J = NULL;
  int x, i;

  //verific mai intai daca exista loc in sala
  if (S->nrLocCrt > (S->nrLocMax - M->nrMaxJucatori)){
    fprintf(out, "Nu exista suficiente locuri in sala!\n");
    return;
  }

  //stochez inceputul listei de jucatori in variabila p
  p = M->jucatori;

  //parcurg lista de mese pentru a insera jucatorii de la masa ce va fi inchisa
  for (L = S->masa; L != NULL && M->nrCrtJucatori > 0; L = L->urm){
    /*trec la urmatoarea iteratie din for daca masa este tocmai masa pe care
    doresc sa o inchid*/
    if (strcmp(((Masa)(L->info))->numeMasa, table) == 0){
      continue;
    }

    //in variabila x calculez numarul de locuri disponibile de la masa respectiva
    x = ((Masa)L->info)->nrMaxJucatori - ((Masa)L->info)->nrCrtJucatori;

    //daca exista macar un loc liber la masa incep inserarea
    if (x > 0){
      /*in variabila q stochez inceputul listei de jucatori din masa la care
      vreau sa inserez*/
      q = ((Masa)(L->info))->jucatori;

      //parcurgere ca sa aflu ultimul jucator de la masa la care vreau sa inserez
      for (u = q->urm; u->urm != q; u = u->urm);

      /*cat timp mai exista locuri la acea masa si mai exista jucatori de mutat
      de la masa ce va fi inchisa*/
      for (i = 0; x > 0 && M->nrCrtJucatori > 0; i++){
        J = (Jucator)(p->urm->info);

        /*aloc o celula cu informatiile aferente jucatorului J, pentru a il
        adauga la masa unde i-a fost gasit loc*/
        aux = AlocCelula((void*) J, sizeof(struct jucator));
        if (!aux) printf("Alocare nereusita");

        u->urm = aux;
        aux->urm = q;
        u = u->urm;

        /*actualizez numarul de jucatori de la masa ce va fi inchisa, dar si
        locurile ramase disponibile la noua masa*/
        M->nrCrtJucatori--;
        x--;
        ((Masa)(L->info))->nrCrtJucatori++;

        p = p->urm;
      }
    }
  }

  /*actualizez gestiunea salii si apelez functia delTable pentru masa respectiva,
  eliberand memoria asociata si refacand legaturile*/
  S->nrLocMax -= M->nrMaxJucatori;
  S->nrMese--;
  delTable(&S, M);
}

//functie corespunzatoare comenzii clasament
void clasament(Sala S, char* table, FILE* out){
  /*gasesc masa la care doresc sa afisez clasamentul si in cazul in care nu
  exista, afisez un mesaj de eroare*/
  Masa M = findTable(S, table);
  if (M == NULL){
    fprintf(out, "Masa %s nu exista!\n", table);
    return;
  }

  fprintf(out, "Clasament %s:\n", table);

  //in rez salvez lista noua in care sortez lista de jucatori
  TLista rez = InitL();
  TLista p = NULL; //lista de jucatori
  TLista u = NULL, aux = NULL, inc = NULL;

  /*variabila auxiliara, pentru inserat jucatori in noua lista, in care voi
  stoca nrMaini si numele jucatorului pe care vreau sa il pun in lista rez*/
  Jucator ins = (Jucator) calloc(1, sizeof(struct jucator));
  if (!ins) return;

  int count = 0, lastMaxVal = 0, curMaxValue = 0, value;
  char* last = calloc(1, NMAX);
  if (!last){
    free(ins);
    return;
  }

  //in variabila inc salvez inceputul listei rezultat
  inc = rez;

  ins->nume = (char*) calloc(1, NMAX);
  if (!ins->nume){
    free(ins);
    free(last);
    return;
  }

  //in variabila p memorez inceputul listei de jucatori (adica santinela)
  p = M->jucatori;

  /*in variabila curMaxValue stochez jucatorul cu cel mai mare nrMaini de la
  masa, dar care sa nu fi fost deja inserat in lista rezultat, initializata
  cu nrMaini al primului jucator (cel imediat de dupa santinela)*/
  curMaxValue = ((Jucator)(p->urm->info))->nrMaini;
  ins->nrMaini = curMaxValue;
  ins->nume = ((Jucator)(p->urm->info))->nume;

  /*fac o parcurgere pentru a determina valoarea maxima din lista si a o insera
  in lista rez*/
  for (u = p->urm; u != p; u = u->urm){
    /*in variabila value memorez nrMaini al jucatorului curent, pentru a compara
    intr-un mod mai readable cu curMaxValue*/
    value = ((Jucator)(u->info))->nrMaini;

    //daca valoarea e strict mai mare, modific informatiile din ins corespunzator
    if (value > curMaxValue){
      ins->nrMaini = value;
      ins->nume = ((Jucator)(u->info))->nume;
      curMaxValue = value;
    }

    /*daca e egala cu curMaxValue, verific daca este in ordinea lexicografica
    ceruta in enunt*/
    else if (value == curMaxValue){
      if (strcmp(((Jucator)(u->info))->nume, ins->nume) > 0){
        ins->nume = ((Jucator)(u->info))->nume;
      }
    }
  }

  /*aloc o celula pentru a o insera in lista rezultat si actualizez count, o
  variabila care stocheaza numarul de jucatori deja introdusi in lista sortata*/
  aux = AlocCelula((void*) ins, sizeof(struct jucator));
  rez->urm = aux;
  aux->urm = inc;
  rez = rez->urm;
  count++;

  //cat timp inca mai exista jucatori care nu au fost deja sortati
  while (count < M->nrCrtJucatori){
    /*in lastMaxVal stochez valoarea maxima din ultima iteratie, iar in last
    numele ultimului jucator inserat in lista rez, deci sortat*/
    lastMaxVal = curMaxValue;
    curMaxValue = 0;
    last = ins->nume;

    //parcurg lista de jucatori
    for (u = p->urm; u != p; u = u->urm){
      value = ((Jucator)(u->info))->nrMaini;

      /*daca valoarea este mai mare decat curMaxValue si nu a mai fost deja
      inserat, actualizez informatiile din ins*/
      if ((value < lastMaxVal) && (value > curMaxValue)){
        curMaxValue = value;
        ins->nrMaini = value;
        ins->nume = ((Jucator)(u->info))->nume;
      }
      /*altfel, daca valoarea e egala cu valoarea maxima curenta si mai mica
      decat ultima valoare maxima (deci e un posibil candidat la inserare),
      verific daca este ok din punct de vedere lexicografic*/
      else if ((value == curMaxValue) && (curMaxValue != lastMaxVal)){
        if ((strcmp(((Jucator)(u->info))->nume, ins->nume)) > 0){
          ins->nume = ((Jucator)(u->info))->nume;
          ins->nrMaini = value;
        }
      }
      /*daca valoarea este egala cu ultima inserata, verific daca nu cumva
      este tocmai jucatorul inserat ultima inserata, iar daca nu fac niste
      verificari asemanatoare cu cele de mai sus*/
      else if (value == lastMaxVal){
        if ((strcmp(((Jucator)(u->info))->nume, last)) < 0){
          if (value == curMaxValue){
            if ((strcmp(((Jucator)(u->info))->nume, ins->nume)) > 0){
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

    /*pentru fiecare iteratie din for inserez o celula cu informatiile din ins,
    adica cea cu urmatorul jucator din lista si fac legaturile*/
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

/*functie care parseaza comenzile din fisierul de evenimente, folosindu-ma de
strtok pentru a delimita argumentele si atoi pentru valorile numerice, cum ar fi
cele pentru gradul de noroc sau ghinion, iar mai apoi apelez functia corespnzatoare*/
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
  /*deschid fisierele cu care lucreaza programul, pe baza argumentelor primite
  din linia de comanda*/
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

  Sala S;

  //initializez sala cu datele citite din fisierul de configuratie al salii
  S = ReadCFG(config);

  while ((read = getline(&line, &len, events)) != -1){
    parseCommand(line, S, out);
  }

  /*eliberarea memoriei din sala, a char* alocat dinamic si inchiderea fisierelor
  folosite pe parcursul programului*/
  TLista aux, p = S->masa;
  while(p != NULL){
    aux = p;
    p = p->urm;
    DistrugeMasa(aux);
    free((Masa)(aux->info));
    free(aux);

  }

  free(S);
  free(line);
  fclose(config);
  fclose(events);
  fclose(out);
  return 0;
}
