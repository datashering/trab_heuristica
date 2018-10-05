#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <dirent.h>
#include "Heuristicas.h"

void get_names(std::vector<std::string> &batch, const char *dir_name) {
  struct dirent *dp;
  DIR *dir = opendir(dir_name);

  batch.clear();
  while ((dp = readdir(dir)) != nullptr) {
    if (dp->d_name[0] != '.') {
      batch.push_back(dp->d_name);
    }
  }
  closedir(dir);
}

int main()
{
  std::vector<std::string> batch;
  std::clock_t inicio;
  double tempo_exe, func_obj;
  std::ofstream arquivo;
// //  >>>>>>>>>>>>>>>>>>>>>   BATCH 10
 get_names(batch, "instancias_c/batch10");
 arquivo.open("saida/Drop_batch10.txt");

 for (std::string nome : batch) {
   std::cout << "batch10 " << nome << std::endl;
   Instancia dados(("instancias_c/batch10/" + nome).c_str());

   inicio = clock();
   func_obj = Drop(dados);
   tempo_exe = static_cast<double>(clock() - inicio) / CLOCKS_PER_SEC;
   arquivo << nome << "-" << func_obj << "-" << tempo_exe << std::endl;
 }
 arquivo.close();
 }
