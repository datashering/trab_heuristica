set I;  # Conjunto de Fabricas
set F;  # Conjunto de Centros de districuição (CD)
set J;  # Conjunto de Zonas de Demanda

param p{i in I};    # Capacidade de produção da fabrica
param h{f in F};    # Capacidade do CD
param d{j in J};    # Quantidade de produto demandado

param b{f in F};          # Custo fixo de abertura do CD
param c{i in I, f in F};  # Custo de transporte unitário de uma fabrica para um CD
param t{f in F, j in J};  # Custo de transporte unitário de um CD para uma zona de demanda

var x{i in I, f in F}, >=0;   # Quantidate transportada i -> f
var z{f in F, j in J}, >=0;   # Quantidade transportada f -> j
var y{f in F}, binary;        # 1 se a facilidade f é aberta, 0 se nao

minimize cost: sum{f in F} y[f]*b[f] + sum{i in I, f in F} x[i,f]*c[i,f] + sum{f in F, j in J} z[f,j]*t[f,j];

r1{j in J}: sum{f in F} z[f,j] = d[j];              # Toda demanda deve ser atendida
r2{f in F}: sum{j in J} z[f,j] <= y[f]*h[f];        # Capacidade do CD + critério de abertura
r3{f in F}: sum{i in I} x[i,f] = sum{j in J} z[f,j];  # Conservação de fluxo no CD
r4{i in I}: sum{f in F} x[i,f] <= p[i];              # Capacidade de produção da fábrica
end;
