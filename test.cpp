#include "rom.cpp"

#ifdef __MINGW32__
#include <windows.h>
#endif

auto main( int argc, char *argv[] ) -> int {
#ifdef __MINGW32__
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(handle, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(handle, mode);
        }
    }
#endif

    Rom rom("Final Fantasy III (USA) (Rev 1).sfc");
    if (!rom.size()) { std::cout << "Error opening rom!" << std::endl; return 0; }

    //std::cout << "Sprite Overlay Data (45 items, compressed, 16 + 256 bytes each):" << std::endl;
    rom.process();
    //rom.recompress(0xC0F4A0, 2, 0xC0F500, 0xC0FD00, 45); // 0xC0FCFF //Sprite Overlay Data (45 items, compressed, 16 + 256 bytes each)
    // rom.recompress(0xC2686C);//, 0xC28A5f); //Title, Intro, Floating Island, & World Cinematic Program (compressed)
    // // recompress(rom, 0xC4BA00);//, 0xC4C007); //Ending Font (compressed)
    // rom.recompress(0xC4C008);//, 0xC4F1C1); //Ending Cinematic Graphics and Tile Formation (compressed)
    // // recompress(rom, 0xC4F477);//, 0xC4F6FA); //Ending Cinematic Graphics (compressed)
    // // recompress(rom, 0xC4F6FB);//, 0xC4FFFF); //Ending Cinematic Graphics (compressed)
    // // recompress(rom, 0xD2E000);//, 0xD2EBFF); //Battle Status Graphics, etc. (compressed)
    // // recompress(rom, 0xD8D000);//, 0xD8DAF1); //Setzer's Airship on Fire Graphics (compressed)
    // // recompress(rom, 0xD8DAF2);//, 0xD8DCFF); //Setzer's Airship on Fire Tile Formation (compressed)
    // //cout << hex << (conv_addr(0xD8DD00)) << endl;
    // //recompress(rom, 0xD8DD00+1);//, 0xD8DFB7); //Pointers to Magitek Train Ride Tile Graphics (29 items, 12×2 bytes each, +$7E0000)
    // //recompress(rom, 0xD8DD00, 2, 0xEF1250, 29*12);

    // rom.recompress(0xD8DFB8);//, 0xD8E5BE); //Vector Panorama Graphics (compressed)
    // rom.recompress(0xD8E5BF);//, 0xD8E6B9); //Vector Panorama Tile Formation (compressed)
    // rom.recompress(0xD8E6BA);//, 0xD8E7b0); //Serpent Trench Palettes (compressed)
    // rom.recompress(0xD8F000);//, 0xD94E71); //Title/Intro Graphics (compressed)
    // rom.recompress(0xD94E96);//, 0xD9568B); //Floating Island Cinematic Graphics (compressed)
    // rom.recompress(0xD9568F);//, 0xD99a50); //Ending Cinematic Graphics (compressed)
    // // recompress(rom, 0xD99D4B);//, 0xD9A4E4); //Ending Cinematic Graphics (compressed)
    // // recompress(rom, 0xD9A4E5);//, 0xD9A7FF); //Ending Cinematic Graphics (compressed)

    // // cout << "Map Tile Properties (42 items, compressed):" << endl;
    // rom.recompress(0xD9CD10, 2, 0xD9A800, 0xD9CC4C, 42);  // 0xD9CD0F //Map Tile Properties (42 items, compressed)
    // // cout << "Map Formations (350 items, compressed):" << endl;
    // rom.recompress(0xD9CD90, 3, 0xD9D1B0, 0xDDFFF2, 350); //Map Formations (350 items, compressed)
    // // cout << "Map Tile Formations (75 items, compressed):" << endl;
    // rom.recompress(0xDFBA00, 3, 0xDE0000, 0xDFB400, 75);  //Map Tile Formations (75 items, compressed)
    // // cout << "BG3 Graphics (19 items, compressed)" << endl;
    // rom.recompress(0xE6CD60, 3, 0xE68780, 0xE6CD60, 18);  //BG3 Graphics (19 items, compressed)
    // // cout << "BG3 Animation Graphics (6 items, compressed):" << endl;
    // rom.recompress(0xE6CDA0, 3, 0xE6CDC0, 0xE6F198, 6);   //BG3 Animation Graphics (6 items, compressed)
    // // cout << "Battle BG Tile Formations (50 items, 2048 bytes each, compressed):" << endl;
    // rom.recompress(0xE71848, 2, 0xE70000, 0xE7A9E7, 49);  //Battle BG Tile Formations (50 items, 2048 bytes each, compressed)
    // // cout << "Battle BG Graphics (75 items, compressed):" << endl;
    // rom.recompress(0xE71650, 3, 0x000000, 0xE96300, 75);  //Battle BG Graphics (75 items, compressed)


    // rom.recompress(0xECE900);//, 0xECFFFE); //World Getting Torn Apart Cinematic Graphics (compressed)
    // rom.recompress(0xEEB290);//, 0xEEC294); //World Map Clouds Graphics (compressed)
    // rom.recompress(0xEEC295);//, 0xEEC701); //World Map Clouds Tile Formation (compressed)
    // // rom.recompress(0xEEC702);//, 0xEED433); //Setzer's Airship Graphics (compressed)
    // rom.recompress(0xEED434);//, 0xEF114E); //World of Balance Map Formation (compressed)
    // rom.recompress(0xEF114F);//, 0xEF324F); //World of Balance Graphics (compressed)
    // // rom.recompress(0xEF3250);//, 0xEF4845); //Magitek Train Ride Graphics (variable size tiles, compressed)
    // rom.recompress(0xEF4A46);//, 0xEF6A55); //World of Ruin Graphics (compressed)
    // rom.recompress(0xEF6A56);//, 0xEF9D16); //World of Ruin Map Formation (compressed)
    // rom.recompress(0xEF9D17);//, 0xEFB630); //Serpent Trench Map Formation (compressed)
    // rom.recompress(0xEFB631);//, 0xEFC623); //Serpent Trench Graphics (compressed)
    // // recompress(rom, 0xEFC624);//, 0xEFCE76); //Some Chocobo Graphics (compressed)
    // // recompress(rom, 0xEFCEB7);//, 0xEFCFB8); //Airship Shadow and Perspective Graphics (compressed)
    // // recompress(rom, 0xEFCFB9);//, 0xEFDC4B); //Various Sprites (ship, esper terra, figaro, etc., compressed)
    // // recompress(rom, 0xEFDC4C);//, 0xEFE49A); //More Chocobo Graphics (world map, compressed)
    // // recompress(rom, 0xEFE49B);//, 0xEFE8B2); //World of Balance Minimap Graphics (compressed)
    // // recompress(rom, 0xEFE8B3);//, 0xEFED25); //World of Ruin Minimap Graphics (compressed)
    // // recompress(rom, 0xEFED26);//, 0xEFFAC7); //Daryl's Airship Graphics (compressed)


    rom.save("test");

    return 0;
}
