#ifndef COMMON_LANG_UNUSED_H
#define COMMON_LANG_UNUSED_H

/// Placed at the beginning of a function it suppress compiler
/// warnings about unused parameters.
#define UNUSED(param_name)                                    \
  ((void)(0 ? ((param_name) = (param_name)) : (param_name)))

#endif
