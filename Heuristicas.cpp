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
std::mt19937 generator(123);

// Funcoes Uteis
double abre_aprox(Instancia &dados, const Solucao &sol, int cd) {
  typedef std::array<double, 4> Tuple;
  std::vector<Tuple> ganho;

  // Calculando o ganho de redirecionar o arco (i,f,j) -> (i,cd,j)
  ganho.reserve(dados.I * dados.F * dados.J);
  for (int f=0; f<dados.F; f++) {
    if (sol.y[f] == 0) {
      continue;
    }

    for (int j=0; j<dados.J; j++) {
      if (sol.z[f][j] == 0) {
        continue;
      }

      for (int i=0; i<dados.I; i++) {
        if (sol.x[i][f] == 0) {
          continue;
        }

        Tuple g;
        g[0] = i;
        g[1] = f;
        g[2] = j;
        g[3] = dados.c[i][f] - dados.c[i][cd] + dados.t[f][j] - dados.t[cd][j];
        ganho.push_back(g);
      }
    }
  }

  // Ordenação (decrescente) do vetor de ganho
  auto comparador = [](Tuple a, Tuple b) { return a[3] > b[3]; };
  std::sort(ganho.begin(), ganho.end(), comparador);

  double redirecionado = 0;
  double fo = sol.func_obj;

  for (Tuple t : ganho) {
    // Caso não tenho mais ganhos interromper
    if (t[3] <=0) {
      break;
    }

    int i = t[0];
    int f = t[1];
    int j = t[2];

    // Definindo a quantidade a ser redirecioada
    double qtd = std::min(sol.x[i][f], sol.z[f][j]);
    qtd = std::min(qtd, dados.h[cd] - redirecionado);
    redirecionado += qtd;

    // Atualização da solução
    fo -= qtd * t[3];
    //sol.x[i][f] -= qtd;
    //sol.z[f][j] -= qtd;
    //sol.x[i][cd] += qtd;
    //sol.z[cd][j] += qtd;

    // Acabou a capacidade interrompe
    if (redirecionado == dados.h[cd]) {
      break;
    }
  }

  return fo + dados.b[cd];

}

double fecha_aprox(Instancia &dados, const Solucao &sol, int cd) {
  typedef std::array<double, 3> Tuple;

  // Calculo a capacidade restante de cada CD aberto
  std::vector<int> cap_cd(dados.F);
  std::vector<int> cds_abertos;
  cds_abertos.reserve(dados.F);

  for (int f=0; f<dados.F; f++) {
    if (sol.y[f] == 0 || f == cd) {
      cap_cd[f] = 0;
      continue;
    }

    int sum = 0;
    for (int i=0; i<dados.I; i++) {
      sum += sol.x[i][f];
    }

    cds_abertos.push_back(f);
    cap_cd[f] = dados.h[f] - sum;
  }

  // Calculo da capacidade restante de cada oferta
  std::vector<int> cap_prod(dados.I);
  for (int i=0; i<dados.I; i++) {
    int sum = 0;

    for (int f : cds_abertos) {
      sum += sol.x[i][f];
    }

    cap_prod[i] = dados.p[i] - sum;
  }

  // Criando vetores de redirecionamento
  std::vector<int> rx(dados.I, 0);
  std::vector<int> rz(dados.J, 0);

  // Atender cada cliente de forma gulosa
  double fo = sol.func_obj - dados.b[cd];

  for (int j=0; j<dados.J; j++) {
    if (sol.z[cd][j] - rz[j] == 0) {
      continue;
    }

    std::vector<Tuple> custos;
    custos.reserve(dados.F * dados.I);

    // Calculando o custo de redirecionar o arco (i,cd,j) -> (i,f,j)
    for (int i=0; i<dados.I; i++) {
      if (cap_prod[i] == 0 || sol.x[i][cd] - rx[i] == 0) {
        continue;
      }

      for (int f : cds_abertos) {
        if (cap_cd[f] == 0) {
          continue;
        }

        Tuple t;
        t[0] = i;
        t[1] = f;
        t[2] = dados.c[i][f] - dados.c[i][cd] + dados.t[f][j] - dados.t[cd][j];
        custos.push_back(t);
      }
    }

    // Ordenação (crescente) do vetor de custos
    auto comparador = [](Tuple a, Tuple b) { return a[2] < b[2]; };
    std::sort(custos.begin(), custos.end(), comparador);

    for (Tuple t : custos) {
      int i =  t[0];
      int f = t[1];

      int resto = sol.z[cd][j] - rz[j];

      int qtd = std::min(cap_cd[f], cap_prod[i]);
      qtd = std::min(qtd, resto);

      cap_cd[f] -= qtd;
      cap_prod[i] -= qtd;

      fo += qtd * t[2];
      rx[i] += qtd;
      rz[j] += qtd;
      // sol.x[i][cd] -= qtd;
      // sol.z[cd][j] -= qtd;
      // sol.x[i][f] += qtd;
      // sol.z[f][j] += qtd;

      if (sol.z[cd][j] - rz[j] == 0) {
        break;
      }

    }
  }

  return fo;
}

