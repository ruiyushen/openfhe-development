//==================================================================================
// BSD 2-Clause License
//
// Copyright (c) 2014-2022, NJIT, Duality Technologies Inc. and other contributors
//
// All rights reserved.
//
// Author TPOC: contact@openfhe.org
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==================================================================================

/*
  unit tests for the SHE capabilities
 */

#include "UnitTestUtils.h"
#include "UnitTestCCParams.h"
#include "UnitTestCryptoContext.h"

#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include <cxxabi.h>
#include "utils/demangle.h"


using namespace lbcrypto;

//===========================================================================================================
enum TEST_CASE_TYPE {
    ADD_PACKED = 0,
    MULT_COEF_PACKED,
    MULT_PACKED,
    EVALATINDEX,
    EVALMERGE,
    EVALSUM,
    METADATA,
    EVALSUM_ALL,
    KS_SINGLE_CRT,
    KS_MOD_REDUCE_DCRT,
};

static std::ostream& operator<<(std::ostream& os, const TEST_CASE_TYPE& type) {
    std::string typeName;
    switch (type) {
    case ADD_PACKED:
        typeName = "ADD_PACKED";
        break;
    case MULT_COEF_PACKED:
        typeName = "MULT_COEF_PACKED";
        break;
    case MULT_PACKED:
        typeName = "MULT_PACKED";
        break;
    case EVALATINDEX:
        typeName = "EVALATINDEX";
        break;
    case EVALMERGE:
        typeName = "EVALMERGE";
        break;
    case EVALSUM:
        typeName = "EVALSUM";
        break;
    case METADATA:
        typeName = "METADATA";
        break;
    case EVALSUM_ALL:
        typeName = "EVALSUM_ALL";
        break;
    case KS_SINGLE_CRT:
        typeName = "KS_SINGLE_CRT";
        break;
    case KS_MOD_REDUCE_DCRT:
        typeName = "KS_MOD_REDUCE_DCRT";
        break;
    default:
        typeName = "UNKNOWN";
        break;
    }
    return os << typeName;
}
//===========================================================================================================
struct TEST_CASE_UTSHE {
    TEST_CASE_TYPE testCaseType;
    // test case description - MUST BE UNIQUE
    std::string description;

    UnitTestCCParams  params;

    // additional test case data
    // ........

    std::string buildTestName() const {
        std::stringstream ss;
        ss << testCaseType << "_" << description;
        return ss.str();
    }
    std::string toString() const {
        std::stringstream ss;
        ss << "testCaseType [" << testCaseType << "], " << params.toString();
        return ss.str();
    }
};

// this lambda provides a name to be printed for every test run by INSTANTIATE_TEST_SUITE_P.
// the name MUST be constructed from digits, letters and '_' only
static auto testName = [](const testing::TestParamInfo<TEST_CASE_UTSHE>& test) {
    return test.param.buildTestName();
};

