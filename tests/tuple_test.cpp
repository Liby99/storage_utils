#include <assert.h>
#include "../include/Prelude.h"

int main() {
  Tuple<int, float, double> t(1, 2.0, 3.0);
  assert(t.get<0>() == 1);
}