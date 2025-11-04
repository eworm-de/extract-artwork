# extract-artwork - Extract artwork from media files

# commands
CC	:= gcc
MD	:= markdown
INSTALL	:= install
CP	:= cp
RM	:= rm

# flags
CFLAGS	+= -std=c11 -O2 -fPIC -Wall -Werror
CFLAGS	+= $(shell pkg-config --cflags --libs libavformat libavutil)
LDFLAGS	+= -Wl,-z,now -Wl,-z,relro -pie

# this is just a fallback in case you do not use git but downloaded
# a release tarball...
DISTVER := 0.0.5
VERSION ?= $(shell git describe --long 2>/dev/null || echo $(DISTVER))

all: extract-artwork README.html

extract-artwork: extract-artwork.c
	$(CC) extract-artwork.c $(CFLAGS) $(LDFLAGS) -o extract-artwork

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
	git archive --format=tar.xz --prefix=extract-artwork-$(DISTVER)/ $(DISTVER) > extract-artwork-$(DISTVER).tar.xz
	gpg --armor --detach-sign --comment extract-artwork-$(DISTVER).tar.xz extract-artwork-$(DISTVER).tar.xz
	git notes --ref=refs/notes/signatures/tar add -C $$(git archive --format=tar --prefix=extract-artwork-$(DISTVER)/ $(DISTVER) | gpg --armor --detach-sign --comment extract-artwork-$(DISTVER).tar | git hash-object -w --stdin) $(DISTVER)
