#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "config.h"
#include "produtos.h"
#include "interface.h"

int check_file(const char *file);
void write_bin_line(produto item, const char *file);
void export_csv(const char *csv, const char *bin);
void save_data(const char *file, const char *origin);

#endif
