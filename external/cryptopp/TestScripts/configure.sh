#!/usr/bin/env bash

# Written and placed in public domain by Jeffrey Walton
#
# This script attempts to update various config_xxx.h files based on the
# current toolchain. It fills a gap where some features are misdetected based
# on compiler version and associated macros, but the feature is (or is not)
# present. For example, modern Android toolchains should be AES-NI and AVX
# capable, but the project removes the feature support.
#
# Use the same compiler and environment to run configure and the makefile.
#
# To use the script, copy the script to the root of the Crypto++ directory.
# Set the environment, and then run the tool:
#
#   export CXX="..."
#   export CXXFLAGS="..."
#   export LDFLAGS="..."
#   ./configure.sh
#
# Android and iOS would use the following if you are using setenv-android.sh
# or setenv-ios.sh to set the environment. Otherwise the script expects
# CXX and CXXFLAGS to be set properly for Android or iOS.
#
#   export CXXFLAGS="$IOS_CXXFLAGS --sysroot=$IOS_SYSROOT"
# or
#   export CXXFLAGS="$ANDROID_CXXFLAGS --sysroot=$ANDROID_SYSROOT"
#
# Do not use this script for a multiarch environment unless the cpu features
# are the same for each arch. For example, -arch i386 -arch x86_64 could
# cause problems if x86 only included SSE4.2, while x64 included AVX.
#
# A wiki page is available for this script at
# https://www.cryptopp.com/wiki/Configure.sh
#
# This script was added at Crypto++ 8.3. Also see GH #850. This script will
# work with earlier versions of the library that use config_xxx.h files.
# The monolithic config.h was split into config_xxx.h in May 2019 at
# Crypto++ 8.3. Also see GH #835, PR #836.


# shellcheck disable=SC2086

# Verify the file exists and is writeable.
if [[ ! -w ./config_asm.h ]]; then
    echo "Crypto++ is too old. Unable to locate config_asm.h"
    exit 1
fi

TMPDIR="${TMPDIR:-$HOME/tmp}"
TPROG="${TPROG:-TestPrograms/test_cxx.cxx}"
TOUT="${TOUT:-a.out}"

CXX="${CXX:-c++}"
LD="${LD:-ld}"
CXXFLAGS="${CXXFLAGS:--DNDEBUG -g2 -O3}"
GREP="${GREP:-grep}"

if [[ -z "$(command -v ${CXX} 2>/dev/null)" ]]; then
  echo "Compiler is not valid. Please install a compiler"
  exit 1
fi

if [[ -z "$(command -v ${LD} 2>/dev/null)" ]]; then
  echo "Linker is not valid. Please install a linker"
  exit 1
fi

# Solaris fixup
if [[ -d /usr/gnu/bin ]]; then
  GREP=/usr/gnu/bin/grep
fi

SUN_COMPILER=$(${CXX} -V 2>/dev/null | ${GREP} -i -c -E 'CC: (Sun|Studio)')
XLC_COMPILER=$(${CXX} -qversion 2>/dev/null | ${GREP} -i -c "IBM XL")
CLANG_COMPILER=$(${CXX} --version 2>/dev/null | ${GREP} -i -c -E 'clang|llvm')

if [[ "$SUN_COMPILER" -ne 0 ]]
then
  IS_X86=$(uname -m 2>&1 | ${GREP} -c -E 'i386|i486|i585|i686')
  IS_X64=$(uname -m 2>&1 | ${GREP} -c -E 'i86pc|x86_64|amd64')
  IS_IA32=$(uname -m 2>&1 | ${GREP} -c -E 'i86pc|i386|i486|i585|i686|x86_64|amd64')
  IS_ARM32=0
  IS_ARMV8=0
  IS_PPC=0
  IS_PPC64=0
elif [[ "$XLC_COMPILER" -ne 0 ]]
then
  IS_X86=0
  IS_X64=0
  IS_IA32=0
  IS_ARM32=0
  IS_ARMV8=0
  IS_PPC=$(uname -m 2>&1 | ${GREP} -c -E 'ppc|powerpc')
  IS_PPC64=$(uname -m 2>&1 | ${GREP} -c -E 'ppc64|powerpc64')
