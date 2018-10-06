#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <dirent.h>
#include <algorithm>
#include "Heuristicas.h"
#include "Solver.h"

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
  std::sort(batch.begin(), batch.end());
}

int main() {
  std::vector<std::string> batch;
  std::clock_t inicio;
  std::ofstream arquivo;
  double tempo_total;

  //  >>>>>>>>>>>>>>>>>>>>>   BATCH 8
  get_names(batch, "instancias_c/batch8");

  arquivo.open("saida/H4_batch8.txt");
  for (std::string nome : batch) {
    std::cout << "batch8 " << nome << std::endl;

    Instancia dados(("instancias_c/batch8/" + nome).c_str());
    Resultados r;

    inicio = clock();
    heuristica_iterativa(dados, 0.1, r);
    tempo_total = static_cast<double>(clock() - inicio) / CLOCKS_PER_SEC;
    arquivo << nome << "-" << tempo_total << "-" << r.fo << std::endl;
  }

  arquivo.close();

  //  >>>>>>>>>>>>>>>>>>>>>   BATCH 9
  get_names(batch, "instancias_c/batch9");

  arquivo.open("saida/H4_batch9.txt");
  for (std::string nome : batch) {
    std::cout << "batch9 " << nome << std::endl;

    Instancia dados(("instancias_c/batch9/" + nome).c_str());
    Resultados r;

    inicio = clock();
    heuristica_iterativa(dados, 0.1, r);
    tempo_total = static_cast<double>(clock() - inicio) / CLOCKS_PER_SEC;
    arquivo << nome << "-" << tempo_total << "-" << r.fo << std::endl;
  }

  arquivo.close();

  //  >>>>>>>>>>>>>>>>>>>>>   BATCH 10
  get_names(batch, "instancias_c/batch10");

  arquivo.open("saida/H4_batch10.txt");
  for (std::string nome : batch) {
    std::cout << "batch10 " << nome << std::endl;

    Instancia dados(("instancias_c/batch10/" + nome).c_str());
    Resultados r;

    inicio = clock();
    heuristica_iterativa(dados, 0.1, r);
    tempo_total = static_cast<double>(clock() - inicio) / CLOCKS_PER_SEC;
    arquivo << nome << "-" << tempo_total << "-" << r.fo << std::endl;
  }

  arquivo.close();

  //  >>>>>>>>>>>>>>>>>>>>>   BATCH 11
  get_names(batch, "instancias_c/batch11");

  arquivo.open("saida/H4_batch11.txt");
  for (std::string nome : batch) {
    std::cout << "batch11 " << nome << std::endl;

    Instancia dados(("instancias_c/batch11/" + nome).c_str());
    Resultados r;

    inicio = clock();
    heuristica_iterativa(dados, 0.1, r);
    tempo_total = static_cast<double>(clock() - inicio) / CLOCKS_PER_SEC;
    arquivo << nome << "-" << tempo_total << "-" << r.fo << std::endl;
  }

  arquivo.close();
}