static std::ostream& operator<<(std::ostream& os, const TEST_CASE_UTSHE& test) {
    return os << test.toString();
}
//===========================================================================================================
// NOTE the SHE tests are all based on these
constexpr usint BATCH     = 16;
constexpr usint BATCH_LRG = 1 << 12;
constexpr usint PTM     = 64;
constexpr usint PTM_LRG = 65537;
constexpr usint BV_DSIZE = 4;
// clang-format off
static std::vector<TEST_CASE_UTSHE> testCases = {
    // TestType,  Descr, Scheme,        RDim, MultDepth, SFBits, DSize,    BatchSz, SecKeyDist,      MaxRelinSkDeg, ModSize, SecLvl,       KSTech, ScalTech,        LDigits, PtMod, StdDev, EvalAddCt, EvalMultCt, KSCt, MultTech,         EncTech
    { ADD_PACKED, "01", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY, 1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { ADD_PACKED, "02", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY, 1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { ADD_PACKED, "03", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY, 1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { ADD_PACKED, "04", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY, 1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { ADD_PACKED, "05", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,        1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { ADD_PACKED, "06", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,        1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { ADD_PACKED, "07", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,        1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { ADD_PACKED, "08", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,        1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { ADD_PACKED, "09", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPS,              STANDARD}, },
    { ADD_PACKED, "10", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPS,              STANDARD}, },
    { ADD_PACKED, "11", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, BEHZ,             STANDARD}, },
    { ADD_PACKED, "12", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, BEHZ,             STANDARD}, },
    { ADD_PACKED, "13", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQ,        STANDARD}, },
    { ADD_PACKED, "14", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQ,        STANDARD}, },
    { ADD_PACKED, "15", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { ADD_PACKED, "16", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { ADD_PACKED, "17", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPS,              STANDARD}, },
    { ADD_PACKED, "18", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPS,              STANDARD}, },
    { ADD_PACKED, "19", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, BEHZ,             STANDARD}, },
    { ADD_PACKED, "20", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, BEHZ,             STANDARD}, },
    { ADD_PACKED, "21", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQ,        STANDARD}, },
    { ADD_PACKED, "22", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQ,        STANDARD}, },
    { ADD_PACKED, "23", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { ADD_PACKED, "24", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { ADD_PACKED, "25", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPS,              POVERQ},   },
    { ADD_PACKED, "26", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPS,              POVERQ},   },
    { ADD_PACKED, "27", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, BEHZ,             POVERQ},   },
    { ADD_PACKED, "28", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, BEHZ,             POVERQ},   },
    { ADD_PACKED, "29", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQ,        POVERQ},   },
    { ADD_PACKED, "30", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQ,        POVERQ},   },
    { ADD_PACKED, "31", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { ADD_PACKED, "32", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { ADD_PACKED, "33", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPS,              POVERQ},   },
    { ADD_PACKED, "34", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPS,              POVERQ},   },
    { ADD_PACKED, "35", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, BEHZ,             POVERQ},   },
    { ADD_PACKED, "36", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, BEHZ,             POVERQ},   },
    { ADD_PACKED, "37", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQ,        POVERQ},   },
    { ADD_PACKED, "38", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQ,        POVERQ},   },
    { ADD_PACKED, "39", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY, DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { ADD_PACKED, "40", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,        DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,      1,      0,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    // ==========================================
    // TestType,        Descr, Scheme,        RDim, MultDepth, SFBits, DSize,    BatchSz, SecKeyDist,       MaxRelinSkDeg, ModSize, SecLvl,       KSTech, ScalTech,        LDigits, PtMod, StdDev, EvalAddCt, EvalMultCt, KSCt, MultTech,         EncTech
    { MULT_COEF_PACKED, "01", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_COEF_PACKED, "02", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_COEF_PACKED, "03", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_COEF_PACKED, "04", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_COEF_PACKED, "05", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_COEF_PACKED, "06", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_COEF_PACKED, "07", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_COEF_PACKED, "08", {BGVRNS_SCHEME, 16,   2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM,   DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_COEF_PACKED, "09", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPS,              STANDARD}, },
    { MULT_COEF_PACKED, "10", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPS,              STANDARD}, },
    { MULT_COEF_PACKED, "11", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, BEHZ,             STANDARD}, },
    { MULT_COEF_PACKED, "12", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, BEHZ,             STANDARD}, },
    { MULT_COEF_PACKED, "13", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQ,        STANDARD}, },
    { MULT_COEF_PACKED, "14", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQ,        STANDARD}, },
    { MULT_COEF_PACKED, "15", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { MULT_COEF_PACKED, "16", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { MULT_COEF_PACKED, "17", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPS,              STANDARD}, },
    { MULT_COEF_PACKED, "18", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPS,              STANDARD}, },
    { MULT_COEF_PACKED, "19", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, BEHZ,             STANDARD}, },
    { MULT_COEF_PACKED, "20", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, BEHZ,             STANDARD}, },
    { MULT_COEF_PACKED, "21", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQ,        STANDARD}, },
    { MULT_COEF_PACKED, "22", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQ,        STANDARD}, },
    { MULT_COEF_PACKED, "23", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { MULT_COEF_PACKED, "24", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { MULT_COEF_PACKED, "25", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPS,              POVERQ},   },
    { MULT_COEF_PACKED, "26", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPS,              POVERQ},   },
    { MULT_COEF_PACKED, "27", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, BEHZ,             POVERQ},   },
    { MULT_COEF_PACKED, "28", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, BEHZ,             POVERQ},   },
    { MULT_COEF_PACKED, "29", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQ,        POVERQ},   },
    { MULT_COEF_PACKED, "30", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQ,        POVERQ},   },
    { MULT_COEF_PACKED, "31", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { MULT_COEF_PACKED, "32", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { MULT_COEF_PACKED, "33", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPS,              POVERQ},   },
    { MULT_COEF_PACKED, "34", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPS,              POVERQ},   },
    { MULT_COEF_PACKED, "35", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, BEHZ,             POVERQ},   },
    { MULT_COEF_PACKED, "36", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, BEHZ,             POVERQ},   },
    { MULT_COEF_PACKED, "37", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQ,        POVERQ},   },
    { MULT_COEF_PACKED, "38", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQ,        POVERQ},   },
    { MULT_COEF_PACKED, "39", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { MULT_COEF_PACKED, "40", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM,   DFLT,   DFLT,      1,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    // ==========================================
    // TestType,   Descr, Scheme,        RDim, MultDepth, SFBits, DSize,    BatchSz, SecKeyDist,       MaxRelinSkDeg, ModSize, SecLvl,       KSTech, ScalTech,        LDigits, PtMod,   StdDev, EvalAddCt, EvalMultCt, KSCt, MultTech,         EncTech
    { MULT_PACKED, "01", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_PACKED, "02", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_PACKED, "03", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_PACKED, "04", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_PACKED, "05", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_PACKED, "06", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_PACKED, "07", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_PACKED, "08", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { MULT_PACKED, "09", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              STANDARD}, },
    { MULT_PACKED, "10", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              STANDARD}, },
    { MULT_PACKED, "11", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             STANDARD}, },
    { MULT_PACKED, "12", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             STANDARD}, },
    { MULT_PACKED, "13", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        STANDARD}, },
    { MULT_PACKED, "14", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        STANDARD}, },
    { MULT_PACKED, "15", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { MULT_PACKED, "16", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { MULT_PACKED, "17", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              STANDARD}, },
    { MULT_PACKED, "18", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              STANDARD}, },
    { MULT_PACKED, "19", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             STANDARD}, },
    { MULT_PACKED, "20", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             STANDARD}, },
    { MULT_PACKED, "21", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        STANDARD}, },
    { MULT_PACKED, "22", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        STANDARD}, },
    { MULT_PACKED, "23", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { MULT_PACKED, "24", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
	{ MULT_PACKED, "25", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              POVERQ},   },
    { MULT_PACKED, "26", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              POVERQ},   },
    { MULT_PACKED, "27", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             POVERQ},   },
    { MULT_PACKED, "28", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             POVERQ},   },
    { MULT_PACKED, "29", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        POVERQ},   },
    { MULT_PACKED, "30", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        POVERQ},   },
    { MULT_PACKED, "31", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { MULT_PACKED, "32", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { MULT_PACKED, "33", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              POVERQ},   },
    { MULT_PACKED, "34", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              POVERQ},   },
    { MULT_PACKED, "35", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             POVERQ},   },
    { MULT_PACKED, "36", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             POVERQ},   },
    { MULT_PACKED, "37", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        POVERQ},   },
    { MULT_PACKED, "38", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        POVERQ},   },
    { MULT_PACKED, "39", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { MULT_PACKED, "40", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    // ==========================================
    // TestType,   Descr, Scheme,        RDim, MultDepth, SFBits, DSize,    BatchSz, SecKeyDist,       MaxRelinSkDeg, ModSize, SecLvl,       KSTech, ScalTech,        LDigits, PtMod,   StdDev, EvalAddCt, EvalMultCt, KSCt, MultTech,         EncTech
    { EVALATINDEX, "01", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALATINDEX, "02", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALATINDEX, "03", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALATINDEX, "04", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALATINDEX, "05", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALATINDEX, "06", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALATINDEX, "07", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALATINDEX, "08", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALATINDEX, "09", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPS,              STANDARD}, },
    { EVALATINDEX, "10", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPS,              STANDARD}, },
    { EVALATINDEX, "11", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    BEHZ,             STANDARD}, },
    { EVALATINDEX, "12", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    BEHZ,             STANDARD}, },
    { EVALATINDEX, "13", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQ,        STANDARD}, },
    { EVALATINDEX, "14", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQ,        STANDARD}, },
    { EVALATINDEX, "15", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQLEVELED, STANDARD}, },
    { EVALATINDEX, "16", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQLEVELED, STANDARD}, },
    { EVALATINDEX, "17", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPS,              STANDARD}, },
    { EVALATINDEX, "18", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPS,              STANDARD}, },
    { EVALATINDEX, "19", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    BEHZ,             STANDARD}, },
    { EVALATINDEX, "20", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    BEHZ,             STANDARD}, },
    { EVALATINDEX, "21", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQ,        STANDARD}, },
    { EVALATINDEX, "22", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQ,        STANDARD}, },
    { EVALATINDEX, "23", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQLEVELED, STANDARD}, },
    { EVALATINDEX, "24", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQLEVELED, STANDARD}, },
	{ EVALATINDEX, "25", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPS,              POVERQ},   },
    { EVALATINDEX, "26", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPS,              POVERQ},   },
    { EVALATINDEX, "27", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    BEHZ,             POVERQ},   },
    { EVALATINDEX, "28", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    BEHZ,             POVERQ},   },
    { EVALATINDEX, "29", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQ,        POVERQ},   },
    { EVALATINDEX, "30", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQ,        POVERQ},   },
    { EVALATINDEX, "31", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQLEVELED, POVERQ},   },
    { EVALATINDEX, "32", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         BV,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQLEVELED, POVERQ},   },
    { EVALATINDEX, "33", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPS,              POVERQ},   },
    { EVALATINDEX, "34", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPS,              POVERQ},   },
    { EVALATINDEX, "35", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    BEHZ,             POVERQ},   },
    { EVALATINDEX, "36", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    BEHZ,             POVERQ},   },
    { EVALATINDEX, "37", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQ,        POVERQ},   },
    { EVALATINDEX, "38", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQ,        POVERQ},   },
    { EVALATINDEX, "39", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQLEVELED, POVERQ},   },
    { EVALATINDEX, "40", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,     HYBRID,     DFLT,            DFLT,    PTM_LRG, DFLT,   DFLT,      0,          1,    HPSPOVERQLEVELED, POVERQ},   },
    // ==========================================
    // TestType,   Descr, Scheme,       RDim, MultDepth, SFBits, DSize,    BatchSz, SecKeyDist,       MaxRelinSkDeg, ModSize, SecLvl,       KSTech, ScalTech,        LDigits, PtMod,   StdDev, EvalAddCt, EvalMultCt, KSCt, MultTech          EncTech
    { EVALMERGE,  "01", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALMERGE,  "02", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALMERGE,  "03", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALMERGE,  "04", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALMERGE,  "05", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALMERGE,  "06", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALMERGE,  "07", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALMERGE,  "08", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { EVALMERGE,  "09", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              STANDARD}, },
    { EVALMERGE,  "10", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              STANDARD}, },
    { EVALMERGE,  "11", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             STANDARD}, },
    { EVALMERGE,  "12", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             STANDARD}, },
    { EVALMERGE,  "13", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        STANDARD}, },
    { EVALMERGE,  "14", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        STANDARD}, },
    { EVALMERGE,  "15", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { EVALMERGE,  "16", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { EVALMERGE,  "17", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              POVERQ},   },
    { EVALMERGE,  "18", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              POVERQ},   },
    { EVALMERGE,  "19", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             POVERQ},   },
    { EVALMERGE,  "20", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             POVERQ},   },
    { EVALMERGE,  "21", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        POVERQ},   },
    { EVALMERGE,  "22", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        POVERQ},   },
    { EVALMERGE,  "23", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { EVALMERGE,  "24", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    // ==========================================
    // TestType,   Descr, Scheme,       RDim, MultDepth, SFBits, DSize,    BatchSz, SecKeyDist,       MaxRelinSkDeg, ModSize, SecLvl,       KSTech, ScalTech,        LDigits, PtMod,   StdDev, EvalAddCt, EvalMultCt, KSCt, MultTech,         EncTech
    { EVALSUM,    "01", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              STANDARD}, },
    { EVALSUM,    "02", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              STANDARD}, },
    { EVALSUM,    "03", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             STANDARD}, },
    { EVALSUM,    "04", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             STANDARD}, },
    { EVALSUM,    "05", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        STANDARD}, },
    { EVALSUM,    "06", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        STANDARD}, },
    { EVALSUM,    "07", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { EVALSUM,    "08", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { EVALSUM,    "09", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              POVERQ},   },
    { EVALSUM,    "10", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              POVERQ},   },
    { EVALSUM,    "11", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             POVERQ},   },
    { EVALSUM,    "12", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             POVERQ},   },
    { EVALSUM,    "13", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        POVERQ},   },
    { EVALSUM,    "14", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        POVERQ},   },
    { EVALSUM,    "15", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { EVALSUM,    "16", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    // ==========================================
    // TestType,   Descr, Scheme,       RDim, MultDepth, SFBits, DSize,    BatchSz, SecKeyDist,       MaxRelinSkDeg, ModSize, SecLvl,       KSTech, ScalTech,        LDigits, PtMod,   StdDev, EvalAddCt, EvalMultCt, KSCt, MultTech,         EncTech
    { METADATA,   "01", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { METADATA,   "02", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { METADATA,   "03", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { METADATA,   "04", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   UNIFORM_TERNARY,  1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { METADATA,   "05", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { METADATA,   "06", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FIXEDAUTO,       DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { METADATA,   "07", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTO,    DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { METADATA,   "08", {BGVRNS_SCHEME, 256,  2,         59,     BV_DSIZE, BATCH,   GAUSSIAN,         1,             60,      HEStd_NotSet, BV,     FLEXIBLEAUTOEXT, DFLT,    PTM_LRG, DFLT,   DFLT,      DFLT,       DFLT, DFLT,             STANDARD}, },
    { METADATA,   "09", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              STANDARD}, },
    { METADATA,   "10", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              STANDARD}, },
    { METADATA,   "11", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             STANDARD}, },
    { METADATA,   "12", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             STANDARD}, },
    { METADATA,   "13", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        STANDARD}, },
    { METADATA,   "14", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        STANDARD}, },
    { METADATA,   "15", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { METADATA,   "16", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, STANDARD}, },
    { METADATA,   "17", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              POVERQ},   },
    { METADATA,   "18", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPS,              POVERQ},   },
    { METADATA,   "19", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             POVERQ},   },
    { METADATA,   "20", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, BEHZ,             POVERQ},   },
    { METADATA,   "21", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        POVERQ},   },
    { METADATA,   "22", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQ,        POVERQ},   },
    { METADATA,   "23", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   UNIFORM_TERNARY,  DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    { METADATA,   "24", {BFVRNS_SCHEME, DFLT, DFLT,      60,     20,       BATCH,   GAUSSIAN,         DFLT,          DFLT,    DFLT,         DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, HPSPOVERQLEVELED, POVERQ},   },
    // ==========================================
    // TestType,    Descr, Scheme,       RDim,      MultDepth, SFBits, DSize, BatchSz,   SecKeyDist, MaxRelinSkDeg, ModSize, SecLvl,  KSTech, ScalTech,        LDigits, PtMod,   StdDev, EvalAddCt, EvalMultCt, KSCt, MultTech, EncTech
    { EVALSUM_ALL, "01", {BFVRNS_SCHEME, BATCH_LRG, DFLT,      60,     20,    BATCH_LRG, DFLT,       DFLT,          DFLT,    DFLT,    DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, DFLT,     STANDARD},  },
    { EVALSUM_ALL, "02", {BFVRNS_SCHEME, BATCH_LRG, DFLT,      60,     20,    BATCH_LRG, DFLT,       DFLT,          DFLT,    DFLT,    DFLT,   FIXEDMANUAL,     DFLT,    PTM_LRG, DFLT,   DFLT,      2,          DFLT, DFLT,     POVERQ},    },
    // ==========================================
    // TestType,      Descr, Scheme,       RDim,      MultDepth, SFBits, DSize, BatchSz, SecKeyDist, MaxRelinSkDeg, ModSize, SecLvl,  KSTech, ScalTech,        LDigits, PtMod,   StdDev, EvalAddCt, EvalMultCt, KSCt, MultTech, EncTech
    { KS_SINGLE_CRT, "01", {BGVRNS_SCHEME, 1<<13,     1,         50,     1,     DFLT,    DFLT,       DFLT,          DFLT,    DFLT,    DFLT,   FIXEDMANUAL,     DFLT,    256,     4,      DFLT,      DFLT,       DFLT, DFLT,     STANDARD},  },
    { KS_SINGLE_CRT, "02", {BGVRNS_SCHEME, 1<<13,     1,         50,     1,     DFLT,    DFLT,       DFLT,          DFLT,    DFLT,    DFLT,   FIXEDAUTO,       DFLT,    256,     4,      DFLT,      DFLT,       DFLT, DFLT,     STANDARD},  },
    { KS_SINGLE_CRT, "03", {BGVRNS_SCHEME, 1<<13,     1,         50,     1,     DFLT,    DFLT,       DFLT,          DFLT,    DFLT,    DFLT,   FLEXIBLEAUTO,    DFLT,    256,     4,      DFLT,      DFLT,       DFLT, DFLT,     STANDARD},  },
    { KS_SINGLE_CRT, "04", {BGVRNS_SCHEME, 1<<13,     1,         50,     1,     DFLT,    DFLT,       DFLT,          DFLT,    DFLT,    DFLT,   FLEXIBLEAUTOEXT, DFLT,    256,     4,      DFLT,      DFLT,       DFLT, DFLT,     STANDARD},  },
    // ==========================================
    // TestType,           Descr, Scheme,       RDim,      MultDepth, SFBits, DSize, BatchSz, SecKeyDist, MaxRelinSkDeg, ModSize, SecLvl,  KSTech, ScalTech,        LDigits, PtMod,   StdDev, EvalAddCt, EvalMultCt, KSCt, MultTech, EncTech
    { KS_MOD_REDUCE_DCRT, "01", {BGVRNS_SCHEME, 1<<13,     1,         50,     1,     DFLT,    DFLT,       DFLT,          DFLT,    DFLT,    DFLT,   FIXEDMANUAL,     DFLT,    256,     4,      DFLT,      DFLT,       DFLT, DFLT,     STANDARD},  },
    // Calling ModReduce in the AUTO modes doesn't do anything because we automatically mod reduce before multiplication,
    // so we don't need unit tests for KS_MOD_REDUCE_DCRT in the AUTO modes.
 };
