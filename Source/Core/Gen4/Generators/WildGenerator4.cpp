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

#include "WildGenerator4.hpp"
#include <Core/Enum/Encounter.hpp>
#include <Core/Enum/Lead.hpp>
#include <Core/Enum/Method.hpp>
#include <Core/Gen4/EncounterArea4.hpp>
#include <Core/Gen4/States/WildState4.hpp>
#include <Core/RNG/LCRNG.hpp>
#include <Core/Util/EncounterSlot.hpp>

WildGenerator4::WildGenerator4(u32 initialAdvances, u32 maxAdvances, u16 tid, u16 sid, u8 genderRatio, Method method,
                               const StateFilter &filter, bool platinum) :
    WildGenerator(initialAdvances, maxAdvances, tid, sid, genderRatio, method, filter), platinum(platinum)
{
}

std::vector<WildState4> WildGenerator4::generate(u32 seed, const EncounterArea4 &encounterArea) const
{
    switch (method)
    {
    case Method::MethodJ:
        return generateMethodJ(seed, encounterArea);
    case Method::MethodK:
        return generateMethodK(seed, encounterArea);
    case Method::ChainedShiny:
        return generateChainedShiny(seed, encounterArea);
    default:
        return std::vector<WildState4>();
    }
}

std::vector<WildState4> WildGenerator4::generateMethodJ(u32 seed, const EncounterArea4 &encounterArea) const
{
    std::vector<WildState4> states;

    PokeRNG rng(seed);
    rng.advance(initialAdvances + offset);

    u8 buffer = 0;
    u8 thresh = encounter == Encounter::OldRod ? 25 : encounter == Encounter::GoodRod ? 50 : encounter == Encounter::SuperRod ? 75 : 0;

    switch (lead)
    {
    case Lead::CuteCharmFemale:
        buffer = 0;
        break;
    case Lead::CuteCharm25M:
        buffer = 0xC8;
        break;
    case Lead::CuteCharm50M:
        buffer = 0x96;
        break;
    case Lead::CuteCharm75M:
        buffer = 0x4B;
        break;
    case Lead::CuteCharm875M:
        buffer = 0x32;
        break;
    default:
        break;
    }

    for (u32 cnt = 0; cnt <= maxAdvances; cnt++, rng.next())
    {
        WildState4 state(initialAdvances + cnt);

        u32 occidentary = initialAdvances + cnt;
        PokeRNG go(rng.getSeed(), &occidentary);

        u16 first = go.nextUShort<true>(); // Encounter slot call, nibble call for fishing

        switch (encounter)
        {
        case Encounter::Grass:
            state.setEncounterSlot(EncounterSlot::jSlot(first, encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot()));
            occidentary += platinum ? 1 : 5; // Compensate for the game's advances after the battle ends
            break;
        case Encounter::Surfing:
            state.setEncounterSlot(EncounterSlot::jSlot(first, encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot(), go.nextUShort<true>()));
            occidentary += platinum ? 1 : 5; // Compensate for the game's advances after the battle ends
            break;
        case Encounter::OldRod:
        case Encounter::GoodRod:
        case Encounter::SuperRod:
            if ((first / 656) >= thresh)
            {
                continue;
            }

            state.setEncounterSlot(EncounterSlot::jSlot(go.nextUShort<true>(), encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot(), go.nextUShort<true>()));
            occidentary += platinum ? 2 : 6; // Compensate for the game's advances after the battle ends
            break;
        default:
            break;
        }

        u32 pid = 0;
        switch (lead)
        {
        case Lead::None:
        case Lead::CompoundEyes:
            if (lead == Lead::CompoundEyes)
            {
                state.setLead(Lead::CompoundEyes);
            }
            // Get hunt nature
            state.setNature(go.nextUShort<true>() / 0xa3e);

            if (!filter.compareNature(state))
            {
                continue;
            }

            // Begin search for valid pid
            do
            {
                u16 low = go.nextUShort<true>();
                u16 high = go.nextUShort<true>();
                pid = static_cast<u32>((high << 16) | low);
            } while (pid % 25 != state.getNature());

            break;
        case Lead::Synchronize:
            if ((go.nextUShort<true>() >> 15) == 0) // Successful synch
            {
                state.setNature(synchNature);
            }
            else // Failed synch
            {
                state.setNature(go.nextUShort<true>() / 0xa3e);
            }

            if (!filter.compareNature(state))
            {
                continue;
            }

            // Begin search for valid pid
            do
            {
                u16 low = go.nextUShort<true>();
                u16 high = go.nextUShort<true>();
                pid = static_cast<u32>((high << 16) | low);
            } while (pid % 25 != state.getNature());

            break;
        default: // Default to cover all cute charm cases
            if ((go.nextUShort<true>() / 0x5556) != 0) // Successful cute charm
            {
                // Get nature
                state.setNature(go.nextUShort<true>() / 0xa3e);

                if (!filter.compareNature(state))
                {
                    continue;
                }

                // Cute charm doesn't hunt for a valid PID, just uses buffer and target nature
                pid = buffer + state.getNature();
            }
            else // Failed cute charm
            {
                // Get nature
                state.setNature(go.nextUShort<true>() / 0xa3e);

                if (!filter.compareNature(state))
                {
                    continue;
                }

                // Begin search for valid pid
                do
                {
                    u16 low = go.nextUShort<true>();
                    u16 high = go.nextUShort<true>();
                    pid = static_cast<u32>((high << 16) | low);
                } while (pid % 25 != state.getNature());
            }

            break;
        }

        state.setPID(pid);
        state.setAbility(pid & 1);
        state.setGender(pid & 255, genderRatio);
        state.setShiny<8>(tsv, (pid >> 16) ^ (pid & 0xffff));

        u16 iv1 = go.nextUShort<true>();
        u16 iv2 = go.nextUShort<true>();

        state.setIVs(iv1, iv2);
        state.calculateHiddenPower();

        u8 item = go.nextUShort<true>() % 100;

        state.setItem(item);

        if (filter.compareState(state))
        {
            state.setOccidentary(occidentary);
            state.setSeed(first);
            states.emplace_back(state);
        }
    }

    return states;
}

