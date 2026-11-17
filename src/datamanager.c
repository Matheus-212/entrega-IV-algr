#include "datamanager.h"
#include <stdio.h>
#include <stdlib.h>

// Cria o arquivo de dados com o estoque inicial
void dados_iniciais()
{
    produto estoque_inicial[10] = {
        {1, "Resistor 100R 1/4W", "Resistores", 0.15, 500, "Multilaser"},
        {2, "Resistor 10K 1/4W", "Resistores", 0.15, 450, "Multilaser"},
        {3, "Capacitor Eletrolitico 10uF", "Capacitores", 0.50, 200, "Epcos"},
        {4, "Capacitor Ceramico 100nF", "Capacitores", 0.30, 300, "Vishay"},
        {5, "Diodo Retificador 1N4007", "Diodos", 0.25, 400, "Fairchild"},
        {6, "Diodo Zener 5.1V", "Diodos", 0.40, 150, "NXP"},
        {7, "LED 5mm Vermelho", "Semicondutores", 0.20, 350, "Kingbright"},
        {8, "Transistor BC548", "Transistores", 0.45, 250, "ON Semi"},
        {9, "CI Regulat. Tensao 7805", "CI", 1.80, 100, "ST Micro"},
        {10, "Trimpot 10K", "Resistores", 1.20, 80, "Bourns"}};

    for (int i = 0; i < 10; i++)
    {
        write_bin_line(estoque_inicial[i], DATA_FILE);
    }
}

// Retorna o menor ID livre (reaproveita IDs de itens removidos)
int gen_id(const char *file)
{
    int i = 1, id = 1, atv = 1;
    produto item;
    FILE *fp = fopen(file, "rb");
    if (fp == NULL)
    {
        printf("Erro de acesso ao arquivos!\n");
        return 0;
    }
    while (atv)
    {
        // Assume que "i" esta livre; arquivo vazio nao trava mais o loop
        id = i;
        atv = 0;
        rewind(fp);
        // Procura "i" no arquivo; se achar, tenta o proximo numero
        while (fread(&item, sizeof(produto), 1, fp) == 1)
        {
            if (i == item.id)
            {
                atv = 1;
                break;
            }
        }
        i++;
    }
    fclose(fp);
    return id;
}

// Adiciona um item no arquivo temporario (cria a copia dele se nao existir)
void add_product(produto *p, const char *file, const char *origin)
{
    if (!(check_file(file)))
    {
        produto item;
        FILE *fb = fopen(origin, "rb");
        FILE *fp = fopen(file, "w");
        if (fp == NULL || fb == NULL)
        {
            // Fecha o que chegou a abrir, para nao deixar arquivo preso
            if (fp != NULL)
                fclose(fp);
            if (fb != NULL)
                fclose(fb);
            printf("Erro de acesso aos arquivos!\n");
            return;
        }
        // Copia os itens do arquivo original para o temporario
        while (fread(&item, sizeof(produto), 1, fb) == 1)
        {
            write_bin_line(item, file);
        }
        fclose(fp);
        fclose(fb);
    }
    write_bin_line(*p, file);
    printf("Item cadastrado com ID %d!\n", p->id);
}

// Troca o temporario pelo arquivo auxiliar
static void swap_to_tmp(const char *tmp)
{
    remove(tmp);
    if (rename(SWAP_FILE, tmp) != 0)
        perror("Erro ao atualizar o temporario");
}

// Reescreve "file" no arquivo auxiliar, depois troca pelo temporario.
// Se "novo" for NULL o item de ID "id" e removido; senao e substituido.
// Retorna 1 se o ID foi encontrado
static int rewrite_without(int id, const produto *novo, const char *file, const char *tmp)
{
    produto item;
    int found = 0;
    FILE *fb = fopen(file, "rb");
    if (fb == NULL)
    {
        printf("Erro de acesso ao %s\n", file);
        return 0;
    }
    FILE *fp = fopen(SWAP_FILE, "w");
    if (fp == NULL)
    {
        fclose(fb);
        printf("Erro de acesso ao %s\n", SWAP_FILE);
        return 0;
    }
    fclose(fp);

    while (fread(&item, sizeof(produto), 1, fb) == 1)
    {
        if (item.id != id)
            write_bin_line(item, SWAP_FILE);
        else
        {
            found = 1;
            if (novo != NULL)
                write_bin_line(*novo, SWAP_FILE);
        }
    }
    fclose(fb);

    if (found)
        swap_to_tmp(tmp);
    else
    {
        remove(SWAP_FILE);
        printf("Erro: ID %d nao encontrado!\n", id);
    }
    return found;
}

// Remove o item do ID informado. Le "file" (original ou temporario) e
// grava o resultado no temporario
void delete_product(int id, const char *file, const char *tmp)
{
    if (rewrite_without(id, NULL, file, tmp))
        printf("Item %d removido!\n", id);
}

// Troca o item de mesmo ID por *p
void update_product(produto *p, const char *file, const char *tmp)
{
    if (rewrite_without(p->id, p, file, tmp))
        printf("Item %d atualizado!\n", p->id);
}
