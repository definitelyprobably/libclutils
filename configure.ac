AC_PREREQ([2.69])
AC_INIT([libclutils], [0.0.0], [ksk@hotmail.co.uk])
AM_INIT_AUTOMAKE([foreign -Wall dist-bzip2 dist-xz])
AM_PROG_AR
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX
AC_PROG_INSTALL
AC_LN_S

AX_CXX_COMPILE_STDCXX([20])


# Define configure flag `--enable-forced-docs-compression' to force compression
# of the installed docs. We will set a make variable if given so that the
# appropriate code can be run in the makefile
AC_ARG_ENABLE([forced_docs_compression],
            [AS_HELP_STRING([--enable-forced-docs-compression],
                            [force compression of installed docs even if the
                             compressed file is larger than the original file.
                             With this option the first program in the list of
                             programs to compress the docs files will be used
                             and the rest ignored])],
            [],
            [enable_forced_docs_compression=no])
AM_CONDITIONAL([FORCE_DOCS_COMPRESSION],
               [test "x$enable_forced_docs_compression" = xyes])

# Define configure flag `--enable-docs-compress-programs'.
# Sets `enable_docs_compress_programs' with information on how the flag was
# given, and sets makefile variable `DCOMPRESS' with the name of the
# compressing program (or else `:' if told to not compress).
AC_ARG_ENABLE([docs_compress_programs],
            [AS_HELP_STRING([--enable-docs-compress-programs@<:@=PROG@:>@],
        [compress certain doc files to be installed on the system. If
         PROG is specified then use that comma-separated list of programs to
         compress the files @<:@default=yes, PROG=xz:bzip2:gzip@:>@])],
    [],
    [enable_docs_compress_programs=yes])
AS_IF([test "x$enable_docs_compress_programs" = xno], [dcompress=:],
      [test "x$enable_docs_compress_programs" = xyes],
          [dcompress="xz:bzip2:gzip"],
      [dcompress="$enable_docs_compress_programs"])
AS_IF([test "x$enable_docs_compress_programs" != xno],
      [oIFS="$IFS"
       IFS=":"
       for compress in $dcompress
       do
           IFS="$oIFS"
           AS_IF([test -z "$compress"], [continue])
           AC_MSG_CHECKING([acceptability of docs compression program, $compress])
           AS_IF([AS_ECHO(["x"]) | "$compress" --force --stdout -- >/dev/null 2>&1],
                 [AC_MSG_RESULT([ok])
                  AS_IF([test -z "$DCOMPRESS"],
                        [DCOMPRESS="$compress"],
                        [DCOMPRESS="${DCOMPRESS}:$compress"])],
                 [AC_MSG_RESULT([no])])
           AS_IF([test "x$enable_forced_docs_compression" = xyes],
                 [break])
       done
       IFS="$oIFS"
       AS_IF([test -z "$DCOMPRESS"],
             [DCOMPRESS=:])],
      [DCOMPRESS=:])
AC_SUBST([DCOMPRESS])

# Define comfigure flag `--enable-forced-manpage-compression' to force
# compression of the installed docs. We will set a make variable if given so
# that the appropriate code can be run in the makefile
AC_ARG_ENABLE([forced_manpage_compression],
            [AS_HELP_STRING([--enable-forced-manpage-compression],
                            [force compression of installed docs even if the
                             compressed file is larger than the original file.
                             With this option the first program in the list of
                             programs to compress the docs files will be used
                             and the rest ignored])],
            [],
            [enable_forced_manpage_compression=no])
AM_CONDITIONAL([FORCE_MANPAGE_COMPRESSION],
               [test "x$enable_forced_manpage_compression" = xyes])

# Define configure flag `--enable-manpage-compress-program'.
# Sets `enable_manpage_compress_program' with information on how the flag was
# given, and sets makefile variable `MCOMPRESS' with the name of the
# compressing program (or else `:' if told to not compress).
AC_ARG_ENABLE([manpage_compress_program],
    [AS_HELP_STRING([--enable-manpage-compress-program@<:@=PROG@:>@],
        [compress the manpage to be installed on the system. If
         PROG is specified then use that program to compress the files
         @<:@default=yes, PROG=gzip@:>@])],
    [],
    [enable_manpage_compress_program=yes])
AS_IF([test "x$enable_manpage_compress_program" = xno], [mcompress=:],
      [test "x$enable_manpage_compress_program" = xyes], [mcompress=gzip],
      [mcompress="$enable_manpage_compress_program"])
