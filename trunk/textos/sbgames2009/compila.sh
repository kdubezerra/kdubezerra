#!/bin/bash

pdflatex IEEE_kdtree
bibtex IEEE_kdtree
pdflatex IEEE_kdtree
pdflatex IEEE_kdtree
pdffonts IEEE_kdtree.pdf