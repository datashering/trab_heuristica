#pragma once

#include <iostream>
#include <vector>

struct CD
{
  int cd;
  double custo;
}

struct Candidatos
{
  //Tamanho total de candidatos
  int tamanho;
  //Vetor com os CDs que ainda podem ser abertos
  std::vector<CD> viaveis;
  //Vetor com os CDs que foram abertos
  std::vector<int> abertos;
  //Capacidade total dos CDs ja abertos
  double capacidade_total;

  Candidatos(int tam);
};

bool Compara_Custo(const CD& c1, const CD& c2);

void Gera_Custo(Inst& inst, Candidatos& cand);

void Gera_Sol(Inst& inst);
