#pragma once

#include <vector>
#include <limits>

//  --- Constantes e Typedef  ---
const double MAX = std::numeric_limits<double>::max();

// Struct para guardar instancia
struct Instancia
{
  int I;                      //Numero de fabricas
  int F;                      //Numero de centros de distribuicao (CD)
  int J;                      //Numero de pontos de demanda
  std::vector<int> p;         //Capacidade de producao das fabricas
  std::vector<int> h;         //Capacidade do CD
  std::vector<int> d;         //Quantidade de produto demandado
  std::vector<double> b;         //Custo fixo de abertura do CD
  double **c;                    //Custo de transporte unitario de uma fabrica pra um CD
  double **t;                    //Custo de transporter unitario de um CD para um ponto de demanda
  int d_total;

  Instancia(const char*);
  ~Instancia();
};

// Struct para guardar solução
struct Solucao
{
  int I, F, J;
  double func_obj;
  int cap_total;
  std::vector<bool> y;
  double **x;
  double **z;

  Solucao(int, int, int);
  ~Solucao();
};

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

// Struct para resolver problema com programação dinamica
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

// Lista Tabu
struct TabuLista {

};

// Funcoes diversas
double abre_aprox(Instancia&, const Solucao&, int);
double fecha_aprox(Instancia&, const Solucao&, int);
double avalia_t1b(Instancia&, Solucao&, int);
