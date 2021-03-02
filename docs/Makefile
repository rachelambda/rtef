# for making report.pdf
.PHONY: view clean all
GARBAGE=_minted-report/ report.aux report.aux.bbl report.aux.blg report.bbl report.bcf report.blg report.dvi report.log report.run.xml report.toc
all: report.pdf
report.pdf: report.tex bibliography.bib
	pdflatex -shell-escape report.tex -o $@
	biber report
	pdflatex -shell-escape report -o $@
	pdflatex -shell-escape report -o $@
	rm -rf $(GARBAGE)

clean:
	rm -rf $(GARBAGE) report.pdf
view: report.pdf
	zathura report.pdf
