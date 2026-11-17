#ifndef CLI_PARSE_H
#define CLI_PARSE_H

#include "produtos.h"

// Valida "Nome,Categoria,Valor,Quantidade,Fornecedor" e preenche *p.
// Retorna 1 se ok e 0 se der erro
int parse_produto(char *info, produto *p);

// Converte o texto da ordem em codigo (1 a 4); retorna 0 se invalido
int parse_ord(const char *txt, int *ord);

// Converte texto em ID (inteiro positivo). Retorna 1 se ok e 0 se invalido
int parse_id(const char *txt, int *id);

#endif
