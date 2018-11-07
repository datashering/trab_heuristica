#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <vector>
#include <numeric>
#include <array>
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
  for (int f = 0; f < F; f++)
  {
    z[f] = new double[J];
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
  for (int f = 0; f < F; f++)
  {
    delete[] z[f];
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
  //std::cout << I << " " << F << " " << J << std::endl;

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
  d_total = 0;
  for (int i = 0; i < J; i++)
  {
    file >> d[i];
    d_total += d[i];
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

Candidatos::Candidatos(int tam): tamanho(tam)
{
  viaveis.resize(tam);
  abertos.reserve(tam);
  capacidade_total = 0;
}

// Struct para resolver problema com programação dinamica
ProgDinamica::ProgDinamica(Instancia &dados) {
  k = dados.F;
  demanda = std::accumulate(dados.d.begin(), dados.d.end(), 0.0);

  capacidades = dados.h;

  sol.resize(k);
  for (int i=0; i<k; i++) {
    sol[i] = true;
  }
  custos.resize(k);

  opt = new double*[k];
  opt_sol = new bool*[k];
  for (int i=0; i<k; i++) {
    opt[i] = new double[demanda + 1];
    opt_sol[i] = new bool[demanda + 1];
  }
}

ProgDinamica::~ProgDinamica() {
  for (int i=0; i<k; i++) {
    delete[] opt[i];
    delete[] opt_sol[i];
  }
  delete[] opt;
  delete[] opt_sol;
}

bool ProgDinamica::resolve() {
  // Caso base
  opt[0][0] = 0;      // Se a demanda = 0 -> opt = 0
  opt_sol[0][0] = 0;  // Nao abre CD
  for (int d=1; d<=demanda; d++) {
    // Se a demanda > capacidade problema inviavel
    if (d > capacidades[0]) {
      opt[0][d] = MAX;
    }
    // Se a demanda < capacidade abre CD
    else {
      opt[0][d] = custos[0];
      opt_sol[0][d] = 1;
    }
  }

  // Caso geral
  int resto;
  for (int i=1; i<k; i++) {
    opt[i][0] = 0;      // Se a demanda = 0, opt = 0
    opt_sol[i][0] = 0;  // Nao abre CD

    for (int j=1; j<=demanda; j++) {
      resto = std::max(0, j - capacidades[i]);

      if (opt[i-1][j] > opt[i-1][resto] + custos[i]) {
        // Abre CD
        opt[i][j] = opt[i-1][resto] + custos[i];
        opt_sol[i][j] = 1;
      }

      else {
        // Nao abre CD
        opt[i][j] = opt[i-1][j];
        opt_sol[i][j] = 0;
      }
    }
  }

  // Reconstruindo solucao
  bool flag = false;

  resto = demanda;
  for (int i=k-1; i>=0; i--) {

    if (sol[i] != opt_sol[i][resto]) {
      sol[i] = opt_sol[i][resto];
      flag = true;
    }

    if (sol[i]) {
      resto = std::max(0, resto - capacidades[i]);
    }
  }
  return flag;
}

// Funcoes diversas

double abre_aprox(Instancia &dados, const Solucao &sol, int cd) {
  typedef std::array<double, 4> Tuple;
  std::vector<Tuple> ganho;

  // Calculando o ganho de redirecionar o arco (i,f,j) -> (i,cd,j)
  ganho.reserve(dados.I * dados.F * dados.J);
  for (int f=0; f<dados.F; f++) {
    if (sol.y[f] == 0) {
      continue;
    }

    for (int j=0; j<dados.J; j++) {
      if (sol.z[f][j] == 0) {
        continue;
      }

      for (int i=0; i<dados.I; i++) {
        if (sol.x[i][f] == 0) {
          continue;
        }

        Tuple g;
        g[0] = i;
        g[1] = f;
        g[2] = j;
        g[3] = dados.c[i][f] - dados.c[i][cd] + dados.t[f][j] - dados.t[cd][j];
        ganho.push_back(g);
      }
    }
  }

  // Ordenação (decrescente) do vetor de ganho
  auto comparador = [](Tuple a, Tuple b) { return a[3] > b[3]; };
  std::sort(ganho.begin(), ganho.end(), comparador);

  double redirecionado = 0;
  double fo = sol.func_obj;

  for (Tuple t : ganho) {
    // Caso não tenho mais ganhos interromper
    if (t[3] <=0) {
      break;
    }

    int i = t[0];
    int f = t[1];
    int j = t[2];

    // Definindo a quantidade a ser redirecioada
    double qtd = std::min(sol.x[i][f], sol.z[f][j]);
    qtd = std::min(qtd, dados.h[cd] - redirecionado);
    redirecionado += qtd;

    // Atualização da solução
    fo -= qtd * t[3];
    //sol.x[i][f] -= qtd;
    //sol.z[f][j] -= qtd;
    //sol.x[i][cd] += qtd;
    //sol.z[cd][j] += qtd;

    // Acabou a capacidade interrompe
    if (redirecionado == dados.h[cd]) {
      break;
    }
  }

  return fo + dados.b[cd];

}

double fecha_aprox(Instancia &dados, const Solucao &sol, int cd) {
  typedef std::array<double, 3> Tuple;

  // Calculo a capacidade restante de cada CD aberto
  std::vector<int> cap_cd(dados.F);
  std::vector<int> cds_abertos;
  cds_abertos.reserve(dados.F);

  for (int f=0; f<dados.F; f++) {
    if (sol.y[f] == 0 || f == cd) {
      cap_cd[f] = 0;
      continue;
    }

    int sum = 0;
    for (int i=0; i<dados.I; i++) {
      sum += sol.x[i][f];
    }

    cds_abertos.push_back(f);
    cap_cd[f] = dados.h[f] - sum;
  }

  // Calculo da capacidade restante de cada oferta
  std::vector<int> cap_prod(dados.I);
  for (int i=0; i<dados.I; i++) {
    int sum = 0;

    for (int f : cds_abertos) {
      sum += sol.x[i][f];
    }

    cap_prod[i] = dados.p[i] - sum;
  }

  // Criando vetores de redirecionamento
  std::vector<int> rx(dados.I, 0);
  std::vector<int> rz(dados.J, 0);

  // Atender cada cliente de forma gulosa
  double fo = sol.func_obj - dados.b[cd];

  for (int j=0; j<dados.J; j++) {
    if (sol.z[cd][j] - rz[j] == 0) {
      continue;
    }

    std::vector<Tuple> custos;
    custos.reserve(dados.F * dados.I);

    // Calculando o custo de redirecionar o arco (i,cd,j) -> (i,f,j)
    for (int i=0; i<dados.I; i++) {
      if (cap_prod[i] == 0 || sol.x[i][cd] - rx[i] == 0) {
        continue;
      }

      for (int f : cds_abertos) {
        if (cap_cd[f] == 0) {
          continue;
        }

        Tuple t;
        t[0] = i;
        t[1] = f;
        t[2] = dados.c[i][f] - dados.c[i][cd] + dados.t[f][j] - dados.t[cd][j];
        custos.push_back(t);
      }
    }

    // Ordenação (crescente) do vetor de custos
    auto comparador = [](Tuple a, Tuple b) { return a[2] < b[2]; };
    std::sort(custos.begin(), custos.end(), comparador);

    for (Tuple t : custos) {
      int i =  t[0];
      int f = t[1];

      int resto = sol.z[cd][j] - rz[j];

      int qtd = std::min(cap_cd[f], cap_prod[i]);
      qtd = std::min(qtd, resto);

      cap_cd[f] -= qtd;
      cap_prod[i] -= qtd;

      fo += qtd * t[2];
      rx[i] += qtd;
      rz[j] += qtd;
      // sol.x[i][cd] -= qtd;
      // sol.z[cd][j] -= qtd;
      // sol.x[i][f] += qtd;
      // sol.z[f][j] += qtd;

      if (sol.z[cd][j] - rz[j] == 0) {
        break;
      }

    }
  }

  return fo;
}

double avalia_t1b(Instancia &dados, Solucao &sol, int cd) {
  if (sol.y[cd] == 0) {
    return abre_aprox(dados, sol, cd);
  }

  else if (dados.d_total < sol.cap_total - dados.b[cd]) {
    return fecha_aprox(dados, sol, cd);
  }

  else {
    return -1;
  }
}
