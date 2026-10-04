#include "DMG/DMG.hpp"
#include "DMG/RegisterInfo.hpp"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <array>
#include <format>

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
        RegisterInfo register_info = dmg.GetRegisters();
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

        check("PC", register_info.PC, data["final"]["pc"]);
        check("SP", register_info.SP, data["final"]["sp"]);
        check("A",  register_info.A,  data["final"]["a"]);
        check("B",  register_info.B,  data["final"]["b"]);
        check("C",  register_info.C,  data["final"]["c"]);
        check("D",  register_info.D,  data["final"]["d"]);
        check("E",  register_info.E,  data["final"]["e"]);
        check("F",  register_info.F,  data["final"]["f"]);
        check("H",  register_info.H,  data["final"]["h"]);
        check("L",  register_info.L,  data["final"]["l"]);

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
        dmg::RegisterInfo register_info{};
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
}
int main()
{
    using namespace dmg;

    Log::Init();
    DMG dmg{dmg::DMG::InitState::TEST};

    for (int i = 0; i < 256; ++i)
    {
        if (std::ranges::find(non_existent_opcodes, i) != non_existent_opcodes.end()) continue;

        std::string file_name = std::format("SST/{:02x}.json", i);
        std::ifstream f{file_name};
        json data = json::parse(f)[0];

        // Load State to DMG
        std::string name = data["name"];
        LoadRegisters(dmg, data);
        LoadMemory(dmg, data);
        dmg.SetState(dmg::DMG::State::PAUSE);
        size_t tcycles = ReadTCyclesNumber(data);
        dmg.StepCycles(tcycles);

        // Run DMG
        if (!CompareDMGStates(dmg, data))
        {
            std::cout << std::format("Test {}: did not pass.", name) << std::endl;
        }
    }

    return 0;
}