#pragma once

#include <iostream>
#include <vector>

struct Drop_Sol
{
  double FO;
  std::vector<bool> facilidades;

  Drop_Sol(int);
}

Drop_Sol Drop(Instancia&);

void Balancea_Instancia(Instancia&, std::vector<bool>&);
