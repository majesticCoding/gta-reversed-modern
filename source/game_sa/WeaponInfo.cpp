/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "WeaponInfo.h"

void CWeaponInfo::InjectHooks() {
    RH_ScopedClass(CWeaponInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(FindWeaponFireType, 0x5BCF30);
    RH_ScopedInstall(LoadWeaponData, 0x5BE670);
    RH_ScopedInstall(Initialise, 0x5BF750);
    RH_ScopedInstall(Shutdown, 0x743C50);
    RH_ScopedOverloadedInstall(GetWeaponInfo, "original", 0x743C60, CWeaponInfo*(*)(eWeaponType, eWeaponSkill));
    RH_ScopedInstall(GetSkillStatIndex, 0x743CD0);
    RH_ScopedInstall(FindWeaponType, 0x743D10);
    RH_ScopedInstall(GetCrouchReloadAnimationID, 0x685700);
    RH_ScopedInstall(GetTargetHeadRange, 0x743D50);
    RH_ScopedInstall(GetWeaponReloadTime, 0x743D70);
}

// 0x5BF750
void CWeaponInfo::Initialise() {
    for (auto& info : aWeaponInfo) {
        info.m_vecFireOffset = CVector();
        info.m_nWeaponFire = WEAPON_FIRE_MELEE;
        info.m_fTargetRange = 0.0f;
        info.m_fWeaponRange = 0.0f;
        info.m_nModelId1 = MODEL_INVALID;
        info.m_nModelId2 = MODEL_INVALID;
        info.m_nSlot = -1;
        info.m_eAnimGroup = ANIM_GROUP_DEFAULT;
        info.m_nAmmoClip = 0;
        info.m_nSkillLevel = 1;
        info.m_nReqStatLevel = 0;
        info.m_fAccuracy = 1.0f;
        info.m_fMoveSpeed = 1.0f;
        info.m_fAnimLoopStart = 0.0f;
        info.m_fAnimLoopEnd = 0.0f;
        info.m_fAnimLoopFire = 0.0f;
        info.m_fAnimLoop2Start = 0.0f;
        info.m_fAnimLoop2End = 0.0f;
        info.m_fAnimLoop2Fire = 0.0f;
        info.m_fBreakoutTime = 0.0f;
        info.m_fSpeed = 0.0f;
        info.m_fRadius = 0.0f;
        info.m_fLifespan = 0.0f;
        info.m_fSpread = 0.0f;
        info.m_nAimOffsetIndex = 0;
        info.m_nFlags = 0;
        info.m_nBaseCombo = 4;
        info.m_nNumCombos = 1;
    }

    for (auto& offset : g_GunAimingOffsets) {
        offset.AimX = 0.0f;
        offset.AimZ = 0.0f;
        offset.DuckX = 0.0f;
        offset.DuckZ = 0.0f;
        offset.RLoadA = 0;
        offset.RLoadB = 0;
        offset.CrouchRLoadA = 0;
    }

    LoadWeaponData();
}

// 0x743C50
void CWeaponInfo::Shutdown() {
    // NOP
}

// 0x5BCF30
eWeaponFire CWeaponInfo::FindWeaponFireType(const char* name) {
    static constexpr struct { std::string_view name; eWeaponFire wf; } mapping[]{
        { "MELEE",       WEAPON_FIRE_MELEE       },
        { "INSTANT_HIT", WEAPON_FIRE_INSTANT_HIT },
        { "PROJECTILE",  WEAPON_FIRE_PROJECTILE  },
        { "AREA_EFFECT", WEAPON_FIRE_AREA_EFFECT },
        { "CAMERA",      WEAPON_FIRE_CAMERA      },
        { "USE",         WEAPON_FIRE_USE         },
    };
    if (const auto it = rng::find(mapping, name, [](const auto& e) { return e.name; }); it != std::end(mapping))
        return it->wf;

    return WEAPON_FIRE_INSTANT_HIT;
}

// Check if weapon has skill stats
// NOTSA
bool CWeaponInfo::WeaponHasSkillStats(eWeaponType type) {
    return type >= WEAPON_PISTOL && type <= WEAPON_TEC9;
}

// Get weapon info index for this type and with this skill
// NOTSA
uint32 CWeaponInfo::GetWeaponInfoIndex(eWeaponType weaponType, eWeaponSkill skill) {
    const auto numWeaponsWithSkill = (WEAPON_TEC9 - WEAPON_PISTOL) + 1;
    switch (skill) {
    case eWeaponSkill::POOR: return (uint32)weaponType + 25u + 0 * numWeaponsWithSkill;
    case eWeaponSkill::STD:  return (uint32)weaponType;
    case eWeaponSkill::PRO:  return (uint32)weaponType + 25u + 1 * numWeaponsWithSkill;
    case eWeaponSkill::COP:  return (uint32)weaponType + 25u + 2 * numWeaponsWithSkill;
    default:                 NOTSA_UNREACHABLE("Invalid weapon skill");
    }
}

// NOTSA
void CWeaponInfo::StreamModelsForWeapon(eStreamingFlags streamingFlags) {
    for (auto modelId : GetModels()) {
        if (modelId != MODEL_INVALID) {
            CStreaming::RequestModel(modelId, streamingFlags | STREAMING_PRIORITY_REQUEST);
        }
    }
    CStreaming::LoadAllRequestedModels(true);
}

// NOTSA
auto GetBaseComboByName(const char* name) {
    static constexpr std::pair<std::string_view, eMeleeCombo> mapping[]{
        { "UNARMED",     MELEE_COMBO_UNARMED_1 },
        { "BBALLBAT",    MELEE_COMBO_BBALLBAT  },
        { "KNIFE",       MELEE_COMBO_KNIFE     },
        { "GOLFCLUB",    MELEE_COMBO_GOLFCLUB  },
        { "SWORD",       MELEE_COMBO_SWORD     },
        { "CHAINSAW",    MELEE_COMBO_CHAINSAW  },
        { "DILDO",       MELEE_COMBO_DILDO     },
        { "FLOWERS",     MELEE_COMBO_FLOWERS   },
    };
    if (const auto it = rng::find(mapping, name, [](const auto& e) { return e.first; }); it != std::end(mapping))
        return it->second;

    return eMeleeCombo::MELEE_COMBO_UNARMED_1;
}

// 0x5BE670
void CWeaponInfo::LoadWeaponData() {
    auto f = CFileMgr::OpenFile("DATA\\WEAPON.DAT", "rb");
    for (auto line = CFileLoader::LoadLine(f); line; line = CFileLoader::LoadLine(f)) {
        if (std::string_view{line}.find("ENDWEAPONDATA") != std::string_view::npos) // Not quite the way they did it, but it's fine.
            break;

        // Read beginning of line here (that is, from the first char up to the first string)
        // This is quite a hacky solution, they should've just skipped to the first non-ws character manually
        char unused[32]{};

        switch ((uint8)line[0]) { // Gotta cast it because of `case 163`
        case '$': { // Gun data
            char weaponName[32]{};
            char fireTypeName[32]{};
            float targetRange{}, weaponRange{};
            int32 modelId1{}, modelId2{};
            int32 slot{};
            char animGrpName[32]{};
            uint32 ammo{};
            uint32 dmg{};
            CVector offset{};
            uint32 skill{};
            uint32 reqStatLevelForSkill{};
            float accuracy{};
            float moveSpeed{};

            struct {
                int32 end{}, start{}, fire{};
            } animLoopInfo[2]{};

            int32 breakoutTime{};
            uint32 flags{};
            float speed{}, radius{};
            float lifespan{}, spread{};

            (void)sscanf(line,
                "%s %s %s %f %f %d %d %d %s %d %d %f %f %f %d %d %f %f %d %d %d %d %d %d %d %x %f %f %f %f",
                unused,
                weaponName,
                fireTypeName,
                &targetRange, &weaponRange,
                &modelId1, &modelId2,
                &slot,
                animGrpName,
                &ammo,
                &dmg,
                &offset.x, &offset.y, &offset.z,
                &skill,
                &reqStatLevelForSkill,
                &accuracy,
                &moveSpeed,
                &animLoopInfo[0].start, &animLoopInfo[0].end, &animLoopInfo[0].fire,
                &animLoopInfo[1].start, &animLoopInfo[1].end, &animLoopInfo[1].fire,
                &breakoutTime,
                &flags,
                &speed,
                &radius,
                &lifespan,
                &spread
            );

            const auto weaponType = FindWeaponType(weaponName);
            const auto skillLevel = WeaponHasSkillStats(weaponType) ? (eWeaponSkill)skill : eWeaponSkill::STD;
            auto& wi = aWeaponInfo[GetWeaponInfoIndex(weaponType, skillLevel)];
            wi.m_nWeaponFire = FindWeaponFireType(fireTypeName);
            wi.m_fTargetRange = targetRange;
            wi.m_fWeaponRange = weaponRange;
            wi.m_nModelId1 = modelId1;
            wi.m_nModelId2 = modelId2;
            wi.m_nSlot = slot;
            wi.m_nAmmoClip = ammo;
            wi.m_nDamage = dmg;
            wi.m_vecFireOffset = offset;
            wi.m_nSkillLevel = (uint32)skillLevel;
            wi.m_nReqStatLevel = reqStatLevelForSkill;
            wi.m_fAccuracy = accuracy;
            wi.m_fMoveSpeed = moveSpeed;
            wi.m_fBreakoutTime = (float)breakoutTime / 30.f;
            wi.m_nFlags = flags;
            wi.m_fSpeed = speed;
            wi.m_fLifespan = lifespan;
            wi.m_fSpread = spread;

            const auto SetAnimLoopInfos = [&](auto& start, auto& end, auto& fire, auto idx) {
                assert(start <= end);

                const auto info = animLoopInfo[idx];
                start = (float)info.start / 30.f;
                end = (float)info.end / 30.f;
                fire = (float)info.fire / 30.f;

                end = start + std::floor(0.1f + (end - start) * 50.f) / 50.f - 0.006f;
            };
            SetAnimLoopInfos(wi.m_fAnimLoopStart, wi.m_fAnimLoopEnd, wi.m_fAnimLoopFire, 0);
            SetAnimLoopInfos(wi.m_fAnimLoop2Start, wi.m_fAnimLoop2End, wi.m_fAnimLoop2Fire, 1);


            if (!std::string_view{ animGrpName }.starts_with("null")) {
                wi.m_eAnimGroup = CAnimManager::GetAnimationGroupId(animGrpName);
            }

            if (wi.m_eAnimGroup >= ANIM_GROUP_PYTHON && wi.m_eAnimGroup <= ANIM_GROUP_SPRAYCAN) {
                wi.m_nAimOffsetIndex = wi.m_eAnimGroup - ANIM_GROUP_PYTHON;
            }

            if (skillLevel == eWeaponSkill::STD && weaponType != eWeaponType::WEAPON_DETONATOR) {
                if (modelId1 > 0) {
                    CModelInfo::GetModelInfo(modelId1)->AsWeaponModelInfoPtr()->m_weaponInfo = weaponType;
                }
            }
            break;
        }
        case '%': { // Gun aiming offsets
            char stealthAnimGrp[32]{};
            float aimX{}, aimZ{};
            float duckX{}, duckZ{};
            uint32 RLoadA{}, RLoadB{};
            uint32 crouchRLoadA{}, crouchRLoadB{};

            (void)sscanf(line, "%s %s %f %f %f %f %d %d %d %d", unused, stealthAnimGrp, &aimX, &aimZ, &duckX, &duckZ, &RLoadA, &RLoadB, &crouchRLoadA, &crouchRLoadB);

            g_GunAimingOffsets[CAnimManager::GetAnimationGroupId(stealthAnimGrp) - ANIM_GROUP_PYTHON] = {
                .AimX = aimX,
                .AimZ = aimZ,

                .DuckX = duckX,
                .DuckZ = duckZ,

                .RLoadA = (int16)RLoadA,
                .RLoadB = (int16)RLoadB,

                .CrouchRLoadA = (int16)crouchRLoadA,
                .CrouchRLoadB = (int16)crouchRLoadB
            };

            break;
        }
        case 163: { // MELEE DATA
            char weaponName[32]{};
            char fireTypeName[32]{};
            float targetRange{}, weaponRange{};
            int32 modelId1{}, modelId2{};
            uint32 slot{};
            char baseComboName[32]{};
            uint32 numCombos{};
            uint32 flags{};
            char stealthAnimGrpName[32]{};

            (void)sscanf(line,
                "%s %s %s %f %f %d %d %d %s %d %x %s",
                unused,
                weaponName,
                fireTypeName,
                &targetRange,
                &weaponRange,
                &modelId1,
                &modelId2,
                &slot,
                baseComboName,
                &numCombos,
                &flags,
                stealthAnimGrpName
            );

            auto wType = FindWeaponType(weaponName);
            auto& wi = aWeaponInfo[(uint32)wType];
            wi.m_nWeaponFire = FindWeaponFireType(fireTypeName);
            wi.m_fTargetRange = targetRange;
            wi.m_fWeaponRange = weaponRange;
            wi.m_nModelId1 = modelId1;
            wi.m_nModelId2 = modelId2;
            wi.m_nSlot = slot;
            wi.m_nBaseCombo = GetBaseComboByName(baseComboName);
            wi.m_nNumCombos = (uint8)numCombos;
            wi.m_nFlags = flags;

            if (!std::string_view{stealthAnimGrpName}.starts_with("null"))
                wi.m_eAnimGroup = CAnimManager::GetAnimationGroupId(stealthAnimGrpName);

            if (modelId1 > 0)
                CModelInfo::GetModelInfo(modelId1)->AsWeaponModelInfoPtr()->m_weaponInfo = wType;

            break;
        }
        }
    }
    CFileMgr::CloseFile(f);
}

// 0x743C60
CWeaponInfo* CWeaponInfo::GetWeaponInfo(eWeaponType weaponID, eWeaponSkill skill) {
    return &aWeaponInfo[GetWeaponInfoIndex(weaponID, skill)];
}

// 0x743CD0
eStats CWeaponInfo::GetSkillStatIndex(eWeaponType weaponType) {
    if (!WeaponHasSkillStats(weaponType)) {
        assert(0);
        return (eStats)-1;
    }

    if (weaponType <= WEAPON_M4)
        return (eStats)(weaponType - WEAPON_PISTOL + STAT_PISTOL_SKILL);

    if (weaponType == WEAPON_TEC9)
        return (eStats)STAT_MACHINE_PISTOL_SKILL;

    if (weaponType == WEAPON_COUNTRYRIFLE)
        return (eStats)STAT_GAMBLING;

    return (eStats)(weaponType + STAT_PISTOL_SKILL);
}

// 0x743D10
eWeaponType CWeaponInfo::FindWeaponType(const char* type) {
    // From 8D6150 (ms_aWeaponNames)
    constexpr const char* names[]{
        "UNARMED",
        "BRASSKNUCKLE",
        "GOLFCLUB",
        "NIGHTSTICK",
        "KNIFE",
        "BASEBALLBAT",
        "SHOVEL",
        "POOLCUE",
        "KATANA",
        "CHAINSAW",
        "DILDO1",
        "DILDO2",
        "VIBE1",
        "VIBE2",
        "FLOWERS",
        "CANE",
        "GRENADE",
        "TEARGAS",
        "MOLOTOV",
        "ROCKET",
        "ROCKET_HS",
        "FREEFALL_BOMB",
        "PISTOL",
        "PISTOL_SILENCED",
        "DESERT_EAGLE",
        "SHOTGUN",
        "SAWNOFF",
        "SPAS12",
        "MICRO_UZI",
        "MP5",
        "AK47",
        "M4",
        "TEC9",
        "COUNTRYRIFLE",
        "SNIPERRIFLE",
        "RLAUNCHER",
        "RLAUNCHER_HS",
        "FTHROWER",
        "MINIGUN",
        "SATCHEL_CHARGE",
        "DETONATOR",
        "SPRAYCAN",
        "EXTINGUISHER",
        "CAMERA",
        "NIGHTVISION",
        "INFRARED",
        "PARACHUTE",
        "", // Yeah, empty string. Unsure why.
        "ARMOUR",
    };
    for (auto i = 0u; i < std::size(names); i++) {
        if (!_stricmp(names[i], type)) {
            return (eWeaponType)i;
        }
    }
    assert(0); // Shouldn't be reachable
    return eWeaponType::WEAPON_UNARMED;
}

// 0x685700
AnimationId CWeaponInfo::GetCrouchReloadAnimationID() const {
    return flags.bCrouchFire && flags.bReload ? ANIM_ID_CROUCHRELOAD : ANIM_ID_WALK;
}

// 0x743D50
float CWeaponInfo::GetTargetHeadRange() const {
    return (float)((uint32)m_nSkillLevel + 2) * m_fWeaponRange / 25.f;
}

// 0x743D70
uint32 CWeaponInfo::GetWeaponReloadTime() const {
    if (flags.bReload)
        return flags.bTwinPistol ? 2000u : 1000u;

    if (flags.bLongReload)
        return 1000u;

    const auto& ao = g_GunAimingOffsets[m_nAimOffsetIndex];
    return std::max(400u, (uint32)std::max({ ao.RLoadA, ao.RLoadB, ao.CrouchRLoadA, ao.CrouchRLoadB }) + 100);
}
