#pragma once

//  --- Meta Heurísca ---

class TabuList {}

void busca_local();

//  ----- Busca Local -----

void busca_local() {
  Instancia dados("instancias_c/batch0/0_50_100_200_5.0");
  LPSolver solver(dados);
  Solucao sol(dados.I, dados.F, dados.J);

  // Inicialização solucao com construtiva
  heuristica_iterativa(dados, sol, 0.1);
  std::cout << "Primeira Solucao = " << sol.func_obj << std::endl;

  for (int f=0; f<dados.F; f++) {
    if (sol.y[f] == 0) {
      solver.fecha_cd(f, dados);
    }
  }

  for (int itr = 0; itr < 10; itr++) {
    double best_fo = MAX;
    int best_move;

    std::cout << "\nInicio itr = " << itr << std::endl;
    for (int f=0; f<dados.F; f++) {
      double fo = avalia_t1b(dados, sol, f);

      if (fo == -1) {
        continue;
      }

      else if (fo < best_fo) {
        std::cout << fo << std::endl;
        best_fo = fo;
        best_move = f;
      }
    }
    std::cout << best_fo << " ";

    solver.troca(best_move, dados);
    solver.resolve();
    solver.atualiza_sol(sol, dados);

    std::cout << sol.func_obj << std::endl;
    break;
  }

}
