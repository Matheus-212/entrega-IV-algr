#include <stdio.h>
#include "interface.h"

// Linha de cima da tabela e titulos das colunas
void print_header()
{
    printf("+-----+--------------------------------+----------------------+------------+------------+--------------------------------+\n");
    printf("| %-3s | %-30s | %-20s | %-10s | %-10s | %-30s |\n", "ID", "Produto", "Categoria", "Valor", "Quantidade", "Fornecedor");
    printf("+-----+--------------------------------+----------------------+------------+------------+--------------------------------+\n");
}

// Uma linha da tabela com os dados do produto
void print_item(produto *item)
{
    printf("| %-3d | %-30s | %-20s | %10.2f | %-10d | %-30s |\n", item->id, item->nm, item->ctg, item->vlr, item->qnt, item->fndr);
}

// Linha final que fecha a tabela
void print_footer()
{
    printf("+-----+--------------------------------+----------------------+------------+------------+--------------------------------+\n");
}
