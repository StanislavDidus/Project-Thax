#include "DMG/DMG.hpp"
#include "DMG/Info.hpp"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <array>
#include <format>
#include <string>

#include "DMG/Utils/Log.hpp"

namespace dmg
{
    using json = nlohmann::json;

    static constexpr std::array<uint8_t, 12> non_existent_opcodes =
    {
        0xD3, 0xE3, 0xE4, 0xF4, 0xDB, 0xEB, 0xEC, 0xFC, 0xDD, 0xED, 0xFD, 0xCB // CB is temporary
    };

    inline bool CompareDMGStates(dmg::DMG& dmg, const json& data)
    {
        DMGInfo dmg_info = dmg.GetDMGInfo();
        bool all_ok = true;

        auto check = [&](const char* name, uint16_t actual, uint16_t exp)
        {
            if (actual != exp)
            {
                std::cout << std::hex << std::uppercase
                           << name << " mismatch: expected 0x" << +exp
                           << ", got 0x" << +actual << std::dec << "\n";
                all_ok = false;
            }
        };

        check("PC", dmg_info.PC, data["final"]["pc"]);
        check("SP", dmg_info.SP, data["final"]["sp"]);
        check("A",  dmg_info.A,  data["final"]["a"]);
        check("B",  dmg_info.B,  data["final"]["b"]);
        check("C",  dmg_info.C,  data["final"]["c"]);
        check("D",  dmg_info.D,  data["final"]["d"]);
        check("E",  dmg_info.E,  data["final"]["e"]);
        check("F",  dmg_info.F,  data["final"]["f"]);
        check("H",  dmg_info.H,  data["final"]["h"]);
        check("L",  dmg_info.L,  data["final"]["l"]);

        json ram = data["final"]["ram"];
        for (auto & i : ram)
        {
            uint16_t address = i[0];
            uint8_t value = i[1];
            if (dmg.ReadMemory(address) != value)
            {
                std::cout << std::hex << std::uppercase
                           << "Memory[0x" << address << "] mismatch: expected 0x"
                           << +value << ", got 0x" << +value
                           << std::dec << "\n";
                all_ok = false;
            }
        }

        return all_ok;
    }

    inline size_t ReadTCyclesNumber(const json& data)
    {
        return data["cycles"].size() * 4;
    }

    inline void LoadRegisters(dmg::DMG& dmg, const json& data)
    {
        dmg::CPUInfo register_info{};
        std::string name = "initial";
        std::string test_name = data["name"];
        register_info.PC = data[name]["pc"];
        register_info.SP = data[name]["sp"];
        register_info.A = data[name]["a"];
        register_info.B = data[name]["b"];
        register_info.C = data[name]["c"];
        register_info.D = data[name]["d"];
        register_info.E = data[name]["e"];
        register_info.F = data[name]["f"];
        register_info.H = data[name]["h"];
        register_info.L = data[name]["l"];
        dmg.SetRegisters(register_info);
    }

    inline void LoadMemory(dmg::DMG& dmg, const json& data)
    {
        dmg.ClearMemory();
        std::string name = "initial";
        json ram = data[name]["ram"];
        for (auto & i : ram)
        {
            dmg.WriteMemory(i[0], i[1]);
        }
    }

    inline void Reset(DMG& dmg)
    {
        dmg.Reset();
    }
}
int main(int argc, char* argv[])
{
    using namespace dmg;

    if (argc == 1)
    {
        std::cerr << "Not enough arguments. Pass a path to the folder with SSTs\n";
        return -1;
    }

    Log::Init();
    DMG dmg{dmg::DMG::InitState::TEST};

    int finished_opcode_tests = 0;
    int finished_specific_tests = 0;
    for (int i = 0; i < 256; ++i)
    {
        if (std::ranges::find(non_existent_opcodes, i) != non_existent_opcodes.end()) continue;

        std::string file_name = std::format("{}/{:02x}.json", std::string{argv[1]}, i);
        std::ifstream f{file_name};

        json root = json::parse(f);
        int j = 0;

        bool failed = false;
        for (const auto& data : root)
        {
            // Load State to DMG
            std::string name = data["name"];
            Reset(dmg);
            LoadRegisters(dmg, data);
            LoadMemory(dmg, data);
            dmg.SetState(dmg::DMG::PlayState::PAUSE);
            size_t tcycles = ReadTCyclesNumber(data);
            dmg.StepCycles(tcycles);

            // Run DMG
            if (!CompareDMGStates(dmg, data))
            {
                std::cout << std::format("Test {}-{}: did not pass.", name, j) << std::endl;
                failed = true;
            }
            else
            {
                finished_specific_tests++;
            }

            ++j;
        }

        if (!failed) finished_opcode_tests++;
    }

    for (int i = 0; i < 256; ++i)
    {
        std::string file_name = std::format("SST/cb {:02x}.json", i);
        std::ifstream f{file_name};

        json root = json::parse(f);
        int j = 0;

        bool failed = false;
        for (const auto& data : root)
        {
            // Load State to DMG
            std::string name = data["name"];
            Reset(dmg);
            LoadRegisters(dmg, data);
            LoadMemory(dmg, data);
            dmg.SetState(dmg::DMG::PlayState::PAUSE);
            size_t tcycles = ReadTCyclesNumber(data);
            dmg.StepCycles(tcycles);

            // Run DMG
            if (!CompareDMGStates(dmg, data))
            {
                std::cout << std::format("Test {}-{}: did not pass.", name, j) << std::endl;
                failed = true;
            }
            else
            {
                finished_specific_tests++;
            }

            ++j;
        }

        if (!failed) finished_opcode_tests++;
    }

    std::cout << std::format("Opcode tests finished {}/500", finished_opcode_tests) << std::endl;
    std::cout << std::format("Opcode specific tests finished {}", finished_specific_tests) << std::endl;

    return 0;
}