elif [[ "$CLANG_COMPILER" -ne 0 ]]
then
  IS_X86=$(${CXX} ${CXXFLAGS} -dM -E ${TPROG} | ${GREP} -i -c -E 'i386|i486|i585|i686')
  IS_X64=$(${CXX} ${CXXFLAGS} -dM -E ${TPROG} | ${GREP} -i -c -E 'i86pc|x86_64|amd64')
  IS_IA32=$(${CXX} ${CXXFLAGS} -dM -E ${TPROG} | ${GREP} -i -c -E 'i86pc|i386|i486|i585|i686|x86_64|amd64')
  IS_ARM32=$(${CXX} ${CXXFLAGS} -dM -E ${TPROG} | ${GREP} -i -c -E 'arm|armhf|armv7|eabihf|armv8')
  IS_ARMV8=$(${CXX} ${CXXFLAGS} -dM -E ${TPROG} | ${GREP} -i -c -E 'aarch32|aarch64|arm64')
  IS_PPC=$(${CXX} ${CXXFLAGS} -dM -E ${TPROG} | ${GREP} -i -c -E 'ppc|powerpc')
  IS_PPC64=$(${CXX} ${CXXFLAGS} -dM -E ${TPROG} | ${GREP} -c -E 'ppc64|powerpc64')
else
  IS_X86=$(${CXX} ${CXXFLAGS} -dumpmachine 2>&1 | ${GREP} -i -c -E 'i386|i486|i585|i686')
  IS_X64=$(${CXX} ${CXXFLAGS} -dumpmachine 2>&1 | ${GREP} -i -c -E 'i86pc|x86_64|amd64')
  IS_IA32=$(${CXX} ${CXXFLAGS} -dumpmachine 2>&1 | ${GREP} -i -c -E 'i86pc|i386|i486|i585|i686|x86_64|amd64')
  IS_ARM32=$(${CXX} ${CXXFLAGS} -dumpmachine 2>&1 | ${GREP} -i -c -E 'arm|armhf|armv7|eabihf|armv8')
  IS_ARMV8=$(${CXX} ${CXXFLAGS} -dumpmachine 2>&1 | ${GREP} -i -c -E 'aarch32|aarch64|arm64')
  IS_PPC=$(${CXX} ${CXXFLAGS} -dumpmachine 2>&1 | ${GREP} -i -c -E 'ppc|powerpc')
  IS_PPC64=$(${CXX} ${CXXFLAGS} -dumpmachine 2>&1 | ${GREP} -i -c -E 'ppc64|powerpc64')
fi

# Default values for setenv-*.sh scripts
IS_IOS="${IS_IOS:-0}"
IS_ANDROID="${IS_ANDROID:-0}"
TIMESTAMP=$(date "+%A, %B %d %Y, %I:%M %p")

# ===========================================================================
# ================================== Fixups =================================
# ===========================================================================

# A 64-bit platform often matches the 32-bit variant due to appending '64'
if [[ "${IS_X64}" -ne 0 ]]; then IS_X86=0; fi
if [[ "${IS_ARMV8}" -ne 0 ]]; then IS_ARM32=0; fi
if [[ "${IS_PPC64}" -ne 0 ]]; then IS_PPC=0; fi

# ===========================================================================
# =================================== Info ==================================
# ===========================================================================

if [[ "${IS_X86}" -ne 0 ]]; then echo "Configuring for x86"; fi
if [[ "${IS_X64}" -ne 0 ]]; then echo "Configuring for x86_64"; fi
if [[ "${IS_ARM32}" -ne 0 ]]; then echo "Configuring for ARM32"; fi
if [[ "${IS_ARMV8}" -ne 0 ]]; then echo "Configuring for Aarch64"; fi
if [[ "${IS_PPC}" -ne 0 ]]; then echo "Configuring for PowerPC"; fi
if [[ "${IS_PPC64}" -ne 0 ]]; then echo "Configuring for PowerPC64"; fi

echo "Compiler: $(command -v ${CXX})"
echo "Linker: $(command -v ${LD})"

# ===========================================================================
# =============================== config_asm.h ==============================
# ===========================================================================

rm -f config_asm.h.new

# Common header
{
  echo '// config_asm.h rewritten by configure.sh script'
  echo '//' "${TIMESTAMP}"
  echo '// Also see https://www.cryptopp.com/wiki/configure.sh'
  echo ''
  echo '#ifndef CRYPTOPP_CONFIG_ASM_H'
  echo '#define CRYPTOPP_CONFIG_ASM_H'
  echo ''
} >> config_asm.h.new

#############################################################################
# Pickup CRYPTOPP_DISABLE_ASM

