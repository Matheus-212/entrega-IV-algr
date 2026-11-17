#include "sort.h"
#include "interface.h"
#include <string.h>

// Funcao de comparacao: retorna > 0 se "a" deve vir depois de "b"
typedef int (*comparador)(const produto *a, const produto *b);

static int cmp_az(const produto *a, const produto *b) { return strcmp(a->nm, b->nm); }
static int cmp_za(const produto *a, const produto *b) { return strcmp(b->nm, a->nm); }
static int cmp_menor(const produto *a, const produto *b) { return a->vlr > b->vlr; }
static int cmp_maior(const produto *a, const produto *b) { return a->vlr < b->vlr; }

// Bubble sort generico (estavel): a ordem vem do comparador
static void bubble_sort(produto *list, int size, comparador cmp)
{
    produto aux;

    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (cmp(&list[j], &list[j + 1]) > 0)
            {
                aux = list[j];
                list[j] = list[j + 1];
                list[j + 1] = aux;
            }
        }
    }
}

void print_body(produto *list, int size)
{
    print_header();

    for (int i = 0; i < size; i++)
    {
        print_item(&list[i]);
    }

    print_footer();
}

void show_sorted(produto *items, int size, int ord)
{
    static const comparador por_ordem[] = {NULL, cmp_az, cmp_za, cmp_menor, cmp_maior};

    if (ord >= 1 && ord <= 4)
        bubble_sort(items, size, por_ordem[ord]);

    print_body(items, size);
}
