#include <stdio.h>
#include <stdlib.h>

struct interface_animal {
  struct {
    const char* (*saySomething)(void);
  }* vtable;
};

typedef struct cat {
  struct _cat_vtable {
    const char* (*meow)(void);
  }* vtable;
  char* name;
} Cat;

typedef struct dog {
  struct _dog_vtable {
    const char* (*bark)(void);
  }* vtable;
  char* name;
} Dog;

const char*
cat_meow(void) { return "Meow!"; }

const char*
dog_bark(void) { return "Bark!"; }

void
make_it_say(struct interface_animal* animal)
{
  puts(animal->vtable.saySomething());
}

struct _cat_vtable _cat_methods = {cat_meow};
struct _dog_vtable _dog_methods = {dog_bark};


int
main(void)
{
  Cat* cat = malloc(sizeof(Cat));
  cat->vtable = &_cat_methods;

  Dog* dog = malloc(sizeof(Dog));
  dog->vtable = &_dog_methods;

  make_it_say(cat); /* Meow! */
  make_it_say(dog); /* Bark! */

  return 0;
}
