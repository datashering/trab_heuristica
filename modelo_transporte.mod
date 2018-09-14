set F;  # Conjunto de Facilidades
set J;  # Conjunto de Zonas de Demanda

param b{f in F};          # Custo fixo de abertura de facilidade
param q{f in F};          # Capacidade da facilidade
param d{j in J};          # Quantidade de produto demanda por zona
param c{f in F, j in J};  # Custo de transporte da facilidade f para a zona de demanda j

var x{f in F, j in J}, >=0;   # Quantidade transportada f -> j
var y{f in F}, binary;        # 1 se a facilidade f é aberta, 0 se nao

minimize cost: sum{f in F} y[f]*b[f] + sum{f in F, j in J} x[f,j]*c[f,j];

r1{j in J}: sum{f in F} x[f,j] = d[j];         # Toda demanda deve ser atendida
r2{f in F}: sum{j in J} x[f,j] <= y[f]*q[f];   # A Quantidadetrasportada deve ser menor que a capacidade
                                               # da facilidade e zero caso a facilidade não seja aberta
end;
