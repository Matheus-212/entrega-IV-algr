#ifndef PRODUTOS_H
#define PRODUTOS_H

#define MAX_NM 50
#define MAX_CTG 30
#define MAX_FNDR 50

typedef struct {
 int id;
 char nm[MAX_NM];
 char ctg[MAX_CTG];
 float vlr;
 int qnt;
 char fndr[MAX_FNDR];
} produto;

#endif
