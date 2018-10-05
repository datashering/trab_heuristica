#pragma once

#include <iostream>
#include <vector>
#include "Uteis.h"

//  --- Heuristica Gulosa ---

struct CD
{
  int cd;
  double custo;
};

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

bool Compara_Custo(const CD&, const CD&);

void Gera_Custo(Instancia&, Candidatos&);

double Gera_Sol(Instancia&);

//  --- Heuristica Drop ---

double Drop(Instancia&);

//  --- Heuristica Add  ---

double Add(Instancia&, int);

//  --- Heuristica Iterativa da Mochila ---

struct Resultados {
  double fo;
  std::vector<int> itr;
  std::vector<double> vec_fo;
};

struct ProgDinamica {
  int k;
  int demanda;
  std::vector<double> custos;
  std::vector<int> capacidades;
  std::vector<bool> sol;

  double **opt;
  bool **opt_sol;

  ProgDinamica(Instancia&);
  ~ProgDinamica();
  bool resolve();
};

void heuristica_iterativa(Instancia&, float, Resultados&);
