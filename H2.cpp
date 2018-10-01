#include <iostream>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include "Instancia.h"
#include "LPSolver.h"

void problema_atedimento(Instancia &dados, std::vector<double> &custos, std::vector<bool> &sol) {
  int k = dados.F;
  int demanda = std::ceil(std::accumulate(dados.d.begin(), dados.d.end(), 0.0));

  double **opt = new double*[k];
  bool **opt_sol = new bool*[k];

  for (int i=0; i<k; i++) {
    opt[i] = new double[demanda + 1];
    opt_sol[i] = new bool[demanda + 1];
  }

  opt[0][0] = 0;
  opt_sol[0][0] = 0;
  for (int j=1; j<=demanda; j++) {
    if (j > dados.h[0]) {
      opt[0][j] = MAX;
    }

    else {
      opt[0][j] = custos[0];
      opt_sol[0][j] = 1;
    }
  }

  int resto;
  for (int i=1; i<k; i++) {
    opt[i][0] = 0;
    opt_sol[i][0] = 0;
    for (int j=1; j<=demanda; j++) {
      resto = std::max(0.0, std::floor(j - dados.h[i]));
      if (opt[i-1][j] > opt[i-1][resto] + custos[i]) {
        opt[i][j] = opt[i-1][resto] + custos[i];
        opt_sol[i][j] = 1;
      }
      else {
        opt[i][j] = opt[i-1][j];
        opt_sol[i][j] = 0;
      }
    }
  }

  resto = demanda;
  for (int i=k-1; i>=0; i--) {
    sol[i] = opt_sol[i][resto];

    if (sol[i]) {
      resto = std::max(0.0, std::floor(resto - dados.h[i]));
    }
  }
  double func_obj = opt[k-1][demanda];

  for (int i=0; i<k; i++) {
    delete[] opt[i];
    delete[] opt_sol[i];
  }
  delete[] opt;
  delete[] opt_sol;
}

int main() {
  Instancia dados("instancias_c/batch1/3-40-30-30-2.0");
  LPSolver solver(dados);
  std::vector<bool> sol(dados.F);
  std::vector<double> custos(dados.b);

  problema_atedimento(dados, custos, sol);

  double demanda = std::accumulate(dados.d.begin(), dados.d.end(), 0.0);
  double oferta = 0;
  for (int i=0; i<dados.F; i++) {
    if (sol[i]) {
      oferta += dados.h[i];
    }
  }
  std::cout << demanda << " - " << oferta << std::endl;

  double func_obj = 0;
  for (int i=0; i<dados.F; i++) {
    if (sol[i]) {
      solver.abre_cd(i, dados);
      func_obj += dados.b[i];
    }
    else {
      solver.fecha_cd(i, dados);
    }
  }

  solver.resolve();

  std::cout << func_obj + solver.func_obj << std::endl;

}
