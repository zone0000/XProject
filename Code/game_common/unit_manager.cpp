#include "stdafx.h"
#include "unit_manager.h"

#include "unit.h"
#include "npc_unit.h"
#include "npc_unitdata.h"
#include "player_unit.h"
#include "player_unitdata.h"

namespace XP
{

UnitManager::UnitManager()
{
}

UnitManager::~UnitManager()
{
}

PlayerUnit* UnitManager::CreatePlayerUnit(PlayerUnitData&& playerUnitData)
{
    LOCK_W(_lock);

    uint32 id = 0;
    if (!_playerIdPool.Alloc(id))
    {
        LOG_ERROR(LOG_FILTER_UNIT, "Fail to create PlayerUnit. id allocation failed.");
        return nullptr;
    }

    UnitId unitId(PlayerUnit::unitType, id);

    PlayerUnit* pPlayerUnit = _playerUnitPool.construct(unitId, playerUnitData);
    if (pPlayerUnit)
    {
        return pPlayerUnit;
    }

    LOG_ERROR(LOG_FILTER_UNIT, "Fail to create PlayerUnit. {}", unitId.GetString());
    return nullptr;
}

NpcUnit* UnitManager::CreateNpcUnit(const NpcUnitData& npcUnitData)
{
    LOCK_W(_lock);

    uint32 id = 0;
    if (!_npcIdPool.Alloc(id))
    {
        LOG_ERROR(LOG_FILTER_UNIT, "Fail to create NpcUnit. id allocation failed.");
        return nullptr;
    }

    UnitId unitId(NpcUnit::unitType, id);

    NpcUnit* pNpcUnit = _npcUnitPool.construct(unitId, npcUnitData);
    if (pNpcUnit)
    {
        return pNpcUnit;
    }

    LOG_ERROR(LOG_FILTER_UNIT, "Fail to create NpcUnit. {}", unitId.GetString());
    return nullptr;
}

void UnitManager::ReleaseUnit(Unit* pUnit)
{
    ASSERT(pUnit);

    switch (pUnit->GetType())
    {
    case eUnitType::PLAYER_UNIT:
        {
            PlayerUnit* pPlayerUnit = static_cast<PlayerUnit*>(pUnit);
            ReleasePlayerUnit(pPlayerUnit);
        } break;
    case eUnitType::NPC_UNIT:
        {
            NpcUnit* pNpcUnit = static_cast<NpcUnit*>(pUnit);
            ReleaseNpcUnit(pNpcUnit);
        } break;
    default:
        ASSERT(false);
        break;
    }
}

void UnitManager::ReleasePlayerUnit(PlayerUnit* pPlayerUnit)
{
    if (!pPlayerUnit)
        return;

    LOCK_W(_lock);
    const auto& unitId = pPlayerUnit->GetUnitId();

    if (!_playerUnitPool.is_from(pPlayerUnit))
    {
        LOG_ERROR(LOG_FILTER_UNIT, "PlayerUnit is not contain from playerUnitPool. {}",
            unitId.GetString());
    }

    _playerIdPool.Free(unitId.GetId());
    _playerUnitPool.destroy(pPlayerUnit);
}

void UnitManager::ReleaseNpcUnit(NpcUnit* pNpcUnit)
{
    if (!pNpcUnit)
        return;

    LOCK_W(_lock);
    const auto& unitId = pNpcUnit->GetUnitId();

    if (!_npcUnitPool.is_from(pNpcUnit))
    {
        LOG_ERROR(LOG_FILTER_UNIT, "NpcUnit is not contain from npcUnitPool. {}",
            unitId.GetString());
    }

    _npcIdPool.Free(unitId.GetId());
    _npcUnitPool.destroy(pNpcUnit);
}

} // namespace XP