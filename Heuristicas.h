#pragma once

#include <iostream>
#include <vector>
#include "Uteis.h"

//  --- Heuristica Gulosa ---

bool compara_custo(const CD&, const CD&);

void gera_custo(Instancia&, Candidatos&);

void heuristica_gulosa(Instancia&, Solucao &sol);

//  --- Heuristica Drop ---

void drop(Instancia&, Solucao&);

//  --- Heuristica Add  ---

void add(Instancia&, Solucao&, int);

//  --- Heuristica Iterativa da Mochila ---

void inicializa_custos(Instancia&, std::vector<double>&);

void heuristica_iterativa(Instancia&, Solucao&, float);

// --- Meta Heuristicas ---

void busca_local();
