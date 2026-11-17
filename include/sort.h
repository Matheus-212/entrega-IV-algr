#ifndef SORT_H
#define SORT_H

#include "produtos.h"

// Imprime a tabela completa: cabecalho, itens e rodape
void print_body(produto *list, int size);

// Imprime os itens na ordem pedida
// 0 = sem ordem, 1 = A-Z, 2 = Z-A, 3 = menor valor, 4 = maior valor
void show_sorted(produto *items, int size, int ord);

#endif
