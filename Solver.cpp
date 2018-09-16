#include <iostream>
#include <glpk.h>

//  76, 81, 87, 74, 98, 93, 58, 99, 64, 78, 83, 72, 63, 77, 77, 76, 88, 79, 89, 59

int main() {
  double demand[20] = { 76, 81, 87, 74, 98, 93, 58, 99, 64, 78, 83, 72, 63, 77, 77, 76, 88, 79, 89, 59 };
  double capacity[10] = { 400, 50, 50, 250, 150, 100, 100, 450, 500, 300 };
  double f_cost[10] = { 240, 212, 445, 263, 267, 253, 321, 284, 186, 374  };
  double v_cost[10][20] = {
    { 33, 37, 23, 12, 29, 20, 29, 37, 34, 37, 31, 11, 39, 43, 27, 15, 35, 20, 10, 31 },
    { 29, 33, 15, 27, 28, 18, 12, 32, 24, 38, 46, 47, 14, 15, 29, 48, 34, 20, 17, 38 },
    { 15, 19, 19, 38, 36, 10, 15, 33, 32, 19, 33, 10, 29, 13, 31, 46, 23, 14, 11, 28 },
    { 31, 23, 49, 11, 37, 42, 26, 20, 49, 40, 38, 38, 46, 27, 47, 25, 32, 31, 43, 22 },
    { 17, 33, 21, 13, 27, 15, 29, 34, 36, 45, 25, 40, 41, 19, 38, 12, 21, 10, 36, 39 },
    { 26, 34, 20, 18, 15, 27, 25, 46, 40, 29, 26, 10, 43, 27, 21, 13, 34, 13, 41, 20 },
    { 30, 27, 47, 24, 23, 41, 39, 10, 49, 45, 30, 18, 27, 49, 47, 48, 13, 12, 42, 41 },
    { 36, 38, 43, 22, 32, 35, 21, 16, 46, 44, 39, 49, 18, 35, 49, 28, 12, 30, 23, 28 },
    { 44, 36, 49, 18, 37, 35, 24, 39, 47, 42, 22, 34, 18, 33, 25, 20, 31, 20, 31, 23 },
    { 29, 28, 13, 10, 20, 37, 11, 48, 38, 49, 32, 22, 22, 14, 10, 47, 39, 23, 40, 31 }
  };

  glp_prob *mip = glp_create_prob();
  int ia[1001], ja[1001];
  double ar[1001];
  glp_set_prob_name(mip, "Location Problem");
  glp_set_obj_dir(mip, GLP_MIN);

  int idx = 1;

  glp_add_cols(mip, 20*10);
  for(int i=1; i<=10; i++) {
    for(int j=1; j<=20; j++) {
      glp_set_col_bnds(mip, idx, GLP_LO, 0.0, 0.0);
      glp_set_obj_coef(mip, idx, v_cost[i-1][j-1]);
      idx++;
    }
  }

  glp_add_cols(mip, 10);
  for (int i=1; i<=10; i++) {
    glp_set_col_kind(mip, idx, GLP_BV);
    glp_set_obj_coef(mip, idx, f_cost[i-1]);
    idx++;
  }

  idx = 1;

  glp_add_rows(mip, 20);
  for (int i=1; i<=20; i++) {
    glp_set_row_bnds(mip, i, GLP_FX, demand[i-1], 0.0);

    for (int j=0; j<10; j++) {
      ia[idx] = i;
      ja[idx] = j*20 + i;
      ar[idx] = 1;
      idx++;
    }
  }

  glp_add_rows(mip, 10);
  for (int i=1; i<=10; i++) {
    glp_set_row_bnds(mip, i+20, GLP_UP, 0.0, 0.0);

    for (int j=1; j<=20; j++){
      ia[idx] = i + 20;
      ja[idx] = j + (i-1)*20;
      ar[idx] = 1;
      idx++;
    }
    ia[idx] = i + 20;
    ja[idx] = (10*20) + i;
    ar[idx] = -capacity[i-1];
    idx++;
  }

  glp_load_matrix(mip, idx-1, ia, ja, ar);

  glp_iocp param;
  glp_init_iocp(&param);
  param.presolve = GLP_ON;

  glp_intopt(mip, &param);

  std::cout << "Objective Fucntion: " << glp_mip_obj_val(mip) << std::endl;


  glp_delete_prob(mip);
}
