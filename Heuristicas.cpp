#include <iostream>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include "Heuristicas.h"
#include "Uteis.h"
#include "Solver.h"


//Funcao que compara dois custos de structs CDs e retorna qual o maior, sera usada no sort
bool Compara_Custo(const CD& c1, const CD& c2)
{
  return c1.custo < c2.custo;
}

//  --- Heuristica Drop ---

//Funcao que retorna qual vetor de facilidades abertas gerara a melhor FO a partir do criterio DROP
double Drop(Instancia& dados)
{
  //Sao criadas as structs para armazenar a melhor solucao de uma iteracao assim como a melhor atual
  Solucao melhor_sol = Solucao(dados.I, dados.F, dados.J);
  //E criada uma classe LPSolver para chamar o solver para resolver o problema do transporte
  LPSolver solver(dados);
  //E criado um vetor para avaliar a melhoria na solucao para cada facilidade fechada
  std::vector<bool> abertas (dados.F, 1);
  int indice;
  double custos_fixos = 0, fo_temp = 0, demand = 0, capac = 0;
  bool flag;

  //E resolvido o problema do transporte com todas as facilidades abertas para se obter uma solucao inicial
  solver.resolve();
  solver.atualiza_sol(melhor_sol);

  //Adiciona-se o custo fixo por abrir todas as facilidades a Solucao
  for (int i = 0; i < dados.F; i++)
  {
    if(abertas[i] == 1)
    {
      custos_fixos = custos_fixos + dados.b[i];
    }
  }
  melhor_sol.func_obj = solver.func_obj + custos_fixos;

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
        if (fo_temp < melhor_sol.func_obj)
        {
          indice = i;
          melhor_sol.func_obj = fo_temp;
          solver.atualiza_sol(melhor_sol);
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
  //Enquanto fechar facilidade gerar reducao de custos continua-se o procedimento
  while (flag);

  //Prints para verificar a solucao, seria melhor fazer uma funcao para isso no Instancia.h
  //std::cout << "Funcao Objetivo Final: " << melhor_sol.func_obj << std::endl;
  /*
  for (int i = 0; i < dados.F; i++)
  {
    //std::cout << "Facilidades abertas: " << abertas[i] << std::endl;
    if (abertas[i] == 1)
    {
      capac = capac + dados.h[i];
      std::cout << "CAPACIDADE DE " << i << ": " << dados.h[i] << std::endl;
      std::cout << "Custo DE aber " << i << ": " << dados.b[i] << std::endl;
      std::cout << melhor_sol.y[i] << std::endl;
    }
  }

  for (int j = 0; j < dados.J; j++)
  {
    demand = demand + dados.d[j];
  }

  std::cout << "CAPACIDADE: " << capac << "DEMANDA: " << demand << std::endl;
  */
  return melhor_sol.func_obj;
}

//  --- Heuristica Add ---

//Funcao que inicializa a Add a partir de algum dos criterios definidos
void inicia_Add(Instancia& dados, std::vector<CD>& candi, int metodo)
{
  //Criterio que ordena pela capacidade dos CDs em ordem decrescente
  if (metodo == 1)
  {
    for (int i = 0; i < dados.F; i++)
    {
      candi[i].cd = i;
      candi[i].custo = 1/dados.h[i];
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
    double demanda, custo_var;
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
  std::sort (candi.begin(), candi.end(), Compara_Custo);
}

double Add(Instancia& dados, int metodos)
{
  LPSolver solver(dados);
  Solucao melhor_sol = Solucao(dados.I, dados.F, dados.J);
  std::vector<bool> abertas (dados.F, 0);
  std::vector<CD> candi (dados.F);
  bool flag;
  int count = 0, indice;
  double custos_fixos = 0, fo_temp = 0, capacidade = 0, demanda_total = 0, capac = 0, demand = 0, custo_var = 0;

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
  inicia_Add(dados, candi, metodos);

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
  melhor_sol.func_obj = solver.func_obj + custos_fixos;
  custo_var = solver.func_obj;
  solver.atualiza_sol(melhor_sol);

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
        if (fo_temp < melhor_sol.func_obj)
        {
          //std::cout << "Entrou " << std::endl;
          indice = i;
          solver.atualiza_sol(melhor_sol);
          melhor_sol.func_obj = fo_temp;
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
      custo_var = solver.func_obj;
    }
  }
  while (flag);

  //Prints Gerais para verificar a solucao
  /*
  std::cout << "Funcao Objetivo Final: " << melhor_sol.func_obj << std::endl;

  for (int i = 0; i < dados.F; i++)
  {
    //std::cout << "Facilidades abertas: " << abertas[i] << std::endl;
    if (abertas[i] == 1)
    {
      capac = capac + dados.h[i];
      std::cout << "CAPACIDADE DE " << i << ": " << dados.h[i] << std::endl;
      std::cout << "Custo DE aber " << i << ": " << dados.b[i] << std::endl;
      //std::cout << melhor_sol.y[i] << std::endl;
    }
  }

  for (int j = 0; j < dados.J; j++)
  {
    demand = demand + dados.d[j];
  }

  std::cout << "CAPACIDADE: " << capac << "DEMANDA: " << demand << std::endl;
  */
  //std::cout << "Custo Fixos: " << melhor_sol.func_obj - custo_var << "Custo Variavel: " << custo_var << std::endl;
  return (custo_var/melhor_sol.func_obj);
}

//  --- Heuristica Gulosa ---

//Construtor da struct Candidatos
Candidatos::Candidatos(int tam): tamanho(tam)
{
  viaveis.resize(tam);
  abertos.reserve(tam);
  capacidade_total = 0;
}

//Funcao que gerara um custo medio para cada CD da struct Candidatos
void Gera_Custo(Instancia& inst, Candidatos& cand)
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
double Gera_Sol(Instancia& dados)
{
  int cont = 0;
  double demanda_total = 0, custos_fixo = 0;
  LPSolver solver (dados);
  Solucao melhor_sol (dados.I, dados.F, dados.J);

  for (int i = 0; i < dados.J; i++)
  {
    demanda_total = demanda_total + dados.d[i];
    //std::cout << "Demanda: " << demanda_total << std::endl;
  }
  //Cria-se uma struct Candidatos com o tamanho total de Centros de Distribuicao
  Candidatos cand(dados.F);
  //Preenche-se a struct com o custo medio para cada um desses centros
  Gera_Custo(dados, cand);
  //Ordena-se o vetor de viaveis a partir do custo de cada CD
  std::sort (cand.viaveis.begin(), cand.viaveis.end(), Compara_Custo);
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
  solver.atualiza_sol(melhor_sol);
  melhor_sol.func_obj = custos_fixo + solver.func_obj;

  return melhor_sol.func_obj;
}

//  --- Heuristica Iterativa da Mochila ---

ProgDinamica::ProgDinamica(Instancia &dados) {
  k = dados.F;
  demanda = std::ceil(std::accumulate(dados.d.begin(), dados.d.end(), 0.0));
  capacidades.resize(k);
  for (int i=0; i<k; i++) {
    capacidades[i] = std::floor(dados.h[i]);
  }
  sol.resize(k);
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

void ProgDinamica::resolve() {
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
  double func_obj = 0;

  resto = demanda;
  for (int i=k-1; i>=0; i--) {
    sol[i] = opt_sol[i][resto];

    if (sol[i]) {
      resto = std::max(0, resto - capacidades[i]);
    }
  }
}

double heuristica_iterativa(Instancia &dados, float alpha) {
  ProgDinamica pd(dados);
  LPSolver solver(dados);
  double current_of, best_of = MAX;
  int best_idx;

  pd.custos = dados.b;

  for (int i=0; i<100; i++) {
    pd.resolve();
    current_of = 0;
    for (int i=0; i<dados.F; i++) {
      if (pd.sol[i]) {
        solver.abre_cd(i, dados);
        current_of += dados.b[i];
      }
      else {
        solver.fecha_cd(i, dados);
      }
    }
    solver.resolve();
    solver.atualiza_custos(dados, pd.custos, alpha);
    // std::cout << current_of << " ";
    current_of += solver.func_obj;
    // std::cout << i << ": " << current_of << std::endl;

    if (current_of < best_of) {
      best_of = current_of;
      best_idx = i;
    }
  }

  std::cout << best_idx << std::endl;

  return best_of;
}
