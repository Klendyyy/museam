    cmake_minimum_required(VERSION 3.16)
project(MuseumManagement VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(MYSQL_CONNECTOR_DIR "C:/Program Files/MySQL/MySQL Connector C 6.1/lib")
if(WIN32 AND EXISTS "${MYSQL_CONNECTOR_DIR}/lib/libmysql.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MYSQL_CONNECTOR_DIR}/lib/libmysql.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
    PrintSupport
    Charts
)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/database/databasemanager.cpp
    src/models/exhibitmodel.cpp
    src/models/collectionmodel.cpp
    src/models/exhibitionmodel.cpp
    src/models/employeemodel.cpp
    src/models/storagemodel.cpp
    src/models/categorymodel.cpp
    src/models/restorationmodel.cpp
    src/models/acquisitionmodel.cpp
    src/dialogs/exhibitdialog.cpp
    src/dialogs/collectiondialog.cpp
    src/dialogs/exhibitiondialog.cpp
    src/dialogs/employeedialog.cpp
    src/dialogs/storagedialog.cpp
    src/dialogs/categorydialog.cpp
    src/dialogs/restorationdialog.cpp
    src/dialogs/acquisitiondialog.cpp
    src/widgets/exhibitwidget.cpp
    src/widgets/collectionwidget.cpp
    src/widgets/exhibitionwidget.cpp
    src/widgets/employeewidget.cpp
    src/widgets/storagewidget.cpp
    src/widgets/dashboardwidget.cpp
    src/widgets/reportswidget.cpp
)

# Header files
set(HEADERS
    src/mainwindow.h
    src/database/databasemanager.h
    src/models/exhibitmodel.h
    src/models/collectionmodel.h
    src/models/exhibitionmodel.h
    src/models/employeemodel.h
    src/models/storagemodel.h
    src/models/categorymodel.h
    src/models/restorationmodel.h
    src/models/acquisitionmodel.h
    src/dialogs/exhibitdialog.h
    src/dialogs/collectiondialog.h
    src/dialogs/exhibitiondialog.h
    src/dialogs/employeedialog.h
    src/dialogs/storagedialog.h
    src/dialogs/categorydialog.h
    src/dialogs/restorationdialog.h
    src/dialogs/acquisitiondialog.h
    src/widgets/exhibitwidget.h
    src/widgets/collectionwidget.h
    src/widgets/exhibitionwidget.h
    src/widgets/employeewidget.h
    src/widgets/storagewidget.h
    src/widgets/dashboardwidget.h
    src/widgets/reportswidget.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::PrintSupport
    Qt6::Charts
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/database
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
)

# Install
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
