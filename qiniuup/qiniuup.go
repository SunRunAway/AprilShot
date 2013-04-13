package main

import (
    "qbox.us/digest_auth"
    "qbox.us/api/v2/rs"
    "os"
    "log"
)

func main() {
    if len(os.Args) != 4 {
        log.Fatal("args is not right.")
    }
    access, secret, fName := os.Args[1], os.Args[2], os.Args[3]
    transport := digest_auth.NewTransport(access, secret, nil)
    rsServer := rs.New(transport)
    f, err := os.Open(fName)
    if err != nil {
        log.Fatal(err)
    }
    defer f.Close()
    fInfo, err := os.Stat(fName)
    if err != nil {
        log.Fatal(err)
    }
    fSize := fInfo.Size()
    
    
    _, _, err = rsServer.Put("aprilshot:"+fName, "image/png", f, fSize, "", "", "")
    if err != nil {
        log.Fatal(err)
    }
}
