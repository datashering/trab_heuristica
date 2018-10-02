#include <fstream>
#include <iostream>
#include "Uteis.h"

Solucao::Solucao(int I, int F, int J): I(I), F(F), J(J)
{
  y.resize(F);
  x = new double*[I];
  for (int i = 0; i < I; i++)
  {
    x[i] = new double[F];
  }
  z = new double*[F];
  for (int i = 0; i < F; i++)
  {
    z[i] = new double[J];
  }
}

Solucao::~Solucao()
{
  //Desalocando o vetor de custo variavel c
  for (int i = 0; i < I; i++)
  {
    delete[] x[i];
  }
  delete[] x;

  //Desalocando o vetor de custo variavel t
  for (int i = 0; i < F; i++)
  {
    delete[] z[i];
  }
  delete[] z;
}

Instancia::Instancia(const char *nome)
{
  std::ifstream file;
  file.open(nome);

  //Verificando se ocorreu algum problema na abertura do arquivo
  if (!file)
  {
    std::cout << "Erro na abertura do arquivo!!" << std::endl;
    exit(0);
  }
  //Lendo quantos pontos de oferta, CD e demanda que a instancia tem
  file >> I >> F >> J;
  std::cout << I << " " << F << " " << J << std::endl;

  //Dando resize nos vector e alocando as matrizes de custos variaveis
  p.resize(I);
  h.resize(F);
  d.resize(J);
  b.resize(F);

  c = new double*[I];
  for (int i = 0; i < I; i++)
  {
    c[i] = new double[F];
  }
  t = new double*[F];
  for (int i = 0; i < F; i++)
  {
    t[i] = new double[J];
  }

  //Lendo a capacidade da fabrica
  for (int i = 0; i < I; i++)
  {
    file >> p[i];
    //std::cout << "Fabrica[" << i+1 << "]: " << p[i] << std::endl;
  }
  //Lendo a capacidade do CD
  for (int i = 0; i < F; i++)
  {
    file >> h[i];
    //std::cout << "Capacidade[" << i+1 << "]: " << h[i] << std::endl;
  }
  //Lendo a demanda
  for (int i = 0; i < J; i++)
  {
    file >> d[i];
    //std::cout << "Demanda[" << i+1 << "]: " << d[i] << std::endl;
  }
  //Lendo o custo fixo de abertura
  for (int i = 0; i < F; i++)
  {
    file >> b[i];
    //std::cout << "Custo Fixo[" << i+1 << "]: " << b[i] << std::endl;
  }
  //Lendo o custo c, de I para F
  for (int i = 0; i < I; i++)
  {
    for (int j = 0; j < F; j++)
    {
      file >> c[i][j];
      //std::cout << "Custo Variavel C[" << i+1 << "][" << j+1 << "]:" << c[i][j] << std::endl;
    }
  }
  //Lendo o custo t, de F para J
  for (int i = 0; i < F; i++)
  {
    for (int j = 0; j < J; j++)
    {
      file >> t[i][j];
      //std::cout << "Custo Variavel T[" << i+1 << "][" << j+1 << "]:" << t[i][j] << std::endl;
    }
  }
  file.close();
}

Instancia::~Instancia()
{
  //Desalocando o vetor de custo variavel c
  for (int i = 0; i < I; i++)
  {
    delete[] c[i];
  }
  delete[] c;

  //Desalocando o vetor de custo variavel t
  for (int i = 0; i < F; i++)
  {
    delete[] t[i];
  }
  delete[] t;
}
