SHELL         = /bin/bash
SPHINXOPTS   ?= -j auto
SPHINXBUILD  ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = ../build

help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