disable_asm=$($GREP -c '\-DCRYPTOPP_DISABLE_ASM' <<< "${CPPFLAGS} ${CXXFLAGS}")
if [[ "$disable_asm" -ne 0 ]]; then

  # Shell redirection
  {
    echo ''
    echo '// Set in CPPFLAGS or CXXFLAGS'
    echo '#define CRYPTOPP_DISABLE_ASM 1'
  } >> config_asm.h.new

fi

#############################################################################
# Intel x86-based machines

if [[ "$disable_asm" -eq 0 && "$IS_IA32" -ne 0 ]]; then

  if [[ "${SUN_COMPILER}" -ne 0 ]]; then
    SSE2_FLAG=-xarch=sse2
    SSE3_FLAG=-xarch=sse3
    SSSE3_FLAG=-xarch=ssse3
    SSE41_FLAG=-xarch=sse4_1
    SSE42_FLAG=-xarch=sse4_2
    CLMUL_FLAG=-xarch=aes
    AESNI_FLAG=-xarch=aes
    RDRAND_FLAG=-xarch=avx_i
    RDSEED_FLAG=-xarch=avx2_i
    AVX_FLAG=-xarch=avx
    AVX2_FLAG=-xarch=avx2
    SHANI_FLAG=-xarch=sha
  else
    SSE2_FLAG=-msse2
    SSE3_FLAG=-msse3
    SSSE3_FLAG=-mssse3
    SSE41_FLAG=-msse4.1
    SSE42_FLAG=-msse4.2
    CLMUL_FLAG=-mpclmul
    AESNI_FLAG=-maes
    RDRAND_FLAG=-mrdrnd
    RDSEED_FLAG=-mrdseed
    AVX_FLAG=-mavx
    AVX2_FLAG=-mavx2
    SHANI_FLAG=-msha
  fi

  # Shell redirection
  {

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${SSE2_FLAG} TestPrograms/test_x86_sse2.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -ne 0 ]]; then
    echo '#define CRYPTOPP_DISABLE_ASM 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${SSE2_FLAG} TestPrograms/test_asm_sse2.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_X86_ASM_AVAILABLE 1'
    if [[ "${IS_X64}" -ne 0 ]]; then
      echo '#define CRYPTOPP_X64_ASM_AVAILABLE 1'
      echo '#define CRYPTOPP_SSE2_ASM_AVAILABLE 1'
    fi
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${SSE2_FLAG} TestPrograms/test_x86_sse2.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    have_sse2=1
    echo '#define CRYPTOPP_SSE2_INTRIN_AVAILABLE 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${SSE3_FLAG} TestPrograms/test_x86_sse3.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    have_sse3=1
    echo '#define CRYPTOPP_SSE3_AVAILABLE 1'
  else
    have_sse3=0
    echo '#define CRYPTOPP_DISABLE_SSE3 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${SSSE3_FLAG} TestPrograms/test_x86_ssse3.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_sse3" -ne 0 ]]; then
    have_ssse3=1
    echo '#define CRYPTOPP_SSSE3_ASM_AVAILABLE 1'
    echo '#define CRYPTOPP_SSSE3_AVAILABLE 1'
  else
    have_ssse3=0
    echo '#define CRYPTOPP_DISABLE_SSSE3 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${SSE41_FLAG} TestPrograms/test_x86_sse41.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_ssse3" -ne 0 ]]; then
    have_sse41=1
    echo '#define CRYPTOPP_SSE41_AVAILABLE 1'
  else
    have_sse41=0
    echo '#define CRYPTOPP_DISABLE_SSE4 1'
    echo '#define CRYPTOPP_DISABLE_SSE41 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${SSE42_FLAG} TestPrograms/test_x86_sse42.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_sse41" -ne 0 ]]; then
    have_sse42=1
    echo '#define CRYPTOPP_SSE42_AVAILABLE 1'
  else
    have_sse42=0
    echo '#define CRYPTOPP_DISABLE_SSE4 1'
    echo '#define CRYPTOPP_DISABLE_SSE42 1'
  fi

  ########################################################
  # AES, CLMUL, RDRAND, RDSEED, SHA and AVX tied to SSE4.2

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${CLMUL_FLAG} TestPrograms/test_x86_clmul.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_sse42" -ne 0 ]]; then
    echo '#define CRYPTOPP_CLMUL_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_CLMUL 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${AESNI_FLAG} TestPrograms/test_x86_aes.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_sse42" -ne 0 ]]; then
    echo '#define CRYPTOPP_AESNI_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_AESNI 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${RDRAND_FLAG} TestPrograms/test_x86_rdrand.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_sse42" -ne 0 ]]; then
    echo '#define CRYPTOPP_RDRAND_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_RDRAND 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${RDSEED_FLAG} TestPrograms/test_x86_rdseed.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_sse42" -ne 0 ]]; then
    echo '#define CRYPTOPP_RDSEED_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_RDSEED 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${SHANI_FLAG} TestPrograms/test_x86_sha.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_sse42" -ne 0 ]]; then
    echo '#define CRYPTOPP_SHANI_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_SHANI 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${AVX_FLAG} TestPrograms/test_x86_avx.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_sse42" -ne 0 ]]; then
    have_avx=1
    echo '#define CRYPTOPP_AVX_AVAILABLE 1'
  else
    have_avx=0
    echo '#define CRYPTOPP_DISABLE_AVX 1'
  fi

  #####################
  # AVX2 depends on AVX

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${AVX2_FLAG} TestPrograms/test_x86_avx2.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_avx" -ne 0 ]]; then
    have_avx2=1
    echo '#define CRYPTOPP_AVX2_AVAILABLE 1'
  else
    have_avx2=0
    echo '#define CRYPTOPP_DISABLE_AVX2 1'
  fi

  # No flags, requires inline ASM
  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_x86_via_rng.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_PADLOCK_RNG_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_PADLOCK_RNG 1'
  fi

  # No flags, requires inline ASM
  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_x86_via_aes.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_PADLOCK_AES_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_PADLOCK_AES 1'
  fi

  # No flags, requires inline ASM
  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_x86_via_sha.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_PADLOCK_SHA_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_PADLOCK_SHA 1'
  fi

  # Clang workaround
  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_asm_mixed.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -ne 0 ]]; then
    echo '#define CRYPTOPP_DISABLE_MIXED_ASM 1'
  fi

  if [[ "${SUN_COMPILER}" -ne 0 ]]; then

    echo ''
    echo '// Fixup for SunCC 12.1-12.4. Bad code generation in AES_Encrypt and friends.'
    echo '#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x5130)'
    echo '# undef CRYPTOPP_AESNI_AVAILABLE'
    echo '#endif'
    echo ''
    echo '// Fixup for SunCC 12.1-12.6. Compiler crash on GCM_Reduce_CLMUL.'
    echo '// http://github.com/weidai11/cryptopp/issues/226'
    echo '#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x5150)'
    echo '# undef CRYPTOPP_CLMUL_AVAILABLE'
    echo '#endif'
  fi

  } >> config_asm.h.new

