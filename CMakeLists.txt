cmake_minimum_required(VERSION 4.2)
project(NEU_2024_DataStructure_TinyDelivery)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/mingw_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# ── GUI 层 ──
set(GUI_SOURCES
        gui/main.cpp
        gui/MainWindow.cpp
        gui/MainWindow.h
        gui/MainWindow.ui
        gui/MapWidget.cpp
        gui/MapWidget.h
        gui/EventPopup.cpp
        gui/EventPopup.h
        gui/EventPopup.ui
        gui/panels/ControlPanel.cpp
        gui/panels/ControlPanel.h
        gui/panels/ControlPanel.ui
        gui/panels/MerchantPanel.cpp
        gui/panels/MerchantPanel.h
        gui/panels/MerchantPanel.ui
        gui/panels/CustomerPanel.cpp
        gui/panels/CustomerPanel.h
        gui/panels/CustomerPanel.ui
        gui/panels/DeliverPanel.cpp
        gui/panels/DeliverPanel.h
        gui/panels/DeliverPanel.ui
)

# ── 逻辑层：核心引擎 ──
set(LOGIC_ENGINE_SOURCES
        logic/GameEngine.cpp
        logic/GameEngine.h
        logic/GameState.cpp
        logic/GameState.h
)

# ── 逻辑层：数据模型 ──
set(LOGIC_MODEL_SOURCES
        logic/models/Models.h
        logic/models/Enums.h
        logic/models/Order.cpp
        logic/models/Order.h
        logic/models/Deliver.cpp
        logic/models/Deliver.h
        logic/models/Merchant.cpp
        logic/models/Merchant.h
        logic/models/Customer.cpp
        logic/models/Customer.h
        logic/models/CityGrid.cpp
        logic/models/CityGrid.h
)

# ── 逻辑层：数据处理模块 ──
set(LOGIC_ALGO_SOURCES
        logic/algorithms/Algorithms.cpp
        logic/algorithms/Algorithms.h
        logic/algorithms/OrderMerger.cpp
        logic/algorithms/OrderMerger.h
        logic/algorithms/DeliverSorter.cpp
        logic/algorithms/DeliverSorter.h
        logic/algorithms/InstructionCompressor.cpp
        logic/algorithms/InstructionCompressor.h
        logic/algorithms/PathFinder.cpp
        logic/algorithms/PathFinder.h
        logic/algorithms/AddressMatcher.cpp
        logic/algorithms/AddressMatcher.h
)

# ── 逻辑层：业务系统 ──
set(LOGIC_SYSTEM_SOURCES
        logic/systems/Systems.h
        logic/systems/DispatchSystem.cpp
        logic/systems/DispatchSystem.h
        logic/systems/EventSystem.cpp
        logic/systems/EventSystem.h
        logic/systems/UpgradeSystem.cpp
        logic/systems/UpgradeSystem.h
)

# ── 逻辑层：工具库 ──
set(LOGIC_UTIL_SOURCES
        logic/utils/log_.h
        logic/utils/math_.h
        logic/utils/random_.h
        logic/utils/time_.h
        logic/utils/json_.h

)

# ── 资源文件 ──
set(RESOURCE_SOURCES
        resources.qrc
)

add_executable(NEU_2024_DataStructure_TinyDelivery
        ${GUI_SOURCES}
        ${LOGIC_ENGINE_SOURCES}
        ${LOGIC_MODEL_SOURCES}
        ${LOGIC_ALGO_SOURCES}
        ${LOGIC_SYSTEM_SOURCES}
        ${LOGIC_UTIL_SOURCES}
        ${RESOURCE_SOURCES}
)
target_link_libraries(NEU_2024_DataStructure_TinyDelivery
        Qt::Core
        Qt::Gui
        Qt::Widgets
        nlohmann_json::nlohmann_json
)

# 设置运行工作目录为项目根目录（确保能找到 resource/config 等文件）
set_target_properties(NEU_2024_DataStructure_TinyDelivery PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
