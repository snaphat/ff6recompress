# ff6recompress
[Apultra](https://github.com/emmanuel-marty/apultra) recompressor for FF6. 

## Explanation
Recompresses all of the data in FF6 (for SNES) by utilizing the apultra algorithm. This saves approximately ~12KB of space in the rom.
It ouputs a json file compatible with [f6tools](https://github.com/everything8215/ff6tools).

##  Oustanding issues
- Decompression algorithm necessary for the game to operate with the recompressed data is not added to the rom.
- Rom checksum is not updated.
- A [rust version](https://github.com/snaphat/ff6recompress-rs) is being worked on that will fix the checksum and add the decompression algorithm.
