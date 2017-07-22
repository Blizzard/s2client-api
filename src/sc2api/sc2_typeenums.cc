#include "sc2api/sc2_typeenums.h"

namespace sc2 {

const char* UnitTypeToName(UnitTypeID unit_type) {
    switch ((UNIT_TYPEID)unit_type) {
        case UNIT_TYPEID::PROTOSS_COLOSSUS:                 return "PROTOSS_COLOSSUS"; // 1366
        case UNIT_TYPEID::TERRAN_TECHLAB:                   return "TERRAN_TECHLAB"; // 172
        case UNIT_TYPEID::TERRAN_REACTOR:                   return "TERRAN_REACTOR"; // 257
        case UNIT_TYPEID::ZERG_INFESTORTERRAN:              return "ZERG_INFESTORTERRAN"; // 25
        case UNIT_TYPEID::ZERG_BANELINGCOCOON:              return "ZERG_BANELINGCOCOON"; // 1985
        case UNIT_TYPEID::ZERG_BANELING:                    return "ZERG_BANELING"; // 32532
        case UNIT_TYPEID::PROTOSS_MOTHERSHIP:               return "PROTOSS_MOTHERSHIP"; // 116
        case UNIT_TYPEID::TERRAN_POINTDEFENSEDRONE:         return "TERRAN_POINTDEFENSEDRONE"; // 337
        case UNIT_TYPEID::ZERG_CHANGELING:                  return "ZERG_CHANGELING"; // 705
        case UNIT_TYPEID::ZERG_CHANGELINGZEALOT:            return "ZERG_CHANGELINGZEALOT"; // 354
        case UNIT_TYPEID::ZERG_CHANGELINGMARINESHIELD:      return "ZERG_CHANGELINGMARINESHIELD"; // 347
        case UNIT_TYPEID::ZERG_CHANGELINGMARINE:            return "ZERG_CHANGELINGMARINE"; // 400
        case UNIT_TYPEID::ZERG_CHANGELINGZERGLINGWINGS:     return "ZERG_CHANGELINGZERGLINGWINGS"; // 150
        case UNIT_TYPEID::ZERG_CHANGELINGZERGLING:          return "ZERG_CHANGELINGZERGLING"; // 36
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:             return "TERRAN_COMMANDCENTER"; // 7210
        case UNIT_TYPEID::TERRAN_SUPPLYDEPOT:               return "TERRAN_SUPPLYDEPOT"; // 27140
        case UNIT_TYPEID::TERRAN_REFINERY:                  return "TERRAN_REFINERY"; // 18039
        case UNIT_TYPEID::TERRAN_BARRACKS:                  return "TERRAN_BARRACKS"; // 15604
        case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:            return "TERRAN_ENGINEERINGBAY"; // 3892
        case UNIT_TYPEID::TERRAN_MISSILETURRET:             return "TERRAN_MISSILETURRET"; // 13847
        case UNIT_TYPEID::TERRAN_BUNKER:                    return "TERRAN_BUNKER"; // 2604
        case UNIT_TYPEID::TERRAN_SENSORTOWER:               return "TERRAN_SENSORTOWER"; // 1020
        case UNIT_TYPEID::TERRAN_GHOSTACADEMY:              return "TERRAN_GHOSTACADEMY"; // 151
        case UNIT_TYPEID::TERRAN_FACTORY:                   return "TERRAN_FACTORY"; // 6793
        case UNIT_TYPEID::TERRAN_STARPORT:                  return "TERRAN_STARPORT"; // 5039
        case UNIT_TYPEID::TERRAN_ARMORY:                    return "TERRAN_ARMORY"; // 1778
        case UNIT_TYPEID::TERRAN_FUSIONCORE:                return "TERRAN_FUSIONCORE"; // 257
        case UNIT_TYPEID::TERRAN_AUTOTURRET:                return "TERRAN_AUTOTURRET"; // 2376
        case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:           return "TERRAN_SIEGETANKSIEGED"; // 5854
        case UNIT_TYPEID::TERRAN_SIEGETANK:                 return "TERRAN_SIEGETANK"; // 12595
        case UNIT_TYPEID::TERRAN_VIKINGASSAULT:             return "TERRAN_VIKINGASSAULT"; // 264
        case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:             return "TERRAN_VIKINGFIGHTER"; // 10550
        case UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING:       return "TERRAN_COMMANDCENTERFLYING"; // 249
        case UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:           return "TERRAN_BARRACKSTECHLAB"; // 4333
        case UNIT_TYPEID::TERRAN_BARRACKSREACTOR:           return "TERRAN_BARRACKSREACTOR"; // 5847
        case UNIT_TYPEID::TERRAN_FACTORYTECHLAB:            return "TERRAN_FACTORYTECHLAB"; // 3598
        case UNIT_TYPEID::TERRAN_FACTORYREACTOR:            return "TERRAN_FACTORYREACTOR"; // 1656
        case UNIT_TYPEID::TERRAN_STARPORTTECHLAB:           return "TERRAN_STARPORTTECHLAB"; // 1518
        case UNIT_TYPEID::TERRAN_STARPORTREACTOR:           return "TERRAN_STARPORTREACTOR"; // 1913
        case UNIT_TYPEID::TERRAN_FACTORYFLYING:             return "TERRAN_FACTORYFLYING"; // 121
        case UNIT_TYPEID::TERRAN_STARPORTFLYING:            return "TERRAN_STARPORTFLYING"; // 70
        case UNIT_TYPEID::TERRAN_SCV:                       return "TERRAN_SCV"; // 1440842
        case UNIT_TYPEID::TERRAN_BARRACKSFLYING:            return "TERRAN_BARRACKSFLYING"; // 294
        case UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED:        return "TERRAN_SUPPLYDEPOTLOWERED"; // 7872
        case UNIT_TYPEID::TERRAN_MARINE:                    return "TERRAN_MARINE"; // 174810
        case UNIT_TYPEID::TERRAN_REAPER:                    return "TERRAN_REAPER"; // 4568
        case UNIT_TYPEID::TERRAN_GHOST:                     return "TERRAN_GHOST"; // 627
        case UNIT_TYPEID::TERRAN_MARAUDER:                  return "TERRAN_MARAUDER"; // 26363
        case UNIT_TYPEID::TERRAN_THOR:                      return "TERRAN_THOR"; // 1935
        case UNIT_TYPEID::TERRAN_HELLION:                   return "TERRAN_HELLION"; // 10949
        case UNIT_TYPEID::TERRAN_MEDIVAC:                   return "TERRAN_MEDIVAC"; // 20625
        case UNIT_TYPEID::TERRAN_BANSHEE:                   return "TERRAN_BANSHEE"; // 2990
        case UNIT_TYPEID::TERRAN_RAVEN:                     return "TERRAN_RAVEN"; // 3540
        case UNIT_TYPEID::TERRAN_BATTLECRUISER:             return "TERRAN_BATTLECRUISER"; // 1333
        case UNIT_TYPEID::TERRAN_NUKE:                      return "TERRAN_NUKE"; // 26
        case UNIT_TYPEID::PROTOSS_NEXUS:                    return "PROTOSS_NEXUS"; // 11198
        case UNIT_TYPEID::PROTOSS_PYLON:                    return "PROTOSS_PYLON"; // 33987
        case UNIT_TYPEID::PROTOSS_ASSIMILATOR:              return "PROTOSS_ASSIMILATOR"; // 14292
        case UNIT_TYPEID::PROTOSS_GATEWAY:                  return "PROTOSS_GATEWAY"; // 10581
        case UNIT_TYPEID::PROTOSS_FORGE:                    return "PROTOSS_FORGE"; // 2592
        case UNIT_TYPEID::PROTOSS_FLEETBEACON:              return "PROTOSS_FLEETBEACON"; // 331
        case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:          return "PROTOSS_TWILIGHTCOUNCIL"; // 1632
        case UNIT_TYPEID::PROTOSS_PHOTONCANNON:             return "PROTOSS_PHOTONCANNON"; // 8659
        case UNIT_TYPEID::PROTOSS_STARGATE:                 return "PROTOSS_STARGATE"; // 2360
        case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:           return "PROTOSS_TEMPLARARCHIVE"; // 300
        case UNIT_TYPEID::PROTOSS_DARKSHRINE:               return "PROTOSS_DARKSHRINE"; // 430
        case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:              return "PROTOSS_ROBOTICSBAY"; // 396
        case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:         return "PROTOSS_ROBOTICSFACILITY"; // 2478
        case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:          return "PROTOSS_CYBERNETICSCORE"; // 4471
        case UNIT_TYPEID::PROTOSS_ZEALOT:                   return "PROTOSS_ZEALOT"; // 19385
        case UNIT_TYPEID::PROTOSS_STALKER:                  return "PROTOSS_STALKER"; // 34911
        case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:              return "PROTOSS_HIGHTEMPLAR"; // 3481
        case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:              return "PROTOSS_DARKTEMPLAR"; // 3001
        case UNIT_TYPEID::PROTOSS_SENTRY:                   return "PROTOSS_SENTRY"; // 3588
        case UNIT_TYPEID::PROTOSS_PHOENIX:                  return "PROTOSS_PHOENIX"; // 1925
        case UNIT_TYPEID::PROTOSS_CARRIER:                  return "PROTOSS_CARRIER"; // 2330
        case UNIT_TYPEID::PROTOSS_VOIDRAY:                  return "PROTOSS_VOIDRAY"; // 5200
        case UNIT_TYPEID::PROTOSS_WARPPRISM:                return "PROTOSS_WARPPRISM"; // 1055
        case UNIT_TYPEID::PROTOSS_OBSERVER:                 return "PROTOSS_OBSERVER"; // 2390
        case UNIT_TYPEID::PROTOSS_IMMORTAL:                 return "PROTOSS_IMMORTAL"; // 4539
        case UNIT_TYPEID::PROTOSS_PROBE:                    return "PROTOSS_PROBE"; // 972805
        case UNIT_TYPEID::PROTOSS_INTERCEPTOR:              return "PROTOSS_INTERCEPTOR"; // 27331
        case UNIT_TYPEID::ZERG_HATCHERY:                    return "ZERG_HATCHERY"; // 14929
        case UNIT_TYPEID::ZERG_CREEPTUMOR:                  return "ZERG_CREEPTUMOR"; // 11530
        case UNIT_TYPEID::ZERG_EXTRACTOR:                   return "ZERG_EXTRACTOR"; // 16701
        case UNIT_TYPEID::ZERG_SPAWNINGPOOL:                return "ZERG_SPAWNINGPOOL"; // 3956
        case UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:            return "ZERG_EVOLUTIONCHAMBER"; // 4154
        case UNIT_TYPEID::ZERG_HYDRALISKDEN:                return "ZERG_HYDRALISKDEN"; // 835
        case UNIT_TYPEID::ZERG_SPIRE:                       return "ZERG_SPIRE"; // 787
        case UNIT_TYPEID::ZERG_ULTRALISKCAVERN:             return "ZERG_ULTRALISKCAVERN"; // 166
        case UNIT_TYPEID::ZERG_INFESTATIONPIT:              return "ZERG_INFESTATIONPIT"; // 709
        case UNIT_TYPEID::ZERG_NYDUSNETWORK:                return "ZERG_NYDUSNETWORK"; // 123
        case UNIT_TYPEID::ZERG_BANELINGNEST:                return "ZERG_BANELINGNEST"; // 1633
        case UNIT_TYPEID::ZERG_ROACHWARREN:                 return "ZERG_ROACHWARREN"; // 1903
        case UNIT_TYPEID::ZERG_SPINECRAWLER:                return "ZERG_SPINECRAWLER"; // 4848
        case UNIT_TYPEID::ZERG_SPORECRAWLER:                return "ZERG_SPORECRAWLER"; // 6823
        case UNIT_TYPEID::ZERG_LAIR:                        return "ZERG_LAIR"; // 1295
        case UNIT_TYPEID::ZERG_HIVE:                        return "ZERG_HIVE"; // 207
        case UNIT_TYPEID::ZERG_GREATERSPIRE:                return "ZERG_GREATERSPIRE"; // 36
        case UNIT_TYPEID::ZERG_EGG:                         return "ZERG_EGG"; // 10096
        case UNIT_TYPEID::ZERG_DRONE:                       return "ZERG_DRONE"; // 1231187
        case UNIT_TYPEID::ZERG_ZERGLING:                    return "ZERG_ZERGLING"; // 205454
        case UNIT_TYPEID::ZERG_OVERLORD:                    return "ZERG_OVERLORD"; // 35779
        case UNIT_TYPEID::ZERG_HYDRALISK:                   return "ZERG_HYDRALISK"; // 33559
        case UNIT_TYPEID::ZERG_MUTALISK:                    return "ZERG_MUTALISK"; // 12009
        case UNIT_TYPEID::ZERG_ULTRALISK:                   return "ZERG_ULTRALISK"; // 2481
        case UNIT_TYPEID::ZERG_ROACH:                       return "ZERG_ROACH"; // 55141
        case UNIT_TYPEID::ZERG_INFESTOR:                    return "ZERG_INFESTOR"; // 1094
        case UNIT_TYPEID::ZERG_CORRUPTOR:                   return "ZERG_CORRUPTOR"; // 6505
        case UNIT_TYPEID::ZERG_BROODLORDCOCOON:             return "ZERG_BROODLORDCOCOON"; // 118
        case UNIT_TYPEID::ZERG_BROODLORD:                   return "ZERG_BROODLORD"; // 1963
        case UNIT_TYPEID::ZERG_BANELINGBURROWED:            return "ZERG_BANELINGBURROWED"; // 51
        case UNIT_TYPEID::ZERG_DRONEBURROWED:               return "ZERG_DRONEBURROWED"; // 97
        case UNIT_TYPEID::ZERG_HYDRALISKBURROWED:           return "ZERG_HYDRALISKBURROWED"; // 7
        case UNIT_TYPEID::ZERG_ROACHBURROWED:               return "ZERG_ROACHBURROWED"; // 362
        case UNIT_TYPEID::ZERG_ZERGLINGBURROWED:            return "ZERG_ZERGLINGBURROWED"; // 94
        case UNIT_TYPEID::ZERG_QUEENBURROWED:               return "ZERG_QUEENBURROWED"; // 1
        case UNIT_TYPEID::ZERG_QUEEN:                       return "ZERG_QUEEN"; // 12673
        case UNIT_TYPEID::ZERG_INFESTORBURROWED:            return "ZERG_INFESTORBURROWED"; // 1049
        case UNIT_TYPEID::ZERG_OVERLORDCOCOON:              return "ZERG_OVERLORDCOCOON"; // 72
        case UNIT_TYPEID::ZERG_OVERSEER:                    return "ZERG_OVERSEER"; // 4249
        case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:         return "TERRAN_PLANETARYFORTRESS"; // 988
        case UNIT_TYPEID::ZERG_ULTRALISKBURROWED:           return "ZERG_ULTRALISKBURROWED"; // 3
        case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:            return "TERRAN_ORBITALCOMMAND"; // 7403
        case UNIT_TYPEID::PROTOSS_WARPGATE:                 return "PROTOSS_WARPGATE"; // 7086
        case UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING:      return "TERRAN_ORBITALCOMMANDFLYING"; // 331
        case UNIT_TYPEID::NEUTRAL_FORCEFIELD:               return "NEUTRAL_FORCEFIELD"; // 1604
        case UNIT_TYPEID::PROTOSS_WARPPRISMPHASING:         return "PROTOSS_WARPPRISMPHASING"; // 164
        case UNIT_TYPEID::ZERG_CREEPTUMORBURROWED:          return "ZERG_CREEPTUMORBURROWED"; // 14871
        case UNIT_TYPEID::ZERG_CREEPTUMORQUEEN:             return "ZERG_CREEPTUMORQUEEN"; // 5696
        case UNIT_TYPEID::ZERG_SPINECRAWLERUPROOTED:        return "ZERG_SPINECRAWLERUPROOTED"; // 92
        case UNIT_TYPEID::ZERG_SPORECRAWLERUPROOTED:        return "ZERG_SPORECRAWLERUPROOTED"; // 97
        case UNIT_TYPEID::PROTOSS_ARCHON:                   return "PROTOSS_ARCHON"; // 2880
        case UNIT_TYPEID::ZERG_NYDUSCANAL:                  return "ZERG_NYDUSCANAL"; // 206
        case UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD:         return "NEUTRAL_RICHMINERALFIELD"; // 49455
        case UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750:      return "NEUTRAL_RICHMINERALFIELD750"; // 43978
        case UNIT_TYPEID::NEUTRAL_XELNAGATOWER:             return "NEUTRAL_XELNAGATOWER"; // 20409
        case UNIT_TYPEID::ZERG_INFESTEDTERRANSEGG:          return "ZERG_INFESTEDTERRANSEGG"; // 994
        case UNIT_TYPEID::ZERG_LARVA:                       return "ZERG_LARVA"; // 170403
        case UNIT_TYPEID::TERRAN_MULE:                      return "TERRAN_MULE"; // 15532
        case UNIT_TYPEID::ZERG_BROODLING:                   return "ZERG_BROODLING"; // 31934
        case UNIT_TYPEID::PROTOSS_ADEPT:                    return "PROTOSS_ADEPT"; // 20812
        case UNIT_TYPEID::NEUTRAL_KARAKFEMALE:              return "NEUTRAL_KARAKFEMALE"; // 2080
        case UNIT_TYPEID::NEUTRAL_UTILITYBOT:               return "NEUTRAL_UTILITYBOT"; // 8083
        case UNIT_TYPEID::NEUTRAL_SCANTIPEDE:               return "NEUTRAL_SCANTIPEDE"; // 4007
        case UNIT_TYPEID::NEUTRAL_MINERALFIELD:             return "NEUTRAL_MINERALFIELD"; // 398595
        case UNIT_TYPEID::NEUTRAL_VESPENEGEYSER:            return "NEUTRAL_VESPENEGEYSER"; // 104626
        case UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER:      return "NEUTRAL_SPACEPLATFORMGEYSER"; // 88208
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRIS6X6:    return "NEUTRAL_DESTRUCTIBLEDEBRIS6X6"; // 23631
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEROCK6X6:      return "NEUTRAL_DESTRUCTIBLEROCK6X6"; // 14638
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEULBR: return "NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEULBR"; // 12432
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEBLUR: return "NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEBLUR"; // 9153
        case UNIT_TYPEID::NEUTRAL_UNBUILDABLEBRICKSDESTRUCTIBLE: return "NEUTRAL_UNBUILDABLEBRICKSDESTRUCTIBLE"; // 22453
        case UNIT_TYPEID::NEUTRAL_UNBUILDABLEPLATESDESTRUCTIBLE: return "NEUTRAL_UNBUILDABLEPLATESDESTRUCTIBLE"; // 20399
        case UNIT_TYPEID::NEUTRAL_MINERALFIELD750:          return "NEUTRAL_MINERALFIELD750"; // 375940
        case UNIT_TYPEID::TERRAN_HELLIONTANK:               return "TERRAN_HELLIONTANK"; // 4200
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERDEBRIS: return "NEUTRAL_COLLAPSIBLETERRANTOWERDEBRIS"; // 153
        case UNIT_TYPEID::NEUTRAL_DEBRISRAMPLEFT:           return "NEUTRAL_DEBRISRAMPLEFT"; // 115
        case UNIT_TYPEID::NEUTRAL_DEBRISRAMPRIGHT:          return "NEUTRAL_DEBRISRAMPRIGHT"; // 23
        case UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE:           return "PROTOSS_MOTHERSHIPCORE"; // 2779
        case UNIT_TYPEID::ZERG_LOCUSTMP:                    return "ZERG_LOCUSTMP"; // 3574
        case UNIT_TYPEID::ZERG_SWARMHOSTBURROWEDMP:         return "ZERG_SWARMHOSTBURROWEDMP"; // 42
        case UNIT_TYPEID::ZERG_SWARMHOSTMP:                 return "ZERG_SWARMHOSTMP"; // 2329
        case UNIT_TYPEID::PROTOSS_ORACLE:                   return "PROTOSS_ORACLE"; // 1247
        case UNIT_TYPEID::PROTOSS_TEMPEST:                  return "PROTOSS_TEMPEST"; // 401
        case UNIT_TYPEID::TERRAN_WIDOWMINE:                 return "TERRAN_WIDOWMINE"; // 7017
        case UNIT_TYPEID::ZERG_VIPER:                       return "ZERG_VIPER"; // 883
        case UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED:         return "TERRAN_WIDOWMINEBURROWED"; // 1580
        case UNIT_TYPEID::ZERG_LURKERMPEGG:                 return "ZERG_LURKERMPEGG"; // 100
        case UNIT_TYPEID::ZERG_LURKERMP:                    return "ZERG_LURKERMP"; // 1466
        case UNIT_TYPEID::ZERG_LURKERMPBURROWED:            return "ZERG_LURKERMPBURROWED"; // 526
        case UNIT_TYPEID::ZERG_LURKERDENMP:                 return "ZERG_LURKERDENMP"; // 84
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNITRAMPLEFT: return "NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNITRAMPLEFT"; // 38
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNITRAMPRIGHT: return "NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNITRAMPRIGHT"; // 5
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLEROCKTOWERPUSHUNIT: return "NEUTRAL_COLLAPSIBLEROCKTOWERPUSHUNIT"; // 3
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNIT: return "NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNIT"; // 32
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLEROCKTOWERDIAGONAL: return "NEUTRAL_COLLAPSIBLEROCKTOWERDIAGONAL"; // 1938
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERDIAGONAL: return "NEUTRAL_COLLAPSIBLETERRANTOWERDIAGONAL"; // 2082
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERRAMPLEFT: return "NEUTRAL_COLLAPSIBLETERRANTOWERRAMPLEFT"; // 5670
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERRAMPRIGHT: return "NEUTRAL_COLLAPSIBLETERRANTOWERRAMPRIGHT"; // 3726
        case UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER:     return "NEUTRAL_PROTOSSVESPENEGEYSER"; // 93826
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEROCKEX1DIAGONALHUGEBLUR: return "NEUTRAL_DESTRUCTIBLEROCKEX1DIAGONALHUGEBLUR"; // 2982
        case UNIT_TYPEID::NEUTRAL_LABMINERALFIELD:          return "NEUTRAL_LABMINERALFIELD"; // 66723
        case UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750:       return "NEUTRAL_LABMINERALFIELD750"; // 64859
        case UNIT_TYPEID::ZERG_RAVAGERCOCOON:               return "ZERG_RAVAGERCOCOON"; // 148
        case UNIT_TYPEID::ZERG_RAVAGER:                     return "ZERG_RAVAGER"; // 4288
        case UNIT_TYPEID::TERRAN_LIBERATOR:                 return "TERRAN_LIBERATOR"; // 5116
        case UNIT_TYPEID::ZERG_RAVAGERBURROWED:             return "ZERG_RAVAGERBURROWED"; // 10
        case UNIT_TYPEID::TERRAN_THORAP:                    return "TERRAN_THORAP"; // 194
        case UNIT_TYPEID::TERRAN_CYCLONE:                   return "TERRAN_CYCLONE"; // 3305
        case UNIT_TYPEID::ZERG_LOCUSTMPFLYING:              return "ZERG_LOCUSTMPFLYING"; // 2531
        case UNIT_TYPEID::PROTOSS_DISRUPTOR:                return "PROTOSS_DISRUPTOR"; // 942
        case UNIT_TYPEID::PROTOSS_ORACLESTASISTRAP:         return "PROTOSS_ORACLESTASISTRAP"; // 215
        case UNIT_TYPEID::PROTOSS_DISRUPTORPHASED:          return "PROTOSS_DISRUPTORPHASED"; // 868
        case UNIT_TYPEID::TERRAN_LIBERATORAG:               return "TERRAN_LIBERATORAG"; // 780
        case UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT:          return "PROTOSS_ADEPTPHASESHIFT"; // 8523
        case UNIT_TYPEID::ZERG_PARASITICBOMBDUMMY:          return "ZERG_PARASITICBOMBDUMMY"; // 65
        case UNIT_TYPEID::TERRAN_KD8CHARGE:                 return "TERRAN_KD8CHARGE"; // 5047
        case UNIT_TYPEID::ZERG_TRANSPORTOVERLORDCOCOON:     return "ZERG_TRANSPORTOVERLORDCOCOON"; // 4
        case UNIT_TYPEID::ZERG_OVERLORDTRANSPORT:           return "ZERG_OVERLORDTRANSPORT"; // 305
        case UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED:         return "PROTOSS_PYLONOVERCHARGED"; // 245
    }

    return "UNKNOWN";
};

const char* AbilityTypeToName(AbilityID ability_type) {
    switch ((ABILITY_ID)ability_type) {
        case ABILITY_ID::SMART:                             return "SMART"; // 1424826
        case ABILITY_ID::MOVE:                              return "MOVE"; // 233
        case ABILITY_ID::PATROL:                            return "PATROL"; // 1680
        case ABILITY_ID::HOLDPOSITION:                      return "HOLDPOSITION"; // 5739
        case ABILITY_ID::EFFECT_SALVAGE:                    return "EFFECT_SALVAGE"; // 137
        case ABILITY_ID::EFFECT_EXPLODE:                    return "EFFECT_EXPLODE"; // 126
        case ABILITY_ID::RESEARCH_INTERCEPTORGRAVITONCATAPULT: return "RESEARCH_INTERCEPTORGRAVITONCATAPULT"; // 41
        case ABILITY_ID::RESEARCH_PHOENIXANIONPULSECRYSTALS: return "RESEARCH_PHOENIXANIONPULSECRYSTALS"; // 10
        case ABILITY_ID::EFFECT_FUNGALGROWTH:               return "EFFECT_FUNGALGROWTH"; // 405
        case ABILITY_ID::EFFECT_GUARDIANSHIELD:             return "EFFECT_GUARDIANSHIELD"; // 413
        case ABILITY_ID::TRAIN_BANELING:                    return "TRAIN_BANELING"; // 15403
        case ABILITY_ID::EFFECT_FEEDBACK:                   return "EFFECT_FEEDBACK"; // 87
        case ABILITY_ID::EFFECT_POINTDEFENSEDRONE:          return "EFFECT_POINTDEFENSEDRONE"; // 214
        case ABILITY_ID::HALLUCINATION_ARCHON:              return "HALLUCINATION_ARCHON"; // 46
        case ABILITY_ID::HALLUCINATION_COLOSSUS:            return "HALLUCINATION_COLOSSUS"; // 16
        case ABILITY_ID::HALLUCINATION_HIGHTEMPLAR:         return "HALLUCINATION_HIGHTEMPLAR"; // 1
        case ABILITY_ID::HALLUCINATION_IMMORTAL:            return "HALLUCINATION_IMMORTAL"; // 1
        case ABILITY_ID::HALLUCINATION_PHOENIX:             return "HALLUCINATION_PHOENIX"; // 82
        case ABILITY_ID::HALLUCINATION_PROBE:               return "HALLUCINATION_PROBE"; // 1
        case ABILITY_ID::HALLUCINATION_STALKER:             return "HALLUCINATION_STALKER"; // 2
        case ABILITY_ID::HALLUCINATION_VOIDRAY:             return "HALLUCINATION_VOIDRAY"; // 6
        case ABILITY_ID::HALLUCINATION_WARPPRISM:           return "HALLUCINATION_WARPPRISM"; // 1
        case ABILITY_ID::HALLUCINATION_ZEALOT:              return "HALLUCINATION_ZEALOT"; // 1
        case ABILITY_ID::EFFECT_HUNTERSEEKERMISSILE:        return "EFFECT_HUNTERSEEKERMISSILE"; // 386
        case ABILITY_ID::EFFECT_CALLDOWNMULE:               return "EFFECT_CALLDOWNMULE"; // 12315
        case ABILITY_ID::EFFECT_GRAVITONBEAM:               return "EFFECT_GRAVITONBEAM"; // 698
        case ABILITY_ID::EFFECT_SPAWNCHANGELING:            return "EFFECT_SPAWNCHANGELING"; // 680
        case ABILITY_ID::RESEARCH_GLIALREGENERATION:        return "RESEARCH_GLIALREGENERATION"; // 368
        case ABILITY_ID::RESEARCH_TUNNELINGCLAWS:           return "RESEARCH_TUNNELINGCLAWS"; // 58
        case ABILITY_ID::EFFECT_INFESTEDTERRANS:            return "EFFECT_INFESTEDTERRANS"; // 357
        case ABILITY_ID::EFFECT_NEURALPARASITE:             return "EFFECT_NEURALPARASITE"; // 14
        case ABILITY_ID::EFFECT_INJECTLARVA:                return "EFFECT_INJECTLARVA"; // 28345
        case ABILITY_ID::EFFECT_SUPPLYDROP:                 return "EFFECT_SUPPLYDROP"; // 690
        case ABILITY_ID::EFFECT_CHRONOBOOST:                return "EFFECT_CHRONOBOOST"; // 3357
        case ABILITY_ID::RESEARCH_CHITINOUSPLATING:         return "RESEARCH_CHITINOUSPLATING"; // 98
        case ABILITY_ID::BUILD_COMMANDCENTER:               return "BUILD_COMMANDCENTER"; // 2839
        case ABILITY_ID::BUILD_SUPPLYDEPOT:                 return "BUILD_SUPPLYDEPOT"; // 11700
        case ABILITY_ID::BUILD_REFINERY:                    return "BUILD_REFINERY"; // 5220
        case ABILITY_ID::BUILD_BARRACKS:                    return "BUILD_BARRACKS"; // 4395
        case ABILITY_ID::BUILD_ENGINEERINGBAY:              return "BUILD_ENGINEERINGBAY"; // 1388
        case ABILITY_ID::BUILD_MISSILETURRET:               return "BUILD_MISSILETURRET"; // 5293
        case ABILITY_ID::BUILD_BUNKER:                      return "BUILD_BUNKER"; // 728
        case ABILITY_ID::BUILD_SENSORTOWER:                 return "BUILD_SENSORTOWER"; // 301
        case ABILITY_ID::BUILD_GHOSTACADEMY:                return "BUILD_GHOSTACADEMY"; // 82
        case ABILITY_ID::BUILD_FACTORY:                     return "BUILD_FACTORY"; // 1795
        case ABILITY_ID::BUILD_STARPORT:                    return "BUILD_STARPORT"; // 1586
        case ABILITY_ID::BUILD_ARMORY:                      return "BUILD_ARMORY"; // 790
        case ABILITY_ID::BUILD_FUSIONCORE:                  return "BUILD_FUSIONCORE"; // 113
        case ABILITY_ID::EFFECT_HEAL:                       return "EFFECT_HEAL"; // 1
        case ABILITY_ID::MORPH_SIEGEMODE:                   return "MORPH_SIEGEMODE"; // 5442
        case ABILITY_ID::MORPH_UNSIEGE:                     return "MORPH_UNSIEGE"; // 2340
        case ABILITY_ID::EFFECT_SCAN:                       return "EFFECT_SCAN"; // 5488
        case ABILITY_ID::EFFECT_YAMATOGUN:                  return "EFFECT_YAMATOGUN"; // 576
        case ABILITY_ID::MORPH_VIKINGASSAULTMODE:           return "MORPH_VIKINGASSAULTMODE"; // 493
        case ABILITY_ID::MORPH_VIKINGFIGHTERMODE:           return "MORPH_VIKINGFIGHTERMODE"; // 135
        case ABILITY_ID::TRAIN_SCV:                         return "TRAIN_SCV"; // 45284
        case ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER:           return "MORPH_SUPPLYDEPOT_LOWER"; // 4383
        case ABILITY_ID::MORPH_SUPPLYDEPOT_RAISE:           return "MORPH_SUPPLYDEPOT_RAISE"; // 1414
        case ABILITY_ID::TRAIN_MARINE:                      return "TRAIN_MARINE"; // 62175
        case ABILITY_ID::TRAIN_REAPER:                      return "TRAIN_REAPER"; // 1763
        case ABILITY_ID::TRAIN_GHOST:                       return "TRAIN_GHOST"; // 267
        case ABILITY_ID::TRAIN_MARAUDER:                    return "TRAIN_MARAUDER"; // 9660
        case ABILITY_ID::TRAIN_SIEGETANK:                   return "TRAIN_SIEGETANK"; // 5833
        case ABILITY_ID::TRAIN_THOR:                        return "TRAIN_THOR"; // 768
        case ABILITY_ID::TRAIN_HELLION:                     return "TRAIN_HELLION"; // 4164
        case ABILITY_ID::TRAIN_HELLBAT:                     return "TRAIN_HELLBAT"; // 539
        case ABILITY_ID::TRAIN_CYCLONE:                     return "TRAIN_CYCLONE"; // 1400
        case ABILITY_ID::TRAIN_WIDOWMINE:                   return "TRAIN_WIDOWMINE"; // 3258
        case ABILITY_ID::TRAIN_MEDIVAC:                     return "TRAIN_MEDIVAC"; // 5059
        case ABILITY_ID::TRAIN_BANSHEE:                     return "TRAIN_BANSHEE"; // 877
        case ABILITY_ID::TRAIN_RAVEN:                       return "TRAIN_RAVEN"; // 869
        case ABILITY_ID::TRAIN_BATTLECRUISER:               return "TRAIN_BATTLECRUISER"; // 552
        case ABILITY_ID::TRAIN_VIKINGFIGHTER:               return "TRAIN_VIKINGFIGHTER"; // 3447
        case ABILITY_ID::TRAIN_LIBERATOR:                   return "TRAIN_LIBERATOR"; // 1985
        case ABILITY_ID::RESEARCH_HISECAUTOTRACKING:        return "RESEARCH_HISECAUTOTRACKING"; // 96
        case ABILITY_ID::RESEARCH_TERRANSTRUCTUREARMORUPGRADE: return "RESEARCH_TERRANSTRUCTUREARMORUPGRADE"; // 72
        case ABILITY_ID::RESEARCH_NEOSTEELFRAME:            return "RESEARCH_NEOSTEELFRAME"; // 18
        case ABILITY_ID::BUILD_NUKE:                        return "BUILD_NUKE"; // 52
        case ABILITY_ID::RESEARCH_STIMPACK:                 return "RESEARCH_STIMPACK"; // 779
        case ABILITY_ID::RESEARCH_COMBATSHIELD:             return "RESEARCH_COMBATSHIELD"; // 638
        case ABILITY_ID::RESEARCH_CONCUSSIVESHELLS:         return "RESEARCH_CONCUSSIVESHELLS"; // 338
        case ABILITY_ID::RESEARCH_INFERNALPREIGNITER:       return "RESEARCH_INFERNALPREIGNITER"; // 127
        case ABILITY_ID::RESEARCH_DRILLINGCLAWS:            return "RESEARCH_DRILLINGCLAWS"; // 55
        case ABILITY_ID::RESEARCH_MAGFIELDLAUNCHERS:        return "RESEARCH_MAGFIELDLAUNCHERS"; // 23
        case ABILITY_ID::RESEARCH_BANSHEECLOAKINGFIELD:     return "RESEARCH_BANSHEECLOAKINGFIELD"; // 179
        case ABILITY_ID::RESEARCH_RAVENCORVIDREACTOR:       return "RESEARCH_RAVENCORVIDREACTOR"; // 49
        case ABILITY_ID::RESEARCH_BANSHEEHYPERFLIGHTROTORS: return "RESEARCH_BANSHEEHYPERFLIGHTROTORS"; // 36
        case ABILITY_ID::RESEARCH_RAVENRECALIBRATEDEXPLOSIVES: return "RESEARCH_RAVENRECALIBRATEDEXPLOSIVES"; // 36
        case ABILITY_ID::RESEARCH_HIGHCAPACITYFUELTANKS:    return "RESEARCH_HIGHCAPACITYFUELTANKS"; // 21
        case ABILITY_ID::RESEARCH_ADVANCEDBALLISTICS:       return "RESEARCH_ADVANCEDBALLISTICS"; // 40
        case ABILITY_ID::RESEARCH_PERSONALCLOAKING:         return "RESEARCH_PERSONALCLOAKING"; // 31
        case ABILITY_ID::BUILD_NEXUS:                       return "BUILD_NEXUS"; // 1685
        case ABILITY_ID::BUILD_PYLON:                       return "BUILD_PYLON"; // 10036
        case ABILITY_ID::BUILD_ASSIMILATOR:                 return "BUILD_ASSIMILATOR"; // 3621
        case ABILITY_ID::BUILD_GATEWAY:                     return "BUILD_GATEWAY"; // 4572
        case ABILITY_ID::BUILD_FORGE:                       return "BUILD_FORGE"; // 744
        case ABILITY_ID::BUILD_FLEETBEACON:                 return "BUILD_FLEETBEACON"; // 106
        case ABILITY_ID::BUILD_TWILIGHTCOUNCIL:             return "BUILD_TWILIGHTCOUNCIL"; // 517
        case ABILITY_ID::BUILD_PHOTONCANNON:                return "BUILD_PHOTONCANNON"; // 2689
        case ABILITY_ID::BUILD_STARGATE:                    return "BUILD_STARGATE"; // 684
        case ABILITY_ID::BUILD_TEMPLARARCHIVE:              return "BUILD_TEMPLARARCHIVE"; // 179
        case ABILITY_ID::BUILD_DARKSHRINE:                  return "BUILD_DARKSHRINE"; // 184
        case ABILITY_ID::BUILD_ROBOTICSBAY:                 return "BUILD_ROBOTICSBAY"; // 192
        case ABILITY_ID::BUILD_ROBOTICSFACILITY:            return "BUILD_ROBOTICSFACILITY"; // 736
        case ABILITY_ID::BUILD_CYBERNETICSCORE:             return "BUILD_CYBERNETICSCORE"; // 759
        case ABILITY_ID::TRAIN_ZEALOT:                      return "TRAIN_ZEALOT"; // 840
        case ABILITY_ID::TRAIN_STALKER:                     return "TRAIN_STALKER"; // 1283
        case ABILITY_ID::TRAIN_HIGHTEMPLAR:                 return "TRAIN_HIGHTEMPLAR"; // 4
        case ABILITY_ID::TRAIN_DARKTEMPLAR:                 return "TRAIN_DARKTEMPLAR"; // 42
        case ABILITY_ID::TRAIN_SENTRY:                      return "TRAIN_SENTRY"; // 150
        case ABILITY_ID::TRAIN_ADEPT:                       return "TRAIN_ADEPT"; // 1163
        case ABILITY_ID::TRAIN_PHOENIX:                     return "TRAIN_PHOENIX"; // 569
        case ABILITY_ID::TRAIN_CARRIER:                     return "TRAIN_CARRIER"; // 465
        case ABILITY_ID::TRAIN_VOIDRAY:                     return "TRAIN_VOIDRAY"; // 1251
        case ABILITY_ID::TRAIN_ORACLE:                      return "TRAIN_ORACLE"; // 375
        case ABILITY_ID::TRAIN_TEMPEST:                     return "TRAIN_TEMPEST"; // 180
        case ABILITY_ID::TRAIN_WARPPRISM:                   return "TRAIN_WARPPRISM"; // 392
        case ABILITY_ID::TRAIN_OBSERVER:                    return "TRAIN_OBSERVER"; // 1057
        case ABILITY_ID::TRAIN_COLOSSUS:                    return "TRAIN_COLOSSUS"; // 548
        case ABILITY_ID::TRAIN_IMMORTAL:                    return "TRAIN_IMMORTAL"; // 1360
        case ABILITY_ID::TRAIN_DISRUPTOR:                   return "TRAIN_DISRUPTOR"; // 418
        case ABILITY_ID::TRAIN_PROBE:                       return "TRAIN_PROBE"; // 30654
        case ABILITY_ID::EFFECT_PSISTORM:                   return "EFFECT_PSISTORM"; // 638
        case ABILITY_ID::BUILD_INTERCEPTORS:                return "BUILD_INTERCEPTORS"; // 100
        case ABILITY_ID::RESEARCH_GRAVITICBOOSTER:          return "RESEARCH_GRAVITICBOOSTER"; // 30
        case ABILITY_ID::RESEARCH_GRAVITICDRIVE:            return "RESEARCH_GRAVITICDRIVE"; // 24
        case ABILITY_ID::RESEARCH_EXTENDEDTHERMALLANCE:     return "RESEARCH_EXTENDEDTHERMALLANCE"; // 117
        case ABILITY_ID::RESEARCH_PSISTORM:                 return "RESEARCH_PSISTORM"; // 112
        case ABILITY_ID::BUILD_HATCHERY:                    return "BUILD_HATCHERY"; // 3197
        case ABILITY_ID::BUILD_EXTRACTOR:                   return "BUILD_EXTRACTOR"; // 5172
        case ABILITY_ID::BUILD_SPAWNINGPOOL:                return "BUILD_SPAWNINGPOOL"; // 1006
        case ABILITY_ID::BUILD_EVOLUTIONCHAMBER:            return "BUILD_EVOLUTIONCHAMBER"; // 1254
        case ABILITY_ID::BUILD_HYDRALISKDEN:                return "BUILD_HYDRALISKDEN"; // 375
        case ABILITY_ID::BUILD_SPIRE:                       return "BUILD_SPIRE"; // 394
        case ABILITY_ID::BUILD_ULTRALISKCAVERN:             return "BUILD_ULTRALISKCAVERN"; // 112
        case ABILITY_ID::BUILD_INFESTATIONPIT:              return "BUILD_INFESTATIONPIT"; // 363
        case ABILITY_ID::BUILD_NYDUSNETWORK:                return "BUILD_NYDUSNETWORK"; // 49
        case ABILITY_ID::BUILD_BANELINGNEST:                return "BUILD_BANELINGNEST"; // 572
        case ABILITY_ID::BUILD_ROACHWARREN:                 return "BUILD_ROACHWARREN"; // 562
        case ABILITY_ID::BUILD_SPINECRAWLER:                return "BUILD_SPINECRAWLER"; // 1524
        case ABILITY_ID::BUILD_SPORECRAWLER:                return "BUILD_SPORECRAWLER"; // 2182
        case ABILITY_ID::MORPH_LAIR:                        return "MORPH_LAIR"; // 945
        case ABILITY_ID::MORPH_HIVE:                        return "MORPH_HIVE"; // 295
        case ABILITY_ID::MORPH_GREATERSPIRE:                return "MORPH_GREATERSPIRE"; // 88
        case ABILITY_ID::RESEARCH_PNEUMATIZEDCARAPACE:      return "RESEARCH_PNEUMATIZEDCARAPACE"; // 463
        case ABILITY_ID::RESEARCH_BURROW:                   return "RESEARCH_BURROW"; // 200
        case ABILITY_ID::RESEARCH_ZERGLINGADRENALGLANDS:    return "RESEARCH_ZERGLINGADRENALGLANDS"; // 114
        case ABILITY_ID::RESEARCH_ZERGLINGMETABOLICBOOST:   return "RESEARCH_ZERGLINGMETABOLICBOOST"; // 1077
        case ABILITY_ID::RESEARCH_GROOVEDSPINES:            return "RESEARCH_GROOVEDSPINES"; // 196
        case ABILITY_ID::RESEARCH_MUSCULARAUGMENTS:         return "RESEARCH_MUSCULARAUGMENTS"; // 259
        case ABILITY_ID::TRAIN_DRONE:                       return "TRAIN_DRONE"; // 65935
        case ABILITY_ID::TRAIN_ZERGLING:                    return "TRAIN_ZERGLING"; // 56040
        case ABILITY_ID::TRAIN_OVERLORD:                    return "TRAIN_OVERLORD"; // 15321
        case ABILITY_ID::TRAIN_HYDRALISK:                   return "TRAIN_HYDRALISK"; // 14493
        case ABILITY_ID::TRAIN_MUTALISK:                    return "TRAIN_MUTALISK"; // 5417
        case ABILITY_ID::TRAIN_ULTRALISK:                   return "TRAIN_ULTRALISK"; // 1014
        case ABILITY_ID::TRAIN_ROACH:                       return "TRAIN_ROACH"; // 22649
        case ABILITY_ID::TRAIN_INFESTOR:                    return "TRAIN_INFESTOR"; // 406
        case ABILITY_ID::TRAIN_CORRUPTOR:                   return "TRAIN_CORRUPTOR"; // 2337
        case ABILITY_ID::TRAIN_VIPER:                       return "TRAIN_VIPER"; // 265
        case ABILITY_ID::TRAIN_SWARMHOST:                   return "TRAIN_SWARMHOST"; // 381
        case ABILITY_ID::MORPH_BROODLORD:                   return "MORPH_BROODLORD"; // 673
        case ABILITY_ID::TRAINWARP_ZEALOT:                  return "TRAINWARP_ZEALOT"; // 5917
        case ABILITY_ID::TRAINWARP_STALKER:                 return "TRAINWARP_STALKER"; // 11057
        case ABILITY_ID::TRAINWARP_HIGHTEMPLAR:             return "TRAINWARP_HIGHTEMPLAR"; // 1663
        case ABILITY_ID::TRAINWARP_DARKTEMPLAR:             return "TRAINWARP_DARKTEMPLAR"; // 1673
        case ABILITY_ID::TRAINWARP_SENTRY:                  return "TRAINWARP_SENTRY"; // 1047
        case ABILITY_ID::TRAINWARP_ADEPT:                   return "TRAINWARP_ADEPT"; // 6652
        case ABILITY_ID::MORPH_OVERSEER:                    return "MORPH_OVERSEER"; // 1705
        case ABILITY_ID::MORPH_PLANETARYFORTRESS:           return "MORPH_PLANETARYFORTRESS"; // 618
        case ABILITY_ID::RESEARCH_PATHOGENGLANDS:           return "RESEARCH_PATHOGENGLANDS"; // 86
        case ABILITY_ID::RESEARCH_NEURALPARASITE:           return "RESEARCH_NEURALPARASITE"; // 16
        case ABILITY_ID::RESEARCH_CENTRIFUGALHOOKS:         return "RESEARCH_CENTRIFUGALHOOKS"; // 310
        case ABILITY_ID::MORPH_ORBITALCOMMAND:              return "MORPH_ORBITALCOMMAND"; // 3238
        case ABILITY_ID::MORPH_WARPGATE:                    return "MORPH_WARPGATE"; // 4694
        case ABILITY_ID::MORPH_GATEWAY:                     return "MORPH_GATEWAY"; // 2
        case ABILITY_ID::EFFECT_FORCEFIELD:                 return "EFFECT_FORCEFIELD"; // 897
        case ABILITY_ID::MORPH_WARPPRISMPHASINGMODE:        return "MORPH_WARPPRISMPHASINGMODE"; // 642
        case ABILITY_ID::MORPH_WARPPRISMTRANSPORTMODE:      return "MORPH_WARPPRISMTRANSPORTMODE"; // 403
        case ABILITY_ID::RESEARCH_BATTLECRUISERWEAPONREFIT: return "RESEARCH_BATTLECRUISERWEAPONREFIT"; // 41
        case ABILITY_ID::RESEARCH_WARPGATE:                 return "RESEARCH_WARPGATE"; // 731
        case ABILITY_ID::RESEARCH_CHARGE:                   return "RESEARCH_CHARGE"; // 211
        case ABILITY_ID::RESEARCH_BLINK:                    return "RESEARCH_BLINK"; // 286
        case ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES:   return "RESEARCH_ADEPTRESONATINGGLAIVES"; // 251
        case ABILITY_ID::EFFECT_NUKECALLDOWN:               return "EFFECT_NUKECALLDOWN"; // 26
        case ABILITY_ID::EFFECT_EMP:                        return "EFFECT_EMP"; // 127
        case ABILITY_ID::TRAIN_QUEEN:                       return "TRAIN_QUEEN"; // 5140
        case ABILITY_ID::EFFECT_TRANSFUSION:                return "EFFECT_TRANSFUSION"; // 1297
        case ABILITY_ID::BEHAVIOR_GENERATECREEPON:          return "BEHAVIOR_GENERATECREEPON"; // 222
        case ABILITY_ID::BEHAVIOR_GENERATECREEPOFF:         return "BEHAVIOR_GENERATECREEPOFF"; // 11
        case ABILITY_ID::EFFECT_AUTOTURRET:                 return "EFFECT_AUTOTURRET"; // 1766
        case ABILITY_ID::MORPH_ARCHON:                      return "MORPH_ARCHON"; // 2122
        case ABILITY_ID::BUILD_NYDUSWORM:                   return "BUILD_NYDUSWORM"; // 90
        case ABILITY_ID::EFFECT_CHARGE:                     return "EFFECT_CHARGE"; // 1
        case ABILITY_ID::EFFECT_CONTAMINATE:                return "EFFECT_CONTAMINATE"; // 6
        case ABILITY_ID::MORPH_MOTHERSHIP:                  return "MORPH_MOTHERSHIP"; // 31
        case ABILITY_ID::TRAIN_MOTHERSHIPCORE:              return "TRAIN_MOTHERSHIPCORE"; // 816
        case ABILITY_ID::MORPH_HELLION:                     return "MORPH_HELLION"; // 46
        case ABILITY_ID::MORPH_HELLBAT:                     return "MORPH_HELLBAT"; // 386
        case ABILITY_ID::EFFECT_BLINDINGCLOUD:              return "EFFECT_BLINDINGCLOUD"; // 160
        case ABILITY_ID::EFFECT_ABDUCT:                     return "EFFECT_ABDUCT"; // 124
        case ABILITY_ID::EFFECT_VIPERCONSUME:               return "EFFECT_VIPERCONSUME"; // 484
        case ABILITY_ID::BEHAVIOR_BUILDINGATTACKON:         return "BEHAVIOR_BUILDINGATTACKON"; // 17
        case ABILITY_ID::BEHAVIOR_BUILDINGATTACKOFF:        return "BEHAVIOR_BUILDINGATTACKOFF"; // 1
        case ABILITY_ID::EFFECT_WIDOWMINEATTACK:            return "EFFECT_WIDOWMINEATTACK"; // 1
        case ABILITY_ID::MORPH_LURKERDEN:                   return "MORPH_LURKERDEN"; // 140
        case ABILITY_ID::HALLUCINATION_ORACLE:              return "HALLUCINATION_ORACLE"; // 3
        case ABILITY_ID::EFFECT_MEDIVACIGNITEAFTERBURNERS:  return "EFFECT_MEDIVACIGNITEAFTERBURNERS"; // 3883
        case ABILITY_ID::EFFECT_ORACLEREVELATION:           return "EFFECT_ORACLEREVELATION"; // 93
        case ABILITY_ID::EFFECT_PHOTONOVERCHARGE:           return "EFFECT_PHOTONOVERCHARGE"; // 1249
        case ABILITY_ID::EFFECT_TIMEWARP:                   return "EFFECT_TIMEWARP"; // 65
        case ABILITY_ID::EFFECT_CAUSTICSPRAY:               return "EFFECT_CAUSTICSPRAY"; // 354
        case ABILITY_ID::EFFECT_IMMORTALBARRIER:            return "EFFECT_IMMORTALBARRIER"; // 3
        case ABILITY_ID::MORPH_RAVAGER:                     return "MORPH_RAVAGER"; // 2187
        case ABILITY_ID::MORPH_LURKER:                      return "MORPH_LURKER"; // 854
        case ABILITY_ID::EFFECT_CORROSIVEBILE:              return "EFFECT_CORROSIVEBILE"; // 5889
        case ABILITY_ID::EFFECT_PURIFICATIONNOVA:           return "EFFECT_PURIFICATIONNOVA"; // 549
        case ABILITY_ID::EFFECT_LOCKON:                     return "EFFECT_LOCKON"; // 458
        case ABILITY_ID::EFFECT_TACTICALJUMP:               return "EFFECT_TACTICALJUMP"; // 99
        case ABILITY_ID::MORPH_THORHIGHIMPACTMODE:          return "MORPH_THORHIGHIMPACTMODE"; // 39
        case ABILITY_ID::MORPH_THOREXPLOSIVEMODE:           return "MORPH_THOREXPLOSIVEMODE"; // 3
        case ABILITY_ID::BEHAVIOR_PULSARBEAMON:             return "BEHAVIOR_PULSARBEAMON"; // 352
        case ABILITY_ID::BEHAVIOR_PULSARBEAMOFF:            return "BEHAVIOR_PULSARBEAMOFF"; // 183
        case ABILITY_ID::EFFECT_LOCUSTSWOOP:                return "EFFECT_LOCUSTSWOOP"; // 2
        case ABILITY_ID::HALLUCINATION_DISRUPTOR:           return "HALLUCINATION_DISRUPTOR"; // 1
        case ABILITY_ID::HALLUCINATION_ADEPT:               return "HALLUCINATION_ADEPT"; // 1
        case ABILITY_ID::EFFECT_VOIDRAYPRISMATICALIGNMENT:  return "EFFECT_VOIDRAYPRISMATICALIGNMENT"; // 321
        case ABILITY_ID::BUILD_STASISTRAP:                  return "BUILD_STASISTRAP"; // 103
        case ABILITY_ID::EFFECT_PARASITICBOMB:              return "EFFECT_PARASITICBOMB"; // 118
        case ABILITY_ID::EFFECT_ADEPTPHASESHIFT:            return "EFFECT_ADEPTPHASESHIFT"; // 1040
        case ABILITY_ID::MORPH_LIBERATORAGMODE:             return "MORPH_LIBERATORAGMODE"; // 3595
        case ABILITY_ID::MORPH_LIBERATORAAMODE:             return "MORPH_LIBERATORAAMODE"; // 1355
        case ABILITY_ID::EFFECT_KD8CHARGE:                  return "EFFECT_KD8CHARGE"; // 2773
        case ABILITY_ID::EFFECT_TEMPESTDISRUPTIONBLAST:     return "EFFECT_TEMPESTDISRUPTIONBLAST"; // 4
        case ABILITY_ID::EFFECT_SPAWNLOCUSTS:               return "EFFECT_SPAWNLOCUSTS"; // 185
        case ABILITY_ID::MORPH_OVERLORDTRANSPORT:           return "MORPH_OVERLORDTRANSPORT"; // 219
        case ABILITY_ID::EFFECT_GHOSTSNIPE:                 return "EFFECT_GHOSTSNIPE"; // 206
        case ABILITY_ID::RESEARCH_SHADOWSTRIKE:             return "RESEARCH_SHADOWSTRIKE"; // 19
        case ABILITY_ID::CANCEL:                            return "CANCEL"; // 1683
        case ABILITY_ID::HALT:                              return "HALT"; // 224
        case ABILITY_ID::BURROWDOWN:                        return "BURROWDOWN"; // 3976
        case ABILITY_ID::BURROWUP:                          return "BURROWUP"; // 1549
        case ABILITY_ID::LOADALL:                           return "LOADALL"; // 40
        case ABILITY_ID::UNLOADALL:                         return "UNLOADALL"; // 796
        case ABILITY_ID::STOP:                              return "STOP"; // 5950
        case ABILITY_ID::HARVEST_GATHER:                    return "HARVEST_GATHER"; // 580
        case ABILITY_ID::HARVEST_RETURN:                    return "HARVEST_RETURN"; // 2392
        case ABILITY_ID::LOAD:                              return "LOAD"; // 6
        case ABILITY_ID::UNLOADALLAT:                       return "UNLOADALLAT"; // 4938
        case ABILITY_ID::UNLOADUNIT:                        return "UNLOADUNIT"; // 484
        case ABILITY_ID::CANCEL_LAST:                       return "CANCEL_LAST"; // 6153
        case ABILITY_ID::CANCEL_SLOT:                       return "CANCEL_SLOT"; // 1438
        case ABILITY_ID::RALLY_UNITS:                       return "RALLY_UNITS"; // 1632
        case ABILITY_ID::ATTACK:                            return "ATTACK"; // 219285
        case ABILITY_ID::EFFECT_STIM:                       return "EFFECT_STIM"; // 6631
        case ABILITY_ID::BEHAVIOR_CLOAKON:                  return "BEHAVIOR_CLOAKON"; // 466
        case ABILITY_ID::BEHAVIOR_CLOAKOFF:                 return "BEHAVIOR_CLOAKOFF"; // 185
        case ABILITY_ID::LAND:                              return "LAND"; // 3383
        case ABILITY_ID::LIFT:                              return "LIFT"; // 4290
        case ABILITY_ID::MORPH_ROOT:                        return "MORPH_ROOT"; // 711
        case ABILITY_ID::MORPH_UPROOT:                      return "MORPH_UPROOT"; // 567
        case ABILITY_ID::BUILD_TECHLAB:                     return "BUILD_TECHLAB"; // 3484
        case ABILITY_ID::BUILD_REACTOR:                     return "BUILD_REACTOR"; // 3510
        case ABILITY_ID::EFFECT_SPRAY:                      return "EFFECT_SPRAY"; // 27
        case ABILITY_ID::EFFECT_REPAIR:                     return "EFFECT_REPAIR"; // 1051
        case ABILITY_ID::EFFECT_MASSRECALL:                 return "EFFECT_MASSRECALL"; // 75
        case ABILITY_ID::EFFECT_BLINK:                      return "EFFECT_BLINK"; // 1512
        case ABILITY_ID::BEHAVIOR_HOLDFIREON:               return "BEHAVIOR_HOLDFIREON"; // 22
        case ABILITY_ID::BEHAVIOR_HOLDFIREOFF:              return "BEHAVIOR_HOLDFIREOFF"; // 1
        case ABILITY_ID::RALLY_WORKERS:                     return "RALLY_WORKERS"; // 1744
        case ABILITY_ID::BUILD_CREEPTUMOR:                  return "BUILD_CREEPTUMOR"; // 16683
        case ABILITY_ID::RESEARCH_PROTOSSAIRARMOR:          return "RESEARCH_PROTOSSAIRARMOR"; // 80
        case ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONS:        return "RESEARCH_PROTOSSAIRWEAPONS"; // 173
        case ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR:       return "RESEARCH_PROTOSSGROUNDARMOR"; // 437
        case ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS:     return "RESEARCH_PROTOSSGROUNDWEAPONS"; // 698
        case ABILITY_ID::RESEARCH_PROTOSSSHIELDS:           return "RESEARCH_PROTOSSSHIELDS"; // 177
        case ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR:      return "RESEARCH_TERRANINFANTRYARMOR"; // 1026
        case ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS:    return "RESEARCH_TERRANINFANTRYWEAPONS"; // 1150
        case ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS:        return "RESEARCH_TERRANSHIPWEAPONS"; // 287
        case ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING: return "RESEARCH_TERRANVEHICLEANDSHIPPLATING"; // 350
        case ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS:     return "RESEARCH_TERRANVEHICLEWEAPONS"; // 463
        case ABILITY_ID::RESEARCH_ZERGFLYERARMOR:           return "RESEARCH_ZERGFLYERARMOR"; // 119
        case ABILITY_ID::RESEARCH_ZERGFLYERATTACK:          return "RESEARCH_ZERGFLYERATTACK"; // 242
        case ABILITY_ID::RESEARCH_ZERGGROUNDARMOR:          return "RESEARCH_ZERGGROUNDARMOR"; // 993
        case ABILITY_ID::RESEARCH_ZERGMELEEWEAPONS:         return "RESEARCH_ZERGMELEEWEAPONS"; // 622
        case ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONS:       return "RESEARCH_ZERGMISSILEWEAPONS"; // 734
    }

    return "UNKNOWN";
};

const char* UpgradeIDToName(UpgradeID upgrade_id) {
    switch ((UPGRADE_ID)upgrade_id) {
        case UPGRADE_ID::CARRIERLAUNCHSPEEDUPGRADE:         return "CARRIERLAUNCHSPEEDUPGRADE";
        case UPGRADE_ID::GLIALRECONSTITUTION:               return "GLIALRECONSTITUTION";
        case UPGRADE_ID::TUNNELINGCLAWS:                    return "TUNNELINGCLAWS";
        case UPGRADE_ID::CHITINOUSPLATING:                  return "CHITINOUSPLATING";
        case UPGRADE_ID::HISECAUTOTRACKING:                 return "HISECAUTOTRACKING";
        case UPGRADE_ID::TERRANBUILDINGARMOR:               return "TERRANBUILDINGARMOR";
        case UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1:       return "TERRANINFANTRYWEAPONSLEVEL1";
        case UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2:       return "TERRANINFANTRYWEAPONSLEVEL2";
        case UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL3:       return "TERRANINFANTRYWEAPONSLEVEL3";
        case UPGRADE_ID::NEOSTEELFRAME:                     return "NEOSTEELFRAME";
        case UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1:        return "TERRANINFANTRYARMORSLEVEL1";
        case UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2:        return "TERRANINFANTRYARMORSLEVEL2";
        case UPGRADE_ID::TERRANINFANTRYARMORSLEVEL3:        return "TERRANINFANTRYARMORSLEVEL3";
        case UPGRADE_ID::STIMPACK:                          return "STIMPACK";
        case UPGRADE_ID::SHIELDWALL:                        return "SHIELDWALL";
        case UPGRADE_ID::PUNISHERGRENADES:                  return "PUNISHERGRENADES";
        case UPGRADE_ID::HIGHCAPACITYBARRELS:               return "HIGHCAPACITYBARRELS";
        case UPGRADE_ID::BANSHEECLOAK:                      return "BANSHEECLOAK";
        case UPGRADE_ID::RAVENCORVIDREACTOR:                return "RAVENCORVIDREACTOR";
        case UPGRADE_ID::PERSONALCLOAKING:                  return "PERSONALCLOAKING";
        case UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1:        return "TERRANVEHICLEWEAPONSLEVEL1";
        case UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2:        return "TERRANVEHICLEWEAPONSLEVEL2";
        case UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL3:        return "TERRANVEHICLEWEAPONSLEVEL3";
        case UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1:           return "TERRANSHIPWEAPONSLEVEL1";
        case UPGRADE_ID::TERRANSHIPWEAPONSLEVEL2:           return "TERRANSHIPWEAPONSLEVEL2";
        case UPGRADE_ID::TERRANSHIPWEAPONSLEVEL3:           return "TERRANSHIPWEAPONSLEVEL3";
        case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1:        return "PROTOSSGROUNDWEAPONSLEVEL1";
        case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2:        return "PROTOSSGROUNDWEAPONSLEVEL2";
        case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3:        return "PROTOSSGROUNDWEAPONSLEVEL3";
        case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:         return "PROTOSSGROUNDARMORSLEVEL1";
        case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:         return "PROTOSSGROUNDARMORSLEVEL2";
        case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:         return "PROTOSSGROUNDARMORSLEVEL3";
        case UPGRADE_ID::PROTOSSSHIELDSLEVEL1:              return "PROTOSSSHIELDSLEVEL1";
        case UPGRADE_ID::PROTOSSSHIELDSLEVEL2:              return "PROTOSSSHIELDSLEVEL2";
        case UPGRADE_ID::PROTOSSSHIELDSLEVEL3:              return "PROTOSSSHIELDSLEVEL3";
        case UPGRADE_ID::OBSERVERGRAVITICBOOSTER:           return "OBSERVERGRAVITICBOOSTER";
        case UPGRADE_ID::GRAVITICDRIVE:                     return "GRAVITICDRIVE";
        case UPGRADE_ID::EXTENDEDTHERMALLANCE:              return "EXTENDEDTHERMALLANCE";
        case UPGRADE_ID::PSISTORMTECH:                      return "PSISTORMTECH";
        case UPGRADE_ID::ZERGMELEEWEAPONSLEVEL1:            return "ZERGMELEEWEAPONSLEVEL1";
        case UPGRADE_ID::ZERGMELEEWEAPONSLEVEL2:            return "ZERGMELEEWEAPONSLEVEL2";
        case UPGRADE_ID::ZERGMELEEWEAPONSLEVEL3:            return "ZERGMELEEWEAPONSLEVEL3";
        case UPGRADE_ID::ZERGGROUNDARMORSLEVEL1:            return "ZERGGROUNDARMORSLEVEL1";
        case UPGRADE_ID::ZERGGROUNDARMORSLEVEL2:            return "ZERGGROUNDARMORSLEVEL2";
        case UPGRADE_ID::ZERGGROUNDARMORSLEVEL3:            return "ZERGGROUNDARMORSLEVEL3";
        case UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL1:          return "ZERGMISSILEWEAPONSLEVEL1";
        case UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL2:          return "ZERGMISSILEWEAPONSLEVEL2";
        case UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL3:          return "ZERGMISSILEWEAPONSLEVEL3";
        case UPGRADE_ID::OVERLORDSPEED:                     return "OVERLORDSPEED";
        case UPGRADE_ID::BURROW:                            return "BURROW";
        case UPGRADE_ID::ZERGLINGATTACKSPEED:               return "ZERGLINGATTACKSPEED";
        case UPGRADE_ID::ZERGLINGMOVEMENTSPEED:             return "ZERGLINGMOVEMENTSPEED";
        case UPGRADE_ID::ZERGFLYERWEAPONSLEVEL1:            return "ZERGFLYERWEAPONSLEVEL1";
        case UPGRADE_ID::ZERGFLYERWEAPONSLEVEL2:            return "ZERGFLYERWEAPONSLEVEL2";
        case UPGRADE_ID::ZERGFLYERWEAPONSLEVEL3:            return "ZERGFLYERWEAPONSLEVEL3";
        case UPGRADE_ID::ZERGFLYERARMORSLEVEL1:             return "ZERGFLYERARMORSLEVEL1";
        case UPGRADE_ID::ZERGFLYERARMORSLEVEL2:             return "ZERGFLYERARMORSLEVEL2";
        case UPGRADE_ID::ZERGFLYERARMORSLEVEL3:             return "ZERGFLYERARMORSLEVEL3";
        case UPGRADE_ID::INFESTORENERGYUPGRADE:             return "INFESTORENERGYUPGRADE";
        case UPGRADE_ID::CENTRIFICALHOOKS:                  return "CENTRIFICALHOOKS";
        case UPGRADE_ID::BATTLECRUISERENABLESPECIALIZATIONS: return "BATTLECRUISERENABLESPECIALIZATIONS";
        case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1:           return "PROTOSSAIRWEAPONSLEVEL1";
        case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2:           return "PROTOSSAIRWEAPONSLEVEL2";
        case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3:           return "PROTOSSAIRWEAPONSLEVEL3";
        case UPGRADE_ID::PROTOSSAIRARMORSLEVEL1:            return "PROTOSSAIRARMORSLEVEL1";
        case UPGRADE_ID::PROTOSSAIRARMORSLEVEL2:            return "PROTOSSAIRARMORSLEVEL2";
        case UPGRADE_ID::PROTOSSAIRARMORSLEVEL3:            return "PROTOSSAIRARMORSLEVEL3";
        case UPGRADE_ID::WARPGATERESEARCH:                  return "WARPGATERESEARCH";
        case UPGRADE_ID::CHARGE:                            return "CHARGE";
        case UPGRADE_ID::BLINKTECH:                         return "BLINKTECH";
        case UPGRADE_ID::PHOENIXRANGEUPGRADE:               return "PHOENIXRANGEUPGRADE";
        case UPGRADE_ID::NEURALPARASITE:                    return "NEURALPARASITE";
        case UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1:  return "TERRANVEHICLEANDSHIPARMORSLEVEL1";
        case UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2:  return "TERRANVEHICLEANDSHIPARMORSLEVEL2";
        case UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL3:  return "TERRANVEHICLEANDSHIPARMORSLEVEL3";
        case UPGRADE_ID::DRILLCLAWS:                        return "DRILLCLAWS";
        case UPGRADE_ID::ADEPTPIERCINGATTACK:               return "ADEPTPIERCINGATTACK";
        case UPGRADE_ID::MAGFIELDLAUNCHERS:                 return "MAGFIELDLAUNCHERS";
        case UPGRADE_ID::EVOLVEGROOVEDSPINES:               return "EVOLVEGROOVEDSPINES";
        case UPGRADE_ID::EVOLVEMUSCULARAUGMENTS:            return "EVOLVEMUSCULARAUGMENTS";
        case UPGRADE_ID::BANSHEESPEED:                      return "BANSHEESPEED";
        case UPGRADE_ID::RAVENRECALIBRATEDEXPLOSIVES:       return "RAVENRECALIBRATEDEXPLOSIVES";
        case UPGRADE_ID::MEDIVACINCREASESPEEDBOOST:         return "MEDIVACINCREASESPEEDBOOST";
        case UPGRADE_ID::LIBERATORAGRANGEUPGRADE:           return "LIBERATORAGRANGEUPGRADE";
        case UPGRADE_ID::DARKTEMPLARBLINKUPGRADE:           return "DARKTEMPLARBLINKUPGRADE";
    }

    return "UNKNOWN";
};

const char* BuffIDToName(BuffID buff_id) {
    switch ((BUFF_ID)buff_id) {
        case BUFF_ID::GRAVITONBEAM:                         return "GRAVITONBEAM";
        case BUFF_ID::GHOSTCLOAK:                           return "GHOSTCLOAK";
        case BUFF_ID::BANSHEECLOAK:                         return "BANSHEECLOAK";
        case BUFF_ID::POWERUSERWARPABLE:                    return "POWERUSERWARPABLE";
        case BUFF_ID::QUEENSPAWNLARVATIMER:                 return "QUEENSPAWNLARVATIMER";
        case BUFF_ID::GHOSTHOLDFIREB:                       return "GHOSTHOLDFIREB";
        case BUFF_ID::EMPDECLOAK:                           return "EMPDECLOAK";
        case BUFF_ID::FUNGALGROWTH:                         return "FUNGALGROWTH";
        case BUFF_ID::GUARDIANSHIELD:                       return "GUARDIANSHIELD";
        case BUFF_ID::TIMEWARPPRODUCTION:                   return "TIMEWARPPRODUCTION";
        case BUFF_ID::NEURALPARASITE:                       return "NEURALPARASITE";
        case BUFF_ID::STIMPACKMARAUDER:                     return "STIMPACKMARAUDER";
        case BUFF_ID::SUPPLYDROP:                           return "SUPPLYDROP";
        case BUFF_ID::STIMPACK:                             return "STIMPACK";
        case BUFF_ID::PSISTORM:                             return "PSISTORM";
        case BUFF_ID::CLOAKFIELDEFFECT:                     return "CLOAKFIELDEFFECT";
        case BUFF_ID::CHARGING:                             return "CHARGING";
        case BUFF_ID::SLOW:                                 return "SLOW";
        case BUFF_ID::CONTAMINATED:                         return "CONTAMINATED";
        case BUFF_ID::BLINDINGCLOUDSTRUCTURE:               return "BLINDINGCLOUDSTRUCTURE";
        case BUFF_ID::ORACLEREVELATION:                     return "ORACLEREVELATION";
        case BUFF_ID::VIPERCONSUMESTRUCTURE:                return "VIPERCONSUMESTRUCTURE";
        case BUFF_ID::BLINDINGCLOUD:                        return "BLINDINGCLOUD";
        case BUFF_ID::MEDIVACSPEEDBOOST:                    return "MEDIVACSPEEDBOOST";
        case BUFF_ID::PURIFY:                               return "PURIFY";
        case BUFF_ID::ORACLEWEAPON:                         return "ORACLEWEAPON";
        case BUFF_ID::IMMORTALOVERLOAD:                     return "IMMORTALOVERLOAD";
        case BUFF_ID::LOCKON:                               return "LOCKON";
        case BUFF_ID::SEEKERMISSILE:                        return "SEEKERMISSILE";
        case BUFF_ID::TEMPORALFIELD:                        return "TEMPORALFIELD";
        case BUFF_ID::VOIDRAYSWARMDAMAGEBOOST:              return "VOIDRAYSWARMDAMAGEBOOST";
        case BUFF_ID::ORACLESTASISTRAPTARGET:               return "ORACLESTASISTRAPTARGET";
        case BUFF_ID::PARASITICBOMB:                        return "PARASITICBOMB";
        case BUFF_ID::PARASITICBOMBUNITKU:                  return "PARASITICBOMBUNITKU";
        case BUFF_ID::PARASITICBOMBSECONDARYUNITSEARCH:     return "PARASITICBOMBSECONDARYUNITSEARCH";
        case BUFF_ID::LURKERHOLDFIREB:                      return "LURKERHOLDFIREB";
        case BUFF_ID::CHANNELSNIPECOMBAT:                   return "CHANNELSNIPECOMBAT";
        case BUFF_ID::TEMPESTDISRUPTIONBLASTSTUNBEHAVIOR:   return "TEMPESTDISRUPTIONBLASTSTUNBEHAVIOR";
        case BUFF_ID::CARRYMINERALFIELDMINERALS:            return "CARRYMINERALFIELDMINERALS";
        case BUFF_ID::CARRYHIGHYIELDMINERALFIELDMINERALS:   return "CARRYHIGHYIELDMINERALFIELDMINERALS";
        case BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGAS:     return "CARRYHARVESTABLEVESPENEGEYSERGAS";
        case BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGASPROTOSS: return "CARRYHARVESTABLEVESPENEGEYSERGASPROTOSS";
        case BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGASZERG: return "CARRYHARVESTABLEVESPENEGEYSERGASZERG";
    }

    return "UNKNOWN";
};

}

