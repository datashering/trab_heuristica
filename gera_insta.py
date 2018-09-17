import numpy as np

def write_instance(f, j):
    J = range(j)
    F = range(f)

    demand = np.array([int(np.random.randint(50,100)) for j in J])
    capacity = np.array([np.random.randint(1,11) for f in F])
    capacity = capacity * (int(demand.sum()*2/capacity.sum()))
    f_cost = [np.random.randint(10000,50000) for f in F]
    v_cost = [[np.random.randint(1,5) for j in J] for f in F]

    write_c(demand, capacity, f_cost, v_cost)
    write_mod(demand, capacity, f_cost, v_cost)


def write_c(demand, capacity, f_cost, v_cost):
    file = open("instancias_c/{:d}-{:d}.dat".format(len(capacity), len(demand)), 'w')

    file.write("{:d} {:d} ".format(len(capacity), len(demand)))
    file.write("\n")

    [file.write("{:d} ".format(d)) for d in demand]
    file.write("\n")

    [file.write("{:d} ".format(b)) for b in capacity]
    file.write("\n")

    [file.write("{:d} ".format(f)) for f in f_cost]
    file.write("\n")

    for line in v_cost:
        [file.write("{:d} ".format(c)) for c in line]
        file.write("\n")

    file.close()

def write_mod(demand, capacity, f_cost, v_cost):
    F = range(len(capacity))
    J = range(len(demand))

    file = open("instancias_mod/{:d}-{:d}.dat".format(len(F), len(J)), 'w')

    file.write("set F := ")
    [file.write("{:d} ".format(f)) for f in F]
    file.write(";\n")

    file.write("set J := ")
    [file.write("{:d} ".format(j)) for j in J]
    file.write(";\n")

    file.write("param: b q := \n")
    [file.write("{:d}  {:d}  {:d}\n".format(f, f_cost[f], capacity[f])) for f in F]
    file.write(";\n")

    file.write("param d := \n")
    [file.write("{:d}  {:d}\n".format(j, demand[j])) for j in J]
    file.write(";\n")

    file.write("param c : ")
    [file.write("{:d}".format(j).ljust(5)) for j in J]
    file.write(":=\n")
    for f in F:
        file.write("{:d}".format(f).ljust(3))
        [file.write(" {:d}".format(v_cost[f][j]).ljust(5)) for j in J]
        file.write("\n")
    file.write(";\nend;")

    file.close()


if __name__ == "__main__":

    write_instance(20, 10);
    write_instance(30, 10);
    write_instance(20, 20);
    write_instance(30, 20);
    write_instance(100, 50);
    write_instance(200,200);
    write_instance(400,200);
    write_instance(500,200);
    write_instance(600,150);
    write_instance(800,400);
