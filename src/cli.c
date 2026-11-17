#include "cli.h"
#include "cli_parse.h"
#include <stdio.h>
#include <string.h>

// Lista de comandos: nome, funcao e descricao (usada no help)
comandos comandosList[] = {
    {"insert", cmd_insert, "Cadastra um item. Use o formato: Nome,Categoria,Valor,Quantidade,Fornecedor (sem espacos extras)"},
    {"delete", cmd_delete, "Remove um item do sistema informando apenas o ID"},
    {"update", cmd_update, "Atualiza um item. Use o formato: ID,Nome,Categoria,Valor,Quantidade,Fornecedor"},
    {"list",   cmd_list, "Lista os itens. Ordem opcional: az, za, menor, maior (ex.: list az)"},
    {"find",   cmd_find, "Busca: find -id|-nm|-ct|-vl|-qt|-fn valor [az|za|menor|maior]"},
    {"save",   cmd_save, "Salva permanentemente todas as alteracoes realizadas"},
    {"export", cmd_export, "Exporta os dados atuais para um arquivo no formato CSV"},
    {"help",   cmd_help, "Exibe a lista de comandos"},
};

const int qnt_funcs = 8;

// Arquivo de trabalho: o temporario (com alteracoes nao salvas) se existir
static const char *arquivo_atual(void)
{
    return check_file(TMP_FILE) ? TMP_FILE : nowfile;
}

// insert Nome,Categoria,Valor,Quantidade,Fornecedor
void cmd_insert(char *info)
{
    produto p = {0};

    if (info == NULL || info[0] == '\0')
    {
        printf("Erro: entrada vazia!\n");
        return;
    }

    // O ID e gerado pelo sistema (olha o temporario tambem,
    // senao dois inserts seguidos recebiam o mesmo ID)
    p.id = gen_id(arquivo_atual());

    if (!parse_produto(info, &p))
        return;

    add_product(&p, TMP_FILE, nowfile);
}

// delete ID
void cmd_delete(char *info)
{
    int id;

    if (info == NULL || info[0] == '\0')
    {
        printf("Erro: ID não informado!\n");
        return;
    }

    if (!parse_id(info, &id))
    {
        printf("Erro: ID inválido!\n");
        return;
    }

    // Parte do temporario (se existir) para nao perder alteracoes nao salvas
    delete_product(id, arquivo_atual(), TMP_FILE);
}

// list [az|za|menor|maior]
void cmd_list(char *info)
{
    int ord = 0;

    if (info != NULL && info[0] != '\0' && !parse_ord(info, &ord))
    {
        printf("Erro: ordem invalida! Use az, za, menor ou maior\n");
        return;
    }

    // Mostra o temporario (com alteracoes nao salvas) se ele existir
    list_products((char *)(check_file(TMP_FILE) ? TMP_FILE : DATA_FILE), &ord);
}

// save (sem argumentos)
void cmd_save(char *info)
{
    if (info != NULL && info[0] != '\0')
    {
        printf("Erro: Esse comando nao possui argumentos\n");
        return;
    }
    save_data(TMP_FILE, DATA_FILE);
}

// export (sem argumentos)
void cmd_export(char *info)
{
    if (info != NULL && info[0] != '\0')
    {
        printf("Erro: Esse comando nao possui argumentos\n");
        return;
    }
    export_csv("dados.csv", DATA_FILE);
}

// Mostra o nome e a descricao de cada comando
void cmd_help(char *info)
{
    (void)info;
    printf("LISTA DE COMANDOS\n");
    printf("=======================\n");
    for (int i = 0; i < qnt_funcs; i++)
    {
        printf("  %-10s", comandosList[i].name);
        if (strlen(comandosList[i].descricao) > 0)
            printf("- %s", comandosList[i].descricao);
        printf("\n");
    }
}

// update ID,Nome,Categoria,Valor,Quantidade,Fornecedor
void cmd_update(char *info)
{
    produto p = {0};
    char id[32] = {0};

    if (info == NULL || info[0] == '\0')
    {
        printf("Erro: entrada vazia!\n");
        return;
    }

    // Separa o ID (ate a primeira virgula) do resto dos campos
    char *virgula = strchr(info, ',');
    if (virgula == NULL || (size_t)(virgula - info) >= sizeof(id))
    {
        printf("Erro: use o formato ID,Nome,Categoria,Valor,Quantidade,Fornecedor\n");
        return;
    }
    strncpy(id, info, virgula - info);

    if (!parse_id(id, &p.id))
    {
        printf("Erro: ID inválido!\n");
        return;
    }

    // Mantem o ID e valida os demais campos como no insert
    if (!parse_produto(virgula + 1, &p))
        return;

    update_product(&p, arquivo_atual(), TMP_FILE);
}
