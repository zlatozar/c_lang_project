# gdb.mk - Provides the "%.debug" target(s)

if HAVE_GDB

GDB = $(GDB_ENVIRONMENT) $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
	--mode=execute $(GDB_PATH) $(GDB_FLAGS)

%.debug: %
	$(AM_V_at)$(GDB) $*

endif