AS_IF([test "x$enable_manpage_compress_program" != xno],
      [AC_MSG_CHECKING([accebtability of manpage compression program, $mcompress])
       AS_IF([AS_ECHO(["x"]) | "$mcompress" --force --stdout -- >/dev/null 2>&1],
             [AC_MSG_RESULT([ok])
              MCOMPRESS="$mcompress"],
             [AC_MSG_RESULT([no])
              MCOMPRESS=:])],
      [MCOMPRESS=:])
AC_SUBST([MCOMPRESS])




AC_ARG_ENABLE([lto],
    [AS_HELP_STRING([--enable-lto@<:@=FLAG@:>@],
        [enable link-time optimizations. If FLAG is specified then use that
         as the option to pass to CXX @<:@default=yes, FLAG=-flto@:>@])],
    [],
    [enable_lto=yes])

AC_ARG_ENABLE([march],
    [AS_HELP_STRING([--enable-march@<:@=FLAG@:>@],
        [enable building to the native architecture of the HOST system. If
         FLAG is specified then use that as the option to pass to CXX
         @<:@default=yes, FLAG=-march=native@:>@])],
    [],
    [enable_march=yes])

# Checks for some optimizing flags (see the file `m4/optim.m4' for details)
AV_OPTIMIZE_FLAGS || AS_EXIT


AC_ARG_ENABLE([checking],
	[AS_HELP_STRING([--enable-checking@<:@=FLAG@:>@],
		[enable standard check flags as a comma-separated list
		 @<:@default=yes, FLAG=-Wall:-Wextra@:>@])],
	[],
	[enable_checking=yes])

AC_ARG_ENABLE([fortify_source],
	[AS_HELP_STRING([--enable-fortify-source@<:@=FLAG@:>@],
		[enable buffer overflow detection for some functions
		 @<:@default=yes, FLAG=-D_FORTIFY_SOURCE=2@:>@])],
	[],
	[enable_fortify_source=yes])

AC_ARG_ENABLE([glibcxx_assertions],
	[AS_HELP_STRING([--enable-glibcxx-assertions@<:@=FLAG@:>@],
		[enable extra error checking in the form of precondition assertions,
		 such as bounds checking in strings and null pointer checks when
		 dereferencing smart pointers
		 @<:@default=yes, FLAG=-D_GLIBCXX_ASSERTIONS@:>@])],
	[],
	[enable_glibcxx_assertions=yes])

AC_ARG_ENABLE([asynchronous_unwind_tables],
	[AS_HELP_STRING([--enable-asynchronous-unwind-tables@<:@=FLAG@:>@],
		[enable generating the unwind table in DWARF format, which could
		 improve backtrace information
		 @<:@default=yes, FLAG=-fasynchronous-unwind-tables@:>@])],
	[],
	[enable_asynchronous_unwind_tables=yes])

AC_ARG_ENABLE([stack_protector],
	[AS_HELP_STRING([--enable-stack-protector@<:@=FLAG@:>@],
		[enable protection against stack smashing attacks
		 @<:@default=yes, FLAG=-fstack-protector-strong@:>@])],
	[],
	[enable_stack_protector=yes])

AC_ARG_ENABLE([stack_clash_protection],
	[AS_HELP_STRING([--enable-stack-clash-protection@<:@=FLAG@:>@],
		[enable protection from stack clash style attacks
		 @<:@default=yes, FLAG=-fstack-clash-protection@:>@])],
	[],
	[enable_stack_clash_protection=yes])

AC_ARG_ENABLE([z_defs],
	[AS_HELP_STRING([--enable-z-defs@<:@=FLAG@:>@],
		[enable catching of undefined symbols during linking
		 @<:@default=yes, FLAG=-Wl,-z,defs@:>@])],
	[],
	[enable_z_defs=yes])

AC_ARG_ENABLE([z_relro],
	[AS_HELP_STRING([--enable-z-relro@<:@=FLAG@:>@],
		[enable protection from some overwrite attacks
		 @<:@default=yes, FLAG=-Wl,-z,relro@:>@])],
	[],
	[enable_z_relro=yes])

AC_ARG_ENABLE([z_now],
	[AS_HELP_STRING([--enable-z-now@<:@=FLAG@:>@],
		[enable enhanced protection from overwrite attacks when used in
		 conjunction with the relro flag above
		 @<:@default=yes, FLAG=-Wl,-z,now@:>@])],
	[],
	[enable_z_now=yes])

# Checks for some object/binary file hardening flags (see the file
# `m4/harden.m4 for details)
AV_HARDEN_FLAGS || AS_EXIT


AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
