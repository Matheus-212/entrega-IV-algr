#include <stdio.h>
#include <stdlib.h>
#include "filemanager.h"

// Retorna 1 se o arquivo existe e 0 se nao existe
int check_file(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        return 0; // o arquivo não encontrado
    }
    fclose(fp);
    return 1; // o arquivo encontrado
}

// Acrescenta um item no final do arquivo binario
void write_bin_line(produto item, const char *file)
{
    FILE *fp = fopen(file, "ab");
    if (fp == NULL)
    {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    // fwrite devolve quantos blocos gravou (esperado: 1); so avisa se falhar
    size_t save = fwrite(&item, sizeof(produto), 1, fp);
    if (save != 1)
    {
        printf("Erro ao salva os dados\n");
    }
    fclose(fp);
    return;
}

// Converte o arquivo binario para CSV
void export_csv(const char *csv, const char *file)
{
    produto item;
    FILE *fp = fopen(csv, "w");
    if (fp == NULL)
    {
        printf("Erro de acesso ao %s\n", csv);
        return;
    }
    FILE *fb = fopen(file, "rb");
    if (fb == NULL)
    {
        fclose(fp);
        printf("Erro de acesso ao %s\n", file);
        return;
    }
    // Primeira linha do CSV: nomes das colunas
    fprintf(fp, "id, produto, categoria, valor, quantidade, fornecedor\n");
    // Uma linha do CSV para cada item do arquivo
    while (fread(&item, sizeof(produto), 1, fb) == 1)
    {
        fprintf(fp, "%d, %s, %s, %.2f, %d, %s\n", item.id, item.nm, item.ctg, item.vlr, item.qnt, item.fndr);
    }
    fclose(fp);
    fclose(fb);
    return;
}

// Troca o arquivo original pelo temporario (confirma as alteracoes)
void save_data(const char *file, const char *origin)
{
    // Sem temporario nao ha o que salvar (evita apagar o original)
    if (!check_file(file))
    {
        printf("Nada para salvar!\n");
        return;
    }

    // Apaga o original antes, pois rename falha se o destino existir
    if (remove(origin) != 0)
    {
        perror("Erro ao deletar o arquivo");
        return;
    }

    if (rename(file, origin) != 0)
    {
        perror("Erro ao renomear o arquivo");
        return;
    }
    return;
}
