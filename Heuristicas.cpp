#include <iostream>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <array>
#include <random>
#include "Heuristicas.h"
#include "Uteis.h"
#include "Solver.h"

// Variaveis globais
std::mt19937 gerador(123);

//    ----- Heuristicas da Literatura -----

// Odena CD's candidatos de acordo com metodos da Literatura
void ordena_cadidatos(std::vector<CD> &candi, int metodo, Instancia &dados)
{
  //Criterio que ordena pela capacidade dos CDs em ordem decrescente
  if (metodo == 1)
  {
    for (int i = 0; i < dados.F; i++)
    {
      candi[i].cd = i;
      candi[i].custo = 1.0/dados.h[i];
    }
  }
  //Criterio que ordena o vetor pela razao da capacidade e custo de cada CD em ordem crescente
  if (metodo == 2)
  {
    for (int i = 0; i < dados.F; i++)
    {
      candi[i].cd = i;
      candi[i].custo = dados.b[i]/dados.h[i];
    }
  }

  if (metodo == 3)
  {
    double custo_var;
    int demanda;
    for (int i = 0; i < dados.F; i++)
    {
      demanda = 0;
      custo_var = 0;
      for (int j = 0; j < dados.J; j++)
      {
        demanda += dados.d[j];
        custo_var += dados.d[j]*dados.t[i][j];
      }
      candi[i].cd = i;
      candi[i].custo = 1/((demanda/custo_var) * dados.h[i] - dados.b[i]);
    }
  }

  if (metodo == 4)
  {
     std::vector<double> custos_ofer (dados.I), custos_deman (dados.J);
     double custo_transp, custo_deman;
     for (int i = 0; i < dados.F; i++)
     {
       for (int l = 0; l < dados.I; l++)
       {
         custos_ofer[l] = dados.c[l][i];
       }
       for (int j = 0; j < dados.J; j++)
       {
         custos_deman[j] = dados.t[i][j];
       }

       std::sort (custos_ofer.begin(), custos_ofer.end());
       std::sort (custos_deman.begin(), custos_deman.end());

       custo_transp = 0, custo_deman = 0;
       for (int l = 0; l < dados.I/3; l++)
       {
         custo_transp += custos_ofer[l];
       }
       for (int j = 0; j < dados.J/3; j++)
       {
         custo_deman += custos_deman[j];
       }
       candi[i].custo = (custo_transp/(dados.I/3) + custo_deman/(dados.J/3) + dados.b[i]/dados.h[i]);
       candi[i].cd = i;
     }
  }

  auto compara_custo = [](CD c1, CD c2) { return c1.custo < c2.custo; };
  std::sort (candi.begin(), candi.end(), compara_custo);
}

void drop(Instancia &dados, Solucao &sol)
{
  //E criada uma classe LPSolver para chamar o solver para resolver o problema do transporte
  LPSolver solver(dados);
  //E criado um vetor para avaliar a melhoria na solucao para cada facilidade fechada
  std::vector<bool> abertas (dados.F, 1);
  int indice;
  double custos_fixos = 0, fo_temp = 0;
  bool flag;

  //E resolvido o problema do transporte com todas as facilidades abertas para se obter uma solucao inicial
  solver.resolve();

  //Adiciona-se o custo fixo por abrir todas as facilidades a Solucao
  for (int i = 0; i < dados.F; i++)
  {
    if(abertas[i] == 1)
    {
      custos_fixos = custos_fixos + dados.b[i];
    }
  }
  sol.func_obj = solver.func_obj + custos_fixos;

  //Enquanto fechar facilidade gerar reducao de custos continua-se o procedimento
  do
  {
    //Para a solucao atual e verificado o efeito de se fechar todas as facilidades separadamente,
    //avaliando qual delas resulta o maior ganho
    flag = false;

    for (int i = 0; i < dados.F; i++)
    {
      //E avaliada a nova solucao apenas se a facilidade estiver aberta, caso contrario nao ha necessidade calcular
      if (abertas[i] == 1)
      {
        //std::cout <<"ENTOUR: " << i << std::endl;
        //Fecha-se a facilidade i
        solver.fecha_cd(i, dados);
        abertas[i] = 0;
        custos_fixos = custos_fixos - dados.b[i];

        //E chamada a funcao para resolver o problema do transporte para o vetor atual que retorna a FO
        solver.resolve();

        fo_temp = solver.func_obj + custos_fixos;
        //std::cout << "FUNC : " << fo_temp << std::endl;
        if (fo_temp < sol.func_obj)
        {
          indice = i;
          sol.func_obj = fo_temp;
          flag = true;
        }
        //Volta-se a abrir a facilidade i para avaliar o impacto da proxima
        solver.abre_cd(i, dados);
        abertas[i] = 1;
        custos_fixos = custos_fixos + dados.b[i];
      }
    }
    //Apos o termino da execucao caso a solucao tenha melhorado e atualizado no solver os indices da melhor solucao
    if (flag)
    {
      solver.fecha_cd(indice, dados);
      abertas[indice] = 0;
      custos_fixos = custos_fixos - dados.b[indice];
    }
  }
  while (flag);

}

