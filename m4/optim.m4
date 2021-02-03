# Look for optimization flags to use
#
# Copyright (c) 2020, Karta Kooner.
#
# You may modify and/or distribute this file as you see fit without
# royalty or prior permission as long as any reasonable person would not
# attribute those modifications to me, and as long as both this notice and
# the copyright notice above are preserved.
#
# serial 2   optim.m4


# AV_OPTIM_CHECK_COMPILE_FLAG([flag], [variable_to_add_flag_to])
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
#       optim_flag_set ("yes" if `flag' accepted, "no" otherwise)
#
AC_DEFUN([AV_OPTIM_CHECK_COMPILE_FLAG],
         [CXXFLAGS_saved="$CXXFLAGS"
          CXXFLAGS="$1"
          AC_MSG_CHECKING([whether $CXX supports $CXXFLAGS])
          AC_COMPILE_IFELSE(dnl
              [AC_LANG_PROGRAM([],[[return 0;]])],
              [AC_MSG_RESULT([yes])
               eval "$2=\"\$$2 \$CXXFLAGS\""
               optim_flag_set=yes],
              [AC_MSG_RESULT([no])
               optim_flag_set=no])
          CXXFLAGS="$CXXFLAGS_saved"
         ])


# AV_OPTIM_CHECK_LINK_FLAG([flag], [variable_to_add_flag_to])
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
#       optim_flag_set ("yes" if `flag' accepted, "no" otherwise)
#
AC_DEFUN([AV_OPTIM_CHECK_LINK_FLAG],
         [LDFLAGS_saved="$LDFLAGS"
          LDFLAGS="$1"
          AC_MSG_CHECKING([whether linker supports $LDFLAGS])
          AC_LINK_IFELSE(dnl
              [AC_LANG_PROGRAM([],[[return 0;]])],
              [AC_MSG_RESULT([yes])
               eval "$2=\"\$$2 \$LDFLAGS\""
               optim_flag_set=yes],
              [AC_MSG_RESULT([no])
               optim_flag_set=no])
          LDFLAGS="$LDFLAGS_saved"
         ])


# AV_OPTIMIZE_FLAGS
# -----------------------------
#
#   This macro checks to see if certain optimization flags are accepted
#   by the compiler:
#       1. $enable_lto if it is not set to "no", or else `-flto'.
#       2. $enable_march if it is not set to "no", or else `-fmarch=native'.
#   or the linker:
#       <none>
#
#   External Macro Calls:
#       AC_LANG_PUSH
#       AC_LANG_POP
#       AS_IF
#       AC_SUBST
#       AV_OPTIM_CHECK_COMPILE_FLAG
#       AV_OPTIM_CHECK_LINK_FLAG
#
#   Variables Called (none altered):
#       enable_lto
#       enable_march
#       enable_gnu_hash
#       enable_as_needed
#
#   Variables Set:
#       OPTIM_CXXFLAGS (AC_SUBST'd)
#       OPTIM_LDFLAGS (AC_SUBST'd)
#
AC_DEFUN([AV_OPTIMIZE_FLAGS],
         [AC_LANG_PUSH([C++])
          optim_flag_set=initial
          AS_IF([test "x$enable_lto" = xyes],
                [AV_OPTIM_CHECK_COMPILE_FLAG([-flto], [OPTIM_CXXFLAGS])],
                [test "x$enable_lto" != xno],
                [AV_OPTIM_CHECK_COMPILE_FLAG([$enable_lto], [OPTIM_CXXFLAGS])])
          AS_IF([test "x$enable_march" = xyes],
                [AV_OPTIM_CHECK_COMPILE_FLAG([-march=native],
                                             [OPTIM_CXXFLAGS])],
                [test "x$enable_march" != xno],
                [AV_OPTIM_CHECK_COMPILE_FLAG([$enable_march],
                                             [OPTIM_CXXFLAGS])])
          AC_SUBST([OPTIM_CXXFLAGS])
          AC_LANG_POP([C++])
          :dnl
         ])
