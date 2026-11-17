#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "filemanager.h"
#include "datamanager.h"
#include "config.h"
#include "produtos.h"
#include "cli.h"

int main()
{
    // Primeira execucao: cria o arquivo de dados com o estoque inicial
    if (!(check_file(DATA_FILE)))
    {
        dados_iniciais();
    }
    // Descarta temporario esquecido de uma execucao anterior
    if (check_file(TMP_FILE))
    {
        remove(TMP_FILE);
    }
    char input[256];
    char comando[8];
    char *parametro;

    printf("Bem vindo\n");

    while (1)
    {
        comando[0] = '\0';
        input[0] = '\0';
        printf("\n> ");
        // Fim da entrada (Ctrl+D / Ctrl+Z): sai do loop em vez de repetir
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        int i = 0;
        // Separa a primeira palavra (o comando), com no maximo 7 letras
        while (input[i] != '\0' && input[i] != ' ' && i < 7)
        {
            comando[i] = input[i];
            i++;
        }
        comando[i] = '\0';
        // O restante da linha vira o parametro do comando
        parametro = &input[i];
        if (*parametro == ' ')
        {
            parametro++;
        }
        // Sair: apaga o temporario (alteracoes nao salvas) e encerra
        if (strcasecmp(comando, "exit") == 0)
        {
            if (check_file(TMP_FILE))
            {
                if (remove(TMP_FILE) != 0)
                {
                    perror("Erro ao deletar o .tmp");
                    system("pause");
                    return 0;
                }
            }
            system("pause");
            return 0;
        }
        int encontrado = 0;
        // Procura o comando na lista e executa a funcao dele
        for (int i = 0; i < qnt_funcs; i++)
        {
            if (strcasecmp(comando, comandosList[i].name) == 0)
            {
                comandosList[i].func(parametro);
                encontrado = 1;
                break;
            }
        }
        if (!encontrado)
        {
            printf("ERRO: comando invalido\n");
        }
    }

    return 0;
}
