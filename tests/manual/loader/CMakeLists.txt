qt_internal_add_manual_test(manual_test_loader
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Quick
        Qt::Quick3D
)

# Resources:
set(qml_resource_files
    "TestModel.qml"
    "main.qml"
)

qt_internal_add_resource(manual_test_loader "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)
