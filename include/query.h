#ifndef QUERY_H
#define QUERY_H

// Le todos os itens do arquivo e imprime na ordem escolhida (*ord)
void list_products(char *file, int *ord);

// Busca itens pelo campo escolhido em "type" e imprime os encontrados
// type: 0 = id, 1 = nome, 2 = categoria, 4 = valor, 5 = quantidade, 6 = fornecedor
void search_product(char *ent, int type, char *file, int ord);

#endif