template <bool bug>
bool createPokemon(PokeRNG &go, WildState4 &state, u8 buffer, u8 synchNature, u8 genderRatio, u16 tsv, const Lead &lead,
                   const StateFilter &filter)
{
    u32 pid = 0;
    if constexpr (bug)
    {
        if (lead == Lead::CuteCharmFemale || lead == Lead::CuteCharm25M || lead == Lead::CuteCharm50M || lead == Lead::CuteCharm75M
            || lead == Lead::CuteCharm875M)
        {
            if ((go.nextUShort<true>() % 3) != 0) // Successfull cute charm
            {
                // Get hunt nature
                state.setNature(go.nextUShort<true>() % 25);

                if (!filter.compareNature(state))
                {
                    return false;
                }

                pid = buffer + state.getNature();

                state.setPID(pid);

                u16 iv1 = go.nextUShort<true>();
                u16 iv2 = go.nextUShort<true>();

                state.setIVs(iv1, iv2);

                return true;
            }
        }

        for (u8 loop = 0; loop < 4; loop++)
        {
            switch (lead)
            {
            case Lead::Synchronize:
                if ((go.nextUShort<true>() & 1) == 0) // Successful synch
                {
                    state.setNature(synchNature);
                }
                else // Failed synch
                {
                    state.setNature(go.nextUShort<true>() % 25);
                }
                break;
            case Lead::CompoundEyes:
                state.setLead(Lead::CompoundEyes);
            default: // Falls through to set nature for all cases other than cute charm and synch
                state.setNature(go.nextUShort<true>() % 25);
                break;
            }

            // Begin search for valid pid
            do
            {
                u16 low = go.nextUShort<true>();
                u16 high = go.nextUShort<true>();
                pid = static_cast<u32>((high << 16) | low);
            } while (pid % 25 != state.getNature());

            state.setPID(pid);

            u16 iv1 = go.nextUShort<true>();
            u16 iv2 = go.nextUShort<true>();

            state.setIVs(iv1, iv2);

            for (u8 iv = 0; iv < 6; iv++)
            {
                if (state.getIV(iv) == 31)
                {
                    return true;
                }
            }
        }

        if (!filter.compareNature(state))
        {
            return false;
        }
    }
    else
    {
        switch (lead)
        {
        case Lead::Synchronize:
            if ((go.nextUShort<true>() & 1) == 0) // Successful synch
            {
                state.setNature(synchNature);
            }
            else // Failed synch
            {
                state.setNature(go.nextUShort<true>() % 25);
            }

            if (!filter.compareNature(state))
            {
                return false;
            }

            // Begin search for valid pid
            do
            {
                u16 low = go.nextUShort<true>();
                u16 high = go.nextUShort<true>();
                pid = static_cast<u32>((high << 16) | low);
            } while (pid % 25 != state.getNature());
            break;
        case Lead::CuteCharmFemale:
        case Lead::CuteCharm25M:
        case Lead::CuteCharm50M:
        case Lead::CuteCharm75M:
        case Lead::CuteCharm875M:
            if ((go.nextUShort<true>() % 3) != 0) // Successfull cute charm
            {
                // Get hunt nature
                state.setNature(go.nextUShort<true>() % 25);

                if (!filter.compareNature(state))
                {
                    return false;
                }

                pid = buffer + state.getNature();
            }
            else
            {
                state.setNature(go.nextUShort<true>() % 25);

                if (!filter.compareNature(state))
                {
                    return false;
                }

                // Begin search for valid pid
                do
                {
                    u16 low = go.nextUShort<true>();
                    u16 high = go.nextUShort<true>();
                    pid = static_cast<u32>((high << 16) | low);
                } while (pid % 25 != state.getNature());
            }
            break;
        case Lead::CompoundEyes:
            state.setLead(Lead::CompoundEyes);
        default: // Falls through to set nature for all cases other than cute charm and synch
            state.setNature(go.nextUShort<true>() % 25);

            if (!filter.compareNature(state))
            {
                return false;
            }

            // Begin search for valid pid
            do
            {
                u16 low = go.nextUShort<true>();
                u16 high = go.nextUShort<true>();
                pid = static_cast<u32>((high << 16) | low);
            } while (pid % 25 != state.getNature());
            break;
        }

        state.setPID(pid);

        u16 iv1 = go.nextUShort<true>();
        u16 iv2 = go.nextUShort<true>();

        state.setIVs(iv1, iv2);
    }

    return true;
}

