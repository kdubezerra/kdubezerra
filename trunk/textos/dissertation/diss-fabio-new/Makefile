### Gerado por NANAPES 0.6.1 -- Não Achei um Nome Adequado Para Esse Script ###
all: diss.ps

FIGURAS=figuras/*.eps
TEXFILES=*.tex

diss.bbl: diss.bib
# Descomente tres das linhas abaixo para gerar a bibliografia
# (duas do bibtex e a do latex ou do pdflatex).
	latex diss.tex
#	pdflatex diss.tex
	bibtex diss
	bibtex diss

diss.ps: diss.dvi
	dvips diss.dvi -o diss.ps

diss.dvi: diss.tex diss.bbl diss.bib $(TEXFILES) $(FIGURAS)
	latex diss.tex
	latex diss.tex
	latex diss.tex

diss.pdf: diss.ps
	ps2pdf -sPAPERSIZE=a4 diss.ps diss.pdf 

pdflatex: diss.tex diss.bbl diss.bib
	pdflatex diss.tex
	pdflatex diss.tex
	pdflatex diss.tex

clean:
	rm -f *.aux
	rm -f *.log
	rm -f *.idx
	rm -f *.ps
	rm -f *.dvi
	rm -f *.toc
	rm -f *.bbl
	rm -f *.blg
	rm -f *~

.PHONY: all clean pdflatex

