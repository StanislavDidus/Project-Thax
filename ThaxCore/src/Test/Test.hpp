#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <array>

#include "../../include/DMG.hpp"
#include "DMGInitState.hpp"

namespace dmg
{
    using json = nlohmann::json;

    static constexpr std::array<uint8_t, 12> non_existent_opcodes =
    {
        0xD3, 0xE3, 0xE4, 0xF4, 0xDB, 0xEB, 0xEC, 0xFC, 0xDD, 0xED, 0xFD, 0xCB // CB is temporary
    };

    inline bool CompareDMGStates(const DMG::DMGInfo& current, const DMGInitState& expected)
    {
        CPU::CPUInfo cpu_info = current.cpu_info;
        bool all_ok = true;

        auto check = [&](const char* name, auto actual, auto exp)
        {
            if (actual != exp)
            {
                std::cout << std::hex << std::uppercase
                           << name << " mismatch: expected 0x" << +exp
                           << ", got 0x" << +actual << std::dec << "\n";
                all_ok = false;
            }
        };

        check("PC", cpu_info.PC, expected.PC);
        check("SP", cpu_info.SP, expected.SP);
        check("A",  cpu_info.A,  expected.A);
        check("B",  cpu_info.B,  expected.B);
        check("C",  cpu_info.C,  expected.C);
        check("D",  cpu_info.D,  expected.D);
        check("E",  cpu_info.E,  expected.E);
        check("F",  cpu_info.F,  expected.F);
        check("H",  cpu_info.H,  expected.H);
        check("L",  cpu_info.L,  expected.L);

        MemoryBus::MemoryInfo memory_info = current.memory_info;
        for (const auto& mapping : expected.memory_mappings)
        {
            uint8_t actual_byte = memory_info.data[mapping.first];
            if (actual_byte != mapping.second)
            {
                std::cout << std::hex << std::uppercase
                           << "Memory[0x" << mapping.first << "] mismatch: expected 0x"
                           << +mapping.second << ", got 0x" << +actual_byte
                           << std::dec << "\n";
                all_ok = false;
            }
        }

        return all_ok;
    }

    inline DMGInitState ReadInfo(const json& data, std::string_view name)
    {
        DMGInitState dmg_init_state{};

        std::string test_name = data["name"];
        dmg_init_state.PC = data[name]["pc"];
        dmg_init_state.SP = data[name]["sp"];
        dmg_init_state.A = data[name]["a"];
        dmg_init_state.B = data[name]["b"];
        dmg_init_state.C = data[name]["c"];
        dmg_init_state.D = data[name]["d"];
        dmg_init_state.E = data[name]["e"];
        dmg_init_state.F = data[name]["f"];
        dmg_init_state.H = data[name]["h"];
        dmg_init_state.L = data[name]["l"];

        json ram = data[name]["ram"];
        for (size_t i = 0; i < ram.size(); ++i)
        {
            MemoryMapping memory_mapping{};
            memory_mapping.first = ram[i][0];
            memory_mapping.second = ram[i][1];
            dmg_init_state.memory_mappings.push_back(memory_mapping);
        }

        return dmg_init_state;
    }

    inline size_t ReadTCyclesNumber(const json& data)
    {
        return data["cycles"].size() * 4;
    }

    inline void LoadTests()
    {
        for (int i = 0; i < 256; ++i)
        {
            if (std::ranges::find(non_existent_opcodes, i) != non_existent_opcodes.end()) continue;

            std::string file_name = std::format("SSTs/{:02x}.json", i);
            std::ifstream f{file_name};
            json data = json::parse(f)[0];

            std::string name = data["name"];
            DMGInitState initial = ReadInfo(data, "initial");
            DMGInitState final = ReadInfo(data, "final");

            // Load State to DMG
            DMG dmg{initial};

            // Run DMG
            dmg.SetState(DMG::State::PAUSE);
            size_t tcycles = ReadTCyclesNumber(data);
            dmg.StepCycles(tcycles);

            DMG::DMGInfo info = dmg.GetInfo();
            if (CompareDMGStates(info, final))
            {
                //std::cout << std::format("Test {}: passed.", name) << std::endl;
            }
            else
            {
                std::cout << std::format("Test {}: did not pass.", name) << std::endl;
            }
        }
    }
}
