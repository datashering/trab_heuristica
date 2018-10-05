#include <glpk.h>
#include <iostream>
#include "Uteis.h"
#include "Solver.h"

//  --- Funcoes para uso interno  ---
void get_info(glp_tree *tree, void *info) {

  double *gap = static_cast<double*>(info);
  double new_gap = glp_ios_mip_gap(tree);

  if (*gap > new_gap) {
    *gap = new_gap;
  }

  return;
}

//   ---  Struct Indice  ---
std::ostream& operator << (std::ostream &o, const Indice &a) {
  o << a.name << "(" << a.idx[0] << "," << a.idx[1] << ")";
  return o;
}

//   ---  Class LPSolver ---
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

  // r2: Capacidade do CD
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
  glp_add_rows(lp, dados.I);
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
//Deve verificar se o get_status retorna da forma como escrita
void LPSolver::resolve() {
  glp_smcp params;
  int glp_status;

  glp_init_smcp(&params);
  params.presolve = GLP_ON;
  params.msg_lev = GLP_MSG_OFF;
  //params.tm_lim = 180;

  //Verificar a flag para o caso do problema ser inviavel
  glp_simplex(lp, &params);
  glp_status = glp_get_status(lp);

  //Funcao que retorna status sobre viabilidade do problema
  if (glp_status != GLP_OPT) {
    //Atribui-se um valor extremamente a sua funcao obj
    func_obj = MAX;
  }
  else {
    func_obj = glp_get_obj_val(lp);
  }
  // std::cout << func_obj << std::endl;
  // double val;
  // for (int i=1; i<lp_var.size(); i++) {
  //   val = glp_get_col_prim(lp, i);
  //   if (val > 0) {
  //     std::cout << lp_var[i] << " = " << val << std::endl;
  //   }
  // }

}

void LPSolver::abre_cd(int idx, Instancia &dados) {
  glp_set_row_bnds(lp, dados.J + idx + 1, GLP_UP, 0.0, dados.h[idx]);
}

void LPSolver::fecha_cd(int idx, Instancia &dados) {
  glp_set_row_bnds(lp, dados.J + idx + 1, GLP_UP, 0.0, 0.0);
}

//TOCHECK
void LPSolver::atualiza_sol(Solucao &sol) {

  //Atualizando Vetor de facilidades abertas
  for (int i = sol.J + 1; i < sol.J + sol.F + 1; i++)
  {
    sol.y[i - sol.J + 1] = static_cast<bool> (glp_get_row_ub(lp, i));
  }
  //Atualizando as matrizes com as variveis de transporte
  for (int i = 0; i < lp_var.size(); i++)
  {
    if (lp_var[i].name == 'x')
    {
      sol.x[lp_var[i].idx[0]][lp_var[i].idx[1]] = glp_get_col_prim(lp, i);
    }
    if (lp_var[i].name == 'z')
    {
      sol.z[lp_var[i].idx[0]][lp_var[i].idx[1]] = glp_get_col_prim(lp, i);
    }
  }

}

void LPSolver::atualiza_custos(Instancia &dados, std::vector<double> &custos, float alpha) {
  int N = lp_var.size();
  int F = dados.F;
  int i, f, j;
  std::vector<double> custo_transp(F, 0);

  for (int idx=1; idx<N; idx++) {
    if (glp_get_col_prim(lp, idx) == 0) {
      continue;
    }

    if (lp_var[idx].name == 'x') {
      i = lp_var[idx].idx[0];
      f = lp_var[idx].idx[1];
      custo_transp[f] += glp_get_col_prim(lp, idx) * dados.c[i][f];
    }

    else if (lp_var[idx].name == 'z') {
      f = lp_var[idx].idx[0];
      j = lp_var[idx].idx[1];
      custo_transp[f] += glp_get_col_prim(lp, idx) * dados.t[f][j];
    }

    else {
      std::cout << "Erro: variável não chama nem x nem z" << std::endl;
    }
  }

  for (int f=0; f<F; f++) {
    if (custo_transp[f] != 0) {
      custos[f] = (1 - alpha)*custos[f] +  alpha*(dados.b[f] + custo_transp[f]);
    }
  }

}

//   --- Class MIPSolver ---
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
  params.tm_lim = 900000;
  params.cb_func = get_info;
  params.cb_info = &mip_gap;

  glp_intopt(mip, &params);
  func_obj = glp_mip_obj_val(mip);
  if (glp_mip_status(mip) == GLP_OPT) {
    mip_gap = 0;
  }
}
