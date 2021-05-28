#! /bin/bash

OS=$(uname -o)
BOCHS_CONFIG=
case "$OS" in
	"Cygwin")
		BOCHS_CONFIG=cygwinbochs.bxs
		;;
	"GNU/Linux")
		BOCHS_CONFIG=gnubochs.bxs
		;;
	*)
		echo "Could not determine environment ($OS unknown)."
		exit 1
		;;
esac

DIR=$(dirname $0)
cd "$DIR"
cd ../../
PROJECT_DIR=$(pwd)
cd "$DIR"
echo "Bochs configuration for $OS environment is $PROJECT_DIR/tools/bochs/$BOCHS_CONFIG."
echo -e "The following lines are the uncommented lines in the bochs configuration file.\n"

grep -v -n "#" "$BOCHS_CONFIG" | grep -E -v '^[[:digit:]]+:[[:space:]]*$'
