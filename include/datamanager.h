#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "config.h"
#include "produtos.h"
#include "interface.h"
#include "filemanager.h"
#include "sort.h"
#include "query.h"

void dados_iniciais();
int gen_id(const char *filename);
void add_product(produto *p, const char *file, const char *origin);
void delete_product(int id, const char *file, const char *tmp);
void update_product(produto *p, const char *file, const char *tmp);

#endif