double avalia_t1b(Instancia &dados, Solucao &sol, int cd) {
  if (sol.y[cd] == 0) {
    return abre_aprox(dados, sol, cd);
  }

  else if (dados.d_total < sol.cap_total - dados.b[cd]) {
    return fecha_aprox(dados, sol, cd);
  }

  else {
    return -1;
  }
}

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

//Construtor da struct Candidatos
Candidatos::Candidatos(int tam): tamanho(tam)
{
  viaveis.resize(tam);
  abertos.reserve(tam);
  capacidade_total = 0;
}

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
  sol.func_obj = custos_fixo + solver.func_obj;
}

//  ----- Heuristica Iterativa -----

ProgDinamica::ProgDinamica(Instancia &dados) {
  k = dados.F;
  demanda = std::accumulate(dados.d.begin(), dados.d.end(), 0.0);

  capacidades = dados.h;

  sol.resize(k);
  for (int i=0; i<k; i++) {
    sol[i] = true;
  }
  custos.resize(k);

  opt = new double*[k];
  opt_sol = new bool*[k];
  for (int i=0; i<k; i++) {
    opt[i] = new double[demanda + 1];
    opt_sol[i] = new bool[demanda + 1];
  }
}

ProgDinamica::~ProgDinamica() {
  for (int i=0; i<k; i++) {
    delete[] opt[i];
    delete[] opt_sol[i];
  }
  delete[] opt;
  delete[] opt_sol;
}

bool ProgDinamica::resolve() {
  // Caso base
  opt[0][0] = 0;      // Se a demanda = 0 -> opt = 0
  opt_sol[0][0] = 0;  // Nao abre CD
  for (int d=1; d<=demanda; d++) {
    // Se a demanda > capacidade problema inviavel
    if (d > capacidades[0]) {
      opt[0][d] = MAX;
    }
    // Se a demanda < capacidade abre CD
    else {
      opt[0][d] = custos[0];
      opt_sol[0][d] = 1;
    }
  }

  // Caso geral
  int resto;
  for (int i=1; i<k; i++) {
    opt[i][0] = 0;      // Se a demanda = 0, opt = 0
    opt_sol[i][0] = 0;  // Nao abre CD

    for (int j=1; j<=demanda; j++) {
      resto = std::max(0, j - capacidades[i]);

      if (opt[i-1][j] > opt[i-1][resto] + custos[i]) {
        // Abre CD
        opt[i][j] = opt[i-1][resto] + custos[i];
        opt_sol[i][j] = 1;
      }

      else {
        // Nao abre CD
        opt[i][j] = opt[i-1][j];
        opt_sol[i][j] = 0;
      }
    }
  }

  // Reconstruindo solucao
  bool flag = false;

  resto = demanda;
  for (int i=k-1; i>=0; i--) {

    if (sol[i] != opt_sol[i][resto]) {
      sol[i] = opt_sol[i][resto];
      flag = true;
    }

    if (sol[i]) {
      resto = std::max(0, resto - capacidades[i]);
    }
  }
  return flag;
}

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
