#include <iostream>
#include "Drop.h"

Drop_Sol::Drop_Sol(int tamanho)
{
  facilidades.resize(tamanho);
}

/*
  A funcao Resolve_PDT(vector<bool>, int) resolvera o problema do transporte e retornara o valor da FO
  para aquele vetor de facilidades abertas
*/

/*Funcao que servira para balancear a instancia para a solucao do PDT, nao sei se e
necessario no caso da DROP, se auxilia ou se nao muda para o solver, de qlq maneira
servira para criar uma facilidade ficticia com o custo de envio mais barato que os da abertas

void Balancea_Instancia(Instancia& dados, std::vector<bool>& abertas)
{

}
*/

//Funcao que retorna qual vetor de facilidades abertas gerara a melhor FO a partir do criterio DROP
Drop_Sol Drop(Instancia& dados)
{
  //Sao criadas as structs para armazenar a melhor solucao de uma iteracao assim como a melhor atual
  Drop_Sol Melhor_Sol = Drop_Sol(dados.F);
  Drop_Sol Sol_atual = Drop_Sol(dados.F);
  //E criado um vetor para avaliar a melhoria na solucao para cada facilidade fechada
  std::vector<bool> abertas (dados.F, 1);
  double resultado;
  //E carregado o problema do transporte no GLPK, a partir do qual sera realizada variacoes nas facilidades abertas
  LPSolver(dados);

  Melhor_Sol.FO = Resolve_PDT(abertas, dados);

  do
  {
    //Para a solucao atual e verificado o efeito de se fechar todas as facilidades separadamente,
    //avaliando qual delas resulta o maior ganho
    Sol_atual = Melhor_Sol;
    abertas = Sol_atual.facilidades;
    for (int i = 0; i < dados.F; i++)
    {
      //E avaliada a nova solucao apenas se a facilidade estiver aberta, caso contrario nao ha necessidade calcular
      if (abertas[i] == 1)
      {
        //Fecha-se a facilidade i
        abertas[i] = 0;

        //E chamada a funcao para resolver o problema do transporte para o vetor atual que retorna a FO
        resultado = Resolve_PDT(abertas, dados);
        if (resultado < Melhor_Sol.FO)
        {
          Melhor_Sol.FO = resultado;
          Melhor_Sol.facilidades = abertas;
        }
        //Volta-se a abrir a facilidade i para avaliar o impacto da proxima
        abertas[i] = 1;
      }
    }
  }
  //Enquanto fechar facilidade gerar reducao de custos continua-se o procedimento
  while (Melhor_Sol < Sol_atual);
}
