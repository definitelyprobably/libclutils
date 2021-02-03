# Installation instructions

Header-only library, so just need to install, although a check routine is also supplied; install as usual (adapt as appropriate):

```sh
$ mkdir build && cd build
$ ../configure
$ make check
$ make install
```

## Configure options

Besides the usual configure options, such as `--prefix`, the library defines the following:

### \-\-with-forced-docs-compression

The install target will compress documentation files only if they are smaller
than the original file. Specifying this option will unconditionally compress
the files.

### \-\-with-docs-compress-programs

The comma-separated list of binaries used to compress documentation files. By default, the install target with compress documentation files with each program sequentially until the compressed file is smaller than the original file, or else leave the file uncompressed. If `--with-forced-docs-compression` is also specified, then the first program in the list is always unconditionally used. The programs must accept the flags `--force` and `--stdout` in order to be considered valid compression programs. The default list is: "xz,bzip2,gzip".

### \-\-with-forced-manpage-compression

The install target will compress manpage files only if they are smaller than the original file. Specifying this option will unconditionally compress the files.

### \-\-with-manpage-compress-program

The program used to compress manpages. The program must accept the flags `--force` and `--stdout` in order to be considered a valid compression program. The default is "gzip".

### \-\-enable-lto

Add the `-flto` option to the compiler. Enabled by default. If given an input other than "yes" or "no", then interpret that input as an alternate option to use other than the default.

### \-\-enable-march

Add the `-march=native` option to the compiler. Enabled by default. If given an input other than "yes" or "no", then interpret that input as an alternate option to use other than the default.

### \-\-enable-fortify-source

Add the `-D_FORTIFY_SOURCE=2` option to the compiler. Enabled by default. If given an input other than "yes" or "no", then interpret that input as an alternate option to use other than the default.

### \-\-enable-glibcxx-assertions

Add the `-D_GLIBCXX_ASSERTIONS` option to the compiler. Enabled by default. If given an input other than "yes" or "no", then interpret that input as an alternate option to use other than the default.

### \-\-enable-asynchronous-unwind-tables

Add the `-fasynchronous-unwind-tables` option to the compiler. Enabled by default. If given an input other than "yes" or "no", then interpret that input as an alternate option to use other than the default.

### \-\-enable-stack-protector

Add the `-fstack-protector-strong` and `-Wstack-protector` options to the compiler. Enabled by default. If given an input other than "yes" or "no", then interpret that input as alternate options to use other than the default.

### \-\-enable-stack-clash-protection

Add the `-fstack-clash-protection` option to the compiler. Enabled by default. If given an input other than "yes" or "no", then interpret that input as an alternate option to use other than the default.

### \-\-enable-z-defs

Add the `-Wl,-z,defs` option to the compiler. Enabled by default. If given an input other than "yes" or "no", then interpret that input as an alternate option to use other than the default.

### \-\-enable-z-relro

Add the `-Wl,-z,relro` option to the compiler. Enabled by default. If given an input other than "yes" or "no", then interpret that input as an alternate option to use other than the default.

### \-\-enable-z-now

Add the `-Wl,-z,now` option to the compiler. Enabled by default. If given an input other than "yes" or "no", then interpret that input as an alternate option to use other than the default.