fi

#############################################################################
# ARM 32-bit machines

if [[ "$disable_asm" -eq 0 && "$IS_ARM32" -ne 0 ]]; then

  # IS_IOS is set when ./setenv-ios is run
  if [[ "$IS_IOS" -ne 0 ]]; then
    ARMV7_FLAG="-arch arm"
    NEON_FLAG="-arch arm"
  elif [[ "$CLANG_COMPILER" -ne 0 ]]; then
    ARMV7_FLAG="-march=armv7"
    NEON_FLAG="-march=armv7 -mfpu=neon"
  else
    ARMV7_FLAG="-march=armv7"
    NEON_FLAG="-mfpu=neon"
  fi

  # Shell redirection
  {

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${NEON_FLAG} TestPrograms/test_arm_neon_header.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_NEON_HEADER 1'
    HDRFLAGS="-DCRYPTOPP_ARM_NEON_HEADER=1"
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${ARMV7_FLAG} TestPrograms/test_cxx.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_ARMV7_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_ARMV7 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${NEON_FLAG} TestPrograms/test_arm_neon.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_NEON_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_NEON 1'
  fi

  # Cryptogams is special. Attempt to compile the actual source files
  # TestPrograms/test_cxx.cxx is needed for main().
  CXX_RESULT=$(${CXX} ${CXXFLAGS} aes_armv4.S TestPrograms/test_cxx.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOGAMS_ARM_AES 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} sha1_armv4.S TestPrograms/test_cxx.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOGAMS_ARM_SHA1 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} sha256_armv4.S TestPrograms/test_cxx.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOGAMS_ARM_SHA256 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} sha512_armv4.S TestPrograms/test_cxx.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOGAMS_ARM_SHA512 1'
  fi

  } >> config_asm.h.new

