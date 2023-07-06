package verushash

import (
	"github.com/663248062/go-verus2hash/verushash"
	"unsafe"
)

// Initialize verushash object once.
var verusHash = VH.NewVerushash()

func VerusHash_V2B2(serializedHeader []byte) []byte {
	hash := make([]byte, 32)
	ptrHash := uintptr(unsafe.Pointer(&hash[0]))
	verusHash.Verushash_v2b2(string(serializedHeader), ptrHash)
	return hash
}