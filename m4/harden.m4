# Look for flags to harden the object files and binaries
#
# Copyright (c) 2020, Karta Kooner.
#
# You may modify and/or distribute this file as you see fit without
# royalty or prior permission as long as any reasonable person would not
# attribute those modifications to me, and as long as both this notice and
# the copyright notice above are preserved.
#
# serial 1   harden.m4


# AV_HARDEN_CHECK_COMPILE_FLAG([flag], [variable_to_add_flag_to])
# --------------------------------------------------------------
# 
#   This macro function simply checks whether the compiler accepts `flag',
#   and adds `flag' to `variable_to_add_flag_to' if it does; otherwise it
#   leaves `variable_to_add_flag_to' alone. Note that this macro does not
#   call AC_LANG_PUSH or AC_LANG_POP -- you will need to do that before and
#   after this macro function is called.
#
#   External Macro Calls:
#       AC_MSG_CHECKING
#       AC_COMPILE_IFELSE
#       AC_LANG_PROGRAM
#       AC_MSG_RESULT
#
#   Variables Set:
#       harden_flag_set ("yes" if `flag' accepted, "no" otherwise)
#
AC_DEFUN([AV_HARDEN_CHECK_COMPILE_FLAG],
         [CXXFLAGS_saved="$CXXFLAGS"
          CXXFLAGS="$1"
          AC_MSG_CHECKING([whether $CXX supports $CXXFLAGS])
          AC_COMPILE_IFELSE(dnl
              [AC_LANG_PROGRAM([],[[return 0;]])],
              [AC_MSG_RESULT([yes])
               eval "$2=\"\$$2 \$CXXFLAGS\""
               harden_flag_set=yes],
              [AC_MSG_RESULT([no])
               harden_flag_set=no])
          CXXFLAGS="$CXXFLAGS_saved"
         ])


# AV_HARDEN_CHECK_LINK_FLAG([flag], [variable_to_add_flag_to])
# --------------------------------------------------------------
# 
#   This macro function simply checks whether the linker accepts `flag',
#   and adds `flag' to `variable_to_add_flag_to' if it does; otherwise it
#   leaves `variable_to_add_flag_to' alone. Note that this macro does not
#   call AC_LANG_PUSH or AC_LANG_POP -- you will need to do that before and
#   after this macro function is called.
#
#   External Macro Calls:
#       AC_MSG_CHECKING
#       AC_LINK_IFELSE
#       AC_LANG_PROGRAM
#       AC_MSG_RESULT
#
#   Variables Set:
#       harden_flag_set ("yes" if `flag' accepted, "no" otherwise)
#
AC_DEFUN([AV_HARDEN_CHECK_LINK_FLAG],
         [LDFLAGS_saved="$LDFLAGS"
          LDFLAGS="$1"
          AC_MSG_CHECKING([whether linker supports $LDFLAGS])
          AC_LINK_IFELSE(dnl
              [AC_LANG_PROGRAM([],[[return 0;]])],
              [AC_MSG_RESULT([yes])
               eval "$2=\"\$$2 \$LDFLAGS\""
               harden_flag_set=yes],
              [AC_MSG_RESULT([no])
               harden_flag_set=no])
          LDFLAGS="$LDFLAGS_saved"
         ])