// clang-format on
//===========================================================================================================
class UTSHE : public ::testing::TestWithParam<TEST_CASE_UTSHE> {
    using Element = DCRTPoly;

protected:
    void SetUp() {}

    void TearDown() {
        CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
    }

    void UnitTest_Add_Packed(const TEST_CASE_UTSHE& testData, const std::string& failmsg = std::string()) {
        try {
            CryptoContext<Element> cc(UnitTestGenerateContext(testData.params));

            std::vector<int64_t> vectorOfInts1 = { 1, 0, 3, 1, 0, 1, 2, 1 };
            Plaintext plaintext1 = cc->MakeCoefPackedPlaintext(vectorOfInts1);

            std::vector<int64_t> vectorOfInts2 = { 2, 1, 3, 2, 2, 1, 3, 0 };
            Plaintext plaintext2 = cc->MakeCoefPackedPlaintext(vectorOfInts2);

            std::vector<int64_t> vectorOfIntsAdd = { 3, 1, 6, 3, 2, 2, 5, 1 };
            Plaintext plaintextAdd = cc->MakeCoefPackedPlaintext(vectorOfIntsAdd);

            std::vector<int64_t> vectorOfIntsSub = { -1, -1, 0, -1, -2, 0, -1, 1 };
            Plaintext plaintextSub = cc->MakeCoefPackedPlaintext(vectorOfIntsSub);

            KeyPair<Element> kp = cc->KeyGen();
            Ciphertext<Element> ciphertext1 = cc->Encrypt(kp.publicKey, plaintext1);
            Ciphertext<Element> ciphertext2 = cc->Encrypt(kp.publicKey, plaintext2);

            Ciphertext<Element> cResult;
            Plaintext results;

            cResult = cc->EvalAdd(ciphertext1, ciphertext2);
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(plaintextAdd->GetLength());
            EXPECT_EQ(plaintextAdd->GetCoefPackedValue(), results->GetCoefPackedValue())
                << failmsg << " EvalAdd fails";

            auto ct1_clone = ciphertext1->Clone();
            cc->EvalAddInPlace(ct1_clone, ciphertext2);
            cc->Decrypt(kp.secretKey, ct1_clone, &results);
            results->SetLength(plaintextAdd->GetLength());
            EXPECT_EQ(plaintextAdd->GetCoefPackedValue(), results->GetCoefPackedValue())
                << failmsg << " EvalAddInPlace fails";

            cResult = ciphertext1 + ciphertext2;
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(plaintextAdd->GetLength());
            EXPECT_EQ(plaintextAdd->GetCoefPackedValue(), results->GetCoefPackedValue())
                << failmsg << " operator+ fails";

            Ciphertext<Element> caddInplace = ciphertext1->Clone();
            caddInplace += ciphertext2;
            cc->Decrypt(kp.secretKey, caddInplace, &results);
            results->SetLength(plaintextAdd->GetLength());
            EXPECT_EQ(plaintextAdd->GetCoefPackedValue(), results->GetCoefPackedValue())
                << failmsg << " operator+= fails";

            cResult = cc->EvalSub(ciphertext1, ciphertext2);
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(plaintextSub->GetLength());
            EXPECT_EQ(plaintextSub->GetCoefPackedValue(), results->GetCoefPackedValue())
                << failmsg << " EvalSub fails";

            cResult = ciphertext1 - ciphertext2;
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(plaintextSub->GetLength());
            EXPECT_EQ(plaintextSub->GetCoefPackedValue(), results->GetCoefPackedValue())
                << failmsg << " operator- fails";

            Ciphertext<Element> csubInplace = ciphertext1->Clone();
            csubInplace -= ciphertext2;
            cc->Decrypt(kp.secretKey, csubInplace, &results);
            results->SetLength(plaintextSub->GetLength());
            EXPECT_EQ(plaintextSub->GetCoefPackedValue(), results->GetCoefPackedValue())
                << failmsg << " operator-= fails";

            cResult = cc->EvalAdd(ciphertext1, plaintext2);
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(plaintextAdd->GetLength());
            EXPECT_EQ(plaintextAdd->GetCoefPackedValue(), results->GetCoefPackedValue())
                << failmsg << " EvalAdd Ct and Pt fails";

            cResult = cc->EvalSub(ciphertext1, plaintext2);
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(plaintextSub->GetLength());
            EXPECT_EQ(plaintextSub->GetCoefPackedValue(), results->GetCoefPackedValue())
                << failmsg << " EvalSub Ct and Pt fails";
        }
        catch (std::exception& e) {
            std::cerr << "Exception thrown from " << __func__ << "(): " << e.what() << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
        catch (...) {
            std::string name(demangle(__cxxabiv1::__cxa_current_exception_type()->name()));
            std::cerr << "Unknown exception of type \"" << name << "\" thrown from " << __func__ << "()" << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
    }

    void UnitTest_Mult_CoefPacked(const TEST_CASE_UTSHE& testData, const std::string& failmsg = std::string()) {
        try {
            CryptoContext<Element> cc(UnitTestGenerateContext(testData.params));

            std::vector<int64_t> vectorOfInts1 = { 1, 0, 3, 1, 0, 1, 2, 1 };
            Plaintext plaintext1 = cc->MakeCoefPackedPlaintext(vectorOfInts1);

            std::vector<int64_t> vectorOfInts2 = { 2, 1, 3, 2, 2, 1, 3, 0 };
            Plaintext plaintext2 = cc->MakeCoefPackedPlaintext(vectorOfInts2);

            // For cyclotomic order != 16, the expected result is the convolution of
            // vectorOfInt21 and vectorOfInts2
            std::vector<int64_t> vectorOfIntsMultLong = { 2,  1,  9,  7, 12, 12, 16,
                                                         12, 19, 12, 7, 7,  7,  3 };
            std::vector<int64_t> vectorOfIntsMult = { -17, -11, 2, 0, 5, 9, 16, 12 };

            Plaintext intArray1 = cc->MakeCoefPackedPlaintext(vectorOfInts1);

            Plaintext intArray2 = cc->MakeCoefPackedPlaintext(vectorOfInts2);

            Plaintext intArrayExpected = cc->MakeCoefPackedPlaintext(
                cc->GetCyclotomicOrder() == 16 ? vectorOfIntsMult : vectorOfIntsMultLong);

            // Initialize the public key containers.
            KeyPair<Element> kp = cc->KeyGen();

            Ciphertext<Element> ciphertext1 = cc->Encrypt(kp.publicKey, intArray1);

            Ciphertext<Element> ciphertext2 = cc->Encrypt(kp.publicKey, intArray2);

            cc->EvalMultKeyGen(kp.secretKey);

            Ciphertext<Element> cResult;
            Plaintext results;

            cResult = cc->EvalMult(ciphertext1, ciphertext2);
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(intArrayExpected->GetLength());
            EXPECT_EQ(intArrayExpected->GetCoefPackedValue(),
                results->GetCoefPackedValue())
                << failmsg << " EvalMult fails";

            cResult = ciphertext1 * ciphertext2;
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(intArrayExpected->GetLength());
            EXPECT_EQ(intArrayExpected->GetCoefPackedValue(),
                results->GetCoefPackedValue())
                << failmsg << " operator* fails";

            Ciphertext<Element> cmulInplace = ciphertext1->Clone();
            cmulInplace *= ciphertext2;
            cc->Decrypt(kp.secretKey, cmulInplace, &results);
            results->SetLength(intArrayExpected->GetLength());
            EXPECT_EQ(intArrayExpected->GetCoefPackedValue(),
                results->GetCoefPackedValue())
                << failmsg << " operator*= fails";

            cResult = cc->EvalMult(ciphertext1, plaintext2);
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(intArrayExpected->GetLength());
            EXPECT_EQ(intArrayExpected->GetCoefPackedValue(),
                results->GetCoefPackedValue())
                << failmsg << " EvalMult Ct and Pt fails";
        }
        catch (std::exception& e) {
            std::cerr << "Exception thrown from " << __func__ << "(): " << e.what() << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
        catch (...) {
            std::string name(demangle(__cxxabiv1::__cxa_current_exception_type()->name()));
            std::cerr << "Unknown exception of type \"" << name << "\" thrown from " << __func__ << "()" << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
    }

    void UnitTest_Mult_Packed(const TEST_CASE_UTSHE& testData, const std::string& failmsg = std::string()) {
        try {
            CryptoContext<Element> cc(UnitTestGenerateContext(testData.params));

            std::vector<int64_t> vectorOfInts1 = { 1, 0, 3, 1, 0, 1, 2, 1 };
            Plaintext plaintext1 = cc->MakePackedPlaintext(vectorOfInts1);

            std::vector<int64_t> vectorOfInts2 = { 2, 1, 3, 2, 2, 1, 3, 1 };
            Plaintext plaintext2 = cc->MakePackedPlaintext(vectorOfInts2);

            // For cyclotomic order != 16, the expected result is the convolution of
            // vectorOfInt21 and vectorOfInts2
            std::vector<int64_t> vectorOfIntsMult = { 2, 0, 9, 2, 0, 1, 6, 1 };

            Plaintext intArray1 = cc->MakePackedPlaintext(vectorOfInts1);

            Plaintext intArray2 = cc->MakePackedPlaintext(vectorOfInts2);

            Plaintext intArrayExpected = cc->MakePackedPlaintext(vectorOfIntsMult);

            // Initialize the public key containers.
            KeyPair<Element> kp = cc->KeyGen();

            Ciphertext<Element> ciphertext1 = cc->Encrypt(kp.publicKey, intArray1);

            Ciphertext<Element> ciphertext2 = cc->Encrypt(kp.publicKey, intArray2);

            cc->EvalMultKeyGen(kp.secretKey);

            Ciphertext<Element> cResult;
            Plaintext results;

            cResult = cc->EvalMult(ciphertext1, ciphertext2);
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(intArrayExpected->GetLength());
            EXPECT_EQ(intArrayExpected->GetPackedValue(), results->GetPackedValue())
                << failmsg << " EvalMult fails";

            cResult = ciphertext1 * ciphertext2;
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(intArrayExpected->GetLength());
            EXPECT_EQ(intArrayExpected->GetPackedValue(), results->GetPackedValue())
                << failmsg << " operator* fails";

            Ciphertext<Element> cmulInplace = ciphertext1->Clone();
            cmulInplace *= ciphertext2;
            cc->Decrypt(kp.secretKey, cmulInplace, &results);
            results->SetLength(intArrayExpected->GetLength());
            EXPECT_EQ(intArrayExpected->GetPackedValue(), results->GetPackedValue())
                << failmsg << " operator*= fails";

            cResult = cc->EvalMult(ciphertext1, plaintext2);
            cc->Decrypt(kp.secretKey, cResult, &results);
            results->SetLength(intArrayExpected->GetLength());
            EXPECT_EQ(intArrayExpected->GetPackedValue(), results->GetPackedValue())
                << failmsg << " EvalMult Ct and Pt fails";
        }
        catch (std::exception& e) {
            std::cerr << "Exception thrown from " << __func__ << "(): " << e.what() << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
        catch (...) {
            std::string name(demangle(__cxxabiv1::__cxa_current_exception_type()->name()));
            std::cerr << "Unknown exception of type \"" << name << "\" thrown from " << __func__ << "()" << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
    }

    void UnitTest_EvalAtIndex(const TEST_CASE_UTSHE& testData, const std::string& failmsg = std::string()) {
        try {
            CryptoContext<Element> cc(UnitTestGenerateContext(testData.params));

            std::vector<int64_t> vectorOfInts1 = { 1, 2,  3,  4,  5,  6,  7,  8,
                                            9, 10, 11, 12, 13, 14, 15, 16 };
            Plaintext plaintext1 = cc->MakePackedPlaintext(vectorOfInts1);

            // Expected results after evaluating EvalAtIndex(3) and EvalAtIndex(-3)
            std::vector<int64_t> vectorOfIntsPlus3 = { 4,  5,  6,  7,  8,  9, 10, 11,
                                                      12, 13, 14, 15, 16, 0, 0,  0 };
            std::vector<int64_t> vectorOfIntsMinus3 = { 0, 0, 0, 1, 2,  3,  4,  5,
                                                       6, 7, 8, 9, 10, 11, 12, 13 };

            Plaintext intArray1 = cc->MakePackedPlaintext(vectorOfInts1);

            Plaintext intArrayPlus3 = cc->MakePackedPlaintext(vectorOfIntsPlus3);
            Plaintext intArrayMinus3 = cc->MakePackedPlaintext(vectorOfIntsMinus3);

            // Initialize the public key containers.
            KeyPair<Element> kp = cc->KeyGen();

            Ciphertext<Element> ciphertext1 = cc->Encrypt(kp.publicKey, intArray1);

            cc->EvalAtIndexKeyGen(kp.secretKey, { 3, -3 });

            Ciphertext<Element> cResult1 = cc->EvalAtIndex(ciphertext1, 3);

            Ciphertext<Element> cResult2 = cc->EvalAtIndex(ciphertext1, -3);

            Plaintext results1;

            Plaintext results2;

            cc->Decrypt(kp.secretKey, cResult1, &results1);

            cc->Decrypt(kp.secretKey, cResult2, &results2);

            results1->SetLength(intArrayPlus3->GetLength());
            EXPECT_EQ(intArrayPlus3->GetPackedValue(), results1->GetPackedValue())
                << failmsg << " EvalAtIndex(3) fails";

            results2->SetLength(intArrayMinus3->GetLength());
            EXPECT_EQ(intArrayMinus3->GetPackedValue(), results2->GetPackedValue())
                << failmsg << " EvalAtIndex(-3) fails";
        }
        catch (std::exception& e) {
            std::cerr << "Exception thrown from " << __func__ << "(): " << e.what() << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
        catch (...) {
            std::string name(demangle(__cxxabiv1::__cxa_current_exception_type()->name()));
            std::cerr << "Unknown exception of type \"" << name << "\" thrown from " << __func__ << "()" << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
    }

    void UnitTest_EvalMerge(const TEST_CASE_UTSHE& testData, const std::string& failmsg = std::string()) {
        try {
            CryptoContext<Element> cc(UnitTestGenerateContext(testData.params));

            // Initialize the public key containers.
            KeyPair<Element> kp = cc->KeyGen();

            std::vector<Ciphertext<Element>> ciphertexts;

            std::vector<int64_t> vectorOfInts1 = { 32, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            Plaintext intArray1 = cc->MakePackedPlaintext(vectorOfInts1);
            ciphertexts.push_back(cc->Encrypt(kp.publicKey, intArray1));

            std::vector<int64_t> vectorOfInts2 = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            Plaintext intArray2 = cc->MakePackedPlaintext(vectorOfInts2);
            ciphertexts.push_back(cc->Encrypt(kp.publicKey, intArray2));

            std::vector<int64_t> vectorOfInts3 = { 4, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            Plaintext intArray3 = cc->MakePackedPlaintext(vectorOfInts3);
            ciphertexts.push_back(cc->Encrypt(kp.publicKey, intArray3));

            std::vector<int64_t> vectorOfInts4 = { 8, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            Plaintext intArray4 = cc->MakePackedPlaintext(vectorOfInts4);
            ciphertexts.push_back(cc->Encrypt(kp.publicKey, intArray4));

            std::vector<int64_t> vectorOfInts5 = { 16, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            Plaintext intArray5 = cc->MakePackedPlaintext(vectorOfInts5);
            ciphertexts.push_back(cc->Encrypt(kp.publicKey, intArray5));

            // Expected results after evaluating EvalAtIndex(3) and EvalAtIndex(-3)
            std::vector<int64_t> vectorMerged = { 32, 2, 4, 8, 16, 0, 0, 0 };
            Plaintext intArrayMerged = cc->MakePackedPlaintext(vectorMerged);

            std::vector<int32_t> indexList = { -1, -2, -3, -4, -5 };

            cc->EvalAtIndexKeyGen(kp.secretKey, indexList);

            auto mergedCiphertext = cc->EvalMerge(ciphertexts);

            Plaintext results1;

            cc->Decrypt(kp.secretKey, mergedCiphertext, &results1);

            results1->SetLength(intArrayMerged->GetLength());
            EXPECT_EQ(intArrayMerged->GetPackedValue(), results1->GetPackedValue())
                << failmsg << " EvalMerge fails";
        }
        catch (std::exception& e) {
            std::cerr << "Exception thrown from " << __func__ << "(): " << e.what() << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
        catch (...) {
            std::string name(demangle(__cxxabiv1::__cxa_current_exception_type()->name()));
            std::cerr << "Unknown exception of type \"" << name << "\" thrown from " << __func__ << "()" << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
    }

    void UnitTest_EvalSum(const TEST_CASE_UTSHE& testData, const std::string& failmsg = std::string()) {
        try {
            CryptoContext<Element> cc(UnitTestGenerateContext(testData.params));

            // Initialize the public key containers.
            KeyPair<Element> kp = cc->KeyGen();

            std::vector<Ciphertext<Element>> ciphertexts;

            uint32_t n = cc->GetRingDimension();

            std::vector<int64_t> vectorOfInts1 = { 1, 2, 3, 4, 5, 6, 7, 8 };
            uint32_t dim = vectorOfInts1.size();
            vectorOfInts1.resize(n);
            for (uint32_t i = dim; i < n; i++) vectorOfInts1[i] = vectorOfInts1[i % dim];
            Plaintext intArray1 = cc->MakePackedPlaintext(vectorOfInts1);
            auto ct1 = cc->Encrypt(kp.publicKey, intArray1);

            cc->EvalSumKeyGen(kp.secretKey);

            auto ctsum1 = cc->EvalSum(ct1, 1);
            auto ctsum2 = cc->EvalSum(ct1, 2);
            auto ctsum3 = cc->EvalSum(ct1, 8);

            std::vector<int64_t> vectorOfInts2 = { 3, 5, 7, 9, 11, 13, 15, 9 };
            vectorOfInts2.resize(n);
            for (uint32_t i = dim; i < n; i++) vectorOfInts2[i] = vectorOfInts2[i % dim];
            Plaintext intArray2 = cc->MakePackedPlaintext(vectorOfInts2);

            std::vector<int64_t> vectorOfIntsAll = { 36, 36, 36, 36, 36, 36, 36, 36 };
            vectorOfIntsAll.resize(n);
            for (uint32_t i = dim; i < n; i++)
                vectorOfIntsAll[i] = vectorOfIntsAll[i % dim];
            Plaintext intArrayAll = cc->MakePackedPlaintext(vectorOfIntsAll);

            Plaintext results1;
            cc->Decrypt(kp.secretKey, ctsum1, &results1);
            Plaintext results2;
            cc->Decrypt(kp.secretKey, ctsum2, &results2);
            Plaintext results3;
            cc->Decrypt(kp.secretKey, ctsum3, &results3);

            intArray1->SetLength(dim);
            intArray2->SetLength(dim);
            intArrayAll->SetLength(dim);
            results1->SetLength(dim);
            results2->SetLength(dim);
            results3->SetLength(dim);

            EXPECT_EQ(intArray1->GetPackedValue(), results1->GetPackedValue())
                << failmsg << " EvalSum for batch size = 1 failed";
            EXPECT_EQ(intArray2->GetPackedValue(), results2->GetPackedValue())
                << failmsg << " EvalSum for batch size = 2 failed";
            EXPECT_EQ(intArrayAll->GetPackedValue(), results3->GetPackedValue())
                << failmsg << " EvalSum for batch size = 8 failed";
        }
        catch (std::exception& e) {
            std::cerr << "Exception thrown from " << __func__ << "(): " << e.what() << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
        catch (...) {
            std::string name(demangle(__cxxabiv1::__cxa_current_exception_type()->name()));
            std::cerr << "Unknown exception of type \"" << name << "\" thrown from " << __func__ << "()" << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
    }

    void UnitTest_Metadata(const TEST_CASE_UTSHE& testData, const std::string& failmsg = std::string()) {
        try {
            CryptoContext<Element> cc(UnitTestGenerateContext(testData.params));

            std::vector<int64_t> input1{ 0,1,2,3,4,5,6,7 };
            std::vector<int64_t> input2{ 0,-1,-2,-3,-4,-5,-6,-7 };
            Plaintext plaintext1 = cc->MakePackedPlaintext(input1);
            Plaintext plaintext2 = cc->MakePackedPlaintext(input2);

            // Generate encryption keys
            KeyPair<Element> kp = cc->KeyGen();
            // Generate multiplication keys
            cc->EvalMultKeyGen(kp.secretKey);
            // Generate rotation keys for offsets +2 (left rotate) and -2 (right rotate)
            cc->EvalAtIndexKeyGen(kp.secretKey, { 2, -2 });
            // Generate keys for EvalSum
            cc->EvalSumKeyGen(kp.secretKey);

            // Encrypt plaintexts
            Ciphertext<Element> ciphertext1 = cc->Encrypt(kp.publicKey, plaintext1);
            Ciphertext<Element> ciphertext2 = cc->Encrypt(kp.publicKey, plaintext2);
            Plaintext results;

            // Populating metadata map in ciphertexts
            auto val1 = std::make_shared<MetadataTest>();
            val1->SetMetadata("ciphertext1");
            MetadataTest::StoreMetadata<Element>(ciphertext1, val1);
            auto val2 = std::make_shared<MetadataTest>();
            val2->SetMetadata("ciphertext2");
            MetadataTest::StoreMetadata<Element>(ciphertext2, val2);

            // Checking if metadata is carried over in EvalAdd(ctx,ctx)
            Ciphertext<Element> cAddCC = cc->EvalAdd(ciphertext1, ciphertext2);
            auto addCCValTest = MetadataTest::GetMetadata<Element>(cAddCC);
            EXPECT_EQ(val1->GetMetadata(), addCCValTest->GetMetadata())
                << "Ciphertext metadata mismatch in EvalAdd(ctx,ctx)";

            // Checking if metadata is carried over in EvalAddInPlace(ctx,ctx)
            Ciphertext<Element> ciphertext1_clone = ciphertext1->Clone();
            cc->EvalAddInPlace(ciphertext1_clone, ciphertext2);
            auto addCCInPlaceValTest = MetadataTest::GetMetadata<Element>(ciphertext1_clone);
            EXPECT_EQ(val1->GetMetadata(), addCCInPlaceValTest->GetMetadata())
                << "Ciphertext metadata mismatch in EvalAddInPlace(ctx,ctx)";

            // Checking if metadata is carried over in EvalAdd(ctx,ptx)
            Ciphertext<Element> cAddCP = cc->EvalAdd(ciphertext1, plaintext1);
            auto addCPValTest = MetadataTest::GetMetadata<Element>(cAddCP);
            EXPECT_EQ(val1->GetMetadata(), addCPValTest->GetMetadata())
                << "Ciphertext metadata mismatch in EvalAdd(ctx,ptx)";

            // Checking if metadata is carried over in EvalSub(ctx,ctx)
            Ciphertext<Element> cSubCC = cc->EvalSub(ciphertext1, ciphertext2);
            auto subCCValTest = MetadataTest::GetMetadata<Element>(cSubCC);
            EXPECT_EQ(val1->GetMetadata(), subCCValTest->GetMetadata())
                << "Ciphertext metadata mismatch in EvalSub(ctx,ctx)";

            // Checking if metadata is carried over in EvalSub(ctx,ptx)
            Ciphertext<Element> cSubCP = cc->EvalSub(ciphertext1, plaintext1);
            auto subCPValTest = MetadataTest::GetMetadata<Element>(cSubCP);
            EXPECT_EQ(val1->GetMetadata(), subCPValTest->GetMetadata())
                << "Ciphertext metadata mismatch in EvalSub(ctx,ptx)";

            // Checking if metadata is carried over in EvalMult(ctx,ctx)
            Ciphertext<Element> cMultCC = cc->EvalMult(ciphertext1, ciphertext2);
            auto multCCValTest = MetadataTest::GetMetadata<Element>(cMultCC);
            EXPECT_EQ(val1->GetMetadata(), multCCValTest->GetMetadata())
                << "Ciphertext metadata mismatch in EvalMult(ctx,ctx)";

            // Checking if metadata is carried over in EvalMult(ctx,ptx)
            Ciphertext<Element> cMultCP = cc->EvalMult(ciphertext1, plaintext1);
            auto multCPValTest = MetadataTest::GetMetadata<Element>(cMultCP);
            EXPECT_EQ(val1->GetMetadata(), multCPValTest->GetMetadata())
                << "Ciphertext metadata mismatch in EvalMult(ctx,ptx)";

            // Checking if metadata is carried over in EvalAtIndex +2 (left rotate)
            auto cAtIndex2 = cc->EvalAtIndex(ciphertext1, 2);
            auto atIndex2ValTest = MetadataTest::GetMetadata<Element>(cAtIndex2);
            EXPECT_EQ(val1->GetMetadata(), atIndex2ValTest->GetMetadata())
                << "Ciphertext metadata mismatch in EvalAtIndex +2";

            // Checking if metadata is carried over in EvalAtIndex -2 (right rotate)
            auto cAtIndexMinus2 = cc->EvalAtIndex(ciphertext1, -2);
            auto atIndexMinus2ValTest =
                MetadataTest::GetMetadata<Element>(cAtIndexMinus2);
            EXPECT_EQ(val1->GetMetadata(), atIndexMinus2ValTest->GetMetadata())
                << "Ciphertext metadata mismatch in EvalAtIndex -2";

            std::vector<double> weights(2);
            for (usint i = 0; i < 2; i++) weights[i] = i;

            std::vector<Ciphertext<Element>> ciphertexts(2);
            ciphertexts[0] = ciphertext1;
            ciphertexts[1] = ciphertext2;
        }
        catch (std::exception& e) {
            std::cerr << "Exception thrown from " << __func__ << "(): " << e.what() << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
        catch (...) {
            std::string name(demangle(__cxxabiv1::__cxa_current_exception_type()->name()));
            std::cerr << "Unknown exception of type \"" << name << "\" thrown from " << __func__ << "()" << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
    }

    void UnitTest_EvalSum_BFVrns_All(const TEST_CASE_UTSHE& testData, const std::string& failmsg = std::string()) {
        try {
            CryptoContext<Element> cc(UnitTestGenerateContext(testData.params));

            // Initialize the public key containers.
            KeyPair<DCRTPoly> kp = cc->KeyGen();

            std::vector<Ciphertext<DCRTPoly>> ciphertexts;

            uint32_t n = cc->GetRingDimension();

            std::vector<int64_t> vectorOfInts1 = { 1, 2, 3, 4, 5, 6, 7, 8 };
            uint32_t dim = vectorOfInts1.size();
            vectorOfInts1.resize(n);
            for (uint32_t i = n - dim; i < n; i++) vectorOfInts1[i] = i;

            Plaintext intArray1 = cc->MakePackedPlaintext(vectorOfInts1);

            std::vector<int64_t> vectorOfIntsAll = { 32768, 32768, 32768, 32768,
                                                    32768, 32768, 32768, 32768 };
            Plaintext intArrayAll = cc->MakePackedPlaintext(vectorOfIntsAll);

            auto ct1 = cc->Encrypt(kp.publicKey, intArray1);

            cc->EvalSumKeyGen(kp.secretKey);

            auto ctsum1 = cc->EvalSum(ct1, BATCH_LRG);

            Plaintext results1;
            cc->Decrypt(kp.secretKey, ctsum1, &results1);

            intArrayAll->SetLength(dim);
            results1->SetLength(dim);

            EXPECT_EQ(intArrayAll->GetPackedValue(), results1->GetPackedValue())
                << " BFVrns EvalSum for batch size = All failed";
        }
        catch (std::exception& e) {
            std::cerr << "Exception thrown from " << __func__ << "(): " << e.what() << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
        catch (...) {
            std::string name(demangle(__cxxabiv1::__cxa_current_exception_type()->name()));
            std::cerr << "Unknown exception of type \"" << name << "\" thrown from " << __func__ << "()" << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
    }

    void UnitTest_Keyswitch_SingleCRT(const TEST_CASE_UTSHE& testData, const std::string& failmsg = std::string()) {
        try {
            CryptoContext<Element> cc(UnitTestGenerateContext(testData.params));

            Plaintext plaintext = cc->MakeStringPlaintext("I am good, what are you?! 32 ch");
            KeyPair<DCRTPoly> kp = cc->KeyGen();

            Ciphertext<DCRTPoly> ciphertext = cc->Encrypt(kp.publicKey, plaintext);

            KeyPair<DCRTPoly> kp2 = cc->KeyGen();
            EvalKey<DCRTPoly> keySwitchHint = cc->KeySwitchGen(kp.secretKey, kp2.secretKey);

            Ciphertext<DCRTPoly> newCt = cc->KeySwitch(ciphertext, keySwitchHint);

            Plaintext plaintextNew;

            cc->Decrypt(kp2.secretKey, newCt, &plaintextNew);

            EXPECT_EQ(plaintext->GetStringValue(), plaintextNew->GetStringValue()) << "Key-Switched Decrypt fails";
        }
        catch (std::exception& e) {
            std::cerr << "Exception thrown from " << __func__ << "(): " << e.what() << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
        catch (...) {
            std::string name(demangle(__cxxabiv1::__cxa_current_exception_type()->name()));
            std::cerr << "Unknown exception of type \"" << name << "\" thrown from " << __func__ << "()" << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
    }

    void UnitTest_Keyswitch_ModReduce_DCRT(const TEST_CASE_UTSHE& testData, const std::string& failmsg = std::string()) {
        try {
            CryptoContext<Element> cc(UnitTestGenerateContext(testData.params));

            Plaintext plaintext = cc->MakeStringPlaintext("I am good, what are you?! 32 ch");

            KeyPair<DCRTPoly> kp = cc->KeyGen();
            Ciphertext<DCRTPoly> ciphertext = cc->Encrypt(kp.publicKey, plaintext);

            KeyPair<DCRTPoly> kp2 = cc->KeyGen();
            EvalKey<DCRTPoly> keySwitchHint = cc->KeySwitchGen(kp.secretKey, kp2.secretKey);

            Ciphertext<DCRTPoly> newCt = cc->KeySwitch(ciphertext, keySwitchHint);

            Plaintext plaintextNewKeySwitch;

            cc->Decrypt(kp2.secretKey, newCt, &plaintextNewKeySwitch);

            EXPECT_EQ(plaintext->GetStringValue(),
                plaintextNewKeySwitch->GetStringValue())
                << "Key-Switched Decrypt fails";

            /**************************KEYSWITCH TEST END******************************/
            /**************************MODREDUCE TEST BEGIN******************************/

            cc->ModReduceInPlace(newCt);
            DCRTPoly sk2PrivateElement(kp2.secretKey->GetPrivateElement());
            sk2PrivateElement.DropLastElement();
            kp2.secretKey->SetPrivateElement(sk2PrivateElement);

            Plaintext plaintextNewModReduce;

            cc->Decrypt(kp2.secretKey, newCt, &plaintextNewModReduce);

            EXPECT_EQ(plaintext->GetStringValue(),
                plaintextNewModReduce->GetStringValue())
                << "Mod Reduced Decrypt fails";

        }
        catch (std::exception& e) {
            std::cerr << "Exception thrown from " << __func__ << "(): " << e.what() << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
        catch (...) {
            std::string name(demangle(__cxxabiv1::__cxa_current_exception_type()->name()));
            std::cerr << "Unknown exception of type \"" << name << "\" thrown from " << __func__ << "()" << std::endl;
            // make it fail
            EXPECT_TRUE(0 == 1) << failmsg;
        }
    }
};
//===========================================================================================================
TEST_P(UTSHE, SHE) {
    setupSignals();
    auto test = GetParam();

    switch (test.testCaseType) {
    case ADD_PACKED:
        UnitTest_Add_Packed(test, test.buildTestName());
        break;
    case MULT_COEF_PACKED:
        UnitTest_Mult_CoefPacked(test, test.buildTestName());
        break;
    case MULT_PACKED:
        UnitTest_Mult_Packed(test, test.buildTestName());
        break;
    case EVALATINDEX:
        UnitTest_EvalAtIndex(test, test.buildTestName());
        break;
    case EVALMERGE:
        UnitTest_EvalMerge(test, test.buildTestName());
        break;
    case EVALSUM:
        UnitTest_EvalSum(test, test.buildTestName());
        break;
    case METADATA:
        UnitTest_Metadata(test, test.buildTestName());
        break;
    case EVALSUM_ALL:
        UnitTest_EvalSum_BFVrns_All(test, test.buildTestName());
        break;
    case KS_SINGLE_CRT:
        UnitTest_Keyswitch_SingleCRT(test, test.buildTestName());
        break;
    case KS_MOD_REDUCE_DCRT:
        UnitTest_Keyswitch_ModReduce_DCRT(test, test.buildTestName());
        break;
    default:
        break;
    }
}

INSTANTIATE_TEST_SUITE_P(UnitTests, UTSHE, ::testing::ValuesIn(testCases), testName);
