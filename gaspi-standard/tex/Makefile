STDFILE=GASPI_Standard_Draft

.PHONY: default

default: pdf

.PHONY: ps

ps:
	latex $(STDFILE)
	latex $(STDFILE)
	latex $(STDFILE)
	dvips $(STDFILE)

.PHONY: pdf

pdf:
	pdflatex $(STDFILE)
	if [ $$((`grep "Rerun" $(STDFILE).log | wc -l`)) -gt 0 ]; then make pdf; fi

.PHONY: clean

RM=rm -rf

latex_exts=aux dvi toc log ps pdf

clean:
	$(foreach latex_ext,$(latex_exts),$(RM) $(STDFILE).$(latex_ext))
	-$(RM) *~