void add(Instancia &dados, Solucao &sol, int metodos)
{
  LPSolver solver(dados);
  std::vector<bool> abertas (dados.F, 0);
  std::vector<CD> candi (dados.F);
  bool flag;
  int count = 0, indice;
  double custos_fixos = 0, fo_temp = 0;
  int capacidade = 0, demanda_total = 0;

  //Fecha-se todas as facilidades no modelo do GLPK
  for (int i = 0; i < dados.F; i++)
  {
    solver.fecha_cd(i, dados);
  }
  //Soma-se toda a demanda da instancia
  for (int j = 0; j < dados.J; j++)
  {
    demanda_total = demanda_total + dados.d[j];
  }

  //Ordena-se CDs por algum criterio para a inicializacao da Add
  ordena_cadidatos(candi, metodos, dados);

  //Abre-se CDs ate que a demanda seja atendida, abrindo-se na ordem definida pela funcao inicia_add
  while (capacidade < demanda_total)
  {
    //std::cout << "CD: " << candi[count].cd << "Razao Capac: " << candi[count].custo << std::endl;
    abertas[candi[count].cd] = 1;
    solver.abre_cd(candi[count].cd, dados);
    capacidade = capacidade + dados.h[candi[count].cd];
    custos_fixos = custos_fixos + dados.b[candi[count].cd];
    count++;
  }

  //Obtem-se uma solucao viavel inicial para o problema, com os CDs abertos a partir da inicia_Add
  solver.resolve();
  sol.func_obj = solver.func_obj + custos_fixos;
  solver.atualiza_sol(sol, dados);

  //Sao abertas facilidades ate que nao gere uma reducao no custo total do problema
  do
  {
    flag = false;
    //Percorre-se todas as facilidades com excessao da ultima
    for (int i = 0; i < dados.F; i++)
    {
      //Caso a facilidade esteja fechada avalia-se o custo de abri-la
      if (abertas[i] ==  0)
      {
        solver.abre_cd(i, dados);
        abertas[i] = 1;
        custos_fixos = custos_fixos + dados.b[i];

        solver.resolve();
        fo_temp = solver.func_obj + custos_fixos;

      //Caso a abertura da facilidade leve a uma reducao dos custos totais salva-se esse resultado
        if (fo_temp < sol.func_obj)
        {
          //std::cout << "Entrou " << std::endl;
          indice = i;
          //solver.atualiza_sol(melhor_sol);
          sol.func_obj = fo_temp;
          flag = true;
        }

	solver.fecha_cd(i, dados);
	abertas[i] = 0;
	custos_fixos = custos_fixos - dados.b[i];
      }
    }

    if (flag)
    {
      solver.abre_cd(indice, dados);
      abertas[indice] = 1;
      custos_fixos = custos_fixos + dados.b[indice];
    }
  }
  while (flag);
}

//   ----- Heuristica Gulosa -----

//Funcao que gerara um custo medio para cada CD da struct Candidatos
void gera_custo(Instancia& inst, Candidatos& cand)
{
  std::vector<double> custov_medio;
  custov_medio.resize(cand.tamanho);
  double custo_total;
  //E salvo quantos custos variaveis existem para o calculo do custo variavel medio
  int numero_custov = inst.I * inst.F + inst.F * inst.J;

  //Soma-se todos os custos e divide pelo total de custos variaveis (I*F + F*J), desta
  //maneira obtem-se um custo variavel medio para cada um dos CDs
  for (int i = 0; i < cand.tamanho; i++)
  {
    for (int j = 0; j < inst.I; j++)
    {
        custo_total = custo_total + inst.c[j][i];
    }
    for (int j = 0; j < inst.J; j++)
    {
        custo_total = custo_total + inst.t[i][j];
    }
    custov_medio[i] = custo_total/numero_custov;

    cand.viaveis[i].cd = i;
    cand.viaveis[i].custo = (inst.b[i]/inst.h[i] + custov_medio[i]);
  }

}

