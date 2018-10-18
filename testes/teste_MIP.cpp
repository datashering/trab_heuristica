#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <dirent.h>
#include <pthread.h>
#include "../Heuristicas.h"
#include "../Solver.h"

void get_names(std::vector<std::string> &batch, const char *dir_name) {
  struct dirent *dp;
  DIR *dir = opendir(dir_name);

  if (!dir) {
    std::cout << "get_name: problema na abertura do arquivo" << std::endl;
    exit(-1);
  }

  batch.clear();
  while ((dp = readdir(dir)) != nullptr) {
    if (dp->d_name[0] != '.') {
      batch.push_back(dp->d_name);
    }
  }
  closedir(dir);
}

void *run_test(void *batch_num) {
  int n = *static_cast<int*>(batch_num);
  std::vector<std::string> batch;
  std::clock_t inicio;
  double tempo_exe, ub, lb, gap;
  std::ofstream arquivo_saida;
  std::string str_n = std::to_string(n);

  get_names(batch, ("../instancias_c/batch" + str_n).c_str());
  arquivo_saida.open("../saida/MIP_batch" + str_n + ".txt");

  for (std::string nome : batch) {
    Instancia dados(("../instancias_c/batch" + str_n + "/" + nome).c_str());
    MIPSolver solver(dados);

    std::cout << "batch" << n << " " << nome <<std::endl;
    inicio = clock();
    solver.resolve();
    tempo_exe = static_cast<double>(clock() - inicio) / CLOCKS_PER_SEC;

    ub = solver.func_obj;
    gap = solver.mip_gap;
    lb = ub - (ub * gap);

    arquivo_saida << nome << "_" << lb << "_" << ub << "_" << gap << "_" << tempo_exe << std::endl;
  }
  arquivo_saida.close();

}

int main(){
  pthread_t threads[5];
  int rc;
  int batch_numbers[5] = {0, 6, 7, 8, 9};

  for (int i=0; i<5; i++) {
    rc = pthread_create(&threads[i], NULL, run_test, static_cast<void*>(&batch_numbers[i]));

    if (rc) {
      std::cout << "Erro: problema na criação do thread " << rc << std::endl;
      exit(-1);
    }
  }

  pthread_exit(NULL);
}
