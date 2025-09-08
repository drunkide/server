package main

/*
#cgo LDFLAGS: -static-libgcc
typedef struct Client Client;
typedef struct BroadcastReason BroadcastReason;
#include "main.h"
*/
import "C"

import (
    "net/http"
    "crypto/rand"
    "crypto/rsa"
    "crypto/tls"
    "crypto/x509"
    "crypto/x509/pkix"
    "math/big"
    "encoding/pem"
    "log"
    "sync"
    "unsafe"
    "os"
    "time"
    "github.com/gorilla/websocket"
)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

type Client struct {
    client *C.Client
    conn *websocket.Conn
    sendText chan []byte
    sendBinary chan []byte
}

var clients = make(map[*C.Client]*Client)
var clientsMutex sync.RWMutex

func (c *Client) kill() {
    clientsMutex.Lock()
    if _, ok := clients[c.client]; ok {
        delete(clients, c.client)
        close(c.sendText)
        close(c.sendBinary)
        c.conn.Close()
        if c.client != nil {
            C.Disconnect(c.client)
        }
    }
    clientsMutex.Unlock()
}

func (c* Client) reader() {
    for {
        mt, msg, err := c.conn.ReadMessage()
        if err != nil {
            c.kill()
            return;
        }

        if mt == websocket.TextMessage {
            buf := C.CBytes(msg)
            C.ReceiveText(c.client, buf, C.size_t(len(msg)))
            C.free(buf)
        } else if mt == websocket.BinaryMessage {
            buf := C.CBytes(msg)
            C.ReceiveBinary(c.client, buf, C.size_t(len(msg)))
            C.free(buf)
        }
    }
}

func (c* Client) textWriter() {
    for msg := range c.sendText {
        if err := c.conn.WriteMessage(websocket.TextMessage, msg); err != nil {
            c.kill()
            return
        }
    }
}

func (c* Client) binaryWriter() {
    for msg := range c.sendBinary {
        if err := c.conn.WriteMessage(websocket.BinaryMessage, msg); err != nil {
            c.kill()
            return
        }
    }
}

func startClient(conn* websocket.Conn) *Client {
    c := &Client{conn: conn, sendText: make(chan[] byte, 64), sendBinary: make(chan[] byte, 64) }

    c.client = C.Connect()
    if c.client == nil {
        conn.Close()
        return nil
    }

    clientsMutex.Lock()
    clients[c.client] = c
    clientsMutex.Unlock()

    go c.textWriter()
    go c.binaryWriter()
    go c.reader()

    return c
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//export GoSendText
func GoSendText(client *C.Client, data unsafe.Pointer, length C.size_t) {
    clientsMutex.RLock()
    c, ok := clients[client]
    clientsMutex.RUnlock()

    if ok {
        msg := C.GoBytes(data, C.int(length))
        select {
            case c.sendText <- msg:
            default:
                c.kill()
        }
    }
}

//export GoSendBinary
func GoSendBinary(client *C.Client, data unsafe.Pointer, length C.size_t) {
    clientsMutex.RLock()
    c, ok := clients[client]
    clientsMutex.RUnlock()

    if ok {
        msg := C.GoBytes(data, C.int(length))
        select {
            case c.sendBinary <- msg:
            default:
                c.kill()
        }
    }
}

//export GoBroadcastText
func GoBroadcastText(reason *C.BroadcastReason, data unsafe.Pointer, length C.size_t) {
    msg := C.GoBytes(data, C.int(length))
    clientsMutex.RLock()
    for _, c := range clients {
        if C.Filter(c.client, reason) {
            select {
                case c.sendText <- msg:
                default:
                    c.kill()
            }
        }
    }
    clientsMutex.RUnlock()
}

//export GoBroadcastBinary
func GoBroadcastBinary(reason *C.BroadcastReason, data unsafe.Pointer, length C.size_t) {
    msg := C.GoBytes(data, C.int(length))
    clientsMutex.RLock()
    for _, c := range clients {
        if C.Filter(c.client, reason) {
            select {
                case c.sendBinary <- msg:
                default:
                    c.kill()
            }
        }
    }
    clientsMutex.RUnlock()
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var upgrader = websocket.Upgrader {
    CheckOrigin: func(r *http.Request) bool {
       return true
    },
}

func ws(w http.ResponseWriter, r *http.Request) {
    conn, err := upgrader.Upgrade(w, r, nil)
    if err != nil {
       return
    }

    startClient(conn)
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

func createCertificate() (tls.Certificate, error) {
    priv, err := rsa.GenerateKey(rand.Reader, 2048)
    if err != nil {
        return tls.Certificate{}, err
    }
    template := x509.Certificate{
        SerialNumber: big.NewInt(1),
        Subject: pkix.Name{Organization: []string{"DrunkIDE"}},
        NotBefore: time.Now(),
        NotAfter: time.Now().Add(10 * 365 * 24 * time.Hour),
        KeyUsage: x509.KeyUsageKeyEncipherment | x509.KeyUsageDigitalSignature,
        IsCA: true,
        BasicConstraintsValid: true,
    }
    der, err := x509.CreateCertificate(rand.Reader, &template, &template, &priv.PublicKey, priv)
    if err != nil {
        return tls.Certificate{}, err
    }
    cert := pem.EncodeToMemory(&pem.Block{Type: "CERTIFICATE", Bytes: der})
    key := pem.EncodeToMemory(&pem.Block{Type: "RSA PRIVATE KEY", Bytes: x509.MarshalPKCS1PrivateKey(priv)})
    return tls.X509KeyPair(cert, key)
}

func fileExists(name string) bool {
    _, err := os.Stat(name)
    return err == nil
}

func loadCertificate(certFile, keyFile string) tls.Certificate {
    if fileExists(certFile) || fileExists(keyFile) {
        cert, err := tls.LoadX509KeyPair(certFile, keyFile)
        if err != nil {
            log.Fatal(err)
        }
        return cert
    } else {
        log.Println("No certificate found on disk. Generating self-signed certificate.")
        cert, err := createCertificate()
        if err != nil {
            log.Fatal(err)
        }
        return cert
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

func main() {
    http.HandleFunc("/ws", ws)
    cert := loadCertificate("server.crt", "server.key")

    C.Init()

    go func() {
        log.Println("Listening WS on :8080")
        if err := http.ListenAndServe(":8080", nil); err != nil {
            log.Fatal(err)
        }
    }()

    go func() {
        log.Println("Listening WSS on :8443")
        server := &http.Server{ Addr: ":8443", TLSConfig: &tls.Config{Certificates: []tls.Certificate{cert}} }
        if err := server.ListenAndServeTLS("", ""); err != nil {
            log.Fatal(err)
        }
    }()

    select {}
}
