import numpy as np

def write_instance(i, f, j, r):
    I = range(i)
    J = range(j)
    F = range(f)

    # Gerando números aleatórios
    p = np.random.uniform(5, 35, i)
    h = np.random.uniform(10,160, f)
    d = np.random.uniform(5, 35, j)
    b = np.random.uniform(0, 90, f) + np.random.uniform(100, 110, f) * np.sqrt(h)

    # Escalando oferta com demanda
    p = p * (d.sum()*r / p.sum())

    # Escalando capacidade com demanda
    h = h * (d.sum()*r / h.sum())

    I_cord = np.random.uniform(0,1,(i,2))
    F_cord = np.random.uniform(0,1,(f,2))
    J_cord = np.random.uniform(0,1,(j,2))

    dist_euc = lambda x, y: np.sqrt( (x[0] - y[0])**2 + (x[1] - y[1])**2 )

    c = [[dist_euc(i,f)*10 for f in F_cord] for i in I_cord]
    t = [[dist_euc(f,j)*10 for j in J_cord] for f in F_cord]

    print(p.sum())
    print(h.sum())
    print(d.sum())

    write_c(p, h, d, b, c, t, r)
    write_mod(p, h, d, b, c, t, r)

def write_c(p, h, d, b, c, t, r):
    file = open("instancias_c/{:d}-{:d}-{:d}-{:.1f}.dat".format(len(p), len(h), len(d), r), 'w')

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

def write_mod(p, h, d, b, c, t, r):

    file =  open("instancias_mod/{:d}-{:d}-{:d}-{:.1f}.dat".format(len(p), len(h), len(d), r), 'w')

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

    write_instance(5, 10, 30, 1.5)
    write_instance(5, 10, 60, 1.5)
    write_instance(5, 10, 90, 1.5)
    write_instance(5, 10, 120, 1.5)
    write_instance(5, 10, 150, 1.5)
    write_instance(5, 10, 180, 1.5)
    write_instance(5, 10, 210, 1.5)
    write_instance(5, 10, 240, 1.5)

    write_instance(5, 10, 30, 1.5)
    write_instance(5, 40, 30, 1.5)
    write_instance(5, 70, 30, 1.5)
    write_instance(5, 100, 30, 1.5)
    write_instance(5, 130, 30, 1.5)
    write_instance(5, 160, 30, 1.5)
    write_instance(5, 190, 30, 1.5)
    write_instance(5, 220, 30, 1.5)

    write_instance(5, 10, 30, 1.5)
    write_instance(35, 10, 30, 1.5)
    write_instance(65, 10, 30, 1.5)
    write_instance(95, 10, 30, 1.5)
    write_instance(125, 10, 30, 1.5)
    write_instance(155, 10, 30, 1.5)
    write_instance(185, 10, 30, 1.5)
    write_instance(215, 10, 30, 1.5)

    write_instance(5, 10, 25, 1.5)
    write_instance(15, 20, 35, 1.5)
    write_instance(25, 30, 45, 1.5)
    write_instance(35, 40, 55, 1.5)
    write_instance(45, 50, 65, 1.5)
    write_instance(55, 60, 75, 1.5)
    write_instance(65, 70, 85, 1.5)
    write_instance(75, 80, 95, 1.5)
    write_instance(85, 90, 105, 1.5)
    write_instance(95, 100, 115, 1.5)
    write_instance(105, 110, 125, 1.5)
    write_instance(115, 120, 135, 1.5)
    write_instance(125, 130, 145, 1.5)
    write_instance(135, 140, 155, 1.5)
    write_instance(145, 150, 165, 1.5)
    write_instance(155, 160, 175, 1.5)
    write_instance(165, 170, 185, 1.5)
    write_instance(175, 180, 195, 1.5)
