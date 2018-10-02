#include <iostream>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include "Heuristicas.h"
#include "Uteis.h"
#include "Solver.h"

//  --- Heuristica Drop ---

//TODO
/*Funcao que servira para balancear a instancia para a solucao do PDT, nao sei se e
necessario no caso da DROP, se auxilia ou se nao muda para o solver, de qlq maneira
servira para criar uma facilidade ficticia com o custo de envio mais barato que os da abertas

void Balancea_Instancia(Instancia& dados, std::vector<bool>& abertas)
{

}
*/

//Funcao que retorna qual vetor de facilidades abertas gerara a melhor FO a partir do criterio DROP
void Drop(Instancia& dados)
{
  //Sao criadas as structs para armazenar a melhor solucao de uma iteracao assim como a melhor atual
  Solucao melhor_sol = Solucao(dados.I, dados.F, dados.J);
  //E criada uma classe LPSolver para chamar o solver para resolver o problema do transporte
  LPSolver solver(dados);
  //E criado um vetor para avaliar a melhoria na solucao para cada facilidade fechada
  std::vector<bool> abertas (dados.F, true);
  int indice;
  double custos_fixos = 0, fo_temp = 0, demand = 0, capac = 0;
  bool flag;

  //E resolvido o problema do transporte com todas as facilidades abertas para se obter uma solucao inicial
  solver.resolve(dados);
  std::cout << "Funcao Objetivo Solver: " << solver.func_obj << std::endl;
  //solver.atualiza_sol(melhor_sol);

  for (int i = 0; i < dados.F; i++)
  {
    if(abertas[i] == true)
    {
      custos_fixos = custos_fixos + dados.b[i];
    }
  }
  melhor_sol.func_obj = solver.func_obj + custos_fixos;
  std::cout << "Funcao Objetivo: " << melhor_sol.func_obj << std::endl;

  do
  {
    //Para a solucao atual e verificado o efeito de se fechar todas as facilidades separadamente,
    //avaliando qual delas resulta o maior ganho
    flag = false;
    //std::cout << solver.func_obj << std::endl;
    //abertas = melhor_sol.y;
    for (int i = 0; i < dados.F; i++)
    {
      custos_fixos = 0;
      //E avaliada a nova solucao apenas se a facilidade estiver aberta, caso contrario nao ha necessidade calcular
      if (abertas[i] == true)
      {
        //std::cout <<"ENTOUR: " << i << std::endl;
        //Fecha-se a facilidade i
        solver.fecha_cd(i, dados);
        abertas[i] = 0;

        //E chamada a funcao para resolver o problema do transporte para o vetor atual que retorna a FO
        solver.resolve(dados);

        for (int j = 0; j < dados.F; j++)
        {
          if(abertas[j] == true)
          {
            custos_fixos = custos_fixos + dados.b[j];
          }
        }
        fo_temp = solver.func_obj + custos_fixos;
        //std::cout << "FUNC : " << fo_temp << std::endl;
        if (fo_temp < melhor_sol.func_obj)
        {
          indice = i;
          //solver.atualiza_sol(melhor_sol);
          melhor_sol.func_obj = fo_temp;
          flag = true;
        }
        //Volta-se a abrir a facilidade i para avaliar o impacto da proxima
        solver.abre_cd(i, dados);
        abertas[i] = 1;
      }
    }
    //Apos o termino da execucao caso a solucao tenha melhorado e atualizado no solver os indices da melhor solucao
    if (flag)
    {
      solver.fecha_cd(indice, dados);
      abertas[indice] = 0;
    }
  }
  //Enquanto fechar facilidade gerar reducao de custos continua-se o procedimento
  while (flag);

  //Prints para verificar a solucao, seria melhor fazer uma funcao para isso no Instancia.h

  std::cout << "Funcao Objetivo FInal: " << melhor_sol.func_obj << std::endl;
  for (int i = 0; i < dados.F; i++)
  {
    //std::cout << "Facilidades abertas: " << abertas[i] << std::endl;
    if (abertas[i] == 1)
    {
      capac = capac + dados.h[i];
      std::cout << "CAPACIDADE DE " << i << ": " << dados.h[i] << std::endl;
      std::cout << "Custo DE aber " << i << ": " << dados.b[i] << std::endl;
    }
  }
  for (int j = 0; j < dados.J; j++)
  {
    demand = demand + dados.d[j];
  }

  std::cout << "CAPCADIDADE: " << capac << "DEMANDA: " << demand << std::endl;


}

//TODO
//  --- Heuristica Add ---

//  --- Heuristica Gulosa ---

//Funcao que compara dois custos de structs CDs e retorna qual o maior, sera usada no sort
bool Compara_Custo(const CD& c1, const CD& c2)
{
  return c1.custo < c2.custo;

}

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
  }

  for (int i = 0; i < cand.tamanho; i++)
  {
    cand.viaveis[i].cd = i;
    cand.viaveis[i].custo = (inst.b[i]/inst.h[i] + custov_medio[i]);
  }
}

//Funcao que definira quais CDs abrir e qual o custo total deles ao final
void Gera_Sol(Instancia& inst)
{
  int cont = 0;
  double demanda_total;

  for (int i = 0; i < inst.J; i++)
  {
    demanda_total = demanda_total + inst.d[i];
  }
  //Cria-se uma struct Candidatos com o tamanho total de Centros de Distribuicao
  Candidatos cand(inst.F);
  //Preenche-se a struct com o custo medio para cada um desses centros
  Gera_Custo(inst, cand);
  //Ordena-se o vetor de viaveis a partir do custo de cada CD
  std::sort (cand.viaveis.begin(), cand.viaveis.end(), Compara_Custo);
  //Enquanto nao tiver capacidade suficiente para se atender a demanda total sao abertos CDs
  while (cand.capacidade_total < demanda_total)
  {
    cand.abertos[cont] = cand.viaveis[cont].cd;
    cont++;
    //Adiciona-se a capacidade_total atendida a capacidade do CD aberto
    cand.capacidade_total = cand.capacidade_total + inst.h[cand.abertos[cont]];
  }

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

double heuristica_iterativa(Instancia &dados) {
  ProgDinamica pd(dados);
  LPSolver solver(dados);
  double current_of, best_of = MAX;

  pd.custos = dados.b;
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
  solver.resolve(dados);
  current_of += solver.func_obj;

  return current_of;
}
