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

#include "Eggs4.hpp"
#include "ui_Eggs4.h"
#include <Core/Enum/Method.hpp>
#include <Core/Gen4/Generators/EggGenerator4.hpp>
#include <Core/Gen4/Profile4.hpp>
#include <Core/Gen4/Searchers/EggSearcher4.hpp>
#include <Core/Parents/ProfileLoader.hpp>
#include <Core/Util/Translator.hpp>
#include <Forms/Controls/Controls.hpp>
#include <Forms/Gen4/Profile/ProfileManager4.hpp>
#include <Forms/Gen4/Tools/Poketch.hpp>
#include <Forms/Gen4/Tools/SeedtoTime4.hpp>
#include <Forms/Models/Gen4/EggModel4.hpp>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QThread>
#include <QTimer>

Eggs4::Eggs4(QWidget *parent) : QWidget(parent), ui(new Ui::Eggs4)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setupModels();
}

Eggs4::~Eggs4()
{
    QSettings setting;
    setting.beginGroup("eggs4");
    setting.setValue("minDelay", ui->textBoxSearcherMinDelay->text());
    setting.setValue("maxDelay", ui->textBoxSearcherMaxDelay->text());
    setting.setValue("minAdvanceIV", ui->textBoxSearcherIVMinAdvance->text());
    setting.setValue("maxAdvanceIV", ui->textBoxSearcherIVMaxAdvance->text());
    setting.setValue("minAdvancePID", ui->textBoxSearcherPIDMinAdvance->text());
    setting.setValue("maxAdvancePID", ui->textBoxSearcherPIDMaxAdvance->text());
    setting.setValue("profile", ui->comboBoxProfiles->currentIndex());
    setting.setValue("geometry", this->saveGeometry());
    setting.endGroup();

    delete ui;
}

void Eggs4::updateProfiles()
{
    profiles = ProfileLoader4::getProfiles();
    profiles.insert(profiles.begin(), Profile4());

    ui->comboBoxProfiles->clear();
    for (const auto &profile : profiles)
    {
        ui->comboBoxProfiles->addItem(QString::fromStdString(profile.getName()));
    }

    QSettings setting;
    int val = setting.value("eggs4/profile", 0).toInt();
    if (val < ui->comboBoxProfiles->count())
    {
        ui->comboBoxProfiles->setCurrentIndex(val);
    }
}

