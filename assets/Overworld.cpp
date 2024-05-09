#include "tiles.h"

TileID tile_165[] = {(u16)165, (u16)81, (u16)82, (u16)83, (u16)84, (u16)41, (u16)42, (u16)43, (u16)44};
TileID tile_383[] = {(u16)383, (u16)383, (u16)386, (u16)389};
TileID tile_384[] = {(u16)384, (u16)384, (u16)387, (u16)390};
TileID tile_385[] = {(u16)385, (u16)385, (u16)388, (u16)391};
TileID tile_423[] = {(u16)423, (u16)423, (u16)426, (u16)429};
TileID tile_424[] = {(u16)424, (u16)424, (u16)427, (u16)430};
TileID tile_425[] = {(u16)425, (u16)425, (u16)428, (u16)431};
void loadAnimatedTiles(BumpAllocator *alloc) {
		AnimatedTile t;
		t = {
		.tileCount = 9,
		.duration = 3,
		.nextTiles = &tile_165[0],
	};
	tileManager->animatedTiles[165] = t;
t = {
		.tileCount = 4,
		.duration = 10,
		.nextTiles = &tile_383[0],
	};
	tileManager->animatedTiles[383] = t;
t = {
		.tileCount = 4,
		.duration = 3,
		.nextTiles = &tile_384[0],
	};
	tileManager->animatedTiles[384] = t;
t = {
		.tileCount = 4,
		.duration = 10,
		.nextTiles = &tile_385[0],
	};
	tileManager->animatedTiles[385] = t;
t = {
		.tileCount = 4,
		.duration = 10,
		.nextTiles = &tile_423[0],
	};
	tileManager->animatedTiles[423] = t;
t = {
		.tileCount = 4,
		.duration = 10,
		.nextTiles = &tile_424[0],
	};
	tileManager->animatedTiles[424] = t;
t = {
		.tileCount = 4,
		.duration = 10,
		.nextTiles = &tile_425[0],
	};
	tileManager->animatedTiles[425] = t;
	
	}