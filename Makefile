# extract-artwork - Extract artwork from media files

CC	:= gcc
MD	:= markdown
INSTALL	:= install
CP	:= cp
RM	:= rm
CFLAGS	+= -std=c11 -O2 -Wall -Werror
CFLAGS	+= $(shell pkg-config --cflags --libs libavformat libavutil)
# this is just a fallback in case you do not use git but downloaded
# a release tarball...
VERSION := 0.0.1

all: extract-artwork README.html

extract-artwork: extract-artwork.c
	$(CC) $(CFLAGS) -o extract-artwork extract-artwork.c

README.html: README.md
	$(MD) README.md > README.html

install: install-bin install-doc

install-bin: extract-artwork
	$(INSTALL) -D -m0755 extract-artwork $(DESTDIR)/usr/bin/extract-artwork

install-doc: README.html
	$(INSTALL) -D -m0644 README.md $(DESTDIR)/usr/share/doc/extract-artwork/README.md
	$(INSTALL) -D -m0644 README.html $(DESTDIR)/usr/share/doc/extract-artwork/README.html

clean:
	$(RM) -f *.o *~ README.html extract-artwork

release:
	git archive --format=tar.xz --prefix=extract-artwork-$(VERSION)/ $(VERSION) > extract-artwork-$(VERSION).tar.xz
	gpg -ab extract-artwork-$(VERSION).tar.xz
