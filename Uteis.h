#pragma once

#include <iostream>
#include <vector>

struct Instancia
{
  int I;                      //Numero de fabricas
  int F;                      //Numero de centros de distribuicao (CD)
  int J;                      //Numero de pontos de demanda
  std::vector<double> p;         //Capacidade de producao das fabricas
  std::vector<double> h;         //Capacidade do CD
  std::vector<double> d;         //Quantidade de produto demandado
  std::vector<double> b;         //Custo fixo de abertura do CD
  double **c;                    //Custo de transporte unitario de uma fabrica pra um CD
  double **t;                    //Custo de transporter unitario de um CD para um ponto de demanda

  Instancia(const char *nome);

  ~Instancia();
};

struct Solucao
{
  double func_obj;
  std::vector<bool> y;
  double **x;
  double **z;

  Solucao(int, int, int);

  ~Solucao();
}