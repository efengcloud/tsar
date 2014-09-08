##CFLAGS = -Wall -fPIC --shared -g -O2 -Wno-strict-aliasing
##LINK = $(CC) -I$(INCLUDE_DIR) $(CFLAGS)

AM_CPPFLAGS = -I $(top_srcdir)/include
export AM_CPPFLAGS
SUBDIRS = src devel conf
	
EXTRA_DIST=rpm/tsar-build.sh  rpm/tsar.spec.in  rpm/tsar-VER.txt\
	include/common.h  include/config.h  include/debug.h include/define.h\
	include/framework.h  include/output_file.h  include/output_nagios.h \
	include/output_print.h  include/public.h  include/tsar.h

release_dir:
	@$(am__remove_distdir)
	test -d .git && git clone . $(distdir) || svn export . $(distdir)
	cd $(distdir) && autoreconf -i
	rm -rf $(distdir)/autom4te.cache $(distdir)/.git

release: release_dir
	tardir=$(distdir) && $(am__tar) | bzip2 -9 -c >$(distdir).tar.bz2
	@$(am__remove_distdir)

release-sign: release
	md5sum -b $(distdir).tar.bz2 >$(distdir).tar.bz2.md5
	sha1sum -b $(distdir).tar.bz2 >$(distdir).tar.bz2.sha1
	gpg --armor --output $(distdir).tar.bz2.asc  --detach-sig $(distdir).tar.bz2

rpm: release
	rpmbuild --version
	mkdir -p $(PWD)/rpmbuild/SPECS $(PWD)/rpmbuild/SOURCES $(PWD)/rpmbuild/BUILD $(PWD)/rpmbuild/RPMS
	cp $(distdir).tar.bz2 $(PWD)/rpmbuild/SOURCES
	cp rpm/tsar.spec $(PWD)/rpmbuild/SPECS
	test -e ~/.rpmmacros && mv .rpmmacros.bak || true
	echo %_topdir $(PWD)/rpmbuild > ~/.rpmmacros
	echo %_release $(release) >> ~/.rpmmacros
	rpmbuild -bb $(PWD)/rpmbuild/SPECS/tsar.spec
	rm -f ~/.rpmmacros
	test -e .rpmmacros.bak && mv .rpmmacros.bak ~/.rpmmacros || true
	mv -f rpmbuild/RPMS/*/*.rpm .
	rm -rf rpmbuild
