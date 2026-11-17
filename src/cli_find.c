#include "cli.h"
#include "cli_parse.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>

// Flag do find -> codigo do campo usado no search_product
static int flag_type(const char *flag)
{
    static const struct { const char *flag; int type; } flags[] = {
        {"-id", 0}, {"-nm", 1}, {"-ct", 2}, {"-vl", 4}, {"-qt", 5}, {"-fn", 6}};

    for (int k = 0; k < 6; k++)
    {
        if (strcasecmp(flag, flags[k].flag) == 0)
            return flags[k].type;
    }
    return -1;
}

// Campos numericos precisam de numero valido. Retorna 1 se ok
static int valor_valido(const char *valor, int type)
{
    char *end;

    if (type != 0 && type != 4 && type != 5)
        return 1;

    errno = 0;
    if (type == 4)
        strtof(valor, &end);
    else
        strtol(valor, &end, 10);

    if (errno == ERANGE || *end != '\0' || end == valor)
    {
        printf(type == 4 ? "Erro: valor numerico invalido!\n"
                         : "Erro: o valor deve ser um numero inteiro!\n");
        return 0;
    }
    return 1;
}

// find -id|-nm|-ct|-vl|-qt|-fn valor [az|za|menor|maior]
void cmd_find(char *info)
{
    int ord = 0;
    char flag[8] = {0};
    char valor[64] = {0};

    if (info == NULL || info[0] != '-')
    {
        printf("Erro: use find -id|-nm|-ct|-vl|-qt|-fn valor [az|za|menor|maior]\n");
        return;
    }

    // Separa a flag (ate o primeiro espaco) do valor
    char *espaco = strchr(info, ' ');
    if (espaco == NULL || (size_t)(espaco - info) >= sizeof(flag))
    {
        printf("Erro: informe a flag e o valor. Ex.: find -fn empresatech az\n");
        return;
    }
    strncpy(flag, info, espaco - info);

    int type = flag_type(flag);
    if (type < 0)
    {
        printf("Erro: flag invalida! Use -id, -nm, -ct, -vl, -qt ou -fn\n");
        return;
    }

    // Ignora espacos extras antes do valor
    char *resto = espaco + 1;
    while (*resto == ' ')
        resto++;

    if (resto[0] == '\0' || strlen(resto) >= sizeof(valor))
    {
        printf("Erro: valor vazio ou muito longo!\n");
        return;
    }
    strcpy(valor, resto);

    // Se a ultima palavra for uma ordem valida, tira ela do valor
    char *ultimo = strrchr(valor, ' ');
    if (ultimo != NULL && parse_ord(ultimo + 1, &ord))
    {
        while (ultimo > valor && *(ultimo - 1) == ' ')
            ultimo--;
        *ultimo = '\0';
    }

    if (valor[0] == '\0')
    {
        printf("Erro: valor vazio!\n");
        return;
    }

    if (!valor_valido(valor, type))
        return;

    // Busca no temporario (se existir) para incluir alteracoes nao salvas
    search_product(valor, type, check_file(TMP_FILE) ? TMP_FILE : DATA_FILE, ord);
}
