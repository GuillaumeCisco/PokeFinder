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

#include "EncounterArea4.hpp"
#include <Core/Parents/Slot.hpp>

EncounterArea4::EncounterArea4(u8 location, u8 rate, Encounter type, const std::vector<Slot> &pokemon) :
    EncounterArea(location, rate, type, pokemon)
{
}

u8 EncounterArea4::calcLevel(u8 index, u16 prng) const
{
    return (prng % (pokemon[index].getMaxLevel() - pokemon[index].getMinLevel() + 1)) + pokemon[index].getMinLevel();
}

u8 EncounterArea4::calcLevel(u8 index) const
{
    return pokemon[index].getMinLevel();
}
