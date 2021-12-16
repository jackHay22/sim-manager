package processing

import (
  "encoding/csv"
  "os"
  "log"
  "fmt"
)

func min(x int, y int) int {
  if x < y {
    return x
  }
  return y
}

/*
 * Take the bandwidth, storage usage for each timestep and
 * write to output file (csv)
 */
func WriteOutput(bandwidthUsage []int, storageUsage []int, filename string) {
  if file, err := os.Create(filename); err == nil {
    defer file.Close()
    writer := csv.NewWriter(file)
    defer writer.Flush()

    header := []string{"Bandwidth", "Storage"}

    //write header
    if csvErr := writer.Write(header); csvErr != nil {
      log.Fatalf("failed to write to csv file %s: %s", filename, csvErr)
    }

    //write rows
    for i := 0; i < min(len(bandwidthUsage), len(storageUsage)); i++ {
      row := []string{fmt.Sprintf("%d", bandwidthUsage[i],),
                      fmt.Sprintf("%d", storageUsage[i])}

      if csvErr := writer.Write(row); csvErr != nil {
        log.Fatalf("failed to write to csv file %s: %s", filename, csvErr)
      }
    }

  } else {
    log.Fatalf("failed to write output to %s", filename)
  }
}
