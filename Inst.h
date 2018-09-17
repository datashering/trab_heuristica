#ifndef INST_H
#define INST_H

struct Inst {
  int F;
  int J;
  int *d;
  int *b;
  int *f;
  int **c;
  Inst(const char *nome);
  ~Inst();
};

#endif
