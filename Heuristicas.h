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

// --- Tabu Search ---

void busca_local();

//  --- Heuristica Genetica

struct Individuo
{
  double fo;
  std::vector<double> chaves;
  void calcula_fo(Instancia&);
};

struct HiperParam
{
  int tam_pop;
  int qtd_pais;
  int qtd_candidatos;
  int qtd_torneios;
  int parada;
  int sobreviventes;
  double taxa_mutacao;
};

class Genetico
{
public:

  int tam_pop;
  int qtd_pais;
  int qtd_candidatos;
  int qtd_torneios;
  int parada;
  int sobreviventes;
  double taxa_mutacao;

  std::vector<Individuo> populacao;
  std::vector<Individuo> selec;

  Genetico(int, int, int, int, int, int, double, int);
  void inicia_populacao();
  void seleciona_populacao(Instancia&);
  void gera_populacao(double);
  bool mutacao();
  void evolucao(Solucao&, Instancia&);
};
