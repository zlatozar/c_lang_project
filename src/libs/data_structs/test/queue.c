#include "data_structs/queue.h"

#include <greatest.h>
#include "test_data.h"

TEST free_empty(void)
{
  Queue_T queue = Queue_new();
  ASSERT(Queue_is_empty(queue));

  Data_T data = Data_new(42);
  Queue_add(queue, (Generic_T)data);

  ASSERT_FALSE(Queue_is_empty(queue));
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(free_empty);
  GREATEST_MAIN_END();
}
