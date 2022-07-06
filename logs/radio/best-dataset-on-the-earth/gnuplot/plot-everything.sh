#!/usr/bin/env bash

find . -name "*.plot" -exec bash -c 'cat {} | gnuplot > {}.png ' \;
