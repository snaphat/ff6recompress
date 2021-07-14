#include "json.hpp"
#include "freespace.cpp"
using nlohmann::json;

class JsonWrap {
    private:
        json config;
        FreeSpace freespace;

    public:
        JsonWrap() {
            this->config = json::parse("{}");
        };

        JsonWrap(const char* _config) {
            this->config = json::parse(_config);

            for (auto r : config["freeSpace"]) {
                auto [beg, end] = to_u32_pair(r);
                freespace.add(beg, end);
            }
        };

        auto add_freespace(uint32 beg, uint32 end) -> void {
            freespace.add(beg, end);

            config["freeSpace"].clear();
            config["freeSpace"] = freespace.emit();
        }

        auto insert(auto field, auto start, auto end) {
            std::stringstream ss;
            ss << std::uppercase << std::hex << "0x" << start << "-" << "0x" << end;
            config["assembly"][field]["range"].clear();
            config["assembly"][field]["range"] = ss.str();
        }

        auto extract(auto field) {
            auto j_entry       = config["assembly"][field];
            auto j_field_name  = j_entry["name"];
            auto j_field_range = j_entry["range"];
            auto j_array_len1  = j_entry["arrayLength"];
            auto j_array_len2  = j_entry["array"]["length"];
            auto j_table       = j_entry["pointerTable"];
            auto j_tbl_range   = j_table["range"];
            auto j_tbl_offset  = j_table["offset"];
            auto j_tbl_ptr_len = j_table["pointerLength"];
            optEntry maybe_entry;
            if(j_field_name.is_string() && j_field_range.is_string()) {
                auto field_name    = j_field_name;
                auto field_range   = to_u32_pair(j_field_range);
                optPtrTbl maybe_tbl;
                if (j_tbl_range.is_string()) {
                    auto tbl_range      = to_u32_pair(j_tbl_range);
                    auto tbl_offset     = j_tbl_offset.is_string()  ? to_u32(j_tbl_offset)                 : 0;
                    auto tbl_ptr_len    = j_tbl_ptr_len.is_number() ? j_tbl_ptr_len.template get<uint32>() : 2;
                    auto array_len      = j_array_len1.is_number()  ? j_array_len1.template get<uint32>()
                                        : j_array_len2.is_number()  ? j_array_len2.template get<uint32>()  : 1;
                    maybe_tbl.emplace(std::make_tuple(tbl_range, tbl_offset, tbl_ptr_len, array_len));
                }
                maybe_entry.emplace(move(std::make_tuple(move(field_name), move(field_range), move(maybe_tbl))));
            }


            return maybe_entry;
        }

        auto save(auto filename) {
            auto json = config.dump(4);
            // Write json.
            auto fd = fopen((filename+".json").c_str(), "wb");
            fwrite(json.c_str(), 1, json.length(), fd);
            fclose(fd);
        }

};

auto _config = R"^({
    "type": "rom",
    "key": "ff3u-sfc",
    "name": "Final Fantasy III 1.0 (U)",
    "length": "0x300000",
    "crc32": "0xA27F1C7A",
    "system": "sfc",
    "mode": "hiROM",
    "hierarchy": [
        {
            "name": "Map",
            "list": [
                {
                    "name": "Maps",
                    "path": "mapProperties"
                }, {
                    "name": "Map Tilesets",
                    "path": "mapTilesets"
                }, {
                    "name": "Map Titles",
                    "path": "mapTitle"
                }, {
                    "name": "Map Parallax",
                    "path": "mapParallax"
                }, {
                    "name": "Map Color Math",
                    "path": "mapColorMath"
                }, {
                    "name": "Map Animation Properties",
                    "path": "mapAnimationProperties"
                }, {
                    "name": "Map Animation Graphics (Layer 3)",
                    "path": "mapAnimationGraphicsLayer3"
                }, {
                    "name": "Map Animation Graphics",
                    "path": "mapAnimationGraphics"
                }, {
                    "name": "Map Palette Animation",
                    "path": "mapPaletteAnimation"
                }, {
                    "name": "Sprite Overlay Graphics",
                    "path": "mapOverlayGraphics"
                }, {
                    "name": "World of Balance Tileset",
                    "path": "worldGraphics1.tileset"
                }, {
                    "name": "World of Ruin Tileset",
                    "path": "worldGraphics2.tileset"
                }, {
                    "name": "Serpent Trench Tileset",
                    "path": "worldGraphics3.tileset"
                }, {
                    "name": "World Map Sky",
                    "path": "worldCloudsLayout"
                }
            ]
        }, {
            "name": "Event",
            "list": [
                {
                    "name": "Event Script",
                    "path": "eventScript"
                }, {
                    "name": "Dialogue",
                    "path": "dialog"
                }, {
                    "name": "NPC Switches",
                    "path": "stringTable.npcSwitches"
                }, {
                    "name": "Map Switches",
                    "path": "stringTable.mapSwitches"
                }
            ]
        }, {
            "name": "Battle",
            "list": [
                {
                    "name": "Battles",
                    "path": "battleProperties"
                }, {
                    "name": "Random Battle Groups",
                    "path": "battleGroupsRandom"
                }, {
                    "name": "Event Battle Groups",
                    "path": "battleGroupsEvent"
                }, {
                    "name": "Battle Dialogue",
                    "path": "battleDialog"
                }, {
                    "name": "Battle Backgrounds",
                    "path": "battleBackgroundProperties"
                }
            ]
        }, {
            "name": "Monster",
            "list": [
                {
                    "name": "Monsters",
                    "path": "monsterProperties"
                }, {
                    "name": "Monster/Esper Graphics",
                    "path": "monsterGraphics"
                }, {
                    "name": "Monster A.I. Script",
                    "path": "monsterScript"
                }, {
                    "name": "Character A.I.",
                    "path": "characterAI"
                }, {
                    "name": "Monster Dialogue",
                    "path": "monsterDialog"
                }
            ]
        }, {
            "name": "Character",
            "list": [
                {
                    "name": "Characters",
                    "path": "characterProperties"
                }, {
                    "name": "Character Graphics",
                    "path": "mapSpriteGraphics"
                }, {
                    "name": "Character Portraits",
                    "path": "portraitGraphics"
                }, {
                    "name": "Vehicle Graphics",
                    "path": "vehicleGraphics"
                }, {
                    "name": "Level Progression",
                    "path": "characterLevelProgression"
                }
            ]
        }, {
            "name": "Ability",
            "list": [
                {
                    "path": "attackProperties"
                }, {
                    "name": "Attack Messages",
                    "path": "attackMessage"
                }, {
                    "name": "Espers",
                    "path": "esperProperties"
                }, {
                    "name": "Esper Bonus Names",
                    "path": "esperBonusNames"
                }, {
                    "name": "Natural Magic",
                    "path": "naturalMagic"
                }, {
                    "name": "Dance",
                    "path": "danceAttacks"
                }, {
                    "path": "attackTilemap3bpp"
                }, {
                    "path": "attackTilemap2bpp"
                }
            ]
        }, {
            "name": "Item",
            "list": [
                {
                    "name": "Items",
                    "path": "itemProperties"
                }, {
                    "name": "Rare Items",
                    "path": "rareItemName"
                }, {
                    "name": "Shops",
                    "path": "shopProperties"
                }, {
                    "name": "Colosseum Items",
                    "path": "itemColosseum"
                }, {
                    "name": "Metamorphosis Items",
                    "path": "itemMetamorphosis"
                }, {
                    "name": "Manual Equip Items",
                    "path": "itemManualEquip"
                }
            ]
        }, {
            "name": "Cinematic",
            "list": [
                {
                    "path": "titleIntroGraphics.introLayout",
                    "name": "Title Screen: Background"
                }, {
                    "path": "titleIntroGraphics.flames2bppLayout",
                    "name": "Title Screen: Flames (2bpp)"
                }, {
                    "path": "titleIntroGraphics.flames4bppLayout",
                    "name": "Title Screen: Flames (4bpp)"
                }, {
                    "path": "titleIntroGraphics.splashLayout",
                    "name": "Title Screen: Game Title"
                }, {
                    "path": "titleIntroGraphics.splashSprites",
                    "name": "Title Screen: Sprites"
                }, {
                    "path": "titleIntroGraphics.magitekLayout",
                    "name": "Opening Credits: Background"
                }, {
                    "path": "titleIntroGraphics.magitekSpriteGraphics",
                    "name": "Opening Credits: Sprites"
                }, {
                    "path": "titleIntroGraphics.introFontGraphics",
                    "name": "Opening Credits: Font"
                }, {
                    "path": "vectorApproachLayout",
                    "name": "Vector Approach"
                }, {
                    "path": "floatingIslandCinematic.layout",
                    "name": "Floating Island"
                }, {
                    "path": "worldOfRuinCinematic.layout",
                    "name": "World of Ruin: Background"
                }, {
                    "path": "worldOfRuinCinematic.spriteGraphics",
                    "name": "World of Ruin: Sprites"
                }, {
                    "path": "endingGraphics.tableLayout",
                    "name": "Ending: Background"
                }, {
                    "path": "endingGraphics.miscLayout",
                    "name": "Ending: Misc."
                }, {
                    "path": "endingGraphics.bookLayout",
                    "name": "Ending: Book"
                }, {
                    "path": "endingGraphics.barsLayout",
                    "name": "Ending: Bars"
                }, {
                    "path": "creditsGraphics.landLayout",
                    "name": "Ending Credits: Land"
                }, {
                    "path": "creditsGraphics.cloudsLayout",
                    "name": "Ending Credits: Clouds"
                }, {
                    "path": "creditsGraphics.seaLayout",
                    "name": "Ending Credits: Sea"
                }, {
                    "path": "creditsGraphics.airshipLayout1",
                    "name": "Ending Credits: Airship 1"
                }, {
                    "path": "creditsGraphics.airshipLayout2",
                    "name": "Ending Credits: Airship 2"
                }, {
                    "path": "theEndGraphics1"
                }, {
                    "path": "theEndGraphics2"
                }
            ]
        }, {
            "name": "System",
            "list": [
                {
                    "name": "SNES Header",
                    "path": "snesHeader"
                }, {
                    "path": "fontGraphicsSmall"
                }, {
                    "path": "fontGraphicsLarge"
                }, {
                    "name": "Menu Window",
                    "path": "windowGraphics"
                }
            ]
        }
    ],
    "assembly": {
        "attackGraphics2bpp": {
            "type": "graphics",
            "name": "Attack Graphics (2bpp)",
            "range": "0xD87000-0xD8C9A0",
            "format": "snes2bpp",
            "palette": "attackPalette"
        },
        "attackGraphics3bpp": {
            "type": "graphics",
            "name": "Attack Graphics (3bpp)",
            "range": "0xD30000-0xD4CA00",
            "format": "snes3bpp",
            "palette": "attackPalette"
        },
        "attackName": {
            "type": "array",
            "name": "Attack Name",
            "range": "0xE6F7B9-0xE6FE8F",
            "array": {
                "length": 175
            },
            "assembly": {
                "type": "text",
                "encoding": "bigText",
                "length": 10
            },
            "stringTable": {
                "length": 256,
                "default": "<attackName[%i-0x51]>",
                "string": {
                    "0x00-0x36": "<spellNames[%i]>",
                    "0x36-0x51": "<esperNames[%i-0x36]> (<esperAttackName[%i-0x36]>)",
                    "0x55-0x5D": "<swdTechNames[%i-0x55]>",
                    "0xFF": "Nothing"
                }
            },
            "reference": [
                {
                    "begin": "0xC15FDE",
                    "mask": "0xFFFFFF"
                }, {
                    "begin": "0xC1650B",
                    "mask": "0xFFFFFF",
                    "offset": -500
                }, {
                    "begin": "0xC15FDE",
                    "mask": "0xFFFFFF"
                }, {
                    "begin": "0xC165C0",
                    "mask": "0xFFFFFF"
                }, {
                    "begin": "0xC16671",
                    "mask": "0xFFFFFF",
                    "offset": -580
                }, {
                    "begin": "0xC16AEE",
                    "mask": "0xFFFFFF"
                }, {
                    "begin": "0xC3526C",
                    "mask": "0xFFFF",
                    "offset": -580
                }, {
                    "begin": "0xC35271",
                    "mask": "0xFF",
                    "bankByte": true
                }
            ]
        },
        "attackMessage": {
            "type": "array",
            "name": "Attack Message",
            "range": "0xD1F000-0xD1F751",
            "pointerAlign": "0x10000",
            "array": {
                "length": 256
            },
            "assembly": {
                "type": "text",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xD1F7A0-0xD1F9A0",
                "offset": "0xD10000"
            },
            "stringTable": {
                "length": 256,
                "default": "<attackMessage[%i]>"
            }
        },
        "attackPalette": {
            "type": "array",
            "name": "Attack Palette",
            "range": "0xD26000-0xD26F00",
            "arrayLength": 240,
            "assembly": {
                "type": "assembly",
                "length": 16,
                "format": "bgr555"
            },
            "stringTable": {
                "length": 240,
                "default": "Attack Palette %i",
                "string": {
                    "0": "Generic Tools",
                    "1": "Drill/Air Anchor",
                    "2": "Debilitator (Sprite)",
                    "3": "Auto Crossbow",
                    "5": "Debilitator (Layer 1)",
                    "8": "Fire Dance, Black Magic",
                    "9": "Spiraler",
                    "10": "Mantra",
                    "11": "Air Blade (Layer 1)",
                    "12": "Aura Bolt, White/Effect Magic, Runic, etc. (Sprite)",
                    "15": "Rage/Lore, SwdTech",
                    "16": "Revenge, Morph",
                    "17": "Shock (Layer 1)",
                    "20": "Shock (Sprite)",
                    "21": "Blitz",
                    "24": "Weapon (Brown/Blue)",
                    "25": "Weapon (Red/Green)",
                    "28": "Full Moon",
                    "29": "Weapon (Gray/Blue)",
                    "30": "Dice",
                    "31": "Atma Weapon",
                    "32": "Weapon (Gold)",
                    "33": "Assassin",
                    "34": "Weapon (Blue)",
                    "36": "Rising Sun",
                    "37": "Weapon (Yellow)",
                    "38": "Bone Club",
                    "40": "Weapon (Brown/Red)",
                    "41": "Weapon (Red)",
                    "42": "Weapon (Gray)",
                    "43": "Aura Lance",
                    "44": "Ice Rod, Pearl Rod",
                    "45": "Fire Rod",
                    "46": "Tack Star",
                    "48": "Diffuser, Riot Blade (Layer 1)",
                    "49": "Flash Rain (Layer 3)",
                    "50": "Blue Hit",
                    "51": "Stunner, Tek Laser",
                    "52": "Red Hit",
                    "53": "Purple Hit",
                    "54": "Black & White Hit",
                    "55": "Bolt Edge (Layer 3)",
                    "56": "Fire, Tri-Dazer, Fire Skean, etc.",
                    "57": "Fire Beam, L.4 Flare, etc. (Sprite)",
                    "58": "Scan, Mute, Rflect, Quick, etc. (Sprite)",
                    "59": "Remedy (Sprite)",
                    "60": "Hope Song, Cat Rain (Sprite)",
                    "62": "Haste, Haste 2 (Sprite)",
                    "63": "Muddle, Float, Life, Life 2, Life 3, etc.",
                    "65": "Regen (Sprite)",
                    "67": "Tri-Dazer, True Edge (Layer 3)",
                    "68": "Hope Song (Layer 3)",
                    "69": "TekBarrier (Sprite)",
                    "73": "Rasp",
                    "74": "Regen (Layer 3)",
                    "75": "Haste (Layer 3)",
                    "76": "Drain, Antdot (Sprite)",
                    "78": "Slow, ForceField (Layer 3)",
                    "80": "W Wind",
                    "81": "Fire 2, Flare, Reviver, Sun Bath",
                    "82": "Ice, Pearl, Moon Song, etc. (Sprite)",
                    "83": "Fire 3, Inferno (Layer 1)",
                    "84": "Ice 2, Dispel (Sprite)",
                    "85": "Bio, Imp (Sprite)",
                    "86": "Ice, Ice 2, Bolt 3 (Layer 1)",
                    "88": "Group Hug (Layer 1)",
                    "89": "Meteor, Grand Train (Layer 1)",
                    "90": "Drain (Layer 1)",
                    "92": "Shell (Layer 1)",
                    "93": "Scan, Remedy, Heal Horn, etc. (Layer 1)",
                    "94": "X-Zone, Meteor, Gem Dust (Layer 3)",
                    "95": "Stop",
                    "96": "TekBarrier (Layer 1)",
                    "97": "Bolt, Bolt 2, Bolt 3 (Sprite)",
                    "99": "Mind Blast",
                    "100": "Meteor, Land Slide (Sprite)",
                    "101": "Quake",
                    "102": "Flare Star (Layer 1)",
                    "103": "Chaos Wing, Engulf (Layer 1)",
                    "104": "Generic Light Green Palette",
                    "105": "Bolt 2, Elf Fire (Layer 1)",
                    "106": "Break, Earth Wall, Cave In (Layer 1)",
                    "108": "Merton",
                    "110": "Ultima, Gem Dust, Storm (Layer 1)",
                    "111": "Doom, Roulette (Sprite)",
                    "112": "L? Pearl (Layer 3)",
                    "113": "Ultima (Layer 3)",
                    "115": "Earth Aura (Layer 1)",
                    "117": "Life Guard (Layer 1)",
                    "119": "Rebirth (Layer 3)",
                    "120": "Generic Blue Palette",
                    "122": "Break (Sprite)",
                    "123": "Bolt Fist, Bolt Edge (Layer 1)",
                    "126": "Justice (Layer 1)",
                    "127": "Sun Flare, WaveCannon, etc. (Layer 3)",
                    "128": "Rebirth (Layer 1)",
                    "129": "Condemned (Layer 3)",
                    "130": "Osmose (Layer 1)",
                    "131": "Chaos Wing (Layer 3)",
                    "132": "Metamorph, Heal Horn (Layer 3)",
                    "134": "Bolt Fist, Sea Song (Layer 3)",
                    "136": "Layer 3 Mask",
                    "137": "Metamorph (Sprite)",
                    "138": "Snare (Layer 3)",
                    "139": "Snare (Layer 1)",
                    "141": "Antlion",
                    "142": "Slide",
                    "143": "Rage, Inviz Edge",
                    "144": "Whump (Sprite)",
                    "145": "Cokatrice (Sprite)",
                    "146": "Wombat (Sprite)",
                    "147": "Tapir (Layer 1)",
                    "148": "Kitty, Wild Bear (Layer 1)",
                    "149": "Lagomorph",
                    "150": "Ice Rabbit (Layer 1)",
                    "151": "Wild Frog (Layer 1)",
                    "152": "Grav Bomb (Layer 1)",
                    "153": "Bio Blast",
                    "154": "Grav Bomb (Sprite)",
                    "155": "Confuser",
                    "156": "X-fer",
                    "157": "Slimer",
                    "158": "Harvester, Sonic Boom (Sprite)",
                    "159": "Bolt Beam (Layer 1)",
                    "160": "Ice Beam (Layer 3)",
                    "161": "Fire Beam/Bolt Beam (Layer 3)",
                    "162": "Tek Missile, Launcher, etc.",
                    "164": "Quasar (Layer 1)",
                    "166": "Condemned (Layer 1)",
                    "167": "Imp Song, Lullaby (Layer 1)",
                    "168": "Acid Rain",
                    "169": "Delta Hit",
                    "170": "Rippler (Layer 1)",
                    "171": "Aero, Sneeze (Layer 1)",
                    "173": "Quasar (Layer 3)",
                    "174": "Goner (Layer 1)",
                    "175": "Cyclonic",
                    "176": "S.Cross (Layer 1)",
                    "177": "Disaster",
                    "179": "Tentacle",
                    "180": "Terra/Tritoch Lightning",
                    "181": "Blizzard Fist",
                    "182": "S.Cross (Sprite)",
                    "184": "H-Bomb",
                    "185": "Aqua Rake",
                    "186": "Chocobop",
                    "187": "Charm",
                    "188": "Moogle Rush",
                    "191": "Takedown",
                    "192": "7-Flush, X-Potion, Elixir, etc.",
                    "193": "Ether, Antidote",
                    "194": "Potion, Soft",
                    "195": "Tonic",
                    "196": "Tincture",
                    "197": "Summon, X-Ether",
                    "198": "Green Cherry",
                    "199": "Evil Toot",
                    "200": "Quasar (Sprite)",
                    "201": "Ice 3, Shock Wave",
                    "202": "Osmose, Soul Out (Sprite)",
                    "203": "Gale Cut (Layer 1)",
                    "204": "BabaBreath",
                    "205": "Lifeshaver",
                    "206": "Wind Slash (Layer 1)",
                    "207": "Sand Storm",
                    "208": "Step Mine",
                    "209": "Super Ball",
                    "210": "Fire 3, Demon Eye (Sprite)",
                    "211": "Demi, Quartr",
                    "212": "Slow, Slow 2 (Sprite)",
                    "216": "Safe (Layer 1)",
                    "218": "Big Guard (Layer 1)",
                    "219": "Wall (Sprite)",
                    "220": "Bserk",
                    "221": "Haste 2 (Layer 3)",
                    "222": "Slow 2 (Layer 3)",
                    "225": "Safe, Aero (Sprite)",
                    "226": "ChokeSmoke",
                    "227": "Vanish",
                    "228": "Mute, Megazerk (Layer 1)",
                    "229": "Hyperdrive",
                    "230": "Overcast (Sprite)",
                    "231": "L.5 Doom",
                    "232": "Purifier (Layer 3)",
                    "233": "Justice (Layer 3)",
                    "234": "Sour Mouth (Layer 3)",
                    "236": "Transforming into Magicite",
                    "237": "Lightning Bolt (Kefka vs. Gestahl)",
                    "238": "Bio Blaster",
                    "239": "Kefka Head, Antdot (Sprite)"
                }
            }
        },
        "attackProperties": {
            "type": "array",
            "name": "Attack Properties",
            "range": "0xC46AC0-0xC478C0",
            "array": {
                "length": 256
            },
            "stringTable": "attackName",
            "assembly": {
                "type": "data",
                "length": 14,
                "assembly": {
                    "spellName": {
                        "type": "assembly",
                        "name": "Name",
                        "external": "spellNames[%i]",
                        "invalid": "this.parent.i >= 0x36"
                    },
                    "spellDescription": {
                        "type": "assembly",
                        "name": "Description",
                        "external": "spellDescriptions[%i]",
                        "invalid": "this.parent.i >= 0x36"
                    },
                    "esperAttackName": {
                        "type": "assembly",
                        "name": "Name",
                        "external": "esperAttackName[%i-0x36]",
                        "invalid": "this.parent.i < 0x36 || this.parent.i >= 0x51"
                    },
                    "esperAttackDescription": {
                        "type": "assembly",
                        "name": "Description",
                        "external": "esperAttackDescriptions[%i-0x36]",
                        "invalid": "this.parent.i < 0x36 || this.parent.i >= 0x51"
                    },
                    "miscName": {
                        "type": "assembly",
                        "name": "Name",
                        "external": "attackName[%i-0x51]",
                        "invalid": "this.parent.i < 0x51 || this.parent.i >= 0x55"
                    },
                    "swdTechName": {
                        "type": "assembly",
                        "name": "Name",
                        "external": "swdTechNames[%i-0x55]",
                        "invalid": "this.parent.i < 0x55 || this.parent.i >= 0x5D"
                    },
                    "swdTechDescription": {
                        "type": "assembly",
                        "name": "Description",
                        "external": "swdTechDescription[%i-0x55]",
                        "invalid": "this.parent.i < 0x55 || this.parent.i >= 0x5D"
                    },
                    "swdTechLevel": {
                        "type": "assembly",
                        "external": "swdTechLevel[%i-0x55]",
                        "invalid": "this.parent.i < 0x55 || this.parent.i >= 0x5D"
                    },
                    "attackName": {
                        "type": "assembly",
                        "name": "Name",
                        "external": "attackName[%i-0x51]",
                        "invalid": "this.parent.i < 0x5D"
                    },
                    "blitzDescription": {
                        "type": "assembly",
                        "name": "Description",
                        "external": "blitzDescription[%i-0x5D]",
                        "invalid": "this.parent.i < 0x5D || this.parent.i >= 0x65"
                    },
                    "blitzCode": {
                        "type": "assembly",
                        "external": "blitzCode[%i-0x5D]",
                        "invalid": "this.parent.i < 0x5D || this.parent.i >= 0x65"
                    },
                    "loreDescription": {
                        "type": "assembly",
                        "name": "Description",
                        "external": "loreDescription[%i-0x8B]",
                        "invalid": "this.parent.i < 0x8B || this.parent.i >= 0xA3"
                    },
                    "initialLore": {
                        "type": "assembly",
                        "external": "initialLore[%i-0x8B]",
                        "invalid": "this.parent.i < 0x8B || this.parent.i >= 0xA3"
                    },
                    "mpCost": {
                        "type": "property",
                        "name": "MP Cost",
                        "begin": 5
                    },
                    "spellPower": {
                        "type": "property",
                        "name": "Spell Power",
                        "begin": 6
                    },
                    "hitRate": {
                        "type": "property",
                        "name": "Hit Rate",
                        "begin": 8
                    },
                    "specialEffect": {
                        "type": "property",
                        "name": "Special Effect",
                        "begin": 9,
                        "mask": "0xFF",
                        "stringTable": {
                            "length": "88",
                            "default": "Attack Special Effect %i",
                            "hideIndex": true,
                            "string": {
                                "0-16": "<stringTable.weaponSpecialEffect[%i]>",
                                "16": "<stringTable.attackName[24]>",
                                "17": "<stringTable.attackName[76]>",
                                "18": "<stringTable.attackName[70]>",
                                "19": "<stringTable.attackName[63]>",
                                "21": "<stringTable.attackName[97]>",
                                "22": "<stringTable.attackName[99]>",
                                "23": "<stringTable.attackName[120]>",
                                "24": "<stringTable.attackName[42]>",
                                "25": "<stringTable.attackName[162]>",
                                "26": "<stringTable.attackName[144]>",
                                "27": "<stringTable.attackName[147]>",
                                "28": "<stringTable.attackName[151]>",
                                "29": "<stringTable.attackName[152]>",
                                "30": "<stringTable.attackName[153]>",
                                "31": "<stringTable.attackName[155]>",
                                "32": "<stringTable.attackName[157]>",
                                "33": "<stringTable.attackName[158]>",
                                "34": "<stringTable.attackName[159]>",
                                "35": "Disable Counterattack",
                                "36": "<stringTable.attackName[69]>",
                                "37": "Miss Flying Targets",
                                "38": "<stringTable.attackName[193]>",
                                "39": "<stringTable.attackName[194]>",
                                "40": "<stringTable.attackName[196]>",
                                "41": "<stringTable.attackName[197]>",
                                "42": "<stringTable.attackName[198]>",
                                "43": "<stringTable.attackName[201]>",
                                "44": "<stringTable.attackName[205]>",
                                "45": "<stringTable.attackName[199]>",
                                "46": "<stringTable.attackName[200]>",
                                "47": "<stringTable.attackName[202]>",
                                "48": "<stringTable.attackName[95]>",
                                "49": "<stringTable.attackName[154]>",
                                "50": "<stringTable.attackName[88]>, <stringTable.attackName[91]>",
                                "51": "<stringTable.attackName[234]>",
                                "52": "<stringTable.attackName[206]>",
                                "53": "<stringTable.attackName[13]>",
                                "54": "<stringTable.attackName[89]>",
                                "55": "<stringTable.attackName[220]>",
                                "56": "<stringTable.attackName[203]>",
                                "57": "<stringTable.attackName[213]>",
                                "58": "<stringTable.attackName[218]>",
                                "59": "<stringTable.attackName[211]>",
                                "60": "<stringTable.attackName[86]>",
                                "61": "<stringTable.attackName[146]>",
                                "62": "<stringTable.attackName[225]>",
                                "63": "<stringTable.attackName[90]>",
                                "64": "<stringTable.attackName[192]>",
                                "65": "1/2 Damage",
                                "66": "1/4 Damage",
                                "67": "<stringTable.attackName[43]>",
                                "68": "<stringTable.attackName[219]>",
                                "69": "<stringTable.attackName[164]>",
                                "72-88": "<stringTable.itemSpecialEffect[%i-72]>",
                                "255": "None"
                            }
                        }
                    },
                    "statusBlock": {
                        "type": "property",
                        "name": "Miss if Immune to Status",
                        "begin": 7,
                        "mask": "0x01",
                        "bool": true
                    },
                    "showMessage": {
                        "type": "property",
                        "name": "Show Message",
                        "begin": 7,
                        "mask": "0x02",
                        "bool": true
                    },
                    "attackMessage": {
                        "type": "assembly",
                        "name": "Message",
                        "external": "attackMessage[%i]",
                        "invalid": "!this.parent.showMessage.value"
                    },
                    "targeting": {
                        "type": "property",
                        "name": "Targeting",
                        "begin": 0,
                        "flag": true,
                        "stringTable": "targetingFlags"
                    },
                    "flags": {
                        "type": "property",
                        "name": "Flags",
                        "begin": 2,
                        "mask": "0xFFFFFF",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Physical Damage",
                                "1": "Instant Death",
                                "2": "Resurrection",
                                "3": "Invert Undead Damage",
                                "4": "Random Target",
                                "5": "Ignore Defense",
                                "6": "No Damage Split",
                                "7": "Target Monsters Only",
                                "8": "Available in Menu",
                                "9": "Ignore Reflect",
                                "10": "Lore",
                                "11": "Enable Runic",
                                "12": "Quick/Warp",
                                "13": "Auto-Retarget",
                                "14": "Air Anchor",
                                "15": "Affect MP",
                                "16": "Restore HP/MP",
                                "17": "Drain",
                                "18": "Remove Status",
                                "19": "Toggle Status",
                                "20": "Use Stamina for Defense",
                                "21": "Can't Dodge",
                                "22": "Level Divisible",
                                "23": "HP Fraction"
                            }
                        }
                    },
                    "element": {
                        "type": "property",
                        "name": "Element",
                        "begin": 1,
                        "flag": true,
                        "stringTable": "element"
                    },
                    "status12": {
                        "type": "property",
                        "name": "Status",
                        "begin": 10,
                        "mask": "0xFFFF",
                        "flag": true,
                        "stringTable": {
                            "length": 16,
                            "default": "<stringTable.statusNamesReversed[%i]>"
                        }
                    },
                    "status34": {
                        "type": "property",
                        "begin": 12,
                        "mask": "0xFFFF",
                        "flag": true,
                        "stringTable": {
                            "length": 16,
                            "default": "<stringTable.statusNamesReversed[%i+16]>"
                        }
                    }
                }
            }
        },
        "attackTilemap2bpp": {
            "type": "tilemap",
            "name": "Attack Tilemap (2bpp)",
            "range": "0xD2C000-0xD2E000",
            "format": ["defaultTile", "ff6-animation-tilemap"],
            "graphics": "attackGraphics2bpp",
            "palette": "attackPalette",
            "height": 256,
            "width": 16,
            "disableZLevel": true
        },
        "attackTilemap3bpp": {
            "type": "tilemap",
            "name": "Attack Tilemap (3bpp)",
            "range": "0xD20000-0xD26000",
            "format": ["defaultTile", "ff6-animation-tilemap"],
            "graphics": "attackGraphics3bpp",
            "palette": "attackPalette",
            "height": 768,
            "width": 16,
            "disableZLevel": true
        },
        "battleBackgroundDance": {
            "type": "array",
            "name": "Battle Background Dance",
            "range": "0xED8E5B-0xED8E9B",
            "array": {
                "length": 64
            },
            "assembly": {
                "type": "property",
                "name": "Dance",
                "stringTable": "danceName",
                "link": "danceAttacks[%i]"
            }
        },
        "battleBackgroundGraphics": {
            "type": "array",
            "name": "Battle Background Graphics",
            "range": "0xE7A9E7-0xE96300",
            "array": {
                "length": 75
            },
            "assembly": {
                "type": "graphics",
                "format": ["snes4bpp", "ff6-lzss"],
                "palette": "battleBackgroundPalette"
            },
            "pointerTable": {
                "range": "0xE71650-0xE71731",
                "pointerLength": 3
            }
        },
        "battleBackgroundLayout": {
            "type": "array",
            "name": "Battle Background Layout",
            "range": "0xE71928-0xE7A9E7",
            "array": {
                "length": 49
            },
            "assembly": {
                "type": "tilemap",
                "format": ["snes4bppTile", "ff6-lzss"],
                "width": 32,
                "height": 32
            },
            "pointerTable": {
                "range": "0xE71848-0xE718AA",
                "offset": "0xE70000"
            }
        },
        "battleBackgroundPalette": {
            "type": "array",
            "name": "Battle Background Palette",
            "range": "0xE70150-0xE71650",
            "array": {
                "length": 56
            },
            "assembly": {
                "length": 96,
                "format": "bgr555"
            }
        },
        "battleBackgroundProperties": {
            "type": "array",
            "name": "Battle Background Properties",
            "range": "0xE70000-0xE70150",
            "stringTable": "battleBackground",
            "array": {
                "length": 56
            },
            "assembly": {
                "type": "data",
                "editor": "FF6BattleBackgroundEditor",
                "length": 6,
                "assembly": {
                    "graphics1": {
                        "type": "data",
                        "name": "Graphics 1",
                        "begin": 0,
                        "length": 1,
                        "assembly": {
                            "g": {
                                "type": "property",
                                "name": "Graphics 1",
                                "mask": "0x7F",
                                "link": "battleBackgroundGraphics[%i]"
                            },
                            "double": {
                                "type": "property",
                                "name": "Double Size Graphics 1",
                                "mask": "0x80",
                                "bool": true
                            }
                        },
                        "special": {
                            "0xFF": "No Graphics 1"
                        }
                    },
                    "graphics2": {
                        "type": "property",
                        "name": "Graphics 2",
                        "begin": 1,
                        "link": "battleBackgroundGraphics[%i]",
                        "special": {
                            "0xFF": "No Graphics 2"
                        }
                    },
                    "graphics3": {
                        "type": "property",
                        "name": "Graphics 3",
                        "begin": 2,
                        "link": "battleBackgroundGraphics[%i]",
                        "special": {
                            "0xFF": "No Graphics 3"
                        }
                    },
                    "layout1": {
                        "type": "property",
                        "name": "Layout",
                        "begin": 3,
                        "link": "battleBackgroundLayout[%i]"
                    },
                    "layout2": {
                        "type": "property",
                        "name": "Layout 2",
                        "begin": 4,
                        "hidden": true
                    },
                    "palette": {
                        "type": "property",
                        "name": "Palette",
                        "begin": 5,
                        "mask": "0x7F"
                    },
                    "wavy": {
                        "type": "property",
                        "name": "Wavy Effect",
                        "begin": 5,
                        "mask": "0x80",
                        "bool": true
                    },
                    "dance": {
                        "type": "assembly",
                        "name": "Dance",
                        "external": "battleBackgroundDance[%i]"
                    }
                }
            }
        },
        "battleCharacterPalette": {
            "type": "array",
            "name": "Battle/Menu Character Palette",
            "range": "0xED6300-0xED6400",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "assembly",
                "length": 32,
                "format": "bgr555"
            },
            "stringTable": {
                "name": "Battle/Menu Character Palette",
                "default": "Battle/Menu Character Palette %i",
                "length": 8,
                "string": {
                    "0": "Edgar, Sabin, Celes (Battle/Menu)",
                    "1": "Locke (Battle/Menu)",
                    "2": "Terra (Battle/Menu)",
                    "3": "Strago, Relm, Gau, Gogo (Battle/Menu)",
                    "4": "Cyan, Shadow, Setzer (Battle/Menu)",
                    "5": "Mog, Umaro (Battle/Menu)",
                    "6": "Esper Terra (Battle/Menu)"
                }
            }
        },
        "battleCommandName": {
            "type": "array",
            "name": "Battle Command Name",
            "range": "0xD8CEA0-0xD8CF80",
            "array": {
                "length": 32
            },
            "assembly": {
                "type": "text",
                "encoding": "smallText",
                "length": 7
            },
            "stringTable": {
                "default": "Battle Command %i",
                "string": {
                    "0x00-0x1F": "<battleCommandName[%i]>",
                    "0x1E": "A.I. Command",
                    "0x1F": "Random Attack",
                    "0x20": "Change Battle",
                    "0x21": "Battle Dialogue",
                    "0x22": "Enemy Roulette",
                    "0x23": "Battle Event",
                    "0x24": "Monster Entry/Exit",
                    "0x25": "Battle Message",
                    "0x26": "Attack w/o Attacker",
                    "0x27": "Scan Info",
                    "0x29": "Status Counter Expired",
                    "0x2A": "Run",
                    "0x2B": "Monster Animation",
                    "0x2C": "Reset Character Action",
                    "0x2D": "Seize Damage",
                    "0x2E": "Change Variable",
                    "0x2F": "Change Switch",
                    "0x30": "Misc. Effects",
                    "0xFF": "Nothing"
                }
            }
        },
        "battleDialog": {
            "type": "array",
            "name": "Battle Dialogue",
            "range": "0xD0D200-0xD0FC7A",
            "pointerAlign": "0x10000",
            "array": {
                "length": 256
            },
            "assembly": {
                "type": "text",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xD0D000-0xD0D200",
                "offset": "0xD00000"
            },
            "stringTable": {
                "length": 256,
                "default": "<battleDialog[%i]>"
            }
        },
        "battleGroupsEvent": {
            "type": "array",
            "name": "Event Battle Group",
            "range": "0xCF5000-0xCF5400",
            "array": {
                "length": 256
            },
            "assembly": {
                "type": "data",
                "assembly": {
                    "battle1": {
                        "type": "property",
                        "name": "Battle 1",
                        "begin": 0,
                        "mask": "0x03FF",
                        "link": "battleProperties[%i]",
                        "stringTable": "battleProperties"
                    },
                    "random1": {
                        "type": "property",
                        "name": "Choose From Next Four",
                        "begin": 1,
                        "mask": "0x80",
                        "bool": true
                    },
                    "battle2": {
                        "type": "property",
                        "name": "Battle 2",
                        "begin": 2,
                        "mask": "0x03FF",
                        "link": "battleProperties[%i]",
                        "stringTable": "battleProperties"
                    },
                    "random2": {
                        "type": "property",
                        "name": "Choose From Next Four",
                        "begin": 3,
                        "mask": "0x80",
                        "bool": true
                    }
                },
                "length": 4
            },
            "stringTable": {
                "default": "Event Battle Group %i",
                "length": 256,
                "string": {
                    "0": "<stringTable.battleProperties[0]>",
                    "1": "<stringTable.battleProperties[2]>",
                    "2": "<stringTable.battleProperties[1]>",
                    "3": "<stringTable.battleProperties[41]>",
                    "4": "<stringTable.battleProperties[84]>",
                    "5": "<stringTable.battleProperties[17]>",
                    "6": "<stringTable.battleProperties[4]>",
                    "7": "Lete River A",
                    "8": "Lete River B",
                    "9": "<stringTable.battleProperties[40]>",
                    "10": "<stringTable.battleProperties[43]>",
                    "11": "<stringTable.battleProperties[64]>",
                    "12": "Imperial Camp (???)",
                    "13": "Imperial Camp (Cyan A.I.)",
                    "14": "<stringTable.battleProperties[59]>",
                    "15": "<stringTable.battleProperties[27]>",
                    "16": "<stringTable.battleProperties[25]>",
                    "17": "<stringTable.battleProperties[34]>",
                    "18": "<stringTable.battleProperties[79]>",
                    "19": "Serpent Trench A",
                    "20": "Serpent Trench B",
                    "21": "Serpent Trench C",
                    "22": "Hills Maze Battle A",
                    "23": "Hills Maze Battle B",
                    "24": "<stringTable.battleProperties[87]>",
                    "25": "Opera House Rafters",
                    "26": "<stringTable.battleProperties[408]>",
                    "27": "<stringTable.battleProperties[418]>",
                    "28": "Vector Guard (Magitek Armor)",
                    "29": "Vector Guard (Soldier)",
                    "30": "<stringTable.battleProperties[157]>",
                    "31": "<stringTable.battleProperties[158]>",
                    "32": "<stringTable.battleProperties[61]>",
                    "34": "<stringTable.battleProperties[476]>",
                    "35": "<stringTable.battleProperties[270]>",
                    "37": "<stringTable.battleProperties[170]>",
                    "38": "Narshe Security Checkpoint",
                    "39": "<stringTable.battleProperties[416]>",
                    "40": "<stringTable.battleProperties[417]>",
                    "41": "Magitek Train Ride A",
                    "42": "Imperial Camp (Chest Trap)",
                    "43": "<stringTable.battleProperties[412]>",
                    "44": "<stringTable.battleProperties[410]>",
                    "45": "<stringTable.battleProperties[411]>",
                    "46": "<stringTable.battleProperties[409]>",
                    "47": "Phantom Train Ghost",
                    "48": "<stringTable.battleProperties[496]>",
                    "49": "<stringTable.battleProperties[497]>",
                    "50": "<stringTable.battleProperties[498]>",
                    "51": "<stringTable.battleProperties[499]>",
                    "52": "<stringTable.battleProperties[500]>",
                    "53": "<stringTable.battleProperties[501]>",
                    "54": "<stringTable.battleProperties[502]>",
                    "55": "<stringTable.battleProperties[503]>",
                    "56": "<stringTable.battleProperties[504]>",
                    "57": "<stringTable.battleProperties[505]>",
                    "58": "<stringTable.battleProperties[506]>",
                    "59": "<stringTable.battleProperties[507]>",
                    "60": "<stringTable.battleProperties[508]>",
                    "61": "<stringTable.battleProperties[509]>",
                    "62": "<stringTable.battleProperties[510]>",
                    "63": "<stringTable.battleProperties[511]>",
                    "64": "<stringTable.battleProperties[432]>",
                    "65": "<stringTable.battleProperties[434]>",
                    "66": "<stringTable.battleProperties[435]>",
                    "67": "<stringTable.battleProperties[436]>",
                    "68": "<stringTable.battleProperties[437]>",
                    "69": "<stringTable.battleProperties[438]>",
                    "70": "<stringTable.battleProperties[439]>",
                    "71": "<stringTable.battleProperties[440]>",
                    "72": "<stringTable.battleProperties[441]>",
                    "73": "<stringTable.battleProperties[442]>",
                    "74": "<stringTable.battleProperties[443]>",
                    "75": "<stringTable.battleProperties[445]>",
                    "76": "<stringTable.battleProperties[446]>",
                    "77": "<stringTable.battleProperties[447]>",
                    "78": "<stringTable.battleProperties[448]>",
                    "79": "<stringTable.battleProperties[449]>",
                    "80": "<stringTable.battleProperties[450]>",
                    "81": "<stringTable.battleProperties[451]>",
                    "82": "<stringTable.battleProperties[452]>",
                    "83": "<stringTable.battleProperties[453]>",
                    "84": "<stringTable.battleProperties[454]>",
                    "85": "<stringTable.battleProperties[455]>",
                    "86": "<stringTable.battleProperties[456]>",
                    "87": "<stringTable.battleProperties[457]>",
                    "88": "<stringTable.battleProperties[458]>",
                    "89": "<stringTable.battleProperties[459]>",
                    "90": "<stringTable.battleProperties[460]>",
                    "91": "<stringTable.battleProperties[461]>",
                    "92": "<stringTable.battleProperties[462]>",
                    "93": "<stringTable.battleProperties[463]>",
                    "94": "<stringTable.battleProperties[464]>",
                    "95": "<stringTable.battleProperties[465]>",
                    "96": "<stringTable.battleProperties[466]>",
                    "97": "<stringTable.battleProperties[392]>",
                    "98": "<stringTable.battleProperties[468]>",
                    "99": "<stringTable.battleProperties[469]>",
                    "100": "<stringTable.battleProperties[470]>",
                    "101": "<stringTable.battleProperties[471]>",
                    "102": "<stringTable.battleProperties[472]>",
                    "103": "<stringTable.battleProperties[473]>",
                    "104": "<stringTable.battleProperties[474]>",
                    "105": "<stringTable.battleProperties[389]>",
                    "106": "<stringTable.battleProperties[390]>",
                    "107": "<stringTable.battleProperties[477]>",
                    "108": "<stringTable.battleProperties[478]>",
                    "109": "<stringTable.battleProperties[479]>",
                    "110": "<stringTable.battleProperties[480]>",
                    "111": "<stringTable.battleProperties[481]>",
                    "112": "<stringTable.battleProperties[482]>",
                    "113": "<stringTable.battleProperties[483]>",
                    "114": "<stringTable.battleProperties[484]>",
                    "115": "<stringTable.battleProperties[485]>",
                    "116": "<stringTable.battleProperties[433]>",
                    "117": "<stringTable.battleProperties[444]>",
                    "118": "<stringTable.battleProperties[391]>",
                    "119": "<stringTable.battleProperties[487]>",
                    "120": "<stringTable.battleProperties[488]>",
                    "121": "<stringTable.battleProperties[384]>",
                    "122": "<stringTable.battleProperties[385]>",
                    "123": "<stringTable.battleProperties[386]>",
                    "124": "<stringTable.battleProperties[388]>",
                    "125": "<stringTable.battleProperties[387]>",
                    "126": "Floating Island Approach",
                    "127": "<stringTable.battleProperties[393]>",
                    "128": "<stringTable.battleProperties[420]>",
                    "129": "<stringTable.battleProperties[421]>",
                    "130": "<stringTable.battleProperties[422]>",
                    "131": "<stringTable.battleProperties[423]>",
                    "132": "<stringTable.battleProperties[366]>",
                    "133": "<stringTable.battleProperties[367]>",
                    "134": "<stringTable.battleProperties[368]>",
                    "135": "<stringTable.battleProperties[369]>",
                    "136": "<stringTable.battleProperties[370]>",
                    "137": "<stringTable.battleProperties[371]>",
                    "138": "<stringTable.battleProperties[372]>",
                    "139": "<stringTable.battleProperties[481]>",
                    "140": "<stringTable.battleProperties[446]>",
                    "141": "<stringTable.battleProperties[419]>",
                    "142": "<stringTable.battleProperties[419]>",
                    "143": "<stringTable.battleProperties[419]>",
                    "144": "Magitek Train Ride B",
                    "145": "<stringTable.battleProperties[354]>",
                    "146": "<stringTable.battleProperties[288]>",
                    "147": "<stringTable.battleProperties[289]>",
                    "148": "<stringTable.battleProperties[290]>",
                    "149": "<stringTable.battleProperties[524]>",
                    "150": "<stringTable.battleProperties[211]>",
                    "151": "<stringTable.battleProperties[287]>",
                    "152": "<stringTable.battleProperties[235]>",
                    "153": "<stringTable.battleProperties[515]>"
                }
            }
        },
        "battleGroupsRandom": {
            "type": "array",
            "name": "Random Battle Group",
            "range": "0xCF4800-0xCF5000",
            "array": {
                "length": 256
            },
            "assembly": {
                "type": "data",
                "assembly": {
                    "battle1": {
                        "type": "property",
                        "name": "Battle 1",
                        "begin": 0,
                        "mask": "0x03FF",
                        "link": "battleProperties[%i]",
                        "stringTable": "battleProperties"
                    },
                    "random1": {
                        "type": "property",
                        "name": "Choose From Next Four",
                        "begin": 1,
                        "mask": "0x80",
                        "bool": true
                    },
                    "battle2": {
                        "type": "property",
                        "name": "Battle 2",
                        "begin": 2,
                        "mask": "0x03FF",
                        "link": "battleProperties[%i]",
                        "stringTable": "battleProperties"
                    },
                    "random2": {
                        "type": "property",
                        "name": "Choose From Next Four",
                        "begin": 3,
                        "mask": "0x80",
                        "bool": true
                    },
                    "battle3": {
                        "type": "property",
                        "name": "Battle 3",
                        "begin": 4,
                        "mask": "0x03FF",
                        "link": "battleProperties[%i]",
                        "stringTable": "battleProperties"
                    },
                    "random3": {
                        "type": "property",
                        "name": "Choose From Next Four",
                        "begin": 5,
                        "mask": "0x80",
                        "bool": true
                    },
                    "battle4": {
                        "type": "property",
                        "name": "Battle 4",
                        "begin": 6,
                        "mask": "0x03FF",
                        "link": "battleProperties[%i]",
                        "stringTable": "battleProperties"
                    },
                    "random4": {
                        "type": "property",
                        "name": "Choose From Next Four",
                        "begin": 7,
                        "mask": "0x80",
                        "bool": true
                    }
                },
                "length": 8
            },
            "stringTable": {
                "default": "Random Battle Group %i",
                "length": 256,
                "string": {
                    "0": "WoB Narshe/Figaro Area (Field)",
                    "1": "WoB Narshe/Figaro Area (Desert)",
                    "2": "WoB Narshe/Figaro Area (Forest)",
                    "3": "WoB South Figaro Area (Field)",
                    "4": "WoB South Figaro Area (Forest)",
                    "5": "WoB Returner's Hideout Area",
                    "6": "WoB Crazy Old Man Area (Field)",
                    "7": "WoB Crazy Old Man Area (Forest)",
                    "8": "WoB Doma Area (Field)",
                    "9": "WoB Kohlingen Area (Grass)",
                    "10": "WoB Zozo/Jidoor Area (Grass)",
                    "11": "WoB Kohlingen/Jidoor Area (Forest)",
                    "12": "WoB Kohlingen Area (Desert)",
                    "13": "WoB Kohlingen Area (Dirt)",
                    "14": "WoB Vector/Tzen Area (Grass)",
                    "15": "WoB Albrook/Maranda Area (Grass)",
                    "17": "WoB Vector/Tzen Area (Forest)",
                    "18": "WoB Albrook/Maranda Area (Forest)",
                    "20": "WoB Southern Continent (Desert)",
                    "21": "WoB Southern Continent (Dirt)",
                    "22": "WoB North Area (Grass)",
                    "23": "WoB North Area (Forest)",
                    "24": "WoB Thamasa Area (Field)",
                    "25": "WoB Thamasa Area (Desert)",
                    "26": "WoB Thamasa Area (Forest)",
                    "27": "WoB Bomb Forest (Forest)",
                    "28": "WoB Triangle Island (Forest)",
                    "29": "WoR Solitary Island (Field)",
                    "30": "WoR Solitary Island (Desert)",
                    "31": "WoR Albrook Area (Grass)",
                    "32": "WoR Triangle Island",
                    "33": "WoR Tzen Area (Dirt)",
                    "34": "WoR Albrook Area (Dirt)",
                    "35": "WoR Albrook Area (Forest)",
                    "36": "WoR Albrook Area (Desert)",
                    "37": "WoR Nikeah/Mobliz Area (Grass)",
                    "38": "WoR Mobliz Area (Dirt)",
                    "39": "WoR Nikeah Area (Dirt)",
                    "40": "WoR Nikeah/Mobliz Area (Desert)",
                    "41": "WoR Figaro Area (Grass)",
                    "42": "WoR Figaro Area (Forest)",
                    "43": "WoR Figaro Area (Dirt)",
                    "44": "WoR Figaro Area (Desert)",
                    "44": "WoR Kohlingen Area (Desert)",
                    "45": "WoR Kohlingen Area (Grass)",
                    "46": "WoR Kohlingen Area (Forest)",
                    "47": "WoR Kohlingen Area (Dirt)",
                    "48": "WoR Maranda/Jidoor Area (Grass)",
                    "49": "WoR Maranda/Jidoor Area (Dirt)",
                    "50": "WoR Maranda/Jidoor Area (Forest)",
                    "51": "WoR Narshe Area (Grass)",
                    "52": "WoR Doma Area (Grass)",
                    "53": "WoR Narshe/Doma Area (Dirt)",
                    "54": "WoR Narshe/Doma Area (Forest)",
                    "55": "WoR Thamasa Area (Grass)",
                    "56": "WoR Thamasa Area (Dirt)",
                    "57": "Narshe: Whelk Cave (Intro)",
                    "58": "Narshe: Tritoch's Cave (Intro)",
                    "59": "Cave to South Figaro A (Terra/Locke/Edgar)",
                    "60": "Cave to South Figaro B (Terra/Locke/Edgar)",
                    "61": "Mt. Kolts Exterior A",
                    "62": "Mt. Kolts Exterior B",
                    "63": "Mt. Kolts Interor A",
                    "64": "Mt. Kolts Interor B",
                    "65": "South Figaro (Escape Tunnel)",
                    "67": "Phantom Forest A (Unused)",
                    "68": "Phantom Forest B",
                    "69": "Phantom Forest C",
                    "70": "Phantom Train (Seating Cars)",
                    "71": "Phantom Train (Seating Car with Switch)",
                    "72": "Phantom Train (Hallway)",
                    "73": "Phantom Train (Dining Car)",
                    "74": "Phantom Train (Caboose Room and Engine)",
                    "75": "Phantom Train (Small Rooms)",
                    "76": "Phantom Train (Exterior)",
                    "77": "Zozo (Interior)",
                    "78": "Zozo (Exterior)",
                    "80": "Magitek Factory (Upper Room)",
                    "81": "Magitek Factory (Lower Room)",
                    "89": "Esper Mountain (Exterior)",
                    "90": "Esper Mountain (Caves)",
                    "92": "Cave to the Sealed Gate A",
                    "93": "Cave to the Sealed Gate B",
                    "94": "Cave to the Sealed Gate C",
                    "95": "Cave to the Sealed Gate D",
                    "104": "Magitek Factory (Garbage Room)",
                    "105": "Magitek Research Facility A",
                    "106": "Magitek Research Facility B",
                    "108": "Vector: Exterior (After Train Ride)",
                    "111": "WoR Maranda Area (Desert)",
                    "112": "Floating Island",
                    "121": "Narshe Caves (WoB)",
                    "122": "Cave to South Figaro (Locke/Celes)",
                    "123": "Floating Island (Collapsing)",
                    "124": "WoB Triangle Island (Grass)",
                    "128": "Tzen (Collapsing House)",
                    "132": "Cave in the Veldt",
                    "137": "Figaro Castle A",
                    "138": "Figaro Castle B",
                    "139": "Figaro Castle C",
                    "140": "Figaro Castle D",
                    "143": "Ancient Castle (Caves)",
                    "144": "Ancient Castle (Interior)",
                    "145": "Ancient Castle (Exterior)",
                    "149": "Darill's Tomb A",
                    "150": "Darill's Tomb B",
                    "151": "Darill's Tomb C",
                    "152": "Fanatics' Tower (1st Level)",
                    "153": "Fanatics' Tower (2nd Level)",
                    "154": "Fanatics' Tower (3rd Level)",
                    "155": "Fanatics' Tower (4th Level)",
                    "156": "Fanatics' Tower (Interior)",
                    "157": "Jidoor (Owzer's Basement)",
                    "159": "Fanatics' Tower (Secret Treasure Room)",
                    "160": "Mt. Zozo (Caves)",
                    "161": "Mt. Zozo (Bridge)",
                    "162": "Mt. Zozo (Cliffs)",
                    "166": "Umaro's Cave A",
                    "167": "Umaro's Cave B",
                    "168": "Umaro's Cave C",
                    "170": "Gogo's Cave A",
                    "171": "Gogo's Cave B",
                    "174": "Phoenix Cave A",
                    "175": "Phoenix Cave B",
                    "181": "Cyan's Dream (Doma/Phantom Train)",
                    "182": "Cyan's Dream (Caves)",
                    "183": "Cyan's Dream (3 Stooges)",
                    "189": "Narshe Town (WoR)",
                    "190": "Narshe Caves A (WoR)",
                    "191": "Narshe Caves B (WoR)",
                    "193": "Ebot's Rock A",
                    "194": "Ebot's Rock B",
                    "208": "Kefka's Tower",
                    "209": "Kefka's Tower",
                    "210": "Kefka's Tower",
                    "211": "Kefka's Tower",
                    "212": "Kefka's Tower",
                    "213": "Kefka's Tower",
                    "214": "Kefka's Tower",
                    "215": "Kefka's Tower",
                    "216": "Kefka's Tower",
                    "217": "Kefka's Tower",
                    "218": "Kefka's Tower",
                    "219": "Kefka's Tower",
                    "220": "Kefka's Tower",
                    "221": "Kefka's Tower",
                    "222": "Kefka's Tower",
                    "223": "Kefka's Tower",
                    "224": "Kefka's Tower",
                    "225": "Kefka's Tower",
                    "226": "Kefka's Tower",
                    "227": "Kefka's Tower"
                }
            }
        },
        "battleMagicPoints": {
            "type": "array",
            "name": "Battle Magic Points",
            "range": "0xDFB400-0xDFB600",
            "array": {
                "length": 512
            },
            "assembly": {
                "type": "data",
                "length": 1,
                "assembly": {
                    "magicPoints": {
                        "type": "property",
                        "name": "Magic Points"
                    }
                }
            }
        },
        "battleProperties": {
            "type": "array",
            "name": "Battle Properties",
            "range": "0xCF6200-0xCF8400",
            "array": {
                "length": 576
            },
            "assembly": {
                "type": "data",
                "editor": "FF6Battle",
                "length": 15,
                "assembly": {
                    "monster1": {
                        "type": "property",
                        "name": "Monster 1",
                        "begin": 2,
                        "max": "0x01FF",
                        "msb": "this.parent.monster1MSB",
                        "stringTable": "monsterName",
                        "link": "monsterProperties[%i]",
                        "special": {
                            "0x01FF": "Empty"
                        }
                    },
                    "monster1MSB": {
                        "type": "property",
                        "name": "Monster 1 MSB",
                        "begin": 14,
                        "mask": "0x01",
                        "bool": true,
                        "hidden": true
                    },
                    "monster1Present": {
                        "type": "property",
                        "name": "Present",
                        "begin": 1,
                        "mask": "0x01",
                        "bool": true
                    },
                    "monster2": {
                        "type": "property",
                        "name": "Monster 2",
                        "begin": 3,
                        "max": "0x01FF",
                        "msb": "this.parent.monster2MSB",
                        "stringTable": "monsterName",
                        "link": "monsterProperties[%i]",
                        "special": {
                            "0x01FF": "Empty"
                        }
                    },
                    "monster2MSB": {
                        "type": "property",
                        "name": "Monster 2 MSB",
                        "begin": 14,
                        "mask": "0x02",
                        "bool": true,
                        "hidden": true
                    },
                    "monster2Present": {
                        "type": "property",
                        "name": "Present",
                        "begin": 1,
                        "mask": "0x02",
                        "bool": true
                    },
                    "monster3": {
                        "type": "property",
                        "name": "Monster 3",
                        "begin": 4,
                        "max": "0x01FF",
                        "msb": "this.parent.monster3MSB",
                        "stringTable": "monsterName",
                        "link": "monsterProperties[%i]",
                        "special": {
                            "0x01FF": "Empty"
                        }
                    },
                    "monster3MSB": {
                        "type": "property",
                        "name": "Monster 3 MSB",
                        "begin": 14,
                        "mask": "0x04",
                        "bool": true,
                        "hidden": true
                    },
                    "monster3Present": {
                        "type": "property",
                        "name": "Present",
                        "begin": 1,
                        "mask": "0x04",
                        "bool": true
                    },
                    "monster4": {
                        "type": "property",
                        "name": "Monster 4",
                        "begin": 5,
                        "max": "0x01FF",
                        "msb": "this.parent.monster4MSB",
                        "stringTable": "monsterName",
                        "link": "monsterProperties[%i]",
                        "special": {
                            "0x01FF": "Empty"
                        }
                    },
                    "monster4MSB": {
                        "type": "property",
                        "name": "Monster 4 MSB",
                        "begin": 14,
                        "mask": "0x08",
                        "bool": true,
                        "hidden": true
                    },
                    "monster4Present": {
                        "type": "property",
                        "name": "Present",
                        "begin": 1,
                        "mask": "0x08",
                        "bool": true
                    },
                    "monster5": {
                        "type": "property",
                        "name": "Monster 5",
                        "begin": 6,
                        "max": "0x01FF",
                        "msb": "this.parent.monster5MSB",
                        "stringTable": "monsterName",
                        "link": "monsterProperties[%i]",
                        "special": {
                            "0x01FF": "Empty"
                        }
                    },
                    "monster5MSB": {
                        "type": "property",
                        "name": "Monster 5 MSB",
                        "begin": 14,
                        "mask": "0x10",
                        "bool": true,
                        "hidden": true
                    },
                    "monster5Present": {
                        "type": "property",
                        "name": "Present",
                        "begin": 1,
                        "mask": "0x10",
                        "bool": true
                    },
                    "monster6": {
                        "type": "property",
                        "name": "Monster 6",
                        "begin": 7,
                        "max": "0x01FF",
                        "msb": "this.parent.monster6MSB",
                        "stringTable": "monsterName",
                        "link": "monsterProperties[%i]",
                        "special": {
                            "0x01FF": "Empty"
                        }
                    },
                    "monster6MSB": {
                        "type": "property",
                        "name": "Monster 6 MSB",
                        "begin": 14,
                        "mask": "0x20",
                        "bool": true,
                        "hidden": true
                    },
                    "monster6Present": {
                        "type": "property",
                        "name": "Present",
                        "begin": 1,
                        "mask": "0x20",
                        "bool": true
                    },
                    "monster1X": {
                        "type": "property",
                        "name": "Monster 1 X",
                        "begin": 8,
                        "mask": "0xF0",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster1Y": {
                        "type": "property",
                        "name": "Monster 1 Y",
                        "begin": 8,
                        "mask": "0x0F",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster2X": {
                        "type": "property",
                        "name": "Monster 2 X",
                        "begin": 9,
                        "mask": "0xF0",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster2Y": {
                        "type": "property",
                        "name": "Monster 2 Y",
                        "begin": 9,
                        "mask": "0x0F",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster3X": {
                        "type": "property",
                        "name": "Monster 3 X",
                        "begin": 10,
                        "mask": "0xF0",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster3Y": {
                        "type": "property",
                        "name": "Monster 3 Y",
                        "begin": 10,
                        "mask": "0x0F",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster4X": {
                        "type": "property",
                        "name": "Monster 4 X",
                        "begin": 11,
                        "mask": "0xF0",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster4Y": {
                        "type": "property",
                        "name": "Monster 4 Y",
                        "begin": 11,
                        "mask": "0x0F",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster5X": {
                        "type": "property",
                        "name": "Monster 5 X",
                        "begin": 12,
                        "mask": "0xF0",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster5Y": {
                        "type": "property",
                        "name": "Monster 5 Y",
                        "begin": 12,
                        "mask": "0x0F",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster6X": {
                        "type": "property",
                        "name": "Monster 6 X",
                        "begin": 13,
                        "mask": "0xF0",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "monster6Y": {
                        "type": "property",
                        "name": "Monster 6 Y",
                        "begin": 13,
                        "mask": "0x0F",
                        "multiplier": 8,
                        "hidden": true
                    },
                    "vramMap": {
                        "type": "property",
                        "name": "VRAM Map",
                        "begin": 0,
                        "mask": "0xF0",
                        "stringTable": "battleVRAMMap"
                    },
                    "monsterEntry": {
                        "type": "assembly",
                        "external": "battlePropertiesAux[%i].monsterEntry"
                    },
                    "magicPoints": {
                        "type": "assembly",
                        "external": "battleMagicPoints[%i].magicPoints",
                        "invalid": "this.parent.i >= 512"
                    },
                    "song": {
                        "type": "assembly",
                        "external": "battlePropertiesAux[%i].song"
                    },
                    "continueSong": {
                        "type": "assembly",
                        "external": "battlePropertiesAux[%i].continueSong"
                    },
                    "characterAI": {
                        "type": "assembly",
                        "external": "battlePropertiesAux[%i].characterAI"
                    },
                    "enableCharacterAI": {
                        "type": "assembly",
                        "external": "battlePropertiesAux[%i].enableCharacterAI"
                    },
                    "flags": {
                        "type": "assembly",
                        "external": "battlePropertiesAux[%i].flags"
                    },
                    "flags2": {
                        "type": "assembly",
                        "external": "battlePropertiesAux[%i].flags2"
                    }
                }
            },
            "stringTable": {
                "length": 576,
                "default": "Battle %i"
            }
        },
        "battlePropertiesAux": {
            "type": "array",
            "name": "Auxiliary Battle Properties",
            "range": "0xCF5900-0xCF6200",
            "array": {
                "length": 576
            },
            "assembly": {
                "type": "data",
                "length": 4,
                "assembly": {
                    "monsterEntry": {
                        "type": "property",
                        "name": "Monster Entry",
                        "begin": 0,
                        "mask": "0x0F"
                    },
                    "flags": {
                        "type": "property",
                        "name": "Flags",
                        "begin": 0,
                        "mask": "0x0EF0",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Disable Normal Attack",
                                "1": "Disable Back Attack",
                                "2": "Disable Pincer Attack",
                                "3": "Disable Side Attack",
                                "5": "Disable Fanfare",
                                "6": "Disable Joker Doom",
                                "7": "Disable Leap"
                            }
                        }
                    },
                    "song": {
                        "type": "property",
                        "name": "Song",
                        "begin": 3,
                        "mask": "0x38",
                        "stringTable": {
                            "string": {
                                "0": "Battle Theme",
                                "1": "The Decisive Battle",
                                "2": "The Fierce Battle",
                                "3": "Returners",
                                "4": "Save Them!",
                                "5": "Dancing Mad",
                                "6": "None",
                                "7": "None"
                            }
                        }
                    },
                    "continueSong": {
                        "type": "property",
                        "name": "Continue Previous Song",
                        "begin": 3,
                        "mask": "0x80",
                        "bool": true
                    },
                    "characterAI": {
                        "type": "property",
                        "name": "Character A.I.",
                        "begin": 2,
                        "stringTable": "characterAI",
                        "link": "characterAI[%i]"
                    },
                    "enableCharacterAI": {
                        "type": "property",
                        "name": "Enable Character A.I.",
                        "begin": 1,
                        "mask": "0x80",
                        "bool": true
                    },
                    "flags2": {
                        "type": "property",
                        "begin": 3,
                        "mask": "0x07",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Disable L+R",
                                "1": "Disable on the Veldt",
                                "2": "Disable Preemptive Attack"
                            }
                        }
                    }
                }
            }
        },
        "battleVRAMMap": {
            "type": "array",
            "name": "Battle VRAM Map",
            "range": "0xC2D034-0xC2D0F4",
            "array": {
                "length": 13
            },
            "pointerTable": {
                "range": "0xC2D01A-0xC2D034",
                "offset": "0xC20000"
            },
            "stringTable": {
                "length": 13,
                "default": "VRAM Map %i",
                "string": {
                    "0": "(4) 64×64",
                    "1": "(2) 64×64, (4) 32×32",
                    "2": "(2) 96×64",
                    "3": "(2) 64×128",
                    "4": "(1) 96×64, (2) 64×64",
                    "5": "(1) 64×128, (2) 64×64",
                    "6": "(1) 128×128",
                    "7": "(1) 96×96, (1) 32×96, (4) 32×32",
                    "8": "(2) 64×64, (2) 32×64",
                    "9": "(1) 96×96",
                    "10": "(1) 64×96, (2) 32×64, (2) 32×32",
                    "11": "(4) 32×64, (1) 64×64",
                    "12": "(2) 64×64, (4) 32×64"
                }
            },
            "assembly": {
                "type": "array",
                "assembly": {
                    "type": "data",
                    "length": 4,
                    "assembly": {
                        "vramAddress": {
                            "type": "property",
                            "name": "VRAM Address",
                            "begin": 0,
                            "mask": "0xFFFF"
                        },
                        "width": {
                            "type": "property",
                            "name": "Width",
                            "begin": 2
                        },
                        "height": {
                            "type": "property",
                            "name": "Height",
                            "begin": 3
                        }
                    }
                }
            }
        },
        "blitzCode": {
            "type": "array",
            "name": "Blitz Code",
            "range": "0xC47A40-0xC47AA0",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "data",
                "length": 12,
                "assembly": {
                    "level": {
                        "type": "assembly",
                        "external": "blitzLevel[%i]"
                    },
                    "cat-code": "Input Code",
                    "length": {
                        "type": "property",
                        "name": "Code Length",
                        "begin": 11,
                        "stringTable": {
                            "hideIndex": true,
                            "string": {
                                "2": "1 Button",
                                "4": "2 Buttons",
                                "6": "3 Buttons",
                                "8": "4 Buttons",
                                "10": "5 Buttons",
                                "12": "6 Buttons",
                                "14": "7 Buttons",
                                "16": "8 Buttons",
                                "18": "9 Buttons",
                                "20": "10 Buttons",
                                "22": "11 Buttons"
                            }
                        }
                    },
                    "button1": {
                        "type": "property",
                        "name": "Button 1",
                        "begin": 0,
                        "stringTable": {
                            "hideIndex": true,
                            "string": {
                                "0": "No Button",
                                "1": "A Button",
                                "3": "X Button",
                                "4": "Y Button",
                                "5": "L Button",
                                "6": "R Button",
                                "7": "Down/Left",
                                "8": "Down",
                                "9": "Down/Right",
                                "10": "Right",
                                "11": "Up/Right",
                                "12": "Up",
                                "13": "Up/Left",
                                "14": "Left"
                            }
                        }
                    },
                    "button2": {
                        "type": "property",
                        "name": "Button 2",
                        "begin": 1,
                        "stringTable": "blitzCode.button1"
                    },
                    "button3": {
                        "type": "property",
                        "name": "Button 3",
                        "begin": 2,
                        "stringTable": "blitzCode.button1"
                    },
                    "button4": {
                        "type": "property",
                        "name": "Button 4",
                        "begin": 3,
                        "stringTable": "blitzCode.button1"
                    },
                    "button5": {
                        "type": "property",
                        "name": "Button 5",
                        "begin": 4,
                        "stringTable": "blitzCode.button1"
                    },
                    "button6": {
                        "type": "property",
                        "name": "Button 6",
                        "begin": 5,
                        "stringTable": "blitzCode.button1"
                    },
                    "button7": {
                        "type": "property",
                        "name": "Button 7",
                        "begin": 6,
                        "stringTable": "blitzCode.button1"
                    },
                    "button8": {
                        "type": "property",
                        "name": "Button 8",
                        "begin": 7,
                        "stringTable": "blitzCode.button1"
                    },
                    "button9": {
                        "type": "property",
                        "name": "Button 9",
                        "begin": 8,
                        "stringTable": "blitzCode.button1"
                    },
                    "button10": {
                        "type": "property",
                        "name": "Button 10",
                        "begin": 9,
                        "stringTable": "blitzCode.button1"
                    },
                    "button11": {
                        "type": "property",
                        "name": "Button 11",
                        "begin": 10,
                        "stringTable": "blitzCode.button1"
                    },
                    "cat-end": ""
                }
            }
        },
        "blitzDescription": {
            "type": "array",
            "name": "Blitz Description",
            "range": "0xCFFC00-0xCFFCE0",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "text",
                "name": "Description",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xCFFF9E-0xCFFFAE",
                "offset": "0xCFFC00"
            }
        },
        "blitzLevel": {
            "type": "array",
            "name": "Blitz Level",
            "range": "0xE6F498-0xE6F4A0",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "property",
                "name": "Learn at Level",
                "max": 99
            }
        },
        "characterAI": {
            "type": "array",
            "name": "Character A.I. Properties",
            "range": "0xD0FD00-0xD0FF40",
            "array": {
                "length": 32
            },
            "assembly": {
                "type": "data",
                "length": 24,
                "assembly": {
                    "background": {
                        "type": "property",
                        "name": "Background",
                        "begin": 1,
                        "stringTable": "battleBackground"
                    },
                    "song": {
                        "type": "property",
                        "name": "Song",
                        "begin": 3,
                        "stringTable": "song",
                        "special": {
                            "0xFF": "Default"
                        }
                    },
                    "flags": {
                        "type": "property",
                        "name": "Flags",
                        "begin": 0,
                        "mask": "0x81",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Hide Names",
                                "7": "Hide Party"
                            }
                        }
                    },
                    "validTargets": {
                        "type": "property",
                        "name": "Valid Targets",
                        "begin": 2,
                        "mask": "0x3F",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Monster Slot 1",
                                "1": "Monster Slot 2",
                                "2": "Monster Slot 3",
                                "3": "Monster Slot 4",
                                "4": "Monster Slot 5",
                                "5": "Monster Slot 6"
                            }
                        }
                    },
                    "slot": {
                        "type": "array",
                        "name": "Character Slots",
                        "begin": 4,
                        "length": 20,
                        "arrayLength": 4,
                        "arrayMin": 4,
                        "arrayMax": 4,
                        "assembly": {
                            "type": "data",
                            "length": 5,
                            "assembly": {
                                "graphics": {
                                    "type": "property",
                                    "name": "Graphics",
                                    "begin": 1,
                                    "mask": "0xFF",
                                    "stringTable": "battleCharacterGraphics",
                                    "disabled": "this.parent.character.getSpecialValue()",
                                    "special": {
                                        "0xFF": "Default Character Graphics"
                                    }
                                },
                                "aiScript": {
                                    "type": "property",
                                    "name": "A.I. Script",
                                    "begin": 2,
                                    "disabled": "this.parent.character.getSpecialValue()",
                                    "link": "monsterProperties[%i+256]",
                                    "stringTable": {
                                        "length": 128,
                                        "default": "<stringTable.monsterName[%i+256]>",
                                        "string": {
                                            "0xFF": "None"
                                        }
                                    }
                                },
                                "character": {
                                    "type": "data",
                                    "name": "Character",
                                    "begin": 0,
                                    "length": 1,
                                    "mask": "0xFF",
                                    "special": {
                                        "0xFF": "No Character"
                                    },
                                    "assembly": {
                                        "c": {
                                            "type": "property",
                                            "name": "Character",
                                            "mask": "0x3F",
                                            "stringTable": "characterNames",
                                            "link": "characterProperties[%i]"
                                        },
                                        "flags": {
                                            "type": "property",
                                            "name": "Flags",
                                            "mask": "0xC0",
                                            "flag": true,
                                            "stringTable": {
                                                "hideIndex": true,
                                                "string": {
                                                    "0": "Enemy Character",
                                                    "1": "Character Hidden"
                                                }
                                            }
                                        }
                                    }
                                },
                                "x": {
                                    "type": "property",
                                    "name": "X Position",
                                    "begin": 3,
                                    "multiplier": 2,
                                    "hidden": true
                                },
                                "y": {
                                    "type": "property",
                                    "name": "Y Position",
                                    "begin": 4,
                                    "multiplier": 2,
                                    "hidden": true
                                }
                            }
                        }
                    }
                }
            },
            "stringTable": {
                "length": 32,
                "default": "Character A.I. %i",
                "string": {
                    "0": "None",
                    "1": "Shadow at Colosseum",
                    "2": "Kefka/Terra Flashback",
                    "3": "Vargas Battle",
                    "4": "Kefka vs. Sabin",
                    "5": "Cyan at Imperial Camp 1",
                    "6": "Cyan at Imperial Camp 2",
                    "7": "Piranha Battle",
                    "8": "Kefka vs. Sabin",
                    "9": "Kefka vs. Terra",
                    "10": "Gau on the Veldt",
                    "12": "Sealed Gate 1",
                    "13": "Sealed Gate 2",
                    "14": "Espers on Blackjack",
                    "15": "Ultros vs. Relm",
                    "16": "Espers go to Thamasa",
                    "17": "Blitz Instructions (Unused)",
                    "18": "Kefka vs. Esper",
                    "19": "Kefka vs. Gestahl",
                    "20": "Kefka vs. Leo"
                }
            }
        },
        "characterExpProgression": {
            "type": "array",
            "name": "Character Experience Progression",
            "editor": "FF6LevelProgression",
            "range": "0xED8220-0xED82E4",
            "array": {
                "length": 98,
                "max": 98
            },
            "assembly": {
                "type": "data",
                "length": 2,
                "assembly": {
                    "exp": {
                        "type": "property",
                        "name": "Experience",
                        "mask": "0xFFFF",
                        "multiplier": 8
                    },
                    "hp": {
                        "type": "assembly",
                        "external": "characterHPProgression[%i].hp"
                    },
                    "mp": {
                        "type": "assembly",
                        "external": "characterMPProgression[%i].mp"
                    }
                }
            }
        },
        "characterHPProgression": {
            "type": "array",
            "name": "Character HP Progression",
            "range": "0xE6F4A0-0xE6F502",
            "array": {
                "length": 98,
                "max": 98
            },
            "assembly": {
                "type": "data",
                "length": 1,
                "assembly": {
                    "hp": {
                        "type": "property",
                        "name": "HP"
                    }
                }
            }
        },
        "characterLevelProgression": {
            "type": "assembly",
            "name": "characterLevelProgression",
            "editor": "FF6LevelProgression"
        },
        "characterMPProgression": {
            "type": "array",
            "name": "Character MP Progression",
            "range": "0xE6F502-0xE6F564",
            "array": {
                "length": 98,
                "max": 98
            },
            "assembly": {
                "type": "data",
                "length": 1,
                "assembly": {
                    "mp": {
                        "type": "property",
                        "name": "MP"
                    }
                }
            }
        },
        "characterNames": {
            "type": "array",
            "name": "Character Name",
            "range": "0xC478C0-0xC47A40",
            "array": {
                "length": 64,
                "max": 64
            },
            "assembly": {
                "type": "text",
                "encoding": "smallText",
                "length": 6
            },
            "stringTable": {
                "length": 64,
                "default": "<characterNames[%i]>"
            }
        },
        "characterProperties": {
            "type": "array",
            "name": "Character Properties",
            "range": "0xED7CA0-0xED8220",
            "array": {
                "length": 64
            },
            "assembly": {
                "type": "data",
                "length": 22,
                "assembly": {
                    "characterName": {
                        "type": "assembly",
                        "name": "Name",
                        "external": "characterNames[%i]"
                    },
                    "hp": {
                        "type": "property",
                        "name": "Base HP",
                        "begin": 0
                    },
                    "mp": {
                        "type": "property",
                        "name": "Base MP",
                        "begin": 1
                    },
                    "command1": {
                        "type": "property",
                        "name": "Command 1",
                        "begin": 2,
                        "stringTable": "battleCommandName"
                    },
                    "command2": {
                        "type": "property",
                        "name": "Command 2",
                        "begin": 3,
                        "stringTable": "battleCommandName"
                    },
                    "command3": {
                        "type": "property",
                        "name": "Command 3",
                        "begin": 4,
                        "stringTable": "battleCommandName"
                    },
                    "command4": {
                        "type": "property",
                        "name": "Command 4",
                        "begin": 5,
                        "stringTable": "battleCommandName"
                    },
                    "vigor": {
                        "type": "property",
                        "name": "Vigor",
                        "begin": 6
                    },
                    "speed": {
                        "type": "property",
                        "name": "Speed",
                        "begin": 7
                    },
                    "stamina": {
                        "type": "property",
                        "name": "Stamina",
                        "begin": 8
                    },
                    "magicPower": {
                        "type": "property",
                        "name": "Magic Power",
                        "begin": 9
                    },
                    "battlePower": {
                        "type": "property",
                        "name": "Battle Power",
                        "begin": 10
                    },
                    "defense": {
                        "type": "property",
                        "name": "Defense",
                        "begin": 11
                    },
                    "magicDefense": {
                        "type": "property",
                        "name": "Magic Defense",
                        "begin": 12
                    },
                    "evade": {
                        "type": "property",
                        "name": "Evade",
                        "begin": 13
                    },
                    "magicBlock": {
                        "type": "property",
                        "name": "Magic Block",
                        "begin": 14
                    },
                    "weapon": {
                        "type": "property",
                        "name": "Weapon",
                        "begin": 15,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "shield": {
                        "type": "property",
                        "name": "Shield",
                        "begin": 16,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "helmet": {
                        "type": "property",
                        "name": "Helmet",
                        "begin": 17,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "armor": {
                        "type": "property",
                        "name": "Armor",
                        "begin": 18,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "relic1": {
                        "type": "property",
                        "name": "Relic 1",
                        "begin": 19,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "relic2": {
                        "type": "property",
                        "name": "Relic 2",
                        "begin": 20,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "runFactor": {
                        "type": "property",
                        "name": "Run Factor",
                        "begin": 21,
                        "mask": "0x03",
                        "stringTable": {
                            "hideIndex": true,
                            "string": {
                                "0": "High",
                                "1": "Normal",
                                "2": "Low",
                                "3": "Very Low"
                            }
                        }
                    },
                    "levelModifier": {
                        "type": "property",
                        "name": "Level Modifier",
                        "begin": 21,
                        "mask": "0x0C",
                        "stringTable": {
                            "hideIndex": true,
                            "string": {
                                "0": "None",
                                "1": "+2",
                                "2": "+5",
                                "3": "-3"
                            }
                        }
                    },
                    "fixedEquipment": {
                        "type": "property",
                        "name": "Fixed Equipment",
                        "begin": 21,
                        "mask": "0x10",
                        "bool": true
                    }
                }
            },
            "stringTable": "characterNames"
        },
        "cinematicProgram": {
            "type": "data",
            "name": "Cinematic Program",
            "range": "0xC2686C-0xC28A60",
            "format": "ff6-lzss",
            "assembly": {
                "splashPalette": {
                    "type": "assembly",
                    "name": "Splash Palette",
                    "range": "0x0135-0x0155",
                    "format": "bgr555"
                },
                "introFontPalette": {
                    "type": "assembly",
                    "name": "Intro Font Palette",
                    "range": "0x1400-0x1410",
                    "format": "bgr555"
                },
                "magitekPalette": {
                    "type": "assembly",
                    "name": "Magitek March Palette",
                    "range": "0x1410-0x1490",
                    "format": "bgr555"
                },
                "magitekColorOffset": {
                    "type": "assembly",
                    "name": "Magitek March Color Offset",
                    "range": "0x14B8-0x1598"
                },
                "floatingIslandPalette": {
                    "type": "assembly",
                    "name": "Floating Island Cinematic Palette",
                    "range": "0x1CD2-0x1CF2",
                    "format": "bgr555"
                },
                "worldOfRuinPalette": {
                    "type": "assembly",
                    "name": "World of Ruin Cinematic Palette",
                    "range": "0x217D-0x21FD",
                    "format": "bgr555"
                },
                "introSpritePalette": {
                    "type": "assembly",
                    "name": "Intro Sprite Palette",
                    "range": "0x2B43-0x2B63",
                    "format": "bgr555"
                },
                "introBackgroundPalette": {
                    "type": "assembly",
                    "name": "Intro Background Palette",
                    "range": "0x2B63-0x2C63",
                    "format": "bgr555"
                }
            }
        },
        "creditsColorOffset1": {
            "type": "assembly",
            "name": "Credits Color Offset (Sea)",
            "range": "0xCFF969-0xCFF9E1"
        },
        "creditsColorOffset2": {
            "type": "assembly",
            "name": "Credits Color Offset (Land)",
            "range": "0xCFF9E1-0xCFFAA9"
        },
        "creditsGraphics": {
            "type": "data",
            "name": "Credits Graphics Data",
            "range": "0xD9568F-0xD99D4B",
            "format": "ff6-lzss",
            "assembly": {
                "graphics1": {
                    "type": "graphics",
                    "name": "Credits Graphics 1",
                    "range": "0x0000-0x17BA",
                    "format": "snes4bpp"
                },
                "graphics2": {
                    "type": "graphics",
                    "name": "Credits Graphics 2",
                    "range": "0x17BA-0x1D3A",
                    "format": "snes4bpp"
                },
                "landLayout": {
                    "type": "tilemap",
                    "name": "Credits Land Layout",
                    "range": "0x1D3A-0x2D3A",
                    "format": ["generic4bppTile", "interlace(32,2,32)"],
                    "graphics": {
                        "path": "creditsGraphics.graphics4"
                    },
                    "palette": [[
                        {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0x70-0x80",
                            "offset": "0x40"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0x80-0x90",
                            "offset": "0x60"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0x90-0xA0",
                            "offset": "0x70"
                        }
                    ]],
                    "colorOffset": {
                        "path": "creditsColorOffset2",
                        "multiplier": 16,
                        "perTile": true
                    },
                    "width": 64,
                    "height": 64
                },
                "graphics3": {
                    "type": "graphics",
                    "name": "Credits Graphics 3",
                    "range": "0x2D3A-0x3C1A",
                    "format": "snes4bpp"
                },
                "cloudsLayout": {
                    "type": "tilemap",
                    "name": "Credits Clouds Layout",
                    "range": "0x3C1A-0x401A",
                    "format": "generic4bppTile",
                    "graphics": {
                        "path": "creditsGraphics.graphics3"
                    },
                    "palette": {
                        "path": "endingCreditsBackgroundPalette",
                        "range": "0x60-0x70"
                    },
                    "width": 32,
                    "height": 32
                },
                "airshipLayout1": {
                    "type": "tilemap",
                    "name": "Credits Airship Layout 1",
                    "range": "0x401A-0x441A",
                    "format": "generic4bppTile",
                    "graphics": {
                        "path": "creditsGraphics.graphics3",
                        "offset": "0x0000"
                    },
                    "palette": {
                        "path": "endingCreditsBackgroundPalette",
                        "range": "0x40-0x50"
                    },
                    "width": 32,
                    "height": 32
                },
                "airshipLayout2": {
                    "type": "tilemap",
                    "name": "Credits Airship Layout 2",
                    "range": "0x441A-0x541A",
                    "format": ["generic4bppTile", "interlace(32,2,32)"],
                    "graphics": {
                        "path": "creditsGraphics.graphics3"
                    },
                    "palette": {
                        "path": "endingCreditsBackgroundPalette",
                        "range": "0x40-0x50"
                    },
                    "width": 64,
                    "height": 32
                },
                "graphics4": {
                    "type": "graphics",
                    "name": "Credits Graphics 4",
                    "range": "0x541A-0x6CFA",
                    "format": "snes4bpp"
                },
                "seaLayout": {
                    "type": "tilemap",
                    "name": "Credits Sea Layout",
                    "range": "0x6CFA-0x74FA",
                    "format": ["generic4bppTile", "interlace(32,2,32)"],
                    "graphics": {
                        "path": "creditsGraphics.graphics4"
                    },
                    "palette": {
                        "path": "endingCreditsBackgroundPalette",
                        "range": "0x50-0x60"
                    },
                    "width": 64,
                    "height": 32
                }
            }
        },
        "danceAttacks": {
            "type": "array",
            "name": "Dance Attacks",
            "range": "0xCFFE80-0xCFFEA0",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "data",
                "length": 4,
                "assembly": {
                    "danceName": {
                        "type": "assembly",
                        "external": "danceName[%i]"
                    },
                    "background": {
                        "type": "assembly",
                        "external": "danceBackground[%i]"
                    },
                    "attack1": {
                        "type": "property",
                        "name": "Attack 1",
                        "begin": 0,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    },
                    "attack2": {
                        "type": "property",
                        "name": "Attack 2",
                        "begin": 1,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    },
                    "attack3": {
                        "type": "property",
                        "name": "Attack 3",
                        "begin": 2,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    },
                    "attack4": {
                        "type": "property",
                        "name": "Attack 4",
                        "begin": 3,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    }
                }
            },
            "stringTable": "danceName"
        },
        "danceBackground": {
            "type": "array",
            "name": "Dance Background",
            "range": "0xD1F9AB-0xD1F9B3",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "property",
                "name": "Background",
                "stringTable": "battleBackground",
                "link": "battleBackgroundProperties[%i]"
            }
        },
        "danceName": {
            "type": "array",
            "name": "Dance Name",
            "range": "0xE6FF9D-0xE6FFFF",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "text",
                "encoding": "smallText",
                "length": 12
            },
            "stringTable": {
                "length": 8,
                "default": "<danceName[%i]>"
            }
        },
        "dialog": {
            "type": "array",
            "name": "Dialogue",
            "range": "0xCD0000-0xCEEFBB",
            "align": "0x10000",
            "pointerAlign": "0x10000",
            "autoBank": true,
            "canRelocate": true,
            "array": {
                "length": 3084
            },
            "assembly": {
                "type": "text",
                "encoding": "dialog",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xCCE602-0xCD0000",
                "offset": "0xCD0000",
                "canRelocate": true,
                "reference": [
                    {
                        "begin": "0xC07FCA",
                        "mask": "0xFFFFFF"
                    }
                ]
            },
            "stringTable": {
                "length": 3084,
                "default": "<dialog[%i]>"
            },
            "reference": [
                {
                    "begin": "0xC07F72",
                    "mask": "0xFF",
                    "bankByte": true
                }, {
                    "begin": "0xC07FC0",
                    "mask": "0xFF",
                    "bankByte": true
                }, {
                    "target": "dialogBankIncrement",
                    "mask": "0xFFFF",
                    "eval": "this.parent.array.find(function(d) { return d.range.begin >= 0x10000; }).i"
                }
            ]
        },
        "dialogBankIncrement": {
            "type": "assembly",
            "name": "Dialogue Bank Increment",
            "range": "0xCCE600-0xCCE602",
            "canRelocate": true,
            "reference": [
                {
                    "begin": "0xC07FD2",
                    "mask": "0xFFFFFF"
                }
            ]
        },
        "endingCreditsBackgroundPalette": {
            "type": "assembly",
            "name": "Ending/Credits Background Palette",
            "range": "0xC2953C-0xC2973C",
            "format": "bgr555"
        },
        "endingGraphics": {
            "type": "data",
            "name": "Ending Graphics Data",
            "range": "0xC4C008-0xC4F477",
            "format": "ff6-lzss",
            "assembly": {
                "graphics1": {
                    "type": "graphics",
                    "name": "Ending Graphics 1",
                    "range": "0x0000-0x1F60",
                    "format": "snes4bpp"
                },
                "graphics2": {
                    "type": "graphics",
                    "name": "Ending Graphics 2",
                    "range": "0x1F60-0x2AA0",
                    "format": "snes4bpp"
                },
                "graphics3": {
                    "type": "graphics",
                    "name": "Ending Graphics 3",
                    "range": "0x2AA0-0x38A0",
                    "format": "snes4bpp"
                },
                "tableLayout": {
                    "type": "tilemap",
                    "name": "Ending Table Layout",
                    "range": "0x38A0-0x4020",
                    "format": "snes4bppTile",
                    "graphics": [[
                        {
                            "path": "endingGraphics.graphics1",
                            "offset": "0x0000"
                        }, {
                            "path": "endingGraphics.graphics2",
                            "offset": "0x4000"
                        }, {
                            "path": "endingGraphics.graphics3",
                            "offset": "0x8000"
                        }
                    ]],
                    "palette": [[
                        {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xA0-0xB0",
                            "offset": "0x10"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xB0-0xC0",
                            "offset": "0x20"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xD0-0xE0",
                            "offset": "0x50"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xE0-0xF0",
                            "offset": "0x60"
                        }
                    ]],
                    "width": 32,
                    "height": 30
                },
                "miscLayout": {
                    "type": "tilemap",
                    "name": "Ending Misc. Layout",
                    "range": "0x4020-0x4448",
                    "format": "snes4bppTile",
                    "graphics": [[
                        {
                            "path": "endingGraphics.graphics1",
                            "offset": "0x0000"
                        }, {
                            "path": "endingGraphics.graphics2",
                            "offset": "0x4000"
                        }, {
                            "path": "endingGraphics.graphics3",
                            "offset": "0x8000"
                        }
                    ]],
                    "palette": [[
                        {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xA0-0xB0",
                            "offset": "0x10"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xB0-0xC0",
                            "offset": "0x20"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xD0-0xE0",
                            "offset": "0x50"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xE0-0xF0",
                            "offset": "0x60"
                        }
                    ]],
                    "width": 32,
                    "height": 17
                },
                "barsLayout": {
                    "type": "tilemap",
                    "name": "Ending Bars Layout",
                    "range": "0x4448-0x4BC8",
                    "format": "snes4bppTile",
                    "graphics": [[
                        {
                            "path": "endingGraphics.graphics1",
                            "offset": "0x0000"
                        }, {
                            "path": "endingGraphics.graphics2",
                            "offset": "0x4000"
                        }, {
                            "path": "endingGraphics.graphics3",
                            "offset": "0x8000"
                        }
                    ]],
                    "palette": [[
                        {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xA0-0xB0",
                            "offset": "0x10"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xB0-0xC0",
                            "offset": "0x20"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xD0-0xE0",
                            "offset": "0x50"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xE0-0xF0",
                            "offset": "0x60"
                        }
                    ]],
                    "width": 32,
                    "height": 30
                },
                "bookLayout": {
                    "type": "tilemap",
                    "name": "Ending Book Layout",
                    "range": "0x4BC8-0x5040",
                    "format": "snes4bppTile",
                    "graphics": [[
                        {
                            "path": "endingGraphics.graphics1",
                            "offset": "0x0000"
                        }, {
                            "path": "endingGraphics.graphics2",
                            "offset": "0x4000"
                        }, {
                            "path": "endingGraphics.graphics3",
                            "offset": "0x8000"
                        }
                    ]],
                    "palette": [[
                        {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xA0-0xB0",
                            "offset": "0x10"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xB0-0xC0",
                            "offset": "0x20"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xD0-0xE0",
                            "offset": "0x50"
                        }, {
                            "path": "endingCreditsBackgroundPalette",
                            "range": "0xE0-0xF0",
                            "offset": "0x60"
                        }
                    ]],
                    "width": 32,
                    "height": 18
                }
            }
        },
        "entranceTriggersMulti": {
            "type": "array",
            "name": "Multi-Tile Entrance Triggers",
            "range": "0xEDF882-0xEDFCAA",
            "isSequential": true,
            "endPointer": true,
            "canRelocate": true,
            "array": {
                "length": 415
            },
            "assembly": {
                "type": "array",
                "assembly": {
                    "type": "data",
                    "assembly": {
                        "map": {
                            "type": "property",
                            "name": "Map",
                            "begin": 3,
                            "mask": "0x01FF",
                            "stringTable": "mapProperties",
                            "link": "mapProperties[%i]"
                        },
                        "x": {
                            "type": "property",
                            "name": "X Position",
                            "begin": 0,
                            "hidden": true
                        },
                        "y": {
                            "type": "property",
                            "name": "Y Position",
                            "begin": 1,
                            "hidden": true
                        },
                        "length": {
                            "type": "property",
                            "name": "Length",
                            "begin": 2,
                            "mask": "0x7F",
                            "offset": 1
                        },
                        "vertical": {
                            "type": "property",
                            "name": "Vertical",
                            "begin": 2,
                            "mask": "0x80",
                            "bool": true
                        },
                        "setParent": {
                            "type": "property",
                            "name": "Set Parent Map",
                            "begin": 4,
                            "mask": "0x02",
                            "bool": true
                        },
                        "z": {
                            "type": "property",
                            "name": "Z-Level",
                            "begin": 4,
                            "mask": "0x04",
                            "stringTable": {
                                "string": {
                                    "0": "Lower",
                                    "1": "Upper"
                                }
                            }
                        },
                        "title": {
                            "type": "property",
                            "name": "Show Map Title",
                            "begin": 4,
                            "mask": "0x08",
                            "bool": true
                        },
                        "direction": {
                            "type": "property",
                            "name": "Facing Direction",
                            "begin": 4,
                            "mask": "0x30",
                            "stringTable": "directions"
                        },
                        "xDest": {
                            "type": "property",
                            "name": "X Destination",
                            "begin": 5
                        },
                        "yDest": {
                            "type": "property",
                            "name": "Y Destination",
                            "begin": 6
                        }
                    },
                    "length": 7
                }
            },
            "pointerTable": {
                "range": "0xEDF480-0xEDF882",
                "offset": "0xEDF480",
                "canRelocate": true,
                "reference": [
                    {
                        "begin": "0xC018EB",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }, {
                        "begin": "0xC018F1",
                        "mask": "0xFFFFFF"
                    }
                ]
            },
            "reference": [
                {
                    "begin": "0xC01904",
                    "mask": "0xFFFFFF",
                    "pointerOffset": true,
                    "offset": -2
                }, {
                    "begin": "0xC0190C",
                    "mask": "0xFFFFFF",
                    "offset": -1,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01917",
                    "mask": "0xFFFFFF",
                    "pointerOffset": true
                }, {
                    "begin": "0xC0191F",
                    "mask": "0xFFFFFF",
                    "pointerOffset": true
                }, {
                    "begin": "0xC01930",
                    "mask": "0xFFFFFF",
                    "pointerOffset": true
                }, {
                    "begin": "0xC0193B",
                    "mask": "0xFFFFFF",
                    "offset": -1,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01943",
                    "mask": "0xFFFFFF",
                    "offset": -1,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01964",
                    "mask": "0xFFFFFF",
                    "offset": -3,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01970",
                    "mask": "0xFFFFFF",
                    "offset": -3,
                    "pointerOffset": true
                }, {
                    "begin": "0xC0197C",
                    "mask": "0xFFFFFF",
                    "offset": -3,
                    "pointerOffset": true
                }, {
                    "begin": "0xC0198B",
                    "mask": "0xFFFFFF",
                    "offset": -5,
                    "pointerOffset": true
                }, {
                    "begin": "0xC019A0",
                    "mask": "0xFFFFFF",
                    "offset": -5,
                    "pointerOffset": true
                }, {
                    "begin": "0xC019AA",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }, {
                    "begin": "0xC019B7",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }, {
                    "begin": "0xC019C5",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }, {
                    "begin": "0xC019E6",
                    "mask": "0xFFFFFF",
                    "offset": -3,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01A15",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }
            ]
        },
        "entranceTriggersSingle": {
            "type": "array",
            "name": "Single-Tile Entrance Triggers",
            "range": "0xDFBF02-0xDFD978",
            "isSequential": true,
            "endPointer": true,
            "canRelocate": true,
            "array": {
                "length": 415
            },
            "assembly": {
                "type": "array",
                "assembly": {
                    "type": "data",
                    "assembly": {
                        "x": {
                            "type": "property",
                            "name": "X Position",
                            "begin": 0,
                            "hidden": true
                        },
                        "y": {
                            "type": "property",
                            "name": "Y Position",
                            "begin": 1,
                            "hidden": true
                        },
                        "map": {
                            "type": "property",
                            "name": "Map",
                            "begin": 2,
                            "mask": "0x01FF",
                            "stringTable": "mapProperties",
                            "link": "mapProperties[%i]"
                        },
                        "setParent": {
                            "type": "property",
                            "name": "Set Parent Map",
                            "begin": 3,
                            "mask": "0x02",
                            "bool": true
                        },
                        "z": {
                            "type": "property",
                            "name": "Z-Level",
                            "begin": 3,
                            "mask": "0x04",
                            "stringTable": {
                                "string": {
                                    "0": "Lower",
                                    "1": "Upper"
                                }
                            }
                        },
                        "title": {
                            "type": "property",
                            "name": "Show Map Title",
                            "begin": 3,
                            "mask": "0x08",
                            "bool": true
                        },
                        "direction": {
                            "type": "property",
                            "name": "Facing Direction",
                            "begin": 3,
                            "mask": "0x30",
                            "stringTable": "directions"
                        },
                        "xDest": {
                            "type": "property",
                            "name": "X Destination",
                            "begin": 4
                        },
                        "yDest": {
                            "type": "property",
                            "name": "Y Destination",
                            "begin": 5
                        }
                    },
                    "length": 6
                }
            },
            "pointerTable": {
                "range": "0xDFBB00-0xDFBF02",
                "offset": "0xDFBB00",
                "canRelocate": true,
                "reference": [
                    {
                        "begin": "0xC01A7E",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }, {
                        "begin": "0xC01A84",
                        "mask": "0xFFFFFF"
                    }, {
                        "begin": "0xEE20EA",
                        "mask": "0xFFFFFF"
                    }, {
                        "begin": "0xEE20F0",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }
                ]
            },
            "reference": [
                {
                    "begin": "0xC01A90",
                    "mask": "0xFFFFFF",
                    "pointerOffset": true
                }, {
                    "begin": "0xC01AAB",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01AB7",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01AC3",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01AD2",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01AE7",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01AF1",
                    "mask": "0xFFFFFF",
                    "offset": -3,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01AFE",
                    "mask": "0xFFFFFF",
                    "offset": -3,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01B0C",
                    "mask": "0xFFFFFF",
                    "offset": -3,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01B2D",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xC01B5F",
                    "mask": "0xFFFFFF",
                    "offset": -3,
                    "pointerOffset": true
                }, {
                    "begin": "0xEE20FF",
                    "mask": "0xFFFFFF",
                    "pointerOffset": true
                }, {
                    "begin": "0xEE2107",
                    "mask": "0xFFFFFF",
                    "offset": -1,
                    "pointerOffset": true
                }, {
                    "begin": "0xEE2111",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xEE213D",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }
            ]
        },
        "esperAttackDescriptions": {
            "type": "array",
            "name": "Esper Attack Descriptions",
            "range": "0xCF3940-0xCF3BAD",
            "array": {
                "length": 27
            },
            "assembly": {
                "type": "text",
                "name": "Attack Desc.",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xCFFE40-0xCFFE80",
                "offset": "0xCF3940",
                "reference": [
                    {
                        "begin": "0xC35C0A",
                        "mask": "0xFFFF"
                    }, {
                        "begin": "0xC35C14",
                        "mask": "0xFF",
                        "bankByte": true
                    }
                ]
            },
            "reference": [
                {
                    "begin": "0xC35C0F",
                    "mask": "0xFFFF"
                }, {
                    "begin": "0xC35C18",
                    "mask": "0xFF",
                    "bankByte": true
                }
            ]
        },
        "esperAttackName": {
            "type": "array",
            "name": "Esper Attack Name",
            "range": "0xE6FE8F-0xE6FF9D",
            "array": {
                "length": 27
            },
            "assembly": {
                "type": "text",
                "name": "Attack Name",
                "encoding": "bigText",
                "length": 10
            },
            "stringTable": {
                "length": 27,
                "default": "<esperAttackName[%i]>"
            }
        },
        "esperBonusDescriptions": {
            "type": "array",
            "name": "Esper Bonus Descriptions",
            "range": "0xEDFE00-0xEDFFA7",
            "array": {
                "length": 17
            },
            "assembly": {
                "type": "text",
                "name": "Description",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xEDFFD0-0xEE0000",
                "offset": "0xEDFE00",
                "reference": [
                    {
                        "begin": "0xC35BF7",
                        "mask": "0xFFFF"
                    }, {
                        "begin": "0xC35C01",
                        "mask": "0xFF",
                        "bankByte": true
                    }
                ]
            },
            "reference": [
                {
                    "begin": "0xC35BFC",
                    "mask": "0xFFFF"
                }, {
                    "begin": "0xC35C05",
                    "mask": "0xFF",
                    "bankByte": true
                }
            ]
        },
        "esperBonusNames": {
            "type": "array",
            "name": "Esper Bonus Names",
            "range": "0xCFFEAE-0xCFFF9E",
            "array": {
                "length": 17
            },
            "assembly": {
                "type": "data",
                "length": 9,
                "assembly": {
                    "bonusName": {
                        "type": "text",
                        "name": "Name",
                        "encoding": "smallText",
                        "length": 9
                    },
                    "description": {
                        "type": "assembly",
                        "external": "esperBonusDescriptions[%i]"
                    }
                }
            },
            "stringTable": {
                "length": 17,
                "default": "<esperBonusNames[%i].bonusName>",
                "string": {
                    "255": "None"
                }
            }
        },
        "esperMenuOrder": {
            "type": "array",
            "name": "Esper Menu Order",
            "range": "0xD1F9B5-0xD1F9D0",
            "array": {
                "length": 27
            },
            "assembly": {
                "type": "property",
                "name": "Menu Order",
                "length": 1
            }
        },
        "esperNames": {
            "type": "array",
            "name": "Esper Name",
            "range": "0xE6F6E1-0xE6F7B9",
            "array": {
                "length": 27
            },
            "assembly": {
                "type": "text",
                "name": "Name",
                "encoding": "smallText",
                "length": 8
            },
            "stringTable": {
                "length": 27,
                "default": "<esperNames[%i]>"
            }
        },
        "esperProperties": {
            "type": "array",
            "name": "Esper Properties",
            "range": "0xD86E00-0xD87000",
            "array": {
                "length": 27
            },
            "assembly": {
                "type": "data",
                "assembly": {
                    "esperName": {
                        "type": "assembly",
                        "external": "esperNames[%i]"
                    },
                    "attackName": {
                        "type": "assembly",
                        "external": "esperAttackName[%i]"
                    },
                    "attackDescription": {
                        "type": "assembly",
                        "external": "esperAttackDescriptions[%i]"
                    },
                    "menuOrder": {
                        "type": "assembly",
                        "external": "esperMenuOrder[%i]"
                    },
                    "spell1": {
                        "type": "property",
                        "name": "Spell 1",
                        "begin": 1,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "rate1": {
                        "type": "property",
                        "name": "Rate 1",
                        "begin": 0,
                        "max": 100
                    },
                    "spell2": {
                        "type": "property",
                        "name": "Spell 2",
                        "begin": 3,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "rate2": {
                        "type": "property",
                        "name": "Rate 2",
                        "begin": 2,
                        "max": 100
                    },
                    "spell3": {
                        "type": "property",
                        "name": "Spell 3",
                        "begin": 5,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "rate3": {
                        "type": "property",
                        "name": "Rate 3",
                        "begin": 4,
                        "max": 100
                    },
                    "spell4": {
                        "type": "property",
                        "name": "Spell 4",
                        "begin": 7,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "rate4": {
                        "type": "property",
                        "name": "Rate 4",
                        "begin": 6,
                        "max": 100
                    },
                    "spell5": {
                        "type": "property",
                        "name": "Spell 5",
                        "begin": 9,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "rate5": {
                        "type": "property",
                        "name": "Rate 5",
                        "begin": 8,
                        "max": 100
                    },
                    "bonus": {
                        "type": "property",
                        "name": "Bonus",
                        "begin": 10,
                        "stringTable": "esperBonusNames",
                        "link": "esperBonusNames[%i]"
                    }
                },
                "length": 11
            },
            "stringTable": "esperNames"
        },
        "eventScript": {
            "type": "script",
            "name": "Event Script",
            "range": "0xCA0000-0xCCE5EE",
            "align": "0x10000",
            "encoding": ["event", "object", "world", "vehicle"],
            "label": {
                "No Event": "0xCA0000",
                "Wait for Dialogue Window": "0xCA0001",
                "Game Start": "0xCA0003",
                "Chest: Item": "0xCA0008",
                "Chest: Spell": "0xCA000C",
                "Chest: GP": "0xCA0010",
                "Chest: Empty": "0xCA0014",
                "Random Battle": "0xCA0018",
                "Tent": "0xCA0034",
                "Warp/Warp Stone": "0xCA0039",
                "Chest: Monster-in-a-box": "0xCA0040",
                "Falcon: Deck": "0xCA0078",
                "Doom Gaze": "0xCA009D",
                "Tent: Animation": "0xCA00EA",
                "Warp: Animation": "0xCA0108",
                "Tent: Animation (World)": "0xCA015E",
                "Kefka's Tower": "0xCA01A2",
                "Phoenix Cave": "0xCA0405",
                "Final Battle/Ending": "0xCA057D",
                "Floating Island: Cinematic": "0xCA5ADE",
                "New Game": "0xCA5E33",
                "Post-Battle": "0xCA5EA9",
                "Return": "0xCA5EB3",
                "Return (World)": "0xCA5EB4",
                "Blackjack Book": {
                    "offset": "0xCAA6C0",
                    "encoding": "vehicle"
                },
                "Check Facing Direction": "0xCACA64",
                "Inn: No Dream": "0xCACD31",
                "Inn: Normal": "0xCACD3C",
                "Inn: Dream 1": "0xCACD5B",
                "Inn: Dream 2": "0xCACDD9",
                "Inn: Dream 3": "0xCACE51",
                "Inn: Dream 4": "0xCACEFE",
                "Inn: Fade Out": "0xCACF67",
                "Inn: Fade In": "0xCACF96",
                "Not Enough GP": "0xCB69FF",
                "Prologue": "0xCC985B",
                "Save Point": "0xCC9AEB",
                "Game Over": "0xCCE566"
            }
        },
        "eventTriggers": {
            "type": "array",
            "name": "Event Triggers",
            "range": "0xC40342-0xC419FE",
            "isSequential": true,
            "endPointer": true,
            "canRelocate": true,
            "array": {
                "length": 415
            },
            "assembly": {
                "type": "array",
                "assembly": {
                    "type": "data",
                    "assembly": {
                        "x": {
                            "type": "property",
                            "name": "X Position",
                            "begin": 0,
                            "hidden": true
                        },
                        "y": {
                            "type": "property",
                            "name": "Y Position",
                            "begin": 1,
                            "hidden": true
                        },
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 2,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        }
                    },
                    "length": 5
                }
            },
            "pointerTable": {
                "range": "0xC40000-0xC40342",
                "offset": "0xC40000",
                "canRelocate": true,
                "reference": [
                    {
                        "begin": "0xC0BCAF",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }, {
                        "begin": "0xC0BCB5",
                        "mask": "0xFFFFFF"
                    }, {
                        "begin": "0xEE2177",
                        "mask": "0xFFFFFF"
                    }, {
                        "begin": "0xEE217D",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }
                ]
            },
            "reference": [
                {
                    "begin": "0xC0BCBE",
                    "mask": "0xFFFFFF",
                    "pointerOffset": true
                }, {
                    "begin": "0xC0BCD4",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xC0BCEE",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }, {
                    "begin": "0xEE218C",
                    "mask": "0xFFFFFF",
                    "pointerOffset": true
                }, {
                    "begin": "0xEE2194",
                    "mask": "0xFFFFFF",
                    "offset": -1,
                    "pointerOffset": true
                }, {
                    "begin": "0xEE219C",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xEE21A5",
                    "mask": "0xFFFFFF",
                    "offset": -3,
                    "pointerOffset": true
                }, {
                    "begin": "0xEE21AD",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }
            ]
        },
        "floatingIslandCinematic": {
            "type": "data",
            "name": "Floating Island Cinematic Graphics Data",
            "range": "0xD94E96-0xD9568F",
            "format": "ff6-lzss",
            "isSequential": true,
            "assembly": {
                "graphics": {
                    "type": "graphics",
                    "name": "Floating Island Cinematic Graphics",
                    "range": "0x0000-0x07C0",
                    "format": "snes4bpp",
                    "palette": "cinematicProgram.floatingIslandPalette",
                    "backColor": true
                },
                "layout": {
                    "type": "tilemap",
                    "name": "Floating Island Cinematic Layout",
                    "range": "0x07C0-0x0900",
                    "format": "generic4bppTile",
                    "graphics": "floatingIslandCinematic.graphics",
                    "palette": "cinematicProgram.floatingIslandPalette",
                    "width": 32,
                    "height": 10,
                    "backColor": true
                }
            }
        },
        "fontGraphicsLarge": {
            "type": "graphics",
            "name": "Large Font Graphics",
            "range": "0xC490C0-0xC4A4C0",
            "format": ["linear1bpp(1)", "byteSwapped(2)"],
            "tileWidth": 16,
            "tileHeight": 11
        },
        "fontGraphicsSmall": {
            "type": "graphics",
            "name": "Small Font Graphics",
            "range": "0xC487C0-0xC48FC0",
            "format": "snes2bpp",
            "palette": "fontPalette"
        },
        "fontPalette": {
            "type": "array",
            "name": "Font Palette",
            "range": "0xD8E800-0xD8E8A0",
            "arrayLength": 40,
            "assembly": {
                "type": "assembly",
                "length": 8,
                "format": "bgr555"
            }
        },
        "initialLore": {
            "type": "array",
            "name": "Initial Lore",
            "range": "0xE6F564-0xE6F567",
            "format": "linear1bpp",
            "array": {
                "length": 24
            },
            "assembly": {
                "type": "data",
                "length": 1,
                "assembly": {
                    "initialLore": {
                        "type": "property",
                        "name": "Lore Known Initially",
                        "mask": "0x01",
                        "bool": true
                    }
                }
            }
        },
        "initialRage": {
            "type": "array",
            "name": "Initial Rage",
            "range": "0xC47AA0-0xC47AC0",
            "format": "linear1bpp",
            "array": {
                "length": 256
            },
            "assembly": {
                "type": "property",
                "name": "Rage Known Initially",
                "mask": "0x01",
                "bool": true,
                "length": 1
            }
        },
        "itemColosseum": {
            "type": "array",
            "name": "Colosseum Items",
            "range": "0xDFB600-0xDFBA00",
            "array": {
                "length": 256
            },
            "stringTable": "itemNames",
            "assembly": {
                "type": "data",
                "length": 4,
                "assembly": {
                    "monster": {
                        "type": "property",
                        "name": "Monster",
                        "begin": 0,
                        "stringTable": "monsterName",
                        "link": "monsterProperties[%i]"
                    },
                    "prize": {
                        "type": "property",
                        "name": "Prize Item",
                        "begin": 2,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "hidePrize": {
                        "type": "property",
                        "name": "Prize Name",
                        "begin": 3,
                        "stringTable": {
                            "hideIndex": true,
                            "string": {
                                "0": "Shown",
                                "255": "Hidden"
                            }
                        }
                    }
                }
            }
        },
        "itemDescriptions": {
            "type": "array",
            "name": "Item Descriptions",
            "range": "0xED6400-0xED7787",
            "array": {
                "length": 256,
                "max": 256
            },
            "assembly": {
                "type": "text",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xED7AA0-0xED7CA0",
                "offset": "0xED6400"
            }
        },
        "itemManualEquip": {
            "type": "data",
            "name": "Manual Equip Items",
            "range": "0xED82E4-0xED82F4",
            "assembly": {
                "item1": {
                    "type": "property",
                    "name": "Item 1",
                    "begin": 0,
                    "stringTable": "itemNames",
                    "link": "itemProperties[%i]"
                },
                "item2": {
                    "type": "property",
                    "name": "Item 2",
                    "begin": 1,
                    "stringTable": "itemNames",
                    "link": "itemProperties[%i]"
                },
                "item3": {
                    "type": "property",
                    "name": "Item 3",
                    "begin": 2,
                    "stringTable": "itemNames",
                    "link": "itemProperties[%i]"
                },
                "item4": {
                    "type": "property",
                    "name": "Item 4",
                    "begin": 3,
                    "stringTable": "itemNames",
                    "link": "itemProperties[%i]"
                },
                "item5": {
                    "type": "property",
                    "name": "Item 5",
                    "begin": 4,
                    "stringTable": "itemNames",
                    "link": "itemProperties[%i]"
                },
                "item6": {
                    "type": "property",
                    "name": "Item 6",
                    "begin": 5,
                    "stringTable": "itemNames",
                    "link": "itemProperties[%i]"
                },
                "item7": {
                    "type": "property",
                    "name": "Item 7",
                    "begin": 6,
                    "stringTable": "itemNames",
                    "link": "itemProperties[%i]"
                },
                "item8": {
                    "type": "property",
                    "name": "Item 8",
                    "begin": 7,
                    "stringTable": "itemNames",
                    "link": "itemProperties[%i]"
                },
                "item9": {
                    "type": "property",
                    "name": "Item 9",
                    "begin": 8,
                    "stringTable": "itemNames",
                    "link": "itemProperties[%i]"
                },
                "item10": {
                    "type": "property",
                    "name": "Item 10",
                    "begin": 9,
                    "stringTable": "itemNames",
                    "link": "itemProperties[%i]"
                }
            }
        },
        "itemMetamorphosis": {
            "type": "array",
            "name": "Metamorphosis Items",
            "range": "0xC47F40-0xC47FA8",
            "array": {
                "length": 26
            },
            "assembly": {
                "type": "data",
                "length": 4,
                "assembly": {
                    "item1": {
                        "type": "property",
                        "name": "Item 1",
                        "begin": 0,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "item2": {
                        "type": "property",
                        "name": "Item 2",
                        "begin": 1,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "item3": {
                        "type": "property",
                        "name": "Item 3",
                        "begin": 2,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "item4": {
                        "type": "property",
                        "name": "Item 4",
                        "begin": 3,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    }
                }
            },
            "stringTable": {
                "default": "Metamorphosis Items %i",
                "length": 26,
                "string": {
                    "0": "Antidote, Green Cherry, Eyedrop, Soft",
                    "1": "Tent, Fenix Down, Warp Stone, Revivify",
                    "2": "Dried Meat",
                    "3": "Remedy",
                    "4": "Mithril",
                    "5": "Gold",
                    "6": "Crystal",
                    "7": "Imp",
                    "8": "Ogre Nix",
                    "9": "Marvel Shoes",
                    "10": "Tintinabar",
                    "11": "Megalixir",
                    "12": "X-Potion",
                    "13": "X-Ether",
                    "14": "Elixir",
                    "15": "Gauntlet",
                    "16": "Genji Glove",
                    "17": "Safety Bit",
                    "18": "Exp. Egg",
                    "19": "Ribbon",
                    "20": "Flame Shield",
                    "21": "Ice Shield",
                    "22": "Thunder Shield",
                    "23": "Cursed Ring, Thornlet, Relic Ring",
                    "24": "Cure Ring, Safety Bit, Pod Bracelet",
                    "25": "Trump, Assassin, Striker"
                }
            }
        },
        "itemNames": {
            "type": "array",
            "name": "Item Names",
            "range": "0xD2B300-0xD2C000",
            "array": {
                "length": 256,
                "max": 256
            },
            "assembly": {
                "type": "text",
                "length": 13,
                "encoding": "smallText"
            },
            "stringTable": {
                "length": 256,
                "default": "<itemNames[%i]>",
                "string": {
                    "0xFF": "Empty"
                }
            }
        },
        "itemProperties": {
            "type": "array",
            "name": "Item Properties",
            "range": "0xD85000-0xD86E00",
            "array": {
                "length": 256
            },
            "stringTable": "itemNames",
            "assembly": {
                "type": "data",
                "length": 30,
                "assembly": {
                    "itemName": {
                        "type": "assembly",
                        "name": "Name",
                        "external": "itemNames[%i]"
                    },
                    "itemDescription": {
                        "type": "assembly",
                        "name": "Description",
                        "external": "itemDescriptions[%i]"
                    },
                    "itemType": {
                        "type": "property",
                        "name": "Type",
                        "begin": 0,
                        "mask": "0x0F",
                        "stringTable": {
                            "string": {
                                "0": "Tool",
                                "1": "Weapon",
                                "2": "Armor",
                                "3": "Shield",
                                "4": "Helmet",
                                "5": "Relic",
                                "6": "Item"
                            }
                        }
                    },
                    "cost": {
                        "type": "property",
                        "name": "Cost",
                        "begin": 28,
                        "mask": "0xFFFF"
                    },
                    "usage": {
                        "type": "property",
                        "name": "Usage",
                        "begin": 0,
                        "mask": "0x70",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Throw",
                                "1": "Battle",
                                "2": "Menu"
                            }
                        }
                    },
                    "targeting": {
                        "type": "property",
                        "name": "Targeting",
                        "begin": 14,
                        "flag": true,
                        "stringTable": "targetingFlags"
                    },
                    "spellCast": {
                        "type": "property",
                        "name": "Spell Cast",
                        "begin": 18,
                        "mask": "0x3F",
                        "stringTable": "spellNames",
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value > 5)"
                    },
                    "randomSpellCast": {
                        "type": "property",
                        "name": "Random Spell Cast",
                        "begin": 18,
                        "mask": "0x40",
                        "bool": true,
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value > 5)"
                    },
                    "breakWhenUsed": {
                        "type": "property",
                        "name": "Break After Use",
                        "begin": 18,
                        "mask": "0x80",
                        "bool": true,
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value > 5)"
                    },
                    "spellLearned": {
                        "type": "property",
                        "name": "Spell Learned",
                        "begin": 4,
                        "stringTable": "spellNames",
                        "invalid": "(this.parent.itemType.value < 2) || (this.parent.itemType.value === 6)"
                    },
                    "spellRate": {
                        "type": "property",
                        "name": "Learn Rate",
                        "begin": 3,
                        "invalid": "(this.parent.itemType.value < 2) || (this.parent.itemType.value === 6)"
                    },
                    "canEquip": {
                        "type": "property",
                        "name": "Can Equip",
                        "begin": 1,
                        "mask": "0xFFFF",
                        "flag": true,
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "length": 16,
                            "default": "<characterNames[%i]>",
                            "string": {
                                "14": "Imp Item",
                                "15": "Heavy Item"
                            }
                        }
                    },
                    "attackPower": {
                        "type": "property",
                        "name": "Attack Power",
                        "begin": 20,
                        "invalid": "this.parent.itemType.value >= 2"
                    },
                    "hitRate": {
                        "type": "property",
                        "name": "Hit Rate",
                        "begin": 21,
                        "invalid": "this.parent.itemType.value >= 2"
                    },
                    "defense": {
                        "type": "property",
                        "name": "Defense",
                        "begin": 20,
                        "invalid": "(this.parent.itemType.value < 2) || (this.parent.itemType.value === 6)"
                    },
                    "magicDefense": {
                        "type": "property",
                        "name": "Magic Defense",
                        "begin": 21,
                        "invalid": "(this.parent.itemType.value < 2) || (this.parent.itemType.value === 6)"
                    },
                    "itemPower": {
                        "type": "property",
                        "name": "Item Power",
                        "begin": 20,
                        "invalid": "this.parent.itemType.value !== 6"
                    },
                    "vigor": {
                        "type": "property",
                        "name": "Vigor",
                        "begin": 16,
                        "mask": "0x0F",
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "hideIndex": true,
                            "string": {
                                "0": "+0",
                                "1": "+1",
                                "2": "+2",
                                "3": "+3",
                                "4": "+4",
                                "5": "+5",
                                "6": "+6",
                                "7": "+7",
                                "9": "-1",
                                "10": "-2",
                                "11": "-3",
                                "12": "-4",
                                "13": "-5",
                                "14": "-6",
                                "15": "-7"
                            }
                        }
                    },
                    "speed": {
                        "type": "property",
                        "name": "Speed",
                        "begin": 16,
                        "mask": "0xF0",
                        "stringTable": "itemProperties.vigor",
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)"
                    },
                    "stamina": {
                        "type": "property",
                        "name": "Stamina",
                        "begin": 17,
                        "mask": "0x0F",
                        "stringTable": "itemProperties.vigor",
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)"
                    },
                    "magicPower": {
                        "type": "property",
                        "name": "Magic Power",
                        "begin": 17,
                        "mask": "0xF0",
                        "stringTable": "itemProperties.vigor",
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)"
                    },
                    "evade": {
                        "type": "property",
                        "name": "Evade",
                        "begin": 26,
                        "mask": "0x0F",
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "hideIndex": true,
                            "string": {
                                "0": "+0",
                                "1": "+10%",
                                "2": "+20%",
                                "3": "+30%",
                                "4": "+40%",
                                "5": "+50%",
                                "6": "-10%",
                                "7": "-20%",
                                "8": "-30%",
                                "9": "-40%",
                                "10": "-50%"
                            }
                        }
                    },
                    "magicBlock": {
                        "type": "property",
                        "name": "Magic Block",
                        "begin": 26,
                        "mask": "0xF0",
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": "itemProperties.evade"
                    },
                    "weaponFlags": {
                        "type": "property",
                        "name": "Weapon Flags",
                        "begin": 19,
                        "flag": true,
                        "invalid": "this.parent.itemType.value !== 1",
                        "stringTable": {
                            "string": {
                                "1": "SwdTech",
                                "5": "No Back Row Penalty",
                                "6": "Two-Handed",
                                "7": "Enable Runic"
                            }
                        }
                    },
                    "weaponSpecialEffect": {
                        "type": "property",
                        "name": "Special Effect",
                        "begin": 27,
                        "mask": "0xF0",
                        "invalid": "this.parent.itemType.value !== 1",
                        "stringTable": "weaponSpecialEffect"
                    },
                    "itemFlags": {
                        "type": "property",
                        "name": "Item Flags",
                        "begin": 19,
                        "flag": true,
                        "invalid": "this.parent.itemType.value !== 6",
                        "stringTable": {
                            "string": {
                                "1": "Damage Undead",
                                "3": "Restore HP",
                                "4": "Restore MP",
                                "5": "Remove Status",
                                "6": "Cause Damage",
                                "7": "Fractional Power (1/16s)"
                            }
                        }
                    },
                    "itemSpecialEffect": {
                        "type": "property",
                        "name": "Special Effect",
                        "begin": 27,
                        "invalid": "this.parent.itemType.value !== 6",
                        "stringTable": "itemSpecialEffect"
                    },
                    "blockGraphics": {
                        "type": "property",
                        "name": "Block Graphics",
                        "begin": 27,
                        "mask": "0x03",
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "string": {
                                "0": "Dagger Parry",
                                "1": "Sword Parry",
                                "2": "Shield Block",
                                "3": "Cape Dodge"
                            }
                        }
                    },
                    "blockType": {
                        "type": "property",
                        "name": "Block Type",
                        "begin": 27,
                        "mask": "0x0C",
                        "flag": true,
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "string": {
                                "0": "Physical Attacks",
                                "1": "Magic Attacks"
                            }
                        }
                    },
                    "elementWeapon": {
                        "type": "property",
                        "name": "Element",
                        "begin": 15,
                        "flag": true,
                        "stringTable": "element",
                        "invalid": "this.parent.itemType.value !== 1"
                    },
                    "elementHalf": {
                        "type": "property",
                        "name": "50% Element",
                        "begin": 15,
                        "flag": true,
                        "stringTable": "element",
                        "invalid": "(this.parent.itemType.value < 2) || (this.parent.itemType.value === 6)"
                    },
                    "elementAbsorb": {
                        "type": "property",
                        "name": "Absorb Element",
                        "begin": 22,
                        "flag": true,
                        "stringTable": "element",
                        "invalid": "(this.parent.itemType.value < 2) || (this.parent.itemType.value === 6)"
                    },
                    "elementNull": {
                        "type": "property",
                        "name": "Null Element",
                        "begin": 23,
                        "flag": true,
                        "stringTable": "element",
                        "invalid": "(this.parent.itemType.value < 2) || (this.parent.itemType.value === 6)"
                    },
                    "elementWeak": {
                        "type": "property",
                        "name": "Weak Element",
                        "begin": 24,
                        "flag": true,
                        "stringTable": "element",
                        "invalid": "(this.parent.itemType.value < 2) || (this.parent.itemType.value === 6)"
                    },
                    "statusImmunity": {
                        "type": "property",
                        "name": "Immune",
                        "begin": 6,
                        "mask": "0xFFFF",
                        "flag": true,
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "length": 16,
                            "default": "<stringTable.statusNamesReversed[%i]>"
                        }
                    },
                    "statusHelpful": {
                        "type": "property",
                        "name": "Helpful Status",
                        "begin": 8,
                        "flag": true,
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "length": 8,
                            "default": "<stringTable.statusNamesReversed[%i+16]>",
                            "string": {
                                "0": "Float"
                            }
                        }
                    },
                    "statusHarmful": {
                        "type": "property",
                        "name": "Harmful Status",
                        "begin": 25,
                        "flag": true,
                        "invalid": "(this.parent.itemType.value < 2) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "length": 8,
                            "default": "<stringTable.statusNamesReversed[%i+8]>"
                        }
                    },
                    "statusItem1": {
                        "type": "property",
                        "name": "Status Effect",
                        "begin": 21,
                        "mask": "0xFFFF",
                        "flag": true,
                        "invalid": "this.parent.itemType.value !== 6",
                        "stringTable": {
                            "length": 16,
                            "default": "<stringTable.statusNamesReversed[%i]>"
                        }
                    },
                    "statusItem2": {
                        "type": "property",
                        "begin": 23,
                        "mask": "0xFFFF",
                        "flag": true,
                        "invalid": "this.parent.itemType.value !== 6",
                        "stringTable": {
                            "length": 16,
                            "default": "<stringTable.statusNamesReversed[%i+16]>"
                        }
                    },
                    "fieldEffects": {
                        "type": "property",
                        "name": "Field Effects",
                        "begin": 5,
                        "mask": "0xFF",
                        "flag": true,
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "string": {
                                "0": "Charm Bangle",
                                "1": "Moogle Charm",
                                "5": "Sprint Shoes",
                                "7": "Tintinabar (no effect)"
                            }
                        }
                    },
                    "relicEffects1": {
                        "type": "property",
                        "name": "Relic Effects",
                        "begin": 9,
                        "mask": "0xFFFFFF",
                        "flag": true,
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "string": {
                                "0": "Raise Fight Damage",
                                "1": "Raise Magic Damage",
                                "2": "HP +25%",
                                "3": "HP +50%",
                                "4": "HP +12.5%",
                                "5": "MP +25%",
                                "6": "MP +50%",
                                "7": "MP +12.5%",
                                "8": "Gale Hairpin",
                                "9": "Back Guard",
                                "10": "Fight → Jump",
                                "11": "Magic → X-Magic",
                                "12": "Sketch → Control",
                                "13": "Slot → GP Rain",
                                "14": "Steal → Capture",
                                "15": "Jump Continuously",
                                "16": "Increase Steal Rate",
                                "17": "Raise Magic Damage",
                                "18": "Increase Sketch Rate",
                                "19": "Increase Control Rate",
                                "20": "100% Hit Rate",
                                "21": "Spells Cost 50% Less",
                                "22": "Spells Cost 1 MP",
                                "23": "Vigor +50%"
                            }
                        }
                    },
                    "relicEffects2": {
                        "type": "property",
                        "begin": 12,
                        "mask": "0xFFFF",
                        "flag": true,
                        "invalid": "(this.parent.itemType.value === 0) || (this.parent.itemType.value === 6)",
                        "stringTable": {
                            "string": {
                                "0": "Fight → X-Fight",
                                "1": "Randomly Counterattack",
                                "2": "Randomly Evade",
                                "3": "Gauntlet",
                                "4": "Genji Glove",
                                "5": "Can Equip Heavy Items",
                                "6": "Protect Weak Allies",
                                "8": "Shell When HP is Low",
                                "9": "Safe When HP is Low",
                                "10": "Reflect When HP is Low",
                                "11": "Double Experience",
                                "12": "Double GP",
                                "15": "Make Character Undead"
                            }
                        }
                    }
                }
            }
        },
        "loreDescription": {
            "type": "array",
            "name": "Lore Description",
            "range": "0xED77A0-0xED7A61",
            "array": {
                "length": 24
            },
            "assembly": {
                "type": "text",
                "name": "Description",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xED7A70-0xED7AA0",
                "offset": "0xED77A0"
            }
        },
        "mapAnimationGraphics": {
            "type": "graphics",
            "name": "Map Animation Graphics",
            "range": "0xE60000-0xE68000",
            "format": "snes4bpp",
            "palette": "mapPalettes"
        },
        "mapAnimationGraphicsLayer3": {
            "type": "array",
            "name": "Map Animation Graphics (Layer 3)",
            "range": "0xE6CDC0-0xE6F198",
            "array": {
                "length": 6
            },
            "assembly": {
                "type": "graphics",
                "format": ["snes2bpp", "ff6-lzss"],
                "palette": {
                    "path": "mapPalettes",
                    "range": "0-16"
                },
                "width": 2
            },
            "pointerTable": {
                "range": "0xE6CDA0-0xE6CDC0",
                "pointerLength": 3,
                "offset": "0xE6CDC0"
            }
        },
        "mapAnimationProperties": {
            "type": "array",
            "name": "Map Animation Properties",
            "range": "0xC091FF-0xC0979F",
            "array": {
                "length": 20
            },
            "assembly": {
                "type": "array",
                "assembly": {
                    "type": "data",
                    "assembly": {
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 0,
                            "mask": "0xFFFF"
                        },
                        "frame1": {
                            "type": "property",
                            "name": "Frame 1",
                            "begin": 2,
                            "mask": "0xFFFF"
                        },
                        "frame2": {
                            "type": "property",
                            "name": "Frame 2",
                            "begin": 4,
                            "mask": "0xFFFF"
                        },
                        "frame3": {
                            "type": "property",
                            "name": "Frame 3",
                            "begin": 6,
                            "mask": "0xFFFF"
                        },
                        "frame4": {
                            "type": "property",
                            "name": "Frame 4",
                            "begin": 8,
                            "mask": "0xFFFF"
                        }
                    },
                    "length": 10
                }
            },
            "pointerTable": {
                "range": "0xC091D5-0xC091FF",
                "offset": "0xC091FF"
            },
            "stringTable": {
                "length": 20,
                "default": "Map Animation %i"
            }
        },
        "mapAnimationPropertiesLayer3": {
            "type": "array",
            "name": "Map Animation Properties (Layer 3)",
            "range": "0xC097AD-0xC09825",
            "array": {
                "length": 6
            },
            "assembly": {
                "type": "data",
                "assembly": {
                    "speed": {
                        "type": "property",
                        "name": "Speed",
                        "begin": 0,
                        "mask": "0xFFFF"
                    },
                    "size": {
                        "type": "property",
                        "name": "Size",
                        "begin": 2,
                        "mask": "0xFFFF"
                    },
                    "frame1": {
                        "type": "property",
                        "name": "Frame 1",
                        "begin": 4,
                        "mask": "0xFFFF"
                    },
                    "frame2": {
                        "type": "property",
                        "name": "Frame 2",
                        "begin": 6,
                        "mask": "0xFFFF"
                    },
                    "frame3": {
                        "type": "property",
                        "name": "Frame 3",
                        "begin": 8,
                        "mask": "0xFFFF"
                    },
                    "frame4": {
                        "type": "property",
                        "name": "Frame 4",
                        "begin": 10,
                        "mask": "0xFFFF"
                    },
                    "frame5": {
                        "type": "property",
                        "name": "Frame 5",
                        "begin": 12,
                        "mask": "0xFFFF"
                    },
                    "frame6": {
                        "type": "property",
                        "name": "Frame 6",
                        "begin": 14,
                        "mask": "0xFFFF"
                    },
                    "frame7": {
                        "type": "property",
                        "name": "Frame 7",
                        "begin": 16,
                        "mask": "0xFFFF"
                    },
                    "frame8": {
                        "type": "property",
                        "name": "Frame 8",
                        "begin": 18,
                        "mask": "0xFFFF"
                    }
                }
            },
            "pointerTable": {
                "range": "0xC0979F-0xC097AD",
                "offset": "0xC097AD"
            },
            "stringTable": {
                "length": 6,
                "default": "Map Animation %i (Layer 3)"
            }
        },
        "mapBattleGroups": {
            "type": "array",
            "name": "Map Battle Groups",
            "range": "0xCF5600-0xCF5800",
            "array": {
                "length": 512
            },
            "assembly": {
                "type": "property",
                "name": "Battle Group",
                "length": 1,
                "mask": "0xFF",
                "link": "battleGroupsRandom[%i]",
                "stringTable": "battleGroupsRandom"
            }
        },
        "mapBattleProbability": {
            "type": "array",
            "name": "Map Battle Probability",
            "range": "0xCF5880-0xCF5900",
            "format": "linear2bpp",
            "array": {
                "length": 512
            },
            "assembly": {
                "type": "property",
                "name": "Probability",
                "length": 1,
                "mask": "0x03",
                "stringTable": {
                    "string": {
                        "0": "Normal",
                        "1": "Low",
                        "2": "High",
                        "3": "Very High"
                    }
                }
            }
        },
        "mapColorMath": {
            "type": "array",
            "name": "Map Color Math Properties",
            "range": "0xC0FE00-0xC0FE40",
            "array": {
                "length": 19
            },
            "assembly": {
                "type": "data",
                "assembly": {
                    "half": {
                        "type": "property",
                        "name": "Half Add/Subtract",
                        "begin": 0,
                        "mask": "0x40",
                        "bool": true
                    },
                    "subtract": {
                        "type": "property",
                        "name": "Subtract",
                        "begin": 0,
                        "mask": "0x80",
                        "bool": true
                    },
                    "mathLayers": {
                        "type": "property",
                        "name": "Color Math",
                        "begin": 0,
                        "mask": "0x3F",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Layer 1",
                                "1": "Layer 2",
                                "2": "Layer 3",
                                "3": "Layer 4",
                                "4": "Sprites",
                                "5": "Back Area"
                            }
                        }
                    },
                    "mainscreen": {
                        "type": "property",
                        "name": "Main Screen",
                        "begin": 1,
                        "mask": "0x3F",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Layer 1",
                                "1": "Layer 2",
                                "2": "Layer 3",
                                "3": "Layer 4",
                                "4": "Sprites"
                            }
                        }
                    },
                    "subscreen": {
                        "type": "property",
                        "name": "Sub Screen",
                        "begin": 2,
                        "mask": "0x3F",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Layer 1",
                                "1": "Layer 2",
                                "2": "Layer 3",
                                "3": "Layer 4",
                                "4": "Sprites"
                            }
                        }
                    }
                },
                "length": 3
            },
            "stringTable": {
                "length": 19,
                "default": "Color Math %i",
                "string": {
                    "0": "None",
                    "1": "Layer 3 Half Addition",
                    "4": "Layer 2 Half Addition",
                    "5": "Layer 3 Full Addition",
                    "6": "Cyan's Dream",
                    "7": "Beach (Raft)",
                    "8": "Layer 1 Half Addition",
                    "12": "Back Area Half Addition",
                    "13": "Floating Island Shadow",
                    "14": "Beach (Fish)",
                    "15": "Darill's Tomb Torches",
                    "17": "Burning Building"
                }
            }
        },
        "mapGraphics": {
            "type": "array",
            "name": "Map Graphics",
            "range": "0xDFDB00-0xE5F400",
            "array": {
                "length": 82,
                "max": 128
            },
            "assembly": {
                "type": "graphics",
                "format": "snes4bpp",
                "palette": "mapPalettes"
            },
            "pointerTable": {
                "range": "0xDFDA00-0xDFDB00",
                "pointerLength": 3,
                "offset": "0xDFDB00"
            },
            "stringTable": {
                "length": 82,
                "default": "Map Graphics %i"
            }
        },
        "mapGraphicsLayer3": {
            "type": "array",
            "name": "Map Graphics (Layer 3)",
            "range": "0xE68780-0xE6CD60",
            "array": {
                "length": 18
            },
            "assembly": {
                "type": "data",
                "format": "ff6-lzss",
                "assembly": {
                    "tileset": {
                        "type": "assembly",
                        "length": "0x40"
                    },
                    "graphics": {
                        "type": "assembly",
                        "begin": "0x40",
                        "length": "0x1000",
                        "format": "snes2bpp"
                    }
                }
            },
            "pointerTable": {
                "range": "0xE6CD60-0xE6CDA0",
                "pointerLength": 3,
                "offset": "0xE68780"
            },
            "stringTable": {
                "length": 18,
                "default": "Map Graphics %i (Layer 3)"
            }
        },
        "mapLayouts": {
            "type": "array",
            "name": "Map Layouts",
            "range": "0xD9D1B0-0xDDFFF2",
            "endPointer": true,
            "canRelocate": true,
            "array": {
                "length": 350
            },
            "assembly": {
                "format": "ff6-lzss"
            },
            "pointerTable": {
                "range": "0xD9CD90-0xD9D1AD",
                "pointerLength": 3,
                "offset": "0xD9D1B0",
                "canRelocate": true,
                "reference": [
                    {
                        "begin": "0xC02893",
                        "mask": "0xFFFFFF"
                    }, {
                        "begin": "0xC028A0",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }, {
                        "begin": "0xC028E7",
                        "mask": "0xFFFFFF"
                    }, {
                        "begin": "0xC028F4",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }, {
                        "begin": "0xC0293D",
                        "mask": "0xFFFFFF"
                    }, {
                        "begin": "0xC0294A",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }
                ]
            },
            "reference": [
                {
                    "begin": "0xC02898",
                    "mask": "0xFFFF"
                }, {
                    "begin": "0xC028A4",
                    "mask": "0xFF",
                    "bankByte": true
                }, {
                    "begin": "0xC028EC",
                    "mask": "0xFFFF"
                }, {
                    "begin": "0xC028F8",
                    "mask": "0xFF",
                    "bankByte": true
                }, {
                    "begin": "0xC02942",
                    "mask": "0xFFFF"
                }, {
                    "begin": "0xC0294E",
                    "mask": "0xFF",
                    "bankByte": true
                }
            ],
            "stringTable": {
                "length": 350,
                "default": "Map Layout %i",
                "string": {
                    "0": "None",
                    "1": "Kohlingen: Exterior (BG1)",
                    "2": "Kohlingen: Exterior (BG2)",
                    "3": "Mobliz: Exterior (BG1)",
                    "4": "Mobliz: Exterior (BG2)",
                    "5": "Thamasa: Exterior (BG1)",
                    "6": "Thamasa: Exterior (BG2)",
                    "7": "South Figaro: Exterior (BG1)",
                    "8": "South Figaro: Exterior (BG2)",
                    "9": "Doma Castle: Exterior (BG1)",
                    "10": "Doma Castle: Exterior (BG2)",
                    "11": "Figaro Castle: Exterior (BG1)",
                    "12": "Figaro Castle: Exterior (BG2)",
                    "13": "Albrook: Exterior (BG1)",
                    "14": "Albrook: Exterior (BG2)",
                    "15": "Albrook: Docks (BG1)",
                    "16": "Albrook: Docks (BG2)",
                    "17": "Kohlingen: Interior (BG1)",
                    "18": "Kohlingen: Interior (BG2)",
                    "19": "Figaro Castle: Interior (BG1)",
                    "20": "Figaro Castle: Interior (BG2)",
                    "21": "Sabin's House: Exterior (BG1)",
                    "22": "Sabin's House: Exterior (BG2)",
                    "23": "Cave to South Figaro (BG1)",
                    "24": "Cave to South Figaro (BG2)",
                    "25": "Lete River: Exterior (BG1)",
                    "26": "Lete River: Exterior (BG2)",
                    "27": "Lete River: Exterior (BG3)",
                    "28": "Imperial Camp: On Foot (BG1)",
                    "29": "Imperial Camp: On Foot (BG2)",
                    "30": "Vector: Castle Interior (BG1)",
                    "31": "Vector: Castle Interior (BG2)",
                    "32": "South Figaro: Interior (BG1)",
                    "33": "South Figaro: Interior (BG2)",
                    "34": "Nikeah: Exterior (BG1)",
                    "35": "Nikeah: Exterior (BG2)",
                    "36": "Cutscene (BG1)",
                    "37": "Cutscene (BG2)",
                    "38": "Cave to the Sealed Gate (BG1)",
                    "39": "Cave to the Sealed Gate (BG2)",
                    "40": "Umaro's Cave (BG1)",
                    "41": "Umaro's Cave (BG2)",
                    "42": "Mt. Kolts: Exterior (BG1)",
                    "43": "Mountains Parallax (BG2)",
                    "45": "Mt. Kolts: Interior (BG1)",
                    "46": "Mt. Kolts: Interior (BG2)",
                    "47": "Mt. Kolts: Interior (BG3)",
                    "48": "Solitary Island: Cliff (BG1)",
                    "49": "Solitary Island: Cliff (BG2)",
                    "50": "Albrook: Interior (BG1)",
                    "51": "Albrook: Interior (BG2)",
                    "52": "Nikeah: Interior (BG1)",
                    "53": "Nikeah: Interior (BG2)",
                    "54": "Narshe: South Exterior (BG1)",
                    "55": "Narshe: South Exterior (BG2)",
                    "56": "Maranda: Exterior (BG1)",
                    "57": "Maranda: Exterior (BG2)",
                    "58": "Phoenix Cave: Upstairs (BG1)",
                    "59": "Phoenix Cave: Upstairs (BG2)",
                    "60": "South Figaro: Docks (BG1)",
                    "61": "South Figaro: Docks (BG2)",
                    "62": "Narshe: Hills Maze (BG1)",
                    "63": "Narshe: Hills Maze (BG2)",
                    "64": "Phantom Train: Front Cars (BG1)",
                    "65": "Phantom Train: Seating Cars (BG1)",
                    "66": "Phantom Train: Seating Cars (BG2)",
                    "67": "Phantom Train: Caboose Room and Engine Room (BG1)",
                    "68": "Phantom Train: Caboose Room and Engine Room (BG2)",
                    "69": "South Figaro: Basement (BG1)",
                    "70": "South Figaro: Basement (BG2)",
                    "71": "Phantom Train: Dining Car (BG1)",
                    "72": "Phantom Train: Dining Car (BG2)",
                    "73": "Phantom Train: Rear Cars (BG1)",
                    "74": "Fanatics' Tower: Bottom (BG1)",
                    "75": "Phantom Train: Seating Car with Switch (BG1)",
                    "76": "Phantom Train: Seating Car with Switch (BG2)",
                    "77": "Fanatics' Tower: Bottom (BG2)",
                    "78": "Thamasa: Burning Building (BG1)",
                    "79": "Phantom Train: Hallway (BG1)",
                    "80": "Phantom Train: Hallway (BG2)",
                    "81": "Phantom Train: Caboose (BG1)",
                    "82": "Phantom Train: Caboose (BG2)",
                    "83": "Zozo: Exterior (BG1)",
                    "84": "Zozo: Exterior (BG2)",
                    "85": "Vector: Town Exterior (BG1)",
                    "86": "Vector: Town Exterior (BG2)",
                    "87": "Figaro Castle: Basement (BG1)",
                    "88": "Figaro Castle: Basement (BG2)",
                    "89": "Returner's Hideout (BG1)",
                    "90": "Returner's Hideout (BG2)",
                    "91": "Phantom Train: Small Rooms (BG1)",
                    "92": "Phantom Train: Small Rooms (BG2)",
                    "93": "Mobliz: Interior (BG1)",
                    "94": "Mobliz: Interior (BG2)",
                    "95": "Thamasa: Interior (BG1)",
                    "96": "Thamasa: Interior (BG2)",
                    "97": "Zozo: Exterior (BG3)",
                    "98": "Big Fog Parallax (BG3)",
                    "99": "Floating Island: Exterior (BG1)",
                    "100": "World Parallax (BG2)",
                    "101": "Vector: Town On Fire (BG1)",
                    "102": "Vector: Town On Fire (BG2)",
                    "103": "Opera House: Theater (BG1)",
                    "104": "Opera House: Theater (BG2)",
                    "105": "Figaro Castle: Submerging (BG1)",
                    "106": "Figaro Castle: Submerging (BG2)",
                    "107": "Narshe: Cliffs (BG1)",
                    "108": "Narshe Parallax (BG2)",
                    "109": "Narshe: Mountaintop (BG1)",
                    "110": "Cyan's Dream: Caves (BG2)",
                    "111": "Narshe: Interior (BG1)",
                    "112": "Narshe: Interior (BG2)",
                    "113": "Narshe: North Exterior (BG1)",
                    "114": "Narshe: North Exterior (BG2)",
                    "115": "Magitek Factory: Other Rooms (BG1)",
                    "116": "Magitek Factory: Other Rooms (BG2)",
                    "117": "Zozo: Interior (BG1)",
                    "118": "Zozo: Interior (BG2)",
                    "119": "Doma Castle: Interior (BG1)",
                    "120": "Doma Castle: Interior (BG2)",
                    "121": "Jidoor: Exterior (BG1)",
                    "122": "Jidoor: Exterior (BG2)",
                    "123": "Jidoor: Interior (BG1)",
                    "124": "Jidoor: Interior (BG2)",
                    "125": "Narshe: North Caves (BG1)",
                    "126": "Narshe: North Caves (BG2)",
                    "127": "Cyan's Dream: Cliffs (BG1)",
                    "128": "Cyan's Dream: Cliffs (BG2)",
                    "129": "Mobliz: WoR Exterior (BG1)",
                    "130": "Mobliz: WoR Exterior (BG2)",
                    "131": "Cyan's Dream: Caves (BG3)",
                    "132": "Cyan's Dream: 3rd Train Car (BG1)",
                    "133": "Vector: Town Interior (BG1)",
                    "134": "Vector: Town Interior (BG2)",
                    "135": "Imperial Base: Exterior (BG1)",
                    "136": "Imperial Base: Exterior (BG2)",
                    "137": "Esper World: Exterior (BG1)",
                    "138": "Esper World: Exterior (BG2)",
                    "139": "Esper Mountain: Exterior B (BG1)",
                    "140": "Vector: Town On Fire (BG3)",
                    "141": "Mt. Zozo: Bridge (BG1)",
                    "142": "Gogo's Room (BG1)",
                    "143": "Esper Mountain: Exterior C (BG1)",
                    "144": "Cyan's Dream: Caves (BG1)",
                    "145": "Mt. Zozo: Cliff to Cyan's Cave (BG1)",
                    "146": "Gogo's Room (BG2)",
                    "147": "Mt. Kolts: Vargas' Area (BG1)",
                    "148": "Mt. Kolts: Vargas' Area (BG2)",
                    "149": "Esper Mountain: Statue Cave/Exterior A (BG1)",
                    "150": "Esper Mountain: Statue Cave/Exterior A (BG2)",
                    "151": "Mt. Zozo: Treasure Cliff (BG1)",
                    "152": "Mt. Zozo: Treasure Cliff (BG2)",
                    "153": "Narshe: South Caves (BG1)",
                    "154": "Narshe: South Caves (BG2)",
                    "155": "Phantom Train: Torches Parallax (BG2)",
                    "156": "Phantom Train: Platform (BG2)",
                    "157": "Narshe: South Exterior (BG3)",
                    "158": "Narshe: North Caves (BG3)",
                    "159": "Figaro Castle: Empty Desert (BG1)",
                    "160": "Cave to South Figaro (BG3)",
                    "161": "Magitek Factory: Upper Room (BG1)",
                    "162": "Magitek Factory: Upper Room (BG2)",
                    "163": "Crescent Mountain: Exterior (BG1)",
                    "164": "Crescent Mountain: Exterior (BG2)",
                    "165": "Crescent Mountain: Exterior (BG3)",
                    "166": "Crazy Old Man's House: Exterior (BG1)",
                    "167": "Crazy Old Man's House: Exterior (BG2)",
                    "168": "Crazy Old Man's House: Interior (BG1)",
                    "169": "Crazy Old Man's House: Interior (BG2)",
                    "170": "Baren Falls: Interior (BG1)",
                    "171": "Baren Falls: Interior (BG2)",
                    "172": "Mountaintop (BG1)",
                    "173": "Waterfall Parallax (BG2)",
                    "174": "Baren Falls: Interior (BG3)",
                    "175": "Esper World: Interior (BG1)",
                    "176": "Esper World: Interior (BG2)",
                    "177": "Esper World: Interior (BG3)",
                    "178": "Thamasa: Burning Building (BG2)",
                    "179": "Riverbank (BG1)",
                    "180": "Riverbank (BG2)",
                    "181": "Darill's Tomb: Entrance (BG1)",
                    "182": "Darill's Tomb: Entrance (BG2)",
                    "183": "Airship: Interior (BG1)",
                    "184": "Airship: Interior (BG2)",
                    "185": "Blackjack: Exterior (BG1)",
                    "186": "Clouds Parallax (BG2)",
                    "187": "Falcon: Exterior (BG1)",
                    "188": "Cyan's Dream: 2nd Train Car (BG1)",
                    "189": "Cyan's Dream: 2nd Train Car (BG2)",
                    "190": "Narshe: North Exterior (BG3)",
                    "191": "Cyan's Dream: 3 Stooges (BG1)",
                    "192": "Nikeah: Docks (BG1)",
                    "193": "Nikeah: Docks (BG2)",
                    "194": "Crescent Mountain: Interior (BG1)",
                    "195": "Crescent Mountain: Interior (BG2)",
                    "196": "Baren Falls: Entrance (BG1)",
                    "197": "Baren Falls: Entrance (BG2)",
                    "198": "Phantom Forest: Main Area (BG1)",
                    "199": "Forest Parallax (BG2)",
                    "200": "Forest Parallax (BG3)",
                    "201": "Vector: Lower Castle Exterior (BG1)",
                    "202": "Vector: Lower Castle Exterior (BG2)",
                    "203": "Vector: Upper Castle Exterior (BG1)",
                    "204": "Vector: Upper Castle Exterior (BG2)",
                    "205": "Solitary Island: Beach (BG1)",
                    "206": "Solitary Island: Beach (BG2)",
                    "207": "Solitary Island: Beach (BG3)",
                    "208": "Solitary Island: House Exterior (BG1)",
                    "209": "Solitary Island: House Exterior (BG2)",
                    "210": "Magitek Research Facility: Esper Room (BG1)",
                    "211": "Magitek Research Facility: Esper Room (BG2)",
                    "212": "Phantom Train: Forest Parallax (BG2)",
                    "213": "Colosseum Guy's House: Exterior (BG1)",
                    "214": "Colosseum Guy's House: Exterior (BG2)",
                    "215": "Magitek Factory: Lower Room (BG1)",
                    "216": "Magitek Factory: Lower Room (BG2)",
                    "217": "Cave to the Sealed Gate: Gate (BG1)",
                    "218": "Cave to the Sealed Gate: Gate (BG2)",
                    "219": "Imperial Camp: Magitek Armor (BG1)",
                    "220": "Imperial Camp: Magitek Armor (BG2)",
                    "221": "Kohlingen: Exterior (BG3)",
                    "222": "Narshe: Cliffs (BG3)",
                    "223": "Returner's Hideout (BG3)",
                    "224": "Thamasa: Burning Building (BG3)",
                    "225": "Phantom Forest: Recovery Spring (BG1)",
                    "226": "Phantom Forest: Fork (BG1)",
                    "227": "Phantom Forest: Path to Phantom Train (BG1)",
                    "228": "Serpent Trench: Caves (BG1)",
                    "229": "Serpent Trench: Caves (BG2)",
                    "230": "Serpent Trench: Caves (BG3)",
                    "231": "Opera House: Sky Parallax (BG2)",
                    "232": "Opera House: Castle (BG1)",
                    "233": "Tzen: Exterior (BG1)",
                    "234": "Tzen: Exterior (BG2)",
                    "235": "Narshe: South Caves (BG3)",
                    "236": "Colosseum Guy's House: Exterior (BG3)",
                    "237": "Darill's Tomb (BG1)",
                    "238": "Darill's Tomb (BG2)",
                    "239": "Darill's Tomb (BG3)",
                    "240": "Esper Mountain: Caves (BG1)",
                    "241": "Esper Mountain: Caves (BG2)",
                    "242": "Esper Mountain: Caves (BG3)",
                    "243": "Gogo's Cave (BG1)",
                    "244": "Gogo's Cave (BG2)",
                    "246": "Esper World: Exterior (BG3)",
                    "247": "Magitek Research Facility: Tube Room (BG3)",
                    "248": "Magitek Research Facility: Esper Room (BG3)",
                    "249": "Esper World: Doors Closing (BG1)",
                    "250": "Esper World: Doors Closing (BG3)",
                    "251": "Fanatics' Tower: Exterior (BG1)",
                    "252": "Fanatics' Tower: Exterior (BG2)",
                    "253": "Fanatics' Tower: Top (BG1)",
                    "254": "Cutscene (BG1)",
                    "255": "Cutscene (BG2)",
                    "256": "Magitek Research Facility: Elevator (BG1)",
                    "257": "Magitek Research Facility: Elevator (BG2)",
                    "258": "Jidoor: Floating Island Shadow (BG3)",
                    "259": "Empty (BG1)",
                    "260": "Mobliz: Exterior (BG3)",
                    "261": "Cyan's Dream: 3rd Train Car (BG2)",
                    "262": "Mobliz: WoR Exterior (BG3)",
                    "263": "Mt. Zozo: Interior (BG1)",
                    "264": "Mt. Zozo: Interior (BG2)",
                    "265": "Mt. Zozo: Interior (BG3)",
                    "266": "Ancient Castle: Cave (BG1)",
                    "267": "Ancient Castle: Cave (BG2)",
                    "268": "Ancient Castle: Interior (BG1)",
                    "269": "Ancient Castle: Interior (BG2)",
                    "270": "Ancient Castle: Exterior (BG1)",
                    "271": "Ancient Castle: Exterior (BG2)",
                    "272": "Fanatics' Tower: Top (BG2)",
                    "273": "Fanatics' Tower: Treasure Room (BG1)",
                    "274": "Colosseum (BG1)",
                    "275": "Colosseum (BG2)",
                    "276": "Darill's Tomb: Stairway (BG1)",
                    "277": "Gogo's Room (BG3)",
                    "278": "Cave in the Veldt (BG1)",
                    "279": "Cave in the Veldt (BG2)",
                    "280": "Kefka's Tower: Exterior (BG1)",
                    "281": "Darill's Tomb: Stairway (BG2)",
                    "282": "Kohlingen: Phoenix (BG3)",
                    "283": "Phoenix Cave: Downstairs (BG1)",
                    "284": "Phoenix Cave: Downstairs (BG2)",
                    "285": "Thamasa: House on Fire (BG3)",
                    "286": "Fanatics' Tower: Treasure Room (BG2)",
                    "287": "Fanatics' Tower: Top Treasure Room (BG1)",
                    "288": "Fanatics' Tower: Top Treasure Room (BG2)",
                    "289": "Ebot's Rock: Teleport Caves (BG1)",
                    "290": "Ebot's Rock: Teleport Caves (BG2)",
                    "291": "Ebot's Rock: Other Caves (BG1)",
                    "292": "Ebot's Rock: Other Caves (BG2)",
                    "293": "Chocobo Stable: Exterior (BG1)",
                    "294": "Chocobo Stable: Exterior (BG2)",
                    "295": "Phoenix Cave: Upstairs (BG3)",
                    "296": "Small Fog Parallax (BG3)",
                    "297": "Kefka's Tower: Exterior (BG2)",
                    "299": "Kefka's Tower: Top (BG1)",
                    "300": "Kefka's Tower: Waterfall Parallax (BG2)",
                    "303": "Kefka's Tower: Lower Switch Room (BG1)",
                    "304": "Kefka's Tower: Lower Switch Room (BG2)",
                    "305": "Gogo's Cave (BG3)",
                    "306": "Lete River: Caves (BG1)",
                    "307": "Lete River: Caves (BG2)",
                    "308": "Lete River: Caves (BG3)",
                    "309": "Kefka's Tower: Castle Rooms (BG3)",
                    "310": "Kefka's Tower: Misc. Rooms (BG1)",
                    "311": "Floating Island: Save Point (BG1)",
                    "312": "Floating Island: Save Point (BG2)",
                    "313": "Floating Island: Save Point (BG3)",
                    "314": "Kefka's Tower: Misc. Rooms (BG2)",
                    "315": "Kefka's Tower: Castle Rooms (BG1)",
                    "316": "Kefka's Tower: Castle Rooms (BG2)",
                    "317": "South Figaro: Exterior (BG3)",
                    "318": "Nikeah: Exterior (BG3)",
                    "319": "Albrook: Exterior (BG3)",
                    "320": "Maranda: Exterior (BG3)",
                    "321": "Kefka's Tower: Top Parallax (BG2)",
                    "322": "Kefka's Tower: Factory Rooms (BG3)",
                    "323": "Magitek Research Facility: Broken Tubes (BG3)",
                    "324": "Map 333 (BG3)",
                    "325": "Kefka's Tower: Top (BG2/BG3)",
                    "326": "Cyan's Dream: 3 Stooges (BG2)",
                    "327": "Albrook: Docks (BG3)",
                    "328": "South Figaro: Docks (BG3)",
                    "329": "Nikeah: Docks (BG3)",
                    "330": "Waterfall Parallax (BG3)",
                    "331": "Riverbank (BG3)",
                    "332": "Kefka's Tower: Tube Room (BG1)",
                    "333": "Kefka's Tower: Tube Room (BG2)",
                    "334": "Kefka's Tower: Factory Rooms (BG1)",
                    "335": "Kefka's Tower: Factory Rooms (BG2)",
                    "336": "Esper Mountain: Statue Cave (BG3)",
                    "337": "Kefka's Tower: Lower Switch Room (BG3)",
                    "338": "Magitek Factory: Upper Room (BG3)",
                    "339": "Magitek Factory: Lower Room (BG3)",
                    "340": "Kefka's Tower: Ending Room (BG1)",
                    "341": "Kefka's Tower: Ending Room (BG2)",
                    "342": "Kefka's Tower: Ending Airship Scene (BG1)",
                    "343": "Kefka's Tower: Ending Airship Scene (BG2)",
                    "344": "Kefka's Tower: Ending Room (BG1)",
                    "345": "Kefka's Tower: Ending Room (BG2)",
                    "346": "Kefka's Tower: Ending Airship Scene (BG3)",
                    "347": "Cave in the Veldt (BG3)",
                    "348": "Umaro's Cave (BG3)",
                    "349": "Sky Parallax (BG2)"
                }
            }
        },
        "mapOverlayGraphics": {
            "type": "graphics",
            "name": "Map Overlay Graphics",
            "range": "0xC0E2A0-0xC0EC20",
            "format": "linear1bpp(1)",
            "backColor": true
        },
        "mapOverlayLayout": {
            "type": "assembly",
            "name": "Map Overlay Layout",
            "range": "0xC0EEA0-0xC0F4A0"
        },
        "mapOverlayProperties": {
            "type": "array",
            "name": "Map Overlay Properties",
            "range": "0xC0F500-0xC0FD00",
            "arrayLength": 45,
            "pointerTable": {
                "range": "0xC0F4A0-0xC0F500",
                "offset": "0xC0F500"
            },
            "assembly": {
                "type": "assembly",
                "format": "ff6-lzss"
            },
            "stringTable": {
                "length": 45,
                "default": "Map Overlay %i",
                "string": {
                    "0": "None",
                    "1": "Town Exterior",
                    "2": "Caves",
                    "3": "Town Interior",
                    "4": "Narshe Exterior",
                    "5": "Caves w/ Furniture",
                    "6": "Mountains Exterior 1",
                    "7": "Train Exterior",
                    "8": "Zozo Exterior",
                    "9": "Train Interior",
                    "10": "Imperial Camp",
                    "11": "Forest",
                    "12": "Opera House",
                    "13": "Destroyed Town",
                    "14": "Magitek Factory",
                    "15": "Figaro Castle Exterior",
                    "16": "Doma Castle Exterior",
                    "17": "Village Exterior 1",
                    "18": "Castle Interior",
                    "19": "Castle Basement",
                    "20": "Village Exterior 2",
                    "21": "Floating Island",
                    "22": "Airship Exterior",
                    "23": "Airship Interior",
                    "24": "Imperial Castle Interior",
                    "25": "Vector Exterior",
                    "27": "Mountains Interior",
                    "28": "Imperial Castle Exterior",
                    "29": "Magitek Research Facility",
                    "30": "Darill's Tomb",
                    "31": "Kefka's Tower",
                    "33": "Mountains Exterior 2",
                    "34": "Snowfields"
                }
            }
        },
        "mapPalettes": {
            "type": "array",
            "name": "Map Palettes",
            "range": "0xEDC480-0xEDF480",
            "array": {
                "length": 48,
                "max": 128
            },
            "assembly": {
                "length": 256,
                "format": "bgr555"
            },
            "stringTable": {
                "length": 48,
                "default": "Map Palette %i",
                "string": {
                    "0": "Figaro Castle - Exterior",
                    "1": "Village - Exterior 1 (WoB)",
                    "2": "Doma Castle - Exterior",
                    "3": "Town - Exterior (WoB)",
                    "4": "Docks",
                    "5": "Caves 1",
                    "6": "Castle - Interior",
                    "7": "River",
                    "8": "Imperial Camp",
                    "9": "Mountains - Interior",
                    "10": "Town - Interior 1",
                    "11": "Opera House 1",
                    "12": "Narshe - Exterior",
                    "13": "Snowfields",
                    "14": "Train - Exterior",
                    "15": "Train - Interior",
                    "16": "Zozo - Exterior",
                    "17": "Vector - Exterior",
                    "18": "Caves w/ Furniture",
                    "19": "Floating Island",
                    "20": "Zozo - Interior",
                    "21": "Caves 2",
                    "22": "Village - Exterior 2 (WoR)",
                    "23": "Mountains - Exterior (WoB)",
                    "24": "Magitek Factory",
                    "25": "Blackjack - Exterior",
                    "26": "Airship - Interior",
                    "27": "Cyan's Dream",
                    "28": "Forest",
                    "29": "Imperial Castle - Exterior",
                    "30": "Beach",
                    "31": "Imperial Castle - Interior",
                    "32": "Fire Caves",
                    "33": "Sealed Gate",
                    "34": "Kefka's Tower",
                    "35": "Burning Building",
                    "36": "Town - Interior 2",
                    "37": "Narshe - Interior",
                    "38": "Village - Exterior 2 (WoB)",
                    "39": "Opera House 2",
                    "40": "Darill's Tomb",
                    "41": "Town - Exterior (WoR)",
                    "42": "Fanatics' Tower",
                    "43": "Kefka's Tower - Top",
                    "44": "Solitary Island - Cliff",
                    "45": "Falcon - Exterior",
                    "46": "Darill's Tomb - Stairway",
                    "47": "Mountains - Exterior (WoR)"
                }
            }
        },
        "mapPaletteAnimation": {
            "type": "array",
            "name": "Map Palette Animation",
            "range": "0xC09825-0xC0989D",
            "arrayLength": 10,
            "arrayMax": 21,
            "assembly": {
                "type": "array",
                "length": 12,
                "arrayLength": 2,
                "arrayMin": 2,
                "arrayMax": 2,
                "assembly": {
                    "type": "data",
                    "length": 6,
                    "assembly": {
                        "animationType": {
                            "type": "property",
                            "name": "Type",
                            "begin": 0,
                            "mask": "0xF0",
                            "stringTable": {
                                "string": {
                                    "0": "None",
                                    "1": "Cycle",
                                    "2": "ROM",
                                    "3": "Pulse",
                                    "8": "Disabled"
                                }
                            }
                        },
                        "frameCounter": {
                            "type": "property",
                            "name": "Frame Counter",
                            "begin": 1,
                            "mask": "0xFF"
                        },
                        "colorCounter": {
                            "type": "property",
                            "name": "Color Counter",
                            "begin": 0,
                            "mask": "0x0F"
                        },
                        "firstColor": {
                            "type": "property",
                            "name": "First Color",
                            "begin": 2,
                            "mask": "0xFF"
                        },
                        "numColors": {
                            "type": "property",
                            "name": "Number of Colors",
                            "begin": 3,
                            "mask": "0xFF"
                        },
                        "romOffset": {
                            "type": "property",
                            "name": "ROM Color Offset",
                            "begin": 4,
                            "mask": "0xFFFF"
                        }
                    }
                }
            },
            "stringTable": {
                "length": 10,
                "default": "Map Palette Animation %i",
                "string": {
                    "0": "River",
                    "1": "Lava/Cyan's Dream",
                    "2": "Imperial Camp",
                    "3": "Beach",
                    "5": "Doma Castle",
                    "6": "Waterfall",
                    "7": "Narshe",
                    "8": "Kefka's Tower",
                    "9": "Darill's Tomb"
                }
            }
        },
        "mapParallax": {
            "type": "array",
            "name": "Map Parallax",
            "range": "0xC0FE40-0xC0FEE8",
            "array": {
                "length": 21,
                "max": 32
            },
            "assembly": {
                "type": "data",
                "assembly": {
                    "cat-layer2": "Layer 2",
                    "hSpeedLayer2": {
                        "type": "property",
                        "name": "Horizontal Speed",
                        "begin": 0,
                        "signed": true
                    },
                    "hMultiplierLayer2": {
                        "type": "property",
                        "name": "Horizontal Multiplier",
                        "begin": 4
                    },
                    "vSpeedLayer2": {
                        "type": "property",
                        "name": "Vertical Speed",
                        "begin": 1,
                        "signed": true
                    },
                    "vMultiplierLayer2": {
                        "type": "property",
                        "name": "Vertical Multiplier",
                        "begin": 5
                    },
                    "cat-layer3": "Layer 3",
                    "hSpeedLayer3": {
                        "type": "property",
                        "name": "Horizontal Speed",
                        "begin": 2,
                        "signed": true
                    },
                    "hMultiplierLayer3": {
                        "type": "property",
                        "name": "Horizontal Multiplier",
                        "begin": 6
                    },
                    "vSpeedLayer3": {
                        "type": "property",
                        "name": "Vertical Speed",
                        "begin": 3,
                        "signed": true
                    },
                    "vMultiplierLayer3": {
                        "type": "property",
                        "name": "Vertical Multiplier",
                        "begin": 7
                    }
                },
                "length": 8
            },
            "stringTable": {
                "default": "Parallax %i",
                "length": 21,
                "string": {
                    "0": "None",
                    "1": "Phantom Forest",
                    "2": "Parallax w/ Fog",
                    "3": "Train",
                    "4": "Parallax",
                    "5": "Opera House",
                    "6": "Floating Island",
                    "8": "Airship",
                    "9": "Sealed Gate",
                    "10": "Fog",
                    "11": "Fog (Moving Down)",
                    "12": "Airship (Breaking Apart)",
                    "13": "Slow Fog",
                    "14": "Fanatics' Tower",
                    "15": "Kefka's Tower (Top)",
                    "17": "Waterfall",
                    "18": "Kefka's Tower",
                    "20": "Fog"
                }
            }
        },
        "mapProperties": {
            "type": "array",
            "name": "Map Properties",
            "range": "0xED8F00-0xEDC480",
            "array": {
                "length": 415,
                "max": 510
            },
            "assembly": {
                "type": "data",
                "editor": "FF6Map",
                "assembly": {
                    "title": {
                        "type": "property",
                        "name": "Title",
                        "begin": 0,
                        "stringTable": "mapTitle",
                        "link": "mapTitle[%i]"
                    },
                    "song": {
                        "type": "property",
                        "name": "Song",
                        "begin": 28,
                        "stringTable": "song"
                    },
                    "scriptPointer": {
                        "type": "assembly",
                        "name": "Event",
                        "external": "mapStartupEvents[%i]"
                    },
                    "flags": {
                        "type": "property",
                        "name": "Flags",
                        "begin": 1,
                        "mask": "0xFF",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Enable X-Zone",
                                "1": "Enable Warp",
                                "7": "Show Timer"
                            }
                        }
                    },
                    "category-layer1": "Layer 1",
                    "layout1": {
                        "type": "property",
                        "name": "Tile Layout",
                        "begin": 13,
                        "mask": "0x03FF",
                        "stringTable": "mapLayouts",
                        "link": "mapLayouts[%i]"
                    },
                    "tileset1": {
                        "type": "property",
                        "name": "Tileset",
                        "begin": 11,
                        "mask": "0x01FC",
                        "stringTable": "mapTilesets",
                        "link": "mapTilesets[%i]"
                    },
                    "tileProperties": {
                        "type": "property",
                        "name": "Tile Properties",
                        "begin": 4,
                        "stringTable": "mapTileProperties"
                    },
                    "overlay": {
                        "type": "property",
                        "name": "Sprite Overlay",
                        "begin": 17,
                        "stringTable": "mapOverlayProperties"
                    },
                    "hSize1": {
                        "type": "property",
                        "name": "Layer Width",
                        "begin": 23,
                        "mask": "0xC0",
                        "stringTable": {
                            "string": {
                                "0": "16 Tiles",
                                "1": "32 Tiles",
                                "2": "64 Tiles",
                                "3": "128 Tiles"
                            }
                        }
                    },
                    "vSize1": {
                        "type": "property",
                        "name": "Layer Height",
                        "begin": 23,
                        "mask": "0x30",
                        "stringTable": {
                            "string": {
                                "0": "16 Tiles",
                                "1": "32 Tiles",
                                "2": "64 Tiles"
                            }
                        }
                    },
                    "wavyLayer1": {
                        "type": "property",
                        "name": "Wavy Effect",
                        "begin": 1,
                        "mask": "0x10",
                        "bool": true
                    },
                    "category-layer2": "Layer 2",
                    "layout2": {
                        "type": "property",
                        "name": "Tile Layout",
                        "begin": 14,
                        "mask": "0x0FFC",
                        "stringTable": "mapLayouts",
                        "link": "mapLayouts[%i]"
                    },
                    "tileset2": {
                        "type": "property",
                        "name": "Tileset",
                        "begin": 12,
                        "mask": "0xFE",
                        "stringTable": "mapTilesets",
                        "link": "mapTilesets[%i]"
                    },
                    "hSize2": {
                        "type": "property",
                        "name": "Layer Width",
                        "begin": 23,
                        "mask": "0x0C",
                        "stringTable": "mapProperties.hSize1"
                    },
                    "vSize2": {
                        "type": "property",
                        "name": "Layer Height",
                        "begin": 23,
                        "mask": "0x03",
                        "stringTable": "mapProperties.vSize1"
                    },
                    "hOffset2": {
                        "type": "property",
                        "name": "Horizontal Shift",
                        "begin": 18,
                        "signed": true
                    },
                    "vOffset2": {
                        "type": "property",
                        "name": "Vertical Shift",
                        "begin": 19,
                        "signed": true
                    },
                    "wavyLayer2": {
                        "type": "property",
                        "name": "Wavy Effect",
                        "begin": 1,
                        "mask": "0x08",
                        "bool": true
                    },
                    "category-layer12Graphics": "Layer 1 & 2 Graphics",
                    "gfx1": {
                        "type": "property",
                        "name": "Graphic Set 1",
                        "begin": 7,
                        "mask": "0x7F",
                        "stringTable": "mapGraphics"
                    },
                    "gfx2": {
                        "type": "property",
                        "name": "Graphic Set 2",
                        "begin": 7,
                        "mask": "0x3F80",
                        "stringTable": "mapGraphics"
                    },
                    "gfx3": {
                        "type": "property",
                        "name": "Graphic Set 3",
                        "begin": 8,
                        "mask": "0x1FC0",
                        "stringTable": "mapGraphics"
                    },
                    "gfx4": {
                        "type": "property",
                        "name": "Graphic Set 4",
                        "begin": 9,
                        "mask": "0x0FE0",
                        "stringTable": "mapGraphics"
                    },
                    "animation": {
                        "type": "property",
                        "name": "Animated Tiles",
                        "begin": 27,
                        "mask": "0x1F",
                        "stringTable": "mapAnimationProperties"
                    },
                    "category-layer3": "Layer 3",
                    "layout3": {
                        "type": "property",
                        "name": "Tile Layout",
                        "begin": 15,
                        "mask": "0x3FF0",
                        "stringTable": "mapLayouts",
                        "link": "mapLayouts[%i]"
                    },
                    "gfxLayer3": {
                        "type": "property",
                        "name": "Graphic Set",
                        "begin": 10,
                        "mask": "0x03F0",
                        "stringTable": "mapGraphicsLayer3"
                    },
                    "animationLayer3": {
                        "type": "property",
                        "name": "Animated Tiles",
                        "begin": 27,
                        "mask": "0xE0",
                        "stringTable": "mapAnimationPropertiesLayer3"
                    },
                    "hSize3": {
                        "type": "property",
                        "name": "Layer Width",
                        "begin": 24,
                        "mask": "0xC0",
                        "stringTable": "mapProperties.hSize1"
                    },
                    "vSize3": {
                        "type": "property",
                        "name": "Layer Height",
                        "begin": 24,
                        "mask": "0x30",
                        "stringTable": "mapProperties.vSize1"
                    },
                    "hOffset3": {
                        "type": "property",
                        "name": "Horizontal Shift",
                        "begin": 20,
                        "signed": true
                    },
                    "vOffset3": {
                        "type": "property",
                        "name": "Vertical Shift",
                        "begin": 21,
                        "signed": true
                    },
                    "layer3Priority": {
                        "type": "property",
                        "name": "Layer 3 Priority",
                        "begin": 2,
                        "mask": "0x80",
                        "bool": true
                    },
                    "wavyLayer3": {
                        "type": "property",
                        "name": "Wavy Effect",
                        "begin": 1,
                        "mask": "0x04",
                        "bool": true
                    },
                    "category-palette": "Palette",
                    "palette": {
                        "type": "property",
                        "name": "Palette",
                        "begin": 25,
                        "stringTable": "mapPalettes"
                    },
                    "paletteAnimation": {
                        "type": "property",
                        "name": "Animation",
                        "begin": 26,
                        "link": "mapPaletteAnimation[%i-1]",
                        "stringTable": {
                            "length": 11,
                            "default": "<stringTable.mapPaletteAnimation[%i-1]>",
                            "string": {
                                "0": "None"
                            }
                        }
                    },
                    "colorMath": {
                        "type": "property",
                        "name": "Color Math",
                        "begin": 32,
                        "stringTable": "mapColorMath",
                        "link": "mapColorMath[%i]"
                    },
                    "category-windowMask": "Window Mask",
                    "windowMask": {
                        "type": "property",
                        "name": "Window Mask",
                        "begin": 6,
                        "mask": "0x03",
                        "stringTable": {
                            "string": {
                                "0": "Default",
                                "1": "Imperial Camp (Flashlight)",
                                "2": "Ebot's Rock (Flashlight)",
                                "3": "Kefka's Tower (Spotlights)"
                            }
                        }
                    },
                    "spotlights": {
                        "type": "property",
                        "name": "Enable Spotlights",
                        "begin": 1,
                        "mask": "0x20",
                        "bool": true
                    },
                    "category-scrolling": "Scrolling",
                    "parallax": {
                        "type": "property",
                        "name": "Parallax",
                        "begin": 22,
                        "stringTable": "mapParallax",
                        "link": "mapParallax[%i]"
                    },
                    "width": {
                        "type": "property",
                        "name": "Map Width",
                        "begin": 30,
                        "min": 1,
                        "special": {
                            "0": "Wrap Around"
                        }
                    },
                    "height": {
                        "type": "property",
                        "name": "Map Height",
                        "begin": 31,
                        "min": 1,
                        "special": {
                            "0": "Wrap Around"
                        }
                    },
                    "category-battle": "Battle",
                    "enableBattles": {
                        "type": "property",
                        "name": "Enable Battles",
                        "begin": 5,
                        "mask": "0x80",
                        "bool": true
                    },
                    "battleGroup": {
                        "type": "assembly",
                        "name": "Battle Group",
                        "external": "mapBattleGroups[%i]"
                    },
                    "battleBackground": {
                        "type": "property",
                        "name": "Background",
                        "begin": 2,
                        "mask": "0x7F",
                        "stringTable": "battleBackground"
                    },
                    "battleProbability": {
                        "type": "assembly",
                        "name": "Probability",
                        "external": "mapBattleProbability[%i]"
                    },
                    "category-unknown": "Unknown",
                    "unknown6_7": {
                        "type": "property",
                        "name": "Unknown 6.7",
                        "begin": 6,
                        "mask": "0x80",
                        "bool": true
                    },
                    "unknown24_0": {
                        "type": "property",
                        "name": "Unknown 24.0",
                        "begin": 24,
                        "mask": "0x01",
                        "bool": true
                    },
                    "unknown24_1": {
                        "type": "property",
                        "name": "Unknown 24.1",
                        "begin": 24,
                        "mask": "0x02",
                        "bool": true
                    },
                    "unknown24_2": {
                        "type": "property",
                        "name": "Unknown 24.2",
                        "begin": 24,
                        "mask": "0x04",
                        "bool": true
                    }
                },
                "length": 33
            },
            "stringTable": {
                "default": "Map %i",
                "length": 415,
                "string": {
					"0x0000": "World of Balance",
					"0x0001": "World of Ruin",
					"0x0002": "Serpent Trench",
					"0x0003": "Empty",
					"0x0004": "Empty (Shadow's Dream)",
					"0x0005": "Empty (Mog Narration)",
					"0x0006": "Blackjack: Exterior",
					"0x0007": "Blackjack: Interior",
					"0x0008": "Blackjack: Parlor",
					"0x0009": "Blank Map (3 Scenarios)",
					"0x000A": "Blackjack: Exterior (Cutscenes)",
					"0x000B": "Falcon: Exterior",
					"0x000C": "Falcon: Interior",
					"0x000D": "Blackjack: Exterior (Breaking Apart)",
					"0x000E": "Chocobo Stable: Exterior (WoB)",
					"0x000F": "Chocobo Stable: Interior",
					"0x0010": "Chocobo Stable: Exterior (WoR)",
					"0x0011": "Falcon: Exterior (Cutscenes)",
					"0x0012": "Narshe: Mountaintop (Intro)",
					"0x0013": "Narshe: South Exterior (Intro)",
					"0x0014": "Narshe: South Exterior (WoB)",
					"0x0015": "Narshe: North Exterior (WoB)",
					"0x0016": "Narshe: Hills Maze (WoB)",
					"0x0017": "Narshe: Cliffs (WoB)",
					"0x0018": "Narshe: Weapon Shop",
					"0x0019": "Narshe: Armor Shop",
					"0x001A": "Narshe: Item Shop",
					"0x001B": "Narshe: Relic Shop",
					"0x001C": "Narshe: Inn",
					"0x001D": "Narshe: Cafe",
					"0x001E": "Narshe: Other Rooms",
					"0x001F": "Narshe: Chocobo Stable",
					"0x0020": "Narshe: South Exterior (WoR)",
					"0x0021": "Narshe: North Exterior (WoR)",
					"0x0022": "Narshe: Hills Maze (WoR)",
					"0x0023": "Narshe: Cliffs (WoR)",
					"0x0024": "Narshe: North Caves (WoR)",
					"0x0025": "Narshe: 3-Party Cave (WoR)",
					"0x0026": "Narshe: South Caves (WoR)",
					"0x0027": "Narshe: North Exterior (Intro)",
					"0x0028": "Narshe: Caves (Unused)",
					"0x0029": "Narshe: Whelk Cave (Intro)",
					"0x002A": "Narshe: Tritoch's Cave (Intro)",
					"0x002B": "Narshe: Tritoch's Cave (WoB)",
					"0x002C": "Narshe: Moogle Cave (WoR)",
					"0x002F": "South Figaro: Exterior (Sneaking Out)",
					"0x0030": "Narshe: Cave to Checkpoint (WoB)",
					"0x0031": "Narshe: Checkpoint Cave (WoB)",
					"0x0032": "Narshe: South Caves (WoB)",
					"0x0033": "Narshe: 3-Party Cave (WoB)",
					"0x0034": "Narshe: Moogle Cave (WoB)",
					"0x0035": "Cave to South Figaro: Siegfried's Tunnel",
					"0x0036": "Figaro Castle: Submerging",
					"0x0037": "Figaro Castle: Exterior",
					"0x0038": "Figaro Castle: Empty Desert",
					"0x0039": "Figaro Castle: West Tower",
					"0x003A": "Figaro Castle: Throne Room",
					"0x003B": "Figaro Castle: Other Rooms",
					"0x003C": "Figaro Castle: Library",
					"0x003D": "Figaro Castle: Switch Room and Prison",
					"0x003E": "Figaro Castle: Upper Basement",
					"0x003F": "Figaro Castle: Lower Basement",
					"0x0040": "Figaro Castle: Engine Room",
					"0x0041": "Figaro Castle: Treasury",
					"0x0042": "Figaro Castle: Basement Treasure Room",
					"0x0043": "Figaro Castle: Night Time",
					"0x0044": "Cave to South Figaro (WoR)",
					"0x0045": "Cave to South Figaro (Locke/Celes)",
					"0x0046": "Cave to South Figaro: Turtle Room (Locke/Celes)",
					"0x0047": "Cave to South Figaro: Outside",
					"0x0048": "Cave to South Figaro (Terra/Locke/Edgar)",
					"0x0049": "Cave to South Figaro: Turtle Room (Terra/Locke/Edgar)",
					"0x004A": "South Figaro: Exterior (WoR)",
					"0x004B": "South Figaro: Exterior (WoB)",
					"0x004C": "South Figaro: Inn/Relic Shop",
					"0x004D": "South Figaro: Arsenal",
					"0x004E": "South Figaro: Cafe",
					"0x004F": "South Figaro: Unused",
					"0x0050": "South Figaro: Chocobo Stable",
					"0x0051": "South Figaro: Rich Man's House",
					"0x0052": "South Figaro: Interior (Unused)",
					"0x0053": "South Figaro: Basement",
					"0x0054": "South Figaro: Clock Room",
					"0x0055": "South Figaro: Item Shop",
					"0x0056": "South Figaro: Other Rooms",
					"0x0057": "South Figaro: Escape Tunnel",
					"0x0058": "South Figaro: Basement Save Point",
					"0x0059": "South Figaro: Basement Treasure Rooms",
					"0x005A": "Cave to South Figaro: Turtle Room (WoR)",
					"0x005B": "South Figaro: Docks",
					"0x005C": "South Figaro: Cave Behind Turtle",
					"0x005D": "Sabin's House: Exterior",
					"0x005E": "Sabin's House: Interior",
					"0x005F": "Mt. Kolts: Entrance",
					"0x0060": "Mt. Kolts: Lower Exterior",
					"0x0061": "Mt. Kolts: Upper Exterior",
					"0x0062": "Mt. Kolts: Vargas' Area",
					"0x0063": "Mt. Kolts: Unused",
					"0x0064": "Mt. Kolts: Interior",
					"0x0065": "Mt. Kolts: Exit",
					"0x0066": "Mt. Kolts: Back Side Exterior",
					"0x0067": "Mt. Kolts: Save Point",
					"0x0068": "Narshe: Beginner's House (main room)",
					"0x0069": "Narshe: Beginner's House (left room)",
					"0x006A": "Narshe: Beginner's House (middle room)",
					"0x006B": "Narshe: Beginner's House (right room)",
					"0x006C": "Returner's Hideout: Entrance",
					"0x006D": "Returner's Hideout: Main Room",
					"0x006E": "Returner's Hideout: Other Rooms",
					"0x006F": "Returner's Hideout: Inn",
					"0x0070": "Returner's Hideout: Tunnel to Lete River",
					"0x0071": "Lete River: Exterior",
					"0x0072": "Lete River: Caves",
					"0x0073": "Crazy Old Man's House: Exterior (WoB)",
					"0x0074": "Crazy Old Man's House: Interior",
					"0x0075": "Imperial Camp: On Foot",
					"0x0077": "Imperial Camp: Magitek Armor",
					"0x0078": "Doma Castle: Exterior (Imperial Siege)",
					"0x0079": "Doma Castle: Exterior (Poisoning)",
					"0x007B": "Doma Castle: Interior",
					"0x007C": "Doma Castle: Cyan's Room",
					"0x007D": "Doma Castle: Exterior (Cyan's Dream)",
					"0x007E": "Doma Castle: Interior (Cyan's Dream)",
					"0x007F": "Duncan's House: Exterior",
					"0x0080": "Duncan's House: Interior",
					"0x0081": "Mountaintop (Ending)",
					"0x0082": "Phantom Forest (Ending)",
					"0x0083": "Crazy Old Man's House: Exterior (WoR)",
					"0x0084": "Phantom Forest: Main Area",
					"0x0085": "Phantom Forest: Recovery Spring",
					"0x0086": "Phantom Forest: Fork",
					"0x0087": "Phantom Forest: Path to Phantom Train",
					"0x0088": "Mountain Waterfall (Ending)",
					"0x0089": "Phantom Train: Platform (After Train Leaves)",
					"0x008A": "Phantom Train: Platform (Party Gets Off)",
					"0x008B": "Phantom Train: Platform (Dead People Get On)",
					"0x008C": "Phantom Train: Platform (Party Gets On)",
					"0x008D": "Phantom Train: Exterior (Front Cars)",
					"0x008E": "Phantom Train: Exterior (Rear Cars)",
					"0x008F": "Phantom Train: Exterior (Cyan's Dream)",
					"0x0090": "Phantom Train: 2nd Train Car (Cyan's Dream)",
					"0x0091": "Phantom Train: Seating Cars",
					"0x0092": "Phantom Train: Caboose Room and Engine Room",
					"0x0093": "Phantom Train: Dining Car",
					"0x0094": "Mobliz: Exterior (Light of Judgement)",
					"0x0095": "Phantom Train: Seating Car with Switch",
					"0x0096": "Mobliz: Cave 2",
					"0x0097": "Phantom Train: Hallway",
					"0x0098": "Phantom Train: Caboose",
					"0x0099": "Phantom Train: Small Rooms",
					"0x009A": "Mobliz: Cave 1",
					"0x009B": "Baren Falls: Interior",
					"0x009C": "Baren Falls: Exterior",
					"0x009D": "Mobliz: Exterior (WoB)",
					"0x009E": "Mobliz: Exterior (WoR)",
					"0x009F": "Baren Falls: Riverbank",
					"0x00A0": "Mobliz: Inn",
					"0x00A1": "Mobliz: Arsenal",
					"0x00A2": "Mobliz: Relic Shop",
					"0x00A3": "Mobliz: Mail House",
					"0x00A4": "Mobliz: Item Shop",
					"0x00A5": "Mobliz: Other Rooms",
					"0x00A6": "Baren Falls: Entrance",
					"0x00A7": "Crescent Mountain: Interior",
					"0x00A8": "Crescent Mountain: Exterior",
					"0x00A9": "Nikeah: Exterior",
					"0x00AA": "Baren Falls: Riverbank (Shadow's Dream)",
					"0x00AB": "Nikeah: Inn",
					"0x00AC": "Nikeah: Cafe",
					"0x00AD": "Nikeah: Chocobo Stable",
					"0x00AE": "Phantom Forest (Shadow's Dream)",
					"0x00AF": "Serpent Trench: Caves",
					"0x00B0": "Mt. Zozo: Bridge",
					"0x00B1": "Mt. Zozo: Treasure Cliff",
					"0x00B2": "Mt. Zozo: Cliff to Cyan's Cave",
					"0x00B3": "Mt. Zozo: Interior",
					"0x00B4": "Mt. Zozo: Cyan's Cave",
					"0x00B5": "Mt. Zozo: Cyan's Cliff",
					"0x00B6": "Thamasa: Exterior (Shadow's Dream)",
					"0x00B7": "Mobliz: Cave 1 (Ending)",
					"0x00B8": "Mobliz: Exterior (Ending)",
					"0x00B9": "Colosseum Guy's House: Exterior",
					"0x00BA": "Colosseum Guy's House: Interior",
					"0x00BB": "Nikeah: Docks",
					"0x00BC": "Kohlingen: Exterior (WoB)",
					"0x00BD": "Kohlingen: Exterior (WoR)",
					"0x00BE": "Lete River: Exterior (Ending)",
					"0x00BF": "Kohlingen: Inn",
					"0x00C0": "Kohlingen: Unused",
					"0x00C1": "Kohlingen: Unused",
					"0x00C2": "Kohlingen: General Store",
					"0x00C3": "Kohlingen: Crazy Man's House",
					"0x00C4": "Maranda: Lola's House",
					"0x00C5": "Kohlingen: Rachel's House",
					"0x00C6": "Jidoor: Exterior",
					"0x00C8": "Jidoor: Auction House",
					"0x00C9": "Jidoor: Item Shop",
					"0x00CA": "Jidoor: Relic Shop",
					"0x00CB": "Jidoor: Armor Shop",
					"0x00CC": "Jidoor: Weapon Shop",
					"0x00CD": "Jidoor: Chocobo Stable",
					"0x00CE": "Jidoor: Inn",
					"0x00CF": "Jidoor: Owzer's Basement",
					"0x00D0": "Jidoor: Chadarnook's Room",
					"0x00D1": "Jidoor: Owzer's House",
					"0x00D2": "Doma Castle: Exterior (Ending)",
					"0x00D3": "Narshe: Cliffs (Ending)",
					"0x00D4": "Gogo's Cave: Bridges (Ending)",
					"0x00D5": "Crescent Mountain: Interior (Ending)",
					"0x00D6": "Mt. Zozo: Bridge (Setzer/Darill Flashback)",
					"0x00D7": "Blank (Falcon Flying Off)",
					"0x00D8": "Esper World: Doors Closing",
					"0x00D9": "Esper World: Exterior",
					"0x00DA": "Esper World: Cave to Exit",
					"0x00DB": "Esper World: Interior",
					"0x00DC": "South Figaro: Docks (Unused)",
					"0x00DD": "Zozo: Exterior",
					"0x00E1": "Zozo: Interior",
					"0x00E2": "Zozo: Ramuh's Room",
					"0x00E7": "Opera House: Theater (WoR)",
					"0x00E8": "Opera House: Backstage (Before Opera)",
					"0x00E9": "Opera House: Stage (Fight Scene)",
					"0x00EA": "Opera House: Stage (Intro)",
					"0x00EB": "Opera House: Stage (Catwalk)",
					"0x00EC": "Opera House: Castle",
					"0x00ED": "Opera House: Lobby",
					"0x00EE": "Opera House: Backstage (During Opera)",
					"0x00EF": "Opera House: Stage (Lights Out)",
					"0x00F0": "Vector: Town Exterior (After Train Ride)",
					"0x00F1": "Vector: Upper Castle Exterior (Cranes)",
					"0x00F2": "Vector: Town Exterior",
					"0x00F3": "Vector: Lower Castle Exterior",
					"0x00F4": "Vector: Upper Castle Exterior",
					"0x00F5": "Vector: Inn",
					"0x00F6": "Vector: Weapon Shop",
					"0x00F7": "Vector: Cafe",
					"0x00F8": "Vector: Armor Shop",
					"0x00F9": "Vector: Tiny House",
					"0x00FA": "Vector: Castle Interior",
					"0x00FB": "Vector: Castle Dining Room",
					"0x00FC": "Vector: Castle Barracks",
					"0x00FD": "Vector: Town Exterior (On Fire)",
					"0x0106": "Magitek Factory: Upper Room",
					"0x0107": "Magitek Factory: Lower Room",
					"0x0108": "Magitek Factory: Garbage Room",
					"0x0109": "Magitek Factory: Unused",
					"0x010A": "Magitek Research Facility: Elevator",
					"0x010B": "Kefka's Tower: Stairway (Unused)",
					"0x010C": "Magitek Research Facility: Tube Room (Ending)",
					"0x010D": "Magitek Research Facility: Stairway",
					"0x010E": "Magitek Factory: Save Point",
					"0x010F": "Magitek Research Facility: Tube Room",
					"0x0110": "Magitek Research Facility: Train Room",
					"0x0111": "Magitek Research Facility: Number 024's Room",
					"0x0112": "Magitek Research Facility: Esper Room",
					"0x0113": "Unused",
					"0x0114": "Gogo's Cave: First 3 Rooms",
					"0x0115": "Gogo's Cave: Short Tunnel",
					"0x0116": "Gogo's Cave: Gogo's Room",
					"0x0117": "Gogo's Cave: Save Point",
					"0x0118": "Gogo's Cave: Last 2 Rooms",
					"0x0119": "Umaro's Cave: Other Caves",
					"0x011A": "Umaro's Cave: Second Cave",
					"0x011B": "Umaro's Cave: Umaro's Room",
					"0x011C": "Maranda: Exterior",
					"0x011D": "Doma Castle: Exterior (WoB)",
					"0x011F": "Kefka's Tower: After Guardian",
					"0x0120": "Maranda: Inn",
					"0x0121": "Maranda: Weapon Shop",
					"0x0122": "Maranda: Armor Shop",
					"0x0123": "Kefka's Tower: Guardian's Room",
					"0x0124": "Kefka's Tower: Small Switch Room",
					"0x0125": "Kefka's Tower",
					"0x0126": "Kefka's Tower: Before Gold Dragon",
					"0x0127": "Kefka's Tower: Before Tube Room",
					"0x0128": "Kefka's Tower: Inferno's Room (Upstairs)",
					"0x0129": "Darill's Tomb: Entrance",
					"0x012A": "Darill's Tomb: First Room",
					"0x012B": "Darill's Tomb: Upstairs",
					"0x012C": "Darill's Tomb: Downstairs",
					"0x012D": "Darill's Tomb: Stairway",
					"0x012E": "Thamasa: Exterior (Ending)",
					"0x012F": "Kefka's Tower",
					"0x0130": "Kefka's Tower: Fixed Dice Room",
					"0x0131": "Tzen: Exterior (WoR)",
					"0x0132": "Tzen: Exterior (WoB)",
					"0x0133": "Tzen: Item Shop",
					"0x0134": "Tzen: Inn",
					"0x0135": "Tzen: Weapon Shop",
					"0x0136": "Tzen: Armor Shop",
					"0x0137": "Tzen: Collapsing House",
					"0x0138": "Tzen: Relic Shop",
					"0x0139": "Phoenix Cave: Downstairs",
					"0x013A": "Phoenix Cave: Downstairs (?)",
					"0x013B": "Phoenix Cave: Upstairs",
					"0x013C": "Phoenix Cave: Upstairs (?)",
					"0x013D": "Cyan's Dream: 3 Stooges",
					"0x013E": "Phoenix Cave: Entrance",
					"0x013F": "Cyan's Dream: Cliffs",
					"0x0140": "Cyan's Dream: Caves",
					"0x0141": "Phantom Train: Third Train Car (Cyan's Dream)",
					"0x0142": "Phantom Train: First Train Car (Cyan's Dream)",
					"0x0143": "Albrook: Exterior (WoB)",
					"0x0144": "Albrook: Exterior (WoR)",
					"0x0145": "Albrook: Inn",
					"0x0146": "Albrook: Weapon Shop",
					"0x0147": "Albrook: Armor Shop",
					"0x0148": "Albrook: Item Shop",
					"0x0149": "Kefka's Tower",
					"0x014A": "Albrook: Cafe/Relic Shop",
					"0x014B": "Kefka's Tower: Atma's Room",
					"0x014C": "Albrook: Docks",
					"0x014E": "Kefka's Tower: Exterior",
					"0x014F": "Kefka's Tower: Gold Dragon's Room",
					"0x0150": "Kefka's Tower",
					"0x0151": "Kefka's Tower: Lower Switch Room",
					"0x0152": "Kefka's Tower: Minerva Room and Tack Star Room",
					"0x0153": "Kefka's Tower",
					"0x0154": "Thamasa: Exterior (WoB)",
					"0x0155": "Thamasa: Exterior (Kefka Attack)",
					"0x0156": "Thamasa: Exterior (Baram & Clyde)",
					"0x0157": "Thamasa: Exterior (WoB, First Time)",
					"0x0158": "Thamasa: Exterior (WoR)",
					"0x0159": "Thamasa: Armory",
					"0x015A": "Thamasa: Inn",
					"0x015B": "Thamasa: Item Shop",
					"0x015C": "Thamasa: Elder's House",
					"0x015D": "Thamasa: Strago & Relm's House",
					"0x015E": "Thamasa: Relic Shop",
					"0x015F": "Thamasa: Burning Building",
					"0x0160": "Kefka's Tower: Tube Room",
					"0x0161": "Cave in the Veldt",
					"0x0162": "Kefka's Tower: Goddess/Skull Dragon Rooms",
					"0x0163": "Kefka's Tower: Doom/Upper Switch Rooms",
					"0x0164": "Kefka's Tower: Top",
					"0x0165": "Kefka's Tower: Top",
					"0x0166": "Floating Island: Save Point",
					"0x0167": "Fanatics' Tower: 2nd Level",
					"0x0168": "Fanatics' Tower: 3rd Level",
					"0x0169": "Fanatics' Tower: 4th Level",
					"0x016A": "Fanatics' Tower: Bottom",
					"0x016B": "Fanatics' Tower: 1st Level",
					"0x016C": "Fanatics' Tower: Top",
					"0x016D": "Fanatics' Tower: 1st Treasure Room",
					"0x016E": "Fanatics' Tower: Top Treasure Room",
					"0x016F": "Fanatics' Tower: 2nd Treasure Room",
					"0x0170": "Fanatics' Tower: 3rd Treasure Room",
					"0x0171": "Fanatics' Tower: 4th Treasure Room",
					"0x0172": "Fanatics' Tower: Secret Treasure Room",
					"0x0173": "Esper Mountain: Statue Cave",
					"0x0174": "Esper Mountain: Exterior A",
					"0x0175": "Esper Mountain: Exterior B",
					"0x0176": "Esper Mountain: Exterior C",
					"0x0177": "Esper Mountain: Caves",
					"0x0178": "Floating Island: Exterior (Start of Collapse)",
					"0x0179": "Imperial Base: Exterior",
					"0x017A": "Imperial Base: Interior",
					"0x017B": "Imperial Base: Unused",
					"0x017C": "Floating Island: Parallax Only",
					"0x017E": "Cave to the Sealed Gate: 1st Cave",
					"0x017F": "Cave to the Sealed Gate: 2nd Cave",
					"0x0180": "Cave to the Sealed Gate: Other Caves",
					"0x0181": "Cave to the Sealed Gate: Lava Puzzle",
					"0x0182": "Cave to the Sealed Gate: Save Point",
					"0x0183": "Cave to the Sealed Gate (Unused)",
					"0x0184": "Cave to the Sealed Gate (Unused)",
					"0x0187": "Cave to the Sealed Gate: Gate",
					"0x0188": "Jidoor: Exterior (Floating Island Shadow)",
					"0x0189": "Floating Island: Exterior (Collapsing)",
					"0x018A": "Floating Island: Exterior",
					"0x018C": "Solitary Island: House Exterior",
					"0x018D": "Solitary Island: House Interior",
					"0x018E": "Solitary Island: Beach (Fish)",
					"0x018F": "Solitary Island: Cliff",
					"0x0190": "Solitary Island: Beach (Raft)",
					"0x0191": "Ancient Castle: Cave",
					"0x0192": "Ancient Castle: Save Point Cave",
					"0x0193": "Solitary Island: Beach",
					"0x0194": "Ebot's Rock: Teleport Caves",
					"0x0195": "Ebot's Rock: Other Caves",
					"0x0196": "Ancient Castle: Interior",
					"0x0197": "Ancient Castle: Exterior",
					"0x0198": "Ancient Castle: Library/Basement",
					"0x0199": "Kefka's Tower: Tube Room",
					"0x019A": "Kefka's Tower: Inferno's Room",
					"0x019B": "Kefka's Tower: Final Switch Room",
					"0x019C": "Kefka's Tower: Behind Poltergeist",
					"0x019D": "Colosseum",
                    "0x01FE": "Previous Map",
                    "0x01FF": "Parent Map"
                }
            }
        },
        "mapSpriteGraphics": {
            "type": "array",
            "name": "Map Sprite Graphics",
            "range": "0xD50000-0xD83000",
            "stringTable": "mapSprites",
            "arrayLength": 165,
            "assembly": {
                "type": "graphics",
                "format": "snes4bpp",
                "palette": ["mapSpritePalettes", "battleCharacterPalette"],
                "width": 16,
                "height": 21,
                "spriteSheet": [
                    {
                        "name": "Character (0-21)",
                        "width": 16,
                        "height": 18,
                        "tilemap": [
                                0,  1,   0,  1,   0,  1,  12, 13,  12, 13,  12, 13,  24, 25,  30, 31,
                                2,  3,   6,  7,  10, 11,  14, 15,  18, 19,  22, 23,  26, 27,  32, 33,
                                4,  5,   8,  9,  "0x10000005", "0x10000004", 16,17, 20,21, "0x10000011","0x10000010", 28,29, 34,35,

                               24, 25,  56, 57,  62, 63,  68, 69,  30, 31,  24, 25,  24, 46,  30, 51,
                               36, 37,  58, 59,  64, 65,  70, 71,  40, 41,  44, 27,  47, 48,  52, 53,
                               38, 39,  60, 61,  66, 67,  72, 73,  42, 43,  45, 29,  49, 50,  54, 55,

                               74, 75,  74, 75,  81, 82,   0,  1,   0,  1, 150, 31, 106,107, 112,113,
                               76, 77,  80, 77,  83, 84,  93, 94,   6, 95, 151, 33, 108,109, 114,115,
                               78, 79,  78, 79,  85, 86,   8,  9,   8,  9,  34, 35, 110,111, 116,117,

                              118,119, 106,  1, 130,  1,  12,113,  12,132,  96, 97, 102,103, 124,125,
                              120,121, 108,  7, 131,  7,  18,115,  18,133,  98, 99, 104,105, 126,127,
                              122,123, 110,  9, 110,  9,  20,117,  20,117, 100,101, 100,101, 128,129,

                              134,135, 138,139, 142,143, 146,147, 163,164, 163,164, 157,158, 165,166,
                              136,137, 140,141, 144,145, 148,149, 152,153, 156,153, 159,160, 167,168,
                                8,  9,  20, 21,  34, 35,   8,  9, 154,155, 154,155, 161,162, 169,170,

                              177,178, 171,172,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                              179,180, 173,174,  87, 88, 89, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                              175,176, 175,176,  90, 91, 92, -1, -1, -1, -1, -1, -1, -1, -1, -1
                        ]
                    }, {
                        "name": "NPC (22-62)",
                        "width": 16,
                        "height": 6,
                        "tilemap": [
                            0,  1,   0,  1,   0,  1,  12, 13,  12, 13,  12, 13,  24, 25,  30, 31,
                            2,  3,   6,  7,  10, 11,  14, 15,  18, 19,  22, 23,  26, 27,  32, 33,
                            4,  5,   8,  9,  "0x10000005", "0x10000004", 16,17, 20,21, "0x10000011","0x10000010", 28,29, 34,35,

                           24, 25,  49, 50,  40,  1,  43,  1,  45, 46,  40, 41,  46, 47,  52, 53,
                           36, 37,  51, 52,  41,  7,  44,  7,  47, 48,  42, 43,  48, 49,  54, 55,
                           38, 39,  53, 54,  42,  9,  42,  9,   8,  9,  44, 45,  50, 51,  50, 51
                        ]
                    }, {
                        "name": "Ramuh (63)",
                        "width": 6,
                        "height": 3,
                        "tilemap": [
                            0,1, 6,1, 9,10,
                            2,3, 7,3, 2,3,
                            4,5, 8,5, 4,5
                        ]
                    }, {
                        "name": "Static NPC (65-86)",
                        "width": 2,
                        "height": 3,
                        "tilemap": [
                            0,1,
                            2,3,
                            4,5
                        ]
                    }, {
                        "name": "Animated Sprite (87-115)",
                        "width": 8,
                        "height": 3,
                        "tilemap": [
                            0,  0,   0,  0,   0,  0,   0,  0,
                            1,  2,   5,  6,   9, 10,  13, 14,
                            3,  4,   7,  8,  11, 12,  15, 16
                        ]
                    }, {
                        "name": "Static Sprite (16×16)",
                        "width": 4,
                        "height": 2,
                        "tilemap": [
                             0, 1,  4, 5,
                             2, 3,  6, 7
                        ]
                    }, {
                        "name": "Static Sprite (32×32)",
                        "width": 4,
                        "height": 4,
                        "tilemap": [
                             0, 1, 2, 3,
                             4, 5, 6, 7,
                             8, 9,10,11,
                            12,13,14,15
                        ]
                    }
                ]
            },
            "pointerObject": {
                "arrayPath": "mapSpritePointers",
                "pointerPath": "pointer"
            }
        },
        "mapSpriteLayouts": {
            "type": "array",
            "name": "Map Sprite Layouts",
            "range": "0xC0CE3A-0xC0D0F2",
            "array": {
                "length": 58
            },
            "assembly": {
                "type": "data",
                "length": 12,
                "assembly": {
                    "tile1": {
                        "type": "property",
                        "name": "Tile 1 Offset",
                        "begin": 0,
                        "mask": "0xFFFF"
                    },
                    "tile2": {
                        "type": "property",
                        "name": "Tile 2 Offset",
                        "begin": 2,
                        "mask": "0xFFFF"
                    },
                    "tile3": {
                        "type": "property",
                        "name": "Tile 3 Offset",
                        "begin": 4,
                        "mask": "0xFFFF"
                    },
                    "tile4": {
                        "type": "property",
                        "name": "Tile 4 Offset",
                        "begin": 6,
                        "mask": "0xFFFF"
                    },
                    "tile5": {
                        "type": "property",
                        "name": "Tile 5 Offset",
                        "begin": 8,
                        "mask": "0xFFFF"
                    },
                    "tile6": {
                        "type": "property",
                        "name": "Tile 6 Offset",
                        "begin": 10,
                        "mask": "0xFFFF"
                    }
                }
            }
        },
        "mapSpritePalettes": {
            "type": "array",
            "name": "Map Sprite Palettes",
            "range": "0xE68000-0xE68400",
            "array": {
                "length": 32
            },
            "assembly": {
                "type": "assembly",
                "length": 32,
                "format": "bgr555"
            },
            "stringTable": {
                "name": "Map Sprite Palettes",
                "default": "Palette %i",
                "length": 32,
                "string": {
                    "0": "Edgar, Sabin, Celes",
                    "1": "Locke",
                    "2": "Terra",
                    "3": "Strago, Relm, Gau, Gogo",
                    "4": "Cyan, Shadow, Setzer",
                    "5": "Mog, Umaro",
                    "6": "Rainbow",
                    "7": "Chocobo/Magitek",
                    "8": "Esper Terra",
                    "9": "Edgar, Sabin, Celes (Alt.)",
                    "10": "Machinery A",
                    "11": "Raft",
                    "12": "Machinery B",
                    "13": "Guardian",
                    "14": "Sealed Gate Door",
                    "15": "Vector Crane",
                    "16": "Gray (Statue/Smoke)",
                    "17": "Treasure Chest",
                    "18": "Chadarnook",
                    "19": "Rock",
                    "20": "Falcon",
                    "21": "Odin",
                    "22": "Kefka's Tower / Orange Parallax",
                    "23": "Kefka's Tower / Blue/White Parallax",
                    "24": "Kefka's Tower / Map 356 Parallax",
                    "25": "Dadaluma",
                    "26": "Green Magicite Smoke",
                    "29": "Airship WoB Parallax"
                }
            }
        },
        "mapSpritePointers": {
            "type": "array",
            "name": "Map Sprite Graphics Pointers",
            "range": "0xC0D0F2-0xC0D386",
            "format": "interlace(2,2,165)",
            "array": {
                "length": 165
            },
            "assembly": {
                "type": "data",
                "assembly": {
                    "pointer": {
                        "type": "property",
                        "name": "Pointer",
                        "begin": 0,
                        "mask": "0xFFFFFF"
                    }
                },
                "length": 4
            }
        },
        "mapStartupEvents": {
            "type": "array",
            "name": "Map Startup Events",
            "range": "0xD1FA00-0xD20000",
            "array": {
                "length": 512
            },
            "assembly": {
                "type": "property",
                "name": "Start-Up Event",
                "length": 3,
                "mask": "0xFFFFFF",
                "script": "eventScript"
            }
        },
        "mapTileProperties": {
            "type": "array",
            "name": "Map Tile Properties",
            "range": "0xD9A800-0xD9CC4C",
            "canRelocate": true,
            "array": {
                "length": 42,
                "max": 128
            },
            "assembly": {
                "type": "array",
                "format": ["interlace(1,2,256)", "ff6-lzss"],
                "array": {
                    "length": 256
                },
                "assembly": {
                    "type": "data",
                    "length": 2,
                    "special": {
                        "0xFFF7": "Impassable",
                        "0x0007": "Through-Tile"
                    },
                    "assembly": {
                        "zLevel": {
                            "type": "property",
                            "name": "Z-Level",
                            "mask": "0x07",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0": "None",
                                    "1": "Upstairs",
                                    "2": "Downstairs",
                                    "3": "Upstairs & Downstairs",
                                    "4": "Bridge"
                                }
                            }
                        },
                        "flags": {
                            "type": "property",
                            "name": "Flags",
                            "mask": "0xFFF8",
                            "flag": true,
                            "stringTable": {
                                "string": {
                                    "0": "Top Sprite Priority",
                                    "1": "Bottom Sprite Priority",
                                    "2": "Door",
                                    "3": "Stairs Up/Right",
                                    "4": "Stairs Up/Left",
                                    "5": "Passable Through Right",
                                    "6": "Passable Through Left",
                                    "7": "Passable Through Bottom",
                                    "8": "Passable Through Top",
                                    "11": "Ladder",
                                    "12": "Passable by NPCs"
                                }
                            }
                        }
                    }
                }
            },
            "pointerTable": {
                "range": "0xD9CD10-0xD9CD66",
                "offset": "0xD9A800",
                "canRelocate": true,
                "reference": [
                    {
                        "begin": "0xC01CD6",
                        "mask": "0xFFFFFF"
                    }
                ]
            },
            "reference": [
                {
                    "begin": "0xC01CDA",
                    "mask": "0xFFFF"
                }, {
                    "begin": "0xC01CE2",
                    "mask": "0xFF",
                    "bankByte": true
                }
            ],
            "stringTable": {
                "length": 42,
                "default": "Tile Properties %i",
                "string": {
                    "0": "None",
                    "1": "Figaro Castle",
                    "2": "Doma Castle",
                    "3": "Village Exterior 1",
                    "4": "Town Exterior",
                    "5": "Castle Interior",
                    "6": "Caves",
                    "7": "Village/Town Interior",
                    "8": "Narshe Exterior",
                    "9": "Caves w/ Furniture",
                    "10": "Castle Basement",
                    "11": "Village Exterior 2",
                    "12": "Mountains Exterior",
                    "13": "Train Exterior",
                    "14": "Zozo",
                    "15": "Floating Island",
                    "16": "Mountains Interior",
                    "17": "Docks",
                    "18": "Narshe Exterior (Intro)",
                    "19": "Train Interior",
                    "20": "Imperial Camp",
                    "21": "Imperial Castle Exterior",
                    "22": "Airship Exterior",
                    "23": "Airship Interior",
                    "24": "Imperial Castle Interior",
                    "25": "Snowfields",
                    "26": "Forest",
                    "27": "Vector",
                    "28": "River",
                    "29": "Opera House",
                    "30": "Destroyed Town",
                    "31": "Magitek Factory",
                    "32": "Darill's Tomb",
                    "33": "Castle (Opera House)",
                    "34": "Burning Building",
                    "35": "Sealed Gate",
                    "36": "Magitek Research Facility",
                    "37": "Magitek Research Facility w/ Garbage",
                    "38": "Beach",
                    "39": "Cyan's Dream",
                    "40": "Kefka's Tower 1",
                    "41": "Kefka's Tower 2"
                }
            }
        },
        "mapTilesets": {
            "type": "array",
            "name": "Map Tilesets",
            "range": "0xDE0000-0xDFB400",
            "array": {
                "length": 75
            },
            "assembly": {
                "type": "tilemap",
                "format": [
                    "snes4bppTile",
                    "interlace(4,16,2)",
                    "interlace(1,2,4)",
                    "interlace(1,8,256)",
                    "ff6-lzss"
                ],
                "width": 32,
                "height": 32,
                "graphics": [],
                "palette": {
                    "path": []
                }
            },
            "pointerTable": {
                "range": "0xDFBA00-0xDFBB00",
                "pointerLength": 3,
                "offset": "0xDE0000"
            },
            "stringTable": {
                "length": 75,
                "default": "Map Tileset %i",
                "string": {
                    "0": "Figaro Castle",
                    "1": "Village Exterior (BG1)",
                    "2": "Village Exterior (BG2)",
                    "3": "Doma Castle (BG1)",
                    "4": "Doma Castle (BG2)",
                    "5": "Town Exterior (BG1)",
                    "6": "Town Exterior (BG2)",
                    "7": "Docks (BG1)",
                    "8": "Docks (BG2)",
                    "9": "Caves (BG1)",
                    "10": "Castle Interior (BG1)",
                    "11": "Castle Interior (BG2)",
                    "12": "River",
                    "13": "Imperial Camp (BG1)",
                    "14": "Imperial Camp (BG2)",
                    "15": "Mountains Interior",
                    "16": "Town Interior (BG1)",
                    "17": "Town Interior (BG2)",
                    "18": "Mountains Exterior (BG1)",
                    "19": "Narshe Exterior (BG1)",
                    "20": "Narshe Exterior (BG2)",
                    "21": "Destroyed Town (BG1)",
                    "22": "Destroyed Town (BG2)",
                    "23": "Snowfields",
                    "24": "Train (BG1)",
                    "25": "Train Interior (BG1)",
                    "26": "Train Interior (BG2)",
                    "27": "Zozo (BG1)",
                    "28": "Zozo (BG2)",
                    "29": "Vector (BG1)",
                    "30": "Vector (BG2)",
                    "31": "Castle Basement (BG1)",
                    "32": "Castle Basement (BG2)",
                    "33": "Caves w/ Furniture",
                    "34": "Magitek Research Facility (BG1)",
                    "35": "Floating Island (BG1)",
                    "36": "Floating Island (BG2)",
                    "37": "Mountains Parallax",
                    "38": "Narshe Caves (BG2)",
                    "39": "Village Exterior (BG1)",
                    "40": "Village Exterior (BG2)",
                    "41": "Kefka's Tower (BG2)",
                    "42": "Waterfall/Sky Parallax",
                    "43": "Kefka's Tower (BG1)",
                    "44": "Mountains Exterior (BG2)",
                    "45": "Train (BG2)",
                    "46": "Caves (BG2)",
                    "47": "Magitek Factory (BG1)",
                    "48": "Magitek Factory (BG2)",
                    "49": "Narshe Cliffs Parallax",
                    "50": "Airship Exterior (BG1)",
                    "51": "Airship Exterior (BG2)",
                    "52": "Airship Interior",
                    "53": "Narshe Exterior (BG1, Intro)",
                    "54": "Cyan's Dream",
                    "55": "Forest (BG1)",
                    "56": "Forest (BG2)",
                    "57": "Imperial Castle Exterior (BG1)",
                    "58": "Imperial Castle Exterior (BG2)",
                    "59": "Imperial Castle Exterior (BG2 w/ Background)",
                    "60": "Beach",
                    "61": "Imperial Castle Interior (BG1)",
                    "62": "Imperial Castle Interior (BG2)",
                    "63": "Magitek Research Facility w/ Garbage (BG1)",
                    "64": "Caves w/ Lava (BG2)",
                    "65": "Sealed Gate (BG1)",
                    "66": "Sealed Gate (BG2)",
                    "67": "Magitek Research Facility (BG2)",
                    "68": "Kefka's Tower Parallax",
                    "70": "Burning Building",
                    "71": "Opera House (BG1)",
                    "72": "Opera House (BG2)",
                    "73": "Darill's Tomb (BG1)",
                    "74": "Darill's Tomb (BG2)"
                }
            }
        },
        "mapTitle": {
            "type": "array",
            "name": "Map Title",
            "range": "0xCEF100-0xCEF463",
            "canRelocate": true,
            "array": {
                "length": 73,
                "max": 256
            },
            "assembly": {
                "type": "text",
                "name": "Title",
                "encoding": "dialog"
            },
            "pointerTable": {
                "range": "0xE68400-0xE68492",
                "offset": "0xCEF100",
                "canRelocate": true,
                "reference": [
                    {
                        "begin": "0xC08009",
                        "mask": "0xFFFFFF"
                    }
                ]
            },
            "stringTable": {
                "length": 73,
                "default": "<mapTitle[%i]>",
                "string": {
                    "0": "No Title"
                }
            },
            "reference": [
                {
                    "begin": "0xC07FFE",
                    "mask": "0xFF",
                    "bankByte": true
                }, {
                    "begin": "0xC0800D",
                    "mask": "0xFFFF"
                }
            ]
        },
        "monsterAttack": {
            "type": "array",
            "name": "Monster Attack",
            "range": "0xCF37C0-0xCF3940",
            "array": {
                "length": 384
            },
            "assembly": {
                "type": "property",
                "name": "Animation",
                "length": 1
            }
        },
        "monsterAttackName": {
            "type": "array",
            "name": "Monster Attack Name",
            "range": "0xCFD0D0-0xCFDFE0",
            "array": {
                "length": 384
            },
            "assembly": {
                "type": "text",
                "name": "Name",
                "encoding": "bigText",
                "length": 10
            },
            "stringTable": {
                "length": 384,
                "default": "<monsterAttackName[%i]>"
            }
        },
        "monsterAttackControl": {
            "type": "array",
            "name": "Monster Control Attack",
            "range": "0xCF3D00-0xCF4300",
            "array": {
                "length": 384
            },
            "assembly": {
                "type": "data",
                "length": 4,
                "assembly": {
                    "attack1": {
                        "type": "property",
                        "name": "Control Attack 1",
                        "begin": 0,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    },
                    "attack2": {
                        "type": "property",
                        "name": "Control Attack 2",
                        "begin": 1,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    },
                    "attack3": {
                        "type": "property",
                        "name": "Control Attack 3",
                        "begin": 2,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    },
                    "attack4": {
                        "type": "property",
                        "name": "Control Attack 4",
                        "begin": 3,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    }
                }
            }
        },
        "monsterAttackRage": {
            "type": "array",
            "name": "Monster Rage Attack",
            "range": "0xCF4600-0xCF4800",
            "array": {
                "length": 256
            },
            "assembly": {
                "type": "data",
                "length": 2,
                "assembly": {
                    "attack1": {
                        "type": "property",
                        "name": "Rage Attack 1",
                        "begin": 0,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    },
                    "attack2": {
                        "type": "property",
                        "name": "Rage Attack 2",
                        "begin": 1,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    }
                }
            }
        },
        "monsterAttackSketch": {
            "type": "array",
            "name": "Monster Sketch Attack",
            "range": "0xCF4300-0xCF4600",
            "array": {
                "length": 384
            },
            "assembly": {
                "type": "data",
                "length": 2,
                "assembly": {
                    "attack1": {
                        "type": "property",
                        "name": "Sketch Attack 1",
                        "begin": 0,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    },
                    "attack2": {
                        "type": "property",
                        "name": "Sketch Attack 2",
                        "begin": 1,
                        "stringTable": "attackName",
                        "link": "attackProperties[%i]"
                    }
                }
            }
        },
        "monsterColosseumType": {
            "type": "array",
            "name": "Monster Colosseum Type",
            "range": "0xECE800-0xECE900",
            "array": {
                "length": 256
            },
            "assembly": {
                "type": "property",
                "name": "Colosseum Type",
                "length": 1,
                "stringTable": {
                    "string": {
                        "0": "Ceiling (top)",
                        "1": "Ground (+0)",
                        "2": "Floating (+8)",
                        "3": "Buried (-8)",
                        "4": "Flying (+40)"
                    }
                }
            }
        },
        "monsterDialog": {
            "type": "array",
            "name": "Monster Dialogue",
            "range": "0xCFE1E0-0xCFF442",
            "pointerAlign": "0x10000",
            "array": {
                "length": 256
            },
            "assembly": {
                "type": "text",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xCFDFE0-0xCFE1E0",
                "offset": "0xCF0000",
                "reference": [
                    {
                        "begin": "0xC19847",
                        "mask": "0xFFFFFF"
                    }
                ]
            },
            "stringTable": {
                "length": 256,
                "default": "<monsterDialog[%i]>"
            },
            "reference": [
                {
                    "begin": "0xC19839",
                    "mask": "0xFF",
                    "bankByte": true
                }
            ]
        },
        "monsterGraphics": {
            "type": "array",
            "name": "Monster Graphics",
            "range": "0xE97000-0xECE200",
            "array": {
                "length": 176
            },
            "assembly": {
                "type": "graphics",
                "palette": "monsterPalette"
            },
            "pointerObject": {
                "arrayPath": "monsterGraphicsProperties",
                "pointerPath": "graphicsPointer"
            },
            "stringTable": {
                "length": 176,
                "default": "Monster Graphics %i",
                "string": {
                    "0": "Guard, Marshal, StillGoing",
                    "1": "Soldier, Commander, Trooper, Commando, Grunt, Officer",
                    "2": "Templar, General, Sp Forces, Leader, Cadet",
                    "3": "Ninja, Covert, Outsider",
                    "4": "Samurai, Retainer, KatanaSoul",
                    "5": "Orog, Ogor, Hemophyte",
                    "6": "Mag Roader",
                    "7": "Hazer, Warlock, Wizard, L.20 Magic",
                    "8": "Dahling, Madam, Barb-e, L.80 Magic",
                    "9": "Rain Man, Joker, Parasoul, L.40 Magic",
                    "10": "Brawler, Iron Fist, Scrapper",
                    "11": "Apokryphos, Goblin",
                    "12": "Dark Force, Wrexsoul, L.70 Magic",
                    "13": "Whisper, Dark Side, Apparite, Pm Stalker, Specter",
                    "14": "Over-Mind, PowerDemon, L.50 Magic",
                    "15": "Osteosaur, Displayer, Allo Ver",
                    "16": "Rhodox, Peepers, Poppers",
                    "17": "Were-Rat, Sewer Rat, Vermin, Wild Rat",
                    "18": "Ursus, Slatter, Prussian, Gold Bear, Ipooh",
                    "19": "Rhinotaur, Rhinox, Ceritops",
                    "20": "Steroidite, Phunbaba",
                    "21": "Leafer, Rhobite, Nohrabbit",
                    "22": "Stray Cat, Wild Cat, Spek Tor",
                    "23": "Lobo, Red Fang, Lunaris, Red Wolf",
                    "24": "Doberman, Vector Pup, Bounty Man, Garm",
                    "25": "Vomammoth, Tusker, Nastidon",
                    "26": "Fidor, Ralph, Bogy",
                    "27": "Baskervor, Chitonid, Adamanchyt",
                    "28": "Suriander, Wart Puck",
                    "29": "Chimera, Rhyos, Vectagoyle",
                    "30": "Behemoth, SrBehemoth, Intangir, GtBehemoth, SrBehemoth",
                    "31": "Mesosaur, Vectaur, Ice Dragon",
                    "32": "Pterodon, Wyvern, Wirey Drgn, Storm Drgn",
                    "33": "FossilFang, Zombone, Black Drgn, Skull Drgn",
                    "34": "White Drgn, Dragon, Red Dragon",
                    "35": "Doom Drgn, CzarDragon, Blue Drgn",
                    "36": "Brachosaur, Brontaur, Gold Drgn",
                    "37": "Tyranosaur, Allosaurus, Chaos Drgn, Dirt Drgn",
                    "38": "Dark Wind, Cirpius, Vindr",
                    "39": "Beakor, Sprinter, Kiwok, Abolisher",
                    "40": "Vulture, Gobbler, Osprey",
                    "41": "Harpy, Harpiai, Aquila",
                    "42": "HermitCrab, GloomShell",
                    "43": "Trapper, Drop, Junk",
                    "44": "Hornet, Mind Candy, Bug",
                    "45": "CrassHoppr, WeedFeeder, Insecare",
                    "46": "Delta Bug, Luridan",
                    "47": "Gilomantis, Toe Cutter, Mantodea",
                    "48": "Trilium, Over Grunk, Mandrake",
                    "49": "Nightshade, Exoray",
                    "50": "TumbleWeed, Crusher",
                    "51": "Bloompire, Uroburos",
                    "52": "Trilobiter, Sand Ray, EarthGuard",
                    "53": "Siegfried",
                    "54": "Nautiloid, Cephaler",
                    "55": "Exocite, Primordite, Maliga",
                    "56": "Anguiform, Latimeria",
                    "57": "Reach Frog, Gigan Toad",
                    "58": "Lizard, Geckorex",
                    "59": "ChickenLip, Cluck",
                    "60": "Hoover, Land Worm, Zone Eater",
                    "61": "Rider, Test Rider, Dante",
                    "62": "Chupon",
                    "63": "Pipsqueak, Tomb Thumb, IronHitman",
                    "64": "M-TekArmor, PlutoArmor, HeavyArmor, Dueller, Mega Armor",
                    "65": "Sky Armor, Sky Cap, Spit Fire, Sky Base",
                    "66": "Telstar, Chaser, Innoc",
                    "67": "Lethal Wpn, Scullion, Io",
                    "68": "Vaporite, Poplium, Rinn, Psychot",
                    "69": "Flan, Muus",
                    "70": "Ing, Misfit, Karkass",
                    "71": "Humpty, Eland",
                    "72": "Brainpan, Phase",
                    "73": "Cruller, Enuo",
                    "74": "Cactrot",
                    "75": "Repo Man, GreaseMonk, Gabbldegak, 1st Class",
                    "76": "Harvester, NeckHunter, Punisher",
                    "77": "Bomb, Grenade, Balloon",
                    "78": "Still Life",
                    "79": "Boxed Set, Pan Dora",
                    "80": "SlamDancer, SoulDancer, Tap Dancer, L.30 Magic",
                    "81": "HadesGigas, Gigantos, Borras, Colossus",
                    "82": "Pug, Pugs, Master Pug",
                    "83": "Magic Urn",
                    "84": "Mover",
                    "85": "Figaliz, Crawler",
                    "86": "Buffalax, Opinicus",
                    "87": "Aspik, Parasite",
                    "88": "Ghost, Spectre, Lich, Necromancr, L.10 Magic",
                    "89": "Areneid, Coelecite, Scorpion",
                    "90": "Actaneon, Anemone, Sea Flower",
                    "91": "Sand Horse, Hipocampus",
                    "92": "Mad Oscar, Evil Oscar",
                    "93": "Crawly, Slurm",
                    "94": "Bleary, Deep Eye",
                    "95": "Critic",
                    "96": "Mag Roader",
                    "97": "Fortis, ProtoArmor",
                    "98": "Trixter, L.90 Magic, Naughty",
                    "99": "Didalos, Dullahan",
                    "100": "Woolly, Nerapa, L.60 Magic",
                    "101": "Veteran",
                    "102": "Whelk, Presenter",
                    "103": "Vargas",
                    "104": "TunnelArmr, Prometheus",
                    "105": "GhostTrain",
                    "106": "Dadaluma",
                    "107": "Shiva",
                    "108": "Ifrit, Esper",
                    "109": "Number 024, MagiMaster",
                    "110": "Number 128, Inferno",
                    "111": "Crane",
                    "112": "Umaro",
                    "113": "Guardian",
                    "114": "Air Force",
                    "115": "Tritoch",
                    "116": "FlameEater",
                    "117": "AtmaWeapon, Atma",
                    "118": "Tentacle, Tentacle",
                    "119": "Doom Gaze",
                    "120": "Chadarnook",
                    "121": "Curley",
                    "122": "Larry",
                    "123": "Moe",
                    "124": "Hidon",
                    "125": "Hidonite",
                    "126": "Doom",
                    "127": "Goddess",
                    "128": "Poltrgeist",
                    "129": "Kefka",
                    "130": "Ultros (Lete River)",
                    "131": "Ultros",
                    "132": "Head",
                    "133": "Whelk Head",
                    "134": "Merchant",
                    "135": "B.Day Suit",
                    "136": "Tentacle",
                    "137": "RightBlade",
                    "138": "Left Blade",
                    "139": "Rough",
                    "140": "Striker",
                    "141": "Laser Gun",
                    "142": "Speck",
                    "143": "MissileBay",
                    "144": "Chadarnook",
                    "145": "Kefka",
                    "146": "Piranha, Rizopas",
                    "147": "SoulSaver",
                    "148": "Ramuh",
                    "149": "Ifrit (Summon)",
                    "150": "Siren",
                    "151": "Terrato",
                    "152": "Shoat",
                    "153": "Maduin",
                    "154": "Bismark",
                    "155": "Stray",
                    "156": "Palidor",
                    "157": "Tritoch",
                    "158": "Odin",
                    "159": "Raiden",
                    "160": "Bahamut",
                    "161": "Alexandr",
                    "162": "Crusader 1",
                    "163": "Kirin",
                    "164": "ZoneSeek",
                    "165": "Carbunkl",
                    "166": "Phantom",
                    "167": "Sraphim",
                    "168": "Golem",
                    "169": "Unicorn",
                    "170": "Fenrir",
                    "171": "Starlet",
                    "172": "Phoenix",
                    "173": "TigerBreak",
                    "174": "Crusader 2",
                    "175": "Imp"
                }
            }
        },
        "monsterGraphicsMap": {
            "type": "data",
            "name": "Monster Graphics Map",
            "range": "0xD2A820-0xD2B224",
            "isSequential": true,
            "assembly": {
                "smallPointer": {
                    "type": "property",
                    "begin": 0,
                    "mask": "0xFFFF"
                },
                "largePointer": {
                    "type": "property",
                    "begin": 2,
                    "mask": "0xFFFF"
                },
                "small": {
                    "type": "array",
                    "range": "0x0004-0x0404",
                    "array": {
                        "length": 128
                    },
                    "assembly": {
                        "type": "assembly",
                        "length": 8
                    },
                    "stringTable": {
                        "length": 128,
                        "default": "Small Monster Graphics Map %i",
                        "string": {
                            "0": "Guard, Marshal, StillGoing",
                            "1": "Soldier, Commander, Trooper, Commando, Grunt, Officer",
                            "2": "Templar, General, Sp Forces, Leader, Cadet",
                            "3": "Ninja, Covert, Outsider",
                            "4": "Samurai, Retainer, KatanaSoul",
                            "5": "Orog, Ogor, Hemophyte",
                            "6": "Mag Roader, Mag Roader",
                            "7": "Hazer, Warlock, Wizard, L.20 Magic",
                            "8": "Dahling, Madam, Barb-e, L.80 Magic",
                            "9": "Rain Man, Joker, Parasoul, L.40 Magic",
                            "10": "Brawler, Iron Fist, Scrapper",
                            "11": "Apokryphos, Goblin",
                            "12": "Dark Force, Wrexsoul, L.70 Magic",
                            "13": "Whisper, Dark Side, Apparite, Pm Stalker, Specter",
                            "14": "Over-Mind, PowerDemon, L.50 Magic",
                            "15": "Rhodox, Peepers, Poppers",
                            "16": "Were-Rat, Sewer Rat, Vermin, Wild Rat",
                            "17": "Ursus, Slatter, Prussian, Gold Bear, Ipooh",
                            "18": "Rhinotaur, Rhinox, Ceritops",
                            "19": "Leafer, Rhobite, Nohrabbit",
                            "20": "Stray Cat, Wild Cat, Spek Tor",
                            "21": "Lobo, Red Fang, Lunaris, Red Wolf",
                            "22": "Doberman, Vector Pup, Bounty Man, Garm",
                            "23": "Vomammoth, Tusker, Nastidon",
                            "24": "Fidor, Ralph, Bogy",
                            "25": "Baskervor, Chitonid, Adamanchyt",
                            "26": "Suriander, Wart Puck",
                            "27": "Mesosaur, Vectaur, Ice Dragon",
                            "28": "Pterodon, Wyvern, Wirey Drgn, Storm Drgn",
                            "29": "FossilFang, Zombone, Black Drgn, Skull Drgn",
                            "30": "Tyranosaur, Allosaurus, Chaos Drgn, Dirt Drgn",
                            "31": "Dark Wind, Cirpius, Vindr",
                            "32": "Beakor, Sprinter, Kiwok, Abolisher",
                            "33": "Vulture, Gobbler, Osprey",
                            "34": "HermitCrab, GloomShell",
                            "35": "Trapper, Drop, Junk",
                            "36": "Hornet, Mind Candy, Bug",
                            "37": "CrassHoppr, WeedFeeder, Insecare",
                            "38": "Delta Bug, Luridan",
                            "39": "Gilomantis, Toe Cutter, Mantodea",
                            "40": "Trilium, Over Grunk, Mandrake",
                            "41": "Nightshade, Exoray",
                            "42": "TumbleWeed, Crusher",
                            "43": "Bloompire, Uroburos",
                            "44": "Trilobiter, Sand Ray, EarthGuard",
                            "45": "Siegfried",
                            "46": "Nautiloid, Cephaler",
                            "47": "Exocite, Primordite, Maliga",
                            "48": "Anguiform, Latimeria",
                            "49": "Reach Frog, Gigan Toad",
                            "50": "Lizard, Geckorex",
                            "51": "ChickenLip, Cluck",
                            "52": "Pipsqueak, Tomb Thumb, IronHitman",
                            "53": "M-TekArmor, PlutoArmor, HeavyArmor, Dueller, Mega Armor",
                            "54": "Sky Armor, Sky Cap, Spit Fire, Sky Base",
                            "55": "Telstar, Chaser, Innoc",
                            "56": "Vaporite, Poplium, Rinn, Psychot",
                            "57": "Flan, Muus",
                            "58": "Ing, Misfit, Karkass",
                            "59": "Humpty, Eland",
                            "60": "Brainpan, Phase",
                            "61": "Cruller, Enuo",
                            "62": "Cactrot",
                            "63": "Repo Man, GreaseMonk, Gabbldegak, 1st Class",
                            "64": "Harvester, NeckHunter, Punisher",
                            "65": "Bomb, Grenade, Balloon",
                            "66": "Still Life",
                            "67": "Boxed Set, Pan Dora",
                            "68": "SlamDancer, SoulDancer, Tap Dancer, L.30 Magic",
                            "69": "Pug, Pugs, Master Pug",
                            "70": "Magic Urn",
                            "71": "Mover",
                            "72": "Figaliz, Crawler",
                            "73": "Aspik, Parasite",
                            "74": "Ghost, Spectre, Lich, Necromancr, L.10 Magic",
                            "75": "Areneid, Coelecite, Scorpion",
                            "76": "Actaneon, Anemone, Sea Flower",
                            "77": "Sand Horse, Hipocampus",
                            "78": "Mad Oscar, Evil Oscar",
                            "79": "Crawly, Slurm",
                            "80": "Bleary, Deep Eye",
                            "81": "Mag Roader",
                            "82": "Fortis, ProtoArmor",
                            "83": "Trixter, L.90 Magic, Naughty",
                            "84": "Woolly, Nerapa, L.60 Magic",
                            "85": "Veteran",
                            "86": "Whelk, Presenter",
                            "87": "GhostTrain",
                            "88": "Shiva",
                            "89": "Ifrit, Kefka vs. Esper",
                            "90": "Umaro, Umaro",
                            "91": "FlameEater",
                            "92": "Tentacle",
                            "93": "Curley",
                            "94": "Larry",
                            "95": "Moe",
                            "96": "Hidonite",
                            "97": "Ultros (Lete River)",
                            "98": "Ultros",
                            "99": "Head",
                            "100": "Whelk Head",
                            "101": "Merchant",
                            "102": "B.Day Suit",
                            "103": "Tentacle",
                            "104": "RightBlade",
                            "105": "Left Blade",
                            "106": "Rough",
                            "107": "Striker",
                            "108": "Laser Gun",
                            "109": "Speck",
                            "110": "MissileBay",
                            "111": "Kefka (Hills Maze), Kefka vs. Leo",
                            "112": "Piranha, Rizopas",
                            "113": "SoulSaver",
                            "114": "Ramuh",
                            "115": "Siren",
                            "116": "Shoat",
                            "117": "Maduin",
                            "118": "Stray",
                            "119": "Kirin",
                            "120": "ZoneSeek",
                            "121": "Carbunkl",
                            "122": "Phantom",
                            "123": "Sraphim",
                            "124": "Golem",
                            "125": "Unicorn",
                            "126": "TigerBreak",
                            "127": "Imp"
                        }
                    }
                },
                "large": {
                    "type": "array",
                    "range": "0x0404-0x0A04",
                    "array": {
                        "length": 48
                    },
                    "assembly": {
                        "type": "assembly",
                        "length": 32,
                        "format": "byteSwapped(2)"
                    },
                    "stringTable": {
                        "length": 48,
                        "default": "Large Monster Graphics Map %i",
                        "string": {
                            "0": "Osteosaur, Displayer, Allo Ver",
                            "1": "Steroidite, Phunbaba",
                            "2": "Chimera, Rhyos, Vectagoyle",
                            "3": "Behemoth, SrBehemoth, Intangir, GtBehemoth, SrBehemoth",
                            "4": "White Drgn, Dragon, Red Dragon",
                            "5": "Doom Drgn, CzarDragon, Blue Drgn",
                            "6": "Brachosaur, Brontaur, Gold Drgn",
                            "7": "Harpy, Harpiai, Aquila",
                            "8": "Hoover, Land Worm, Zone Eater",
                            "9": "Rider, Test Rider, Dante",
                            "10": "Chupon",
                            "11": "Lethal Wpn, Scullion, Io",
                            "12": "HadesGigas, Gigantos, Borras, Colossus",
                            "13": "Buffalax, Opinicus",
                            "14": "Critic",
                            "15": "Didalos, Dullahan",
                            "16": "Vargas",
                            "17": "TunnelArmr, Prometheus",
                            "18": "Dadaluma",
                            "19": "Number 024, MagiMaster",
                            "20": "Number 128, Inferno",
                            "21": "Crane",
                            "22": "Guardian",
                            "23": "Air Force",
                            "24": "Tritoch",
                            "25": "AtmaWeapon, Atma",
                            "26": "Doom Gaze",
                            "27": "Chadarnook",
                            "28": "Hidon",
                            "29": "Doom",
                            "30": "Goddess",
                            "31": "Poltrgeist",
                            "32": "Kefka",
                            "33": "Chadarnook",
                            "34": "Ifrit (Summon)",
                            "35": "Terrato",
                            "36": "Bismark",
                            "37": "Palidor",
                            "38": "Tritoch",
                            "39": "Odin",
                            "40": "Raiden",
                            "41": "Bahamut",
                            "42": "Alexandr",
                            "43": "Crusader 1",
                            "44": "Fenrir",
                            "45": "Starlet",
                            "46": "Phoenix",
                            "47": "Crusader 2"
                        }
                    }
                }
            }
        },
        "monsterGraphicsProperties": {
            "type": "array",
            "name": "Monster Graphics Properties",
            "range": "0xD27000-0xD27820",
            "array": {
                "length": 416
            },
            "assembly": {
                "type": "data",
                "length": 5,
                "assembly": {
                    "graphicsPointer": {
                        "type": "property",
                        "name": "Graphics Pointer",
                        "mask": "0x7FFF",
                        "multiplier": 8,
                        "offset": "0xE97000",
                        "pointerTo": "monsterGraphics"
                    },
                    "is3bpp": {
                        "type": "property",
                        "name": "3bpp Graphics",
                        "mask": "0x80",
                        "begin": 1,
                        "bool": true
                    },
                    "palette": {
                        "type": "property",
                        "name": "Palette",
                        "mask": "0x03FF",
                        "begin": 2,
                        "multiplier": 16,
                        "offset": "0xD27820",
                        "format": "byteSwapped",
                        "pointerTo": "monsterPalette"
                    },
                    "smallMap": {
                        "type": "property",
                        "name": "Graphics Map",
                        "begin": 4,
                        "stringTable": "monsterGraphicsMap.small",
                        "invalid": "this.parent.useLargeMap.value"
                    },
                    "largeMap": {
                        "type": "property",
                        "name": "Graphics Map",
                        "begin": 4,
                        "stringTable": "monsterGraphicsMap.large",
                        "invalid": "!this.parent.useLargeMap.value"
                    },
                    "useLargeMap": {
                        "type": "property",
                        "name": "Use Large Map",
                        "mask": "0x80",
                        "begin": 2,
                        "bool": true
                    }
                }
            }
        },
        "monsterItems": {
            "type": "array",
            "name": "Monster Items",
            "range": "0xCF3000-0xCF3600",
            "array": {
                "length": 384
            },
            "assembly": {
                "type": "data",
                "length": 4,
                "assembly": {
                    "rareSteal": {
                        "type": "property",
                        "name": "Rare Steal",
                        "begin": 0,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "commonSteal": {
                        "type": "property",
                        "name": "Common Steal",
                        "begin": 1,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "rareDrop": {
                        "type": "property",
                        "name": "Rare Drop",
                        "begin": 2,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "commonDrop": {
                        "type": "property",
                        "name": "Common Drop",
                        "begin": 3,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    }
                }
            }
        },
        "monsterName": {
            "type": "array",
            "name": "Monster Name",
            "range": "0xCFC050-0xCFD0D0",
            "array": {
                "length": 384
            },
            "assembly": {
                "type": "text",
                "name": "Name",
                "encoding": "smallText",
                "length": 10
            },
            "stringTable": {
                "length": 384,
                "default": "<monsterName[%i]>",
                "string": {
                    "273": "<monsterName[%i]> (Vector)",
                    "274": "<monsterName[%i]> (Kefka's Tower)",
                    "276": "<monsterName[%i]> (Intro)",
                    "277": "<monsterName[%i]> (Terra Transforms)",
                    "282": "Final Battle Script (Unused)",
                    "300": "<monsterName[%i]> (Lete River)",
                    "301": "<monsterName[%i]> (Opera House)",
                    "302": "<monsterName[%i]> (Esper Mountain)",
                    "324": "<monsterName[%i]> (WoR)",
                    "330": "<monsterName[%i]> (Hills Maze)",
                    "360": "<monsterName[%i]> (Airship)",
                    "366": "Terra's Flashback",
                    "367": "Kefka (Imperial Camp)",
                    "368": "Cyan (Imperial Camp)",
                    "370": "Gau (Veldt)",
                    "371": "Kefka vs. Leo",
                    "372": "Kefka (Esper Gate)",
                    "378": "Kefka vs. Esper",
                    "379": "Battle Event",
                    "382": "Shadow (Colosseum)",
                    "383": "Colosseum"
                }
            }
        },
        "monsterPalette": {
            "type": "array",
            "name": "Monster Palette",
            "range": "0xD27820-0xD2A130",
            "array": {
                "length": 348
            },
            "assembly": {
                "format": "bgr555"
            },
            "pointerObject": {
                "arrayPath": "monsterGraphicsProperties",
                "pointerPath": "palette"
            },
            "stringTable": {
                "default": "Monster Palette %i",
                "string": {
                    "0": "Guard",
                    "1": "Soldier",
                    "2": "Templar",
                    "3": "Ninja",
                    "4": "Samurai",
                    "5": "Orog",
                    "6": "Mag Roader",
                    "7": "Hazer",
                    "8": "Dahling",
                    "9": "Rain Man",
                    "10": "Brawler",
                    "11": "Apokryphos",
                    "12": "Dark Force",
                    "13": "Whisper",
                    "14": "Over-Mind",
                    "15": "Osteosaur",
                    "16": "Rhodox",
                    "17": "Were-Rat",
                    "18": "Ursus",
                    "19": "Rhinotaur",
                    "20": "Steroidite",
                    "21": "Leafer",
                    "22": "Stray Cat",
                    "23": "Lobo",
                    "24": "Doberman",
                    "25": "Vomammoth",
                    "26": "Fidor",
                    "27": "Baskervor",
                    "28": "Suriander",
                    "29": "Chimera",
                    "30": "Behemoth",
                    "31": "Mesosaur",
                    "32": "Pterodon",
                    "33": "FossilFang",
                    "34": "White Drgn",
                    "35": "Doom Drgn",
                    "36": "Brachosaur",
                    "37": "Tyranosaur",
                    "38": "Dark Wind",
                    "39": "Beakor",
                    "40": "Vulture",
                    "41": "Harpy",
                    "42": "HermitCrab",
                    "43": "Trapper",
                    "44": "Hornet",
                    "45": "CrassHoppr",
                    "46": "Delta Bug",
                    "47": "Gilomantis",
                    "48": "Trilium",
                    "49": "Nightshade",
                    "50": "TumbleWeed",
                    "51": "Bloompire",
                    "52": "Trilobiter",
                    "53": "Siegfried",
                    "54": "Nautiloid",
                    "55": "Exocite",
                    "56": "Anguiform",
                    "57": "Reach Frog",
                    "58": "Lizard",
                    "59": "ChickenLip",
                    "60": "Hoover",
                    "61": "Rider",
                    "62": "Chupon",
                    "63": "Pipsqueak",
                    "64": "M-TekArmor",
                    "65": "Sky Armor",
                    "66": "Telstar",
                    "67": "Lethal Wpn",
                    "68": "Vaporite",
                    "69": "Flan",
                    "70": "Ing",
                    "71": "Humpty",
                    "72": "Brainpan",
                    "73": "Cruller",
                    "74": "Cactrot",
                    "75": "Repo Man",
                    "76": "Harvester",
                    "77": "Bomb",
                    "78": "Still Life",
                    "79": "Boxed Set",
                    "80": "SlamDancer",
                    "81": "HadesGigas",
                    "82": "Pug",
                    "83": "Magic Urn",
                    "84": "Mover",
                    "85": "Figaliz",
                    "86": "Buffalax",
                    "87": "Aspik",
                    "88": "Ghost",
                    "89": "Sand Ray",
                    "90": "Areneid",
                    "91": "Actaneon",
                    "92": "Sand Horse",
                    "93": "Mad Oscar",
                    "94": "Crawly",
                    "95": "Bleary",
                    "96": "Marshal",
                    "97": "Trooper",
                    "98": "General",
                    "99": "Covert",
                    "100": "Ogor",
                    "101": "Warlock",
                    "102": "Madam",
                    "103": "Joker",
                    "104": "Iron Fist",
                    "105": "Goblin",
                    "106": "Apparite",
                    "107": "PowerDemon",
                    "108": "Displayer",
                    "109": "Peepers",
                    "110": "Sewer Rat",
                    "111": "Slatter",
                    "112": "Rhinox",
                    "113": "Rhobite",
                    "114": "Wild Cat, Spek Tor",
                    "115": "Red Fang",
                    "116": "Bounty Man",
                    "117": "Tusker",
                    "118": "Ralph",
                    "119": "Chitonid",
                    "120": "Wart Puck",
                    "121": "Rhyos",
                    "122": "SrBehemoth",
                    "123": "Vectaur",
                    "124": "Wyvern",
                    "125": "Zombone",
                    "126": "Dragon",
                    "127": "Brontaur",
                    "128": "Allosaurus",
                    "129": "Cirpius",
                    "130": "Sprinter",
                    "131": "Gobbler",
                    "132": "Harpiai",
                    "133": "GloomShell",
                    "134": "Drop",
                    "135": "Mind Candy",
                    "136": "WeedFeeder",
                    "137": "Luridan",
                    "138": "Toe Cutter",
                    "139": "Over Grunk",
                    "140": "Exoray",
                    "141": "Crusher",
                    "142": "Uroburos",
                    "143": "Primordite",
                    "144": "Cephaler",
                    "145": "Maliga",
                    "146": "Gigan Toad",
                    "147": "Geckorex",
                    "148": "Cluck",
                    "149": "Land Worm",
                    "150": "Test Rider",
                    "151": "PlutoArmor",
                    "152": "Tomb Thumb",
                    "153": "HeavyArmor",
                    "154": "Chaser",
                    "155": "Scullion",
                    "156": "Poplium",
                    "157": "Intangir",
                    "158": "Misfit",
                    "159": "Eland",
                    "160": "Enuo",
                    "161": "Deep Eye",
                    "162": "GreaseMonk",
                    "163": "NeckHunter",
                    "164": "Grenade",
                    "165": "Critic",
                    "166": "Pan Dora",
                    "167": "SoulDancer",
                    "168": "Gigantos",
                    "169": "Mag Roader",
                    "170": "Parasite",
                    "171": "EarthGuard",
                    "172": "Coelecite",
                    "173": "Anemone",
                    "174": "Hipocampus",
                    "175": "Evil Oscar",
                    "176": "Slurm",
                    "177": "Latimeria",
                    "178": "StillGoing",
                    "179": "Phase",
                    "180": "Outsider",
                    "181": "Barb-e",
                    "182": "Parasoul",
                    "183": "Pm Stalker, Specter",
                    "184": "Hemophyte",
                    "185": "Sp Forces, Leader",
                    "186": "Nohrabbit",
                    "187": "Wizard",
                    "188": "Scrapper",
                    "189": "Ceritops",
                    "190": "Commando",
                    "191": "Opinicus",
                    "192": "Poppers",
                    "193": "Lunaris",
                    "194": "Garm",
                    "195": "Vindr",
                    "196": "Kiwok",
                    "197": "Nastidon",
                    "198": "Insecare",
                    "199": "Vermin",
                    "200": "Mantodea",
                    "201": "Bogy",
                    "202": "Prussian",
                    "203": "Black Drgn",
                    "204": "Adamanchyt",
                    "205": "Dante",
                    "206": "Wirey Drgn",
                    "207": "Dueller",
                    "208": "Psychot",
                    "209": "Muus",
                    "210": "Karkass",
                    "211": "Punisher",
                    "212": "Gabbldegak",
                    "213": "GtBehemoth",
                    "214": "Scorpion",
                    "215": "Chaos Drgn",
                    "216": "Spit Fire",
                    "217": "Vectagoyle",
                    "218": "Lich",
                    "219": "Osprey",
                    "220": "Mag Roader",
                    "221": "Bug",
                    "222": "Sea Flower",
                    "223": "Fortis",
                    "224": "Abolisher",
                    "225": "Aquila",
                    "226": "Junk",
                    "227": "Mandrake",
                    "228": "1st Class",
                    "229": "Tap Dancer",
                    "230": "Necromancr",
                    "231": "Borras",
                    "232": "Mag Roader",
                    "233": "Gold Bear",
                    "234": "Innoc",
                    "235": "Trixter",
                    "236": "Red Wolf",
                    "237": "Didalos",
                    "238": "Woolly",
                    "239": "Veteran",
                    "240": "Sky Base",
                    "241": "IronHitman",
                    "242": "Io",
                    "243": "Whelk",
                    "244": "Presenter",
                    "245": "Mega Armor",
                    "246": "Vargas",
                    "247": "TunnelArmr",
                    "248": "Prometheus",
                    "249": "GhostTrain",
                    "250": "Dadaluma",
                    "251": "Shiva",
                    "252": "Ifrit",
                    "253": "Number 024",
                    "254": "Number 128",
                    "255": "Inferno",
                    "256": "Crane",
                    "257": "Umaro",
                    "258": "Guardian",
                    "259": "Air Force",
                    "260": "Tritoch",
                    "261": "FlameEater",
                    "262": "AtmaWeapon",
                    "263": "Nerapa",
                    "264": "SrBehemoth",
                    "265": "Tentacle",
                    "266": "Dullahan",
                    "267": "Doom Gaze",
                    "268": "Chadarnook",
                    "269": "Curley",
                    "270": "Larry",
                    "271": "Moe",
                    "272": "Wrexsoul",
                    "273": "Hidon",
                    "274": "KatanaSoul",
                    "275": "L.30 Magic",
                    "276": "Hidonite",
                    "277": "Doom",
                    "278": "Goddess",
                    "279": "Poltrgeist",
                    "280": "Kefka",
                    "281": "L.40 Magic",
                    "282": "Ultros (Lete River)",
                    "283": "Ultros",
                    "284": "L.20 Magic",
                    "285": "L.10 Magic",
                    "286": "L.50 Magic",
                    "287": "Head",
                    "288": "Whelk Head",
                    "289": "CzarDragon",
                    "290": "Master Pug",
                    "291": "L.60 Magic",
                    "292": "Merchant",
                    "293": "B.Day Suit",
                    "294": "RightBlade, Left Blade",
                    "295": "Rough, Striker",
                    "296": "L.70 Magic",
                    "297": "Laser Gun, MissileBay",
                    "298": "Speck",
                    "299": "Chadarnook",
                    "300": "Ice Dragon",
                    "301": "Kefka",
                    "302": "Storm Drgn",
                    "303": "Dirt Drgn",
                    "304": "Gold Drgn",
                    "305": "Skull Drgn",
                    "306": "Blue Drgn",
                    "307": "Red Dragon",
                    "308": "Piranha",
                    "309": "Rizopas",
                    "310": "L.90 Magic",
                    "311": "ProtoArmor",
                    "312": "MagiMaster",
                    "313": "SoulSaver",
                    "314": "Naughty",
                    "315": "Phunbaba",
                    "316": "Zone Eater",
                    "317": "Officer",
                    "318": "Esper",
                    "319": "Atma",
                    "320": "Ramuh",
                    "321": "Ifrit",
                    "322": "Siren",
                    "323": "Terrato",
                    "324": "Shoat",
                    "325": "Maduin",
                    "326": "Bismark",
                    "327": "Stray",
                    "328": "Palidor",
                    "329": "Tritoch",
                    "330": "Odin",
                    "331": "Raiden",
                    "332": "Bahamut",
                    "333": "Alexandr",
                    "334": "Crusader 1",
                    "335": "Kirin",
                    "336": "ZoneSeek",
                    "337": "Carbunkl",
                    "338": "Phantom",
                    "339": "Sraphim",
                    "340": "Golem",
                    "341": "Unicorn",
                    "342": "Fenrir",
                    "343": "Starlet",
                    "344": "Phoenix",
                    "345": "TigerBreak (Unused)",
                    "346": "Crusader 2",
                    "347": "Crusader 3"
                }
            }
        },
        "monsterProperties": {
            "type": "array",
            "name": "Monster Properties",
            "range": "0xCF0000-0xCF3000",
            "stringTable": "monsterName",
            "array": {
                "length": 384
            },
            "assembly": {
                "type": "data",
                "length": 32,
                "assembly": {
                    "monsterName": {
                        "type": "assembly",
                        "external": "monsterName[%i]"
                    },
                    "level": {
                        "type": "property",
                        "name": "Level",
                        "begin": 16
                    },
                    "speed": {
                        "type": "property",
                        "name": "Speed",
                        "begin": 0
                    },
                    "attackPower": {
                        "type": "property",
                        "name": "Attack Power",
                        "begin": 1
                    },
                    "hitRate": {
                        "type": "property",
                        "name": "Hit Rate",
                        "begin": 2
                    },
                    "evade": {
                        "type": "property",
                        "name": "Evade",
                        "begin": 3
                    },
                    "magicBlock": {
                        "type": "property",
                        "name": "Magic Block",
                        "begin": 4
                    },
                    "defense": {
                        "type": "property",
                        "name": "Defense",
                        "begin": 5
                    },
                    "magicDefense": {
                        "type": "property",
                        "name": "Magic Defense",
                        "begin": 6
                    },
                    "magicPower": {
                        "type": "property",
                        "name": "Magic Power",
                        "begin": 7
                    },
                    "hp": {
                        "type": "property",
                        "name": "HP",
                        "begin": 8,
                        "mask": "0xFFFF"
                    },
                    "mp": {
                        "type": "property",
                        "name": "MP",
                        "begin": 10,
                        "mask": "0xFFFF"
                    },
                    "experience": {
                        "type": "property",
                        "name": "Experience",
                        "begin": 12,
                        "mask": "0xFFFF"
                    },
                    "gp": {
                        "type": "property",
                        "name": "GP",
                        "begin": 14,
                        "mask": "0xFFFF"
                    },
                    "category-attacks": "Attacks",
                    "script": {
                        "type": "property",
                        "external": "monsterScriptPointers[%i]"
                    },
                    "fightAnimation": {
                        "type": "property",
                        "name": "Fight Animation",
                        "begin": 26,
                        "stringTable": "itemNames"
                    },
                    "sketch1": {
                        "type": "assembly",
                        "external": "monsterAttackSketch[%i].attack1"
                    },
                    "sketch2": {
                        "type": "assembly",
                        "external": "monsterAttackSketch[%i].attack2"
                    },
                    "control1": {
                        "type": "assembly",
                        "external": "monsterAttackControl[%i].attack1"
                    },
                    "control2": {
                        "type": "assembly",
                        "external": "monsterAttackControl[%i].attack2"
                    },
                    "control3": {
                        "type": "assembly",
                        "external": "monsterAttackControl[%i].attack3"
                    },
                    "control4": {
                        "type": "assembly",
                        "external": "monsterAttackControl[%i].attack4"
                    },
                    "rage1": {
                        "type": "assembly",
                        "external": "monsterAttackRage[%i].attack1",
                        "invalid": "this.parent.i > 0xFF"
                    },
                    "rage2": {
                        "type": "assembly",
                        "external": "monsterAttackRage[%i].attack2",
                        "invalid": "this.parent.i > 0xFF"
                    },
                    "initialRage": {
                        "type": "assembly",
                        "external": "initialRage[%i]",
                        "invalid": "this.parent.i > 0xFF"
                    },
                    "category-special": "Special Attack",
                    "attackName": {
                        "type": "assembly",
                        "external": "monsterAttackName[%i]"
                    },
                    "attackAnimation": {
                        "type": "assembly",
                        "external": "monsterAttack[%i]"
                    },
                    "attackEffect": {
                        "type": "property",
                        "name": "Effect",
                        "begin": 31,
                        "mask": "0x3F",
                        "stringTable": {
                            "length": 51,
                            "string": {
                                "0-32": "Inflict <stringTable.statusNamesReversed[%i]>",
                                "32": "1.5× Damage",
                                "33": "2.0× Damage",
                                "34": "2.5× Damage",
                                "35": "3.0× Damage",
                                "36": "3.5× Damage",
                                "37": "4.0× Damage",
                                "38": "4.5× Damage",
                                "39": "5.0× Damage",
                                "40": "5.5× Damage",
                                "41": "6.0× Damage",
                                "42": "6.5× Damage",
                                "43": "7.0× Damage",
                                "44": "7.5× Damage",
                                "45": "8.0× Damage",
                                "46": "8.5× Damage",
                                "47": "9.0× Damage",
                                "48": "Drain HP",
                                "49": "Drain MP",
                                "50": "Remove <stringTable.statusNames[16]>"
                            }
                        }
                    },
                    "attackFlags": {
                        "type": "property",
                        "name": "Flags",
                        "begin": 31,
                        "mask": "0xC0",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "No Damage",
                                "1": "Can't Miss"
                            }
                        }
                    },
                    "category-status": "Flags",
                    "statusImmunity": {
                        "type": "property",
                        "name": "Immune",
                        "begin": 20,
                        "mask": "0xFFFFFF",
                        "flag": true,
                        "stringTable": {
                            "length": 24,
                            "default": "<stringTable.statusNamesReversed[%i]>",
                            "string": {
                                "16": "Flying"
                            }
                        }
                    },
                    "statusSet": {
                        "type": "property",
                        "name": "Set",
                        "begin": 27,
                        "mask": "0xFFFFFF",
                        "flag": true,
                        "stringTable": "monsterProperties.statusImmunity"
                    },
                    "elementAbsorb": {
                        "type": "property",
                        "name": "Absorb",
                        "begin": 23,
                        "mask": "0xFF",
                        "flag": true,
                        "stringTable": "element"
                    },
                    "elementNull": {
                        "type": "property",
                        "name": "Nullify",
                        "begin": 24,
                        "mask": "0xFF",
                        "flag": true,
                        "stringTable": "element"
                    },
                    "elementWeak": {
                        "type": "property",
                        "name": "Weak",
                        "begin": 25,
                        "mask": "0xFF",
                        "flag": true,
                        "stringTable": "element"
                    },
                    "flags": {
                        "type": "property",
                        "name": "Misc.",
                        "begin": 18,
                        "mask": "0xFFFF",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Dies at Zero MP",
                                "2": "Hide Name",
                                "4": "Human",
                                "6": "Imp Critical",
                                "7": "Undead",
                                "8": "Harder to Run",
                                "9": "First Strike",
                                "10": "Can't Suplex",
                                "11": "Can't Run",
                                "12": "Can't Scan",
                                "13": "Can't Sketch",
                                "14": "Special Event",
                                "15": "Can't Control"
                            }
                        }
                    },
                    "flags2": {
                        "type": "property",
                        "begin": 30,
                        "mask": "0x82",
                        "flag": true,
                        "stringTable": {
                            "string": {
                                "0": "Speck",
                                "6": "Piranha"
                            }
                        }
                    },
                    "category-items": "Items",
                    "rareSteal": {
                        "type": "assembly",
                        "external": "monsterItems[%i].rareSteal"
                    },
                    "commonSteal": {
                        "type": "assembly",
                        "external": "monsterItems[%i].commonSteal"
                    },
                    "rareDrop": {
                        "type": "assembly",
                        "external": "monsterItems[%i].rareDrop"
                    },
                    "commonDrop": {
                        "type": "assembly",
                        "external": "monsterItems[%i].commonDrop"
                    },
                    "morphItems": {
                        "type": "property",
                        "name": "Metamorphosis",
                        "begin": 17,
                        "mask": "0x1F",
                        "link": "itemMetamorphosis[%i]",
                        "stringTable": "itemMetamorphosis"
                    },
                    "morphRate": {
                        "type": "property",
                        "name": "Morph Rate",
                        "begin": 17,
                        "mask": "0xE0",
                        "stringTable": {
                            "string": {
                                "0": "255/256 (99.6%)",
                                "1": "3/4 (75%)",
                                "2": "1/2 (50%)",
                                "3": "1/4 (25%)",
                                "4": "1/8 (12.5%)",
                                "5": "1/16 (6.25%)",
                                "6": "1/32 (3.125%)",
                                "7": "0 (0%)"
                            }
                        }
                    },
                    "category-graphics": "Graphics",
                    "graphics": {
                        "type": "property",
                        "name": "Graphics",
                        "external": "monsterGraphicsProperties[%i].graphicsPointer"
                    },
                    "is3bpp": {
                        "type": "property",
                        "name": "3bpp Graphics",
                        "external": "monsterGraphicsProperties[%i].is3bpp"
                    },
                    "smallMap": {
                        "type": "property",
                        "external": "monsterGraphicsProperties[%i].smallMap"
                    },
                    "largeMap": {
                        "type": "property",
                        "external": "monsterGraphicsProperties[%i].largeMap"
                    },
                    "useLargeMap": {
                        "type": "property",
                        "external": "monsterGraphicsProperties[%i].useLargeMap"
                    },
                    "palette": {
                        "type": "property",
                        "external": "monsterGraphicsProperties[%i].palette"
                    },
                    "verticalOffset": {
                        "type": "assembly",
                        "name": "Vertical Offset",
                        "external": "monsterVerticalOffset[%i]"
                    },
                    "colosseumType": {
                        "type": "assembly",
                        "name": "Colosseum Type",
                        "external": "monsterColosseumType[%i]",
                        "invalid": "this.parent.i > 0xFF"
                    }
                }
            }
        },
        "monsterScript": {
            "type": "script",
            "name": "Monster Script",
            "range": "0xCF8700-0xCFC049",
            "encoding": "monster",
            "reference": [
                {
                    "begin": "0xC21A45",
                    "mask": "0xFFFFFF"
                }, {
                    "begin": "0xC21A65",
                    "mask": "0xFFFFFF",
                    "offset": -2
                }, {
                    "begin": "0xC21A6C",
                    "mask": "0xFFFFFF"
                }, {
                    "begin": "0xC22D89",
                    "mask": "0xFFFFFF"
                }
            ]
        },
        "monsterScriptPointers": {
            "type": "array",
            "name": "Script Pointer",
            "range": "0xCF8400-0xCF8700",
            "array": {
                "length": 384
            },
            "assembly": {
                "type": "property",
                "name": "Script Pointer",
                "mask": "0xFFFF",
                "length": 2,
                "script": "monsterScript"
            },
            "reference": [
                {
                    "begin": "0xC22D79",
                    "mask": "0xFFFFFF"
                }
            ]
        },
        "monsterVerticalOffset": {
            "type": "array",
            "name": "Monster Vertical Offset",
            "range": "0xCF3600-0xCF3780",
            "array": {
                "length": 384
            },
            "assembly": {
                "type": "property",
                "name": "Vertical Offset",
                "length": 1
            }
        },
        "naturalMagic": {
            "type": "array",
            "name": "Natural Magic",
            "range": "0xECE3C0-0xECE400",
            "array": {
                "length": 2
            },
            "assembly": {
                "type": "data",
                "length": 32,
                "assembly": {
                    "cat-spell1": "Spell 1",
                    "spell1": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 0,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level1": {
                        "type": "property",
                        "name": "Level",
                        "begin": 1,
                        "max": 99
                    },
                    "cat-spell2": "Spell 2",
                    "spell2": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 2,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level2": {
                        "type": "property",
                        "name": "Level",
                        "begin": 3,
                        "max": 99
                    },
                    "cat-spell3": "Spell 3",
                    "spell3": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 4,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level3": {
                        "type": "property",
                        "name": "Level",
                        "begin": 5,
                        "max": 99
                    },
                    "cat-spell4": "Spell 4",
                    "spell4": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 6,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level4": {
                        "type": "property",
                        "name": "Level",
                        "begin": 7,
                        "max": 99
                    },
                    "cat-spell5": "Spell 5",
                    "spell5": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 8,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level5": {
                        "type": "property",
                        "name": "Level",
                        "begin": 9,
                        "max": 99
                    },
                    "cat-spell6": "Spell 6",
                    "spell6": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 10,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level6": {
                        "type": "property",
                        "name": "Level",
                        "begin": 11,
                        "max": 99
                    },
                    "cat-spell7": "Spell 7",
                    "spell7": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 12,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level7": {
                        "type": "property",
                        "name": "Level",
                        "begin": 13,
                        "max": 99
                    },
                    "cat-spell8": "Spell 8",
                    "spell8": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 14,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level8": {
                        "type": "property",
                        "name": "Level",
                        "begin": 15,
                        "max": 99
                    },
                    "cat-spell9": "Spell 9",
                    "spell9": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 16,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level9": {
                        "type": "property",
                        "name": "Level",
                        "begin": 17,
                        "max": 99
                    },
                    "cat-spell10": "Spell 10",
                    "spell10": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 18,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level10": {
                        "type": "property",
                        "name": "Level",
                        "begin": 19,
                        "max": 99
                    },
                    "cat-spell11": "Spell 11",
                    "spell11": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 20,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level11": {
                        "type": "property",
                        "name": "Level",
                        "begin": 21,
                        "max": 99
                    },
                    "cat-spell12": "Spell 12",
                    "spell12": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 22,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level12": {
                        "type": "property",
                        "name": "Level",
                        "begin": 23,
                        "max": 99
                    },
                    "cat-spell13": "Spell 13",
                    "spell13": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 24,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level13": {
                        "type": "property",
                        "name": "Level",
                        "begin": 25,
                        "max": 99
                    },
                    "cat-spell14": "Spell 14",
                    "spell14": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 26,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level14": {
                        "type": "property",
                        "name": "Level",
                        "begin": 27,
                        "max": 99
                    },
                    "cat-spell15": "Spell 15",
                    "spell15": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 28,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level15": {
                        "type": "property",
                        "name": "Level",
                        "begin": 29,
                        "max": 99
                    },
                    "cat-spell16": "Spell 16",
                    "spell16": {
                        "type": "property",
                        "name": "Spell",
                        "begin": 30,
                        "stringTable": "spellNames",
                        "link": "attackProperties[%i]"
                    },
                    "level16": {
                        "type": "property",
                        "name": "Level",
                        "begin": 31,
                        "max": 99
                    }
                }
            },
            "stringTable": {
                "string": {
                    "0": "<characterNames[0]>",
                    "1": "<characterNames[6]>"
                }
            }
        },
        "npcProperties": {
            "type": "array",
            "name": "NPC Properties",
            "range": "0xC41D52-0xC46A6B",
            "isSequential": true,
            "endPointer": true,
            "canRelocate": true,
            "array": {
                "length": 415
            },
            "assembly": {
                "type": "array",
                "assembly": {
                    "type": "data",
                    "length": 9,
                    "assembly": {
                        "x": {
                            "type": "property",
                            "name": "X Position",
                            "begin": 4,
                            "mask": "0x7F",
                            "hidden": true
                        },
                        "y": {
                            "type": "property",
                            "name": "Y Position",
                            "begin": 5,
                            "mask": "0x3F",
                            "hidden": true
                        },
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 2,
                            "mask": "0xFFC0",
                            "stringTable": "npcSwitches"
                        },
                        "cat-behavior": "Behavior",
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 0,
                            "mask": "0x03FFFF",
                            "script": "eventScript",
                            "invalid": "this.parent.vehicle.value === 0 && this.parent.specialNPC.value"
                        },
                        "movement": {
                            "type": "property",
                            "name": "Movement",
                            "begin": 7,
                            "mask": "0x0F",
                            "stringTable": {
                                "string": {
                                    "0": "None",
                                    "1": "Script",
                                    "2": "User",
                                    "3": "Random"
                                }
                            }
                        },
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 5,
                            "mask": "0xC0",
                            "stringTable": {
                                "string": {
                                    "0": "Very Slow",
                                    "1": "Slow",
                                    "2": "Normal",
                                    "3": "Fast"
                                }
                            }
                        },
                        "reaction": {
                            "type": "property",
                            "name": "Reaction",
                            "begin": 8,
                            "mask": "0x04",
                            "stringTable": {
                                "string": {
                                    "0": "Face Party",
                                    "1": "None"
                                }
                            },
                            "invalid": "this.parent.vehicle.value === 0 && this.parent.specialNPC.value"
                        },
                        "slave": {
                            "type": "property",
                            "name": "Slave NPC",
                            "begin": 2,
                            "mask": "0x02",
                            "bool": true,
                            "invalid": "this.parent.vehicle.value !== 0 || !this.parent.specialNPC.value"
                        },
                        "master": {
                            "type": "property",
                            "name": "Master NPC",
                            "begin": 1,
                            "mask": "0x1F",
                            "disabled": "!this.parent.slave.value",
                            "invalid": "this.parent.vehicle.value !== 0 || !this.parent.specialNPC.value"
                        },
                        "offset": {
                            "type": "property",
                            "name": "Offset",
                            "begin": 1,
                            "mask": "0xE0",
                            "invalid": "this.parent.vehicle.value !== 0 || !this.parent.specialNPC.value"
                        },
                        "offsetDirection": {
                            "type": "property",
                            "name": "Offset Direction",
                            "begin": 2,
                            "mask": "0x01",
                            "stringTable": {
                                "string": {
                                    "0": "Right",
                                    "1": "Down"
                                }
                            },
                            "invalid": "this.parent.vehicle.value !== 0 || !this.parent.specialNPC.value"
                        },
                        "cat-appearance": "Appearance",
                        "graphics": {
                            "type": "property",
                            "name": "Graphics",
                            "begin": 6,
                            "stringTable": "mapSprites"
                        },
                        "palette": {
                            "type": "property",
                            "name": "Palette",
                            "begin": 2,
                            "mask": "0x1C",
                            "stringTable": {
                                "length": 8,
                                "default": "<stringTable.mapSpritePalettes[%i]>"
                            }
                        },
                        "vehicle": {
                            "type": "property",
                            "name": "Vehicle",
                            "begin": 7,
                            "mask": "0xC0",
                            "stringTable": {
                                "string": {
                                    "0": "None",
                                    "1": "Chocobo",
                                    "2": "Magitek Armor",
                                    "3": "Raft"
                                }
                            },
                            "disabled": "this.parent.animation.value !== 0",
                            "invalid": "this.parent.vehicle.value === 0 && this.parent.specialNPC.value"
                        },
                        "showRider": {
                            "type": "property",
                            "name": "Show Rider",
                            "begin": 4,
                            "mask": "0x80",
                            "bool": true,
                            "disabled": "this.parent.vehicle.value === 0",
                            "invalid": "this.parent.vehicle.value === 0 && this.parent.specialNPC.value"
                        },
                        "direction": {
                            "type": "property",
                            "name": "Facing Direction",
                            "begin": 8,
                            "mask": "0x03",
                            "stringTable": "directions",
                            "disabled": "this.parent.animation.value !== 0"
                        },
                        "specialNPC": {
                            "type": "property",
                            "name": "Special Graphics",
                            "begin": 4,
                            "mask": "0x80",
                            "bool": true,
                            "hidden": true,
                            "invalid": "this.parent.vehicle.value !== 0 || !this.parent.specialNPC.value"
                        },
                        "vramAddress": {
                            "type": "property",
                            "name": "VRAM Address",
                            "begin": 0,
                            "mask": "0x7F",
                            "invalid": "this.parent.vehicle.value !== 0 || !this.parent.specialNPC.value"
                        },
                        "is32x32": {
                            "type": "property",
                            "name": "Size",
                            "begin": 8,
                            "mask": "0x04",
                            "stringTable": {
                                "string": {
                                    "0": "16×16",
                                    "1": "32×32"
                                }
                            },
                            "invalid": "this.parent.vehicle.value !== 0 || !this.parent.specialNPC.value"
                        },
                        "hFlip": {
                            "type": "property",
                            "name": "Horizontal Flip",
                            "begin": 0,
                            "mask": "0x80",
                            "bool": true,
                            "invalid": "this.parent.vehicle.value !== 0 || !this.parent.specialNPC.value"
                        },
                        "spritePriority": {
                            "type": "property",
                            "name": "Sprite Priority",
                            "begin": 7,
                            "mask": "0x30",
                            "stringTable": {
                                "string": {
                                    "0": "Normal",
                                    "1": "High",
                                    "2": "Low",
                                    "3": "Low"
                                }
                            }
                        },
                        "layerPriority": {
                            "type": "property",
                            "name": "Layer Priority",
                            "begin": 8,
                            "mask": "0x18",
                            "stringTable": {
                                "string": {
                                    "0": "Default",
                                    "1": "Top Sprite Only",
                                    "2": "Foreground",
                                    "3": "Background"
                                }
                            }
                        },
                        "layer2": {
                            "type": "property",
                            "name": "Scrolling Layer",
                            "begin": 2,
                            "mask": "0x20",
                            "stringTable": {
                                "string": {
                                    "0": "Layer 1",
                                    "1": "Layer 2"
                                }
                            }
                        },
                        "cat-animation": "Animation",
                        "animation": {
                            "type": "property",
                            "name": "Animation",
                            "begin": 8,
                            "mask": "0xE0",
                            "stringTable": {
                                "string": {
                                    "0": "Default",
                                    "1": "None",
                                    "2": "Special",
                                    "3": "Knocked Out",
                                    "4": "None",
                                    "5": "None",
                                    "6": "None",
                                    "7": "None"
                                }
                            }
                        },
                        "animationType": {
                            "type": "property",
                            "name": "Type",
                            "begin": 8,
                            "mask": "0x03",
                            "stringTable": {
                                "string": {
                                    "0": "One Frame",
                                    "1": "Flip Horizontally",
                                    "2": "Two Frames",
                                    "3": "Four Frames"
                                }
                            },
                            "disabled": "this.parent.animation.value === 0"
                        },
                        "animationSpeed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 7,
                            "mask": "0xC0",
                            "stringTable": {
                                "string": {
                                    "0": "Fastest",
                                    "1": "Fast",
                                    "2": "Medium",
                                    "3": "Slow"
                                }
                            },
                            "disabled": "this.parent.animation.value === 0"
                        }
                    }
                }
            },
            "pointerTable": {
                "range": "0xC41A10-0xC41D52",
                "offset": "0xC41A10",
                "canRelocate": true,
                "reference": [
                    {
                        "begin": "0xC052BD",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }, {
                        "begin": "0xC052C3",
                        "mask": "0xFFFFFF"
                    }
                ]
            },
            "reference": [
                {
                    "begin": "0xC052D5",
                    "mask": "0xFFFFFF",
                    "pointerOffset": true
                }, {
                    "begin": "0xC052DC",
                    "mask": "0xFFFFFF",
                    "offset": -1,
                    "pointerOffset": true
                }, {
                    "begin": "0xC052E3",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xC052EC",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xC052F9",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xC05306",
                    "mask": "0xFFFFFF",
                    "offset": -2,
                    "pointerOffset": true
                }, {
                    "begin": "0xC05322",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }, {
                    "begin": "0xC0532D",
                    "mask": "0xFFFFFF",
                    "offset": -4,
                    "pointerOffset": true
                }, {
                    "begin": "0xC0533B",
                    "mask": "0xFFFFFF",
                    "offset": -5,
                    "pointerOffset": true
                }, {
                    "begin": "0xC0535B",
                    "mask": "0xFFFFFF",
                    "offset": -5,
                    "pointerOffset": true
                }, {
                    "begin": "0xC0536A",
                    "mask": "0xFFFFFF",
                    "offset": -6,
                    "pointerOffset": true
                }, {
                    "begin": "0xC05374",
                    "mask": "0xFFFFFF",
                    "offset": -7,
                    "pointerOffset": true
                }, {
                    "begin": "0xC05380",
                    "mask": "0xFFFFFF",
                    "offset": -7,
                    "pointerOffset": true
                }, {
                    "begin": "0xC0538B",
                    "mask": "0xFFFFFF",
                    "offset": -7,
                    "pointerOffset": true
                }, {
                    "begin": "0xC05398",
                    "mask": "0xFFFFFF",
                    "offset": -8,
                    "pointerOffset": true
                }, {
                    "begin": "0xC053AE",
                    "mask": "0xFFFFFF",
                    "offset": -8,
                    "pointerOffset": true
                }, {
                    "begin": "0xC053BD",
                    "mask": "0xFFFFFF",
                    "offset": -8,
                    "pointerOffset": true
                }, {
                    "begin": "0xC053D1",
                    "mask": "0xFFFFFF",
                    "offset": -8,
                    "pointerOffset": true
                }, {
                    "begin": "0xC053E4",
                    "mask": "0xFFFFFF",
                    "offset": -8,
                    "pointerOffset": true
                }
            ]
        },
        "portraitGraphics": {
            "type": "array",
            "name": "Character Portrait Graphics",
            "range": "0xED1D00-0xED5860",
            "array": {
                "length": 19
            },
            "assembly": {
                "type": "graphics",
                "length": "0x0320",
                "format": "snes4bpp",
                "palette": "portraitPalette[%i]",
                "width": 5,
                "height": 5,
                "spriteSheet": {
                    "tilemap": [
                         0, 1, 2, 3, 8,
                        16,17,18,19, 9,
                         4, 5, 6, 7,10,
                        20,21,22,23,11,
                        13,14,15,24,12
                    ]
                }
            },
            "stringTable": {
                "string": {
                    "0": "Terra",
                    "1": "Locke",
                    "2": "Cyan",
                    "3": "Shadow",
                    "4": "Edgar",
                    "5": "Sabin",
                    "6": "Celes",
                    "7": "Strago",
                    "8": "Relm",
                    "9": "Setzer",
                    "10": "Mog",
                    "11": "Gau",
                    "12": "Gogo",
                    "13": "Umaro",
                    "14": "Soldier",
                    "15": "Imp",
                    "16": "Leo",
                    "17": "Banon",
                    "18": "Ghost"
                }
            }
        },
        "portraitPalette": {
            "type": "array",
            "name": "Character Portrait Palette",
            "range": "0xED5860-0xED5AC0",
            "array": {
                "length": 19
            },
            "assembly": {
                "type": "assembly",
                "length": 32,
                "format": "bgr555"
            },
            "stringTable": "portraitGraphics"
        },
        "rareItemDescription": {
            "type": "array",
            "name": "Rare Item Description",
            "range": "0xCEFCB0-0xCEFEDF",
            "array": {
                "length": 20,
                "max": 20
            },
            "assembly": {
                "type": "text",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xCEFB60-0xCEFBA0",
                "offset": "0xCEFCB0"
            }
        },
        "rareItemName": {
            "type": "array",
            "name": "Rare Item Name",
            "range": "0xCEFBA0-0xCEFCB0",
            "array": {
                "length": 20
            },
            "assembly": {
                "type": "data",
                "length": 13,
                "assembly": {
                    "itemName": {
                        "type": "text",
                        "name": "Name",
                        "length": 13,
                        "encoding": "smallText"
                    },
                    "description": {
                        "type": "assembly",
                        "name": "Description",
                        "external": "rareItemDescription[%i]"
                    }
                }
            },
            "stringTable": {
                "length": 20,
                "default": "<rareItemName[%i].itemName>",
                "string": {
                    "2-6": "<rareItemDescription[%i]>"
                }
            }
        },
        "shopProperties": {
            "type": "array",
            "name": "Shop Properties",
            "range": "0xC47AC0-0xC47F40",
            "array": {
                "length": 128
            },
            "assembly": {
                "type": "data",
                "length": 9,
                "assembly": {
                    "shopType": {
                        "type": "property",
                        "name": "Type",
                        "begin": 0,
                        "mask": "0x07",
                        "stringTable": {
                            "string": {
                                "0": "None",
                                "1": "Weapon",
                                "2": "Armor",
                                "3": "Item",
                                "4": "Relics",
                                "5": "Vendor"
                            }
                        }
                    },
                    "priceAdjust": {
                        "type": "property",
                        "name": "Price Modifier",
                        "begin": 0,
                        "mask": "0x38",
                        "stringTable": {
                            "string": {
                                "0": "Normal",
                                "1": "+50%",
                                "2": "+100%",
                                "3": "-50%",
                                "4": "-50% female / +50% male",
                                "5": "-50% male / +50% female",
                                "6": "-50% for Edgar"
                            }
                        }
                    },
                    "item1": {
                        "type": "property",
                        "name": "Item 1",
                        "begin": 1,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "item2": {
                        "type": "property",
                        "name": "Item 2",
                        "begin": 2,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "item3": {
                        "type": "property",
                        "name": "Item 3",
                        "begin": 3,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "item4": {
                        "type": "property",
                        "name": "Item 4",
                        "begin": 4,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "item5": {
                        "type": "property",
                        "name": "Item 5",
                        "begin": 5,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "item6": {
                        "type": "property",
                        "name": "Item 6",
                        "begin": 6,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "item7": {
                        "type": "property",
                        "name": "Item 7",
                        "begin": 7,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    },
                    "item8": {
                        "type": "property",
                        "name": "Item 8",
                        "begin": 8,
                        "stringTable": "itemNames",
                        "link": "itemProperties[%i]"
                    }
                }
            },
            "stringTable": {
                "length": 128,
                "default": "Shop %i",
                "string": {
                    "0": "Narshe: Weapons (Before Vector)",
                    "1": "Narshe: Armor (Before Vector)",
                    "2": "Narshe: Relics (Before Vector)",
                    "3": "Narshe: Items (Before Vector)",
                    "4": "Figaro Castle: Items (1st Time)",
                    "5": "South Figaro: Weapons (WoB)",
                    "6": "South Figaro: Armor (WoB)",
                    "7": "South Figaro: Relics (WoB)",
                    "8": "South Figaro: Items (WoB)",
                    "9": "Mobliz: Weapons",
                    "10": "Mobliz: Armor",
                    "11": "Mobliz: Relics",
                    "12": "Mobliz: Items",
                    "13": "Nikeah: Weapons (WoB)",
                    "14": "Nikeah: Armor (WoB)",
                    "15": "Nikeah: Items (WoB)",
                    "16": "Nikeah: Relics (WoB)",
                    "17": "Kohlingen: Weapons (WoB)",
                    "18": "Kohlingen: Armor (WoB)",
                    "19": "Kohlingen: Items (WoB)",
                    "20": "Jidoor: Weapons (WoB)",
                    "21": "Jidoor: Armor (WoB)",
                    "22": "Jidoor: Items (WoB)",
                    "23": "Jidoor: Relics (WoB)",
                    "24": "Albrook: Items (WoB)",
                    "25": "Albrook: Weapons (WoB)",
                    "26": "Albrook: Armor (WoB)",
                    "27": "Vector: Weapons",
                    "28": "Vector: Armor",
                    "29": "Tzen: Weapons (WoB)",
                    "30": "Tzen: Armor (WoB)",
                    "31": "Tzen: Items (WoB)",
                    "32": "Tzen: Relics (WoB)",
                    "33": "Thamasa: Weapons (WoB)",
                    "34": "Thamasa: Armor (WoB)",
                    "35": "Thamasa: Items (WoB)",
                    "36": "Returner's Hideout: Items",
                    "37": "Maranda: Weapons (WoB)",
                    "38": "Maranda: Armor (WoB)",
                    "39": "Crazy Old Man's House: Vendor",
                    "40": "Albrook: Relics (WoB)",
                    "41": "Narshe: Weapons (After Vector)",
                    "42": "Narshe: Armor (After Vector)",
                    "43": "Narshe: Relics (After Vector)",
                    "44": "Narshe: Items (After Vector)",
                    "45": "Thamasa: Relics (WoB)",
                    "46": "Blackjack: Items",
                    "47": "Figaro Castle: Items (2nd Time)",
                    "48": "Albrook: Items (WoR)",
                    "49": "Albrook: Weapons (WoR)",
                    "50": "Albrook: Armor (WoR)",
                    "51": "Albrook: Relics (WoR)",
                    "52": "Tzen: Weapons (WoR)",
                    "53": "Tzen: Armor (WoR)",
                    "54": "Tzen: Items (WoR)",
                    "55": "Tzen: Relic (WoR)",
                    "56": "Nikeah: Weapons (WoR)",
                    "57": "Nikeah: Armor (WoR)",
                    "58": "Nikeah: Items (WoR)",
                    "59": "Nikeah: Relics (WoR)",
                    "60": "South Figaro: Weapons (WoR)",
                    "61": "South Figaro: Armor (WoR)",
                    "62": "South Figaro: Relics (WoR)",
                    "63": "South Figaro: Items (WoR)",
                    "64": "Figaro Castle: Items (WoR)",
                    "65": "Kohlingen: Weapons (WoR)",
                    "66": "Kohlingen: Armor (WoR)",
                    "67": "Kohlingen: Items (WoR)",
                    "68": "Narshe: Weapons (WoR)",
                    "69": "Narshe: Armor (WoR)",
                    "70": "Narshe: Relics (WoR)",
                    "71": "Narshe: Items (WoR)",
                    "72": "Thamasa: Weapons (WoR)",
                    "73": "Thamasa: Armor (WoR)",
                    "74": "Thamasa: Items (WoR)",
                    "75": "Thamasa: Relics (WoR)",
                    "76": "Jidoor: Weapons (WoR)",
                    "77": "Jidoor: Armor (WoR)",
                    "78": "Jidoor: Items (WoR)",
                    "79": "Jidoor: Relics (WoR)",
                    "80": "Maranda: Weapons (WoR)",
                    "81": "Maranda: Armor (WoR)",
                    "82": "Figaro Castle: Tools (1st Time)",
                    "83": "Figaro Castle: Tools (2nd Time)",
                    "84": "Figaro Castle: Tools (WoR)",
                    "85": "Phantom Train: Items",
                    "86": "Unused: Items"
                }
            }
        },
        "snesHeader": {
            "type": "data",
            "name": "SNES Header",
            "range": "0xC0FFB0-0xC0FFE0",
            "assembly": {
                "makerCode": {
                    "type": "text",
                    "name": "Maker Code",
                    "length": 2
                },
                "gameCode": {
                    "type": "text",
                    "name": "Game Code",
                    "begin": 2,
                    "length": 4
                },
                "gameTitle": {
                    "type": "text",
                    "name": "Game Title",
                    "begin": 16,
                    "length": 21
                },
                "expansionRAMSize": {
                    "type": "property",
                    "name": "Exp. RAM Size",
                    "begin": 13,
                    "stringTable": {
                        "string": {
                            "0": "None",
                            "1": "16 KBit",
                            "3": "64 KBit",
                            "5": "256 KBit",
                            "6": "512 KBit",
                            "7": "1 MBit"
                        }
                    }
                },
                "specialVersion": {
                    "type": "property",
                    "name": "Special Version",
                    "begin": 14
                },
                "cartridgeSubType": {
                    "type": "property",
                    "name": "Cartridge Sub-Type",
                    "begin": 15
                },
                "mapMode": {
                    "type": "property",
                    "name": "Map Mode",
                    "begin": 37,
                    "mask": "0x0F",
                    "stringTable": {
                        "string": {
                            "0": "Mode 20",
                            "1": "Mode 21",
                            "3": "Mode 23 (SA-1)",
                            "5": "Mode 25"
                        }
                    }
                },
                "cpuSpeed": {
                    "type": "property",
                    "name": "CPU Speed",
                    "begin": 37,
                    "mask": "0xF0",
                    "stringTable": {
                        "string": {
                            "2": "2.68 MHz (normal speed)",
                            "3": "3.58 MHz (high speed)"
                        }
                    }
                },
                "cartridgeType": {
                    "type": "property",
                    "name": "Cartridge Type",
                    "begin": 38,
                    "mask": "0x0F",
                    "stringTable": {
                        "string": {
                            "0": "ROM Only",
                            "1": "ROM + RAM",
                            "2": "ROM + RAM + Battery",
                            "3": "ROM + Co-processor",
                            "4": "ROM + Co-processor + RAM",
                            "5": "ROM + Co-processor + RAM + Battery",
                            "6": "ROM + Co-processor + Battery"
                        }
                    }
                },
                "coprocessorType": {
                    "type": "property",
                    "name": "Co-processor Type",
                    "begin": 38,
                    "mask": "0xF0",
                    "hidden": "this.parent.cartridgeType.value < 3",
                    "stringTable": {
                        "string": {
                            "0": "DSP",
                            "1": "Super FX",
                            "2": "OBC1",
                            "3": "SA-1",
                            "14": "Other",
                            "15": "Custom Chip"
                        }
                    }
                },
                "romSize": {
                    "type": "property",
                    "name": "ROM Size",
                    "begin": 39,
                    "stringTable": {
                        "string": {
                            "9": "3-4 MBit",
                            "10": "5-8 MBit",
                            "11": "9-16 MBit",
                            "12": "17-32 MBit",
                            "13": "33-64 MBit"
                        }
                    }
                },
                "ramSize": {
                    "type": "property",
                    "name": "RAM Size",
                    "begin": 40,
                    "stringTable": {
                        "string": {
                            "0": "None",
                            "1": "16 KBit",
                            "3": "64 KBit",
                            "5": "256 KBit",
                            "6": "512 KBit",
                            "7": "1 MBit"
                        }
                    }
                },
                "destinationCode": {
                    "type": "property",
                    "name": "Destination Code",
                    "begin": 41,
                    "stringTable": {
                        "string": {
                            "0": "Japan",
                            "1": "North America",
                            "2": "Europe",
                            "3": "Scandinavia",
                            "6": "French",
                            "7": "Dutch",
                            "8": "Spanish",
                            "9": "German",
                            "10": "Italian",
                            "11": "Chinese",
                            "13": "Korean",
                            "14": "Common",
                            "15": "Canada",
                            "16": "Brazil",
                            "17": "Australia"
                        }
                    }
                },
                "fixedValue": {
                    "type": "property",
                    "name": "Fixed Value",
                    "begin": 42
                },
                "romVersion": {
                    "type": "property",
                    "name": "ROM Version",
                    "begin": 43
                },
                "checksumInverse": {
                    "type": "property",
                    "name": "Checksum Inverse",
                    "begin": 44,
                    "mask": "0xFFFF",
                    "hidden": true
                },
                "checksum": {
                    "type": "property",
                    "name": "Checksum",
                    "begin": 46,
                    "hidden": true,
                    "mask": "0xFFFF"
                }
            }
        },
        "spellDescriptions": {
            "type": "array",
            "name": "Spell Descriptions",
            "range": "0xD8C9A0-0xD8CEA0",
            "arrayLength": 54,
            "assembly": {
                "type": "text",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xD8CF80-0xD8D000",
                "offset": "0xD8C9A0"
            }
        },
        "spellNames": {
            "type": "array",
            "name": "Spell Name",
            "range": "0xE6F567-0xE6F6E1",
            "array": {
                "length": 54
            },
            "assembly": {
                "type": "text",
                "encoding": "smallText",
                "length": 7
            },
            "stringTable": {
                "length": 54,
                "default": "<spellNames[%i]>",
                "string": {
                    "255": "None"
                }
            }
        },
        "statusNames": {
            "type": "array",
            "name": "Status Name",
            "range": "0xC2ADE1-0xC2AF21",
            "array": {
                "length": 32
            },
            "assembly": {
                "type": "text",
                "length": 10,
                "encoding": "smallText"
            },
            "stringTable": {
                "length": 32,
                "default": "<statusNames[%i]>",
                "string": {
                    "4": "Magitek",
                    "8": "Sleep",
                    "9": "HP Leak",
                    "23": "Dance",
                    "25": "Interceptor",
                    "26": "Hidden",
                    "27": "Chant",
                    "28": "Morph",
                    "30": "Freeze",
                    "31": "Rage"
                }
            }
        },
        "swdTechDescription": {
            "type": "array",
            "name": "SwdTech Description",
            "range": "0xCFFD00-0xCFFE00",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "text",
                "name": "Description",
                "encoding": "nullTerminated",
                "multiLine": true
            },
            "pointerTable": {
                "range": "0xCFFFAE-0xCFFFBE",
                "offset": "0xCFFD00"
            }
        },
        "swdTechLevel": {
            "type": "array",
            "name": "SwdTech Level",
            "range": "0xE6F490-0xE6F498",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "data",
                "length": 1,
                "assembly": {
                    "level": {
                        "type": "property",
                        "name": "Learn at Level",
                        "max": 99
                    }
                }
            }
        },
        "swdTechNames": {
            "type": "array",
            "name": "SwdTech Name",
            "range": "0xCF3C40-0xCF3D00",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "text",
                "encoding": "smallText",
                "length": 12
            },
            "stringTable": {
                "length": 8,
                "default": "<swdTechNames[%i]>"
            }
        },
        "theEndGraphics1": {
            "type": "graphics",
            "name": "\"The End\" Graphics 1",
            "range": "0xE96300-0xE97000",
            "format": "snes4bpp",
            "palette": "theEndPalette"
        },
        "theEndGraphics2": {
            "type": "graphics",
            "name": "\"The End\" Graphics 2",
            "range": "0xC9FE00-0xC9FF00",
            "format": "snes4bpp",
            "palette": "theEndPalette",
            "width": 8
        },
        "theEndPalette": {
            "type": "graphics",
            "name": "\"The End\" Palette",
            "range": "0xC9FF00-0xCA0000",
            "format": "bgr555"
        },
        "titleIntroGraphics": {
            "type": "data",
            "name": "Title/Intro Cinematic Graphics Data",
            "range": "0xD8F000-0xD94E95",
            "format": "ff6-lzss",
            "assembly": {
                "magitekSpriteGraphics": {
                    "type": "graphics",
                    "name": "Magitek March Sprite Graphics",
                    "range": "0x0000-0x0940",
                    "format": "snes4bpp",
                    "palette": {
                        "path": "cinematicProgram.magitekPalette",
                        "range": "48-64"
                    }
                },
                "magitekBGGraphics": {
                    "type": "graphics",
                    "name": "Magitek March Background Graphics",
                    "range": "0x0940-0x0BC0",
                    "format": "snes4bpp",
                    "palette": "cinematicProgram.magitekPalette"
                },
                "magitekLayout": {
                    "type": "tilemap",
                    "name": "Magitek March Tile Layout",
                    "range": "0x0BC0-0x1BC0",
                    "format": ["generic4bppTile", "interlace(32,2,32)"],
                    "graphics": [[
                        {
                            "path": "mapGraphics[34]",
                            "offset": "0x0000",
                            "range": "0x0000-0x1000"
                        }, {
                            "path": "mapGraphics[31]",
                            "offset": "0x1000",
                            "range": "0x1000-0x2000"
                        }, {
                            "path": "mapGraphics[32]",
                            "offset": "0x2000",
                            "range": "0x0000-0x1000"
                        }, {
                            "path": "titleIntroGraphics.magitekBGGraphics",
                            "offset": "0x3000"
                        }
                    ]],
                    "palette": {
                        "path": "cinematicProgram.magitekPalette",
                        "offset": "0x10"
                    },
                    "colorOffset": {
                        "path": "cinematicProgram.magitekColorOffset",
                        "multiplier": 16,
                        "perTile": true
                    },
                    "width": 64,
                    "height": 64
                },
                "introLayout": {
                    "type": "tilemap",
                    "name": "Intro Tile Layout",
                    "range": "0x1BC0-0x33C0",
                    "format": ["snes4bppTile", "interlace(2048,2,2)"],
                    "graphics": [[
                        {
                            "path": "mapGraphics[75]",
                            "offset": "0x0000"
                        }, {
                            "path": "titleIntroGraphics.introGraphics",
                            "offset": "0x4000"
                        }
                    ]],
                    "palette": [[
                        {
                            "path": "cinematicProgram.introBackgroundPalette",
                            "offset": "0x20",
                            "range": "0x60-0x70"
                        }, {
                            "path": "cinematicProgram.introBackgroundPalette",
                            "offset": "0x30",
                            "range": "0x60-0x70"
                        }, {
                            "path": "cinematicProgram.introBackgroundPalette",
                            "offset": "0x50",
                            "range": "0x40-0x50"
                        }, {
                            "path": "cinematicProgram.introBackgroundPalette",
                            "offset": "0x60",
                            "range": "0x50-0x60"
                        }, {
                            "path": "cinematicProgram.introBackgroundPalette",
                            "offset": "0x70",
                            "range": "0x70-0x80"
                        }
                    ]],
                    "width": 32,
                    "height": 96
                },
                "introGraphics": {
                    "type": "graphics",
                    "name": "Intro Background Graphics",
                    "range": "0x33C0-0x75C0",
                    "format": "snes4bpp",
                    "palette": "cinematicProgram.introBackgroundPalette"
                },
                "introFontGraphics": {
                    "type": "graphics",
                    "name": "Intro Font Graphics",
                    "range": "0x75C0-0x7930",
                    "format": "snes2bpp",
                    "palette": "cinematicProgram.introFontPalette"
                },
                "flames2bppLayout": {
                    "type": "tilemap",
                    "name": "Intro Flame Tile Layout (2bpp)",
                    "range": "0x7930-0x7B30",
                    "format": "snes2bppTile",
                    "graphics": "titleIntroGraphics.flames2bpp",
                    "palette": {
                        "path": "cinematicProgram.introBackgroundPalette",
                        "range": "0x24-0x28"
                    },
                    "width": 32,
                    "height": 8
                },
                "flames2bpp": {
                    "type": "graphics",
                    "name": "Intro Flame Graphics (2bpp)",
                    "range": "0x7B30-0x81E0",
                    "format": "snes2bpp",
                    "palette": "cinematicProgram.introBackgroundPalette"
                },
                "flames4bppLayout": {
                    "type": "tilemap",
                    "name": "Intro Flame Tile Layout (4bpp)",
                    "range": "0x81E0-0x83E0",
                    "format": "snes4bppTile",
                    "graphics": "titleIntroGraphics.flames4bpp",
                    "palette": {
                        "path": "cinematicProgram.introBackgroundPalette",
                        "range": "0x10-0x20"
                    },
                    "width": 32,
                    "height": 8
                },
                "flames4bpp": {
                    "type": "graphics",
                    "name": "Intro Flame Graphics (4bpp)",
                    "range": "0x83E0-0x8BE0",
                    "format": "snes4bpp",
                    "palette": "cinematicProgram.introBackgroundPalette"
                },
                "splashLayout": {
                    "type": "tilemap",
                    "name": "Splash Screen Tile Layout",
                    "range": "0x8BE0-0x8DE0",
                    "format": "snes4bppTile",
                    "graphics": "titleIntroGraphics.splashBackground",
                    "palette": "cinematicProgram.splashPalette",
                    "width": 32,
                    "height": 8
                },
                "splashBackground": {
                    "type": "graphics",
                    "name": "Splash Screen Background Graphics",
                    "range": "0x8DE0-0x9EE0",
                    "format": "snes4bpp",
                    "palette": "cinematicProgram.splashPalette"
                },
                "splashSprites": {
                    "type": "graphics",
                    "name": "Intro/Splash Sprite Graphics",
                    "range": "0x9EF0-0xAEA8",
                    "format": "snes4bpp",
                    "palette": "cinematicProgram.introSpritePalette"
                }
            }
        },
        "treasureProperties": {
            "type": "array",
            "name": "Treasure Properties",
            "range": "0xED8634-0xED8BCA",
            "isSequential": true,
            "endPointer": true,
            "canRelocate": true,
            "array": {
                "length": 415
            },
            "assembly": {
                "type": "array",
                "assembly": {
                    "type": "data",
                    "length": 5,
                    "assembly": {
                        "x": {
                            "type": "property",
                            "name": "X Position",
                            "begin": 0,
                            "hidden": true
                        },
                        "y": {
                            "type": "property",
                            "name": "Y Position",
                            "begin": 1,
                            "hidden": true
                        },
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 2,
                            "mask": "0x01FF",
                            "stringTable": "treasureSwitches"
                        },
                        "treasureType": {
                            "type": "property",
                            "name": "Type",
                            "begin": 3,
                            "mask": "0xF0",
                            "stringTable": {
                                "name": "Treasure Types",
                                "string": {
                                    "0": "Empty",
                                    "2": "Monster-in-a-box",
                                    "4": "Item",
                                    "8": "GP"
                                }
                            }
                        },
                        "battle": {
                            "type": "property",
                            "name": "Battle",
                            "begin": 4,
                            "link": "battleGroupsEvent[%i]",
                            "stringTable": "battleGroupsEvent",
                            "invalid": "this.parent.treasureType.value !== 2"
                        },
                        "item": {
                            "type": "property",
                            "name": "Item",
                            "begin": 4,
                            "stringTable": "itemNames",
                            "link": "itemProperties[%i]",
                            "invalid": "this.parent.treasureType.value !== 4"
                        },
                        "gp": {
                            "type": "property",
                            "name": "GP",
                            "begin": 4,
                            "multiplier": 100,
                            "invalid": "this.parent.treasureType.value !== 8"
                        }
                    }
                }
            },
            "pointerTable": {
                "range": "0xED82F4-0xED8634",
                "offset": "0xED8634",
                "canRelocate": true,
                "reference": [
                    {
                        "begin": "0xC015DE",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }, {
                        "begin": "0xC015E4",
                        "mask": "0xFFFFFF"
                    }, {
                        "begin": "0xC04BDB",
                        "mask": "0xFFFFFF",
                        "offset": -2
                    }, {
                        "begin": "0xC04BE1",
                        "mask": "0xFFFFFF"
                    }
                ]
            },
            "reference": [
                {
                    "begin": "0xC015F2",
                    "mask": "0xFFFFFF"
                }, {
                    "begin": "0xC015F8",
                    "mask": "0xFFFFFF",
                    "offset": -1
                }, {
                    "begin": "0xC015FF",
                    "mask": "0xFFFFFF",
                    "offset": -2
                }, {
                    "begin": "0xC0160A",
                    "mask": "0xFFFFFF",
                    "offset": -2
                }, {
                    "begin": "0xC04BED",
                    "mask": "0xFFFFFF"
                }, {
                    "begin": "0xC04BF5",
                    "mask": "0xFFFFFF",
                    "offset": -1
                }, {
                    "begin": "0xC04C09",
                    "mask": "0xFFFFFF",
                    "offset": -4
                }, {
                    "begin": "0xC04C0F",
                    "mask": "0xFFFFFF",
                    "offset": -2
                }
            ]
        },
        "vectorApproachGraphics": {
            "type": "graphics",
            "name": "Vector Approach Graphics",
            "range": "0xD8DFB8-0xD8E5BF",
            "format": ["snes4bpp", "ff6-lzss"],
            "palette": "vectorApproachPalette"
        },
        "vectorApproachLayout": {
            "type": "tilemap",
            "name": "Vector Approach Tile Layout",
            "range": "0xD8E5BF-0xD8E6BA",
            "format": ["snes4bppTile", "ff6-lzss"],
            "graphics": "vectorApproachGraphics",
            "palette": {
                "path": "vectorApproachPalette",
                "offset": "0x70"
            },
            "width": 32,
            "height": 16
        },
        "vectorApproachPalette": {
            "type": "assembly",
            "name": "Vector Approach Palette",
            "range": "0xEFCE77-0xEFCE97",
            "format": "bgr555"
        },
        "vehicleGraphics": {
            "type": "graphics",
            "name": "Vehicle Graphics",
            "range": "0xD83000-0xD85000",
            "format": "snes4bpp",
            "palette": ["mapSpritePalettes[7]", "mapSpritePalettes[11]"]
        },
        "vehicleEvents": {
            "type": "array",
            "name": "Vehicle Event",
            "range": "0xEEB269-0xEEB27E",
            "array": {
                "length": 7
            },
            "assembly": {
                "type": "data",
                "assembly": {
                    "scriptPointer": {
                        "type": "property",
                        "name": "Script Pointer",
                        "mask": "0xFFFFFF",
                        "script": "eventScript"
                    }
                },
                "length": 3
            },
            "stringTable": {
                "length": 7,
                "default": "Vehicle Event %i",
                "string": {
                    "0": "Blackjack: Deck",
                    "1": "Tent (World)",
                    "2": "Blackjack: Ground Entrance",
                    "3": "Phoenix Cave (World)",
                    "4": "Kefka's Tower (World)",
                    "5": "Gogo's Lair",
                    "6": "Doom Gaze (World)"
                }
            }
        },
        "windowGraphics": {
            "type": "array",
            "name": "Menu Window Graphics",
            "range": "0xED0000-0xED1C00",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "graphics",
                "length": "0x0380",
                "format": "snes4bpp",
                "palette": "windowPalette[%i]",
                "width": 4,
                "spriteSheet": {
                    "width": 6,
                    "height": 6,
                    "tilemap": [
                        16,17,18,17,18,19,
                        20, 0, 1, 2, 3,21,
                        22, 4, 5, 6, 7,23,
                        20, 8, 9,10,11,21,
                        22,12,13,14,15,23,
                        24,25,26,25,26,27
                    ]
                }
            }
        },
        "windowPalette": {
            "type": "array",
            "name": "Menu Window Palette",
            "range": "0xED1C00-0xED1D00",
            "array": {
                "length": 8
            },
            "assembly": {
                "type": "assembly",
                "length": 32,
                "format": "bgr555"
            }
        },
        "worldBattleGroup": {
            "type": "array",
            "name": "World Map Battle Group",
            "range": "0xCF5400-0xCF5600",
            "array": {
                "length": 128
            },
            "assembly": {
                "type": "data",
                "length": 4,
                "assembly": {
                    "grass": {
                        "type": "property",
                        "name": "Field (Grass)",
                        "begin": 0,
                        "link": "battleGroupsRandom[%i]",
                        "stringTable": "battleGroupsRandom"
                    },
                    "forest": {
                        "type": "property",
                        "name": "Forest",
                        "begin": 1,
                        "link": "battleGroupsRandom[%i]",
                        "stringTable": "battleGroupsRandom"
                    },
                    "desert": {
                        "type": "property",
                        "name": "Desert",
                        "begin": 2,
                        "link": "battleGroupsRandom[%i]",
                        "stringTable": "battleGroupsRandom"
                    },
                    "dirt": {
                        "type": "property",
                        "name": "Field (Dirt)",
                        "begin": 3,
                        "link": "battleGroupsRandom[%i]",
                        "stringTable": "battleGroupsRandom"
                    },
                    "probability": {
                        "type": "assembly",
                        "external": "worldBattleProbability[%i]"
                    }
                }
            }
        },
        "worldBattleProbability": {
            "type": "array",
            "name": "World Map Battle Probability",
            "range": "0xCF5800-0xCF5880",
            "format": "linear2bpp",
            "array": {
                "length": 128
            },
            "assembly": {
                "type": "property",
                "name": "Probability",
                "length": 1,
                "mask": "0x03",
                "stringTable": {
                    "string": {
                        "0": "Normal",
                        "1": "Low",
                        "2": "High",
                        "3": "None"
                    }
                }
            }
        },
        "worldCloudsGraphics": {
            "type": "graphics",
            "name": "World Map Clouds Graphics",
            "range": "0xEEB290-0xEEC295",
            "format": ["snes4bpp", "ff6-lzss"],
            "palette": {
                "path": ["worldPalette1", "worldPalette2"]
            }
        },
        "worldCloudsLayout": {
            "type": "tilemap",
            "name": "World Map Clouds Layout",
            "range": "0xEEC295-0xEEC702",
            "format": ["snes4bppTile", "interlace(64,2,32)", "ff6-lzss"],
            "graphics": "worldCloudsGraphics",
            "palette": [
                {
                    "path": "worldPalette1",
                    "range": "0-16"
                }, {
                    "path": "worldPalette2",
                    "range": "0-16"
                }, {
                    "path": "worldPalette3",
                    "range": "192-208"
                }
            ],
            "width": 64,
            "height": 32
        },
        "worldGraphics1": {
            "type": "data",
            "name": "World of Balance Graphics Data",
            "range": "0xEF114F-0xEF3250",
            "format": "ff6-lzss",
            "isSequential": true,
            "assembly": {
                "tileset": {
                    "type": "tilemap",
                    "name": "World of Balance Tileset",
                    "range": "0x0000-0x0400",
                    "format": ["generic4bppTile", "interlace(2,16,2)"],
                    "width": 32,
                    "height": 32,
                    "graphics": "worldGraphics1.graphics",
                    "palette": "worldPalette1",
                    "colorOffset": {
                        "path": "worldGraphics1.paletteAssignment",
                        "multiplier": 16,
                        "perTile": true
                    }
                },
                "graphics": {
                    "type": "graphics",
                    "name": "World of Balance Graphics",
                    "range": "0x0400-0x2400",
                    "format": "linear4bpp",
                    "palette": "worldPalette1"
                },
                "paletteAssignment": {
                    "type": "assembly",
                    "name": "World of Balance Palette Assignment",
                    "range": "0x2400-0x2480",
                    "format": "linear4bpp"
                }
            }
        },
        "worldGraphics2": {
            "type": "data",
            "name": "World of Ruin Graphics Data",
            "range": "0xEF4A46-0xEF6A56",
            "format": "ff6-lzss",
            "isSequential": true,
            "assembly": {
                "tileset": {
                    "type": "tilemap",
                    "name": "World of Ruin Tileset",
                    "range": "0x0000-0x0400",
                    "format": ["generic4bppTile", "interlace(2,16,2)"],
                    "width": 32,
                    "height": 32,
                    "graphics": "worldGraphics2.graphics",
                    "palette": "worldPalette2",
                    "colorOffset": {
                        "path": "worldGraphics2.paletteAssignment",
                        "multiplier": 16,
                        "perTile": true
                    }
                },
                "graphics": {
                    "type": "graphics",
                    "name": "World of Ruin Graphics",
                    "range": "0x0400-0x2400",
                    "format": "linear4bpp",
                    "palette": "worldPalette2"
                },
                "paletteAssignment": {
                    "type": "assembly",
                    "name": "World of Ruin Palette Assignment",
                    "range": "0x2400-0x2480",
                    "format": "linear4bpp"
                }
            }
        },
        "worldGraphics3": {
            "type": "data",
            "name": "Serpent Trench Graphics Data",
            "range": "0xEFB631-0xEFC624",
            "format": "ff6-lzss",
            "isSequential": true,
            "assembly": {
                "tileset": {
                    "type": "tilemap",
                    "name": "Serpent Trench Tileset",
                    "range": "0x0000-0x0400",
                    "format": ["generic4bppTile", "interlace(2,16,2)"],
                    "width": 32,
                    "height": 32,
                    "graphics": "worldGraphics3.graphics",
                    "palette": {
                        "path": "worldPalette3",
                        "range": "0-128"
                    },
                    "colorOffset": {
                        "path": "worldGraphics3.paletteAssignment",
                        "multiplier": 16,
                        "perTile": true
                    }
                },
                "graphics": {
                    "type": "graphics",
                    "name": "Serpent Trench Graphics",
                    "range": "0x0400-0x2400",
                    "format": "linear4bpp",
                    "palette": "worldPalette3"
                },
                "paletteAssignment": {
                    "type": "assembly",
                    "name": "Serpent Trench Palette Assignment",
                    "range": "0x2400-0x2480",
                    "format": "linear4bpp"
                }
            }
        },
        "worldLayout1": {
            "type": "assembly",
            "name": "World of Balance Layout",
            "range": "0xEED434-0xEF114F",
            "format": "ff6-lzss",
            "canRelocate": true,
            "reference": [
                {
                    "begin": "0xEEB20F",
                    "mask": "0xFFFFFF"
                }
            ]
        },
        "worldLayout2": {
            "type": "assembly",
            "name": "World of Ruin Layout",
            "range": "0xEF6A56-0xEF9D17",
            "format": "ff6-lzss",
            "canRelocate": true,
            "reference": [
                {
                    "begin": "0xEEB224",
                    "mask": "0xFFFFFF"
                }
            ]
        },
        "worldLayout3": {
            "type": "assembly",
            "name": "Serpent Trench Layout",
            "range": "0xEF9D17-0xEFB631",
            "format": "ff6-lzss",
            "canRelocate": true,
            "reference": [
                {
                    "begin": "0xEEB227",
                    "mask": "0xFFFFFF"
                }
            ]
        },
        "worldOfRuinCinematic": {
            "type": "data",
            "name": "World of Ruin Cinematic Data",
            "range": "0xECE900-0xED0000",
            "format": "ff6-lzss",
            "isSequential": true,
            "assembly": {
                "bgGraphics": {
                    "type": "graphics",
                    "name": "World of Ruin Cinematic Background Graphics",
                    "range": "0x0000-0x0FE0",
                    "format": "snes4bpp",
                    "palette": {
                        "path": "cinematicProgram.worldOfRuinPalette",
                        "range": "0-16"
                    }
                },
                "layout": {
                    "type": "tilemap",
                    "name": "World of Ruin Cinematic Layout",
                    "range": "0x0FE0-0x16E0",
                    "format": "snes4bppTile",
                    "graphics": "worldOfRuinCinematic.bgGraphics",
                    "palette": {
                        "path": "cinematicProgram.worldOfRuinPalette",
                        "range": "0-16"
                    },
                    "width": 32,
                    "height": 28,
                    "backColor": true
                },
                "spriteGraphics": {
                    "type": "graphics",
                    "name": "World of Ruin Cinematic Sprite Graphics",
                    "range": "0x16E0-0x24C0",
                    "format": "snes4bpp",
                    "palette": {
                        "path": "cinematicProgram.worldOfRuinPalette",
                        "range": "16-64"
                    }
                }
            }
        },
        "worldPalette1": {
            "type": "assembly",
            "name": "World of Balance Palette",
            "range": "0xD2EC00-0xD2ED00",
            "format": "bgr555"
        },
        "worldPalette2": {
            "type": "assembly",
            "name": "World of Ruin Palette",
            "range": "0xD2ED00-0xD2EE00",
            "format": "bgr555"
        },
        "worldPalette3": {
            "type": "assembly",
            "name": "Serpent Trench Palette",
            "range": "0xD8E6BA-0xD8E800",
            "format": ["bgr555", "ff6-lzss"]
        },
        "worldTileProperties": {
            "type": "array",
            "name": "World Map Tile Properties",
            "range": "0xEE9B14-0xEE9F14",
            "array": {
                "length": 2
            },
            "assembly": {
                "type": "array",
                "array": {
                    "length": 256
                },
                "length": 512,
                "assembly": {
                    "type": "data",
                    "length": 2,
                    "assembly": {
                        "flags": {
                            "type": "property",
                            "name": "Flags",
                            "mask": "0xE073",
                            "flag": true,
                            "stringTable": {
                                "string": {
                                    "0": "Impassable on Chocobo",
                                    "1": "Airship Can't Land",
                                    "4": "Impassable on Foot",
                                    "5": "Forest",
                                    "6": "Enable Random Battles",
                                    "13": "Veldt",
                                    "14": "Phoenix Cave",
                                    "15": "Kefka's Tower"
                                }
                            }
                        },
                        "airshipShadow": {
                            "type": "property",
                            "name": "Airship Shadow",
                            "mask": "0x0C",
                            "stringTable": {
                                "string": {
                                    "0": "Small",
                                    "1": "Medium",
                                    "2": "Large",
                                    "3": "Very Large"
                                }
                            }
                        },
                        "battleBackground1": {
                            "type": "property",
                            "name": "Background",
                            "begin": 1,
                            "mask": "0x07",
                            "invalid": "this.parent.parent.i !== 0",
                            "stringTable": {
                                "string": {
                                    "0": "Field (WoB)",
                                    "1": "Forest (WoR)",
                                    "2": "Desert (WoB)",
                                    "3": "Forest (WoB)",
                                    "4": "Zozo Interior",
                                    "5": "Field (WoR)",
                                    "6": "Veldt",
                                    "7": "Clouds"
                                }
                            }
                        },
                        "battleBackground2": {
                            "type": "property",
                            "name": "Background",
                            "begin": 1,
                            "mask": "0x07",
                            "invalid": "this.parent.parent.i !== 1",
                            "stringTable": {
                                "string": {
                                    "0": "Field (WoB)",
                                    "1": "Forest (WoR)",
                                    "2": "Desert (WoR)",
                                    "3": "Forest (WoB)",
                                    "4": "Field (WoR)",
                                    "5": "Field (WoR)",
                                    "6": "Veldt",
                                    "7": "Clouds"
                                }
                            }
                        }
                    }
                }
            }
        }
    },
    "scriptEncoding": {
        "event": {
            "name": "Event Script",
            "delegate": "FF6Script",
            "command": {
                "battle": {
                    "name": "Event Battle",
                    "category": "Battle",
                    "length": 3,
                    "opcode": ["0x4D", "0x4C"],
                    "assembly": {
                        "type": {
                            "type": "property",
                            "name": "Type",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x4C": "Collision",
                                    "0x4D": "Normal"
                                }
                            }
                        },
                        "battle": {
                            "type": "property",
                            "name": "Battle",
                            "begin": 1,
                            "link": "battleGroupsEvent[%i]",
                            "stringTable": "battleGroupsEvent"
                        },
                        "background": {
                            "type": "property",
                            "name": "Background",
                            "begin": 2,
                            "mask": "0x3F",
                            "stringTable": "battleBackground"
                        },
                        "flags": {
                            "type": "property",
                            "name": "Flags",
                            "begin": 2,
                            "mask": "0xC0",
                            "flag": true,
                            "stringTable": {
                                "string": {
                                    "0": "Disable Sound Effect",
                                    "1": "Disable Blur"
                                }
                            }
                        }
                    }
                },
                "battleRandom": {
                    "name": "Random Battle",
                    "category": "Battle",
                    "opcode": "0x4E"
                },
                "battleTreasure": {
                    "name": "Monster-in-a-box Battle",
                    "category": "Battle",
                    "opcode": "0x8E"
                },
                "characterEquip": {
                    "name": "Change a Character's Equipment",
                    "category": "Character",
                    "opcode": ["0x8D", "0x9C"],
                    "length": 2,
                    "assembly": {
                        "equipment": {
                            "type": "property",
                            "name": "Equipment",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x8D": "Remove",
                                    "0x9C": "Optimize"
                                }
                            }
                        },
                        "character": {
                            "type": "property",
                            "name": "Character",
                            "begin": 1,
                            "stringTable": "eventCharacters"
                        }
                    }
                },
                "characterHP": {
                    "name": "Change a Character's HP/MP",
                    "category": "Character",
                    "opcode": ["0x8B", "0x8C"],
                    "length": 3,
                    "assembly": {
                        "hpmp": {
                            "type": "property",
                            "name": "HP/MP",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x8B": "HP",
                                    "0x8C": "MP"
                                }
                            }
                        },
                        "character": {
                            "type": "property",
                            "name": "Character",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        },
                        "amount": {
                            "type": "property",
                            "name": "Amount",
                            "begin": 2,
                            "stringTable": {
                                "string": {
                                    "0x00": "+1",
                                    "0x01": "+2",
                                    "0x02": "+4",
                                    "0x03": "+8",
                                    "0x04": "+16",
                                    "0x05": "+32",
                                    "0x06": "+64",
                                    "0x07": "+128",
                                    "0x7F": "Set to Max",
                                    "0x80": "−1",
                                    "0x81": "−2",
                                    "0x82": "−4",
                                    "0x83": "−8",
                                    "0x84": "−16",
                                    "0x85": "−32",
                                    "0x86": "−64",
                                    "0x87": "−128",
                                    "0xFF": "Set to Max"
                                }
                            }
                        }
                    }
                },
                "characterName": {
                    "name": "Change a Character's Name",
                    "category": "Character",
                    "opcode": "0x7F",
                    "length": 3,
                    "assembly": {
                        "character": {
                            "type": "property",
                            "name": "Character",
                            "begin": 1,
                            "stringTable": "eventCharacters"
                        },
                        "characterName": {
                            "type": "property",
                            "name": "Name",
                            "begin": 2,
                            "stringTable": "characterNames",
                            "link": "characterProperties[%i]"
                        }
                    }
                },
                "characterParty": {
                    "name": "Change Character's Party",
                    "category": "Character",
                    "opcode": "0x3F",
                    "length": 3,
                    "assembly": {
                        "character": {
                            "type": "property",
                            "name": "Character",
                            "begin": 1,
                            "stringTable": "eventCharacters"
                        },
                        "party": {
                            "type": "property",
                            "name": "Party",
                            "begin": 2,
                            "min": 1,
                            "max": 7,
                            "special": {
                                "0": "Remove from Party"
                            }
                        }
                    }
                },
                "characterPortrait": {
                    "name": "Show Character Portrait",
                    "category": "Character",
                    "opcode": "0xE7",
                    "length": 2,
                    "assembly": {
                        "character": {
                            "type": "property",
                            "name": "Name",
                            "begin": 2,
                            "stringTable": "portraitGraphics"
                        }
                    }
                },
                "characterProperties": {
                    "name": "Change a Character's Properties",
                    "category": "Character",
                    "opcode": "0x40",
                    "length": 3,
                    "assembly": {
                        "character": {
                            "type": "property",
                            "name": "Character",
                            "begin": 1,
                            "stringTable": "eventCharacters"
                        },
                        "properties": {
                            "type": "property",
                            "name": "Properties",
                            "begin": 2,
                            "stringTable": "characterNames",
                            "link": "characterProperties[%i]"
                        }
                    }
                },
                "characterRestore": {
                    "name": "Restore Character to Full HP/MP",
                    "category": "Character",
                    "opcode": "0x77",
                    "length": 2,
                    "assembly": {
                        "character": {
                            "type": "property",
                            "name": "Character",
                            "begin": 1,
                            "stringTable": "eventCharacters"
                        }
                    }
                },
                "characterStatus": {
                    "name": "Change a Character's Status",
                    "category": "Character",
                    "opcode": "0x88-0x8B",
                    "length": 4,
                    "assembly": {
                        "effect": {
                            "type": "property",
                            "name": "Effect",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x88": "Remove (AND)",
                                    "0x89": "Toggle (XOR)",
                                    "0x8A": "Set (OR)"
                                }
                            }
                        },
                        "character": {
                            "type": "property",
                            "name": "Character",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        },
                        "status": {
                            "type": "property",
                            "name": "Status",
                            "begin": 2,
                            "mask": "0xFFFF",
                            "flag": true,
                            "stringTable": {
                                "string": {
                                    "0-8": "<stringTable.statusNamesReversed[%i]>",
                                    "8-16": "<stringTable.statusNamesReversed[%i+16]>"
                                }
                            }
                        }
                    }
                },
                "cinematic": {
                    "name": "Show Cinematic",
                    "category": "Cinematic",
                    "opcode": ["0xA8", "0xA9", "0xAA", "0xAD", "0xAE", "0xBB", "0xBF"],
                    "assembly": {
                        "cinematic": {
                            "type": "property",
                            "name": "Cinematic",
                            "stringTable": {
                                "string": {
                                    "0xA8": "Floating Island",
                                    "0xA9": "Title Screen",
                                    "0xAA": "Intro",
                                    "0xAD": "World of Ruin",
                                    "0xAE": "Magitek Factory Train",
                                    "0xBB": "The End",
                                    "0xBF": "Airship (Ending)"
                                }
                            }
                        }
                    }
                },
                "cinematicEnding": {
                    "name": "Show Ending Cinematic",
                    "category": "Cinematic",
                    "opcode": "0xBA",
                    "length": 2,
                    "assembly": {
                        "scene": {
                            "type": "property",
                            "name": "Scene",
                            "begin": 1,
                            "stringTable": {
                                "string": {
                                    "2": "Clouds 1",
                                    "4": "Clouds 2",
                                    "5": "Clouds 3",
                                    "9": "Tiny Airship",
                                    "12": "Sea with Boat",
                                    "16": "Sea with Airship",
                                    "20": "Land with Airship",
                                    "24": "Land without Sprites",
                                    "28": "Big Airship",
                                    "32": "Airship with Jet Trails",
                                    "40": "Shadow",
                                    "45": "Book",
                                    "50": "Cyan",
                                    "60": "Mog",
                                    "64": "Edgar/Sabin",
                                    "70": "Gogo",
                                    "74": "\"And You\"",
                                    "80": "Gau",
                                    "90": "Terra",
                                    "100": "Locke/Celes",
                                    "110": "Relm",
                                    "120": "Strago",
                                    "130": "Setzer",
                                    "135": "Umaro"
                                }
                            }
                        }
                    }
                },
                "dialog": {
                    "name": "Display Dialogue",
                    "category": "Dialogue",
                    "length": 3,
                    "opcode": ["0x4B", "0x48"],
                    "assembly": {
                        "dialog": {
                            "type": "property",
                            "name": "Dialogue",
                            "begin": 1,
                            "mask": "0x3FFF",
                            "stringTable": "dialog",
                            "link": "dialog[%i]"
                        },
                        "dialogType": {
                            "type": "property",
                            "name": "Type",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x48": "Don't Wait for Keypress",
                                    "0x4B": "Wait for Keypress"
                                }
                            }
                        },
                        "flags": {
                            "type": "property",
                            "name": "Flags",
                            "begin": 2,
                            "mask": "0xC0",
                            "flag": true,
                            "stringTable": {
                                "string": {
                                    "0": "Show Text Only",
                                    "1": "Bottom of Screen"
                                }
                            }
                        }
                    }
                },
                "dialogKey": {
                    "name": "Wait for Dialogue Keypress",
                    "category": "Dialogue",
                    "opcode": "0x4A"
                },
                "dialogWait": {
                    "name": "Wait for Dialogue Window",
                    "category": "Dialogue",
                    "opcode": "0x49"
                },
                "end": {
                    "name": "End of Script",
                    "category": "Control",
                    "opcode": "0xFF"
                },
                "inventoryBumRush": {
                    "name": "Unlock Bum Rush",
                    "category": "Inventory",
                    "opcode": "0x90"
                },
                "inventoryEsper": {
                    "name": "Give/Take Esper",
                    "category": "Inventory",
                    "opcode": ["0x86", "0x87"],
                    "length": 2,
                    "assembly": {
                        "giveTake": {
                            "type": "property",
                            "name": "Give/Take",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x86": "Give",
                                    "0x87": "Take"
                                }
                            }
                        },
                        "esper": {
                            "type": "property",
                            "name": "Esper",
                            "begin": 1,
                            "offset": -54,
                            "stringTable": "esperNames",
                            "link": "esperProperties[%i]"
                        }
                    }
                },
                "inventoryGP": {
                    "name": "Give/Take GP",
                    "category": "Inventory",
                    "opcode": ["0x84", "0x85"],
                    "length": 3,
                    "assembly": {
                        "giveTake": {
                            "type": "property",
                            "name": "Give/Take",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x84": "Give",
                                    "0x85": "Take"
                                }
                            }
                        },
                        "gp": {
                            "type": "property",
                            "name": "GP",
                            "begin": 1,
                            "mask": "0xFFFF"
                        }
                    }
                },
                "inventoryItem": {
                    "name": "Give/Take Item",
                    "category": "Inventory",
                    "opcode": ["0x80", "0x81"],
                    "length": 2,
                    "assembly": {
                        "giveTake": {
                            "type": "property",
                            "name": "Give/Take",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x80": "Give",
                                    "0x81": "Take"
                                }
                            }
                        },
                        "item": {
                            "type": "property",
                            "name": "Item",
                            "begin": 1,
                            "stringTable": "itemNames",
                            "link": "itemProperties[%i]"
                        }
                    }
                },
                "inventorySwdTech": {
                    "name": "Unlock All SwdTechs",
                    "category": "Inventory",
                    "opcode": "0x8F"
                },
                "jumpBattleSwitch": {
                    "name": "Jump Based on Battle Switch",
                    "category": "Control",
                    "length": 5,
                    "opcode": "0xB7",
                    "assembly": {
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 1,
                            "stringTable": "battleSwitchGlobal"
                        },
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 2,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        }
                    }
                },
                "jumpCharacter": {
                    "name": "Jump Based on Character Switch",
                    "category": "Control",
                    "length": 5,
                    "opcode": "0xBE",
                    "assembly": {
                        "count": {
                            "type": "property",
                            "name": "Count",
                            "begin": 1,
                            "mask": "0x0F",
                            "hidden": true
                        },
                        "pointerArray": {
                            "type": "array",
                            "name": "Script Pointers",
                            "begin": 2,
                            "array": {
                                "length": 1,
                                "min": 1,
                                "max": 16
                            },
                            "assembly": {
                                "type": "data",
                                "length": 3,
                                "assembly": {
                                    "character": {
                                        "type": "property",
                                        "name": "Character",
                                        "begin": 2,
                                        "mask": "0xF0",
                                        "stringTable": {
                                            "length": 16,
                                            "default": "<stringTable.eventCharacters[%i]>",
                                            "string": {
                                                "0": "<stringTable.eventCharacters[%i]> (Equal To)",
                                                "1": "<stringTable.eventCharacters[%i]> (Greater Than)",
                                                "2": "<stringTable.eventCharacters[%i]> (Less Than)"
                                            }
                                        }
                                    },
                                    "scriptPointer": {
                                        "type": "property",
                                        "name": "Pointer",
                                        "begin": 0,
                                        "mask": "0x03FFFF",
                                        "script": "eventScript"
                                    }
                                }
                            }
                        }
                    }
                },
                "jumpDialog": {
                    "name": "Jump Based on Dialogue Choice",
                    "category": "Control",
                    "length": 4,
                    "opcode": "0xB6",
                    "assembly": {
                        "pointerArray": {
                            "type": "array",
                            "name": "Script Pointers",
                            "begin": 1,
                            "array": {
                                "length": 1,
                                "min": 1,
                                "max": 8
                            },
                            "assembly": {
                                "type": "data",
                                "length": 3,
                                "assembly": {
                                    "scriptPointer": {
                                        "type": "property",
                                        "name": "Pointer",
                                        "begin": 0,
                                        "mask": "0xFFFFFF",
                                        "script": "eventScript"
                                    }
                                }
                            }
                        }
                    }
                },
                "jumpSub": {
                    "name": "Call Subroutine",
                    "category": "Control",
                    "length": 4,
                    "opcode": "0xB2",
                    "assembly": {
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 1,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        }
                    }
                },
                "jumpSubRepeat": {
                    "name": "Call Subroutine (Repeat)",
                    "category": "Control",
                    "length": 5,
                    "opcode": "0xB3",
                    "assembly": {
                        "count": {
                            "type": "property",
                            "name": "Repeat Count",
                            "begin": 1
                        },
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 2,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        }
                    }
                },
                "jumpSwitch": {
                    "name": "Jump Based on Switch",
                    "category": "Control",
                    "length": 6,
                    "opcode": "0xC0-0xD0",
                    "assembly": {
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 3,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        },
                        "count": {
                            "type": "property",
                            "name": "Switch Count",
                            "offset": 1,
                            "mask": "0x07",
                            "hidden": true
                        },
                        "anyAll": {
                            "type": "property",
                            "name": "Any/All",
                            "mask": "0x08",
                            "stringTable": {
                                "string": {
                                    "0": "Any",
                                    "1": "All"
                                }
                            }
                        },
                        "switchArray": {
                            "type": "array",
                            "name": "Switches",
                            "begin": 1,
                            "array": {
                                "length": 1,
                                "min": 1,
                                "max": 8
                            },
                            "assembly": {
                                "type": "data",
                                "length": 2,
                                "assembly": {
                                    "switch": {
                                        "type": "property",
                                        "name": "Switch",
                                        "begin": 0,
                                        "mask": "0x7FFF",
                                        "stringTable": "eventSwitches"
                                    },
                                    "state": {
                                        "type": "property",
                                        "name": "State",
                                        "begin": 0,
                                        "mask": "0x8000",
                                        "stringTable": {
                                            "string": {
                                                "0": "Off",
                                                "1": "On"
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                },
                "jumpRandom": {
                    "name": "Jump (50% Random)",
                    "category": "Control",
                    "length": 4,
                    "opcode": "0xBD",
                    "assembly": {
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 1,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        }
                    }
                },
                "jumpReturn": {
                    "name": "Return from Subroutine",
                    "category": "Control",
                    "opcode": "0xFE"
                },
                "map": {
                    "name": "Change Map",
                    "category": "Map",
                    "length": 6,
                    "opcode": ["0x6A", "0x6B"],
                    "assembly": {
                        "map": {
                            "type": "property",
                            "name": "Map",
                            "begin": 1,
                            "mask": "0x01FF",
                            "stringTable": "mapProperties",
                            "link": "mapProperties[%i]"
                        },
                        "setParent": {
                            "type": "property",
                            "name": "Set Parent Map",
                            "begin": 2,
                            "mask": "0x02",
                            "bool": true
                        },
                        "z": {
                            "type": "property",
                            "name": "Z-Level",
                            "begin": 2,
                            "mask": "0x04",
                            "invalid": "this.parent.map.value < 3",
                            "stringTable": {
                                "string": {
                                    "0": "Lower",
                                    "1": "Upper"
                                }
                            }
                        },
                        "title": {
                            "type": "property",
                            "name": "Show Map Title",
                            "begin": 2,
                            "mask": "0x08",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        },
                        "direction": {
                            "type": "property",
                            "name": "Facing Direction",
                            "begin": 2,
                            "mask": "0x30",
                            "invalid": "this.parent.map.value < 3",
                            "stringTable": "directions"
                        },
                        "xDest": {
                            "type": "property",
                            "name": "X Destination",
                            "begin": 3
                        },
                        "yDest": {
                            "type": "property",
                            "name": "Y Destination",
                            "begin": 4
                        },
                        "vehicle": {
                            "type": "property",
                            "name": "Vehicle",
                            "begin": 5,
                            "mask": "0x03",
                            "invalid": "(this.parent.map.value > 2) && (this.parent.map.value != 511)",
                            "stringTable": "vehiclesWorld"
                        },
                        "noSizeUpdate": {
                            "type": "property",
                            "name": "No Size Update",
                            "begin": 5,
                            "mask": "0x20",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        },
                        "manualFade": {
                            "type": "property",
                            "name": "Manual Fade-In",
                            "begin": 5,
                            "mask": "0x40",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        },
                        "enableMapEvent": {
                            "type": "property",
                            "name": "Enable Map Event",
                            "begin": 5,
                            "mask": "0x80",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        }
                    }
                },
                "mapAnimationCounter": {
                    "name": "Set Map Animation Counter",
                    "category": "Map",
                    "opcode": "0x64",
                    "length": 3,
                    "assembly": {
                        "tile": {
                            "type": "property",
                            "name": "Tile",
                            "begin": 1,
                            "mask": "0x07"
                        },
                        "frame": {
                            "type": "property",
                            "name": "Frame",
                            "begin": 2,
                            "mask": "0x03"
                        }
                    }
                },
                "mapAnimationSpeed": {
                    "name": "Set Map Animation Speed",
                    "category": "Map",
                    "opcode": "0x65",
                    "length": 3,
                    "assembly": {
                        "tile": {
                            "type": "property",
                            "name": "Tile",
                            "begin": 1,
                            "mask": "0x07"
                        },
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 2,
                            "signed": true
                        }
                    }
                },
                "mapBackground": {
                    "name": "Change Map Layer",
                    "category": "Map",
                    "opcode": ["0x73", "0x74"],
                    "length": 5,
                    "assembly": {
                        "update": {
                            "type": "property",
                            "name": "Update",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0x73": "Update Immediately",
                                    "0x74": "No Update"
                                }
                            }
                        },
                        "layer": {
                            "type": "property",
                            "name": "Layer",
                            "begin": 2,
                            "mask": "0xC0",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0": "Layer 1",
                                    "1": "Layer 2",
                                    "2": "Layer 3"
                                }
                            }
                        },
                        "x": {
                            "type": "property",
                            "name": "X Position",
                            "begin": 1
                        },
                        "y": {
                            "type": "property",
                            "name": "Y Position",
                            "begin": 2,
                            "mask": "0x3F"
                        },
                        "w": {
                            "type": "property",
                            "name": "Width",
                            "begin": 3
                        },
                        "h": {
                            "type": "property",
                            "name": "Height",
                            "begin": 4
                        }
                    }
                },
                "mapUpdate": {
                    "name": "Update Map Layers After Changes",
                    "category": "Map",
                    "opcode": "0x75"
                },
                "mapPalette": {
                    "name": "Change Map Palette",
                    "category": "Map",
                    "opcode": "0x60",
                    "length": 3,
                    "assembly": {
                        "vramPalette": {
                            "type": "property",
                            "name": "VRAM Palette",
                            "begin": 1,
                            "stringTable": "mapVRAMPalette"
                        },
                        "palette": {
                            "type": "property",
                            "name": "Palette",
                            "begin": 2,
                            "stringTable": "mapSpritePalettes"
                        }
                    }
                },
                "mapParent": {
                    "name": "Set Parent Map",
                    "category": "Map",
                    "length": 6,
                    "opcode": "0x6C",
                    "assembly": {
                        "map": {
                            "type": "property",
                            "name": "Map",
                            "begin": 1,
                            "mask": "0x01FF",
                            "stringTable": "mapProperties",
                            "link": "mapProperties[%i]"
                        },
                        "direction": {
                            "type": "property",
                            "name": "Facing Direction",
                            "begin": 5,
                            "mask": "0x03",
                            "stringTable": "directions"
                        },
                        "xDest": {
                            "type": "property",
                            "name": "X Destination",
                            "begin": 3
                        },
                        "yDest": {
                            "type": "property",
                            "name": "Y Destination",
                            "begin": 4
                        }
                    }
                },
                "menuColosseum": {
                    "name": "Open Colosseum Menu",
                    "category": "Menu",
                    "opcode": "0x9A"
                },
                "menuFinalBattle": {
                    "name": "Open Final Battle Menu",
                    "category": "Menu",
                    "opcode": "0x9D"
                },
                "menuLoad": {
                    "name": "Open Game Load Menu",
                    "category": "Menu",
                    "opcode": "0xAB"
                },
                "menuName": {
                    "name": "Open Name Change Menu",
                    "category": "Menu",
                    "opcode": "0x98",
                    "length": 2,
                    "assembly": {
                        "character": {
                            "type": "property",
                            "name": "Character",
                            "begin": 1,
                            "stringTable": "eventCharacters"
                        }
                    }
                },
                "menuParty": {
                    "name": "Open Party Select Menu",
                    "category": "Menu",
                    "opcode": "0x99",
                    "length": 4,
                    "assembly": {
                        "party": {
                            "type": "property",
                            "name": "Parties",
                            "begin": 1,
                            "min": 1,
                            "mask": "0x07"
                        },
                        "resetParties": {
                            "type": "property",
                            "name": "Reset Parties",
                            "begin": 1,
                            "mask": "0x80",
                            "bool": true
                        },
                        "character": {
                            "type": "property",
                            "name": "Required",
                            "begin": 2,
                            "mask": "0xFFFF",
                            "flag": true,
                            "stringTable": "eventObjects"
                        }
                    }
                },
                "menuRestore": {
                    "name": "Restore Saved Game",
                    "category": "Control",
                    "opcode": "0x4F"
                },
                "menuShop": {
                    "name": "Open Shop Menu",
                    "category": "Menu",
                    "opcode": "0x9B",
                    "length": 2,
                    "assembly": {
                        "shop": {
                            "type": "property",
                            "name": "Shop",
                            "begin": 1,
                            "link": "shopProperties[%i]",
                            "stringTable": "shopProperties"
                        }
                    }
                },
                "objectCharacters": {
                    "name": "Update Character Objects",
                    "category": "Object",
                    "opcode": "0x45"
                },
                "objectCollisions": {
                    "name": "Enable/Disable Collision Event",
                    "category": "Object",
                    "length": 2,
                    "opcode": ["0x7C", "0x7D"],
                    "assembly": {
                        "enable": {
                            "type": "property",
                            "name": "Collisions",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x7C": "Enable",
                                    "0x7D": "Disable"
                                }
                            }
                        },
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        }
                    }
                },
                "objectCreate": {
                    "name": "Create/Delete Object",
                    "category": "Object",
                    "length": 2,
                    "opcode": ["0x3D", "0x3E"],
                    "assembly": {
                        "create": {
                            "type": "property",
                            "name": "Create/Delete",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x3D": "Create",
                                    "0x3E": "Delete"
                                }
                            }
                        },
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        }
                    }
                },
                "objectEvent": {
                    "name": "Change Object Event",
                    "category": "Object",
                    "length": 5,
                    "opcode": "0x7A",
                    "assembly": {
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        },
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 2,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        }
                    }
                },
                "objectGraphics": {
                    "name": "Change Object Graphics",
                    "category": "Object",
                    "length": 3,
                    "opcode": "0x37",
                    "assembly": {
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        },
                        "graphics": {
                            "type": "property",
                            "name": "Graphics",
                            "begin": 2,
                            "stringTable": "mapSprites"
                        }
                    }
                },
                "objectLoad": {
                    "name": "Load Saved Character Object Data",
                    "category": "Object",
                    "opcode": "0xAC"
                },
                "objectPalette": {
                    "name": "Change Object Palette",
                    "category": "Object",
                    "length": 3,
                    "opcode": "0x43",
                    "assembly": {
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        },
                        "palette": {
                            "type": "property",
                            "name": "Palette",
                            "begin": 2,
                            "stringTable": {
                                "length": 8,
                                "default": "<stringTable.mapSpritePalettes[%i]>"
                            }
                        }
                    }
                },
                "objectParty": {
                    "name": "Create the Party Object",
                    "category": "Object",
                    "opcode": "0x47"
                },
                "objectPassability": {
                    "name": "Enable/Disable Passability",
                    "category": "Object",
                    "length": 2,
                    "opcode": ["0x36", "0x78"],
                    "assembly": {
                        "passability": {
                            "type": "property",
                            "name": "Passability",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x36": "Disable",
                                    "0x78": "Enable"
                                }
                            }
                        },
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        }
                    }
                },
                "objectPyramid": {
                    "name": "Show Pyramid",
                    "category": "Object",
                    "opcode": "0xA7",
                    "length": 2,
                    "assembly": {
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        }
                    }
                },
                "objectScript": {
                    "name": "Object Script",
                    "category": "Object",
                    "length": 2,
                    "opcode": "0x00-0x35",
                    "assembly": {
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 0,
                            "mask": "0x3F",
                            "stringTable": "eventObjects"
                        },
                        "wait": {
                            "type": "property",
                            "name": "Wait Until Complete",
                            "begin": 1,
                            "mask": "0x80",
                            "bool": true
                        },
                        "scriptLength": {
                            "type": "property",
                            "name": "Script Length",
                            "begin": 1,
                            "mask": "0x7F",
                            "hidden": true
                        }
                    }
                },
                "objectShow": {
                    "name": "Show/Hide Object",
                    "category": "Object",
                    "length": 2,
                    "opcode": ["0x41", "0x42"],
                    "assembly": {
                        "show": {
                            "type": "property",
                            "name": "Show/Hide",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x41": "Show",
                                    "0x42": "Hide"
                                }
                            }
                        },
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        }
                    }
                },
                "objectVehicle": {
                    "name": "Change Object Vehicle",
                    "category": "Object",
                    "length": 3,
                    "opcode": "0x44",
                    "assembly": {
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        },
                        "vehicle": {
                            "type": "property",
                            "name": "Vehicle",
                            "begin": 2,
                            "mask": "0x60",
                            "stringTable": "vehicles"
                        },
                        "showRider": {
                            "type": "property",
                            "name": "Show Rider",
                            "begin": 2,
                            "mask": "0x80",
                            "bool": true
                        }
                    }
                },
                "objectWait": {
                    "name": "Wait For Object",
                    "category": "Object",
                    "length": 2,
                    "opcode": "0x35",
                    "assembly": {
                        "object": {
                            "type": "property",
                            "name": "Object",
                            "begin": 1,
                            "stringTable": "eventObjects"
                        }
                    }
                },
                "party": {
                    "name": "Set the Active Party",
                    "category": "Party",
                    "opcode": "0x46",
                    "length": 2,
                    "assembly": {
                        "party": {
                            "type": "property",
                            "name": "Party",
                            "begin": 1,
                            "min": 1,
                            "max": 7
                        }
                    }
                },
                "partyCharacters": {
                    "name": "Set the Party Characters",
                    "category": "Party",
                    "opcode": "0x3C",
                    "length": 5,
                    "assembly": {
                        "slot1": {
                            "type": "property",
                            "name": "Slot 1",
                            "begin": 1,
                            "stringTable": "eventCharacters",
                            "special": {
                                "0xFF": "Empty"
                            }
                        },
                        "slot2": {
                            "type": "property",
                            "name": "Slot 2",
                            "begin": 2,
                            "stringTable": "eventCharacters",
                            "special": {
                                "0xFF": "Empty"
                            }
                        },
                        "slot3": {
                            "type": "property",
                            "name": "Slot 3",
                            "begin": 3,
                            "stringTable": "eventCharacters",
                            "special": {
                                "0xFF": "Empty"
                            }
                        },
                        "slot4": {
                            "type": "property",
                            "name": "Slot 4",
                            "begin": 4,
                            "stringTable": "eventCharacters",
                            "special": {
                                "0xFF": "Empty"
                            }
                        }
                    }
                },
                "partyControl": {
                    "name": "Enable/Disable User Control",
                    "category": "Control",
                    "opcode": ["0x3A", "0x3B"],
                    "length": 1,
                    "assembly": {
                        "control": {
                            "type": "property",
                            "name": "User Control",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0x3A": "Enable",
                                    "0x3B": "Disable"
                                }
                            }
                        }
                    }
                },
                "partyMap": {
                    "name": "Move a Party to a Map",
                    "category": "Party",
                    "opcode": "0x79",
                    "length": 4,
                    "assembly": {
                        "party": {
                            "type": "property",
                            "name": "Party",
                            "begin": 1,
                            "min": 1,
                            "max": 7
                        },
                        "map": {
                            "type": "property",
                            "name": "Map",
                            "begin": 2,
                            "mask": "0x01FF",
                            "stringTable": "mapProperties",
                            "link": "mapProperties[%i]"
                        }
                    }
                },
                "partyPosition": {
                    "name": "Change the Active Party's Position",
                    "category": "Party",
                    "opcode": "0x7E",
                    "length": 3,
                    "assembly": {
                        "x": {
                            "type": "property",
                            "name": "X Position",
                            "begin": 1
                        },
                        "y": {
                            "type": "property",
                            "name": "Y Position",
                            "begin": 2
                        }
                    }
                },
                "partyReset": {
                    "name": "Reset the Previous Party",
                    "category": "Party",
                    "opcode": "0x82"
                },
                "partyRestore": {
                    "name": "Restore the Previous Party",
                    "category": "Party",
                    "opcode": "0x7B"
                },
                "repeatEnd": {
                    "name": "End Repeat",
                    "category": "Control",
                    "opcode": "0xB1"
                },
                "repeatStart": {
                    "name": "Start Repeat",
                    "category": "Control",
                    "length": 2,
                    "opcode": "0xB0",
                    "assembly": {
                        "count": {
                            "type": "property",
                            "name": "Repeat Count",
                            "begin": 1
                        }
                    }
                },
                "repeatSwitch": {
                    "name": "End Repeat (Switch)",
                    "category": "Control",
                    "length": 3,
                    "opcode": "0xBC",
                    "assembly": {
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 1,
                            "mask": "0xFFFF",
                            "stringTable": "eventSwitches"
                        }
                    }
                },
                "screenFade": {
                    "name": "Fade In/Out",
                    "category": "Screen",
                    "opcode": ["0x96", "0x97"],
                    "assembly": {
                        "fade": {
                            "type": "property",
                            "name": "Fade",
                            "stringTable": {
                                "string": {
                                    "0x96": "Fade In",
                                    "0x97": "Fade Out"
                                }
                            }
                        }
                    }
                },
                "screenFadeManual": {
                    "name": "Fade In/Out (manual)",
                    "category": "Screen",
                    "opcode": ["0x59", "0x5A"],
                    "length": 2,
                    "assembly": {
                        "fade": {
                            "type": "property",
                            "name": "Fade",
                            "stringTable": {
                                "string": {
                                    "0x59": "Fade In",
                                    "0x5A": "Fade Out"
                                }
                            }
                        },
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 1
                        }
                    }
                },
                "screenFadeStop": {
                    "name": "Stop Screen Fade",
                    "category": "Screen",
                    "opcode": "0x5B"
                },
                "screenFadeWait": {
                    "name": "Wait for Screen Fade",
                    "category": "Screen",
                    "opcode": "0x5C"
                },
                "screenFlash": {
                    "name": "Flash the Screen",
                    "category": "Screen",
                    "opcode": "0x55",
                    "length": 2,
                    "assembly": {
                        "color": {
                            "type": "property",
                            "name": "Color",
                            "begin": 1,
                            "mask": "0xE0",
                            "stringTable": "fixedColor"
                        }
                    }
                },
                "screenFlashlight": {
                    "name": "Show Flashlight",
                    "category": "Screen",
                    "opcode": "0x63",
                    "length": 2,
                    "assembly": {
                        "radius": {
                            "type": "property",
                            "name": "Radius",
                            "begin": 1,
                            "mask": "0x1F"
                        }
                    }
                },
                "screenMath": {
                    "name": "Set Fixed Color Math",
                    "category": "Screen",
                    "opcode": ["0x56", "0x57"],
                    "length": 2,
                    "assembly": {
                        "math": {
                            "type": "property",
                            "name": "Math",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0x56": "Addition",
                                    "0x57": "Subtraction"
                                }
                            }
                        },
                        "color": {
                            "type": "property",
                            "name": "Color",
                            "begin": 1,
                            "mask": "0xE0",
                            "stringTable": "fixedColor"
                        },
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 1,
                            "mask": "0x18"
                        },
                        "intensity": {
                            "type": "property",
                            "name": "Intensity",
                            "begin": 1,
                            "mask": "0x07"
                        }
                    }
                },
                "screenMathDisable": {
                    "name": "Disable Fixed Color Math",
                    "category": "Screen",
                    "opcode": "0x54"
                },
                "screenMosaic": {
                    "name": "Pixelate the Screen",
                    "category": "Screen",
                    "opcode": "0x62",
                    "length": 2,
                    "assembly": {
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 1,
                            "min": "1",
                            "max": "15"
                        }
                    }
                },
                "screenPalettes": {
                    "name": "Modify Color Palettes (All)",
                    "category": "Screen",
                    "opcode": ["0x50", "0x52"],
                    "length": 2,
                    "assembly": {
                        "palettes": {
                            "type": "property",
                            "name": "Palettes",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0x50": "Background",
                                    "0x52": "Sprite"
                                }
                            }
                        },
                        "function": {
                            "type": "property",
                            "name": "Function",
                            "begin": 1,
                            "mask": "0xE0",
                            "stringTable": {
                                "string": {
                                    "0": "Restore Colors to Normal",
                                    "1": "Increment Colors",
                                    "2": "Add to Colors (Broken)",
                                    "3": "Un-Increment Colors",
                                    "4": "Decrement Colors",
                                    "5": "Subtract from Colors",
                                    "6": "Un-Decrement Colors",
                                    "7": "Restore Colors to Normal"
                                }
                            }
                        },
                        "color": {
                            "type": "property",
                            "name": "Color",
                            "begin": 1,
                            "mask": "0x1C",
                            "stringTable": "fixedColorReversed"
                        },
                        "intensity": {
                            "type": "property",
                            "name": "Intensity",
                            "begin": 1,
                            "mask": "0x03"
                        }
                    }
                },
                "screenPalettesRange": {
                    "name": "Modify Color Palettes (Range)",
                    "category": "Screen",
                    "opcode": ["0x51", "0x53"],
                    "length": 4,
                    "assembly": {
                        "palettes": {
                            "type": "property",
                            "name": "Palettes",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0x51": "Background",
                                    "0x53": "Sprite"
                                }
                            }
                        },
                        "function": {
                            "type": "property",
                            "name": "Function",
                            "begin": 1,
                            "mask": "0xE0",
                            "stringTable": {
                                "string": {
                                    "0": "Restore Colors to Normal",
                                    "1": "Increment Colors",
                                    "2": "Add to Colors (Broken)",
                                    "3": "Un-Increment Colors",
                                    "4": "Decrement Colors",
                                    "5": "Subtract from Colors",
                                    "6": "Un-Decrement Colors",
                                    "7": "Restore Colors to Normal"
                                }
                            }
                        },
                        "color": {
                            "type": "property",
                            "name": "Color",
                            "begin": 1,
                            "mask": "0x1C",
                            "stringTable": "fixedColorReversed"
                        },
                        "intensity": {
                            "type": "property",
                            "name": "Intensity",
                            "begin": 1,
                            "mask": "0x03"
                        },
                        "colorFirst": {
                            "type": "property",
                            "name": "First Color",
                            "begin": 2
                        },
                        "colorLast": {
                            "type": "property",
                            "name": "Last Color",
                            "begin": 3
                        }
                    }
                },
                "screenScroll": {
                    "name": "Scroll Background",
                    "category": "Screen",
                    "opcode": ["0x5D-0x60", "0x70-0x73"],
                    "length": 3,
                    "assembly": {
                        "layer": {
                            "type": "property",
                            "name": "Layer",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0x5D": "Layer 1 (1's Complement)",
                                    "0x5E": "Layer 2 (1's Complement)",
                                    "0x5F": "Layer 3 (1's Complement)",
                                    "0x70": "Layer 1",
                                    "0x71": "Layer 2",
                                    "0x72": "Layer 3"
                                }
                            }
                        },
                        "hScroll": {
                            "type": "property",
                            "name": "H-Scroll",
                            "begin": 1,
                            "signed": true
                        },
                        "vScroll": {
                            "type": "property",
                            "name": "V-Scroll",
                            "begin": 2,
                            "signed": true
                        }
                    }
                },
                "screenScrollLock": {
                    "name": "Lock/Unlock the Screen",
                    "category": "Screen",
                    "opcode": ["0x38", "0x39"],
                    "length": 1,
                    "assembly": {
                        "lock": {
                            "type": "property",
                            "name": "Lock/Unlock",
                            "stringTable": {
                                "string": {
                                    "0x38": "Lock",
                                    "0x39": "Unlock"
                                }
                            }
                        }
                    }
                },
                "screenShake": {
                    "name": "Shake the Screen",
                    "category": "Screen",
                    "opcode": "0x58",
                    "length": 2,
                    "assembly": {
                        "amplitude": {
                            "type": "property",
                            "name": "Amplitude",
                            "begin": 1,
                            "mask": "0x03"
                        },
                        "frequency": {
                            "type": "property",
                            "name": "Frequency",
                            "begin": 1,
                            "mask": "0x0C"
                        },
                        "layers": {
                            "type": "property",
                            "name": "Affected Layers",
                            "begin": 1,
                            "mask": "0xF0",
                            "flag": true,
                            "stringTable": {
                                "string": {
                                    "0": "Layer 1",
                                    "1": "Layer 2",
                                    "2": "Layer 3",
                                    "3": "Sprites"
                                }
                            }
                        }
                    }
                },
                "screenTint": {
                    "name": "Tint Colors",
                    "category": "Screen",
                    "opcode": "0x61",
                    "length": 4,
                    "assembly": {
                        "color": {
                            "type": "property",
                            "name": "Color",
                            "begin": 1,
                            "stringTable": "fixedColor"
                        },
                        "colorFirst": {
                            "type": "property",
                            "name": "First Color",
                            "begin": 2
                        },
                        "colorLast": {
                            "type": "property",
                            "name": "Last Color",
                            "begin": 3
                        }
                    }
                },
                "spcInterrupt": {
                    "name": "SPC Interrupt",
                    "category": "Sound",
                    "length": 4,
                    "opcode": "0xF6",
                    "assembly": {
                        "interrupt": {
                            "type": "property",
                            "name": "Interrupt",
                            "begin": 1,
                            "stringTable": {
                                "string": {
                                    "0x00": "No Interrupt",
                                    "0x10": "Play Song",
                                    "0x11": "Play Song (Pause Current Song)",
                                    "0x14": "Play Song (Alt. Start)",
                                    "0x15": "Play Song (Alt. Start, Pause Current Song)",
                                    "0x18": "Play Sound Effect",
                                    "0x20": "System Sound Effect: Select",
                                    "0x21": "System Sound Effect: Move/Cancel",
                                    "0x22": "System Sound Effect: Error",
                                    "0x23": "System Sound Effect: Success",
                                    "0x24": "System Sound Effect: Delete",
                                    "0x28": "System Sound Effect: Player 1 Active",
                                    "0x29": "System Sound Effect: Player 2 Active",
                                    "0x2C": "System Sound Effect: Ching",
                                    "0x80": "Set Song/Sound Effect Volume",
                                    "0x81": "Set Song Volume",
                                    "0x82": "Set Sound Effect Volume",
                                    "0x83": "Set Sound Effect Pan",
                                    "0x84": "Set Tempo",
                                    "0x85": "Set Pitch",
                                    "0x89": "Enable Conditional Jump",
                                    "0xF0": "Stop All Sound",
                                    "0xF1": "Stop Song",
                                    "0xF2": "Stop Sound Effect",
                                    "0xF3": "Set Mono/Stereo Mode",
                                    "0xF4": "Mute Voices",
                                    "0xF5": "Pause/Unpause Music",
                                    "0xF6": "Enable/Disable Fast Forward",
                                    "0xFC": "Set Echo Delay",
                                    "0xFD": "Set DSP Register",
                                    "0xFE": "Data Transfer",
                                    "0xFF": "Reset"
                                }
                            }
                        },
                        "song": {
                            "type": "property",
                            "name": "Song",
                            "begin": "2",
                            "invalid": "(this.parent.interrupt.value < 0x10) || (this.parent.interrupt.value > 0x15)",
                            "stringTable": "song"
                        },
                        "volume": {
                            "type": "property",
                            "name": "Volume",
                            "begin": "3",
                            "invalid": "(this.parent.interrupt.value < 0x10) || (this.parent.interrupt.value > 0x15)"
                        },
                        "resetCommand": {
                            "type": "property",
                            "name": "Command",
                            "begin": "2",
                            "invalid": "this.parent.interrupt.value !== 0xFF",
                            "stringTable": {
                                "string": {
                                    "0x01": "Enable/Disable Waveform Output",
                                    "0x02": "Enable/Disable Echo",
                                    "0xF0": "Reset SPC"
                                }
                            }
                        },
                        "resetValue": {
                            "type": "property",
                            "name": "Enable/Disable",
                            "begin": "3",
                            "invalid": "(this.parent.interrupt.value !== 0xFF) || (this.parent.resetCommand.value === 0xF0)",
                            "stringTable": {
                                "string": {
                                    "0": "Disable",
                                    "1": "Enable"
                                }
                            }
                        }
                    }
                },
                "spcSong": {
                    "name": "Play Song",
                    "category": "Sound",
                    "length": 2,
                    "opcode": "0xF0",
                    "assembly": {
                        "song": {
                            "type": "property",
                            "name": "Song",
                            "begin": 1,
                            "mask": "0x7F",
                            "stringTable": "song"
                        },
                        "altStart": {
                            "type": "property",
                            "name": "Alt. Start",
                            "begin": 1,
                            "mask": "0x80",
                            "bool": true
                        }
                    }
                },
                "spcSongFadeIn": {
                    "name": "Fade In Song",
                    "category": "Sound",
                    "length": 3,
                    "opcode": "0xF1",
                    "assembly": {
                        "song": {
                            "type": "property",
                            "name": "Song",
                            "begin": 1,
                            "mask": "0x7F",
                            "stringTable": "song"
                        },
                        "altStart": {
                            "type": "property",
                            "name": "Alt. Start",
                            "begin": 1,
                            "mask": "0x80",
                            "bool": true
                        },
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 2
                        }
                    }
                },
                "spcSongFadeOut": {
                    "name": "Fade Out Song",
                    "category": "Sound",
                    "length": 2,
                    "opcode": "0xF2",
                    "assembly": {
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 1
                        }
                    }
                },
                "spcSongManual": {
                    "name": "Play Song (manual)",
                    "category": "Sound",
                    "length": 3,
                    "opcode": "0xEF",
                    "assembly": {
                        "song": {
                            "type": "property",
                            "name": "Song",
                            "begin": 1,
                            "mask": "0x7F",
                            "stringTable": "song"
                        },
                        "altStart": {
                            "type": "property",
                            "name": "Alt. Start",
                            "begin": 1,
                            "mask": "0x80",
                            "bool": true
                        },
                        "volume": {
                            "type": "property",
                            "name": "Volume",
                            "begin": 2
                        }
                    }
                },
                "spcSongPrevious": {
                    "name": "Fade In Previous Song",
                    "category": "Sound",
                    "length": 2,
                    "opcode": "0xF3",
                    "assembly": {
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 1
                        }
                    }
                },
                "spcSoundEffect": {
                    "name": "Play Sound Effect",
                    "category": "Sound",
                    "length": 2,
                    "opcode": "0xF4",
                    "assembly": {
                        "soundEffect": {
                            "type": "property",
                            "name": "Sound Effect",
                            "begin": 1
                        }
                    }
                },
                "spcSoundEffectManual": {
                    "name": "Play Sound Effect (manual)",
                    "category": "Sound",
                    "length": 4,
                    "opcode": "0xF5",
                    "assembly": {
                        "soundEffect": {
                            "type": "property",
                            "name": "Sound Effect",
                            "begin": 1
                        },
                        "pan": {
                            "type": "property",
                            "name": "Pan",
                            "begin": 2
                        },
                        "panEnvelope": {
                            "type": "property",
                            "name": "Pan Envelope",
                            "begin": 3
                        }
                    }
                },
                "spcSync": {
                    "name": "Sync SPC",
                    "category": "Sound",
                    "length": 2,
                    "opcode": "0xF9",
                    "assembly": {
                        "position": {
                            "type": "property",
                            "name": "Position",
                            "begin": 1
                        }
                    }
                },
                "spcWait": {
                    "name": "Wait for SPC",
                    "category": "Sound",
                    "length": 1,
                    "opcode": ["0xF8", "0xFA"],
                    "assembly": {
                        "waitType": {
                            "type": "property",
                            "name": "Type",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xF8": "Port-2",
                                    "0xFA": "Port-3"
                                }
                            }
                        }
                    }
                },
                "switchBattle": {
                    "name": "Change Battle Switch",
                    "category": "Control",
                    "length": 2,
                    "opcode": ["0xB8", "0xB9"],
                    "assembly": {
                        "onOff": {
                            "type": "property",
                            "name": "On/Off",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xB8": "On",
                                    "0xB9": "Off"
                                }
                            }
                        },
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 1,
                            "stringTable": "battleSwitchGlobal"
                        }
                    }
                },
                "switch": {
                    "name": "Change Switch",
                    "category": "Control",
                    "length": 2,
                    "opcode": "0xD0-0xDE",
                    "assembly": {
                        "bank": {
                            "type": "property",
                            "name": "Bank",
                            "begin": 0,
                            "hidden": true
                        },
                        "onOff": {
                            "type": "property",
                            "name": "On/Off",
                            "mask": "0",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0": "On",
                                    "1": "Off"
                                }
                            }
                        },
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 1,
                            "max": "0x06FF",
                            "stringTable": "eventSwitches"
                        }
                    }
                },
                "switchControl": {
                    "name": "Set Control Switches",
                    "category": "Control",
                    "length": 1,
                    "opcode": "0xDE-0xE5",
                    "assembly": {
                        "switches": {
                            "type": "property",
                            "name": "Switches",
                            "stringTable": {
                                "string": {
                                    "0xDE": "Current Party Characters",
                                    "0xDF": "Valid Character Objects",
                                    "0xE0": "Initialized Characters",
                                    "0xE1": "Available Characters",
                                    "0xE2": "Showing Character",
                                    "0xE3": "Characters in Any Party",
                                    "0xE4": "Current Party Index"
                                }
                            }
                        }
                    }
                },
                "timerStart": {
                    "name": "Start Timer",
                    "category": "Control",
                    "opcode": "0xA0",
                    "length": 6,
                    "assembly": {
                        "timer": {
                            "type": "property",
                            "name": "Timer",
                            "begin": 5,
                            "mask": "0x0C"
                        },
                        "duration": {
                            "type": "property",
                            "name": "Duration (Frames)",
                            "begin": 1,
                            "mask": "0xFFFF"
                        },
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 3,
                            "mask": "0x03FFFF",
                            "script": "eventScript"
                        },
                        "flags": {
                            "type": "property",
                            "name": "Flags",
                            "begin": 5,
                            "mask": "0xF0",
                            "flag": true,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0": "Show in Menu/Battle (Timer 0 only)",
                                    "1": "End Battle if Timer Expires",
                                    "2": "Show on Map (Timer 0 only)",
                                    "3": "Pause in Menu/Battle"
                                }
                            }
                        }
                    }
                },
                "timerStop": {
                    "name": "Stop Timer",
                    "category": "Control",
                    "opcode": "0xA1",
                    "length": 2,
                    "assembly": {
                        "timer": {
                            "type": "property",
                            "name": "Timer",
                            "begin": 1,
                            "mask": "0x03"
                        }
                    }
                },
                "variable": {
                    "name": "Variable Operations",
                    "category": "Control",
                    "length": 4,
                    "opcode": "0xE8-0xEC",
                    "assembly": {
                        "operation": {
                            "type": "property",
                            "name": "Operation",
                            "stringTable": {
                                "string": {
                                    "0xE8": "Set",
                                    "0xE9": "Add to",
                                    "0xEA": "Subtract from",
                                    "0xEB": "Compare"
                                }
                            }
                        },
                        "variable": {
                            "type": "property",
                            "name": "Variable",
                            "begin": 1,
                            "stringTable": "eventVariables"
                        },
                        "varValue": {
                            "type": "property",
                            "name": "Value",
                            "begin": 2,
                            "mask": "0xFFFF"
                        }
                    }
                },
                "wait": {
                    "name": "Wait",
                    "category": "Control",
                    "length": 1,
                    "opcode": "0x91-0x96",
                    "assembly": {
                        "duration": {
                            "type": "property",
                            "name": "Duration",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0x91": "15 Frames (0.25s)",
                                    "0x92": "30 Frames (0.5s)",
                                    "0x93": "45 Frames (0.75s)",
                                    "0x94": "60 Frames (1s)",
                                    "0x95": "120 Frames (2s)"
                                }
                            }
                        }
                    }
                },
                "waitManual": {
                    "name": "Wait (Manual)",
                    "category": "Control",
                    "length": 2,
                    "opcode": ["0xB4", "0xB5"],
                    "assembly": {
                        "units": {
                            "type": "property",
                            "name": "Units",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xB4": "Frames",
                                    "0xB5": "Seconds"
                                }
                            }
                        },
                        "durationFrames": {
                            "type": "property",
                            "name": "Duration",
                            "begin": 1,
                            "invalid": "this.parent.units.value !== 0xB4"
                        },
                        "durationSeconds": {
                            "type": "property",
                            "name": "Duration",
                            "begin": 1,
                            "multiplier": 0.25,
                            "invalid": "this.parent.units.value !== 0xB5"
                        }
                    }
                }
            }
        },
        "monster": {
            "name": "Monster Script",
            "delegate": "FF6MonsterScript",
            "command": {
                "animation": {
                    "name": "Monster Animation",
                    "opcode": ["0xFA00-0xFA09", "0xFA0A-0xFB00"],
                    "length": 4,
                    "assembly": {
                        "animation": {
                            "type": "property",
                            "name": "Animation",
                            "begin": 1,
                            "stringTable": "monsterAnimation"
                        },
                        "monsters": {
                            "type": "property",
                            "name": "Monsters",
                            "begin": 2,
                            "min": 1,
                            "flag": true,
                            "stringTable": {
                                "string": {
                                    "0": "Slot 1",
                                    "1": "Slot 2",
                                    "2": "Slot 3",
                                    "3": "Slot 4",
                                    "4": "Slot 5",
                                    "5": "Slot 6"
                                }
                            },
                            "special": {
                                "0": "This Monster",
                                "0xFF": "All Monsters"
                            }
                        }
                    }
                },
                "attack": {
                    "name": "Attack (Choose 1 of 3)",
                    "opcode": "0xF0",
                    "length": 4,
                    "assembly": {
                        "attack1": {
                            "type": "property",
                            "name": "Attack 1",
                            "begin": 1,
                            "link": "attackProperties[%i]",
                            "stringTable": {
                                "length": 255,
                                "default": "<stringTable.attackName[%i]>",
                                "string": {
                                    "0xFE": "Do Nothing"
                                }
                            }
                        },
                        "attack2": {
                            "type": "property",
                            "name": "Attack 2",
                            "begin": 2,
                            "link": "attackProperties[%i]",
                            "stringTable": {
                                "length": 255,
                                "default": "<stringTable.attackName[%i]>",
                                "string": {
                                    "0xFE": "Do Nothing"
                                }
                            }
                        },
                        "attack3": {
                            "type": "property",
                            "name": "Attack 3",
                            "begin": 3,
                            "link": "attackProperties[%i]",
                            "stringTable": {
                                "length": 255,
                                "default": "<stringTable.attackName[%i]>",
                                "string": {
                                    "0xFE": "Do Nothing"
                                }
                            }
                        }
                    }
                },
                "attackSingle": {
                    "name": "Attack (Single)",
                    "opcode": "0x00-0xF0",
                    "assembly": {
                        "attack": {
                            "type": "property",
                            "name": "Attack",
                            "link": "attackProperties[%i]",
                            "stringTable": {
                                "length": 240,
                                "default": "<stringTable.attackName[%i]>"
                            }
                        }
                    }
                },
                "battleEvent": {
                    "name": "Battle Event",
                    "opcode": "0xF7",
                    "length": 2,
                    "assembly": {
                        "battleEvent": {
                            "type": "property",
                            "name": "Battle Event",
                            "begin": 1
                        }
                    }
                },
                "changeBattle": {
                    "name": "Change Battle",
                    "opcode": "0xF2",
                    "length": 4,
                    "assembly": {
                        "battle": {
                            "type": "property",
                            "name": "Battle",
                            "begin": 2,
                            "mask": "0x7FFF",
                            "link": "battleProperties[%i]",
                            "stringTable": "battleProperties"
                        },
                        "exitEffect": {
                            "type": "property",
                            "name": "Exit Effect",
                            "begin": 1,
                            "mask": "0x7F"
                        },
                        "scroll": {
                            "type": "property",
                            "name": "Scroll Background",
                            "begin": 1,
                            "mask": "0x80",
                            "bool": true
                        },
                        "restoreMonsters": {
                            "type": "property",
                            "name": "Restore Monsters",
                            "begin": 2,
                            "mask": "0x8000",
                            "bool": true
                        }
                    }
                },
                "command": {
                    "name": "Command",
                    "opcode": "0xF4",
                    "length": 4,
                    "assembly": {
                        "command1": {
                            "type": "property",
                            "name": "Command 1",
                            "begin": 1,
                            "stringTable": "battleCommandName"
                        },
                        "command2": {
                            "type": "property",
                            "name": "Command 2",
                            "begin": 2,
                            "stringTable": "battleCommandName"
                        },
                        "command3": {
                            "type": "property",
                            "name": "Command 3",
                            "begin": 3,
                            "stringTable": "battleCommandName"
                        }
                    }
                },
                "conditional": {
                    "name": "Conditional",
                    "opcode": "0xFC",
                    "length": 4,
                    "assembly": {
                        "condition": {
                            "type": "property",
                            "name": "Condition",
                            "begin": 1,
                            "stringTable": {
                                "string": {
                                    "0": "Never",
                                    "1": "Command",
                                    "2": "Attack",
                                    "3": "Item",
                                    "4": "Element",
                                    "5": "Any Action",
                                    "6": "HP",
                                    "7": "MP",
                                    "8": "Has Status",
                                    "9": "Does Not Have Status",
                                    "10": "Never",
                                    "11": "Monster Timer",
                                    "12": "Variable Less Than",
                                    "13": "Variable Greater Than",
                                    "14": "Level Less Than",
                                    "15": "Level Greater Than",
                                    "16": "One Type of Monster",
                                    "17": "Monsters Alive",
                                    "18": "Monsters Dead",
                                    "19": "Targets Alive",
                                    "20": "Switch On",
                                    "21": "Switch Off",
                                    "22": "Battle Timer",
                                    "23": "Target Valid",
                                    "24": "Gau is Present",
                                    "25": "Monster Slot",
                                    "26": "Weak vs. Element",
                                    "27": "Battle Index",
                                    "28": "Always"
                                }
                            }
                        },
                        "target": {
                            "type": "property",
                            "name": "Target",
                            "begin": 2,
                            "invalid": "![6,7,8,9,14,15,23,26].includes(this.parent.condition.value)",
                            "stringTable": "battleTargets"
                        },
                        "command1": {
                            "type": "property",
                            "name": "Command 1",
                            "begin": 2,
                            "invalid": "(this.parent.condition.value !== 1)",
                            "stringTable": "battleCommandName"
                        },
                        "command2": {
                            "type": "property",
                            "name": "Command 2",
                            "begin": 3,
                            "invalid": "(this.parent.condition.value !== 1)",
                            "stringTable": "battleCommandName"
                        },
                        "attack1": {
                            "type": "property",
                            "name": "Attack 1",
                            "begin": 2,
                            "invalid": "(this.parent.condition.value !== 2)",
                            "stringTable": "attackName",
                            "link": "attackProperties[%i]"
                        },
                        "attack2": {
                            "type": "property",
                            "name": "Attack 2",
                            "begin": 3,
                            "invalid": "(this.parent.condition.value !== 2)",
                            "stringTable": "attackName",
                            "link": "attackProperties[%i]"
                        },
                        "item1": {
                            "type": "property",
                            "name": "Item 1",
                            "begin": 2,
                            "invalid": "(this.parent.condition.value !== 3)",
                            "stringTable": "itemNames",
                            "link": "itemProperties[%i]"
                        },
                        "item2": {
                            "type": "property",
                            "name": "Item 2",
                            "begin": 3,
                            "invalid": "(this.parent.condition.value !== 3)",
                            "stringTable": "itemNames",
                            "link": "itemProperties[%i]"
                        },
                        "element": {
                            "type": "property",
                            "name": "Element",
                            "begin": 2,
                            "flag": true,
                            "invalid": "(this.parent.condition.value !== 4)",
                            "stringTable": "element"
                        },
                        "hp": {
                            "type": "property",
                            "name": "HP",
                            "begin": 3,
                            "multiplier": 128,
                            "invalid": "(this.parent.condition.value !== 6)"
                        },
                        "mp": {
                            "type": "property",
                            "name": "MP",
                            "begin": 3,
                            "invalid": "(this.parent.condition.value !== 7)"
                        },
                        "status": {
                            "type": "property",
                            "name": "Status",
                            "begin": 3,
                            "invalid": "![8,9].includes(this.parent.condition.value)",
                            "stringTable": "statusNamesReversed"
                        },
                        "timer": {
                            "type": "property",
                            "name": "Timer",
                            "begin": 2,
                            "multiplier": 2,
                            "invalid": "![11,22].includes(this.parent.condition.value)"
                        },
                        "variable": {
                            "type": "property",
                            "name": "Variable",
                            "begin": 2,
                            "hidden": "![12,13].includes(this.parent.condition.value)",
                            "invalid": "![12,13,20,21].includes(this.parent.condition.value)",
                            "stringTable": "battleVariable"
                        },
                        "value": {
                            "type": "property",
                            "name": "Value",
                            "begin": 3,
                            "invalid": "![12,13].includes(this.parent.condition.value)"
                        },
                        "level": {
                            "type": "property",
                            "name": "Level",
                            "begin": 3,
                            "invalid": "![14,15].includes(this.parent.condition.value)"
                        },
                        "monsters": {
                            "type": "property",
                            "name": "Monsters",
                            "begin": 2,
                            "min": 1,
                            "invalid": "![17,18,25].includes(this.parent.condition.value)",
                            "flag": true,
                            "stringTable": {
                                "string": {
                                    "0": "Slot 1",
                                    "1": "Slot 2",
                                    "2": "Slot 3",
                                    "3": "Slot 4",
                                    "4": "Slot 5",
                                    "5": "Slot 6"
                                }
                            },
                            "special": {
                                "0": "This Monster",
                                "0xFF": "All Monsters"
                            }
                        },
                        "countType": {
                            "type": "property",
                            "name": "Type",
                            "begin": 2,
                            "invalid": "(this.parent.condition.value !== 19)",
                            "stringTable": {
                                "string": {
                                    "0": "Characters (≥)",
                                    "1": "Monsters (≤)"
                                }
                            }
                        },
                        "count": {
                            "type": "property",
                            "name": "Count",
                            "begin": 3,
                            "invalid": "(this.parent.condition.value !== 19)"
                        },
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 3,
                            "mask": "0x07",
                            "max": "0x07FF",
                            "msb": "this.parent.variable",
                            "invalid": "![20,21].includes(this.parent.condition.value)",
                            "stringTable": "battleSwitch"
                        },
                        "element2": {
                            "type": "property",
                            "name": "Element",
                            "begin": 3,
                            "flag": true,
                            "invalid": "(this.parent.condition.value !== 26)",
                            "stringTable": "element"
                        },
                        "battle": {
                            "type": "property",
                            "name": "Battle",
                            "begin": 2,
                            "mask": "0xFFFF",
                            "invalid": "(this.parent.condition.value !== 27)",
                            "link": "battleProperties[%i]",
                            "stringTable": "battleProperties"
                        }
                    }
                },
                "dialog": {
                    "name": "Display Dialogue",
                    "opcode": "0xF3",
                    "length": 3,
                    "assembly": {
                        "dialog": {
                            "type": "property",
                            "name": "Dialogue",
                            "begin": 1,
                            "link": "monsterDialog[%i]",
                            "stringTable": "monsterDialog"
                        }
                    }
                },
                "end": {
                    "name": "End of Script",
                    "opcode": "0xFF"
                },
                "endConditional": {
                    "name": "End Conditional",
                    "opcode": "0xFE"
                },
                "item": {
                    "name": "Use/Throw Item",
                    "opcode": "0xF6",
                    "length": 4,
                    "assembly": {
                        "useThrow": {
                            "type": "property",
                            "name": "Use/Throw",
                            "begin": 1,
                            "stringTable": {
                                "string": {
                                    "0": "Use",
                                    "1": "Throw"
                                }
                            }
                        },
                        "item1": {
                            "type": "property",
                            "name": "2/3 Chance",
                            "begin": 2,
                            "stringTable": "itemNames",
                            "link": "itemProperties[%i]"
                        },
                        "item2": {
                            "type": "property",
                            "name": "1/3 Chance",
                            "begin": 3,
                            "stringTable": "itemNames",
                            "link": "itemProperties[%i]"
                        }
                    }
                },
                "misc": {
                    "name": "Misc. Effects",
                    "opcode": "0xFB00-0xFB0E",
                    "length": 3,
                    "assembly": {
                        "effect": {
                            "type": "property",
                            "name": "Effect",
                            "begin": 1,
                            "stringTable": {
                                "string": {
                                    "0": "Reset Monster Timer",
                                    "1": "Target Becomes Invincible",
                                    "2": "Terminate Battle",
                                    "3": "Add Gau to the Party",
                                    "4": "Reset Battle Timer",
                                    "5": "Target Loses Invincibility",
                                    "6": "Target Can Be Targeted",
                                    "7": "Target Can't Be Targeted",
                                    "8": "Fill Target's ATB Gauge",
                                    "9": "Gau Returns from the Veldt",
                                    "11": "Set Status",
                                    "12": "Remove Status",
                                    "13": "Hide This Monster (Piranha)"
                                }
                            }
                        },
                        "target": {
                            "type": "property",
                            "name": "Target",
                            "begin": 2,
                            "invalid": "(this.parent.effect.value !== 1) && (this.parent.effect.value !== 5) && (this.parent.effect.value !== 6) && (this.parent.effect.value !== 7) && (this.parent.effect.value !== 8)",
                            "stringTable": "battleTargets"
                        },
                        "status": {
                            "type": "property",
                            "name": "Status",
                            "begin": 2,
                            "invalid": "(this.parent.effect.value !== 11) && (this.parent.effect.value !== 12)",
                            "stringTable": "statusNamesReversed"
                        }
                    }
                },
                "showHide": {
                    "name": "Show/Hide Monster",
                    "opcode": "0xF5",
                    "length": 4,
                    "assembly": {
                        "showHide": {
                            "type": "property",
                            "name": "Show/Hide",
                            "begin": 2,
                            "stringTable": {
                                "string": {
                                    "0": "Show - Set HP to Max",
                                    "1": "Hide - Normal",
                                    "2": "Show - At Current HP",
                                    "3": "Hide - Allow Targeting",
                                    "4": "Hide - Battle Won't End",
                                    "5": "Hide - Debug Mode"
                                }
                            }
                        },
                        "animation": {
                            "type": "property",
                            "name": "Animation",
                            "begin": 1
                        },
                        "monsters": {
                            "type": "property",
                            "name": "Monsters",
                            "begin": 3,
                            "min": 1,
                            "flag": true,
                            "stringTable": {
                                "string": {
                                    "0": "Slot 1",
                                    "1": "Slot 2",
                                    "2": "Slot 3",
                                    "3": "Slot 4",
                                    "4": "Slot 5",
                                    "5": "Slot 6"
                                }
                            },
                            "special": {
                                "0": "This Monster",
                                "0xFF": "All Monsters"
                            }
                        }
                    }
                },
                "soundEffect": {
                    "name": "Play Sound Effect",
                    "opcode": "0xFA09",
                    "length": 4,
                    "assembly": {
                        "soundEffect": {
                            "type": "property",
                            "name": "Sound Effect",
                            "begin": 2
                        },
                        "pan": {
                            "type": "property",
                            "name": "Pan",
                            "begin": 3
                        }
                    }
                },
                "switch": {
                    "name": "Change Switch",
                    "opcode": "0xF9",
                    "length": 4,
                    "assembly": {
                        "operation": {
                            "type": "property",
                            "name": "Operation",
                            "begin": 1,
                            "stringTable": {
                                "string": {
                                    "0": "Toggle",
                                    "1": "On",
                                    "2": "Off"
                                }
                            }
                        },
                        "variable": {
                            "type": "property",
                            "name": "Variable",
                            "begin": 2,
                            "hidden": true
                        },
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 3,
                            "mask": "0x07",
                            "max": "0x07FF",
                            "msb": "this.parent.variable",
                            "stringTable": "battleSwitch"
                        }
                    }
                },
                "target": {
                    "name": "Change Target",
                    "opcode": "0xF1",
                    "length": 2,
                    "assembly": {
                        "target": {
                            "type": "property",
                            "name": "Target",
                            "begin": 1,
                            "stringTable": "battleTargets"
                        }
                    }
                },
                "wait": {
                    "name": "Wait One Turn",
                    "opcode": "0xFD"
                },
                "variable": {
                    "name": "Change Variable",
                    "opcode": "0xF8",
                    "length": 3,
                    "assembly": {
                        "variable": {
                            "type": "property",
                            "name": "Variable",
                            "begin": 1,
                            "stringTable": "battleVariable"
                        },
                        "operation": {
                            "type": "property",
                            "name": "Operation",
                            "begin": 2,
                            "mask": "0xC0",
                            "stringTable": {
                                "string": {
                                    "0": "Set Variable",
                                    "2": "Add to Variable",
                                    "3": "Subtract from Variable"
                                }
                            }
                        },
                        "value": {
                            "type": "property",
                            "name": "Value",
                            "begin": 2,
                            "mask": "0x3F"
                        }
                    }
                }
            }
        },
        "object": {
            "name": "Object Script",
            "delegate": "FF6Script",
            "command": {
                "action": {
                    "name": "Do Action",
                    "opcode": "0x00-0x80",
                    "assembly": {
                        "action": {
                            "type": "property",
                            "name": "Action",
                            "begin": 0,
                            "mask": "0x3F",
                            "stringTable": "mapSpriteActions"
                        },
                        "hFlip": {
                            "type": "property",
                            "name": "Flip Horizontally",
                            "begin": 0,
                            "mask": "0x40",
                            "bool": true
                        }
                    }
                },
                "animation": {
                    "name": "Enable/Disable Animation",
                    "opcode": ["0xC6", "0xC7"],
                    "assembly": {
                        "animation": {
                            "type": "property",
                            "name": "Animation",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0xC6": "Enable",
                                    "0xC7": "Disable"
                                }
                            }
                        }
                    }
                },
                "branch": {
                    "name": "Branch",
                    "length": 2,
                    "opcode": "0xFA-0xFE",
                    "assembly": {
                        "branch": {
                            "type": "property",
                            "name": "Branch",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0xFA": "Backward (50% Chance)",
                                    "0xFB": "Forward (50% Change)",
                                    "0xFC": "Backward",
                                    "0xFD": "Forward"
                                }
                            }
                        },
                        "offset": {
                            "type": "property",
                            "name": "Offset",
                            "begin": 1
                        }
                    }
                },
                "direction": {
                    "name": "Change Direction",
                    "opcode": "0xCC-0xD0",
                    "assembly": {
                        "direction": {
                            "type": "property",
                            "name": "Direction",
                            "begin": 0,
                            "mask": "0x03",
                            "stringTable": "directions"
                        }
                    }
                },
                "end": {
                    "name": "End of Script",
                    "opcode": "0xFF"
                },
                "jump": {
                    "name": "Jump in the Air",
                    "opcode": ["0xDC", "0xDD"],
                    "assembly": {
                        "jump": {
                            "type": "property",
                            "name": "Jump",
                            "begin": 0,
                            "mask": "0x01",
                            "stringTable": {
                                "string": {
                                    "0": "Low",
                                    "1": "High"
                                }
                            }
                        }
                    }
                },
                "jumpEvent": {
                    "name": "Jump to Event",
                    "length": 4,
                    "opcode": "0xF9",
                    "assembly": {
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 1,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        }
                    }
                },
                "layerPriority": {
                    "name": "Change Layer Priority",
                    "length": 2,
                    "opcode": "0xC8",
                    "assembly": {
                        "priority": {
                            "type": "property",
                            "name": "Priority",
                            "begin": 1,
                            "stringTable": {
                                "string": {
                                    "0": "Default",
                                    "1": "Top Sprite Only",
                                    "2": "Foreground",
                                    "3": "Background"
                                }
                            }
                        }
                    }
                },
                "move": {
                    "name": "Move",
                    "opcode": "0x80-0xA0",
                    "assembly": {
                        "direction": {
                            "type": "property",
                            "name": "Direction",
                            "begin": 0,
                            "mask": "0x03",
                            "stringTable": "directions"
                        },
                        "distance": {
                            "type": "property",
                            "name": "Distance",
                            "begin": 0,
                            "offset": 1,
                            "mask": "0x1C"
                        }
                    }
                },
                "moveDiagonal": {
                    "name": "Move Diagonally",
                    "opcode": "0xA0-0xAC",
                    "assembly": {
                        "direction": {
                            "type": "property",
                            "name": "Direction",
                            "begin": 0,
                            "mask": "0x0F",
                            "stringTable": {
                                "string": {
                                    "0": "Up/Right 1×1",
                                    "1": "Down/Right 1×1",
                                    "2": "Down/Left 1×1",
                                    "3": "Up/Left 1×1",
                                    "4": "Up/Right 2×1",
                                    "5": "Up/Right 1×2",
                                    "6": "Down/Right 1×2",
                                    "7": "Down/Right 2×1",
                                    "8": "Down/Left 2×1",
                                    "9": "Down/Left 1×2",
                                    "10": "Up/Left 1×2",
                                    "11": "Up/Left 2×1"
                                }
                            }
                        }
                    }
                },
                "position": {
                    "name": "Set Object Position",
                    "opcode": "0xD5",
                    "length": 3,
                    "assembly": {
                        "x": {
                            "type": "property",
                            "name": "X Position",
                            "begin": 1
                        },
                        "y": {
                            "type": "property",
                            "name": "Y Position",
                            "begin": 2
                        }
                    }
                },
                "scroll": {
                    "name": "Scroll to Object",
                    "opcode": "0xD7"
                },
                "showHide": {
                    "name": "Show/Hide Object",
                    "opcode": ["0xD0", "0xD1"],
                    "assembly": {
                        "showHide": {
                            "type": "property",
                            "name": "Show/Hide",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xD0": "Show",
                                    "0xD1": "Hide"
                                }
                            }
                        }
                    }
                },
                "speed": {
                    "name": "Change Speed",
                    "opcode": "0xC0-0xC6",
                    "assembly": {
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xC0": "Very Slow",
                                    "0xC1": "Slow",
                                    "0xC2": "Normal",
                                    "0xC3": "Fast",
                                    "0xC4": "Fast ×2",
                                    "0xC5": "Fast ×4"
                                }
                            }
                        }
                    }
                },
                "switch": {
                    "name": "Change Switch",
                    "length": 2,
                    "opcode": "0xE1-0xE7",
                    "assembly": {
                        "bank": {
                            "type": "property",
                            "name": "Bank",
                            "begin": 0,
                            "hidden": true
                        },
                        "onOff": {
                            "type": "property",
                            "name": "On/Off",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0": "On",
                                    "1": "Off"
                                }
                            }
                        },
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 1,
                            "stringTable": "mapSwitches"
                        }
                    }
                },
                "vehicle": {
                    "name": "Change Vehicle",
                    "length": 2,
                    "opcode": "0xC9",
                    "assembly": {
                        "vehicle": {
                            "type": "property",
                            "name": "Vehicle",
                            "begin": 1,
                            "mask": "0x60",
                            "stringTable": "vehicles"
                        },
                        "showRider": {
                            "type": "property",
                            "name": "Show Rider",
                            "begin": 1,
                            "mask": "0x80",
                            "bool": true
                        }
                    }
                },
                "wait": {
                    "name": "Wait",
                    "length": 2,
                    "opcode": "0xE0",
                    "assembly": {
                        "duration": {
                            "type": "property",
                            "name": "Duration",
                            "begin": 1
                        }
                    }
                }
            }
        },
        "vehicle": {
            "name": "Vehicle Script",
            "delegate": "FF6Script",
            "command": {
                "airshipPosition": {
                    "name": "Set Airship Position",
                    "opcode": "0xC7",
                    "length": 3,
                    "assembly": {
                        "x": {
                            "type": "property",
                            "name": "X Position",
                            "begin": 1
                        },
                        "y": {
                            "type": "property",
                            "name": "Y Position",
                            "begin": 2
                        }
                    }
                },
                "altitude": {
                    "name": "Change Altitude",
                    "opcode": "0xC5",
                    "length": 3,
                    "assembly": {
                        "altitude": {
                            "type": "property",
                            "name": "Altitude",
                            "begin": 1,
                            "mask": "0xFFFF"
                        }
                    }
                },
                "arrows": {
                    "name": "Direction Arrows",
                    "opcode": "0xDA-0xDD",
                    "assembly": {
                        "arrows": {
                            "type": "property",
                            "name": "Arrows",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xDA": "Show Direction Arrows",
                                    "0xDB": "Lock Direction Arrows",
                                    "0xDC": "Hide Direction Arrows"
                                }
                            }
                        }
                    }
                },
                "battle": {
                    "name": "Event Battle",
                    "length": 3,
                    "opcode": "0xCA-0xD0",
                    "assembly": {
                        "battle": {
                            "type": "property",
                            "name": "Battle",
                            "begin": 1,
                            "stringTable": "battleGroupsEvent",
                            "link": "battleGroupsEvent[%i]"
                        },
                        "background": {
                            "type": "property",
                            "name": "Background",
                            "begin": 2,
                            "mask": "0x3F"
                        },
                        "blur": {
                            "type": "property",
                            "name": "Disable Blur",
                            "begin": 2,
                            "mask": "0x80",
                            "bool": true
                        },
                        "soundEffect": {
                            "type": "property",
                            "name": "Disable Sound Effect",
                            "begin": 2,
                            "mask": "0x40",
                            "bool": true
                        }
                    }
                },
                "cameraDirection": {
                    "name": "Change Camera Direction",
                    "opcode": "0xC1",
                    "length": 3,
                    "assembly": {
                        "direction": {
                            "type": "property",
                            "name": "Direction",
                            "begin": 1,
                            "mask": "0xFFFF"
                        }
                    }
                },
                "cinematic": {
                    "name": "Show Cinematic",
                    "opcode": ["0xF2", "0xF3", "0xF5", "0xF6", "0xF8-0xFD", "0xFE"],
                    "assembly": {
                        "cinematic": {
                            "type": "property",
                            "name": "Cinematic",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xF2": "Ending Airship Scene",
                                    "0xF3": "Light of Judgement 1",
                                    "0xF5": "Light of Judgement 2",
                                    "0xF6": "Unknown",
                                    "0xF8": "Light of Judgement 3",
                                    "0xF9": "Light of Judgement 4",
                                    "0xFA": "Falcon Rising from Water",
                                    "0xFB": "Airship Smoking",
                                    "0xFC": "Airship Crashing",
                                    "0xFE": "Vector Approach"
                                }
                            }
                        }
                    }
                },
                "direction": {
                    "name": "Change Direction",
                    "opcode": "0xC1-0xC5",
                    "length": 3,
                    "assembly": {
                        "directionType": {
                            "type": "property",
                            "name": "Type",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xC1": "Camera Direction",
                                    "0xC2": "Movement Direction",
                                    "0xC3": "Unknown Direction (0xC3)",
                                    "0xC4": "Unknown Direction (0xC4)"
                                }
                            }
                        },
                        "direction": {
                            "type": "property",
                            "name": "Direction",
                            "begin": 1,
                            "mask": "0xFFFF"
                        }
                    }
                },
                "end": {
                    "name": "End of Script",
                    "opcode": "0xFF"
                },
                "graphic": {
                    "name": "Change Vehicle Graphic",
                    "opcode": ["0xF4", "0xF7", "0xFD"],
                    "assembly": {
                        "graphic": {
                            "type": "property",
                            "name": "Graphic",
                            "begin": 0,
                            "stringTable": {
                                "string": {
                                    "0xF4": "Falcon",
                                    "0xF7": "Bird",
                                    "0xFD": "Esper Terra"
                                }
                            }
                        }
                    }
                },
                "jumpSwitch": {
                    "name": "Jump Based on Switch",
                    "length": 6,
                    "opcode": "0xB0-0xC0",
                    "assembly": {
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 3,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        },
                        "count": {
                            "type": "property",
                            "name": "Switch Count",
                            "offset": 1,
                            "mask": "0x07",
                            "hidden": true
                        },
                        "anyAll": {
                            "type": "property",
                            "name": "Any/All",
                            "mask": "0x08",
                            "stringTable": {
                                "string": {
                                    "0": "Any",
                                    "1": "All"
                                }
                            }
                        },
                        "switchArray": {
                            "type": "array",
                            "name": "Switches",
                            "begin": 1,
                            "array": {
                                "length": 1,
                                "min": 1,
                                "max": 8
                            },
                            "assembly": {
                                "type": "data",
                                "length": 2,
                                "assembly": {
                                    "switch": {
                                        "type": "property",
                                        "name": "Switch",
                                        "begin": 0,
                                        "mask": "0x7FFF",
                                        "stringTable": "eventSwitches"
                                    },
                                    "state": {
                                        "type": "property",
                                        "name": "State",
                                        "begin": 0,
                                        "mask": "0x8000",
                                        "stringTable": {
                                            "string": {
                                                "0": "Off",
                                                "1": "On"
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                },
                "map": {
                    "name": "Change Map",
                    "length": 6,
                    "opcode": ["0xD2", "0xD3"],
                    "assembly": {
                        "map": {
                            "type": "property",
                            "name": "Map",
                            "begin": 1,
                            "mask": "0x01FF",
                            "stringTable": "mapProperties",
                            "link": "mapProperties[%i]"
                        },
                        "setParent": {
                            "type": "property",
                            "name": "Set Parent Map",
                            "begin": 2,
                            "mask": "0x02",
                            "bool": true
                        },
                        "z": {
                            "type": "property",
                            "name": "Z-Level",
                            "begin": 2,
                            "mask": "0x04",
                            "invalid": "this.parent.map.value < 3",
                            "stringTable": {
                                "string": {
                                    "0": "Lower",
                                    "1": "Upper"
                                }
                            }
                        },
                        "title": {
                            "type": "property",
                            "name": "Show Map Title",
                            "begin": 2,
                            "mask": "0x08",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        },
                        "direction": {
                            "type": "property",
                            "name": "Facing Direction",
                            "begin": 2,
                            "mask": "0x30",
                            "invalid": "this.parent.map.value < 3",
                            "stringTable": "directions"
                        },
                        "xDest": {
                            "type": "property",
                            "name": "X Destination",
                            "begin": 3
                        },
                        "yDest": {
                            "type": "property",
                            "name": "Y Destination",
                            "begin": 4
                        },
                        "vehicle": {
                            "type": "property",
                            "name": "Vehicle",
                            "begin": 5,
                            "mask": "0x03",
                            "invalid": "(this.parent.map.value > 2) && (this.parent.map.value != 511)",
                            "stringTable": "vehiclesWorld"
                        },
                        "noSizeUpdate": {
                            "type": "property",
                            "name": "No Size Update",
                            "begin": 5,
                            "mask": "0x20",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        },
                        "manualFade": {
                            "type": "property",
                            "name": "Manual Fade-In",
                            "begin": 5,
                            "mask": "0x40",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        },
                        "enableMapEvent": {
                            "type": "property",
                            "name": "Enable Map Event",
                            "begin": 5,
                            "mask": "0x80",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        }
                    }
                },
                "miniMap": {
                    "name": "Show/Hide Mini-Map",
                    "opcode": ["0xDD", "0xDF"],
                    "assembly": {
                        "miniMap": {
                            "type": "property",
                            "name": "Mini-Map",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xDD": "Hide Mini-Map",
                                    "0xDF": "Show Mini-Map"
                                }
                            }
                        }
                    }
                },
                "move": {
                    "name": "Move Vehicle",
                    "length": 2,
                    "opcode": "0x00-0x80",
                    "assembly": {
                        "flags": {
                            "type": "property",
                            "name": "Flags",
                            "mask": "0x7F",
                            "flag": true,
                            "stringTable": {
                                "string": {
                                    "0": "Sharp Turn",
                                    "1": "Go Back",
                                    "2": "Go Forward",
                                    "3": "Turn Right",
                                    "4": "Turn Left",
                                    "5": "Go Up",
                                    "6": "Go Down"
                                }
                            }
                        },
                        "duration": {
                            "type": "property",
                            "name": "Duration",
                            "begin": 1
                        }
                    }
                },
                "moveForward": {
                    "name": "Move Forward",
                    "opcode": "0xC6",
                    "length": 3,
                    "assembly": {
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 1,
                            "mask": "0xFFFF"
                        }
                    }
                },
                "screenFade": {
                    "name": "Fade In/Out",
                    "opcode": ["0xD8", "0xD9"],
                    "assembly": {
                        "fade": {
                            "type": "property",
                            "name": "Fade",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xD8": "Fade In",
                                    "0xD9": "Fade Out"
                                }
                            }
                        }
                    }
                },
                "showHide": {
                    "name": "Show/Hide Vehicle Sprite",
                    "opcode": ["0xD0", "0xD1"],
                    "assembly": {
                        "showHide": {
                            "type": "property",
                            "name": "Show/Hide",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xD0": "Show Vehicle",
                                    "0xD1": "Hide Vehicle"
                                }
                            }
                        }
                    }
                },
                "switch": {
                    "name": "Change Switch",
                    "length": 3,
                    "opcode": ["0xC8", "0xC9"],
                    "assembly": {
                        "onOff": {
                            "type": "property",
                            "name": "On/Off",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xC8": "On",
                                    "0xC9": "Off"
                                }
                            }
                        },
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 1,
                            "mask": "0xFFFF",
                            "max": "0x06FF",
                            "stringTable": "eventSwitches"
                        }
                    }
                },
                "unknown_C0": {
                    "name": "Unknown Command $C0",
                    "opcode": "0xC0",
                    "length": 2,
                    "assembly": {
                        "parameter": {
                            "type": "property",
                            "name": "Parameter",
                            "begin": 1
                        }
                    }
                },
                "unknown_DE": {
                    "name": "Unknown Command $DE",
                    "opcode": "0xDE",
                    "length": 3,
                    "assembly": {
                        "parameter1": {
                            "type": "property",
                            "name": "Parameter 1",
                            "begin": 1
                        },
                        "parameter2": {
                            "type": "property",
                            "name": "Parameter 2",
                            "begin": 2
                        }
                    }
                },
                "wait": {
                    "name": "Wait",
                    "length": 2,
                    "opcode": "0xE0",
                    "assembly": {
                        "duration": {
                            "type": "property",
                            "name": "Duration",
                            "begin": 1
                        }
                    }
                }
            }
        },
        "world": {
            "name": "World Script",
            "delegate": "FF6Script",
            "command": {
                "action": {
                    "name": "Do Action",
                    "opcode": "0x00-0x80",
                    "assembly": {
                        "action": {
                            "type": "property",
                            "name": "Action",
                            "begin": 0,
                            "mask": "0x3F",
                            "stringTable": "mapSpriteActions"
                        },
                        "hFlip": {
                            "type": "property",
                            "name": "Flip Horizontally",
                            "begin": 0,
                            "mask": "0x40",
                            "bool": true
                        }
                    }
                },
                "direction": {
                    "name": "Change Direction",
                    "opcode": "0xCC-0xD0",
                    "assembly": {
                        "direction": {
                            "type": "property",
                            "name": "Direction",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xCC": "Up",
                                    "0xCD": "Right",
                                    "0xCE": "Down",
                                    "0xCF": "Left"
                                }
                            }
                        }
                    }
                },
                "end": {
                    "name": "End of Script",
                    "opcode": "0xFF"
                },
                "figaro": {
                    "name": "Figaro Castle Animation",
                    "opcode": ["0xFD", "0xFE"],
                    "assembly": {
                        "animation": {
                            "type": "property",
                            "name": "Animation",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xFD": "Figaro Castle Submerging",
                                    "0xFE": "Figaro Castle Emerging"
                                }
                            }
                        }
                    }
                },
                "jumpDirection": {
                    "name": "Jump Based on Facing Direction",
                    "length": 5,
                    "opcode": "0xD5",
                    "assembly": {
                        "direction": {
                            "type": "property",
                            "name": "Direction",
                            "begin": 1,
                            "stringTable": "directions"
                        },
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 2,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        }
                    }
                },
                "jumpKeypress": {
                    "name": "Jump Based on Keypress",
                    "length": 4,
                    "opcode": "0xD4",
                    "assembly": {
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 1,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        }
                    }
                },
                "jumpSwitch": {
                    "name": "Jump Based on Switch",
                    "length": 6,
                    "opcode": "0xB0-0xC0",
                    "assembly": {
                        "scriptPointer": {
                            "type": "property",
                            "name": "Script Pointer",
                            "begin": 3,
                            "mask": "0xFFFFFF",
                            "script": "eventScript"
                        },
                        "count": {
                            "type": "property",
                            "name": "Switch Count",
                            "offset": 1,
                            "mask": "0x07",
                            "hidden": true
                        },
                        "anyAll": {
                            "type": "property",
                            "name": "Any/All",
                            "mask": "0x08",
                            "stringTable": {
                                "string": {
                                    "0": "Any",
                                    "1": "All"
                                }
                            }
                        },
                        "switchArray": {
                            "type": "array",
                            "name": "Switches",
                            "begin": 1,
                            "array": {
                                "length": 1,
                                "min": 1,
                                "max": 8
                            },
                            "reference": [
                                {
                                    "target": "this.parent.count",
                                    "arrayLength": true
                                }
                            ],
                            "assembly": {
                                "type": "data",
                                "length": 2,
                                "assembly": {
                                    "switch": {
                                        "type": "property",
                                        "name": "Switch",
                                        "begin": 0,
                                        "mask": "0x7FFF",
                                        "stringTable": "eventSwitches"
                                    },
                                    "state": {
                                        "type": "property",
                                        "name": "State",
                                        "begin": 0,
                                        "mask": "0x8000",
                                        "stringTable": {
                                            "string": {
                                                "0": "Off",
                                                "1": "On"
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                },
                "map": {
                    "name": "Change Map",
                    "length": 6,
                    "opcode": ["0xD2", "0xD3"],
                    "assembly": {
                        "map": {
                            "type": "property",
                            "name": "Map",
                            "begin": 1,
                            "mask": "0x01FF",
                            "stringTable": "mapProperties",
                            "link": "mapProperties[%i]"
                        },
                        "setParent": {
                            "type": "property",
                            "name": "Set Parent Map",
                            "begin": 2,
                            "mask": "0x02",
                            "bool": true
                        },
                        "z": {
                            "type": "property",
                            "name": "Z-Level",
                            "begin": 2,
                            "mask": "0x04",
                            "invalid": "this.parent.map.value < 3",
                            "stringTable": {
                                "string": {
                                    "0": "Lower",
                                    "1": "Upper"
                                }
                            }
                        },
                        "title": {
                            "type": "property",
                            "name": "Show Map Title",
                            "begin": 2,
                            "mask": "0x08",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        },
                        "direction": {
                            "type": "property",
                            "name": "Facing Direction",
                            "begin": 2,
                            "mask": "0x30",
                            "invalid": "this.parent.map.value < 3",
                            "stringTable": "directions"
                        },
                        "xDest": {
                            "type": "property",
                            "name": "X Destination",
                            "begin": 3
                        },
                        "yDest": {
                            "type": "property",
                            "name": "Y Destination",
                            "begin": 4
                        },
                        "vehicle": {
                            "type": "property",
                            "name": "Vehicle",
                            "begin": 5,
                            "mask": "0x03",
                            "invalid": "(this.parent.map.value > 2) && (this.parent.map.value != 511)",
                            "stringTable": "vehiclesWorld"
                        },
                        "noSizeUpdate": {
                            "type": "property",
                            "name": "No Size Update",
                            "begin": 5,
                            "mask": "0x20",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        },
                        "manualFade": {
                            "type": "property",
                            "name": "Manual Fade-In",
                            "begin": 5,
                            "mask": "0x40",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        },
                        "enableMapEvent": {
                            "type": "property",
                            "name": "Enable Map Event",
                            "begin": 5,
                            "mask": "0x80",
                            "invalid": "this.parent.map.value < 3",
                            "bool": true
                        }
                    }
                },
                "miniMap": {
                    "name": "Show/Hide Mini-Map",
                    "opcode": ["0xDD", "0xDF"],
                    "assembly": {
                        "miniMap": {
                            "type": "property",
                            "name": "Mini-Map",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xDD": "Hide Mini-Map",
                                    "0xDF": "Show Mini-Map"
                                }
                            }
                        }
                    }
                },
                "move": {
                    "name": "Move",
                    "opcode": "0x80-0xA0",
                    "assembly": {
                        "direction": {
                            "type": "property",
                            "name": "Direction",
                            "begin": 0,
                            "mask": "0x03",
                            "stringTable": "directions"
                        },
                        "distance": {
                            "type": "property",
                            "name": "Distance",
                            "begin": 0,
                            "offset": 1,
                            "mask": "0x1C"
                        }
                    }
                },
                "moveDiagonal": {
                    "name": "Move Diagonally",
                    "opcode": "0xA0-0xAC",
                    "assembly": {
                        "direction": {
                            "type": "property",
                            "name": "Direction",
                            "begin": 0,
                            "mask": "0x0F",
                            "stringTable": {
                                "string": {
                                    "0": "Up/Right 1×1",
                                    "1": "Down/Right 1×1",
                                    "2": "Down/Left 1×1",
                                    "3": "Up/Left 1×1",
                                    "4": "Up/Right 2×1",
                                    "5": "Up/Right 1×2",
                                    "6": "Down/Right 1×2",
                                    "7": "Down/Right 2×1",
                                    "8": "Down/Left 2×1",
                                    "9": "Down/Left 1×2",
                                    "10": "Up/Left 1×2",
                                    "11": "Up/Left 2×1"
                                }
                            }
                        }
                    }
                },
                "screenFade": {
                    "name": "Fade In/Out",
                    "opcode": ["0xD8", "0xD9"],
                    "assembly": {
                        "fade": {
                            "type": "property",
                            "name": "Fade",
                            "stringTable": {
                                "string": {
                                    "0xD8": "Fade In",
                                    "0xD9": "Fade Out"
                                }
                            }
                        }
                    }
                },
                "ship": {
                    "name": "Change to Ship Sprite",
                    "opcode": "0xFC"
                },
                "showHide": {
                    "name": "Show/Hide Character Sprite",
                    "opcode": ["0xD0", "0xD1"],
                    "assembly": {
                        "showHide": {
                            "type": "property",
                            "name": "Show/Hide",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xD0": "Show",
                                    "0xD1": "Hide"
                                }
                            }
                        }
                    }
                },
                "speed": {
                    "name": "Change Movement Speed",
                    "opcode": "0xC0-0xC5",
                    "assembly": {
                        "speed": {
                            "type": "property",
                            "name": "Speed",
                            "begin": 0,
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xC0": "Slow ×2",
                                    "0xC1": "Slow",
                                    "0xC2": "Normal",
                                    "0xC3": "Fast",
                                    "0xC4": "Fast ×2"
                                }
                            }
                        }
                    }
                },
                "switch": {
                    "name": "Change Switch",
                    "length": 3,
                    "opcode": ["0xC8", "0xC9"],
                    "assembly": {
                        "onOff": {
                            "type": "property",
                            "name": "On/Off",
                            "stringTable": {
                                "hideIndex": true,
                                "string": {
                                    "0xC8": "On",
                                    "0xC9": "Off"
                                }
                            }
                        },
                        "switch": {
                            "type": "property",
                            "name": "Switch",
                            "begin": 1,
                            "mask": "0xFFFF",
                            "max": "0x06FF",
                            "stringTable": "eventSwitches"
                        }
                    }
                },
                "wait": {
                    "name": "Wait",
                    "length": 2,
                    "opcode": "0xE0",
                    "assembly": {
                        "duration": {
                            "type": "property",
                            "name": "Duration",
                            "begin": 1
                        }
                    }
                }
            }
        }
    },
    "charTable": {
        "bigText": {
            "name": "Big Text Characters",
            "char": {
                "0xD3": "“",
                "0xD6": "\\holy",
                "0xD7": "×",
                "0xD8": "\\lightning",
                "0xD9": "\\wind",
                "0xDA": "\\earth",
                "0xDB": "\\ice",
                "0xDC": "\\fire",
                "0xDD": "\\water",
                "0xDE": "\\poison"
            }
        },
        "dialog": {
            "name": "Dialogue",
            "char": {
                "0x00": "\\0",
                "0x01": "\\n",
                "0x02": "\\char00",
                "0x03": "\\char01",
                "0x04": "\\char02",
                "0x05": "\\char03",
                "0x06": "\\char04",
                "0x07": "\\char05",
                "0x08": "\\char06",
                "0x09": "\\char07",
                "0x0A": "\\char08",
                "0x0B": "\\char09",
                "0x0C": "\\char10",
                "0x0D": "\\char11",
                "0x0E": "\\char12",
                "0x0F": "\\char13",
                "0x10": "\\pause",
                "0x11": "\\pause[",
                "0x12": "\\key",
                "0x13": "\\page",
                "0x14": "\\t[",
                "0x15": "\\choice",
                "0x16": "\\key[",
                "0x19": "\\gp",
                "0x1A": "\\item",
                "0x1B": "\\spell",
                "0x20": "A",
                "0x21": "B",
                "0x22": "C",
                "0x23": "D",
                "0x24": "E",
                "0x25": "F",
                "0x26": "G",
                "0x27": "H",
                "0x28": "I",
                "0x29": "J",
                "0x2A": "K",
                "0x2B": "L",
                "0x2C": "M",
                "0x2D": "N",
                "0x2E": "O",
                "0x2F": "P",
                "0x30": "Q",
                "0x31": "R",
                "0x32": "S",
                "0x33": "T",
                "0x34": "U",
                "0x35": "V",
                "0x36": "W",
                "0x37": "X",
                "0x38": "Y",
                "0x39": "Z",
                "0x3A": "a",
                "0x3B": "b",
                "0x3C": "c",
                "0x3D": "d",
                "0x3E": "e",
                "0x3F": "f",
                "0x40": "g",
                "0x41": "h",
                "0x42": "i",
                "0x43": "j",
                "0x44": "k",
                "0x45": "l",
                "0x46": "m",
                "0x47": "n",
                "0x48": "o",
                "0x49": "p",
                "0x4A": "q",
                "0x4B": "r",
                "0x4C": "s",
                "0x4D": "t",
                "0x4E": "u",
                "0x4F": "v",
                "0x50": "w",
                "0x51": "x",
                "0x52": "y",
                "0x53": "z",
                "0x54": "0",
                "0x55": "1",
                "0x56": "2",
                "0x57": "3",
                "0x58": "4",
                "0x59": "5",
                "0x5A": "6",
                "0x5B": "7",
                "0x5C": "8",
                "0x5D": "9",
                "0x5E": "!",
                "0x5F": "?",
                "0x60": "/",
                "0x61": ":",
                "0x62": "”",
                "0x63": "’",
                "0x64": "-",
                "0x65": ".",
                "0x66": ",",
                "0x67": "…",
                "0x68": ";",
                "0x69": "#",
                "0x6A": "+",
                "0x6B": "(",
                "0x6C": ")",
                "0x6D": "%",
                "0x6E": "~",
                "0x6F": "*",
                "0x70": "@",
                "0x71": "♬",
                "0x72": "=",
                "0x73": "“",
                "0x76": "\\holy",
                "0x77": "×",
                "0x78": "\\lightning",
                "0x79": "\\wind",
                "0x7A": "\\earth",
                "0x7B": "\\ice",
                "0x7C": "\\fire",
                "0x7D": "\\water",
                "0x7E": "\\poison",
                "0x7F": " "
            }
        },
        "dte": {
            "name": "DTE",
            "char": {
                "0x80": "e ",
                "0x81": " t",
                "0x82": ": ",
                "0x83": "th",
                "0x84": "t ",
                "0x85": "he",
                "0x86": "s ",
                "0x87": "er",
                "0x88": " a",
                "0x89": "re",
                "0x8A": "in",
                "0x8B": "ou",
                "0x8C": "d ",
                "0x8D": " w",
                "0x8E": " s",
                "0x8F": "an",
                "0x90": "o ",
                "0x91": " h",
                "0x92": " o",
                "0x93": "r ",
                "0x94": "n ",
                "0x95": "at",
                "0x96": "to",
                "0x97": " i",
                "0x98": ", ",
                "0x99": "ve",
                "0x9A": "ng",
                "0x9B": "ha",
                "0x9C": " m",
                "0x9D": "Th",
                "0x9E": "st",
                "0x9F": "on",
                "0xA0": "yo",
                "0xA1": " b",
                "0xA2": "me",
                "0xA3": "y ",
                "0xA4": "en",
                "0xA5": "it",
                "0xA6": "ar",
                "0xA7": "ll",
                "0xA8": "ea",
                "0xA9": "I ",
                "0xAA": "ed",
                "0xAB": " f",
                "0xAC": " y",
                "0xAD": "hi",
                "0xAE": "is",
                "0xAF": "es",
                "0xB0": "or",
                "0xB1": "l ",
                "0xB2": " c",
                "0xB3": "ne",
                "0xB4": "’s",
                "0xB5": "nd",
                "0xB6": "le",
                "0xB7": "se",
                "0xB8": " I",
                "0xB9": "a ",
                "0xBA": "te",
                "0xBB": " l",
                "0xBC": "pe",
                "0xBD": "as",
                "0xBE": "ur",
                "0xBF": "u ",
                "0xC0": "al",
                "0xC1": " p",
                "0xC2": "g ",
                "0xC3": "om",
                "0xC4": " d",
                "0xC5": "f ",
                "0xC6": " g",
                "0xC7": "ow",
                "0xC8": "rs",
                "0xC9": "be",
                "0xCA": "ro",
                "0xCB": "us",
                "0xCC": "ri",
                "0xCD": "wa",
                "0xCE": "we",
                "0xCF": "Wh",
                "0xD0": "et",
                "0xD1": " r",
                "0xD2": "nt",
                "0xD3": "m ",
                "0xD4": "ma",
                "0xD5": "I’",
                "0xD6": "li",
                "0xD7": "ho",
                "0xD8": "of",
                "0xD9": "Yo",
                "0xDA": "h ",
                "0xDB": " n",
                "0xDC": "ee",
                "0xDD": "de",
                "0xDE": "so",
                "0xDF": "gh",
                "0xE0": "ca",
                "0xE1": "ra",
                "0xE2": "n’",
                "0xE3": "ta",
                "0xE4": "ut",
                "0xE5": "el",
                "0xE6": "! ",
                "0xE7": "fo",
                "0xE8": "ti",
                "0xE9": "We",
                "0xEA": "lo",
                "0xEB": "e!",
                "0xEC": "ld",
                "0xED": "no",
                "0xEE": "ac",
                "0xEF": "ce",
                "0xF0": "k ",
                "0xF1": " u",
                "0xF2": "oo",
                "0xF3": "ke",
                "0xF4": "ay",
                "0xF5": "w ",
                "0xF6": "!!",
                "0xF7": "ag",
                "0xF8": "il",
                "0xF9": "ly",
                "0xFA": "co",
                "0xFB": ". ",
                "0xFC": "ch",
                "0xFD": "go",
                "0xFE": "ge",
                "0xFF": "e…"
            }
        },
        "fixedLength": {
            "name": "Fixed Length",
            "char": {
                "0xFE": " ",
                "0xFF": "\\pad"
            }
        },
        "nullTerminated": {
            "name": "Null-Terminated",
            "char": {
                "0x00": "\\0",
                "0xFE": "\\pad",
                "0xFF": " "
            }
        },
        "smallText": {
            "name": "Small Text Characters",
            "char": {
                "0xD4": "↑",
                "0xD5": "→",
                "0xD6": "↙",
                "0xD7": "×",
                "0xD8": "\\dagger",
                "0xD9": "\\sword",
                "0xDA": "\\spear",
                "0xDB": "\\katana",
                "0xDC": "\\staff",
                "0xDD": "\\brush",
                "0xDE": "\\star",
                "0xDF": "\\special",
                "0xE0": "\\card",
                "0xE1": "\\claw",
                "0xE2": "\\shield",
                "0xE3": "\\helm",
                "0xE4": "\\armor",
                "0xE5": "\\tool",
                "0xE6": "\\scroll",
                "0xE7": "\\relic",
                "0xE8": "\\white",
                "0xE9": "\\black",
                "0xEA": "\\effect"
            }
        },
        "text": {
            "name": "Text",
            "char": {
                "0x01": "\\n",
                "0x02": "\\char[",
                "0x05": "\\pause",
                "0x07": "\\key",
                "0x0E": "\\item",
                "0x0F": "\\attack",
                "0x10": "\\var0",
                "0x11": "\\var1",
                "0x12": "\\string[",
                "0x13": "\\var2",
                "0x14": "\\var3",
                "0x80": "A",
                "0x81": "B",
                "0x82": "C",
                "0x83": "D",
                "0x84": "E",
                "0x85": "F",
                "0x86": "G",
                "0x87": "H",
                "0x88": "I",
                "0x89": "J",
                "0x8A": "K",
                "0x8B": "L",
                "0x8C": "M",
                "0x8D": "N",
                "0x8E": "O",
                "0x8F": "P",
                "0x90": "Q",
                "0x91": "R",
                "0x92": "S",
                "0x93": "T",
                "0x94": "U",
                "0x95": "V",
                "0x96": "W",
                "0x97": "X",
                "0x98": "Y",
                "0x99": "Z",
                "0x9A": "a",
                "0x9B": "b",
                "0x9C": "c",
                "0x9D": "d",
                "0x9E": "e",
                "0x9F": "f",
                "0xA0": "g",
                "0xA1": "h",
                "0xA2": "i",
                "0xA3": "j",
                "0xA4": "k",
                "0xA5": "l",
                "0xA6": "m",
                "0xA7": "n",
                "0xA8": "o",
                "0xA9": "p",
                "0xAA": "q",
                "0xAB": "r",
                "0xAC": "s",
                "0xAD": "t",
                "0xAE": "u",
                "0xAF": "v",
                "0xB0": "w",
                "0xB1": "x",
                "0xB2": "y",
                "0xB3": "z",
                "0xB4": "0",
                "0xB5": "1",
                "0xB6": "2",
                "0xB7": "3",
                "0xB8": "4",
                "0xB9": "5",
                "0xBA": "6",
                "0xBB": "7",
                "0xBC": "8",
                "0xBD": "9",
                "0xBE": "!",
                "0xBF": "?",
                "0xC0": "/",
                "0xC1": ":",
                "0xC2": "”",
                "0xC3": "’",
                "0xC4": "-",
                "0xC5": ".",
                "0xC6": ",",
                "0xC7": "…",
                "0xC8": ";",
                "0xC9": "#",
                "0xCA": "+",
                "0xCB": "(",
                "0xCC": ")",
                "0xCD": "%",
                "0xCE": "~",
                "0xD2": "="
            }
        }
    },
    "textEncoding": {
        "bigText": {
            "name": "Big Text",
            "charTable": [
                "bigText",
                "text",
                "fixedLength"
            ]
        },
        "dialog": {
            "name": "Dialogue",
            "charTable": [
                "dialog",
                "dte"
            ]
        },
        "nullTerminated": {
            "name": "Null-Terminated",
            "charTable": [
                "bigText",
                "text",
                "nullTerminated"
            ]
        },
        "smallText": {
            "name": "Small Text",
            "charTable": [
                "text",
                "smallText",
                "fixedLength"
            ]
        }
    },
    "stringTable": {
        "battleBackground": {
            "name": "Battle Backgrounds",
            "default": "Background %i",
            "length": 56,
            "string": {
                "0": "Field (WoB)",
                "1": "Forest (WoR)",
                "2": "Desert (WoB)",
                "3": "Forest (WoB)",
                "4": "Zozo Interior",
                "5": "Field (WoR)",
                "6": "Veldt",
                "7": "Clouds",
                "8": "Narshe Exterior",
                "9": "Narshe Caves A",
                "10": "Caves",
                "11": "Mountains Exterior",
                "12": "Mountains Interior",
                "13": "River",
                "14": "Imperial Camp",
                "15": "Train Exterior",
                "16": "Train Interior",
                "17": "Narshe Caves B",
                "18": "Snowfields",
                "19": "Town Exterior",
                "20": "Imperial Castle",
                "21": "Floating Island",
                "22": "Kefka&apos;s Tower Exterior",
                "23": "Opera House Stage",
                "24": "Opera House Catwalk",
                "25": "Burning Building",
                "26": "Castle Interior",
                "27": "Magitek Research Facility",
                "28": "Colosseum",
                "29": "Magitek Factory",
                "30": "Village Exterior",
                "31": "Waterfall",
                "32": "Owzer's House",
                "33": "Train Tracks",
                "34": "Sealed Gate",
                "35": "Underwater",
                "36": "Zozo",
                "37": "Airship (WoB, Centered)",
                "38": "Darill&apos;s Tomb",
                "39": "Doma Castle",
                "40": "Kefka's Tower Interior",
                "41": "Airship (WoR)",
                "42": "Fire Caves",
                "43": "Town Interior",
                "44": "Magitek Train Car",
                "45": "Fanatics' Tower",
                "46": "Cyan&apos;s Dream",
                "47": "Desert (WoR)",
                "48": "Airship (WoB)",
                "50": "GhostTrain",
                "51": "Doom",
                "52": "Poltergeist",
                "53": "Goddess",
                "54": "Final Kefka",
                "55": "Tentacles",
                "63": "Map Default",
                "255": "No Background Override"
            }
        },
        "battleCharacterGraphics": {
            "name": "Battle Character Graphics",
            "default": "Battle Character Graphics %i",
            "length": 24,
            "string": {
                "0-23": "<stringTable.mapSprites[%i]>",
                "23": "Green Soldier"
            }
        },
        "battleSwitch": {
            "name": "Local Battle Switch",
            "length": 296,
            "default": "Local Battle Switch %i",
            "string": {
                "32-192": "<stringTable.battleSwitchGlobal[%i-32]>",
                "192-200": "Forcefield: <stringTable.element[%i-192]>",
                "200-208": "Number of Targets",
                "208-216": "Number of Types of Monsters",
                "288": "Monster Switch 0",
                "289": "Monster Switch 1",
                "290": "Monster Switch 2",
                "291": "Monster Switch 3",
                "292": "Monster Switch 4",
                "293": "Monster Switch 5",
                "294": "Monster Switch 6",
                "295": "Monster Switch 7"
            }
        },
        "battleSwitchGlobal": {
            "name": "Global Battle Switch",
            "length": 160,
            "default": "Global Battle Switch %i",
            "string": {
                "40-56": "Conditional Battle Switch %i",
                "56": "Magic Only (Fanatics' Tower)",
                "57": "Double Morph Duration",
                "58": "Permanent Morph",
                "64": "Defeated in Battle",
                "65": "Enable MMMMagic Scene",
                "66": "Morph is Available",
                "67": "Espers are Available",
                "68": "Ran Away From Battle",
                "69": "Ran Out of Time During Battle",
                "70": "Timers are Shown in Battle",
                "71": "Eaten by Zone Eater",
                "72": "Doom Gaze is Defeated",
                "73": "<characterNames[11]> is available",
                "75": "<characterNames[3]> Can't Leave After Battle",
                "76": "<characterNames[1]> is a Merchant",
                "77": "<characterNames[1]> is a Green Soldier",
                "80-96": "Doom Gaze's HP",
                "96-104": "Cursed Shield Battle Counter"
            }
        },
        "battleTargets": {
            "length": "0x4E",
            "default": "Target %i",
            "string": {
                "0x00-0x30": "<characterNames[%i]>",
                "0x30": "Monster Slot 1",
                "0x31": "Monster Slot 2",
                "0x32": "Monster Slot 3",
                "0x33": "Monster Slot 4",
                "0x34": "Monster Slot 5",
                "0x35": "Monster Slot 6",
                "0x36": "Self",
                "0x37": "All Monsters (Not Self)",
                "0x38": "All Monsters",
                "0x39": "Random Monster (Not Self)",
                "0x3A": "Random Monster",
                "0x3B": "All Dead Characters",
                "0x3C": "Random Dead Character",
                "0x3D": "All Dead Monsters",
                "0x3E": "Random Dead Monster",
                "0x3F": "All Characters with Reflect",
                "0x40": "Random Character with Reflect",
                "0x41": "All Monsters with Reflect",
                "0x42": "Random Monster with Reflect",
                "0x43": "All Characters",
                "0x44": "Random Character",
                "0x45": "Previous Attacker",
                "0x46": "All Targets",
                "0x47": "Nothing",
                "0x48": "Character Slot 1",
                "0x49": "Character Slot 2",
                "0x4A": "Character Slot 3",
                "0x4B": "Character Slot 4",
                "0x4C": "Random Target(s)",
                "0x4D": "\"<stringTable.attackName[0xCA]>\" Target"
            }
        },
        "battleVariable": {
            "name": "Local Battle Variable",
            "length": 37,
            "default": "Local Battle Variable %i",
            "string": {
                "4-24": "<stringTable.battleVariableGlobal[%i-4]>",
                "24": "Forcefield Elements",
                "25": "Number of Targets",
                "26": "Number of Types of Monsters",
                "36": "Monster Variable"
            }
        },
        "battleVariableGlobal": {
            "name": "Global Battle Variable",
            "length": 20,
            "default": "Global Battle Variable %i",
            "string": {
                "5": "Conditional Battle Switches 1",
                "6": "Conditional Battle Switches 2",
                "7": "Battle Switches 1",
                "8": "Battle Switches 2",
                "9": "Battle Switches 3",
                "10": "Doom Gaze's HP 1",
                "11": "Doom Gaze's HP 2",
                "12": "Cursed Shield Counter"
            }
        },
        "blitzName": {
            "name": "Blitz Name",
            "length": 8,
            "link": "attackName[%i+12]"
        },
        "directions": {
            "name": "Facing Directions",
            "string": {
                "0": "Up",
                "1": "Right",
                "2": "Down",
                "3": "Left"
            }
        },
        "element": {
            "name": "Element",
            "string": {
                "0": "Fire",
                "1": "Ice",
                "2": "Lightning",
                "3": "Poison",
                "4": "Wind",
                "5": "Holy",
                "6": "Earth",
                "7": "Water"
            }
        },
        "eventCharacters": {
            "name": "Event Characters",
            "length": 16,
            "default": "Character %i",
            "string": {
                "0-14": "<characterNames[%i]>"
            }
        },
        "eventObjects": {
            "name": "Event Objects",
            "length": 52,
            "default": "Object %i",
            "string": {
                "0-16": "<stringTable.eventCharacters[%i]>",
                "16-48": "NPC %i",
                "48": "Camera",
                "49": "Party Character 1",
                "50": "Party Character 2",
                "51": "Party Character 3",
                "52": "Party Character 4"
            }
        },
        "eventSwitches": {
            "name": "Event Switches",
            "length": "0x0700",
            "default": "Switch %i",
            "string": {
                "0x0000-0x0300": "<stringTable.mapSwitches[%i]>",
                "0x0300-0x0700": "<stringTable.npcSwitches[%i-0x0300]>"
            }
        },
        "eventVariables": {
            "name": "Event Variables",
            "length": 8,
            "default": "Variable %i",
            "string": {
                "0": "Points from the Emperor's Banquet",
                "1": "Narshe security checkpoint variable",
                "6": "Number of dragons left",
                "7": "Cid's health/Pieces of coral"
            }
        },
        "fixedColor": {
            "name": "Fixed Color",
            "string": {
                "0": "Black",
                "1": "Red",
                "2": "Green",
                "3": "Yellow",
                "4": "Blue",
                "5": "Magenta",
                "6": "Cyan",
                "7": "White"
            }
        },
        "fixedColorReversed": {
            "name": "Fixed Color (Reversed)",
            "string": {
                "0": "Black",
                "1": "Blue",
                "2": "Green",
                "3": "Cyan",
                "4": "Red",
                "5": "Magenta",
                "6": "Yellow",
                "7": "White"
            }
        },
        "itemSpecialEffect": {
            "name": "Item Special Effect",
            "hideIndex": true,
            "string": {
                "0": "None",
                "1": "<stringTable.itemNames[249]>",
                "2": "<stringTable.itemNames[250]>",
                "3": "<stringTable.itemNames[252]>",
                "4": "<stringTable.itemNames[238]>, <stringTable.itemNames[239]>",
                "5": "<stringTable.itemNames[253]>",
                "6": "<stringTable.itemNames[254]>",
                "8": "<stringTable.battleCommandName[28]>",
                "9": "<stringTable.battleCommandName[24]>",
                "10": "<stringTable.battleCommandName[5]>",
                "11": "<stringTable.battleCommandName[14]>",
                "12": "<stringTable.battleCommandName[17]>",
                "13": "<stringTable.battleCommandName[13]>",
                "14": "<stringTable.itemNames[167]>",
                "15": "<stringTable.itemNames[169]>",
                "255": "None"
            }
        },
        "loreName": {
            "name": "Lore Name",
            "length": 24,
            "link": "attackName[%i+0x3A]"
        },
        "mapSpriteActions": {
            "name": "Map Sprite Actions",
            "default": "Action %i",
            "string": {
                "0x00": "Walking Down 1",
                "0x01": "Facing Down",
                "0x02": "Walking Down 2",
                "0x03": "Walking Up 1",
                "0x04": "Facing Up",
                "0x05": "Walking Up 2",
                "0x06": "Walking Left 1",
                "0x07": "Facing Left",
                "0x08": "Walking Left 2",
                "0x09": "Kneeling, Facing Left",
                "0x0A": "Ready to Attack, Facing Left",
                "0x0B": "Hit",
                "0x0C": "Facing Left, Battle",
                "0x0D": "Back Arm Raised, Facing Left",
                "0x0E": "Front Arm Raised, Facing Left",
                "0x0F": "Arms Up, Facing Left",
                "0x10": "Casting 1",
                "0x11": "Casting 2",
                "0x12": "Knocked Out",
                "0x13": "Eyes Closed, Facing Down",
                "0x14": "Winking",
                "0x15": "Eyes Closed, Facing Left",
                "0x16": "Arms Up, Facing Down",
                "0x17": "Arms Up, Facing Up",
                "0x18": "Angry",
                "0x19": "Waving 1, Facing Down",
                "0x1A": "Waving 2, Facing Down",
                "0x1B": "Waving 1, Facing Up",
                "0x1C": "Waving 2, Facing Up",
                "0x1D": "Laughing 1",
                "0x1E": "Laughing 2",
                "0x1F": "Surprised",
                "0x20": "Looking Down, Facing Down",
                "0x21": "Looking Down, Facing Up",
                "0x22": "Looking Down, Facing Left",
                "0x23": "Head Turned Left, Facing Down",
                "0x24": "Wagging Finger 1",
                "0x25": "Wagging Finger 2",
                "0x26": "Terra: Crying",
                "0x27": "Tent",
                "0x28": "Knocked Out, Alternate",
                "0x29": "NPC: Special",
                "0x2A": "NPC: Waving 1",
                "0x2B": "NPC: Waving 2",
                "0x2C": "NPC: Looking Down",
                "0x2D": "NPC: Special",
                "0x2E": "Riding 1",
                "0x2F": "Riding 2",
                "0x30": "Ramuh: Raising Staff",
                "0x31": "Ramuh: Eyes Closed",
                "0x32": "Special Animation: Frame 1",
                "0x33": "Special Animation: Frame 2",
                "0x34": "Special Animation: Frame 3",
                "0x35": "Special Animation: Frame 4",
                "0x36": "Opera Singer: Mouth Open",
                "0x37": "Opera Singer: Mouth Closed"
            }
        },
        "mapSprites": {
            "name": "Map Sprites",
            "default": "Map Sprite %i",
            "string": {
                "0": "Terra",
                "1": "Locke",
                "2": "Cyan",
                "3": "Shadow",
                "4": "Edgar",
                "5": "Sabin",
                "6": "Celes",
                "7": "Strago",
                "8": "Relm",
                "9": "Setzer",
                "10": "Mog",
                "11": "Gau",
                "12": "Gogo",
                "13": "Umaro",
                "14": "Soldier",
                "15": "Imp",
                "16": "Leo",
                "17": "Banon",
                "18": "Esper Terra",
                "19": "Merchant",
                "20": "Ghost",
                "21": "Kefka",
                "22": "Gestahl",
                "23": "Old Man",
                "24": "Man",
                "25": "Dog",
                "26": "Celes w/ Dress",
                "27": "Rich Man",
                "28": "Draco",
                "29": "Arvis",
                "30": "Pilot",
                "31": "Ultros",
                "32": "Spiffy Gau",
                "33": "Hooker",
                "34": "Chancellor",
                "35": "Baram",
                "36": "Old Woman",
                "37": "Woman",
                "38": "Boy",
                "39": "Girl",
                "40": "Bird",
                "41": "Rachel",
                "42": "Katarin",
                "43": "Impresario",
                "44": "Esper Elder",
                "45": "Yura",
                "46": "Siegfried",
                "47": "Cid",
                "48": "Maduin",
                "49": "Bandit",
                "50": "Vargas/Dadaluma",
                "51": "Monster",
                "52": "Narshe Guard",
                "53": "Train Conductor",
                "54": "Shopkeeper",
                "55": "Faerie",
                "56": "Wolf",
                "57": "Dragon",
                "58": "Fish",
                "59": "Figaro Guard",
                "60": "Darill",
                "61": "Chupon",
                "62": "Emperor's Servant",
                "63": "Ramuh",
                "64": "Figaro Guard Riding",
                "65": "Celes in Chains",
                "66": "Gau as Sabin",
                "67": "Gau as Locke",
                "68": "King Doma",
                "69": "Number 128",
                "70": "Magi Warrior A",
                "71": "Skull Statue",
                "72": "Ifrit",
                "73": "Phantom",
                "74": "Shiva",
                "75": "Unicorn",
                "76": "Bismark",
                "77": "Carbuncl",
                "78": "Shoat",
                "79": "Owzer A",
                "80": "Owzer B",
                "81": "Blackjack",
                "82": "Dead Figaro Guard",
                "83": "Number 024",
                "84": "Treasure Chest",
                "85": "Magi Warrior B",
                "86": "Atma",
                "87": "Small Statue",
                "88": "Flowers",
                "89": "Envelope",
                "90": "Plant",
                "91": "Magicite",
                "92": "Book",
                "93": "Baby",
                "94": "Question Mark",
                "95": "Exclamation Point",
                "96": "Slave Crown",
                "97": "Weight",
                "98": "Bird w/ Bandana",
                "99": "Eyes in the Dark",
                "100": "Bandana",
                "101": "Nothing",
                "102": "Bird Flying Left",
                "103": "Bird Flying Up",
                "104": "Big Sparkle",
                "105": "Multiple Sparkles",
                "106": "Small Sparkle",
                "107": "Coin",
                "108": "Rat",
                "109": "Turtle",
                "110": "Small Bird Flying Up",
                "111": "Save Point",
                "112": "Flame",
                "113": "Explosion",
                "114": "Tentacle A",
                "115": "Tentacle B",
                "116": "Big Switch",
                "117": "Floor Switch",
                "118": "Rock",
                "119": "Crane Hook C",
                "120": "Elevator",
                "121": "Flying Esper Terra A1",
                "122": "Flying Esper Terra A2",
                "123": "Ending Terra C",
                "124": "Diving Helmet",
                "125": "Guardian A",
                "126": "Guardian B",
                "127": "Guardian C",
                "128": "Crane Hook B",
                "129": "Guardian D",
                "130": "Guardian E",
                "131": "Guardian F",
                "132": "Crane Hook A",
                "133": "Magitek Factory Machine",
                "134": "Gate A",
                "135": "Gate B",
                "136": "Gate C",
                "137": "Air Force",
                "138": "Leo's Sword",
                "139": "Magitek Train Car A",
                "140": "Magitek Train Car B",
                "141": "Magitek Train Car C",
                "142": "Magitek Train Car D",
                "143": "Crane A",
                "144": "Crane B",
                "145": "Crane C",
                "146": "Chadarnook A",
                "147": "Chadarnook B",
                "148": "Chadarnook C",
                "149": "Falcon A",
                "150": "Falcon B",
                "151": "Falcon C",
                "152": "Flying Esper Terra B",
                "153": "Tritoch",
                "154": "Odin",
                "155": "Goddess A",
                "156": "Doom A",
                "157": "Poltergeist A",
                "158": "Goddess B",
                "159": "Goddess C",
                "160": "Doom/Poltergeist B",
                "161": "Doom/Poltergeist C",
                "162": "Ending Terra A",
                "163": "Ending Terra B",
                "164": "Small Bird Flying Left"
            }
        },
        "mapSwitches": {
            "name": "Map Switches",
            "path": "Field/Events",
            "length": 768,
            "default": "Map Switch %i",
            "string": {
                "1": "Terra Intro Event",
                "3": "Locke Intro Event",
                "4": "Edgar Intro Event",
                "5": "Sabin Intro Event",
                "6": "Figaro Castle / Kefka Event",
                "7": "Figaro Castle / Sabin Event",
                "8": "Figaro Castle / Following Locke",
                "10": "South Figaro / Shadow walking to pub",
                "11": "Shadow Intro Event",
                "13": "Mt. Kolts / Vargas Shadow 1",
                "14": "Mt. Kolts / Vargas Shadow 2",
                "15": "Mt. Kolts / Vargas Shadow 3",
                "16": "Mt. Kolts / Vargas Event",
                "17": "Returner's Hideout / Met Banon",
                "20": "Returner's Hideout / Declined Once",
                "21": "Returner's Hideout / Declined Twice",
                "22": "Returner's Hideout / Declined Three Times",
                "23": "Returner's Hideout / Wounded Returner Event",
                "24": "Returner's Hideout / Banon joined",
                "25": "Lete River / First Battle",
                "26": "Lete River / Ultros Battle",
                "27": "South Figaro / Celes Intro",
                "28": "South Figaro / Celes Named",
                "29": "South Figaro / Celes Rescued",
                "30": "Cave to South Figaro / TunnelArmr Battle",
                "31": "Narshe / Banon & Guard",
                "32": "Narshe / Secret Entrance",
                "33": "Narshe / Banon & Elder",
                "34": "Crazy Old Man's House / Stove",
                "36": "Shadow's First Dream",
                "38": "Shadow's Second Dream",
                "39": "Shadow's Third Dream",
                "40": "Shadow's Fourth Dream",
                "41": "Kefka's Tower / Intro Event",
                "42": "Floating Island / Met Shadow",
                "43": "Imperial Camp / Second Leo Event",
                "44": "Imperial Camp / Kefka 1",
                "45": "Imperial Camp / Kefka 2",
                "46": "Imperial Camp / Kefka 3",
                "47": "Imperial Camp / Kefka 4",
                "48": "Doma Castle / Cyan Intro",
                "49": "Doma Castle / King Doma",
                "50": "Doma Castle / Cyan's Family",
                "51": "Imperial Camp / Cyan 1",
                "52": "Imperial Camp / Cyan 2",
                "53": "Imperial Camp / Cyan 3",
                "54": "Imperial Camp / Cyan 4",
                "55": "Imperial Camp / After Escape",
                "56": "Phantom Train / Intro Event",
                "57": "Phantom Train / Train Departed",
                "58": "Phantom Forest / Train Gone",
                "59": "Phantom Train / GhostTrain Battle",
                "60": "Baren Falls / Intro Event",
                "63": "Gau Intro Event",
                "65": "Crescent Mountain / Diving Helmet Event",
                "67": "Nikeah / Cyan & Hooker Event",
                "68": "Nikeah / Sabin & Cyan Travel to Narshe",
                "69": "Narshe / After 3 Scenarios",
                "70": "Narshe / After Snowfield Battle",
                "71": "Narshe / Following Terra",
                "72": "Figaro Castle / Sabin is wandering around",
                "73": "Figaro Castle / Edgar & Sabin Event",
                "74": "South Figaro / Sabin & Duncan's Wife Event",
                "75": "Kohlingen / Rachel Flashback Event",
                "76": "Kohlingen / Rachel & Crazy Man Event",
                "83": "Zozo / Ramuh Intro Event",
                "84": "Zozo / After Ramuh",
                "94": "Reached Southern Continent",
                "95": "Magitek Factory / Kefka & Espers Event",
                "96": "Magitek Factory / Ifrit & Shiva Event",
                "104": "Magitek Research Facility / Kefka & Celes Event",
                "105": "Magitek Research Facility / Start of Train Ride",
                "106": "Magitek Research Facility / End of Train Ride",
                "107": "Magitek Research Facility / Crane Battle",
                "108": "Esper World / Intro Event",
                "109": "Esper World / Maduin & Madonna Event",
                "110": "Esper World / Terra is Born",
                "112": "Esper World / End of Flashback",
                "121": "Cave to the Sealed Gate / After Gate Event",
                "122": "Cave to the Sealed Gate / Airship Crashed",
                "123": "Vector / After Esper Attack",
                "124": "Vector / Before Banquet",
                "125": "Vector / After Banquet",
                "126": "Umaro Intro Event",
                "127": "Kefka's Tower / Rock Slide",
                "128": "Kefka's Tower / Gate",
                "129": "Blackjack / Cid & Setzer",
                "131": "Albrook / Boarded Ship",
                "133": "Albrook / Met Leo",
                "134": "Albrook / Terra & Leo at Night",
                "135": "Albrook / Locke & Celes at Night",
                "137": "Albrook / Reached Island",
                "139": "Thamasa / Fire Kid",
                "140": "Thamasa / Cure Kid",
                "141": "Thamasa / Named Strago and Relm",
                "142": "Thamasa / Woken up at Inn",
                "143": "Thamasa / Burning Building Intro",
                "144": "Thamasa / FlameEater",
                "145": "Thamasa / Heading to Esper Mountain",
                "146": "Thamasa / Shadow Left",
                "149": "Esper Mountain / Ultros",
                "151": "Esper Mountain / Statues",
                "152": "World of Balance / Esper Mountain Visible",
                "153": "Esper Mountain / Met Espers",
                "154": "Albrook / Play Shadow's Music",
                "155": "Thamasa / Fought Kefka",
                "156": "Thamasa / Leo's Grave",
                "157": "Thamasa / Events Completed",
                "158": "Floating Island is Accessible",
                "159": "Floating Island is not Accessible",
                "160": "Floating Island / IAF",
                "163": "Floating Island / Left Shadow Behind",
                "164": "World of Ruin",
                "166": "Hide Party on Map 10",
                "167": "Met Crimson Robber 1",
                "168": "Met Crimson Robber 2",
                "169": "Met Crimson Robber 3",
                "170": "Met Crimson Robber 4",
                "171": "Crimson Robber Left Cafe",
                "172": "On Crimson Robber Ship",
                "173": "Cave to South Figaro / Music ???",
                "174": "Cave to South Figaro / Noise 1",
                "175": "Cave to South Figaro / Noise 2",
                "176": "Cave to South Figaro / Noise 3",
                "177": "Cave to South Figaro / Recovery Spring",
                "178": "Solitary Island / Examined Dead Cid",
                "179": "Solitary Island / Cid Recovered",
                "180": "Solitary Island / Cid Died",
                "182": "Narshe / Got Ragnarok",
                "184": "Narshe / Got Cursed Shield",
                "186": "Fanatics' Tower / Got Strago",
                "188": "Kefka's Tower / Guardian",
                "189": "Kefka's Tower / Inferno",
                "190": "Mobliz / Terra & Katarin Missing",
                "191": "Mobliz / Got Terra",
                "194": "Darill's Tomb / DLRO",
                "195": "Darill's Tomb / ERAU",
                "196": "Darill's Tomb / QSSI",
                "197": "Darill's Tomb / WEHT",
                "198": "Figaro Castle / Got Edgar (WoR)",
                "199": "Figaro Castle / Tentacles",
                "200": "Ancient Castle / Got Odin",
                "202": "Kohlingen / Got Setzer",
                "203": "Darill's Tomb / Opened Entrance",
                "204": "Darill's Tomb / Flashback Scene",
                "205": "Darill's Tomb / Got Falcon",
                "208": "Kefka's Tower / Two Doorways",
                "210": "Mt. Zozo / Got Cyan",
                "212": "Gogo's Cave / Got Gogo",
                "215": "Phoenix Cave / Got Locke",
                "217": "Owzer's House / Got Starlet",
                "218": "Doma Castle / Cyan's Dream",
                "220": "World of Ruin / Figaro Castle -> Kohlingen",
                "222": "Figaro Castle / On Fire",
                "256": "Sabin's House / Edgar Dialogue",
                "257": "South Figaro / Merchant -> Old Man's House",
                "258": "South Figaro / Merchant -> Pub",
                "259": "South Figaro / Locke is a Green Soldier",
                "260": "South Figaro / Locke is a Merchant",
                "261": "South Figaro / Is Occupied",
                "262": "World of Ruin / Figaro Castle -> East",
                "263": "South Figaro / Delivered Cider",
                "264": "Cave to South Figaro / Guard will Leave",
                "266": "Mt. Kolts / Vargas Hops Down",
                "267": "World of Balance / Figaro Castle -> East",
                "268": "World of Balance / Figaro Castle -> Kohlingen",
                "269": "South Figaro / Wound the Clock",
                "270": "Named Setzer",
                "272": "Opera House / Ultros Intro",
                "273": "Opera House / Performed Correctly",
                "274": "Darill's Tomb / Solved Tomb Puzzle",
                "275": "Opera House / Failed Once",
                "276": "Opera House / Failed Twice",
                "277": "Opera House / Failed Three Times",
                "278": "Esperville / Event 1",
                "279": "Esperville / Event 2",
                "280": "Esperville / Event 3",
                "281": "Opera House / Shadow Left",
                "291": "Cave to South Figaro / Can Jump on Turtle",
                "292": "Number of Characters in Party / Bit 1",
                "293": "Number of Characters in Party / Bit 2",
                "294": "Number of Characters in Party / Bit 3",
                "295": "Disable Warp",
                "296": "Narshe / First Guards",
                "297": "Narshe / Second Guards",
                "298": "Narshe / Third Guards",
                "299": "Narshe / Entered Cave",
                "300": "Narshe / Whelk Gate",
                "304": "Narshe / Fourth Guards",
                "305": "Narshe / Fifth Guards",
                "309": "Narshe / Whelk",
                "318": "Narshe / Green Soldier 1",
                "319": "Narshe / Green Soldier 2",
                "320": "Narshe / Green Soldier 3",
                "321": "Narshe / Green Soldier 4",
                "322": "Narshe / Green Soldier 5",
                "323": "Narshe / Green Soldier 6",
                "324": "Narshe / Brown Soldier 1",
                "325": "Narshe / Brown Soldier 2",
                "326": "Narshe / Brown Soldier 3",
                "327": "Narshe / Brown Soldier 5",
                "328": "Narshe / Brown Soldier 6",
                "364": "Jidoor / Bought ZoneSeek",
                "365": "Jidoor / Bought Golem",
                "366": "Cave in the Veldt / Switch",
                "367": "Learned Airship Controls",
                "416": "<characterNames[0]>",
                "417": "<characterNames[1]>",
                "418": "<characterNames[2]>",
                "419": "<characterNames[3]>",
                "420": "<characterNames[4]>",
                "421": "<characterNames[5]>",
                "422": "<characterNames[6]>",
                "423": "<characterNames[7]>",
                "424": "<characterNames[8]>",
                "425": "<characterNames[9]>",
                "426": "<characterNames[10]>",
                "427": "<characterNames[11]>",
                "428": "<characterNames[12]>",
                "429": "<characterNames[13]>",
                "430": "Character 14",
                "431": "Character 15",
                "432": "Party is facing up",
                "433": "Party is facing right",
                "434": "Party is facing down",
                "435": "Party is facing left",
                "436": "A button is pressed",
                "439": "Serpent Trench arrow is pointing left",
                "440": "Alternate World Map Music",
                "441": "Airship is Visible",
                "442": "Airship is Anchored",
                "443": "Party is on the veldt",
                "446": "Not enough GP",
                "447": "Party is on a save point",
                "449": "Disable sprint shoes",
                "450": "Disable main menu",
                "454": "Enable character portrait",
                "459": "Thamasa / Play Relm's Song",
                "460": "Song Override",
                "461": "Disable Random Battles",
                "462": "Enable Party Switching",
                "463": "Map Event is Running",
                "464": "<stringTable.rareItemName[0]>",
                "465": "<stringTable.rareItemName[1]>",
                "466": "<stringTable.rareItemName[2]>",
                "467": "<stringTable.rareItemName[3]>",
                "468": "<stringTable.rareItemName[4]>",
                "469": "<stringTable.rareItemName[5]>",
                "470": "<stringTable.rareItemName[6]>",
                "471": "<stringTable.rareItemName[7]>",
                "472": "<stringTable.rareItemName[8]>",
                "473": "<stringTable.rareItemName[9]>",
                "474": "<stringTable.rareItemName[10]>",
                "475": "<stringTable.rareItemName[11]>",
                "476": "<stringTable.rareItemName[12]>",
                "477": "<stringTable.rareItemName[13]>",
                "478": "<stringTable.rareItemName[14]>",
                "479": "<stringTable.rareItemName[15]>",
                "480": "<stringTable.rareItemName[16]>",
                "481": "<stringTable.rareItemName[17]>",
                "482": "<stringTable.rareItemName[18]>",
                "483": "<stringTable.rareItemName[19]>",
                "484": "Unused Rare Item 21",
                "485": "Unused Rare Item 22",
                "486": "Unused Rare Item 23",
                "487": "Unused Rare Item 24",
                "488": "Unused Rare Item 25",
                "489": "Unused Rare Item 26",
                "490": "Unused Rare Item 27",
                "491": "Unused Rare Item 28",
                "494": "Selected a Valid Colosseum Item",
                "495": "Fighting Shadow in the Colosseum",
                "496": "Local Control 1",
                "497": "Local Control 2",
                "498": "Local Control 3",
                "499": "Local Control 4",
                "500": "Local Control 5",
                "501": "Local Control 6",
                "502": "Local Control 7",
                "503": "Local Control 8",
                "504": "Local Control 9",
                "505": "Local Control 10",
                "506": "Local Control 11",
                "507": "Local Control 12",
                "508": "Local Control 13",
                "509": "Local Control 14",
                "510": "Local Control 15",
                "511": "Local Control 16",
                "512": "Imperial Castle / Talked to 2 soldiers",
                "513": "Imperial Castle / Talked to 3 soldiers",
                "514": "Imperial Castle / Talked to 4 soldiers",
                "515": "Imperial Castle / Talked to 5 soldiers",
                "516": "Imperial Castle / Talked to 6 soldiers",
                "517": "Imperial Castle / Talked to 7 soldiers",
                "518": "Imperial Castle / Talked to 8 soldiers",
                "519": "Imperial Castle / Talked to 9 soldiers",
                "520": "Imperial Castle / Talked to 10 soldiers",
                "521": "Imperial Castle / Talked to 11 soldiers",
                "522": "Imperial Castle / Talked to 12 soldiers",
                "523": "Imperial Castle / Talked to 13 soldiers",
                "524": "Imperial Castle / Talked to 14 soldiers",
                "525": "Imperial Castle / Talked to 15 soldiers",
                "526": "Imperial Castle / Talked to 16 soldiers",
                "527": "Imperial Castle / Talked to 17 soldiers",
                "528": "Imperial Castle / Talked to 18 soldiers",
                "529": "Imperial Castle / Talked to 19 soldiers",
                "530": "Imperial Castle / Talked to 20 soldiers",
                "531": "Imperial Castle / Talked to 21 soldiers",
                "532": "Imperial Castle / Talked to 22 soldiers",
                "533": "Imperial Castle / Talked to 23 soldiers",
                "534": "Imperial Castle / Talked to 24 soldiers",
                "535": "Imperial Castle / Talked to soldier 1",
                "536": "Imperial Castle / Talked to soldier 2",
                "537": "Imperial Castle / Talked to soldier 3",
                "538": "Imperial Castle / Talked to soldier 4",
                "539": "Imperial Castle / Talked to soldier 5",
                "540": "Imperial Castle / Talked to soldier 6",
                "541": "Imperial Castle / Talked to soldier 7",
                "542": "Imperial Castle / Talked to soldier 8",
                "543": "Imperial Castle / Talked to soldier 9",
                "544": "Imperial Castle / Talked to soldier 10",
                "545": "Imperial Castle / Talked to soldier 11",
                "546": "Imperial Castle / Talked to soldier 12",
                "547": "Imperial Castle / Talked to soldier 13",
                "548": "Imperial Castle / Talked to soldier 14",
                "549": "Imperial Castle / Talked to soldier 15",
                "550": "Imperial Castle / Talked to soldier 16",
                "551": "Imperial Castle / Talked to soldier 17",
                "552": "Imperial Castle / Talked to soldier 18",
                "553": "Imperial Castle / Talked to soldier 19",
                "554": "Imperial Castle / Talked to soldier 20",
                "555": "Imperial Castle / Talked to soldier 21",
                "556": "Imperial Castle / Talked to soldier 22",
                "557": "Imperial Castle / Talked to soldier 23",
                "558": "Imperial Castle / Talked to soldier 24",
                "665": "Mt. Kolts / Storm Dragon is Flying Around",
                "666": "Mt. Kolts / Storm Dragon was Defeated",
                "683": "Retain Timers 1",
                "684": "Retain Timers 2",
                "685": "Retain Timers 3",
                "686": "Retain Timers 4",
                "700": "Continue Music During Battle",
                "704": "Enable Fade Bars (ending)",
                "708": "Party 1 switch A",
                "709": "Party 1 switch B",
                "710": "Party 1 switch C",
                "711": "Party 1 switch D",
                "712": "Party 2 switch A",
                "713": "Party 2 switch B",
                "714": "Party 2 switch C",
                "715": "Party 2 switch D",
                "716": "Party 3 switch A",
                "717": "Party 3 switch B",
                "718": "Party 3 switch C",
                "719": "Party 3 switch D",
                "720": "Phoenix Cave / Water is drained",
                "721": "Phoenix Cave / Spikes have been lowered 3",
                "722": "Phoenix Cave / Stones rearranged (party 1)",
                "723": "Phoenix Cave / Stones rearranged (party 2)",
                "724": "Phoenix Cave / Stepping stones (party 1)",
                "725": "Phoenix Cave / Stepping stones (party 2)",
                "726": "Phoenix Cave / Lowered platform",
                "727": "Umaro's Cave / Obtained Fenrir",
                "728": "Narshe / Security Checkpoint Intro",
                "729": "Kohlingen / Disable music in Rachel's house",
                "730": "Fanatics' Tower / Top room treasure chest",
                "731": "Fanatics' Tower / Defeated MagiMaster",
                "732": "Fanatics' Tower / Pressed switch",
                "733": "Ancient Castle / Obtained Raiden",
                "734": "Ancient Castle / Pressed switch",
                "735": "Ancient Castle / Flashback scene",
                "736": "Terra is initialized",
                "737": "Locke is initialized",
                "738": "Cyan is initialized",
                "739": "Shadow is initialized",
                "740": "Edgar is initialized",
                "741": "Sabin is initialized",
                "742": "Celes is initialized",
                "743": "Strago is initialized",
                "744": "Relm is initialized",
                "745": "Setzer is initialized",
                "746": "Mog is initialized",
                "747": "Gau is initialized",
                "748": "Gogo is initialized",
                "749": "Umaro is initialized",
                "750": "Character 15 is initialized",
                "751": "Character 16 is initialized",
                "752": "Terra is available",
                "753": "Locke is available",
                "754": "Cyan is available",
                "755": "Shadow is available",
                "756": "Edgar is available",
                "757": "Sabin is available",
                "758": "Celes is available",
                "759": "Strago is available",
                "760": "Relm is available",
                "761": "Setzer is available",
                "762": "Mog is available",
                "763": "Gau is available",
                "764": "Gogo is available",
                "765": "Umaro is available",
                "766": "Go to Narshe scene after Magitek March",
                "767": "At least one saved game"
            }
        },
        "mapVRAMPalette": {
            "name": "Map VRAM Palette",
            "string": {
                "0": "Background Palette 0",
                "1": "Background Palette 1",
                "2": "Background Palette 2",
                "3": "Background Palette 3",
                "4": "Background Palette 4",
                "5": "Background Palette 5",
                "6": "Background Palette 6",
                "7": "Background Palette 7",
                "8": "Sprite Palette 0",
                "9": "Sprite Palette 1",
                "10": "Sprite Palette 2",
                "11": "Sprite Palette 3",
                "12": "Sprite Palette 4",
                "13": "Sprite Palette 5",
                "14": "Sprite Palette 6",
                "15": "Sprite Palette 7"
            }
        },
        "monsterAnimation": {
            "name": "Monster Animation",
            "string": {
                "0": "Flash Red",
                "1": "Move Back 8 (slow)",
                "2": "Move Forward 8 (slow)",
                "3": "Move Back 8 (instantly)",
                "4": "Move Forward 8 (instantly)",
                "5": "Characters Run Right",
                "6": "Characters Run Left",
                "7": "Move Back 64 (instantly)",
                "8": "Move Forward 64 (instantly)",
                "10": "Kefka Head",
                "11": "Atma Glow (long)",
                "12": "Atma Glow (short)",
                "13": "Kefka Death"
            }
        },
        "npcSwitches": {
            "name": "NPC Switches",
            "path": "Field/Events",
            "length": 1024,
            "default": "NPC %i",
            "string": {
                "0": "Always Visible",
                "2": "Figaro Castle: Guards Blocking Stairs",
                "3": "South Figaro: NPC's (Not Occupied)",
                "4": "South Figaro: NPC's",
                "5": "South Figaro: Shadow (Cafe)",
                "6": "Sabin's House: Old Man",
                "7": "South Figaro: Merchant (Cafe)",
                "8": "FIgaro Castle: Edgar (Throne Room)",
                "9": "Figaro Castle: NPC's (Throne Room)",
                "10": "South Figaro: Merchant (Old Man's House)",
                "11": "Figaro Castle: NPC's (Exterior)",
                "12": "South Figaro: NPC's (When Occupied)",
                "13": "Figaro Castle: Guards Blocking Doors",
                "14": "Figaro Castle: Guards Next to Doors",
                "15": "Figaro Castle: NPC's (Interior)",
                "16": "Figaro Castle: NPC's (Interior)",
                "17": "Figaro Castle: Locke (Exterior)",
                "18": "Cave to South Figaro: Guards (Outside)",
                "19": "Figaro Castle: Locke (East Tower)",
                "20": "Zozo: Esper Terra",
                "21": "Figaro Castle: Top Guard",
                "22": "Figaro Castle: Chancellor (Interior)",
                "23": "South Figaro: Celes in Chains",
                "24": "South Figaro: Upper Green Soldier",
                "25": "South Figaro: Lower Green Soldier",
                "27": "South Figaro: Brown Soldier Who Takes a Break",
                "28": "Mt. Kolts: Vargas",
                "29": "Cave to South Figaro/Mt. Kolts: Soldiers",
                "30": "Zozo: Ramuh",
                "31": "Zozo: Ramuh Magicite",
                "32": "Zozo: Siren Magicite",
                "33": "Zozo: Kirin Magicite",
                "34": "Zozo: Stray Magicite",
                "41": "3 Scenarios / Locke",
                "42": "3 Scenarios / Sabin",
                "43": "3 Scenarios / Banon",
                "44": "3 Scenarios / Terra",
                "45": "3 Scenarios / Edgar",
                "46": "Mt. Kolts: Shadow Sabin (Upper Exterior)",
                "47": "Jidoor: Impresario (Owzer's House)",
                "49": "Jidoor: Letter (Owzer's House)",
                "50": "Opera House: Impresario (Lobby)",
                "55": "Esper World: Yura",
                "56": "Esper World: Madonna (Exterior)",
                "57": "Esper World: Madonna (In Bed)",
                "58": "Esper World: Fairy (Interior)",
                "59": "Esper World: Elder (Interior)",
                "60": "Esper World: NPC's (Interior)",
                "61": "Esper World: NPC's (Exterior)",
                "62": "Esper World: Elder (Exterior)",
                "64": "Opera House: Impresario Blocking Entrance (Lobby)",
                "65": "Opera House: Man Blocking Entrance (Lobby)",
                "66": "Opera House: Ultros (Lobby)",
                "67": "Opera House: Draco A (Intro)",
                "68": "Opera House: Draco B (Intro)",
                "69": "Opera House: Letter (Backstage)",
                "70": "Opera House: Lower Dancing Couple",
                "71": "Opera House: Draco/Fighting Soldiers",
                "72": "Opera House: Upper Dancing Couples",
                "73": "Opera House: Actors (Catwalk)",
                "74": "Zozo: Dadaluma",
                "75": "Opera House: Ultros (Catwalk)",
                "76": "Opera House: Rat A",
                "77": "Opera House: Rat B",
                "78": "Opera House: Rat C",
                "79": "Opera House: Rat D",
                "80": "Opera House: Rat E",
                "85": "Opera House: Blocked Door to Catwalk",
                "86": "Zozo: Rusted Door",
                "87": "Esper World: Madonna (Cave)",
                "88": "South Figaro: Sprint Shoes Guy",
                "89": "Figaro Castle: Lone Wolf (Prison)",
                "90": "Esper World: Baby Terra (Interior)",
                "91": "Esper World: Madonna (Interior)",
                "92": "Esper World: Elder (Sealing Gate)",
                "93": "Esper World: Gestahl/Soldiers (Cave)",
                "94": "Floating Island: Shadow",
                "95": "Floating Island: Atma Weapon",
                "96": "South Figaro: Magitek Armor",
                "97": "Floating Island: Nerapa",
                "102": "Opera House: Letter (Lobby)",
                "103": "Solitary Island: Cid in Bed",
                "104": "Solitary Island: Blocking Stairs",
                "105": "Solitary Island: A yummy fish",
                "106": "Solitary Island: Just a fish",
                "107": "Solitary Island: A rotten fish",
                "108": "Solitary Island: Fish",
                "109": "Solitary Island: Raft in Basement",
                "110": "Solitary Island: Cid Walking Around",
                "112": "Solitary Island: Bird (Cliff)",
                "113": "Solitary Island: Bird (Beach)",
                "114": "Solitary Island: Letter",
                "115": "Nikeah: Old Woman (Cafe)",
                "116": "Nikeah: Thieves (Cafe)",
                "117": "Nikeah: Thieves (Exterior)",
                "118": "Nikeah: Gerad (Exterior)",
                "119": "Nikeah: Thieves (Docks)",
                "120": "Nikeah: Gerad (Docks)",
                "121": "South Figaro: Other NPC's ???",
                "122": "South Figaro: Thieves (WoR)",
                "123": "South Figaro: Male Lover (WoR)",
                "124": "South Figaro: Female Lover (WoR)",
                "125": "Blackjack: Shadow ???",
                "126": "South Figaro: Gerad (Inn)",
                "127": "Cave to South Figaro: Gerad/Thieves",
                "128": "Figaro Castle: Prisoners",
                "129": "Figaro Castle: Guard (Prison)",
                "130": "Figaro Castle: Dead Guards",
                "131": "Figaro Castle: Gerad (Prison)",
                "132": "Zozo: Bird",
                "133": "Zozo: Ghost",
                "134": "Opera House: Man (Lobby)",
                "135": "Opera House: Dirt Dragon",
                "136": "Falcon: Terra",
                "137": "Falcon: Locke",
                "138": "Falcon: Shadow",
                "139": "Falcon: Cyan",
                "140": "Falcon: Edgar",
                "141": "Falcon: Sabin",
                "142": "Falcon: Celes",
                "143": "Falcon: Strago",
                "144": "Falcon: Relm",
                "145": "Falcon: Setzer",
                "146": "Falcon: Mog",
                "147": "Falcon: Gau",
                "148": "Falcon: Gogo",
                "149": "Falcon: Umaro",
                "150": "Darill's Tomb: Blocked Door Underwater",
                "151": "Figaro Castle: Locked Doors While Submerged",
                "152": "Cave to South Figaro: Siegfried (Tunnel)",
                "153": "Cave to South Figaro: Siegfried (South Entrance)",
                "154": "Kefka's Tower: Kefka",
                "155": "Solitary Island: Palidor Magicite",
                "157": "Kohlingen: Plant",
                "158": "Kefka's Tower: Sparkles (Top)",
                "159": "Falcon: NPC's (Cutscenes)",
                "160": "Character Portrait",
                "240": "Figaro Castle: Tentacles (Engine Room)",
                "241": "Figaro Castle: Gerad (Engine Room)",
                "242": "Figaro Castle: Thieves (Engine Room)",
                "243": "Falcon: Darill",
                "244": "Opera House: Draco/Maria (Fight Scene)",
                "254": "Local Control 2",
                "255": "Local Control 1",
                "259": "Imperial Camp: Leo",
                "260": "Imperial Camp: Kefka (Tent)",
                "261": "Imperial Camp: Kefka (Poison)",
                "275": "Returner's Hideout: First Soldier",
                "278": "Returner's Hideout: Sabin",
                "279": "Returner's Hideout: Banon (Main Room)",
                "284": "Returner's Hideout: Wounded Soldier",
                "285": "Returner's Hideout: Banon (Back Room)",
                "286": "Returner's Hideout: Treasure Room Soldier",
                "287": "Returner's Hideout: Edgar",
                "288": "Returner's Hideout: Locke",
                "289": "Returner's Hideout: Banon (Entrance)",
                "290": "Lete River: Ultros",
                "293": "Crazy Old Man's House: Crazy Old Man",
                "294": "Crazy Old Man's House: Shadow",
                "295": "Crazy Old Man's House: Interceptor",
                "296": "Lete River: Raft (Exterior)",
                "297": "Imperial Camp: Magitek Armor A",
                "298": "Imperial Camp: Magitek Armor B",
                "299": "Imperial Camp: Cyan",
                "308": "Crazy Old Man's House: Merchant",
                "315": "Returner's Hideout: Inn Soldier",
                "342": "Blackjack: Setzer (Interior)",
                "343": "Blackjack: Coin",
                "344": "Blackjack: Setzer (Celes' Room)",
                "345": "Blackjack: Setzer (Exterior)",
                "346": "Blackjack: Refreshments Guy",
                "347": "Blackjack: Shop Guy",
                "348": "Blackjack: Book",
                "349": "Imperial Base: Locked Treasure Room",
                "350": "Imperial Base: Soldier A",
                "351": "Imperial Base: Soldier B",
                "352": "Imperial Base: Soldier C",
                "353": "Imperial Base: Soldier D",
                "354": "Imperial Base: Soldier E",
                "355": "Imperial Base: Soldier F",
                "356": "Imperial Base: Soldier G",
                "357": "Imperial Base: Soldier H",
                "358": "Imperial Base: Soldier I",
                "360": "Cave to the Sealed Gate: Gate",
                "366": "Cave to the Sealed Gate: Kefka",
                "367": "Cave to the Sealed Gate: Soldier A",
                "368": "Cave to the Sealed Gate: Soldier B",
                "369": "Cave to the Sealed Gate: Return to Airship",
                "370": "Blackjack: Cid",
                "371": "Blackjack: Man in Casino",
                "372": "Thamasa: Baram",
                "373": "Thamasa: Clyde",
                "375": "Blackjack: Terra",
                "376": "Blackjack: Locke",
                "377": "Blackjack: Cyan",
                "378": "Blackjack: Shadow",
                "379": "Blackjack: Edgar",
                "380": "Blackjack: Sabin",
                "381": "Blackjack: Celes",
                "382": "Blackjack: Strago",
                "383": "Blackjack: Relm",
                "384": "Blackjack: Setzer",
                "385": "Blackjack: Mog",
                "386": "Blackjack: Gau",
                "387": "Blackjack: Gogo",
                "388": "Blackjack: Umaro",
                "389": "Cave to the Sealed Gate: Ninja",
                "391": "Jidoor: Relm (Owzer's Basement)",
                "392": "Jidoor: Fat Owzer",
                "396": "Blackjack: Refreshments Sparkle",
                "399": "Jidoor: Chadarnook A",
                "400": "Jidoor: Chadarnook B",
                "401": "Jidoor: Chadarnook C",
                "402": "Jidoor: Emperor Painting",
                "498": "Jidoor: Ultros Painting",
                "495": "Blackjack: Unequip Guy",
                "507": "Cave to the Sealed Gate: Coin ???",
                "508": "Lete River: Raft (Cave 1)",
                "509": "Lete River: Raft (Cave 2)",
                "510": "Cave to the Sealed Gate: Rocks",
                "512": "Aways Visible",
                "513": "Local Control",
                "518": "Albrook Ship: Terra/Locke",
                "519": "Thamasa: Strago/Interceptor (Inn)",
                "520": "Albrook Ship: Shadow",
                "522": "Thamasa: Flames Behind Door",
                "529": "Doma Castle: Dead People",
                "531": "Albrook Docks: Shadow/Interceptor (Lower)",
                "536": "Thamasa: WoB NPC's (Exterior)",
                "538": "Thamasa: Shadow/Interceptor",
                "539": "Esper Mountain: Espers",
                "541": "Thamasa: Kefka/Soldiers",
                "542": "Thamasa: Party Characters (Kefka Attack)",
                "543": "Thamasa: Espers",
                "545": "Esper Mountain: Relm/Ultros (Statue Cave)",
                "546": "Albrook Docks: Shadow/Celes (Upper)",
                "580": "Cyan's Dream: NPC's (Cave)",
                "581": "Cyan's Dream: NPC's (Cliffs)",
                "587": "Gogo's Cave: Gogo",
                "588": "Cave in the Veldt: Interceptor (Entrance)",
                "594": "Cave in the Veldt: Shadow",
                "595": "Cave in the Veldt: Relm",
                "596": "Cave in the Veldt: Interceptor (Boss)",
                "597": "Cave in the Veldt: Monster",
                "598": "Thamasa: Shadow (In Bed)",
                "600": "Colosseum: NPC's",
                "601": "Thamasa: Elder (Elder's House)",
                "604": "Thamasa: Gungho (In Bed)",
                "605": "Ebot's Rock: Treasure Chest (event only)",
                "606": "Ebot's Rock: Hidon",
                "607": "Ebot's Rock: Strago",
                "610": "Thamasa: Shadow (Burning Building)",
                "614": "Thamasa: Relm (In Bed)",
                "759": "Local Control",
                "768": "Narshe: NPC's",
                "770": "Narshe: Arvis/Banon (Arvis' House)",
                "771": "Narshe: Arvis (Intro)",
                "772": "Narshe: NPC's (Intro)",
                "773": "Narshe: Tritoch (Intro)",
                "774": "Narshe: Locked Back Door (Arvis' House, Interior)",
                "775": "Narshe: Slave Crown",
                "776": "Narshe: Locked Back Door (Arvis' House, Exterior)",
                "777": "Narshe: 3-Party Moogles/Guards That Corner Terra",
                "778": "Narshe: 3-Party Monster A",
                "779": "Narshe: 3-Party Monster B",
                "780": "Narshe: 3-Party Monster C",
                "781": "Narshe: 3-Party Monster D",
                "782": "Narshe: 3-Party Monster E",
                "783": "Narshe: 3-Party Monster F",
                "784": "Narshe: Blocked Exits in 3-Party Cave",
                "785": "Figaro Castle: Kefka/Soldiers (Empty Desert)",
                "787": "Narshe: Tritoch (Cliffs, WoB)",
                "788": "Narshe: NPC's (Cliffs, WoB)",
                "789": "Narshe: Locke (Arvis' House)",
                "790": "Narshe: Celes (Arvis' House)",
                "791": "Narshe: Edgar (Arvis' House)",
                "792": "Narshe: Sabin (Arvis' House)",
                "793": "Narshe: Gau (Arvis' House)",
                "794": "Narshe: Cyan (Arvis' House)",
                "810": "Narshe: Guards at Front Entrance",
                "811": "Vector: NPC's",
                "812": "Vector: Gestahl Speech NPC's",
                "815": "Vector: Soldiers (Banquet)",
                "817": "Narshe: Marshal/Terra",
                "818": "Save Point",
                "819": "Narshe: Save Point (Hills Maze)",
                "820": "Vector: Soldiers (Banquet)",
                "822": "Vector: Sabin",
                "827": "Vector: Returner Sympathizer",
                "829": "Narshe: Treasure House NPC's",
                "830": "Narshe: Lone Wolf (Treasure House)",
                "831": "Narshe: Lone Wolf (WoB)",
                "832": "Narshe: Lone Wolf/Mog (Cliffs)",
                "833": "Narshe: Blocked Bridge (Cliffs)",
                "834": "Narshe: Moogles (Moogle Cave, WoB)",
                "835": "Narshe: Mog (Moogle Cave, WoB)",
                "836": "Magitek Research Facility: Map NPC's",
                "837": "Magitek Research Facility: Event NPC's",
                "838": "Magitek Factory: Ifrit/Shiva",
                "839": "Magitek Factory: Ifrit Magicite",
                "840": "Magitek Factory: Shiva Magicite",
                "841": "Magitek Research Facility: Number 024",
                "842": "Magitek Research Facility: Espers (Esper Room)",
                "843": "Magitek Research Facility: Blocked Door (Esper Room)",
                "844": "Vector: Guardian",
                "845": "Vector: Blocked Magitek Factory",
                "846": "Narshe: Arvis/Banon/Elder (Elder's House)",
                "847": "Kohlingen: NPC's (WoB/Interior)",
                "848": "Kohlingen: Rachel Flashback NPC's",
                "850": "Vector: NPC's (On Fire)",
                "856": "Narshe: Checkpoint Sparkles",
                "857": "Umaro's Cave: Skull Statue",
                "858": "Umaro's Cave: Terrato Magicite",
                "859": "Umaro's Cave: Umaro",
                "862": "Albrook: NPC's (WoB/Interior)",
                "863": "Albrook: Soldiers (Exterior)",
                "864": "Albrook: Celes",
                "865": "Albrook: Soldiers (Cafe/Docks)",
                "866": "Tzen: NPC's (WoB/Interior)",
                "867": "Tzen: Soldiers",
                "869": "Albrook: NPC's (WoR)",
                "870": "Tzen: Magicite Guy",
                "872": "Tzen: NPC's (WoR)",
                "874": "Tzen: NPC's (Collapsing House)",
                "876": "Tzen: Sabin",
                "877": "Tzen: Boy in Collapsing House",
                "888": "Fanatics' Tower: NPC's (Bottom)",
                "889": "Fanatics' Tower: Strago",
                "890": "Maranda: Lola w/o Flowers",
                "891": "Maranda: Lola/Flowers",
                "894": "Kohlingen: NPC's (WoR)",
                "896": "Kohlingen: Birds",
                "897": "Mt. Zozo: Flowers/Letter",
                "898": "Mt. Zozo: Cyan",
                "899": "Mt. Zozo: Flying Bird",
                "900": "Mt. Zozo: Cyan's Treasure Chest",
                "901": "Mt. Zozo: Key to Cyan's Treasure Chest",
                "902": "Mt. Zozo: Storm Dragon",
                "903": "Phoenix Cave: Magicite",
                "904": "Narshe: Locked Front Door (Arvis' House)",
                "906": "Narshe: Tritoch/Ragnarok Magicite",
                "907": "Colosseum: Imperial Soldier",
                "908": "Narshe: Tritoch (Cliffs, WoR)",
                "909": "Narshe: Mog (Moogle Cave, WoR)",
                "910": "Kohlingen: Crazy Man 1",
                "911": "Kohlingen: Crazy Man 2",
                "912": "Narshe: Beginner's House NPC's",
                "913": "Narshe: Beginner's House Ghost",
                "914": "Chocobo Stable: NPC's",
                "915": "Phoenix Cave: Crane Hook/Exit Warp",
                "916": "Fanatics' Tower: White Dragon",
                "917": "Narshe: Ice Dragon",
                "918": "Ancient Castle: Odin",
                "919": "Ancient Castle: Queen Statue",
                "920": "Phoenix Cave: Locke",
                "921": "Fanatics' Tower: NPC's (Top)",
                "923": "Ancient Castle: NPC's",
                "924": "Phoenix Cave: Red Dragon",
                "928": "Ancient Castle: Queen's Diary",
                "929": "Ancient Castle: Blue Dragon",
                "930": "Narshe: Man Outside Beginner's House",
                "931": "Vector: Train Car and Soldier",
                "932": "Kefka's Tower: Floor Switches",
                "933": "Kefka's Tower: Left Weight (Top)",
                "935": "Kefka's Tower: Right Weight (Top)",
                "936": "Maranda: Letter (Lola's House)",
                "938": "Kefka's Tower: Left Weight (Bottom)",
                "939": "Kefka's Tower: Right Weight (Bottom)",
                "940": "Banon's House: Banon (Exterior)",
                "941": "Narshe: Ragnarok/Cursed Shield Guys",
                "942": "Vector: Blocked Entrance to Magitek Factory",
                "943": "Kefka's Tower: Treasure Chest Switch",
                "944": "Kefka's Tower: Doom",
                "945": "Kefka's Tower: Goddess",
                "946": "Kefka's Tower: Poltergeist",
                "947": "Kefka's Tower: Gold Dragon",
                "948": "Kefka's Tower: Skull Dragon",
                "949": "Banon's House: Banon A",
                "950": "Kohlingen: Old Man (Unequip)",
                "952": "Kefka's Tower: Doom Save Point",
                "953": "Kefka's Tower: Goddess Save Point",
                "954": "Kefka's Tower: Guardian Event",
                "955": "Kefka's Tower: Guardian Sprites",
                "956": "Kefka's Tower: Inferno",
                "957": "Kefka's Tower: Atma",
                "958": "Kefka's Tower: Atma Save Point",
                "959": "Banon's House: Banon B",
                "960": "Narshe: Locked Inn",
                "961": "Narshe: Locked Weapon Shop",
                "962": "Narshe: Locked Relic Shop",
                "963": "Narshe: Locked Treasure Room",
                "964": "Narshe: Locked Building",
                "965": "Narshe: Locked Weapon Shop"
            }
        },
        "song": {
            "name": "Songs",
            "default": "Song %i",
            "string": {
                "0": "Silence",
                "1": "The Prelude",
                "2": "Opening Theme 1",
                "3": "Opening Theme 2",
                "4": "Opening Theme 3",
                "5": "Awakening",
                "6": "Terra",
                "7": "Shadow",
                "8": "Strago",
                "9": "Gau",
                "10": "Edgar & Sabin",
                "11": "Coin Song",
                "12": "Cyan",
                "13": "Locke",
                "14": "Forever Rachel",
                "15": "Relm",
                "16": "Setzer",
                "17": "Epitaph",
                "18": "Celes",
                "19": "Techno de Chocobo",
                "20": "The Decisive Battle",
                "21": "Johnny C. Bad",
                "22": "Kefka",
                "23": "The Mines of Narshe",
                "24": "Phantom Forest",
                "25": "Veldt",
                "26": "Save Them!",
                "27": "The Emperor Gestahl",
                "28": "Troops March On",
                "29": "Under Martial Law",
                "30": "Waterfall",
                "31": "Metamorphosis",
                "32": "Phantom Train",
                "33": "Another World of Beasts",
                "34": "Grand Finale 2",
                "35": "Mt. Kolts",
                "36": "Battle Theme",
                "37": "Fanfare",
                "38": "The Wedding Waltz 1",
                "39": "Aria de Mezzo Caraterre",
                "40": "The Serpent Trench",
                "41": "Slam Shuffle",
                "42": "Kids Run Through the City Corner",
                "43": "???",
                "44": "Grand Finale 1",
                "45": "Gogo",
                "46": "Returners",
                "47": "Fanfare",
                "48": "Umaro",
                "49": "Mog",
                "50": "The Unforgiven",
                "51": "The Fierce Battle",
                "52": "The Day After",
                "53": "Blackjack",
                "54": "Catastrophe",
                "55": "The Magic House",
                "56": "Nighty Night",
                "57": "Wind",
                "58": "Windy Shores",
                "59": "Dancing Mad 1, 2 & 3",
                "60": "Train Braking",
                "61": "Spinach Rag",
                "62": "Rest in Piece",
                "63": "Chocobos Running",
                "64": "The Dream of a Train",
                "65": "Overture 1",
                "66": "Overture 2",
                "67": "Overture 3",
                "68": "The Wedding Waltz 1",
                "69": "The Wedding Waltz 2",
                "70": "The Wedding Waltz 3",
                "71": "Devil's Lab",
                "72": "Fire!/Explosion",
                "73": "Cranes Rising",
                "74": "Inside the Burning House",
                "75": "New Continent",
                "76": "Searching for Friends",
                "77": "Fanatics",
                "78": "Last Dungeon and Aura",
                "79": "Dark World",
                "80": "Dancing Mad 5",
                "81": "Silence",
                "82": "Dancing Mad 4",
                "83": "Ending Theme 1",
                "84": "Ending Theme 2",
                "0xFF": "Continue Previous Song"
            }
        },
        "statusNamesReversed": {
            "name": "Status Names (Reverse Order)",
            "string": {
                "0-8": "<stringTable.statusNames[7-%i]>",
                "8-16": "<stringTable.statusNames[23-%i]>",
                "16-24": "<stringTable.statusNames[39-%i]>",
                "24-32": "<stringTable.statusNames[55-%i]>"
            }
        },
        "targetingFlags": {
            "name": "Targeting Flags",
            "string": {
                "0": "Can Move Cursor",
                "1": "One Side Only",
                "2": "Auto-Select All",
                "3": "Auto-Select One Side",
                "4": "Auto-Confirm",
                "5": "Manual Party Select",
                "6": "Enemy Default",
                "7": "Random Target"
            }
        },
        "treasureSwitches": {
            "name": "Treasure Switches",
            "length": 512,
            "default": "Treasure %i",
            "string": {
                "1": "Narshe: Outside Arvis' Back Door",
                "2": "Narshe: Arvis' Clock",
                "3": "Narshe: Treasure Room A",
                "4": "Narshe: Treasure Room B",
                "5": "Narshe: Treasure Room C",
                "6": "Narshe: Treasure Room D",
                "7": "Narshe: Treasure Room E",
                "8": "Narshe: Treasure Room F",
                "9": "Narshe: Treasure Room G",
                "10": "Narshe: Elder's Clock",
                "11": "Narshe: South Caves A",
                "12": "Narshe: South Caves B",
                "13": "Narshe: Moogle Cave",
                "14": "Figaro Castle: Right Shop A",
                "15": "Figaro Castle: Right Shop B",
                "16": "Figaro Castle: Left Shop",
                "17": "Figaro Castle: Stairs to Main Tower",
                "18": "Cave to South Figaro A",
                "19": "Cave to South Figaro B",
                "20": "South Figaro: Back Entrance to Big House",
                "21": "South Figaro: Barrel Between Shops",
                "22": "South Figaro: Barrel Near Docks",
                "23": "South Figaro: Crate Near Docks",
                "24": "South Figaro: Barrel Near Entrance",
                "25": "South Figaro: Crate Near Chocobo Stable",
                "26": "South Figaro: Clock Room A",
                "27": "South Figaro: Clock Room B",
                "28": "South Figaro: Clock Room C",
                "29": "South Figaro: Clock Room D",
                "30": "South Figaro: Bucket in Old Man's House",
                "31": "South Figaro: Clock in Secret Passage",
                "32": "South Figaro: Escape Tunnel A",
                "33": "South Figaro: Escape Tunnel B",
                "34": "South Figaro: Escape Tunnel C",
                "35": "South Figaro: Escape Tunnel D",
                "36": "Sabin's House: Bucket",
                "37": "Mt. Kolts: Exterior A",
                "38": "Mt. Kolts: Exterior B",
                "39": "Mt. Kolts: Interior A",
                "40": "Mt. Kolts: Interior B",
                "41": "Returner's Hideout: Pot Near Table",
                "42": "Returner's Hideout: Room With Bed",
                "43": "Returner's Hideout: Treaure Room A",
                "44": "Returner's Hideout: Treaure Room B",
                "45": "Returner's Hideout: Treaure Room C",
                "46": "Returner's Hideout: Treaure Room D",
                "47": "Returner's Hideout: Treaure Room E",
                "48": "Returner's Hideout: Banon's Room",
                "49": "Returner's Hideout: Hidden Cave",
                "50": "Imperial Camp: Top Right Tent",
                "51": "Doma Castle: Clock Near Beds",
                "52": "Doma Castle: Pot Near Fireplace",
                "53": "Doma Castle: Treasure Tower A",
                "54": "Doma Castle: Treasure Tower B",
                "55": "Phantom Train: Dining Car",
                "56": "Phantom Train: Small Room A",
                "57": "Phantom Train: Small Room B",
                "58": "Phantom Train: Small Room C",
                "59": "Phantom Train: Small Room D",
                "60": "Phantom Train: Small Room E",
                "61": "Mobliz: Mail House Clock",
                "62": "Nikeah: Clock in Inn",
                "63": "Serpent Trench A",
                "64": "Serpent Trench B",
                "65": "Kohlingen: Chest in Crazy Man's House",
                "66": "Jidoor: Pot in Owzer's House",
                "67": "Phantom Train: Caboose Room",
                "68": "Cave to the Sealed Gate: Chest Near Save Point",
                "69": "Kohlingen: Clock in Rachel's House",
                "70": "Zozo: Ramuh's Room A",
                "71": "Zozo: Ramuh's Room B",
                "72": "Zozo: Shop",
                "73": "Zozo: Chain Saw",
                "74": "Zozo: Tincture",
                "75": "Zozo: Thief Glove",
                "76": "Zozo: Fire Knuckle",
                "77": "Vector: Castle A",
                "78": "Vector: Castle B",
                "79": "Vector: Castle C",
                "80": "Vector: Castle D",
                "81": "Vector: Castle E",
                "82": "Magitek Factory: Upper Room A",
                "83": "Magitek Factory: Upper Room B",
                "84": "Magitek Factory: Upper Room C",
                "85": "Magitek Factory: Upper Room D",
                "86": "Magitek Factory: Upper Room E",
                "87": "Magitek Factory: Upper Room F",
                "88": "Magitek Factory: Lower Room A",
                "89": "Magitek Factory: Lower Room B",
                "90": "Magitek Factory: Lower Room C",
                "91": "Magitek Factory: Lower Room D",
                "92": "Magitek Factory: Lower Room E",
                "93": "Magitek Factory: Lower Room F",
                "94": "Magitek Research Facility: Tube Room",
                "95": "Floating Island A",
                "96": "Doma Castle: Cyan's Room",
                "97": "Cave to South Figaro C",
                "98": "Albrook: Barrel",
                "99": "Albrook: Crate at Docks",
                "100": "Albrook: Pot in Weapon Shop",
                "101": "Albrook: Armor Shop A",
                "102": "Albrook: Armor Shop B",
                "103": "Albrook: Clock in Inn",
                "104": "Thamasa: Burning Building A",
                "105": "Thamasa: Burning Building B",
                "106": "Esper Mountain: Exterior A",
                "107": "Esper Mountain: Exterior B",
                "108": "Esper Mountain: Exterior C",
                "109": "Esper Mountain: Interior",
                "110": "Imperial Base A",
                "111": "Imperial Base B",
                "112": "Imperial Base C",
                "113": "Imperial Base D",
                "114": "Imperial Base E",
                "115": "Imperial Base F",
                "116": "Imperial Base G",
                "117": "Imperial Base H",
                "118": "Imperial Base I",
                "119": "Imperial Base J",
                "120": "Imperial Base K",
                "121": "Imperial Base L",
                "122": "Cave to the Sealed Gate A",
                "123": "Cave to the Sealed Gate B",
                "124": "Cave to the Sealed Gate C",
                "125": "Cave to the Sealed Gate D",
                "126": "Cave to the Sealed Gate E",
                "127": "Cave to the Sealed Gate F",
                "128": "Cave to the Sealed Gate G",
                "129": "Cave to the Sealed Gate H",
                "130": "Cave to the Sealed Gate I",
                "131": "Cave to the Sealed Gate J",
                "132": "Cave to the Sealed Gate K",
                "133": "Cave to the Sealed Gate: Lava Puzzle A",
                "134": "Cave to the Sealed Gate: Lava Puzzle B",
                "135": "Floating Island B",
                "136": "Floating Island C",
                "137": "Floating Island: Collapsing",
                "138": "Narshe: Right Beginner's Room",
                "139": "Narshe: Middle Beginner's Room",
                "140": "Narshe: Left Beginner's Room",
                "141": "Narshe: Pot in Right Beginner's Room",
                "142": "Tzen: Collapsing House A",
                "143": "Tzen: Collapsing House B",
                "144": "Tzen: Collapsing House C",
                "145": "Tzen: Collapsing House D",
                "146": "Tzen: Collapsing House E",
                "147": "Tzen: Collapsing House F",
                "148": "Tzen: Collapsing House G",
                "149": "Tzen: Collapsing House H",
                "150": "Figaro Castle: Lower Basement A",
                "151": "Figaro Castle: Lower Basement B",
                "152": "Figaro Castle: Lower Basement C",
                "153": "Figaro Castle: Lower Basement D",
                "154": "Figaro Castle: Basement Treasure Room",
                "155": "Figaro Castle: Soul Sabre",
                "156": "Darill's Tomb A",
                "157": "Darill's Tomb B",
                "158": "Darill's Tomb C",
                "159": "Darill's Tomb D",
                "160": "Darill's Tomb E",
                "161": "Darill's Tomb F",
                "162": "Mt. Zozo: Interior A",
                "163": "Mt. Zozo: Interior B",
                "164": "Mt. Zozo: Interior C",
                "165": "Mt. Zozo: Interior D",
                "166": "Mt. Zozo: Treasure Cliff",
                "167": "Mobliz: Pot in Cave 2",
                "168": "Umaro's Cave A",
                "169": "Umaro's Cave B",
                "170": "Umaro's Cave C",
                "171": "Phoenix Cave A",
                "172": "Phoenix Cave B",
                "173": "Phoenix Cave C",
                "174": "Phoenix Cave D",
                "175": "Phoenix Cave E",
                "176": "Phoenix Cave F",
                "177": "Phoenix Cave G",
                "179": "Doma Castle: Chest Near Beds",
                "180": "Fanatics' Tower: 1st Level",
                "181": "Fanatics' Tower: 2nd Level",
                "182": "Fanatics' Tower: 3rd Level",
                "183": "Fanatics' Tower: 4th Level",
                "184": "Fanatics' Tower: Top Floor",
                "185": "Fanatics' Tower: Secret Room",
                "186": "Phantom Train: Cyan's Dream A",
                "187": "Phantom Train: Cyan's Dream B",
                "188": "Phantom Train: Cyan's Dream C",
                "189": "Phantom Train: Cyan's Dream D",
                "190": "Phoenix Cave H",
                "191": "Cave in the Veldt A",
                "192": "Cave in the Veldt B",
                "193": "Cave in the Veldt C",
                "194": "Gogo's Cave: Lowest Cave A",
                "195": "Phoenix Cave I",
                "196": "Ancient Castle: Cave A",
                "197": "Ancient Castle: Cave B",
                "198": "Ancient Castle: Cave C",
                "199": "Ancient Castle: Cave D",
                "200": "Ancient Castle: Cave E",
                "201": "Ancient Castle: Cave F",
                "202": "Ancient Castle: Interior A",
                "203": "Ancient Castle: Interior B",
                "204": "Ancient Castle: Interior C",
                "205": "Ancient Castle: Interior D",
                "206": "Ancient Castle: Interior E",
                "207": "Narshe: Moogle Charm",
                "208": "Gogo's Cave: Platform Cave A",
                "209": "Gogo's Cave: Lowest Cave B",
                "210": "Imperial Camp: Top Left Tent",
                "211": "Imperial Camp: Bottom Left Tent A",
                "212": "Imperial Camp: Bottom Left Tent B",
                "213": "Jidoor: Owzer's Basement A",
                "214": "Kefka's Tower: Exterior A",
                "215": "Kefka's Tower: Fixed Dice",
                "216": "Kefka's Tower: Small Switch Room",
                "217": "Kefka's Tower: Exterior B",
                "218": "Kefka's Tower: Exterior C",
                "219": "Kefka's Tower: Exterior D",
                "220": "Kefka's Tower: Exterior E",
                "221": "Kefka's Tower: Exterior F",
                "222": "Kefka's Tower: Exterior G",
                "223": "Kefka's Tower: Minerva",
                "224": "Kefka's Tower: Tack Star",
                "225": "Kefka's Tower: Inferno's Room A",
                "226": "Kefka's Tower: Factory Room A",
                "227": "Thamasa: Strago & Relm's House",
                "228": "Gogo's Cave: Right Before Gogo's Room",
                "229": "Pot in Colosseum Guy's House",
                "230": "South Figaro: Lower Left Barrel",
                "231": "South Figaro: Barrel on Wall",
                "232": "Gogo's Cave: Smashing Room A",
                "233": "Gogo's Cave: Smashing Room B",
                "234": "Gogo's Cave: Smashing Room C",
                "235": "Kefka's Tower: Inferno's Room B",
                "236": "Gogo's Cave: Platform Cave B",
                "237": "Gogo's Cave: Platform Cave C",
                "238": "Mt. Zozo: Cyan's Cave",
                "239": "Zozo: Pot A",
                "240": "Zozo: Pot B",
                "241": "Kefka's Tower: Factory Room B",
                "242": "Kefka's Tower: Exterior H",
                "243": "Phantom Train: Small Room E",
                "244": "Maranda: Bottom Right Crate",
                "245": "Maranda: Bottom Left Crate",
                "246": "Thamasa: Barrel A",
                "247": "Thamasa: Barrel B",
                "248": "Thamasa: Barrel C",
                "249": "Thamasa: Barrel D",
                "250": "Thamasa: Barrel E",
                "251": "Mobliz: Barrel (WoR)",
                "252": "South Figaro: Basement Treasure Room A",
                "253": "South Figaro: Basement Treasure Room B",
                "254": "South Figaro: Basement Treasure Room C",
                "255": "South Figaro: Basement Treasure Room D",
                "256": "South Figaro: Basement Treasure Room E",
                "257": "Imperial Base Hidden Chest",
                "258": "Jidoor: Owzer's Basement B"
            }
        },
        "vehicles": {
            "name": "Vehicles",
            "string": {
                "0": "None",
                "1": "Chocobo",
                "2": "Magitek Armor",
                "3": "Raft"
            }
        },
        "vehiclesWorld": {
            "name": "World Map Vehicles",
            "string": {
                "0": "None",
                "1": "Airship",
                "2": "Chocobo"
            }
        },
        "weaponSpecialEffect": {
            "name": "Weapon Special Effect",
            "hideIndex": true,
            "string": {
                "0": "None",
                "1": "<stringTable.itemNames[4]>",
                "2": "<stringTable.itemNames[28]>",
                "3": "<stringTable.itemNames[5]>, <stringTable.itemNames[41]>, <stringTable.itemNames[76]>, <stringTable.itemNames[80]>",
                "4": "<stringTable.itemNames[6]>",
                "5": "<stringTable.itemNames[18]>",
                "6": "<stringTable.itemNames[22]>",
                "7": "MP Critical",
                "8": "<stringTable.itemNames[73]>, <stringTable.itemNames[75]>",
                "9": "<stringTable.itemNames[81]>, <stringTable.itemNames[82]>",
                "10": "<stringTable.itemNames[9]>",
                "11": "<stringTable.itemNames[46]>",
                "12": "<stringTable.itemNames[51]>",
                "13": "<stringTable.itemNames[25]>",
                "14": "<stringTable.itemNames[23]>",
                "15": "MP Critical (More)"
            }
        }
    },
    "freeSpace": [
        "0xC0D613-0xC0DFA0",
        "0xC0FF18-0xC0FFB0",
        "0xC1FFE5-0xC20000",
        "0xC26469-0xC26800",
        "0xC28A60-0xC28A70",
        "0xC2A65A-0xC2A800",
        "0xC2FAA4-0xC2FC6D",
        "0xC3F091-0xC40000",
        "0xC419FE-0xC41A10",
        "0xC46A6B-0xC46AC0",
        "0xC4A4C0-0xC4BA00",
        "0xC53C4E-0xC53C5E",
        "0xC9FCEC-0xC9FE00",
        "0xCCE5EE-0xCCE600",
        "0xCEEFBB-0xCEF100",
        "0xCEF463-0xCEF600",
        "0xCEFEDF-0xCF0000",
        "0xCF3BAD-0xCF3C40",
        "0xCFC049-0xCFC050",
        "0xCFF442-0xCFF450",
        "0xCFFB29-0xCFFC00",
        "0xCFFCE0-0xCFFD00",
        "0xCFFFBE-0xD00000",
        "0xD0CF4A-0xD0D000",
        "0xD0FC7A-0xD0FD00",
        "0xD1E989-0xD1EAD7",
        "0xD1F751-0xD1F7A0",
        "0xD1F9A0-0xD1F9AB",
        "0xD1F9D0-0xD1FA00",
        "0xD2A130-0xD2A820",
        "0xD2B224-0xD2B300",
        "0xD4C998-0xD4CA00",
        "0xD4F646-0xD50000",
        "0xD8EE47-0xD8F000",
        "0xD9CC4C-0xD9CD10",
        "0xD9CD66-0xD9CD90",
        "0xD9D1AD-0xD9D1B0",
        "0xDDFFF2-0xDE0000",
        "0xDFD978-0xDFDA00",
        "0xE5FEC6-0xE60000",
        "0xE68492-0xE68780",
        "0xECE200-0xECE3C0",
        "0xED7787-0xED77A0",
        "0xED7A61-0xED7A70",
        "0xED8BCA-0xED8E5B",
        "0xEDFCAA-0xEDFE00",
        "0xEDFFA7-0xEDFFD0",
        "0xEEAF01-0xEEB200",
        "0xEFFBC8-0xEFFEF0"
    ]
}
)^";
