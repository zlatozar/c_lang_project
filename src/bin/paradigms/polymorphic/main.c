#include "polymorphic/interface.h"

#include "polymorphic/first_impl.h"
#include "polymorphic/second_impl.h"

static void
calculate(Interface_T interface)
{
  void* impl = interface->Interface_new();

  interface->Interface_add(impl, 1, 2);
  interface->Interface_destroy(impl);
}

int
main(void)
{
  /* Use fist interface implementation. */
  calculate(&impl_one);

  /* Use second interface implementation. */
  calculate(&impl_two);
}
