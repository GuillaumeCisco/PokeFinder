#-------------------------------------------------
#
# Project created by QtCreator 2017-07-28T23:02:16
#
#-------------------------------------------------

QT       += core gui
QT       += xml
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PokeFinder
TEMPLATE = app
VERSION = 1.2.0
QMAKE_TARGET_DESCRIPTION = PokeFinder
QMAKE_TARGET_COPYRIGHT = Admiral_Fish

RC_ICONS += pokefinder.ico
ICON += pokefinder.icns

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TRANSLATIONS += \
    Languages/PokeFinder_en.ts \
    Languages/PokeFinder_fr.ts \
    Languages/PokeFinder_es.ts \
    Languages/PokeFinder_de.ts \
    Languages/PokeFinder_it.ts \
    Languages/PokeFinder_ja.ts \
    Languages/PokeFinder_ko.ts \
    Languages/PokeFinder_zh_Hans_CN.ts

RESOURCES += \
    resources.qrc

FORMS += \
    Forms/Controls/IVFilter.ui \
    Forms/Gen3/Eggs3.ui \
    Forms/Gen3/GameCubeRTC.ui \
    Forms/Gen3/IDs3.ui \
    Forms/Gen3/JirachiGeneration.ui \
    Forms/Gen3/PIDtoIVs.ui \
    Forms/Gen3/PokeSpot.ui \
    Forms/Gen3/SeedToTime3.ui \
    Forms/Gen3/Stationary3.ui \
    Forms/Gen3/Wild3.ui \
    Forms/ProfileManager/ProfileManager3.ui \
    Forms/ProfileManager/ProfileManager3NewEdit.ui \
    Forms/Util/IVtoPID.ui \
    Forms/Util/Researcher.ui \
    Forms/MainWindow.ui

HEADERS += \
    Forms/Controls/IVFilter.hpp \
    Forms/Controls/QCheckList.hpp \
    Forms/Controls/QTextBox.hpp \
    Forms/Gen3/Eggs3.hpp \
    Forms/Gen3/GameCubeRTC.hpp \
    Forms/Gen3/IDs3.hpp \
    Forms/Gen3/JirachiGeneration.hpp \
    Forms/Gen3/PIDtoIVs.hpp \
    Forms/Gen3/PokeSpot.hpp \
    Forms/Gen3/SeedToTime3.hpp \
    Forms/Gen3/Stationary3.hpp \
    Forms/Gen3/Wild3.hpp \
    Forms/ProfileManager/ProfileManager3.hpp \
    Forms/ProfileManager/ProfileManager3NewEdit.hpp \
    Forms/Util/IVtoPID.hpp \
    Forms/Util/Researcher.hpp \
    Forms/MainWindow.hpp \
    libPokeFinder/Gen3/Egg3.hpp \
    libPokeFinder/Gen3/Frame3.hpp \
    libPokeFinder/Gen3/Generator3.hpp \
    libPokeFinder/Gen3/NatureLock.hpp \
    libPokeFinder/Gen3/Profile3.hpp \
    libPokeFinder/Gen3/Searcher3.hpp \
    libPokeFinder/Objects/Egg.hpp \
    libPokeFinder/Objects/Encounter.hpp \
    libPokeFinder/Objects/EncounterSlot.hpp \
    libPokeFinder/Objects/Frame.hpp \
    libPokeFinder/Objects/FrameCompare.hpp \
    libPokeFinder/Objects/Generator.hpp \
    libPokeFinder/Objects/Lead.hpp \
    libPokeFinder/Objects/Method.hpp \
    libPokeFinder/Objects/Nature.hpp \
    libPokeFinder/Objects/Power.hpp \
    libPokeFinder/Objects/Profile.hpp \
    libPokeFinder/Objects/Searcher.hpp \
    libPokeFinder/Objects/Utilities.hpp \
    libPokeFinder/RNG/IRNG.hpp \
    libPokeFinder/RNG/IRNG64.hpp \
    libPokeFinder/RNG/LCRNG.hpp \
    libPokeFinder/RNG/LCRNG64.hpp \
    libPokeFinder/RNG/MTRNG.hpp \
    libPokeFinder/RNG/RNGCache.hpp \
    libPokeFinder/RNG/RNGEuclidean.hpp \
    libPokeFinder/RNG/SFMT.hpp \
    libPokeFinder/RNG/TinyMT.hpp \
    Models/Gen3/Egg3Model.hpp \
    Models/Gen3/PokeSpotModel.hpp \
    Models/Gen3/Profile3Model.hpp \
    Models/Gen3/Searcher3Model.hpp \
    Models/Gen3/Stationary3Model.hpp \
    Models/Gen3/Wild3Model.hpp \
    Models/ResearcherModel.hpp \
    Util/ResearcherFrame.hpp

SOURCES += \
    Forms/Controls/IVFilter.cpp \
    Forms/Controls/QCheckList.cpp \
    Forms/Controls/QTextBox.cpp \
    Forms/Gen3/Eggs3.cpp \
    Forms/Gen3/GameCubeRTC.cpp \
    Forms/Gen3/IDs3.cpp \
    Forms/Gen3/JirachiGeneration.cpp \
    Forms/Gen3/PIDtoIVs.cpp \
    Forms/Gen3/PokeSpot.cpp \
    Forms/Gen3/SeedToTime3.cpp \
    Forms/Gen3/Stationary3.cpp \
    Forms/Gen3/Wild3.cpp \
    Forms/ProfileManager/ProfileManager3.cpp \
    Forms/ProfileManager/ProfileManager3NewEdit.cpp \
    Forms/Util/IVtoPID.cpp \
    Forms/Util/Researcher.cpp \
    Forms/MainWindow.cpp \
    libPokeFinder/Gen3/Egg3.cpp \
    libPokeFinder/Gen3/Frame3.cpp \
    libPokeFinder/Gen3/Generator3.cpp \
    libPokeFinder/Gen3/NatureLock.cpp \
    libPokeFinder/Gen3/Profile3.cpp \
    libPokeFinder/Gen3/Searcher3.cpp \
    libPokeFinder/Objects/EncounterSlot.cpp \
    libPokeFinder/Objects/Frame.cpp \
    libPokeFinder/Objects/FrameCompare.cpp \
    libPokeFinder/Objects/Nature.cpp \
    libPokeFinder/Objects/Power.cpp \
    libPokeFinder/Objects/Utilities.cpp \
    libPokeFinder/RNG/LCRNG.cpp \
    libPokeFinder/RNG/LCRNG64.cpp \
    libPokeFinder/RNG/MTRNG.cpp \
    libPokeFinder/RNG/RNGCache.cpp \
    libPokeFinder/RNG/RNGEuclidean.cpp \
    libPokeFinder/RNG/SFMT.cpp \
    libPokeFinder/RNG/TinyMT.cpp \
    Models/Gen3/Egg3Model.cpp \
    Models/Gen3/PokeSpotModel.cpp \
    Models/Gen3/Profile3Model.cpp \
    Models/Gen3/Searcher3Model.cpp \
    Models/Gen3/Stationary3Model.cpp \
    Models/Gen3/Wild3Model.cpp \
    Models/ResearcherModel.cpp \
    Util/ResearcherFrame.cpp \
    main.cpp
