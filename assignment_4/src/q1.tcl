set ns [new Simulator]

set nf [open q1.nam w]
$ns namtrace-all $nf

set tf [open q1.tr w]
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

set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]

$ns duplex-link $n1 $n2 $bw 10ms DropTail
$ns duplex-link $n2 $n3 $bw 10ms DropTail
$ns queue-limit $n1 $n2 $q
$ns queue-limit $n2 $n3 $q

set tcp0 [new Agent/TCP]
$ns attach-agent $n1 $tcp0
set sink [new Agent/TCPSink]
$ns attach-agent $n2 $sink
$ns connect $tcp0 $sink
$tcp0 set fid_ 0

set tcp1 [new Agent/TCP]
$ns attach-agent $n3 $tcp1
set sink [new Agent/TCPSink]
$ns attach-agent $n2 $sink
$ns connect $tcp1 $sink
$tcp1 set fid_ 1

set ftp0 [new Application/FTP]
$ftp0 attach-agent $tcp0
$ftp0 set type_ FTP

set ftp1 [new Application/FTP]
$ftp1 attach-agent $tcp1
$ftp1 set type_ FTP

$ns at 0.5 "$ftp0 start"
$ns at 4.5 "$ftp0 stop"
$ns at 0.5 "$ftp1 start"
$ns at 4.5 "$ftp1 stop"
$ns at 5.0 "finish"
$ns run
