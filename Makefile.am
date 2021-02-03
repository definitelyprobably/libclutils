ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src tests
dist_doc_DATA = COPYING README.md INSTALL.md AUTHORS.md NEWS.md ChangeLog.md

if FORCE_DOCS_COMPRESSION
install-data-hook:
	-for i in "$(DESTDIR)$(docdir)/NEWS.md" \
			  "$(DESTDIR)$(docdir)/AUTHORS.md" \
			  "$(DESTDIR)$(docdir)/ChangeLog.md" \
			  "$(DESTDIR)$(docdir)/COPYING" ;\
	do \
		libarithmos_dcomp_internal_var='@DCOMPRESS@' ;\
		oIFS="$$IFS" ; IFS=':' ; export IFS ;\
		for compress in $$libarithmos_dcomp_internal_var ;\
		do \
			IFS="$$oIFS" ;\
			$$compress --force -- "$$i" ;\
			break ;\
		done ;\
	done
else
install-data-hook:
	-for i in "$(DESTDIR)$(docdir)/NEWS.md" \
			  "$(DESTDIR)$(docdir)/AUTHORS.md" \
			  "$(DESTDIR)$(docdir)/ChangeLog.md" \
			  "$(DESTDIR)$(docdir)/COPYING" ;\
	do \
		libarithmos_dcomp_internal_var='@DCOMPRESS@' ;\
		oIFS="$$IFS" ; IFS=':' ; export IFS ;\
		for compress in $$libarithmos_dcomp_internal_var ;\
		do \
			IFS="$$oIFS" ;\
			if expr `$$compress --stdout -- "$$i" | wc -c` '>=' `cat -- "$$i" | wc -c` >/dev/null 2>&1 ;\
			then : ;\
			else \
				$$compress --force -- "$$i" && break ;\
			fi ;\
		done ;\
	done
endif

uninstall-hook:
	-rm -f --  "$(DESTDIR)$(docdir)"/NEWS.* \
			   "$(DESTDIR)$(docdir)"/AUTHORS.* \
			   "$(DESTDIR)$(docdir)"/ChangeLog.* \
			   "$(DESTDIR)$(docdir)"/COPYING*
