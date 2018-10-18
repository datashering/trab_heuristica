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
  int capacidade_total;

  Candidatos(int tam);
};

bool compara_custo(const CD&, const CD&);

void gera_custo(Instancia&, Candidatos&);

double gera_sol(Instancia&);

//  --- Heuristica Drop ---

double drop(Instancia&);

//  --- Heuristica Add  ---

double add(Instancia&, int);

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

void inicializa_custos(Instancia&, std::vector<double>&);

void heuristica_iterativa(Instancia&, float, Resultados&);