void Eggs4::setupModels()
{
    generatorModel = new EggGeneratorModel4(ui->tableViewGenerator, Method::DPPtIVs);
    searcherModel = new EggSearcherModel4(ui->tableViewSearcher, Method::DPPtIVs);

    generatorMenu = new QMenu(ui->tableViewGenerator);
    searcherMenu = new QMenu(ui->tableViewSearcher);

    ui->tableViewGenerator->setModel(generatorModel);
    ui->tableViewSearcher->setModel(searcherModel);

    ui->textBoxGeneratorInitialAdvances->setValues(InputType::Advance32Bit);
    ui->textBoxGeneratorMaxAdvances->setValues(InputType::Advance32Bit);
    ui->textBoxGeneratorSeed->setValues(InputType::Seed32Bit);

    ui->textBoxSearcherIVMinAdvance->setValues(InputType::Advance32Bit);
    ui->textBoxSearcherIVMaxAdvance->setValues(InputType::Advance32Bit);
    ui->textBoxSearcherPIDMinAdvance->setValues(InputType::Advance32Bit);
    ui->textBoxSearcherPIDMaxAdvance->setValues(InputType::Advance32Bit);
    ui->textBoxSearcherMinDelay->setValues(InputType::Advance32Bit);
    ui->textBoxSearcherMaxDelay->setValues(InputType::Advance32Bit);

    ui->comboBoxGeneratorMethod->setup({ toInt(Method::DPPtIVs), toInt(Method::Gen4Normal) });

    ui->filterGenerator->disableControls(Controls::EncounterSlots);
    ui->filterSearcher->disableControls(Controls::EncounterSlots | Controls::UseDelay | Controls::DisableFilter);

    ui->eggSettingsGenerator->setup(Game::Gen4);
    ui->eggSettingsSearcher->setup(Game::Gen4);

    QAction *outputTXTGenerator = generatorMenu->addAction(tr("Output Results to TXT"));
    QAction *outputCSVGenerator = generatorMenu->addAction(tr("Output Results to CSV"));
    calcPoketchGenerator = generatorMenu->addAction(tr("Calculate Poketch"));
    calcPoketchGenerator->setVisible(false);
    connect(outputTXTGenerator, &QAction::triggered, this, [=] { ui->tableViewGenerator->outputModel(); });
    connect(outputCSVGenerator, &QAction::triggered, this, [=] { ui->tableViewGenerator->outputModel(true); });
    connect(calcPoketchGenerator, &QAction::triggered, this, &Eggs4::calcPoketch);

    QAction *seedToTime = searcherMenu->addAction(tr("Generate times for seed"));
    QAction *outputTXTSearcher = searcherMenu->addAction(tr("Output Results to TXT"));
    QAction *outputCSVSearcher = searcherMenu->addAction(tr("Output Results to CSV"));
    connect(seedToTime, &QAction::triggered, this, &Eggs4::seedToTime);
    connect(outputTXTSearcher, &QAction::triggered, this, [=] { ui->tableViewSearcher->outputModel(); });
    connect(outputCSVSearcher, &QAction::triggered, this, [=] { ui->tableViewSearcher->outputModel(true); });

    connect(ui->comboBoxProfiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Eggs4::profilesIndexChanged);
    connect(ui->pushButtonGenerate, &QPushButton::clicked, this, &Eggs4::generate);
    connect(ui->pushButtonSearch, &QPushButton::clicked, this, &Eggs4::search);
    connect(ui->tableViewGenerator, &QTableView::customContextMenuRequested, this, &Eggs4::tableViewGeneratorContextMenu);
    connect(ui->tableViewSearcher, &QTableView::customContextMenuRequested, this, &Eggs4::tableViewSearcherContextMenu);
    connect(ui->pushButtonProfileManager, &QPushButton::clicked, this, &Eggs4::profileManager);
    connect(ui->eggSettingsGenerator, &EggSettings::toggleInheritance, generatorModel, &EggGeneratorModel4::toggleInheritance);
    connect(ui->eggSettingsSearcher, &EggSettings::toggleInheritance, searcherModel, &EggSearcherModel4::toggleInheritance);
    connect(ui->comboBoxGeneratorMethod, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Eggs4::generatorMethodChanged);

    updateProfiles();

    QSettings setting;
    setting.beginGroup("eggs4");
    if (setting.contains("minDelay"))
    {
        ui->textBoxSearcherMinDelay->setText(setting.value("minDelay").toString());
    }
    if (setting.contains("maxDelay"))
    {
        ui->textBoxSearcherMaxDelay->setText(setting.value("maxDelay").toString());
    }
    if (setting.contains("minAdvanceIV"))
    {
        ui->textBoxSearcherIVMinAdvance->setText(setting.value("minAdvanceIV").toString());
    }
    if (setting.contains("maxAdvanceIV"))
    {
        ui->textBoxSearcherIVMaxAdvance->setText(setting.value("maxAdvanceIV").toString());
    }
    if (setting.contains("minAdvancePID"))
    {
        ui->textBoxSearcherPIDMinAdvance->setText(setting.value("minAdvancePID").toString());
    }
    if (setting.contains("maxAdvancePID"))
    {
        ui->textBoxSearcherPIDMaxAdvance->setText(setting.value("maxAdvancePID").toString());
    }
    if (setting.contains("geometry"))
    {
        this->restoreGeometry(setting.value("geometry").toByteArray());
    }
    setting.endGroup();
}

