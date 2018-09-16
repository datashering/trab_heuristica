#include <iostream>
#include <fstream>

struct inst {
  int F;
  int J;
  int *d;
  int *b;
  int *f;
  int **c;
};

void aloca_struc(inst &dados){
  dados.d = new int[dados.J];
  dados.b = new int[dados.F];
  dados.f = new int[dados.F];
  dados.c = new int*[dados.F];
  for (int i = 0; i < dados.F; i++){
    dados.c[i] = new int[dados.J];
  }
}

void libera_struc(inst& dados){
  delete[] dados.d;
  delete[] dados.b;
  delete[] dados.f;
  for(int i = 0; i < dados.F; i++){
    delete[] dados.c[i];
  }
  delete[] dados.c;
}

void le_arq(inst& dados, char nome[]){
  std::ifstream file;
  file.open(nome);
  //Verificando se ocorreu algum problema na abertura do arquivo
  if (!file) {
    std::cout << "Erro na abertura do arquivo!!" << std::endl;
    return;
  }
  //Lendo quantos pontos de oferta e demanda a instancia tem
  file >> dados.F >> dados.J;
  std::cout << dados.F << " " << dados.J << std::endl;
  //Aloca a memoria para a struct dados após ler quantas fábricas e pontos de demandas ela tera
  aloca_struc(dados);

  //Lendo a demanda
  for(int i = 0; i < dados.J; i++){
    file >> dados.d[i];
    std::cout << "Demanda[" << i+1 << "]: " << dados.d[i] << std::endl;
  }
  //Lendo a capacidade
  for(int i = 0; i < dados.F; i++){
    file >> dados.b[i];
    std::cout << "Capacidade[" << i+1 << "]: " << dados.b[i] << std::endl;
  }
  //Lendo os custos fixos
  for(int i = 0; i < dados.F; i++){
    file >> dados.f[i];
    std::cout << "Custo Fixo[" << i+1 << "]: " << dados.f[i] << std::endl;
  }
  //Lendo os custos variaveis da oferta I para demanda J
  for(int i = 0; i < dados.F; i++){
    for(int j = 0; j < dados.J; j++){
      file >> dados.c[i][j];
      std::cout << "Custo Variavel[" << i+1 << "][" << j+1 << "]:" << dados.c[i][j] << std::endl;
    }
  }

  file.close();
}

int main(){
  inst dados;
  char nome[50];

  std::cout << "Digite o nome da instancia: " << std::endl;
  std::cin >> nome;

  le_arq(dados, nome);

  libera_struc(dados);
}
