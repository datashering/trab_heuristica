#include <fstream>
#include <iostream>
#include "Inst.h"

Inst::Inst(const char *nome) {
  std::ifstream file;
  file.open(nome);

  //Verificando se ocorreu algum problema na abertura do arquivo
  if (!file) {
    std::cout << "Erro na abertura do arquivo!!" << std::endl;
    return;
  }
  //Lendo quantos pontos de oferta e demanda a instancia tem
  file >> F >> J;
  std::cout << F << " " << J << std::endl;

  //Aloca a memoria para a struct dados após ler quantas fábricas e pontos de demandas ela tera
  d = new int[J];
  b = new int[F];
  f = new int[F];
  c = new int*[F];
  for (int i = 0; i < F; i++){
    c[i] = new int[J];
  }

  //Lendo a demanda
  for(int i = 0; i < J; i++){
    file >> d[i];
    //std::cout << "Demanda[" << i+1 << "]: " << d[i] << std::endl;
  }
  //Lendo a capacidade
  for(int i = 0; i < F; i++){
    file >> b[i];
    //std::cout << "Capacidade[" << i+1 << "]: " << b[i] << std::endl;
  }
  //Lendo os custos fixos
  for(int i = 0; i < F; i++){
    file >> f[i];
    //std::cout << "Custo Fixo[" << i+1 << "]: " << f[i] << std::endl;
  }
  //Lendo os custos variaveis da oferta I para demanda J
  for(int i = 0; i < F; i++){
    for(int j = 0; j < J; j++){
      file >> c[i][j];
      //std::cout << "Custo Variavel[" << i+1 << "][" << j+1 << "]:" << c[i][j] << std::endl;
    }
  }

  file.close();
}

Inst::~Inst() {
  delete[] d;
  delete[] b;
  delete[] f;
  for(int i = 0; i < F; i++){
    delete[] c[i];
  }
  delete[] c;
}
