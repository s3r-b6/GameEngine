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
	Name string   `xml:"name,attr"`
	Data []string `xml:"data"`
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
		fmt.Printf("Name: %s\n", layer.Name)
		fmt.Println("Data:", layer.Data)
	}

	return result, true
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
			data, ok := processRoomsFile(fname)
			if !ok {
				log.Println("Failed to process", fname)
				continue
			}

			fname = fname[:strings.Index(fname, ".tmx")]
			output := "./" + fname + ".cpp"

			file, err := os.Create(output)
			if err != nil {
				log.Println("Error creating file:", err)
				continue
			}
			defer file.Close()

			err = file.Truncate(0)
			if err != nil {
				log.Println("Error clearing file contents:", err)
				continue
			}

			for _, l := range data.Layers {
				joined := strings.Join(l.Data, ",")
				byteData := []byte(joined)

				file.Write([]byte("int " + fname + l.Name + "[] = {"))

				n, err := file.Write(byteData)
				if err != nil {
					fmt.Println("Error writing to", output, err)
					return
				}
				file.Write([]byte("};"))

				log.Printf("Written %v bytes to %s", n, output)
			}
		}
	}
}
