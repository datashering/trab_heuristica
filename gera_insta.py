import numpy as np

def write_instance(i, f, j, r1, r2, sub_dir=""):
    I = range(i)
    J = range(j)
    F = range(f)

    # Gerando números aleatórios
    p = np.random.uniform(5, 35, i)
    h = np.random.uniform(10,160, f)
    d = np.random.uniform(5, 35, j)
    b = np.random.uniform(0, 90, f) + np.random.uniform(100, 110, f) * np.sqrt(h)

    # Escalando oferta com demanda
    p = p * (d.sum()*r2 / p.sum())

    # Escalando capacidade com demanda
    h = h * (d.sum()*r1 / h.sum())

    I_cord = np.random.uniform(0,1,(i,2))
    F_cord = np.random.uniform(0,1,(f,2))
    J_cord = np.random.uniform(0,1,(j,2))

    dist_euc = lambda x, y: np.sqrt( (x[0] - y[0])**2 + (x[1] - y[1])**2 )

    c = [[dist_euc(i,f)*15 for f in F_cord] for i in I_cord]
    t = [[dist_euc(f,j)*15 for j in J_cord] for f in F_cord]

    # Escrevendo .dat para modelo em C
    file_name = "instancias_c/" + sub_dir + "{:d}-{:d}-{:d}-{:.1f}-{:.1f}".format(len(p), len(h), len(d), r1, r2)
    file = open(file_name, 'w')
    write_c(p, h, d, b, c, t, file)
    file.close()

    # Escrevendo .dat para modelo e math prog
    # file_name = "instancias_mod/" + sub_dir + "{:d}-{:d}-{:d}-{:.1f}.dat".format(len(p), len(h), len(d), r)
    # file =  open(file_name, 'w')
    # write_mod(p, h, d, b, c, t, file)
    # file.close()

def write_c(p, h, d, b, c, t, file):
    file.write("{:d} {:d} {:d}".format(len(p), len(h), len(d)))
    file.write("\n")

    [file.write("{:.3f} ".format(x)) for x in p]
    file.write("\n")

    [file.write("{:.3f} ".format(x)) for x in h]
    file.write("\n")

    [file.write("{:.3f} ".format(x)) for x in d]
    file.write("\n")

    [file.write("{:.3f} ".format(x)) for x in b]
    file.write("\n")

    for line in c:
        [file.write("{:.3f} ".format(x)) for x in line]
        file.write("\n")

    for line in t:
        [file.write("{:.3f} ".format(x)) for x in line]
        file.write("\n")

    file.close()

def write_mod(p, h, d, b, c, t, file):
    I = range(len(p))
    F = range(len(h))
    J = range(len(d))

    file.write("set I := ")
    [file.write("{:d} ".format(i)) for i in I]
    file.write(";\n")

    file.write("set F := ")
    [file.write("{:d} ".format(f)) for f in F]
    file.write(";\n")

    file.write("set J := ")
    [file.write("{:d} ".format(j)) for j in J]
    file.write(";\n")

    file.write("param p := \n")
    [file.write("{:d}  {:.3f}\n".format(i, p[i])) for i in I]
    file.write(";\n")

    file.write("param d := \n")
    [file.write("{:d}  {:.3f}\n".format(j, d[j])) for j in J]
    file.write(";\n")

    file.write("param: b h := \n")
    [file.write("{:d}  {:.3f}  {:.3f}\n".format(f, b[f], h[f])) for f in F]
    file.write(";\n")

    file.write("param c : ")
    [file.write("{:d}".format(f).ljust(8)) for f in F]
    file.write(":=\n")
    for i in I:
        file.write("{:d}".format(i).ljust(3))
        [file.write(" {:.3f}".format(c[i][f]).ljust(8)) for f in F]
        file.write("\n")
    file.write(";\n")

    file.write("param t : ")
    [file.write("{:d}".format(j).ljust(8)) for j in J]
    file.write(":=\n")
    for f in F:
        file.write("{:d}".format(f).ljust(3))
        [file.write(" {:.3f}".format(t[f][j]).ljust(8)) for j in J]
        file.write("\n")
    file.write(";\nend;")

    file.close()

if __name__ == "__main__":

    V = range(20, 401, 20)
    R = [1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0]

    #Batch0
    [write_instance(3, 4, 5, 1.5, 1.5, "batch0/" + str(v) + "-") for v in range(5)]
    [write_instance(3, 5, 10, 1.5, 1.5, "batch0/" + str(v) + "-") for v in range(5)]
    [write_instance(10, 50, 100, 3.0, 1.5, "batch0/" + str(v) + "-") for v in range(5)]
    [write_instance(25, 50, 150, 3.0, 1.5, "batch0/" + str(v) + "-") for v in range(5)]
    [write_instance(50, 100, 500, 3.0, 1.5, "batch0/" + str(v) + "-") for v in range(1)]
    [write_instance(50, 200, 500, 5.0, 1.5, "batch0/" + str(v) + "-") for v in range(1)]


    # # Batch1
    # for i in V:
    #     [write_instance(i, 20, 20, 1.5, 1.5, "batch1/" + str(v) + "-") for v in range(5)]
    #
    # # Batch2
    # for f in V:
    #     [write_instance(20, f, 20, 1.5, 1.5, "batch2/" + str(v) + "-") for v in range(5)]
    #
    # # Batch3
    # for j in V:
    #     [write_instance(20, 20, j, 1.5, 1.5, "batch3/" + str(v) + "-") for v in range(5)]
    #
    # # Batch4
    # for x in V:
    #     [write_instance(x, x, x, 1.5, 1.5, "batch4/" + str(v) + "-") for v in range(5)]
    #
    # # Batch5
    # for r in R:
    #     [write_instance(20, 40, 100, r, 1.5, "batch5/" + str(v) + "-") for v in range(3)]
    #
    # # Batch6
    # for r in R:
    #     [write_instance(20, 40, 100, 1.5, r, "batch6/" + str(v) + "-") for v in range(3)]
    #
    # # Batch7
    # for r in R:
    #     [write_instance(20, 40, 100, r, r, "batch7/" + str(v) + "-") for v in range(3)]
