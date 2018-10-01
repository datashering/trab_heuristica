#pragma once

#include <glpk.h>
#include <vector>
#include <limits>
#include <iostream>
#include "Instancia.h"

//const double MAX = std::numeric_limits<double>::max();

class MIPSolver {
public:

  glp_prob *mip;                 // glpk
  double func_obj;               // Função Objetivo
  double mip_gap;                // gap

  MIPSolver(Instancia&);
  ~MIPSolver();

  void resolve();
};
