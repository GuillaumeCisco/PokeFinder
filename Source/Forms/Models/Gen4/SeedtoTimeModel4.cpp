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

#include "SeedtoTimeModel4.hpp"
#include <Core/Util/Utilities.hpp>
#include <Core/Enum/Game.hpp>

SeedtoTimeModel4::SeedtoTimeModel4(QObject *parent, bool flag, Game version) :
    TableModel<SeedTime>(parent), calibrate(flag), version(version)
{
}

void SeedtoTimeModel4::setFlags(bool flag, Game version)
{
    calibrate = flag;
    this->version = version;
    emit headerDataChanged(Qt::Horizontal, 0, columnCount());
}

int SeedtoTimeModel4::columnCount(const QModelIndex & /*parent*/) const
{
    if (calibrate)
    {
        return (version & Game::HGSS) != Game::None ? 5 : 4;
    }
    return 2;
}

QVariant SeedtoTimeModel4::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const auto &state = model[index.row()];
        if (calibrate)
        {
            switch (index.column())
            {
            case 0:
                return QString::number(state.getSeed(), 16).toUpper().toUpper().rightJustified(8, '0');
            case 1:
                return QString::fromStdString(state.getDateTime().toString());
            case 2:
                return state.getDelay();
            case 3:
                return QString::fromStdString(state.getSequence());
            case 4:
            {
                std::string str = state.getInfo().getRouteString();
                return str.empty() ? tr("No roamers") : QString::fromStdString(str);
            }
            }
        }
        else
        {
            switch (index.column())
            {
            case 0:
                return QString::fromStdString(state.getDateTime().toString());
            case 1:
                return state.getDelay();
            }
        }
    }
    return QVariant();
}

QVariant SeedtoTimeModel4::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (calibrate)
        {
            return (version & Game::HGSS) != Game::None ? header1[section] : header2[section];
        }
        return header3[section];
    }
    return QVariant();
}
