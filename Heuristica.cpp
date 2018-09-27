#include <iostream>
#include "Heuristica.h"
#include "Inst.h"

//Funcao que compara dois custos de structs CDs e retorna qual o maior, sera usada no sort
bool Compara_Custo(const CD& c1, const CD& c2)
{
  return c1.custo < c2.custo;
}

//Construtor da struct Candidatos
void Candidatos::Candidatos(int tam)
{
  tam = tam;
  viaveis.resize(Opcoes);
  abertos.reserve(Opcoes);
  capacidade_total = 0;
}

//Funcao que gerara um custo medio para cada CD da struct Candidatos
void Gera_Custo(Inst& inst, Candidatos& cand)
{
  std::vector<double> custov_medio;
  custov_medio.resize(cand.tam);
  double custo_total;
  //E salvo quantos custos variaveis existem para o calculo do custo variavel medio
  int numero_custov = cand.I * cand.F + cand.F * cand.J;

  //Soma-se todos os custos e divide pelo total de custos variaveis (I*F + F*J), desta
  //maneira obtem-se um custo variavel medio para cada um dos CDs
  for (int i = 0; i < cand.tam; i++)
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

  for (int i = 0; i < cand.tam; i++)
  {
    cand.viaveis[i].cd = i;
    cand.viaveis[i].custo = (inst.b[i]/inst.h[i] + custov_medio[i]);
  }
}

//Funcao que definira quais CDs abrir e qual o custo total deles ao final
void Gera_Sol(Inst& inst)
{
  int cont = 0;
  double demanda_total;

  for (int i = 0; i < inst.J; i++)
  {
    demanda_total = demanda_total + d[i];
  }
  //Cria-se uma struct Candidatos com o tamanho total de Centros de Distribuicao
  Candidatos cand(inst.F);
  //Preenche-se a struct com o custo medio para cada um desses centros
  Gera_Razao(inst, cand);
  //Ordena-se o vetor de viaveis a partir do custo de cada CD
  std::sort (Candidatos.viaveis.begin(), Candidatos.viaveis.end(), Compara_Custo);
  //Enquanto nao tiver capacidade suficiente para se atender a demanda total sao abertos CDs
  while (cand.capacidade_total < demanda_total)
  {
    cand.abertos[cont] = cand.viaveis[cont].cd;
    cont++;
    //Adiciona-se a capacidade_total atendida a capacidade do CD aberto
    cand.capacidade_total = cand.capacidade_total + inst.h[cand.abertos[cont]];
  }



}
