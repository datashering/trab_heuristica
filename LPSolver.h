#pragma once

#include <glpk.h>
#include <vector>
#include <limits>
#include <iostream>
#include "Instancia.h"

const double MAX = std::numeric_limits<double>::max();

struct Indice {
  char name;
  int idx[2];
};

class LPSolver {
public:

  glp_prob *lp;                 // glpk
  std::vector<Indice> lp_var;   // Indices das variáves do lp
  double func_obj;              // Função Objetivo

  LPSolver(Instancia&);
  ~LPSolver();

  void resolve();
  void abre_cd(int);
  void fecha_cd(int);
  //void atualiza_sol(Solucao&);
};
