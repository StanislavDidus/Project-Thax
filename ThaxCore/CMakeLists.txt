cmake_minimum_required(VERSION 4.2)

project(ThaxCore)

set(CMAKE_CXX_STANDARD 23)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Don't build shared libraries." FORCE)

add_subdirectory(externals)

set(HEADER_FILES
        src/CPU/CPU.hpp
        src/MemoryBus/Bus.hpp
        include/DMG/DMG.hpp
        include/DMG/Utils/Log.hpp
        src/CPU/ControlUnit.hpp
        src/Utils/Math.hpp
        src/CPU/Registers/ByteRegister.hpp
        src/CPU/Registers/ByteRegisterPair.hpp
        src/CPU/Registers/TwoByteRegister.hpp
        src/CPU/Registers/ATwoByteRegister.hpp
        src/CPU/Registers/FlagRegister.hpp
        src/CPU/Registers/AByteRegister.hpp
        src/CPU/Registers/Address.hpp
        include/DMG/RegisterInfo.hpp
        src/Miscellaneous/Timer.hpp
        src/Miscellaneous/Cartridge.hpp
        src/InternalGlobals.hpp
        src/MemoryBus/StandardBus.hpp
        src/MemoryBus/TestBus.hpp
        src/Miscellaneous/InterruptController.hpp
        src/Miscellaneous/SerialPort.hpp
        src/Miscellaneous/Joypad.hpp
        src/PPU/PPU.hpp
        src/PPU/Tile.hpp
        src/PPU/Tilemap.hpp
        src/PPU/Object.hpp
        src/PPU/PixelFetcher.hpp
        src/PPU/Registers/LCDCRegister.hpp
        include/DMG/ExternalGlobals.hpp
        src/PPU/Registers/STATRegister.hpp
        src/PPU/FIFO.hpp
)

set(SOURCE_FILES
        src/Utils/Log.cpp
        src/CPU/ControlUnit.cpp
        src/DMG.cpp
        src/CPU/CPU.cpp
        src/Miscellaneous/Timer.cpp
        src/Miscellaneous/Joypad.cpp
        src/Miscellaneous/InterruptController.cpp
        src/CPU/Opcodes_Impl.cpp
        src/CPU/CB_Opcodes_Impl.cpp
        src/PPU/PPU.cpp
        src/PPU/Object.cpp
        src/PPU/Registers/LCDCRegister.cpp
        src/PPU/Registers/STATRegister.cpp
        src/PPU/FIFO.cpp
)

set(ALL_FILES
        ${HEADER_FILES}
        ${SOURCE_FILES}
        include/DMG/Input.hpp
)

add_library(ThaxCore ${ALL_FILES})


#add_custom_target(copy_command
        #COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/copy_files.cmake
#)
#add_dependencies(GameBoyCore copy_command)

target_include_directories(ThaxCore
        PRIVATE src/
        PUBLIC include/
)

#target_link_libraries(GameBoyEm PUBLIC spdlog::spdlog glfw imgui nlohmann_json::nlohmann_json)
target_link_libraries(ThaxCore spdlog::spdlog)
