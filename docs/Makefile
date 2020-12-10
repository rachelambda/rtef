# for making report.pdf
all: report.pdf
report.pdf: report.md
	pandoc $< -o $@
.PHONY: view
view: report.pdf
	zathura $<

