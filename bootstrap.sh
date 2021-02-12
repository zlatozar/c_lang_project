#!/bin/sh

# TODO: Prod and Dev cases
autoreconf -iv && ./configure --enable-debug
