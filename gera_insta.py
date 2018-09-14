import numpy as np

def write_instance(i, j, f, p):
    I = range(i)
    J = range(j)
    F = range(f)
    P = range(p)
    file = open("instancias/{:d}-{:d}-{:d}-{:d}.txt".format(i,j,f,p), 'w')

    demand = np.array([[int(abs(np.random.normal(100,30))) for p in P] for j in J])
    weight = np.array([abs(np.random.normal(1,0.3)) for p in P])
    f_cost = [abs(np.random.normal(100,30)) for f in F]
    capacity = np.array([abs(np.random.normal(10,3)) for f in F])
    capacity = capacity * ((weight*demand).sum()*1.5/capacity.sum())

    [file.write("{:.1f} ".format(capacity[f])) for f in F]
    file.write("\n")
    [file.write("{:.1f} ".format(weight[p])) for p in P]
    file.write("\n")
    [file.write("{:.1f} ".format(f_cost[f])) for f in F]
    file.write("\n")
    for j in J:
        [file.write("{:d} ".format(demand[j][p])) for p in P]
        file.write("\n")

    for idx in range(len(I)*len(J)):
        [file.write("{:.1f} ".format(abs(np.random.normal(1,0.3)))) for f in F]
        file.write("\n")

    file.close()


if __name__ == "__main__":

    write_instance(i = 3, j = 7, f = 5, p = 5)
