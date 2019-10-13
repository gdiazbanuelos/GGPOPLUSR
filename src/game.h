#pragma once

//put respect here
enum gameinputs : unsigned short {
			Up = 0x10,
			Right = 0x20,
			Down = 0x40,
			Left = 0x80,
			Punch = 0x4000,
			Kick = 0x2000,
			Slash = 0x8000,
			HSlash = 0x200,
			Dust = 0x100
};

//Maybe put other enumerable addresses here too?