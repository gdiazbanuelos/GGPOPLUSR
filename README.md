# GGAC+R GGPO-BASED CASTER

GGPO Repo Link: https://github.com/pond3r/ggpo

It's happening, but no code will be written that hooks up with GGPO until we understand the game state completely.

Thanks to [GGPO](https://www.ggpo.net)'s recent switch to [open-source](https://github.com/pond3r/ggpo), it is now possible to write casters without building an entire netcode from the ground up.

In fighting games, latency hurts more than in any other variety of game. Games that do well in that department (even casters)
are few and far between (MBAACC's caster by madscientist, Killer Instinct, Skullgirls (also GGPO))

Currently there's just a CE table trying to gather game state so that it can be collected and synchronized through GGPO.

Feel free to contact me with any questions pretaining to **this caster** (two casters is far out, man)

If you'd like to communicate with me and help out, also reach out to me through the following:

Twitter: [@MemeMongerBPM](https://twitter.com/MemeMongerBPM)

Discord: @Labryz#5752

Looking for C++ devs that can hook and navigate x86 ASM and/or navigate GGPO source.

Also looking for people like me who can take this game apart to try and understand what's going on under the hood.

You don't have to be either of the above, just reach out with your questions/comments/concerns.

## Building

GGPOPLUSR uses [CMake](https://cmake.org/) as a build engine and
[Detours](https://github.com/microsoft/Detours) as a method of installing
code at runtime, and depends on the Win32 SDK. To build the launcher:
1. Clone and build Detours.
2. Download and generate a CMake file.
3. Use Visual Studio (or your preferred CMake generator) to actually build it.

## Running

1. Create a `steam_appid.txt` file in the Guilty Gear folder, with the contents "348550".
2. Run the launcher.
3. Point the launcher at the Guilty Gear folder.
4. Click Launch.

## Bigger Problems

* Skipping straight to 2P versus
* If we can't skip to PvP, how will this get to vs?
    * Find out how to disable rendering (step 1)
    * Find out how to disable Intro Sound/Main Menu BGM (step 1b)
    * Find out how to hijack input method messages to the exe
        *First purpose: prevent pausing in vs
        *Second purpose: prevent backing out of 2P vs with select
        *Third purpose: tool-assist path from press start screen to 2P vs character select
        *Fourth purpose(?): Send configured inputs from input -> caster -> game, will make all 3 easier
* Synchronizing match start after stage pick
    * Uneven load times might even affect inter-round time as well
* What do we synchronize on mid-match?    
* How do we stop the game without it not responding for slower loaders (into or out of the match) to catch up?
* Where's Player 2's buffer?
* Which of the (currently 3) input buffers gets written to first?
    * If you intercept the first one, will the others follow suit?


## Input Buffer

Currently there are three of them for P1 (AFAIK)

- All three independent of button settings
- All three independent of KB/Controller
  - May cause KB vs Controller issues...
- All three are 2 Bytes Long

### Buttons

| button   | hex    | binary              |
| -------- | ------ | ------------------- |
| left     | 0x8000 | 1000 0000 0000 0000 |
| down     | 0x4000 | 0100 0000 0000 0000 |
| right    | 0x2000 | 0010 0000 0000 0000 |
| up       | 0x1000 | 0001 0000 0000 0000 |
| P        | 0x0080 | 0000 0000 1000 0000 |
| K        | 0x0040 | 0000 0000 0100 0000 |
| H        | 0x0020 | 0000 0000 0010 0000 |
| S        | 0x0010 | 0000 0000 0001 0000 |
| D        | 0x0008 | 0000 0000 0000 1000 |
| Record   | 0x0004 | 0000 0000 0000 0100 |
| Taunt    | 0x0002 | 0000 0000 0000 0010 |
| Playback | 0x0001 | 0000 0000 0000 0001 |

Start needs to be blocked at all times, select only when a player hasn't picked a character yet (or maybe also blocked all the time, I don't know...)