fi

#############################################################################
# ARM 64-bit machines

if [[ "$disable_asm" -eq 0 && "$IS_ARMV8" -ne 0 ]]; then

  # IS_IOS is set when ./setenv-ios is run
  if [[ "$IS_IOS" -ne 0 ]]; then
    ARMV8_FLAG="-arch arm64"
    ARMV81_CRC_FLAG="-arch arm64"
    ARMV81_CRYPTO_FLAG="-arch arm64"
    ARMV84_CRYPTO_FLAG="-arch arm64"
  else
    ARMV8_FLAG="-march=armv8-a"
    ARMV81_CRC_FLAG="-march=armv8-a+crc"
    ARMV81_CRYPTO_FLAG="-march=armv8-a+crypto"
    ARMV84_CRYPTO_FLAG="-march=armv8.4-a+crypto"
  fi

  # Shell redirection
  {

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_arm_neon_header.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_NEON_HEADER 1'
    HDRFLAGS="-DCRYPTOPP_ARM_NEON_HEADER=1"
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} TestPrograms/test_arm_acle_header.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_ACLE_HEADER 1'
    HDRFLAGS="${HDRFLAGS} -DCRYPTOPP_ARM_ACLE_HEADER=1"
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} TestPrograms/test_arm_neon.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_NEON_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_NEON 1'
  fi

  # This should be an unneeded test. ASIMD on Aarch64 is NEON on A32 and T32
  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} TestPrograms/test_arm_asimd.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_ASIMD_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_ASIMD 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${ARMV81_CRC_FLAG} TestPrograms/test_arm_crc.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_CRC32_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_CRC32 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${ARMV81_CRYPTO_FLAG} TestPrograms/test_arm_aes.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_AES_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_AES 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${ARMV81_CRYPTO_FLAG} TestPrograms/test_arm_pmull.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_PMULL_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_PMULL 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${ARMV81_CRYPTO_FLAG} TestPrograms/test_arm_sha1.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_SHA_AVAILABLE 1'
    echo '#define CRYPTOPP_ARM_SHA1_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_SHA 1'
    echo '#define CRYPTOPP_DISABLE_ARM_SHA1 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${ARMV81_CRYPTO_FLAG} TestPrograms/test_arm_sha256.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_SHA2_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_SHA2 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${ARMV84_CRYPTO_FLAG} TestPrograms/test_arm_sha3.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_SHA3_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_SHA3 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${ARMV84_CRYPTO_FLAG} TestPrograms/test_arm_sha512.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_SHA512_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_SHA512 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${ARMV84_CRYPTO_FLAG} TestPrograms/test_arm_sm3.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_SM3_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_SM3 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${HDRFLAGS} ${ARMV84_CRYPTO_FLAG} TestPrograms/test_arm_sm4.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_ARM_SM4_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_ARM_SM4 1'
  fi

  } >> config_asm.h.new

fi

#############################################################################
# PowerPC machines