std::vector<WildState4> WildGenerator4::generateMethodK(u32 seed, const EncounterArea4 &encounterArea) const
{
    std::vector<WildState4> states;

    PokeRNG rng(seed);
    rng.advance(initialAdvances + offset);

    u8 buffer = 0;
    u16 rate = encounterArea.getRate();
    if (lead == Lead::SuctionCups
        && (encounter == Encounter::OldRod || encounter == Encounter::GoodRod || encounter == Encounter::SuperRod))
    {
        rate <<= 1;
    }

    switch (lead)
    {
    case Lead::CuteCharmFemale:
        buffer = 0;
        break;
    case Lead::CuteCharm25M:
        buffer = 0xC8;
        break;
    case Lead::CuteCharm50M:
        buffer = 0x96;
        break;
    case Lead::CuteCharm75M:
        buffer = 0x4B;
        break;
    case Lead::CuteCharm875M:
        buffer = 0x32;
        break;
    default:
        break;
    }

    for (u32 cnt = 0; cnt <= maxAdvances; cnt++, rng.next())
    {
        WildState4 state(initialAdvances + cnt);

        u32 occidentary = initialAdvances + cnt;
        PokeRNG go(rng.getSeed(), &occidentary);

        u16 first = go.nextUShort<true>(); // Encounter slot, nibble for fishing, nibble for rock smash

        switch (encounter)
        {
        case Encounter::Grass:
            state.setEncounterSlot(EncounterSlot::kSlot(first, encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot()));
            occidentary += 1; // Compensate for the game's advances after the battle ends
            if (!createPokemon<false>(go, state, buffer, synchNature, genderRatio, tsv, lead, filter))
            {
                continue;
            }
            break;
        case Encounter::Surfing:
            state.setEncounterSlot(EncounterSlot::kSlot(first, encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot(), go.nextUShort<true>()));
            occidentary += 1; // Compensate for the game's advances after the battle ends
            if (!createPokemon<false>(go, state, buffer, synchNature, genderRatio, tsv, lead, filter))
            {
                continue;
            }
            break;
        case Encounter::OldRod:
        case Encounter::GoodRod:
        case Encounter::SuperRod:
            if ((first % 100) >= rate)
            {
                continue;
            }

            state.setEncounterSlot(EncounterSlot::kSlot(go.nextUShort<true>(), encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot()));
            occidentary += 2; // Compensate for the game's advances after the battle ends
            go.next();
            if (!createPokemon<false>(go, state, buffer, synchNature, genderRatio, tsv, lead, filter))
            {
                continue;
            }
            break;
        case Encounter::RockSmash:
            if ((first % 100) >= rate)
            {
                continue;
            }

            state.setEncounterSlot(EncounterSlot::kSlot(go.nextUShort<true>(), encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot(), go.nextUShort<true>()));
            occidentary += 1; // Compensate for the game's advances after the battle ends
            if (!createPokemon<false>(go, state, buffer, synchNature, genderRatio, tsv, lead, filter))
            {
                continue;
            }
            break;
        case Encounter::BugCatchingContest:
            state.setEncounterSlot(EncounterSlot::kSlot(first, encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot(), go.nextUShort<true>()));
            if (!createPokemon<true>(go, state, buffer, synchNature, genderRatio, tsv, lead, filter))
            {
                continue;
            }
            break;
        case Encounter::Headbutt:
            state.setEncounterSlot(EncounterSlot::kSlot(first, encounter));
            if (!filter.compareEncounterSlot(state))
            {
                continue;
            }

            state.setLevel(encounterArea.calcLevel(state.getEncounterSlot(), go.nextUShort<true>()));
            occidentary += 1; // Compensate for the game's advances after the battle ends
            if (!createPokemon<false>(go, state, buffer, synchNature, genderRatio, tsv, lead, filter))
            {
                continue;
            }
            break;
        default:
            break;
        }

        u32 pid = state.getPID();

        state.setAbility(pid & 1);
        state.setGender(pid & 255, genderRatio);
        state.setShiny<8>(tsv, (pid >> 16) ^ (pid & 0xffff));
        state.calculateHiddenPower();

        u8 item = go.nextUShort<true>() % 100;

        state.setItem(item);

        if (filter.compareState(state))
        {
            state.setOccidentary(occidentary);
            state.setSeed(first);
            states.emplace_back(state);
        }
    }

    return states;
}

std::vector<WildState4> WildGenerator4::generateChainedShiny(u32 seed, const EncounterArea4 &encounterArea) const
{
    std::vector<WildState4> states;

    PokeRNG rng(seed);
    rng.advance(initialAdvances + offset);

    for (u32 cnt = 0; cnt <= maxAdvances; cnt++, rng.next())
    {
        WildState4 state(initialAdvances + cnt);

        PokeRNG go(rng.getSeed());
        u16 first = go.nextUShort();

        u16 low = first & 7;
        u16 high = go.nextUShort() & 7;

        for (int i = 3; i < 16; i++)
        {
            low |= (go.nextUShort() & 1) << i;
        }
        high |= (low ^ tid ^ sid) & 0xfff8;

        u16 iv1 = go.nextUShort();
        u16 iv2 = go.nextUShort();

        state.setPID(high, low);
        state.setAbility(low & 1);
        state.setGender(low & 255, genderRatio);
        state.setNature(state.getPID() % 25);
        state.setShiny<8>(tsv, high ^ low);

        state.setIVs(iv1, iv2);
        state.calculateHiddenPower();

        if (filter.compareState(state))
        {
            state.setSeed(first);
            states.emplace_back(state);
        }
    }

    return states;
}
