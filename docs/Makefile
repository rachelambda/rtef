# for making report.pdf
all: report.pdf
report.pdf: report.tex
	pdflatex report.tex -o $@
.PHONY: view
view: report.pdf
	zathura $<

