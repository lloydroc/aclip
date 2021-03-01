AC_INIT([aclip], [1.0], [lloyd@lloydrochester.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
AC_CHECK_LIB([asound], [snd_pcm_open])
AC_CHECK_LIB([asound], [snd_pcm_hw_params_alloca])
AC_CHECK_LIB([asound], [snd_pcm_hw_params_any])
AC_CHECK_LIB([asound], [snd_pcm_hw_params_set_access])
AC_CHECK_LIB([asound], [snd_pcm_readi])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC


AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
