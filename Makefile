CC=cc
DOCC=doxygen
CFLAGS=-g -Wall 

REFDIR=.
SRCDIR=$(REFDIR)/src
BINDIR=$(REFDIR)/bin
DOCDIR=$(REFDIR)/doc

CSOURCE=$(wildcard $(SRCDIR)/compileBST.c)


all: binary


$(BINDIR)/compileBST: $(CSOURCE)
	$(CC) $(CFLAGS)  $^ -o $@

$(DOCDIR)/index.html: $(SRCDIR)/Doxyfile $(CSOURCE) 
	$(DOCC) $(SRCDIR)/Doxyfile

binary: $(BINDIR)/compileBST

doc: $(DOCDIR)/index.html

clean:
	rm -rf $(BINDIR)/* 


.PHONY: all doc binary
