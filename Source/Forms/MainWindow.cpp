/*
 * This file is part of PokéFinder
 * Copyright (C) 2017-2022 by Admiral_Fish, bumba, and EzPzStreamz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <Core/Util/Translator.hpp>
#include <Forms/Gen3/Eggs3.hpp>
#include <Forms/Gen3/GameCube.hpp>
#include <Forms/Gen3/IDs3.hpp>
#include <Forms/Gen3/Profile/ProfileManager3.hpp>
#include <Forms/Gen3/Static3.hpp>
#include <Forms/Gen3/Tools/GameCubeRTC.hpp>
#include <Forms/Gen3/Tools/GameCubeSeedFinder.hpp>
#include <Forms/Gen3/Tools/JirachiPattern.hpp>
#include <Forms/Gen3/Tools/PIDIV.hpp>
#include <Forms/Gen3/Tools/PokeSpot.hpp>
#include <Forms/Gen3/Tools/SeedTime3.hpp>
#include <Forms/Gen3/Tools/SpindaPainter.hpp>
#include <Forms/Gen3/Wild3.hpp>
#include <Forms/Gen4/Eggs4.hpp>
#include <Forms/Gen4/IDs4.hpp>
#include <Forms/Gen4/Profile/ProfileManager4.hpp>
#include <Forms/Gen4/Static4.hpp>
#include <Forms/Gen4/Tools/ChainedSID.hpp>
#include <Forms/Gen4/Tools/SeedtoTime4.hpp>
#include <Forms/Gen4/Wild4.hpp>
#include <Forms/Gen5/DreamRadar.hpp>
#include <Forms/Gen5/Eggs5.hpp>
#include <Forms/Gen5/Event5.hpp>
#include <Forms/Gen5/HiddenGrotto.hpp>
#include <Forms/Gen5/IDs5.hpp>
#include <Forms/Gen5/Profile/ProfileCalibrator5.hpp>
#include <Forms/Gen5/Profile/ProfileManager5.hpp>
#include <Forms/Gen5/Static5.hpp>
#include <Forms/Gen8/DenMap.hpp>
#include <Forms/Gen8/Eggs8.hpp>
#include <Forms/Gen8/Event8.hpp>
#include <Forms/Gen8/IDs8.hpp>
#include <Forms/Gen8/Profile/ProfileManager8.hpp>
#include <Forms/Gen8/Raids.hpp>
#include <Forms/Gen8/Static8.hpp>
#include <Forms/Gen8/Wild8.hpp>
#include <Forms/Util/EncounterLookup.hpp>
#include <Forms/Util/IVCalculator.hpp>
#include <Forms/Util/IVtoPID.hpp>
#include <Forms/Util/Researcher.hpp>
#include <Forms/Util/Settings.hpp>
#include <QActionGroup>
#include <QDate>
#include <QDesktopServices>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QtNetwork>
#include <version.h>

MainWindow::MainWindow(bool profile, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("Pok\303\251Finder %1").arg(POKEFINDER_VERSION));

    setupModels();

    QTimer::singleShot(1000, this, [this, profile] {
        if (!profile)
        {
            QMessageBox message(QMessageBox::Warning, tr("Unable to locate profiles"),
                                tr("Please update path to your profiles file to restore existing profiles."));
            message.exec();
        }

        checkUpdates();
    });
}

MainWindow::~MainWindow()
{
    QSettings setting;
    setting.setValue("mainWindow/geometry", this->saveGeometry());

    delete ui;
    delete static3;
    delete wild3;
    delete egg3;
    delete gamecube;
    delete ids3;
    delete static4;
    delete wild4;
    delete egg4;
    delete ids4;
    delete static5;
    delete event5;
    delete dreamRadar;
    delete hiddenGrotto;
    delete egg5;
    delete ids5;
    delete static8;
    delete wild8;
    delete event8;
    delete raids;
    delete egg8;
    delete ids8;
}

void MainWindow::setupModels()
{
    connect(ui->pushButtonStatic3, &QPushButton::clicked, this, &MainWindow::openStatic3);
    connect(ui->pushButtonWild3, &QPushButton::clicked, this, &MainWindow::openWild3);
    connect(ui->pushButtonGameCube, &QPushButton::clicked, this, &MainWindow::openGameCube);
    connect(ui->pushButtonEgg3, &QPushButton::clicked, this, &MainWindow::openEgg3);
    connect(ui->pushButtonIDs3, &QPushButton::clicked, this, &MainWindow::openIDs3);
    connect(ui->actionGameCubeRTC, &QAction::triggered, this, &MainWindow::openGameCubeRTC);
    connect(ui->actionGameCubeSeedFinder, &QAction::triggered, this, &MainWindow::openGameCubeSeedFinder);
    connect(ui->actionIVtoPID3, &QAction::triggered, this, &MainWindow::openIVtoPID);
    connect(ui->actionJirachiPattern, &QAction::triggered, this, &MainWindow::openJirachiPattern);
    connect(ui->actionPIDtoIV, &QAction::triggered, this, &MainWindow::openPIDtoIV);
    connect(ui->actionPokeSpot, &QAction::triggered, this, &MainWindow::openPokeSpot);
    connect(ui->actionProfileManager3, &QAction::triggered, this, &MainWindow::openProfileManager3);
    connect(ui->actionSeedtoTime3, &QAction::triggered, this, &MainWindow::openSeedtoTime3);
    connect(ui->actionSpindaPainter, &QAction::triggered, this, &MainWindow::openSpindaPainter);

    connect(ui->pushButtonStatic4, &QPushButton::clicked, this, &MainWindow::openStatic4);
    connect(ui->pushButtonWild4, &QPushButton::clicked, this, &MainWindow::openWild4);
    connect(ui->pushButtonEgg4, &QPushButton::clicked, this, &MainWindow::openEgg4);
    connect(ui->pushButtonIDs4, &QPushButton::clicked, this, &MainWindow::openIDs4);
    connect(ui->actionProfileManager4, &QAction::triggered, this, &MainWindow::openProfileManager4);
    connect(ui->actionIVtoPID4, &QAction::triggered, this, &MainWindow::openIVtoPID);
    connect(ui->actionSeedtoTime4, &QAction::triggered, this, &MainWindow::openSeedtoTime4);
    connect(ui->actionSIDfromChainedShiny, &QAction::triggered, this, &MainWindow::openSIDFromChainedShiny);

    connect(ui->pushButtonStatic5, &QPushButton::clicked, this, &MainWindow::openStatic5);
    connect(ui->pushButtonEvent5, &QPushButton::clicked, this, &MainWindow::openEvent5);
    connect(ui->pushButtonDreamRadar, &QPushButton::clicked, this, &MainWindow::openDreamRadar);
    connect(ui->pushButtonHiddenGrotto, &QPushButton::clicked, this, &MainWindow::openHiddenGrotto);
    connect(ui->pushButtonEgg5, &QPushButton::clicked, this, &MainWindow::openEgg5);
    connect(ui->pushButtonIDs5, &QPushButton::clicked, this, &MainWindow::openIDs5);
    connect(ui->actionProfileCalibrator, &QAction::triggered, this, &MainWindow::openProfileCalibrator);
    connect(ui->actionProfileManager5, &QAction::triggered, this, &MainWindow::openProfileManager5);

    connect(ui->pushButtonStatic8, &QPushButton::clicked, this, &MainWindow::openStatic8);
    connect(ui->pushButtonWild8, &QPushButton::clicked, this, &MainWindow::openWild8);
    connect(ui->pushButtonEvent8, &QPushButton::clicked, this, &MainWindow::openEvent8);
    connect(ui->pushButtonRaid, &QPushButton::clicked, this, &MainWindow::openRaids);
    connect(ui->pushButtonEgg8, &QPushButton::clicked, this, &MainWindow::openEgg8);
    connect(ui->pushButtonIDs8, &QPushButton::clicked, this, &MainWindow::openIDs8);
    connect(ui->actionDenMap, &QAction::triggered, this, &MainWindow::openDenMap);
    connect(ui->actionDownloadEventData, &QAction::triggered, this, &MainWindow::downloadEventData);
    connect(ui->actionProfileManager8, &QAction::triggered, this, &MainWindow::openProfileManager8);

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::openAbout);
    connect(ui->actionEncounterLookup, &QAction::triggered, this, &MainWindow::openEncounterLookup);
    connect(ui->actionIVCalculator, &QAction::triggered, this, &MainWindow::openIVCalculator);
    connect(ui->actionResearcher, &QAction::triggered, this, &MainWindow::openResearcher);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::openSettings);

    QSettings setting;
    if (setting.contains("mainWindow/geometry"))
    {
        this->restoreGeometry(setting.value("mainWindow/geometry").toByteArray());
    }
}

void MainWindow::checkUpdates()
{
    QSettings setting;
    QDate today = QDate::currentDate();
    QDate lastOpened = setting.value("settings/lastOpened", today).toDate();

    if (lastOpened.daysTo(today) > 0)
    {
        auto json = QJsonDocument::fromJson(downloadFile("https://api.github.com/repos/Admiral-Fish/PokeFinder/releases/latest")).object();
        QString webVersion = json["tag_name"].toString().right(5);
        if (!webVersion.isEmpty() && POKEFINDER_VERSION != webVersion)
        {
            QMessageBox info(QMessageBox::Question, tr("Update Check"),
                             tr("An update is available. Would you like to download the newest version?"),
                             QMessageBox::Yes | QMessageBox::No);
            if (info.exec() == QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl("https://github.com/Admiral-Fish/PokeFinder/releases/latest"));
            }
        }
    }

    setting.setValue("settings/lastOpened", today);
}

QByteArray MainWindow::downloadFile(const QString &url)
{
    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    std::unique_ptr<QNetworkReply> reply(manager.get(request));

    QEventLoop loop;
    connect(reply.get(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    return reply->readAll();
}

void MainWindow::updateProfiles(int num)
{
    if (num == 3)
    {
        if (static3)
        {
            static3->updateProfiles();
        }
        if (wild3)
        {
            wild3->updateProfiles();
        }
        if (gamecube)
        {
            gamecube->updateProfiles();
        }
        if (egg3)
        {
            egg3->updateProfiles();
        }
    }
    else if (num == 4)
    {
        if (static4)
        {
            static4->updateProfiles();
        }
        if (wild4)
        {
            wild4->updateProfiles();
        }
        if (egg4)
        {
            egg4->updateProfiles();
        }
    }
    else if (num == 5)
    {
        if (static5)
        {
            static5->updateProfiles();
        }
        if (event5)
        {
            event5->updateProfiles();
        }
        if (dreamRadar)
        {
            dreamRadar->updateProfiles();
        }
        if (ids5)
        {
            ids5->updateProfiles();
        }
        if (egg5)
        {
            egg5->updateProfiles();
        }
        if (hiddenGrotto)
        {
            hiddenGrotto->updateProfiles();
        }
    }
    else if (num == 8)
    {
        if (static8)
        {
            static8->updateProfiles();
        }
        if (wild8)
        {
            wild8->updateProfiles();
        }
        if (event8)
        {
            event8->updateProfiles();
        }
        if (raids)
        {
            raids->updateProfiles();
        }
        if (egg8)
        {
            egg8->updateProfiles();
        }
        if (ids8)
        {
            ids8->updateProfiles();
        }
    }
}

void MainWindow::openStatic3()
{
    if (!static3)
    {
        static3 = new Static3();
        connect(static3, &Static3::alertProfiles, this, &MainWindow::updateProfiles);
    }
    static3->show();
}

void MainWindow::openWild3()
{
    if (!wild3)
    {
        wild3 = new Wild3();
        connect(wild3, &Wild3::alertProfiles, this, &MainWindow::updateProfiles);
    }
    wild3->show();
}

void MainWindow::openGameCube()
{
    if (!gamecube)
    {
        gamecube = new GameCube();
        connect(gamecube, &GameCube::alertProfiles, this, &MainWindow::updateProfiles);
    }
    gamecube->show();
}

void MainWindow::openEgg3()
{
    if (!egg3)
    {
        egg3 = new Eggs3();
        connect(egg3, &Eggs3::alertProfiles, this, &MainWindow::updateProfiles);
    }
    egg3->show();
}

void MainWindow::openIDs3()
{
    if (!ids3)
    {
        ids3 = new IDs3();
    }
    ids3->show();
}

void MainWindow::openGameCubeRTC()
{
    auto *rtc = new GameCubeRTC();
    rtc->show();
}

void MainWindow::openGameCubeSeedFinder()
{
    auto *finder = new GameCubeSeedFinder();
    finder->show();
}

void MainWindow::openIVtoPID()
{
    auto *ivToPID = new IVtoPID();
    ivToPID->show();
}

void MainWindow::openJirachiPattern()
{
    auto *jirachi = new JirachiPattern();
    jirachi->show();
}

void MainWindow::openPIDtoIV()
{
    auto *pidToIV = new PIDIV();
    pidToIV->show();
}

void MainWindow::openPokeSpot()
{
    auto *pokeSpot = new PokeSpot();
    pokeSpot->show();
}

void MainWindow::openProfileManager3()
{
    auto *manager = new ProfileManager3();
    connect(manager, &ProfileManager3::updateProfiles, this, [=] { updateProfiles(3); });
    manager->show();
}

void MainWindow::openSeedtoTime3()
{
    auto *seedToTime = new SeedTime3();
    seedToTime->show();
}

void MainWindow::openSpindaPainter()
{
    auto *spinda = new SpindaPainter();
    spinda->show();
}

void MainWindow::openStatic4()
{
    if (!static4)
    {
        static4 = new Static4();
        connect(static4, &Static4::alertProfiles, this, &MainWindow::updateProfiles);
    }
    static4->show();
}

void MainWindow::openWild4()
{
    if (!wild4)
    {
        wild4 = new Wild4();
        connect(wild4, &Wild4::alertProfiles, this, &MainWindow::updateProfiles);
    }
    wild4->show();
}

void MainWindow::openEgg4()
{
    if (!egg4)
    {
        egg4 = new Eggs4();
        connect(egg4, &Eggs4::alertProfiles, this, &MainWindow::updateProfiles);
    }
    egg4->show();
}

void MainWindow::openIDs4()
{
    if (!ids4)
    {
        ids4 = new IDs4();
    }
    ids4->show();
}

void MainWindow::openProfileManager4()
{
    auto *manager = new ProfileManager4();
    connect(manager, &ProfileManager4::updateProfiles, this, [=] { updateProfiles(4); });
    manager->show();
}

void MainWindow::openSeedtoTime4()
{
    auto *seedToTime = new SeedtoTime4();
    seedToTime->show();
}

void MainWindow::openSIDFromChainedShiny()
{
    auto *chainedSID = new ChainedSID();
    chainedSID->show();
}

void MainWindow::openStatic5()
{
    if (!static5)
    {
        static5 = new Static5();
        connect(static5, &Static5::alertProfiles, this, &MainWindow::updateProfiles);
    }
    static5->show();

    if (!static5->hasProfiles())
    {
        QMessageBox message(QMessageBox::Warning, tr("No profiles found"),
                            tr("Please use the Profile Calibrator under Gen 5 Tools to create one."));
        message.exec();
        static5->close();
    }
}

void MainWindow::openEvent5()
{
    if (!event5)
    {
        event5 = new Event5();
        connect(event5, &Event5::alertProfiles, this, &MainWindow::updateProfiles);
    }
    event5->show();

    if (!event5->hasProfiles())
    {
        QMessageBox message(QMessageBox::Warning, tr("No profiles found"),
                            tr("Please use the Profile Calibrator under Gen 5 Tools to create one."));
        message.exec();
        event5->close();
    }
}

void MainWindow::openDreamRadar()
{
    if (!dreamRadar)
    {
        dreamRadar = new DreamRadar();
        connect(dreamRadar, &DreamRadar::alertProfiles, this, &MainWindow::updateProfiles);
    }
    dreamRadar->show();

    if (!dreamRadar->hasProfiles())
    {
        QMessageBox message(QMessageBox::Warning, tr("No profiles found"),
                            tr("Please use the Profile Calibrator under Gen 5 Tools to create one."));
        message.exec();
        dreamRadar->close();
    }
}

void MainWindow::openHiddenGrotto()
{
    if (!hiddenGrotto)
    {
        hiddenGrotto = new HiddenGrotto();
        connect(hiddenGrotto, &HiddenGrotto::alertProfiles, this, &MainWindow::updateProfiles);
    }
    hiddenGrotto->show();

    if (!hiddenGrotto->hasProfiles())
    {
        QMessageBox message(QMessageBox::Warning, tr("No profiles found"),
                            tr("Please use the Profile Calibrator under Gen 5 Tools to create one."));
        message.exec();
        hiddenGrotto->close();
    }
}

void MainWindow::openEgg5()
{
    if (!egg5)
    {
        egg5 = new Eggs5();
        connect(egg5, &Eggs5::alertProfiles, this, &MainWindow::updateProfiles);
    }
    egg5->show();

    if (!egg5->hasProfiles())
    {
        QMessageBox message(QMessageBox::Warning, tr("No profiles found"),
                            tr("Please use the Profile Calibrator under Gen 5 Tools to create one."));
        message.exec();
        egg5->close();
    }
}

void MainWindow::openIDs5()
{
    if (!ids5)
    {
        ids5 = new IDs5();
        connect(ids5, &IDs5::alertProfiles, this, &MainWindow::updateProfiles);
    }
    ids5->show();

    if (!ids5->hasProfiles())
    {
        QMessageBox message(QMessageBox::Warning, tr("No profiles found"),
                            tr("Please use the Profile Calibrator under Gen 5 Tools to create one."));
        message.exec();
        ids5->close();
    }
}

void MainWindow::openProfileCalibrator()
{
    auto *calibrator = new ProfileCalibrator5();
    connect(calibrator, &ProfileCalibrator5::alertProfiles, this, &MainWindow::updateProfiles);
    calibrator->show();
}

void MainWindow::openProfileManager5()
{
    auto *manager = new ProfileManager5();
    connect(manager, &ProfileManager5::updateProfiles, this, [=] { updateProfiles(5); });
    manager->show();
}

void MainWindow::openStatic8()
{
    if (!static8)
    {
        static8 = new Static8();
        connect(static8, &Static8::alertProfiles, this, &MainWindow::updateProfiles);
    }
    static8->show();
}

void MainWindow::openWild8()
{
    if (!wild8)
    {
        wild8 = new Wild8();
        connect(wild8, &Wild8::alertProfiles, this, &MainWindow::updateProfiles);
    }
    wild8->show();
}

void MainWindow::openEvent8()
{
    if (!event8)
    {
        event8 = new Event8();
        connect(event8, &Event8::alertProfiles, this, &MainWindow::updateProfiles);
    }
    event8->show();
}

void MainWindow::openRaids()
{
    if (!raids)
    {
        raids = new Raids();
        connect(raids, &Raids::alertProfiles, this, &MainWindow::updateProfiles);
    }
    raids->show();
}

void MainWindow::openEgg8()
{
    if (!egg8)
    {
        egg8 = new Eggs8();
        connect(egg8, &Eggs8::alertProfiles, this, &MainWindow::updateProfiles);
    }
    egg8->show();
}

void MainWindow::openIDs8()
{
    if (!ids8)
    {
        ids8 = new IDs8();
        connect(ids8, &IDs8::alertProfiles, this, &MainWindow::updateProfiles);
    }
    ids8->show();
}

void MainWindow::openDenMap()
{
    auto *map = new DenMap();
    map->show();
}

void MainWindow::downloadEventData()
{
    auto fileResponse
        = downloadFile("https://raw.githubusercontent.com/Admiral-Fish/RaidFinder/master/Resources/Encounters/Event/files.txt");
    if (fileResponse.isEmpty())
    {
        QMessageBox error(QMessageBox::Critical, tr("Download failed"),
                          tr("Make sure you are connected to the internet and have OpenSSL setup"), QMessageBox::Ok);
        error.exec();
        return;
    }

    QStringList infos = QString(fileResponse).split('\n');
    QStringList files;
    QStringList entries;
    for (const QString &info : infos)
    {
        QStringList data = info.split(',');
        QString file = data.at(0);
        u16 specie = data.at(1).toUShort();

        files.prepend(file);

        file = file.left(file.indexOf('_'));
        file.insert(2, '-');
        file.insert(5, '-');

        entries.prepend(QString("%1: %2").arg(file, QString::fromStdString(*Translator::getSpecies(specie))));
    }

    bool flag;
    QString item = QInputDialog::getItem(this, tr("Download Event Data"), tr("Event"), entries, 0, false, &flag,
                                         Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    if (!flag)
    {
        return;
    }

    int index = entries.indexOf(item);
    auto eventResponse
        = downloadFile("https://raw.githubusercontent.com/Admiral-Fish/RaidFinder/master/Resources/Encounters/Event/" + files.at(index));
    if (eventResponse.isEmpty())
    {
        QMessageBox error(QMessageBox::Critical, tr("Download failed"),
                          tr("Make sure you are connected to the internet and have OpenSSL setup"), QMessageBox::Ok);
        error.exec();
        return;
    }

    QFile f(QApplication::applicationDirPath() + "/nests_event.json");
    if (f.open(QIODevice::WriteOnly))
    {
        f.write(qUncompress(eventResponse));
        f.close();

        QMessageBox message(QMessageBox::Question, tr("Download finished"), tr("Restart to see event data. Restart now?"),
                            QMessageBox::Yes | QMessageBox::No);
        if (message.exec() == QMessageBox::Yes)
        {
            QProcess::startDetached(QApplication::applicationFilePath());
            QApplication::quit();
        }
    }
}

void MainWindow::openProfileManager8()
{
    auto *manager = new ProfileManager8();
    connect(manager, &ProfileManager8::updateProfiles, this, [=] { updateProfiles(8); });
    manager->show();
}

void MainWindow::openAbout()
{
    QStringList info
        = { QString("Version: %1").arg(POKEFINDER_VERSION), QString("Branch: %1").arg(GIT_BRANCH), QString("Commit: %1").arg(GIT_COMMIT) };

    QMessageBox about(QMessageBox::Information, tr("About"), info.join("\n"), QMessageBox::Close);
    about.exec();
}

void MainWindow::openEncounterLookup()
{
    auto *lookup = new EncounterLookup();
    lookup->show();
}

void MainWindow::openIVCalculator()
{
    auto *iv = new IVCalculator();
    iv->show();
}

void MainWindow::openResearcher()
{
    auto *r = new Researcher();
    r->show();
}

void MainWindow::openSettings()
{
    auto *s = new Settings();
    s->show();
}
