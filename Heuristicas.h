#pragma once

#include <iostream>
#include <vector>
#include "Uteis.h"

//  --- Heuristica Gulosa ---

bool compara_custo(const CD&, const CD&);

void gera_custo(Instancia&, Candidatos&);

void heuristica_gulosa(Instancia&, Solucao &sol);

//  --- Heuristica Drop ---

void drop(Instancia&, Solucao&, bool);

//  --- Heuristica Add  ---

void add(Instancia&, Solucao&, int, bool);

//  --- Heuristica Iterativa da Mochila ---

void inicializa_custos(Instancia&, std::vector<double>&);

void heuristica_iterativa(Instancia&, Solucao&, float);

//  --- MetaHeuristicas ---

//  --- Heuristica Genetica

class Genetico
{
public:

  struct populacao
  {
    double fo;
    std::vector<bool> chaves;
  };

  int tam_pop;
  int num_part;
  int parada;
  double mutacao;
  std::vector<populacao> pop;

  Genetico(int, int, int, double, int);
  void inicia_populacao();
  void reproduz_populacao(int);
  void redefine_populacao(int);
  void evolucao(Solucao&);
};
