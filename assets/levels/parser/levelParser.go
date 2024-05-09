package main

import (
	"encoding/xml"
	"fmt"
	"io"
	"log"
	"os"
	"strings"
)

type Map struct {
	Layers []Layer `xml:"layer"`
}

type Layer struct {
	Name   string `xml:"name,attr"`
	Width  int    `xml:"width,attr"`
	Height int    `xml:"height,attr"`
	Data   string `xml:"data"`
}

func main() {
	files, err := os.ReadDir("./")
	if err != nil {
		log.Fatal(err)
		return
	}

	for _, f := range files {
		fname := f.Name()
		if strings.HasSuffix(fname, ".tmx") {
			processFile(fname)
		}
	}
}

func processRoomsFile(f string) (Map, bool) {
	var result Map

	var file *os.File
	var xmlData []byte
	var err error

	file, err = os.Open(f)
	if err != nil {
		log.Fatal("Failed to read", f)
		return Map{}, false
	}

	xmlData, err = io.ReadAll(file)
	if err != nil {
		log.Fatal("Failed to read XML. ERR:", err)
		return Map{}, false
	}

	err = xml.Unmarshal([]byte(xmlData), &result)
	if err != nil {
		log.Fatal("Failed to unmarshal:", err)
		return Map{}, false
	}

	for _, layer := range result.Layers {
		log.Printf("Name: %s\n", layer.Name)
		log.Println("Data:", layer.Data)
	}

	return result, true
}

func processFile(fname string) {
	data, ok := processRoomsFile(fname)
	if !ok {
		log.Println("Failed to process", fname)
		return
	}

	fname = fname[:strings.Index(fname, ".tmx")]
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

	for i, l := range data.Layers {
		byteData := []byte(l.Data)

		if i == 0 {
			file.Write([]byte("u16 " + fname + l.Name + "[] = {"))

			n, err := file.Write(byteData)
			if err != nil {
				log.Println("Error writing to", output, err)
				return
			}
			file.Write([]byte("};\n\n"))
			log.Printf("Written %v bytes to %s", n, output)
			continue
		}

		file.Write([]byte("PosTile " + fname + l.Name + "[] = {\n"))
		j := 0
		for k := 0; k < l.Width*l.Height; k++ {
			x := k % l.Width
			y := k / l.Width

			commaIdx := strings.IndexByte(l.Data[j:], ',')
			if commaIdx == -1 {
				commaIdx = len(l.Data) - j
			}
			id := strings.TrimSpace(l.Data[j : j+commaIdx])
			if id != "0" {
				file.Write([]byte(fmt.Sprintf("{.id = %s, .posX = %d, .posY = %d},\n", id, x*16, y*16)))
				log.Println("Non-empty tile", id, x, y)
			}
			j += commaIdx + 1
		}
		file.Write([]byte("{.id = MAX_U16, .posX = (u16)0, .posY = (u16)0}\n"))
		file.Write([]byte("};\n\n"))

	}
}
