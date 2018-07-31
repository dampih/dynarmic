/* This file is part of the dynarmic project.
 * Copyright (c) 2018 MerryMage
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#include <catch.hpp>

#include <dynarmic/A64/exclusive_monitor.h>

#include "testenv.h"

TEST_CASE("A64: ADD", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x8b020020; // ADD X0, X1, X2
    env.code_mem[1] = 0x14000000; // B .

    jit.SetRegister(0, 0);
    jit.SetRegister(1, 1);
    jit.SetRegister(2, 2);
    jit.SetPC(0);

    env.ticks_left = 2;
    jit.Run();

    REQUIRE(jit.GetRegister(0) == 3);
    REQUIRE(jit.GetRegister(1) == 1);
    REQUIRE(jit.GetRegister(2) == 2);
    REQUIRE(jit.GetPC() == 4);
}

TEST_CASE("A64: REV", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0xdac00c00; // REV X0, X0
    env.code_mem[1] = 0x5ac00821; // REV W1, W1
    env.code_mem[2] = 0x14000000; // B .

    jit.SetRegister(0, 0xaabbccddeeff1100);
    jit.SetRegister(1, 0xaabbccdd);
    jit.SetPC(0);

    env.ticks_left = 3;
    jit.Run();

    REQUIRE(jit.GetRegister(0) == 0x11ffeeddccbbaa);
    REQUIRE(jit.GetRegister(1) == 0xddccbbaa);
    REQUIRE(jit.GetPC() == 8);
}

TEST_CASE("A64: REV32", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0xdac00800; // REV32 X0, X0
    env.code_mem[1] = 0x14000000; // B .

    jit.SetRegister(0, 0xaabbccddeeff1100);
    jit.SetPC(0);

    env.ticks_left = 2;
    jit.Run();
    REQUIRE(jit.GetRegister(0) == 0xddccbbaa0011ffee);
    REQUIRE(jit.GetPC() == 4);
}

TEST_CASE("A64: REV16", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0xdac00400; // REV16 X0, X0
    env.code_mem[1] = 0x5ac00421; // REV16 W1, W1
    env.code_mem[2] = 0x14000000; // B .

    jit.SetRegister(0, 0xaabbccddeeff1100);
    jit.SetRegister(1, 0xaabbccdd);

    jit.SetPC(0);

    env.ticks_left = 3;
    jit.Run();
    REQUIRE(jit.GetRegister(0) == 0xbbaaddccffee0011);
    REQUIRE(jit.GetRegister(1) == 0xbbaaddcc);
    REQUIRE(jit.GetPC() == 8);
}

TEST_CASE("A64: AND", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x8a020020; // AND X0, X1, X2
    env.code_mem[1] = 0x14000000; // B .

    jit.SetRegister(0, 0);
    jit.SetRegister(1, 1);
    jit.SetRegister(2, 3);
    jit.SetPC(0);

    env.ticks_left = 2;
    jit.Run();

    REQUIRE(jit.GetRegister(0) == 1);
    REQUIRE(jit.GetRegister(1) == 1);
    REQUIRE(jit.GetRegister(2) == 3);
    REQUIRE(jit.GetPC() == 4);
}

TEST_CASE("A64: Bitmasks", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x3200c3e0; // ORR W0, WZR, #0x01010101
    env.code_mem[1] = 0x320c8fe1; // ORR W1, WZR, #0x00F000F0
    env.code_mem[2] = 0x320003e2; // ORR W2, WZR, #1
    env.code_mem[3] = 0x14000000; // B .

    jit.SetPC(0);

    env.ticks_left = 4;
    jit.Run();

    REQUIRE(jit.GetRegister(0) == 0x01010101);
    REQUIRE(jit.GetRegister(1) == 0x00F000F0);
    REQUIRE(jit.GetRegister(2) == 1);
    REQUIRE(jit.GetPC() == 12);
}

TEST_CASE("A64: ANDS NZCV", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x6a020020; // ANDS W0, W1, W2
    env.code_mem[1] = 0x14000000; // B .

    SECTION("N=1, Z=0") {
        jit.SetRegister(0, 0);
        jit.SetRegister(1, 0xFFFFFFFF);
        jit.SetRegister(2, 0xFFFFFFFF);
        jit.SetPC(0);

        env.ticks_left = 2;
        jit.Run();

        REQUIRE(jit.GetRegister(0) == 0xFFFFFFFF);
        REQUIRE(jit.GetRegister(1) == 0xFFFFFFFF);
        REQUIRE(jit.GetRegister(2) == 0xFFFFFFFF);
        REQUIRE(jit.GetPC() == 4);
        REQUIRE((jit.GetPstate() & 0xF0000000) == 0x80000000);
    }

    SECTION("N=0, Z=1") {
        jit.SetRegister(0, 0);
        jit.SetRegister(1, 0xFFFFFFFF);
        jit.SetRegister(2, 0x00000000);
        jit.SetPC(0);

        env.ticks_left = 2;
        jit.Run();

        REQUIRE(jit.GetRegister(0) == 0x00000000);
        REQUIRE(jit.GetRegister(1) == 0xFFFFFFFF);
        REQUIRE(jit.GetRegister(2) == 0x00000000);
        REQUIRE(jit.GetPC() == 4);
        REQUIRE((jit.GetPstate() & 0xF0000000) == 0x40000000);
    }
    SECTION("N=0, Z=0") {
        jit.SetRegister(0, 0);
        jit.SetRegister(1, 0x12345678);
        jit.SetRegister(2, 0x7324a993);
        jit.SetPC(0);

        env.ticks_left = 2;
        jit.Run();

        REQUIRE(jit.GetRegister(0) == 0x12240010);
        REQUIRE(jit.GetRegister(1) == 0x12345678);
        REQUIRE(jit.GetRegister(2) == 0x7324a993);
        REQUIRE(jit.GetPC() == 4);
        REQUIRE((jit.GetPstate() & 0xF0000000) == 0x00000000);
    }
}

TEST_CASE("A64: CBZ", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x34000060; // CBZ X0, label
    env.code_mem[1] = 0x320003e2; // MOV X2, 1
    env.code_mem[2] = 0x14000000; // B.
    env.code_mem[3] = 0x321f03e2; // label: MOV X2, 2
    env.code_mem[4] = 0x14000000; // B .

    SECTION("no branch") {
        jit.SetPC(0);
        jit.SetRegister(0, 1);

        env.ticks_left = 4;
        jit.Run();

        REQUIRE(jit.GetRegister(2) == 1);
        REQUIRE(jit.GetPC() == 8);
    }

    SECTION("branch") {
        jit.SetPC(0);
        jit.SetRegister(0, 0);

        env.ticks_left = 4;
        jit.Run();

        REQUIRE(jit.GetRegister(2) == 2);
        REQUIRE(jit.GetPC() == 16);
    }
}

TEST_CASE("A64: TBZ", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x36180060; // TBZ X0, 3, label
    env.code_mem[1] = 0x320003e2; // MOV X2, 1
    env.code_mem[2] = 0x14000000; // B .
    env.code_mem[3] = 0x321f03e2; // label: MOV X2, 2
    env.code_mem[4] = 0x14000000; // B .

    SECTION("no branch") {
        jit.SetPC(0);
        jit.SetRegister(0, 0xFF);

        env.ticks_left = 4;
        jit.Run();

        REQUIRE(jit.GetRegister(2) == 1);
        REQUIRE(jit.GetPC() == 8);
    }

    SECTION("branch with zero") {
        jit.SetPC(0);
        jit.SetRegister(0, 0);

        env.ticks_left = 4;
        jit.Run();

        REQUIRE(jit.GetRegister(2) == 2);
        REQUIRE(jit.GetPC() == 16);
    }

    SECTION("branch with non-zero") {
        jit.SetPC(0);
        jit.SetRegister(0, 1);

        env.ticks_left = 4;
        jit.Run();

        REQUIRE(jit.GetRegister(2) == 2);
        REQUIRE(jit.GetPC() == 16);
    }
}

TEST_CASE("A64: FABD", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x6eb5d556; // FABD.4S V22, V10, V21
    env.code_mem[1] = 0x14000000; // B .

    jit.SetPC(0);
    jit.SetVector(10, {0xb4858ac77ff39a87, 0x9fce5e14c4873176});
    jit.SetVector(21, {0x56d3f085ff890e2b, 0x6e4b0a41801a2d00});

    env.ticks_left = 2;
    jit.Run();

    REQUIRE(jit.GetVector(22) == Vector{0x56d3f0857fc90e2b, 0x6e4b0a4144873176});
}

TEST_CASE("A64: 128-bit exclusive read/write", "[a64]") {
    TestEnv env;
    Dynarmic::A64::ExclusiveMonitor monitor{1};

    Dynarmic::A64::UserConfig conf;
    conf.callbacks = &env;
    conf.processor_id = 0;

    SECTION("Local Monitor Only") {
        conf.global_monitor = nullptr;
    }
    SECTION("Global Monitor") {
        conf.global_monitor = &monitor;
    }

    Dynarmic::A64::Jit jit{conf};

    env.code_mem[0] = 0xc87f0861; // LDXP X1, X2, [X3]
    env.code_mem[1] = 0xc8241865; // STXP W4, X5, X6, [X3]
    env.code_mem[2] = 0x14000000; // B .

    jit.SetPC(0);
    jit.SetRegister(3, 0x1234567812345678);
    jit.SetRegister(4, 0xbaadbaadbaadbaad);
    jit.SetRegister(5, 0xaf00d1e5badcafe0);
    jit.SetRegister(6, 0xd0d0cacad0d0caca);

    env.ticks_left = 3;
    jit.Run();

    REQUIRE(jit.GetRegister(1) == 0x7f7e7d7c7b7a7978);
    REQUIRE(jit.GetRegister(2) == 0x8786858483828180);
    REQUIRE(jit.GetRegister(4) == 0);
    REQUIRE(env.MemoryRead64(0x1234567812345678) == 0xaf00d1e5badcafe0);
    REQUIRE(env.MemoryRead64(0x1234567812345680) == 0xd0d0cacad0d0caca);
}

TEST_CASE("A64: CNTPCT_EL0", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0xd53be021; // MRS X1, CNTPCT_EL0
    env.code_mem[1] = 0xd503201f; // NOP
    env.code_mem[2] = 0xd503201f; // NOP
    env.code_mem[3] = 0xd503201f; // NOP
    env.code_mem[4] = 0xd503201f; // NOP
    env.code_mem[5] = 0xd503201f; // NOP
    env.code_mem[6] = 0xd503201f; // NOP
    env.code_mem[7] = 0xd53be022; // MRS X2, CNTPCT_EL0
    env.code_mem[8] = 0xcb010043; // SUB X3, X2, X1
    env.code_mem[9] = 0x14000000; // B .

    env.ticks_left = 10;
    jit.Run();

    REQUIRE(jit.GetRegister(3) == 7);
}

TEST_CASE("A64: FNMSUB 1", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x1f618a9c; // FNMSUB D28, D20, D1, D2
    env.code_mem[1] = 0x14000000; // B .

    jit.SetPC(0);
    jit.SetVector(20, {0xe73a51346164bd6c, 0x8080000000002b94});
    jit.SetVector(1, {0xbf8000007fffffff, 0xffffffff00002b94});
    jit.SetVector(2, {0x0000000000000000, 0xc79b271e3f000000});

    env.ticks_left = 2;
    jit.Run();

    REQUIRE(jit.GetVector(28) == Vector{0x66ca513533ee6076, 0x0000000000000000});
}

TEST_CASE("A64: FNMSUB 2", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x1f2ab88e; // FNMSUB S14, S4, S10, S14
    env.code_mem[1] = 0x14000000; // B .

    jit.SetPC(0);
    jit.SetVector(4, {0x3c9623b101398437, 0x7ff0abcd0ba98d27});
    jit.SetVector(10, {0xffbfffff3eaaaaab, 0x3f0000003f8147ae});
    jit.SetVector(14, {0x80000000007fffff, 0xe73a513400000000});
    jit.SetFpcr(0x00400000);

    env.ticks_left = 2;
    jit.Run();

    REQUIRE(jit.GetVector(14) == Vector{0x0000000080045284, 0x0000000000000000});
}

TEST_CASE("A64: FMADD", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x1f5e0e4a; // FMADD D10, D18, D30, D3
    env.code_mem[1] = 0x14000000; // B .

    jit.SetPC(0);
    jit.SetVector(18, {0x8000007600800000, 0x7ff812347f800000});
    jit.SetVector(30, {0xff984a3700000000, 0xe73a513480800000});
    jit.SetVector(3, {0x3f000000ff7fffff, 0x8139843780000000});
    jit.SetFpcr(0x00400000);

    env.ticks_left = 2;
    jit.Run();

    REQUIRE(jit.GetVector(10) == Vector{0x3f059921bf0dbfff, 0x0000000000000000});
}

TEST_CASE("A64: FMLA.4S (denormal)", "[a64]") {
    TestEnv env;
    Dynarmic::A64::Jit jit{Dynarmic::A64::UserConfig{&env}};

    env.code_mem[0] = 0x4e2fcccc; // FMLA.4S V12, V6, V15
    env.code_mem[1] = 0x14000000; // B .

    jit.SetPC(0);
    jit.SetVector(12, {0x3c9623b17ff80000, 0xbff0000080000076});
    jit.SetVector(6, {0x7ff80000ff800000, 0x09503366c1200000});
    jit.SetVector(15, {0x3ff0000080636d24, 0xbf800000e73a5134});
    jit.SetFpcr(0x01000000);

    env.ticks_left = 2;
    jit.Run();

    REQUIRE(jit.GetVector(12) == Vector{0x7ff800007fc00000, 0xbff0000068e8e581});
}
