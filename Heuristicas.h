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

bool Compara_Custo(const CD& c1, const CD& c2);

void Gera_Custo(Instancia& inst, Candidatos& cand);

void Gera_Sol(Instancia& inst);


//  --- Heuristica Drop ---

Solucao Drop(Instancia&);

//TODO
void Balancea_Instancia(Instancia&, std::vector<bool>&);

//TODO
//  --- Heuristica Add  ---

//  --- Heuristica Iterativa da Mochila ---

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
  void resolve();
};

void heuristica_iterativa (Instancia &dados);
