#pragma once

#include <glpk.h>
#include  <vector.h>
#include "Instancia.h"

struct Indice {
  char name;
  int idx[2];
}

class LPSolver {
  glp_prob *lp;       // glpk
  int *ia, *ja;       // Indices para matriz de restrições
  double *ar;         // Valores da matriz de restrições

  std::vector<Indice> idx_vec;
  double func_obj;    // Função Objetivo

  LPSolver(Inst&);
  ~LPSolver();

  void resolve();
  void abre_cd(int);
  void fecha_cd(int);
  void atualiza_sol(Solucao&);
}
