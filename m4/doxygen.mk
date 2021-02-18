# doxygen.mk - Provides the "html" target

if HAVE_DOXYGEN

DOXYGEN_DOXYFILE ?= Doxyfile

AX_V_doxygen = $(ax__v_doxygen_$(V))
ax__v_doxygen_ = $(ax__v_doxygen_$(AM_DEFAULT_VERBOSITY))
ax__v_doxygen_0 = @echo "  DOX   " $<;
ax__v_doxygen_1 =

DOXYGEN = $(DOXYGEN_PATH) $(DOXYGEN_DOXYFILE)

html-local: $(DOXYGEN_DOXYFILE)
	$(AX_V_doxygen)$(DOXYGEN)

endif
