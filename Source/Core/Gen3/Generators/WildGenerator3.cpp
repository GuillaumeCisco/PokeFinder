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

#include "WildGenerator3.hpp"
#include <Core/Enum/Encounter.hpp>
#include <Core/Enum/Game.hpp>
#include <Core/Enum/Lead.hpp>
#include <Core/Enum/Method.hpp>
#include <Core/Gen3/EncounterArea3.hpp>
#include <Core/Parents/States/WildState.hpp>
#include <Core/RNG/LCRNG.hpp>
#include <Core/Util/EncounterSlot.hpp>

WildGenerator3::WildGenerator3(u32 initialAdvances, u32 maxAdvances, u16 tid, u16 sid, u8 genderRatio, Method method,
                               const StateFilter &filter, Game version) :
    WildGenerator(initialAdvances, maxAdvances, tid, sid, genderRatio, method, filter), version(version)
{
}

std::vector<WildState> WildGenerator3::generate(u32 seed, const EncounterArea3 &encounterArea) const
{
    std::vector<WildState> states;

    PokeRNG rng(seed);
    rng.advance(initialAdvances + offset);

    u16 rate = encounterArea.getRate() * 16;
    // RockSmash/Surfing/Fishing encounters have different rng calls inside RSE Safari Zone,
    // so we set a flag to check if we're searching these kind of spreads
    bool rseSafari = encounterArea.rseSafariZone() && (version & Game::RSE) != Game::None;
    bool rock = (version & Game::FRLG) != Game::None;

    bool cuteCharmFlag = false;
    bool (*cuteCharm)(u32);
    switch (lead)
    {
    case Lead::CuteCharm125F:
        cuteCharm = [](u32 pid) { return (pid & 0xff) < 31; };
        break;
    case Lead::CuteCharm875M:
        cuteCharm = [](u32 pid) { return (pid & 0xff) >= 31; };
        break;
    case Lead::CuteCharm25F:
        cuteCharm = [](u32 pid) { return (pid & 0xff) < 63; };
        break;
    case Lead::CuteCharm75M:
        cuteCharm = [](u32 pid) { return (pid & 0xff) >= 63; };
        break;
    case Lead::CuteCharm50F:
        cuteCharm = [](u32 pid) { return (pid & 0xff) < 127; };
        break;
    case Lead::CuteCharm50M:
        cuteCharm = [](u32 pid) { return (pid & 0xff) >= 127; };
        break;
    case Lead::CuteCharm75F:
        cuteCharm = [](u32 pid) { return (pid & 0xff) < 191; };
        break;
    case Lead::CuteCharm25M:
        cuteCharm = [](u32 pid) { return (pid & 0xff) >= 191; };
        break;
    default:
        cuteCharm = [](u32 pid) {
            (void)pid;
            return true;
        };
    }

    for (u32 cnt = 0; cnt <= maxAdvances; cnt++, rng.next())
    {
        WildState state(initialAdvances + cnt);
        PokeRNG go(rng.getSeed());

        switch (encounter)
        {
        case Encounter::RockSmash:
            if (rseSafari || !rock) // account RockSmash extra rng call inside RSE Safari Zone
            {
                go.next();
            }
            if ((go.nextUShort() % 2880) >= rate)
            {
                continue;
            }

            state.setEncounterSlot(EncounterSlot::hSlot(go.nextUShort(), encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot(), go.nextUShort()));
            if (rseSafari) // account RockSmash extra rng call inside RSE Safari Zone
            {
                go.advance(1);
            }

            break;
        case Encounter::Grass:
            go.next();
            state.setEncounterSlot(EncounterSlot::hSlot(go.nextUShort(), encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot()));
            go.advance(rseSafari ? 2 : 1);
            break;
        case Encounter::Surfing:
        case Encounter::OldRod:
        case Encounter::GoodRod:
        case Encounter::SuperRod:
            if (!rseSafari) // account Surfing/Fishing extra rng call outside RSE Safari Zone
            {
                go.next();
            }
            state.setEncounterSlot(EncounterSlot::hSlot(go.nextUShort(), encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot(), go.nextUShort()));
            if (rseSafari) // account Surfing/Fishing extra rng call inside RSE Safari Zone
            {
                go.next();
            }
            break;
        default:
            break;
        }

        if (lead == Lead::None)
        {
            state.setNature(go.nextUShort() % 25);
        }
        else if (lead == Lead::Synchronize)
        {
            if ((go.nextUShort() & 1) == 0) // state is synchable so set nature to synch nature
            {
                state.setNature(synchNature);
            }
            else // Synch failed so grab hunt nature from next RNG call
            {
                state.setNature(go.nextUShort() % 25);
            }
        }
        else // Covers cutecharm
        {
            cuteCharmFlag = go.nextUShort() % 3 > 0;
            state.setNature(go.nextUShort() % 25);
        }

        if (!filter.compareNature(state))
        {
            continue;
        }

        // Now search for a Method 124 PID that matches our hunt nature
        u32 pid;
        do
        {
            u16 low = go.nextUShort();
            u16 high = go.nextUShort();
            pid = (high << 16) | low;
        } while (pid % 25 != state.getNature() || (cuteCharmFlag && !cuteCharm(pid)));

        state.setPID(pid);
        state.setAbility(pid & 1);
        state.setGender(pid & 255, genderRatio);
        state.setShiny<8>(tsv, (pid & 0xffff) ^ (pid >> 16));

        // Valid PID is found now time to generate IVs
        u16 iv1;
        u16 iv2;
        if (method == Method::MethodH1)
        {
            iv1 = go.nextUShort();
            iv2 = go.nextUShort();
        }
        else if (method == Method::MethodH2)
        {
            go.next();
            iv1 = go.nextUShort();
            iv2 = go.nextUShort();
        }
        else
        {
            iv1 = go.nextUShort();
            go.next();
            iv2 = go.nextUShort();
        }
        state.setIVs(iv1, iv2);
        state.calculateHiddenPower();

        if (filter.compareState(state))
        {
            states.emplace_back(state);
        }
    }

    return states;
}
