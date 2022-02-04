package main

import (
	"fmt"
	"unsafe"
	"golang.org/x/sys/unix"
)

func main() {
	fmt.Println("hello")
	PR_SCHED_CORE_SCOPE_THREAD := 0
	if _, _, errno := unix.Syscall6(unix.SYS_PRCTL, unix.PR_SCHED_CORE,
		unix.PR_SCHED_CORE_CREATE, 0,
		uintptr(PR_SCHED_CORE_SCOPE_THREAD),
		0, 0); errno != 0 {
		fmt.Printf("Failed to core tag: %v", errno)
	}

	var cookie uint64
	if _, _, errno := unix.Syscall6(unix.SYS_PRCTL, unix.PR_SCHED_CORE,
		unix.PR_SCHED_CORE_GET, 0, uintptr(PR_SCHED_CORE_SCOPE_THREAD),
		uintptr(unsafe.Pointer(&cookie)), 0); errno != 0 {
		fmt.Printf("Failed to get core tag: %v", errno)
	}
	fmt.Printf("Core Tag=%d", cookie)
}