if [[ "$disable_asm" -eq 0 &&  ("$IS_PPC" -ne 0 || "$IS_PPC64" -ne 0) ]]; then

  if [[ "${XLC_COMPILER}" -ne 0 ]]; then
    POWER9_FLAG="-qarch=pwr9 -qaltivec"
    POWER8_FLAG="-qarch=pwr8 -qaltivec"
    POWER7_VSX_FLAG="-qarch=pwr7 -qvsx -qaltivec"
    POWER7_PWR_FLAG="-qarch=pwr7 -qaltivec"
    ALTIVEC_FLAG="-qaltivec"
  else
    POWER9_FLAG="-mcpu=power9"
    POWER8_FLAG="-mcpu=power8"
    POWER7_VSX_FLAG="-mcpu=power7 -mvsx"
    POWER7_PWR_FLAG="-mcpu=power7"
    ALTIVEC_FLAG="-maltivec"
  fi

  # Shell redirection
  {

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${ALTIVEC_FLAG} TestPrograms/test_ppc_altivec.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    have_altivec=1
    echo '#define CRYPTOPP_ALTIVEC_AVAILABLE 1'
  else
    have_altivec=0
    echo '#define CRYPTOPP_DISABLE_ALTIVEC 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${POWER7_PWR_FLAG} TestPrograms/test_ppc_power7.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_altivec" -ne 0 ]]; then
    have_power7=1
    echo '#define CRYPTOPP_POWER7_AVAILABLE 1'
  else
    have_power7=0
    echo '#define CRYPTOPP_DISABLE_POWER7 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${POWER8_FLAG} TestPrograms/test_ppc_power8.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_power7" -ne 0 ]]; then
    have_power8=1
    echo '#define CRYPTOPP_POWER8_AVAILABLE 1'
  else
    have_power8=0
    echo '#define CRYPTOPP_DISABLE_POWER8 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${POWER9_FLAG} TestPrograms/test_ppc_power9.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -ne 0 && "$have_power8" -ne 0 ]]; then
    have_power9=1
    echo '#define CRYPTOPP_POWER9_AVAILABLE 1'
  else
    have_power9=0
    echo '#define CRYPTOPP_DISABLE_POWER9 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${POWER8_FLAG} TestPrograms/test_ppc_aes.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_power8" -ne 0 ]]; then
    echo '#define CRYPTOPP_POWER8_AES_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_POWER8_AES 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${POWER8_FLAG} TestPrograms/test_ppc_vmull.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_power8" -ne 0 ]]; then
    echo '#define CRYPTOPP_POWER8_VMULL_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_POWER8_VMULL 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} ${POWER8_FLAG} TestPrograms/test_ppc_sha.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 && "$have_power8" -ne 0 ]]; then
    echo '#define CRYPTOPP_POWER8_SHA_AVAILABLE 1'
  else
    echo '#define CRYPTOPP_DISABLE_POWER8_SHA 1'
  fi

  } >> config_asm.h.new

fi

# Common footer
{
  echo ''
  echo '#endif  // CRYPTOPP_CONFIG_ASM_H'
  echo ''
} >> config_asm.h.new

if [[ -e config_asm.h ]]; then
  cp config_asm.h config_asm.h.old
  mv config_asm.h.new config_asm.h
fi

# ===========================================================================
# =============================== config_cxx.h ==============================
# ===========================================================================

rm -f config_cxx.h.new

# Common header
{
  echo '// config_cxx.h rewritten by configure.sh script'
  echo '//' "${TIMESTAMP}"
  echo '// Also see https://www.cryptopp.com/wiki/configure.sh'
  echo ''
  echo '#ifndef CRYPTOPP_CONFIG_CXX_H'
  echo '#define CRYPTOPP_CONFIG_CXX_H'
} >> config_cxx.h.new