void Eggs4::generate()
{
    if (!ui->eggSettingsGenerator->compatibleParents())
    {
        QMessageBox box(QMessageBox::Warning, tr("Incompatible Parents"), tr("Gender of selected parents are not compatible for breeding"));
        box.exec();
        return;
    }

    generatorModel->clearModel();

    u32 initialAdvances = ui->textBoxGeneratorInitialAdvances->getUInt();
    u32 maxAdvances = ui->textBoxGeneratorMaxAdvances->getUInt();
    u32 seed = ui->textBoxGeneratorSeed->getUInt();
    u16 tid = currentProfile->getTID();
    u16 sid = currentProfile->getSID();

    auto method = static_cast<Method>(ui->comboBoxGeneratorMethod->currentData().toInt());
    Daycare daycare = ui->eggSettingsGenerator->getDaycareSettings();
    if (method == Method::Gen4Normal)
    {
        if (daycare.getMasuda())
        {
            method = Method::Gen4Masuda;
        }
    }
    else
    {
        Game version = currentProfile->getVersion();
        method = (version & Game::HGSS) != Game::None ? Method::HGSSIVs : Method::DPPtIVs;
    }
    generatorModel->setMethod(method);

    StateFilter filter(ui->filterGenerator->getGender(), ui->filterGenerator->getAbility(), ui->filterGenerator->getShiny(),
                       ui->filterGenerator->getDisableFilters(), ui->filterGenerator->getMinIVs(), ui->filterGenerator->getMaxIVs(),
                       ui->filterGenerator->getNatures(), ui->filterGenerator->getHiddenPowers(), {});

    EggGenerator4 generator(initialAdvances, maxAdvances, tid, sid, ui->filterGenerator->getGenderRatio(), method, filter, daycare);

    auto states = generator.generate(seed);
    generatorModel->addItems(states);
}

void Eggs4::search()
{
    if (!ui->eggSettingsSearcher->compatibleParents())
    {
        QMessageBox box(QMessageBox::Warning, tr("Incompatible Parents"), tr("Gender of selected parents are not compatible for breeding"));
        box.exec();
        return;
    }

    searcherModel->clearModel();

    Method methodModel;
    Daycare daycare = ui->eggSettingsSearcher->getDaycareSettings();
    switch (ui->comboBoxSearcherMethod->currentIndex())
    {
    case 0:
        methodModel = (currentProfile->getVersion() & Game::HGSS) != Game::None ? Method::HGSSIVs : Method::DPPtIVs;
        break;
    case 1:
        methodModel = daycare.getMasuda() ? Method::Gen4Masuda : Method::Gen4Normal;
        break;
    case 2:
        methodModel = Method::Gen4Combined;
        break;
    }
    searcherModel->setMethod(methodModel);

    ui->pushButtonSearch->setEnabled(false);
    ui->pushButtonCancel->setEnabled(true);

    u16 tid = currentProfile->getTID();
    u16 sid = currentProfile->getSID();
    u8 genderRatio = ui->filterSearcher->getGenderRatio();

    StateFilter filter(ui->filterSearcher->getGender(), ui->filterSearcher->getAbility(), ui->filterSearcher->getShiny(), false,
                       ui->filterSearcher->getMinIVs(), ui->filterSearcher->getMaxIVs(), ui->filterSearcher->getNatures(),
                       ui->filterSearcher->getHiddenPowers(), {});

    u32 minDelay = ui->textBoxSearcherMinDelay->getUInt();
    u32 maxDelay = ui->textBoxSearcherMaxDelay->getUInt();
    u32 minAdvanceIV = ui->textBoxSearcherIVMinAdvance->getUInt();
    u32 maxAdvanceIV = ui->textBoxSearcherIVMaxAdvance->getUInt();
    u32 minAdvancePID = ui->textBoxSearcherPIDMinAdvance->getUInt();
    u32 maxAdvancePID = ui->textBoxSearcherPIDMaxAdvance->getUInt();

    Method methodIV = (currentProfile->getVersion() & Game::HGSS) != Game::None ? Method::HGSSIVs : Method::DPPtIVs;
    EggGenerator4 generatorIV(minAdvanceIV, maxAdvanceIV, tid, sid, genderRatio, methodIV, filter, daycare);

    Method methodPID = daycare.getMasuda() ? Method::Gen4Masuda : Method::Gen4Normal;
    EggGenerator4 generatorPID(minAdvancePID, maxAdvancePID, tid, sid, genderRatio, methodPID, filter, daycare);

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(static_cast<int>(256 * 24 * (maxDelay - minDelay + 1)));

    auto *searcher = new EggSearcher4(tid, sid, genderRatio, methodModel, filter);

    auto *thread = QThread::create(
        [=] { searcher->startSearch(minDelay, maxDelay, ui->comboBoxSearcherMethod->currentIndex(), generatorIV, generatorPID); });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(ui->pushButtonCancel, &QPushButton::clicked, [searcher] { searcher->cancelSearch(); });

    auto *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, [=] {
        searcherModel->addItems(searcher->getResults());
        ui->progressBar->setValue(searcher->getProgress());
    });
    connect(thread, &QThread::finished, timer, &QTimer::stop);
    connect(thread, &QThread::finished, timer, &QTimer::deleteLater);
    connect(timer, &QTimer::destroyed, this, [=] {
        ui->pushButtonSearch->setEnabled(true);
        ui->pushButtonCancel->setEnabled(false);
        searcherModel->addItems(searcher->getResults());
        ui->progressBar->setValue(searcher->getProgress());
        delete searcher;
    });

    thread->start();
    timer->start(1000);
}

