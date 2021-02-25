#include "algorithms/array.h"
#include "lang/assert.h"

void
minmax(int numlist[], int n, int* p_min_, int* p_max_)
{
  Require(n > 0);

  int min2, max2;
  if (n == 1) {
    *p_min_ = *p_max_ = numlist [0];

  } else if (n == 2) {
    if (numlist[0] < numlist[1]) {
      *p_min_ = numlist[0];
      *p_max_ = numlist[1];
    } else {
      *p_min_ = numlist[1];
      *p_max_ = numlist[0];
    }

  } else {
    minmax(numlist, n / 2, p_min_, p_max_);
    minmax(numlist + n / 2,  n - (n / 2), &min2, &max2);

    if (min2 < *p_min_)
    { *p_min_ = min2; }

    if (max2 > *p_max_)
    { *p_max_ = max2; }
  }
}
