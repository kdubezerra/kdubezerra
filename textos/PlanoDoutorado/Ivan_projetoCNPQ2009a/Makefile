DOCUMENT=planoDoutorado

CAPITULOS=$(wildcard *.tex)

.SUFFIXES: .dia .eps

.dia.eps:
	dia --nosplash --export=$@ $<
.eps.pdf:
	epstopdf $< $@

all: $(DOCUMENT).pdf

view: $(DOCUMENT).pdf
	evince $<

$(DOCUMENT).pdf: $(DOCUMENT).tex $(CAPITULOS) $(DOCUMENT).bib
	pdflatex $(DOCUMENT) && bibtex $(DOCUMENT) && pdflatex $(DOCUMENT) && pdflatex $(DOCUMENT) 

clean:
	rm -f *.aux *.bbl *.blg *.lof *.lot *.log *.ps $(DOCUMENT).pdf *.dvi  *~ *.toc