void Eggs4::profilesIndexChanged(int index)
{
    if (index >= 0)
    {
        currentProfile = &profiles[index];

        ui->labelProfileTIDValue->setText(QString::number(currentProfile->getTID()));
        ui->labelProfileSIDValue->setText(QString::number(currentProfile->getSID()));
        ui->labelProfileGameValue->setText(QString::fromStdString(currentProfile->getVersionString()));

        auto method = static_cast<Method>(ui->comboBoxGeneratorMethod->currentData().toInt());
        if (method == Method::Gen4Normal
            && (currentProfile->getVersion() == Game::Diamond || currentProfile->getVersion() == Game::Pearl
                || currentProfile->getVersion() == Game::Platinum))
        {
            calcPoketchGenerator->setVisible(true);
        }
        else
        {
            calcPoketchGenerator->setVisible(false);
        }
    }
}

void Eggs4::tableViewGeneratorContextMenu(QPoint pos)
{
    if (generatorModel->rowCount() > 0)
    {
        generatorMenu->popup(ui->tableViewGenerator->viewport()->mapToGlobal(pos));
    }
}

void Eggs4::tableViewSearcherContextMenu(QPoint pos)
{
    if (searcherModel->rowCount() > 0)
    {
        searcherMenu->popup(ui->tableViewSearcher->viewport()->mapToGlobal(pos));
    }
}

void Eggs4::seedToTime()
{
    QModelIndex index = ui->tableViewSearcher->currentIndex();
    QString seed = searcherModel->data(searcherModel->index(index.row(), 0)).toString();

    auto *time = new SeedtoTime4(seed, currentProfile->getVersion());
    time->show();
}

void Eggs4::calcPoketch()
{
    QModelIndex index = ui->tableViewGenerator->currentIndex();
    int advances = generatorModel->data(generatorModel->index(index.row(), 0)).toInt();

    auto *poketch = new Poketch(advances);
    poketch->show();
}

void Eggs4::profileManager()
{
    auto *manager = new ProfileManager4();
    connect(manager, &ProfileManager4::updateProfiles, this, [=] { emit alertProfiles(4); });
    manager->show();
}

void Eggs4::generatorMethodChanged(int index)
{
    auto method = static_cast<Method>(ui->comboBoxGeneratorMethod->currentData().toInt());
    if (method == Method::Gen4Normal && (currentProfile->getVersion() & Game::DPPt) != Game::None)
    {
        calcPoketchGenerator->setVisible(true);
    }
    else
    {
        calcPoketchGenerator->setVisible(false);
    }
}