# AV_HARDEN_FLAGS
# -----------------------------
#
#   This macro checks to see if certain object/binary file hardening flags
#   are accepted by the compiler. Compiler and linker flags are handled
#   separately.
#
#   Compiler flags are handled via $enable_harden_compiler_flags. If not set
#   to "no" then set to check for the flags contained in that variable, the
#   individual flags being colon separated.
#
#   Linker flags are handled via $enable_harden_linker_flags. If not set to
#   "no" then set to check for the flags contained in that variable, the
#   individual flags being colon separated.
#
#   External Macro Calls:
#       AC_LANG_PUSH
#       AC_LANG_POP
#       AS_IF
#       AC_SUBST
#       AV_OPTIM_CHECK_FLAG
#
#   Variables Called (none altered):
#       enable_fortify_source
#       enable_glibcxx_assertions
#       enable_asynchronous_unwind_tables
#       enable_stack_protector
#       enable_stack_clash_protection
#       enable_z_defs
#       enable_z_relro
#       enable_z_now
#
#   Variables Set:
#       HARDEN_CXXFLAGS (AC_SUBST'd)
#
AC_DEFUN([AV_HARDEN_FLAGS],
         [AC_LANG_PUSH([C++])
          harden_flag_set=initial
		  AS_IF([test "x$enable_checking" = xyes],
		  		[AV_HARDEN_CHECK_COMPILE_FLAG([-Wall],
											  [HARDEN_CPPFLAGS])
				 AV_HARDEN_CHECK_COMPILE_FLAG([-Wextra],
				 							  [HARDEN_CPPFLAGS])],
				[test "x$enable_checking" != xno],
				[oIFS="$IFS"
				 IFS=":"
				 for i in $enable_checking
				 do
				 	IFS="$oIFS"
					AV_HARDEN_CHECK_COMPILE_FLAG([$i],
												 [HARDEN_CPPFLAGS])
				 done
				 IFS="$oIFS"])
		  AS_IF([test "x$enable_fortify_source" = xyes],
		  		[AV_HARDEN_CHECK_COMPILE_FLAG([-D_FORTIFY_SOURCE=2],
											  [HARDEN_CPPFLAGS])],
				[test "x$enable_fortify_source" != xno],
				[AV_HARDEN_CHECK_COMPILE_FLAG([$enable_fortify_source],
											  [HARDEN_CPPFLAGS])])
		  AS_IF([test "x$enable_glibcxx_assertions" = xyes],
		  		[AV_HARDEN_CHECK_COMPILE_FLAG([-D_GLIBCXX_ASSERTIONS],
											  [HARDEN_CPPFLAGS])],
				[test "x$enable_glibcxx_assertions" != xno],
				[AV_HARDEN_CHECK_COMPILE_FLAG([$enable_glibcxx_assertions],
											  [HARDEN_CPPFLAGS])])
		  AS_IF([test "x$enable_asynchronous_unwind_tables" = xyes],
		  		[AV_HARDEN_CHECK_COMPILE_FLAG([-fasynchronous-unwind-tables],
											  [HARDEN_CXXFLAGS])],
				[test "x$enable_asynchronous_unwind_tables" != xno],
				[AV_HARDEN_CHECK_COMPILE_FLAG([$enable_asynchronous_unwind_tables],
											  [HARDEN_CXXFLAGS])])
          harden_flag_set=initial
		  AS_IF([test "x$enable_stack_protector" = xyes],
		  		[AV_HARDEN_CHECK_COMPILE_FLAG([-fstack-protector-strong],
											  [HARDEN_CXXFLAGS])
				 AS_IF([test "x$harden_flag_set" = xyes],
				 	   [AV_HARDEN_CHECK_COMPILE_FLAG([-Wstack-protector],
					   								 [HARDEN_CXXFLAGS])])],
				[test "x$enable_stack_protector" != xno],
				[AV_HARDEN_CHECK_COMPILE_FLAG([$enable_stack_protector],
											  [HARDEN_CXXFLAGS])])
		  AS_IF([test "x$enable_stack_clash_protection" = xyes],
		  		[AV_HARDEN_CHECK_COMPILE_FLAG([-fstack-clash-protection],
											  [HARDEN_CXXFLAGS])],
				[test "x$enable_stack_clash_protection" != xno],
				[AV_HARDEN_CHECK_COMPILE_FLAG([$enable_stack_clash_protection],
											  [HARDEN_CXXFLAGS])])
		  AS_IF([test "x$enable_z_defs" = xyes],
		  		[AV_HARDEN_CHECK_COMPILE_FLAG([-Wl,-z,defs],
											  [HARDEN_LDFLAGS])],
				[test "x$enable_z_defs" != xno],
				[AV_HARDEN_CHECK_COMPILE_FLAG([$enable_z_defs],
											  [HARDEN_LDFLAGS])])
		  AS_IF([test "x$enable_z_relro" = xyes],
		  		[AV_HARDEN_CHECK_COMPILE_FLAG([-Wl,-z,relro],
											  [HARDEN_LDFLAGS])],
				[test "x$enable_z_relro" != xno],
				[AV_HARDEN_CHECK_COMPILE_FLAG([$enable_z_relro],
											  [HARDEN_LDFLAGS])])
		  AS_IF([test "x$enable_z_now" = xyes],
		  		[AV_HARDEN_CHECK_COMPILE_FLAG([-Wl,-z,now],
											  [HARDEN_LDFLAGS])],
				[test "x$enable_z_now" != xno],
				[AV_HARDEN_CHECK_COMPILE_FLAG([$enable_z_now],
											  [HARDEN_LDFLAGS])])
          AC_SUBST([HARDEN_CPPFLAGS])
          AC_SUBST([HARDEN_CXXFLAGS])
          AC_SUBST([HARDEN_LDFLAGS])
          AC_LANG_POP([C++])
          :dnl
         ])
