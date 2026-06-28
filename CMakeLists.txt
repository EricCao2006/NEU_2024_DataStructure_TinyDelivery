cmake_minimum_required(VERSION 4.2)
project(NEU_2024_DataStructure_TinyDelivery)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/mingw_64")
include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(NEU_2024_DataStructure_TinyDelivery gui/main.cpp
        gui/MainWindow.cpp
        gui/MapWidget.cpp
        gui/panels/ControlPanel.cpp
        gui/panels/MerchantPanel.cpp
        gui/panels/CustomerPanel.cpp
        gui/panels/DeliverPanel.cpp
        gui/EventPopup.cpp
        logic/GameEngine.cpp
        logic/GameEngine.h
        logic/GameState.cpp
        logic/GameState.h
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
        logic/algorithms/OrderMerger.cpp
        logic/algorithms/DeliverSorter.cpp
        logic/algorithms/InstructionCompressor.cpp
        logic/algorithms/PathFinder.cpp
        logic/algorithms/AddressMatcher.cpp
        logic/systems/DispatchSystem.cpp
        logic/systems/DispatchSystem.h
        logic/systems/EventSystem.cpp
        logic/systems/EventSystem.h
        logic/systems/UpgradeSystem.cpp
        logic/systems/UpgradeSystem.h
        logic/utils/Random_.h
        logic/utils/Time_.h
        logic/utils/Log_.h
        logic/utils/Math_.h
        logic/utils/json_.h
        logic/models/Models.h
        logic/algorithms/Algorithms.h
        logic/systems/Systems.h
        gui/mainwindow.cpp
        gui/mainwindow.h
        gui/mainwindow.ui
        gui/MapWidget.cpp
        gui/MapWidget.h
        gui/EventPopup.cpp
        gui/EventPopup.h
        gui/EventPopup.ui
        gui/panels/ControlPanel.cpp
        gui/panels/ControlPanel.h
        gui/panels/ControlPanel.ui
        gui/panels/CustomerPanel.cpp
        gui/panels/CustomerPanel.h
        gui/panels/CustomerPanel.ui
        gui/panels/DeliverPanel.cpp
        gui/panels/DeliverPanel.h
        gui/panels/DeliverPanel.ui
        gui/panels/MerchantPanel.cpp
        gui/panels/MerchantPanel.h
        gui/panels/MerchantPanel.ui
        logic/algorithms/Algorithms.cpp
        logic/algorithms/AddressMatcher.h
        logic/algorithms/AddressMatcher.h
        logic/algorithms/DeliverSorter.h
        logic/algorithms/InstructionCompressor.h
        logic/algorithms/OrderMerger.h
        logic/algorithms/PathFinder.h
)
target_link_libraries(NEU_2024_DataStructure_TinyDelivery
        Qt::Core
        Qt::Gui
        Qt::Widgets
        nlohmann_json::nlohmann_json
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
