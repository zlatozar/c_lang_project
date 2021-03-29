#include "data_structs/queue.h"

#include <greatest.h>
#include "test_data.h"

TEST new_free(void)
{
  Queue_T queue = Queue_new();
  ASSERT(Queue_is_empty(queue));

  Data_T data = Data_new(42);
  Queue_add(queue, (Generic_T)data);

  ASSERT_FALSE(Queue_is_empty(queue));

  Queue_free(queue);
  PASS();
}

TEST length(void)
{
  Queue_T queue = Queue_new();

  Queue_add(queue, get_next_elm(1));
  Queue_add(queue, get_next_elm(2));
  Queue_add(queue, get_next_elm(3));

  ASSERT_FALSE(Queue_is_empty(queue));
  ASSERT(3 == Queue_length(queue));

  Queue_free(queue);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(new_free);
  RUN_TEST(length);
  GREATEST_MAIN_END();
}
