#pragma once

#include <glpk.h>
#include <vector>
#include <limits>
#include <iostream>
#include "Uteis.h"

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
  void abre_cd(int, Instancia&);
  void fecha_cd(int, Instancia&);
  void troca(int, Instancia&);
  void atualiza_sol(Solucao&, Instancia&);
  void atualiza_custos(Instancia&, std::vector<double>&, float);
};

class MIPSolver {
public:

  glp_prob *mip;                 // glpk
  double func_obj;               // Função Objetivo
  double mip_gap;                // gap

  MIPSolver(Instancia&);
  ~MIPSolver();

  void resolve();
};
