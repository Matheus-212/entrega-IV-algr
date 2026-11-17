#ifndef CLI_H
#define CLI_H

#include "datamanager.h"
#include "filemanager.h"
#include "config.h"
#include "produtos.h"

typedef struct
{
    char name[8];
    void (*func)(char *);
    char descricao[128];
} comandos;

extern const int qnt_funcs;
extern comandos comandosList[];

void cmd_insert(char *info);
void cmd_delete (char *info);
void cmd_list(char *info);
void cmd_find(char *info);
void cmd_save(char *info);
void cmd_export(char *info);
void cmd_help(char *info);
void cmd_update(char *info);

#endif
