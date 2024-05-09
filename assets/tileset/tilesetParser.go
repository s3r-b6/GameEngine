package main

import (
	"encoding/xml"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
	"strings"
)

type TileSet struct {
	Tiles []Tile `xml:"tile"`
}

type Tile struct {
	Id        string    `xml:"id,attr"`
	Animation Animation `xml:"animation"`
}

type Animation struct {
	Frames []Frame `xml:"frame"`
}

type Frame struct {
	Tileid   string `xml:"tileid,attr"`
	Duration string `xml:"duration,attr"`
}

func main() {
	files, err := os.ReadDir("./")
	if err != nil {
		log.Fatal(err)
		return
	}

	for _, f := range files {
		fname := f.Name()
		if strings.HasSuffix(fname, ".tsx") {
			processFile(fname)
		}
	}
}

func processRoomsFile(f string) (TileSet, bool) {
	var result TileSet

	var file *os.File
	var xmlData []byte
	var err error

	file, err = os.Open(f)
	if err != nil {
		log.Fatal("Failed to read", f)
		return TileSet{}, false
	}

	xmlData, err = io.ReadAll(file)
	if err != nil {
		log.Fatal("Failed to read XML. ERR:", err)
		return TileSet{}, false
	}

	err = xml.Unmarshal([]byte(xmlData), &result)
	if err != nil {
		log.Fatal("Failed to unmarshal:", err)
		return TileSet{}, false
	}

	for _, tile := range result.Tiles {
		log.Printf("Name: %s\n", tile.Id)
		log.Printf("Frame duration: %s\n", tile.Animation.Frames[0].Duration)
		for _, frame := range tile.Animation.Frames {
			log.Println(">> FrameTile :", frame.Tileid)
		}
	}

	return result, true
}

func processFile(fname string) {
	data, ok := processRoomsFile(fname)
	if !ok {
		log.Println("Failed to process", fname)
		return
	}

	fname = fname[:strings.Index(fname, ".tsx")]
	output := "../" + fname + ".cpp"

	file, err := os.Create(output)
	if err != nil {
		log.Println("Error creating file:", err)
		return
	}
	defer file.Close()

	err = file.Truncate(0)
	if err != nil {
		log.Println("Error clearing file contents:", err)
		return
	}

	file.Write([]byte("#include \"tiles.h\"\n\n"))

	globalStr := ""

	functionStr := `void loadAnimatedTiles(BumpAllocator *alloc) {
		AnimatedTile t;
		%s	
	}`

	// Count, id, for i in frames...
	mapStr := `t = {
		.tileCount = %d,
		.duration = %d,
		.nextTiles = &tile_%d[0],
	};
	tileManager->animatedTiles[%d] = t;`

	tilesData := ""
	for _, tile := range data.Tiles {
		id, _ := strconv.Atoi(tile.Id)

		arrayStr := ""
		arrayStr += fmt.Sprintf("(u16)%d", id+1)
		var duration string
		for _, frame := range tile.Animation.Frames {
			tileid, _ := strconv.Atoi(frame.Tileid)
			arrayStr += fmt.Sprintf(", (u16)%d", tileid+1)
			duration = frame.Duration
		}

		frCount := len(tile.Animation.Frames) + 1
		globalStr += fmt.Sprintf("TileID tile_%d[] = {%s};\n", id+1, arrayStr)
		animDuration, _ := strconv.Atoi(duration)
		animDuration /= 60
		tilesData += fmt.Sprintf(mapStr, frCount, animDuration, id+1, id+1)
		tilesData += "\n"
	}
	file.Write([]byte(globalStr))
	file.Write([]byte(fmt.Sprintf(functionStr, tilesData)))
}
