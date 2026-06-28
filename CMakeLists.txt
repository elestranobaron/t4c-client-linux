cmake_minimum_required(VERSION 3.16)
project(T4CClient172Linux VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(SDL3_TTF sdl3-ttf)
if(NOT SDL3_TTF_FOUND)
    find_package(SDL3_ttf QUIET CONFIG)
endif()

set(T4C_CLIENT_DIR "${CMAKE_SOURCE_DIR}/client/T4C Client")
set(T4C_PLATFORM_DIR "${CMAKE_SOURCE_DIR}/client/platform/linux")

add_compile_definitions(LINUX_PORT CLT_VERSION=1720 T4C_HAS_WORLD_VIEW=1)

set(T4C_LINUX_FORCE_INCLUDE "${T4C_PLATFORM_DIR}/T4CLinuxPlatform.h")

set(T4C_NETWORK_SOURCES
    "${T4C_CLIENT_DIR}/Comm.cpp"
    "${T4C_CLIENT_DIR}/TFCPacket.cpp"
    "${T4C_CLIENT_DIR}/UDP/NMPacketManager.cpp"
    "${T4C_CLIENT_DIR}/UDP/UDPClient.cpp"
    "${T4C_CLIENT_DIR}/UDP/Socket.cpp"
    "${T4C_PLATFORM_DIR}/IOCPCompat.cpp"
    "${T4C_PLATFORM_DIR}/T4CLinuxCommPort.cpp"
    "${T4C_PLATFORM_DIR}/T4CWinSync.cpp"
    src/network/T4CClientStubs.cpp
    src/network/T4CCommBridge.cpp
    src/network/T4CLoginSession.cpp
    src/network/T4CNetworkDebugLog.cpp
)

set(T4C_GUI_SOURCES
    src/gui/LoginScreen.cpp
    src/gui/CharacterSelectScreen.cpp
    src/gui/CharacterCreateScreen.cpp
    src/gui/IntroductionScreen.cpp
    src/gui/T4CCharacterQuestionnaire.cpp
    src/gui/LoginClientConfig.cpp
    src/gui/T4CUiFont.cpp
    src/gui/T4CScrollingBanner.cpp
    src/gui/LauncherChrome.cpp
)

set(T4C_APP_SOURCES
    src/main.cpp
    src/runtime/T4CRuntimePaths.cpp
    src/assets/map/T4CTmiWorldMap.cpp
    src/assets/map/T4CZoneMap.cpp
    src/assets/map/T4CMapRle.cpp
    src/assets/map/T4CV2WorldMap.cpp
    src/assets/map/T4CV2SpriteAtlas.cpp
    src/assets/map/T4CMapObjectSprites.cpp
    src/game/T4CPuppet.cpp
    src/game/T4CGroundObjectSprites.cpp
    src/game/T4CInvItemSprites.cpp
    src/game/T4CMovementBaseline.cpp
    src/game/T4CUnitSpriteNames.cpp
    src/game/T4CGameHud.cpp
    src/game/T4CCharacterWindow.cpp
    src/game/GameWorldScreen.cpp
    src/audio/T4CGameMusic.cpp
    src/audio/T4CGameMusicZone.cpp
    ${T4C_GUI_SOURCES}
    ${T4C_NETWORK_SOURCES}
)

add_executable(t4c_client ${T4C_APP_SOURCES})

target_include_directories(t4c_client PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${T4C_CLIENT_DIR}"
    "${T4C_CLIENT_DIR}/UDP"
    "${T4C_CLIENT_DIR}/Zlib"
    "${T4C_PLATFORM_DIR}"
    "${CMAKE_SOURCE_DIR}/client"
    ${SDL3_INCLUDE_DIRS}
)

if(SDL3_TTF_FOUND)
    target_include_directories(t4c_client PRIVATE ${SDL3_TTF_INCLUDE_DIRS})
endif()

target_compile_options(t4c_client PRIVATE
    -include "${T4C_LINUX_FORCE_INCLUDE}"
    -Wall
    -Wno-unused-parameter
    -Wno-unknown-pragmas
)

target_link_libraries(t4c_client PRIVATE
    ${SDL3_LIBRARIES}
    ZLIB::ZLIB
    Threads::Threads
    m
)

if(SDL3_TTF_FOUND)
    target_link_libraries(t4c_client PRIVATE ${SDL3_TTF_LIBRARIES})
elseif(TARGET SDL3_ttf::SDL3_ttf)
    target_link_libraries(t4c_client PRIVATE SDL3_ttf::SDL3_ttf)
else()
    message(WARNING "SDL3_ttf not found — polices UI desactivees")
endif()

set(T4C_E2E_PROBE_SOURCES
    src/tools/e2e_auth_probe.cpp
    src/game/T4CMovementBaseline.cpp
    src/game/T4CUnitSpriteNames.cpp
    src/audio/T4CGameMusic.cpp
    src/audio/T4CGameMusicZone.cpp
    src/runtime/T4CRuntimePaths.cpp
    ${T4C_NETWORK_SOURCES}
)

add_executable(t4c_e2e_auth_probe ${T4C_E2E_PROBE_SOURCES})
target_include_directories(t4c_e2e_auth_probe PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${T4C_CLIENT_DIR}"
    "${T4C_CLIENT_DIR}/UDP"
    "${T4C_CLIENT_DIR}/Zlib"
    "${T4C_PLATFORM_DIR}"
    "${CMAKE_SOURCE_DIR}/client"
    ${SDL3_INCLUDE_DIRS}
)
target_compile_options(t4c_e2e_auth_probe PRIVATE
    -include "${T4C_LINUX_FORCE_INCLUDE}"
    -Wall
    -Wno-unused-parameter
    -Wno-unknown-pragmas
)
target_link_libraries(t4c_e2e_auth_probe PRIVATE
    ${SDL3_LIBRARIES}
    ZLIB::ZLIB
    Threads::Threads
    m
)

add_executable(t4c_regression_tests
    src/tests/t4c_regression_tests.cpp
    src/game/T4CMovementBaseline.cpp
    src/game/T4CUnitSpriteNames.cpp
)
target_include_directories(t4c_regression_tests PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_compile_options(t4c_regression_tests PRIVATE -Wall -Wextra)
target_link_libraries(t4c_regression_tests PRIVATE m)

enable_testing()
add_test(NAME regression COMMAND t4c_regression_tests)

message(STATUS "T4C Client 1.72 Linux — login + monde test")
message(STATUS "  T4C_RUNTIME -> assets (defaut: ./runtime)")
message(STATUS "  t4c_e2e_auth_probe -> test auth headless")
message(STATUS "  t4c_regression_tests -> non-regression baseline (ctest / run_regression_tests.sh)")
