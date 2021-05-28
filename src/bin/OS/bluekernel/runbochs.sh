#! /bin/bash

OS=$(uname -o)
BOCHS_CONFIG=
BOCHS=bochs
case "$OS" in
	"Cygwin")
		BOCHS_CONFIG=./tools/bochs/cygwinbochs.bxs
		BOCHS=bochsdbg
		;;
	"GNU/Linux")
		BOCHS_CONFIG=./tools/bochs/gnubochs.bxs
		;;
	*)
		echo "Could not determine environment ($OS unknown)."
		exit 1
		;;
esac

echo "Running Bochs in $OS environment, using $BOCHS_CONFIG."
"$BOCHS" -q -f "$BOCHS_CONFIG"
