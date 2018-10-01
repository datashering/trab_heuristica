#include <glpk.h>
#include <iostream>
#include "Instancia.h"
#include "MIPSolver.h"

const double MAX = std::numeric_limits<double>::max();

void get_info(glp_tree *tree, void *info) {

  double *gap = static_cast<double*>(info);
  double new_gap = glp_ios_mip_gap(tree);

  if (*gap > new_gap) {
    *gap = new_gap;
  }

  return;
}

//   ---  MIPSolver ---
MIPSolver::MIPSolver(Instancia &dados) {
  func_obj = MAX;
  mip_gap = MAX;
  // Instanciando solver do glpk
  mip = glp_create_prob();
  glp_set_prob_name(mip, "Problema da Localização");
  glp_set_obj_dir(mip, GLP_MIN);

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

  int *y_idx = new int[dados.F];


  // Criando modelo
  int count = 0;

  // Var Xif
  glp_add_cols(mip, dados.I * dados.F);
  for (int i=1; i<=dados.I; i++) {
    for (int f=1; f<=dados.F; f++) {
      count++;
      glp_set_col_bnds(mip, count, GLP_LO, 0.0, 0.0);
      glp_set_obj_coef(mip, count, dados.c[i-1][f-1]);
      x_idx[i-1][f-1] = count;
    }
  }

  // Var Zfj
  glp_add_cols(mip, dados.F * dados.J);
  for (int f=1; f<=dados.F; f++) {
    for (int j=1; j<=dados.J; j++) {
      count++;
      glp_set_col_bnds(mip, count, GLP_LO, 0.0, 0.0);
      glp_set_obj_coef(mip, count, dados.t[f-1][j-1]);
      z_idx[f-1][j-1] = count;
    }
  }

  // Var Yf
  glp_add_cols(mip, dados.F);
  for (int f=1; f<=dados.F; f++) {
    count++;
    glp_set_col_kind(mip, count, GLP_BV);
    glp_set_obj_coef(mip, count, dados.b[f-1]);
    y_idx[f-1] = count;
  }

  int row_count = 0;
  count = 0;

  // r1: Toda demanda deve ser atendida
  glp_add_rows(mip, dados.J);
  for (int j=1; j<=dados.J; j++) {
    row_count++;
    glp_set_row_bnds(mip, row_count, GLP_FX, dados.d[j-1], 0.0);

    for (int f=1; f<=dados.F; f++) {
      count++;
      ia[count] = row_count;
      ja[count] = z_idx[f-1][j-1];
      ar[count] = 1;
    }
  }

  // r2: Capacidade do CD + critério de abertura
  glp_add_rows(mip, dados.F);
  for (int f=1; f<=dados.F; f++) {
    row_count++;
    glp_set_row_bnds(mip, row_count, GLP_UP, 0.0, 0.0);

    for (int j=1; j<=dados.J; j++) {
      count++;
      ia[count] = row_count;
      ja[count] = z_idx[f-1][j-1];
      ar[count] = 1;
    }

    count++;
    ia[count] = row_count;
    ja[count] = y_idx[f-1];
    ar[count] = -dados.h[f-1];
  }

  // r3: Conservação de fluxo no CD
  glp_add_rows(mip, dados.F);
  for (int f=1; f<=dados.F; f++) {
    row_count++;
    glp_set_row_bnds(mip, row_count, GLP_FX, 0.0, 0.0);

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
  glp_add_rows(mip, dados.I);
  for (int i=1; i<=dados.I; i++) {
    row_count++;
    glp_set_row_bnds(mip, row_count, GLP_UP, 0.0, dados.p[i-1]);

    for (int f=1; f<=dados.F; f++) {
      count++;
      ia[count] = row_count;
      ja[count] = x_idx[i-1][f-1];
      ar[count] = 1;
    }
  }

  // Carregando modelo
  glp_load_matrix(mip ,count, ia, ja, ar);

  // Desalocando
  for (int i=0; i<dados.I; i++) {
    delete[] x_idx[i];
  }
  delete[] x_idx;

  for (int f=0; f<dados.F; f++) {
    delete[] z_idx[f];
  }
  delete[] z_idx;
  delete[] y_idx;

  delete[] ia;
  delete[] ja;
  delete[] ar;
}

MIPSolver::~MIPSolver() {
  glp_delete_prob(mip);
}

void MIPSolver::resolve() {
  glp_iocp params;

  glp_init_iocp(&params);
  params.presolve = GLP_ON;
  params.msg_lev = GLP_MSG_OFF;
  params.tm_lim = 600000;
  params.cb_func = get_info;
  params.cb_info = &mip_gap;

  glp_intopt(mip, &params);
  func_obj = glp_mip_obj_val(mip);
  if (glp_mip_status(mip) == GLP_OPT) {
    mip_gap = 0;
  }
}
