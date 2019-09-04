set ns [new Simulator]

set nf [open q2.nam w]
$ns namtrace-all $nf

set tf [open q2.tr w]
$ns trace-all $tf

proc finish {} {
    global ns nf tf
    $ns flush-trace
    close $nf
    close $tf
    exit 0
}

set q [lindex $argv 0]
set bw [lindex $argv 1]

set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

$ns duplex-link $n0 $n2 $bw 10ms DropTail
$ns duplex-link $n1 $n2 $bw 10ms DropTail
$ns duplex-link $n2 $n3 $bw 10ms DropTail
$ns duplex-link $n3 $n4 $bw 10ms DropTail
$ns duplex-link $n3 $n5 $bw 10ms DropTail

$ns queue-limit $n0 $n2 $q
$ns queue-limit $n1 $n2 $q
$ns queue-limit $n2 $n3 $q
$ns queue-limit $n3 $n4 $q
$ns queue-limit $n3 $n5 $q

Agent/Ping instproc recv {from rtt} {
}

set ping0 [new Agent/Ping]
set ping1 [new Agent/Ping]
set ping4 [new Agent/Ping]
set ping5 [new Agent/Ping]

$ping0 set packetSize_ 50000
$ping5 set packetSize_ 50000

$ping0 set interval_ 0.0001
$ping5 set interval_ 0.0001

$ns attach-agent $n0 $ping0
$ns attach-agent $n1 $ping1
$ns attach-agent $n4 $ping4
$ns attach-agent $n5 $ping5

$ns connect $ping0 $ping4
$ns connect $ping5 $ping1

$ns at 0.1 "$ping0 send"
$ns at 0.2 "$ping0 send"
$ns at 0.3 "$ping0 send"
$ns at 0.4 "$ping0 send"
$ns at 0.5 "$ping0 send"
$ns at 0.6 "$ping0 send"
$ns at 0.7 "$ping0 send"
$ns at 0.8 "$ping0 send"
$ns at 0.9 "$ping0 send"
$ns at 1.0 "$ping0 send"
$ns at 1.1 "$ping0 send"
$ns at 1.2 "$ping0 send"
$ns at 1.3 "$ping0 send"
$ns at 1.4 "$ping0 send"
$ns at 1.5 "$ping0 send"
$ns at 1.6 "$ping0 send"
$ns at 1.7 "$ping0 send"
$ns at 1.8 "$ping0 send"
$ns at 1.9 "$ping0 send"
$ns at 2.0 "$ping0 send"
$ns at 2.1 "$ping0 send"
$ns at 2.2 "$ping0 send"
$ns at 2.3 "$ping0 send"
$ns at 2.4 "$ping0 send"
$ns at 2.5 "$ping0 send"
$ns at 2.6 "$ping0 send"
$ns at 2.7 "$ping0 send"
$ns at 2.8 "$ping0 send"
$ns at 2.9 "$ping0 send"

$ns at 0.1 "$ping5 send"
$ns at 0.2 "$ping5 send"
$ns at 0.3 "$ping5 send"
$ns at 0.4 "$ping5 send"
$ns at 0.5 "$ping5 send"
$ns at 0.6 "$ping5 send"
$ns at 0.7 "$ping5 send"
$ns at 0.8 "$ping5 send"
$ns at 0.9 "$ping5 send"
$ns at 1.0 "$ping5 send"
$ns at 1.1 "$ping5 send"
$ns at 1.2 "$ping5 send"
$ns at 1.3 "$ping5 send"
$ns at 1.4 "$ping5 send"
$ns at 1.5 "$ping5 send"
$ns at 1.6 "$ping5 send"
$ns at 1.7 "$ping5 send"
$ns at 1.8 "$ping5 send"
$ns at 1.9 "$ping5 send"
$ns at 2.0 "$ping5 send"
$ns at 2.1 "$ping5 send"
$ns at 2.2 "$ping5 send"
$ns at 2.3 "$ping5 send"
$ns at 2.4 "$ping5 send"
$ns at 2.5 "$ping5 send"
$ns at 2.6 "$ping5 send"
$ns at 2.7 "$ping5 send"
$ns at 2.8 "$ping5 send"
$ns at 2.9 "$ping5 send"

$ns at 3.0 "finish"
$ns run
