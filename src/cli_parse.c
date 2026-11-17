#include "cli_parse.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>

// Posicao da proxima virgula (ou do fim do texto) a partir de "i"
static int field_end(const char *s, int i)
{
    while (s[i] != ',' && s[i] != '\0')
        i++;
    return i;
}

// Copia um campo de texto de tamanho "len" para dst (capacidade "cap").
// Retorna 0 e mostra "erro" se estiver vazio ou nao couber
static int copy_text(char *dst, size_t cap, const char *src, int len, const char *erro)
{
    if (len == 0 || (size_t)len >= cap)
    {
        printf("%s\n", erro);
        return 0;
    }
    memcpy(dst, src, len);
    dst[len] = '\0';
    return 1;
}

int parse_produto(char *info, produto *p)
{
    int start = 0;
    int i;
    char vlr[32] = {0};
    char qnt[32] = {0};
    char *end;

    // Precisa de 4 virgulas, senao as buscas abaixo leriam alem do texto
    int virgulas = 0;
    for (int k = 0; info[k] != '\0'; k++)
    {
        if (info[k] == ',')
            virgulas++;
    }
    if (virgulas < 4)
    {
        printf("Erro: quantidade de campos incorreta!\n");
        return 0;
    }

    // Nome
    i = field_end(info, start);
    if (!copy_text(p->nm, sizeof(p->nm), info + start, i - start,
                   "Erro: nome vazio ou muito longo!"))
        return 0;
    start = ++i;

    // Categoria
    i = field_end(info, start);
    if (!copy_text(p->ctg, sizeof(p->ctg), info + start, i - start,
                   "Erro: categoria vazia ou muito longa!"))
        return 0;
    start = ++i;

    // Valor (usa o buffer local vlr, nao sizeof(p->vlr))
    i = field_end(info, start);
    if (!copy_text(vlr, sizeof(vlr), info + start, i - start,
                   "Erro: valor invalido ou muito longo!"))
        return 0;

    errno = 0;
    p->vlr = strtof(vlr, &end);
    // Rejeita erro de conversao, lixo no fim do texto e valor negativo
    if (errno == ERANGE || *end != '\0' || end == vlr || p->vlr < 0)
    {
        printf("Erro: valor invalido!\n");
        return 0;
    }
    start = ++i;

    // Quantidade (usa o buffer local qnt, aceita 4 digitos ou mais)
    i = field_end(info, start);
    if (!copy_text(qnt, sizeof(qnt), info + start, i - start,
                   "Erro: quantidade invalida ou muito longa!"))
        return 0;

    errno = 0;
    long quantidade = strtol(qnt, &end, 10);
    if (errno == ERANGE || *end != '\0' || end == qnt || quantidade < 0 || quantidade > 2147483647L)
    {
        printf("Erro: quantidade inválida!\n");
        return 0;
    }
    p->qnt = (int)quantidade;
    start = ++i;

    // Fornecedor (o resto do texto; nao pode ter virgula)
    if (!copy_text(p->fndr, sizeof(p->fndr), info + start, (int)strlen(info + start),
                   "Erro: fornecedor vazio ou muito longo!"))
        return 0;

    if (strchr(p->fndr, ',') != NULL)
    {
        printf("Erro: quantidade de campos incorreta!\n");
        return 0;
    }
    return 1;
}

int parse_ord(const char *txt, int *ord)
{
    static const char *nomes[] = {"az", "za", "menor", "maior"};

    for (int k = 0; k < 4; k++)
    {
        if (strcasecmp(txt, nomes[k]) == 0)
        {
            *ord = k + 1;
            return 1;
        }
    }
    return 0;
}

int parse_id(const char *txt, int *id)
{
    char *end;

    errno = 0;
    long valor = strtol(txt, &end, 10);

    // O ID precisa ser um numero inteiro positivo
    if (errno == ERANGE || *end != '\0' || end == txt || valor <= 0 || valor > 2147483647L)
        return 0;

    *id = (int)valor;
    return 1;
}