# Shell redirection
{
  echo ''
  echo '// ***************** C++98 and C++03 ********************'
  echo ''

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx98_exception.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '// Ancient Crypto++ define, dating back to C++98.'
    echo '#define CRYPTOPP_UNCAUGHT_EXCEPTION_AVAILABLE 1'
    echo '#define CRYPTOPP_CXX98_UNCAUGHT_EXCEPTION 1'
  else
    echo '// Ancient Crypto++ define, dating back to C++98.'
    echo '// #define CRYPTOPP_UNCAUGHT_EXCEPTION_AVAILABLE 1'
    echo '// #define CRYPTOPP_CXX98_UNCAUGHT_EXCEPTION 1'
  fi

  echo ''
  echo '// ***************** C++11 and above ********************'
  echo ''

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11 1'
  else
    echo '// test_cxx11.cxx returned non-zero result'
    echo '// #define CRYPTOPP_CXX11 1'
  fi

  echo ''
  echo '#if defined(CRYPTOPP_CXX11)'
  echo ''

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_atomic.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_ATOMIC 1'
  else
    echo '// #define CRYPTOPP_CXX11_ATOMIC 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_auto.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_AUTO 1'
  else
    echo '// #define CRYPTOPP_CXX11_AUTO 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_sync.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_SYNCHRONIZATION 1'
  else
    echo '// #define CRYPTOPP_CXX11_SYNCHRONIZATION 1'
  fi

  # CRYPTOPP_CXX11_DYNAMIC_INIT is old name
  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_staticinit.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_STATIC_INIT 1'
    echo '#define CRYPTOPP_CXX11_DYNAMIC_INIT 1'
  else
    echo '// #define CRYPTOPP_CXX11_STATIC_INIT 1'
    echo '// #define CRYPTOPP_CXX11_DYNAMIC_INIT 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_deletefn.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_DELETED_FUNCTIONS 1'
  else
    echo '// #define CRYPTOPP_CXX11_DELETED_FUNCTIONS 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_alignas.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_ALIGNAS 1'
  else
    echo '// #define CRYPTOPP_CXX11_ALIGNAS 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_alignof.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_ALIGNOF 1'
  else
    echo '// #define CRYPTOPP_CXX11_ALIGNOF 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_initializer.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_INITIALIZER_LIST 1'
  else
    echo '// #define CRYPTOPP_CXX11_INITIALIZER_LIST 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_lambda.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_LAMBDA 1'
  else
    echo '// #define CRYPTOPP_CXX11_LAMBDA 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_noexcept.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_NOEXCEPT 1'
  else
    echo '// #define CRYPTOPP_CXX11_NOEXCEPT 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_vartemplates.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_VARIADIC_TEMPLATES 1'
  else
    echo '// #define CRYPTOPP_CXX11_VARIADIC_TEMPLATES 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_constexpr.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_CONSTEXPR 1'
  else
    echo '// #define CRYPTOPP_CXX11_CONSTEXPR 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_enumtype.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_STRONG_ENUM 1'
  else
    echo '// #define CRYPTOPP_CXX11_STRONG_ENUM 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_nullptr.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_NULLPTR 1'
  else
    echo '// #define CRYPTOPP_CXX11_NULLPTR 1'
  fi

  # 2-argument static assert
  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx11_assert.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX11_ASSERT 1'
  else
    echo '// #define CRYPTOPP_CXX11_ASSERT 1'
  fi

  echo ''
  echo '#endif  // CRYPTOPP_CXX11'

  echo ''
  echo '// ***************** C++14 and above ********************'
  echo ''

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx14.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX14 1'
  else
    echo '// test_cxx14.cxx returned non-zero result'
    echo '// #define CRYPTOPP_CXX14 1'
  fi

  echo ''
  echo '#if defined(CRYPTOPP_CXX14)'
  echo ''
  echo '// No dead bodies here. Move on...'
  echo ''
  echo '#endif  // CRYPTOPP_CXX14'

  echo ''
  echo '// ***************** C++17 and above ********************'
  echo ''

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx17.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX17 1'
  else
    echo '// test_cxx17.cxx returned non-zero result'
    echo '// #define CRYPTOPP_CXX17 1'
  fi

  echo ''
  echo '#if defined(CRYPTOPP_CXX17)'
  echo ''

  # 1-argument static assert
  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx17_assert.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX17_ASSERT 1'
  else
    echo '// #define CRYPTOPP_CXX17_ASSERT 1'
  fi

  CXX_RESULT=$(${CXX} ${CXXFLAGS} TestPrograms/test_cxx17_exceptions.cxx -o ${TOUT} 2>&1 | wc -w)
  if [[ "${CXX_RESULT}" -eq 0 ]]; then
    echo '#define CRYPTOPP_CXX17_UNCAUGHT_EXCEPTIONS 1'
  else
    echo '// #define CRYPTOPP_CXX17_UNCAUGHT_EXCEPTIONS 1'
  fi

  echo ''
  echo '#endif  // CRYPTOPP_CXX17'

  echo ''
  echo '// ***************** C++ fixups ********************'
  echo ''

  echo '#if defined(CRYPTOPP_CXX11_NOEXCEPT)'
  echo '#  define CRYPTOPP_THROW noexcept(false)'
  echo '#  define CRYPTOPP_NO_THROW noexcept(true)'
  echo '#else'
  echo '#  define CRYPTOPP_THROW'
  echo '#  define CRYPTOPP_NO_THROW'
  echo '#endif // CRYPTOPP_CXX11_NOEXCEPT'
  echo ''
  echo '// C++11 nullptr_t type safety and analysis'
  echo '#if defined(CRYPTOPP_CXX11_NULLPTR) && !defined(NULLPTR)'
  echo '#  define NULLPTR nullptr'
  echo '#elif !defined(NULLPTR)'
  echo '#  define NULLPTR NULL'
  echo '#endif // CRYPTOPP_CXX11_NULLPTR'

} >> config_cxx.h.new

# Common footer
{
  echo ''
  echo '#endif  // CRYPTOPP_CONFIG_CXX_H'
  echo ''
} >> config_cxx.h.new

if [[ -e config_cxx.h ]]; then
  cp config_cxx.h config_cxx.h.old
  mv config_cxx.h.new config_cxx.h
fi

rm -f "${TOUT}"

exit 0
