#include <glpk.h>
#include <iostream>
#include "Instancia.h"
#include "LPSolver.h"

//   ---  Indice  ---
std::ostream& operator << (std::ostream &o, const Indice &a) {
  o << a.name << "(" << a.idx[0] << "," << a.idx[1] << ")";
  return o;
}

//   ---  LPSolver ---
LPSolver::LPSolver(Instancia &dados) {
  func_obj = MAX;
  // Instanciando solver do glpk
  lp = glp_create_prob();
  glp_set_prob_name(lp, "Problema do Transporte");
  glp_set_obj_dir(lp, GLP_MIN);

  // Alocando arrays para matriz de restrições  IF + JF  F(I + J)
  int max_size = (dados.J + 2*dados.F + dados.I) * (dados.F * (dados.I + dados.J)) + 1;

  int *ia = new int[max_size];
  int *ja = new int[max_size];
  double *ar = new double[max_size];

  // Alocando vetor de indices das variáveis modelo -> glpk
  int **x_idx = new int*[dados.I];
  for (int i=0; i<dados.I; i++) {
    x_idx[i] = new int[dados.F];
  }

  int **z_idx = new int*[dados.F];
  for (int f=0; f<dados.F; f++) {
    z_idx[f] = new int[dados.J];
  }

  // Alocando vetor de indices das variáveis glpk -> modelo
  int count = 0;
  lp_var.resize(dados.I * dados.F + dados.F * dados.J + 1);

  // Criando modelo
  // Var Xif
  glp_add_cols(lp, dados.I * dados.F);
  for (int i=1; i<=dados.I; i++) {
    for (int f=1; f<=dados.F; f++) {
      count++;
      glp_set_col_bnds(lp, count, GLP_LO, 0.0, 0.0);
      glp_set_obj_coef(lp, count, dados.c[i-1][f-1]);
      x_idx[i-1][f-1] = count;
      lp_var[count].name = 'x';
      lp_var[count].idx[0] = i-1;
      lp_var[count].idx[1] = f-1;
    }
  }

  // Var Zfj
  glp_add_cols(lp, dados.F * dados.J);
  for (int f=1; f<=dados.F; f++) {
    for (int j=1; j<=dados.J; j++) {
      count++;
      glp_set_col_bnds(lp, count, GLP_LO, 0.0, 0.0);
      glp_set_obj_coef(lp, count, dados.t[f-1][j-1]);
      z_idx[f-1][j-1] = count;
      lp_var[count].name = 'z';
      lp_var[count].idx[0] = f - 1;
      lp_var[count].idx[1] = j - 1;
    }
  }

  int row_count = 0;
  count = 0;

  // r1: Toda demanda deve ser atendida
  glp_add_rows(lp, dados.J);
  for (int j=1; j<=dados.J; j++) {
    row_count++;
    glp_set_row_bnds(lp, row_count, GLP_FX, dados.d[j-1], 0.0);

    for (int f=1; f<=dados.F; f++) {
      count++;
      ia[count] = row_count;
      ja[count] = z_idx[f-1][j-1];
      ar[count] = 1;
    }
  }

  // r2: Capacidade do CD + critério de abertura
  glp_add_rows(lp, dados.F);
  for (int f=1; f<=dados.F; f++) {
    row_count++;
    glp_set_row_bnds(lp, row_count, GLP_UP, 0.0, dados.h[f-1]);

    for (int j=1; j<=dados.J; j++) {
      count++;
      ia[count] = row_count;
      ja[count] = z_idx[f-1][j-1];
      ar[count] = 1;
    }
  }

  // r3: Conservação de fluxo no CD
  glp_add_rows(lp, dados.F);
  for (int f=1; f<=dados.F; f++) {
    row_count++;
    glp_set_row_bnds(lp, row_count, GLP_FX, 0.0, 0.0);

    for (int i=1; i<=dados.I; i++) {
      count++;
      ia[count] = row_count;
      ja[count] = x_idx[i-1][f-1];
      ar[count] = 1;
    }

    for (int j=1; j<=dados.J; j++) {
      count++;
      ia[count] = row_count;
      ja[count] = z_idx[f-1][j-1];
      ar[count] = -1;
    }
  }

  // r4: Capacidade de produção da fábrica
  glp_add_rows(lp, dados.F);
  for (int i=1; i<=dados.I; i++) {
    row_count++;
    glp_set_row_bnds(lp, row_count, GLP_UP, 0.0, dados.p[i-1]);

    for (int f=1; f<=dados.F; f++) {
      count++;
      ia[count] = row_count;
      ja[count] = x_idx[i-1][f-1];
      ar[count] = 1;
    }
  }

  // Carregando modelo
  glp_load_matrix(lp,count, ia, ja, ar);

  // Desalocando
  for (int i=0; i<dados.I; i++) {
    delete[] x_idx[i];
  }
  delete[] x_idx;

  for (int f=0; f<dados.F; f++) {
    delete[] z_idx[f];
  }
  delete[] z_idx;

  delete[] ia;
  delete[] ja;
  delete[] ar;
}

LPSolver::~LPSolver() {
  glp_delete_prob(lp);
}

void LPSolver::resolve() {
  glp_smcp params;

  glp_init_smcp(&params);
  params.presolve = GLP_ON;
  params.tm_lim = 180;

  glp_simplex(lp, &params);
  func_obj = glp_get_obj_val(lp);

  // std::cout << func_obj << std::endl;
  // double val;
  // for (int i=1; i<lp_var.size(); i++) {
  //   val = glp_get_col_prim(lp, i);
  //   if (val > 0) {
  //     std::cout << lp_var[i] << " = " << val << std::endl;
  //   }
  // }

}

void LPSolver::abre_cd(int idx) {

}

void LPSolver::fecha_cd(int idx) {

}
