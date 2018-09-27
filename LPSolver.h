#pragma once

#include <glpk.h>
#include "Inst.h"

class LPSolver {
  glp_prob *lp;       // glpk
  int *ia, *ja;       // Indices para matriz de restrições
  double *ar;         // Valores da matriz de restrições
  double func_obj;    // Função Objetivo

  LPSolver(Inst&);
  ~LPSolver();

  void resolve();
  void abre_cd(int);
  void fecha_cd(int);
}
