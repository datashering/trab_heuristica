#include <glpk.h>
#include "Inst.h"
#include "LPSolver.h"

LPSolver::LPSolver(Inst &inst) {
  // Instanciando solver do glpk
  lp = glp_create_prob();
  glp_set_prob_name(lp, "Problema do Transporte");
  glp_set_obj_dir(lp, GLP_MIN);

  // Alocando arrays para matriz de restrições
  int n_linhas = inst.J + 2 * inst.F + inst.I + 1;
  int n_cols = inst.I * inst.J + inst.F * inst.J + 1;

  ia = new int[n_linhas];
  ja = new int[n_cols];
  ar = new double[n_linas * n_colunas];



}

int main() {
  int *ia, *ja;
  double *ar;
  int coef_idx = 1;

  // Criação do problema
  glp_prob *mip = glp_create_prob();
  glp_set_prob_name(mip, "Location Problem");
  glp_set_obj_dir(mip, GLP_MIN);

  // Leitura dos dados e alocação dos vetores de coeficientes
  Inst dados("instancias_c/20-10.dat");
  int max_idx = 2 * dados.F * dados.J + dados.F + 1;
  ia = new int[max_idx];
  ja = new int[max_idx];
  ar = new double[max_idx];

  // Criando matriz de indices para Xij
  int **x_idx = new int*[dados.F + 1];
  for (int f=0; f<dados.F + 1; f++) {
    x_idx[f] = new int[dados.J + 1];
  }
  // Adiocionando colunas referentes a Xij
  glp_add_cols(mip, dados.F*dados.J);
  // Definindo Xij
  for(int f=1; f<=dados.F; f++) {
    for(int j=1; j<=dados.J; j++) {
      glp_set_col_bnds(mip, coef_idx, GLP_LO, 0.0, 0.0);
      glp_set_obj_coef(mip, coef_idx, dados.c[f-1][j-1]);
      x_idx[f][j] = coef_idx;
      coef_idx++;
    }
  }

  // Criando matriz de indices para Yf
  int *y_idx = new int[dados.F + 1];
  glp_add_cols(mip, dados.F);
  // Definindo Yf
  for (int f=1; f<=dados.F; f++) {
    glp_set_col_kind(mip, coef_idx, GLP_BV);
    glp_set_obj_coef(mip, coef_idx, dados.f[f-1]);
    y_idx[f] = coef_idx;
    coef_idx++;
  }

  coef_idx = 1;
  int row_idx = 1;

  glp_add_rows(mip, dados.J);
  for (int j=1; j<=dados.J; j++) {
    glp_set_row_bnds(mip, row_idx, GLP_FX, dados.d[j-1], 0.0);

    for (int f=1; f<=dados.F; f++) {
      ia[coef_idx] = row_idx;
      ja[coef_idx] = x_idx[f][j];
      ar[coef_idx] = 1;
      coef_idx++;
    }
    row_idx++;
  }

  glp_add_rows(mip, dados.F);
  for (int f=1; f<=dados.F; f++) {
    glp_set_row_bnds(mip, row_idx, GLP_UP, 0.0, 0.0);

    for (int j=1; j<=dados.J; j++){
      ia[coef_idx] = row_idx;
      ja[coef_idx] = x_idx[f][j];
      ar[coef_idx] = 1;
      coef_idx++;
    }
    ia[coef_idx] = row_idx;
    ja[coef_idx] = y_idx[f];
    ar[coef_idx] = -dados.b[f-1];
    coef_idx++;
    row_idx++;
  }
  // for (int i=0; i<coef_idx; i++) {
  //   std::cout << i << " " << ja[i] << std::endl;
  // }

  glp_load_matrix(mip, coef_idx-1, ia, ja, ar);

  glp_iocp param;
  glp_init_iocp(&param);
  param.presolve = GLP_ON;

  glp_intopt(mip, &param);

  std::cout << "Objective Fucntion: " << glp_mip_obj_val(mip) << std::endl;


  glp_delete_prob(mip);
  delete[] y_idx;
  for (int f=0; f<dados.F+1; f++) {
    delete[] x_idx[f];
  }
  delete[] x_idx;
}