//Funcao que definira quais CDs abrir e qual o custo total deles ao final
void heuristica_gulosa(Instancia &dados, Solucao &sol)
{
  int cont = 0;
  double custos_fixo = 0;
  int demanda_total = 0;
  LPSolver solver (dados);

  auto compara_custo = [](CD c1, CD c2) { return c1.custo < c2.custo; };

  for (int i = 0; i < dados.J; i++)
  {
    demanda_total = demanda_total + dados.d[i];
    //std::cout << "Demanda: " << demanda_total << std::endl;
  }
  //Cria-se uma struct Candidatos com o tamanho total de Centros de Distribuicao
  Candidatos cand(dados.F);
  //Preenche-se a struct com o custo medio para cada um desses centros
  gera_custo(dados, cand);
  //Ordena-se o vetor de viaveis a partir do custo de cada CD
  std::sort (cand.viaveis.begin(), cand.viaveis.end(), compara_custo);
  //Fecham-se todos os cds no Solver do GLPK
  for (int i = 0; i < dados.F; i++)
  {
    solver.fecha_cd(i, dados);
  }
  //Enquanto nao tiver capacidade suficiente para se atender a demanda total sao abertos CDs
  while (cand.capacidade_total < demanda_total)
  {
    cand.abertos[cont] = cand.viaveis[cont].cd;
    solver.abre_cd(cand.abertos[cont], dados);
    //Adiciona-se a capacidade_total atendida a capacidade do CD aberto
    cand.capacidade_total = cand.capacidade_total + dados.h[cand.abertos[cont]];
    //std::cout << "Capacidade: " << cand.capacidade_total << std::endl;
    custos_fixo += dados.b[cand.abertos[cont]];
    //std::cout << "Custos_fixos: " << custos_fixo << std::endl;
    cont++;
  }
  //Chama-se o solver para resolver o problema do transporte para as facilidades abertas
  solver.resolve();
  solver.atualiza_sol(sol, dados);

}

//  ----- Heuristica Iterativa -----

void inicializa_custos(Instancia &dados, std::vector<double> &custos) {
  std::vector<double> custos_ofer (dados.I), custos_deman (dados.J);

  for (int f = 0; f < dados.F; f++) {
    for (int i = 0; i < dados.I; i++) {
      custos_ofer[i] = dados.c[i][f];
    }

    for (int j = 0; j < dados.J; j++) {
      custos_deman[j] = dados.t[f][j];
    }

    std::sort (custos_ofer.begin(), custos_ofer.end());
    std::sort (custos_deman.begin(), custos_deman.end());

    double custo_ofer = 0, custo_deman = 0;
    for (int i = 0; i < dados.I/3; i++) {
      custo_ofer += custos_ofer[i];
    }

    for (int j = 0; j < dados.J/3; j++) {
      custo_deman += custos_deman[j];
    }

    custos[f] = (custo_ofer/(dados.I/3) + custo_deman/(dados.J/3))*dados.h[f]/2 + dados.b[f];
  }
}

void heuristica_iterativa(Instancia &dados, Solucao &sol, float alpha) {
  ProgDinamica pd(dados);
  LPSolver solver(dados);
  double fo_atual;;

  sol.func_obj = MAX;

  //pd.custos = dados.b;
  inicializa_custos(dados, pd.custos);

  for (int i=0; i<50; i++) {
    // Se a alocação de CDs mudou
    if (pd.resolve()) {
      fo_atual = 0;
      // Atualiza LP e calcula custo fixo
      for (int j=0; j<dados.F; j++) {
        if (pd.sol[j]) {
          solver.abre_cd(j, dados);
          fo_atual += dados.b[j];
        }
        else {
          solver.fecha_cd(j, dados);
        }
      }

      // Recalcula custo variado
      solver.resolve();
      fo_atual += solver.func_obj;

      // Se encontrou solucao melhor atualiza info
      if (fo_atual < sol.func_obj) {
        solver.atualiza_sol(sol, dados);
      }
    }

    //Atualiza custos
    solver.atualiza_custos(dados, pd.custos, alpha);
  }

}

void busca_local() {
  Instancia dados("instancias_c/batch0/0_50_100_200_5.0");
  LPSolver solver(dados);
  Solucao sol(dados.I, dados.F, dados.J);

  // Inicialização solucao com construtiva
  heuristica_gulosa(dados, sol);
  std::cout << "Primeira Solucao = " << sol.func_obj << std::endl;

  double best_fo = sol.func_obj;
  bool end = true;

  while (true) {
    int best_move;

    for (int f=0; f<dados.F; f++) {
      double fo = avalia_t1b(dados, sol, f);

      if (fo == -1) {
        continue;
      }

      else if (fo < best_fo) {
        end  = false;
        best_fo = fo;
        best_move = f;
      }
    }
    std::cout << std::endl;

    if (end) {
      break;
    }
    end = true;

    solver.troca(best_move, dados);
    solver.resolve();
    solver.atualiza_sol(sol, dados);

    std::cout << sol.func_obj << std::endl;
  }

  std::cout << best_fo << std::endl;
}
