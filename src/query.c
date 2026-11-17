#include "query.h"
#include "sort.h"
#include "produtos.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

// Adiciona um item ao vetor dinamico. Retorna 0 se faltar memoria
static int push_item(produto **items, int *size, const produto *item)
{
    produto *temp = realloc(*items, (*size + 1) * sizeof(produto));
    if (temp == NULL)
        return 0;

    *items = temp;
    (*items)[(*size)++] = *item;
    return 1;
}

// Retorna 1 se o item bate com o texto digitado no campo escolhido
static int match_item(const produto *item, const char *ent, int type)
{
    switch (type)
    {
    case 0:
        return item->id == atoi(ent);
    case 1:
        return strcasecmp(item->nm, ent) == 0;
    case 2:
        return strcasecmp(item->ctg, ent) == 0;
    case 4:
        return item->vlr == strtof(ent, NULL);
    case 5:
        return item->qnt == atoi(ent);
    case 6:
        return strcasecmp(item->fndr, ent) == 0;
    }
    return 0;
}

void list_products(char *file, int *ord)
{
    produto *items = NULL;
    produto item;
    int size = 0;

    FILE *fp = fopen(file, "rb");
    if (fp == NULL)
    {
        printf("Erro de acesso ao arquivo!\n");
        return;
    }

    while (fread(&item, sizeof(produto), 1, fp) == 1)
    {
        if (!push_item(&items, &size, &item))
        {
            free(items);
            fclose(fp);
            return;
        }
    }
    fclose(fp);

    show_sorted(items, size, *ord);
    free(items);
}

void search_product(char *ent, int type, char *file, int ord)
{
    produto *items = NULL;
    produto item;
    int size = 0;

    FILE *fp = fopen(file, "rb");
    if (fp == NULL)
    {
        printf("Erro de acesso ao arquivo!\n");
        return;
    }

    while (fread(&item, sizeof(produto), 1, fp) == 1)
    {
        if (item.id == 0)
            break;

        if (match_item(&item, ent, type) && !push_item(&items, &size, &item))
        {
            free(items);
            fclose(fp);
            return;
        }
    }
    fclose(fp);

    if (size == 0)
        printf("Nenhum item encontrado!\n");
    else
        show_sorted(items, size, ord);

    free(items);
}
