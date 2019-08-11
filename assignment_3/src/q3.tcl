set ns [new Simulator]

proc myRand {min max} {
    expr {int(rand() * 100) % ($max + 1 - $min) + $min}
}

set nf [open out.nam w]
$ns namtrace-all $nf

proc finish {} {
    global ns nf
    $ns flush-trace
    close $nf
    exec nam out.nam &
    exit
}

set n 0
puts "Enter number of nodes:"
gets stdin n

for {set i 0} {$i < $n} {incr i} {
	set nodes($i) [$ns node]
}

$nodes(0) shape box
$nodes(0) color red

puts "Enter number of connections:"
gets stdin k

set colors {}
while {[llength $colors] < $k} {
    set r [myRand 1 0xFF]
    set g [myRand 1 0xFF]
    set b [myRand 1 0xFF]
    set c [format "#%02x%02x%02x" $r $g $b]
    if {$c ni $colors} {
		$ns color [llength $colors] $c
    	lappend colors $c
    }
}

for {set i 1} {$i < $n} {incr i} {
	$ns duplex-link $nodes(0) $nodes($i) 1Mb 10ms DropTail
}

puts "Please enter connections as comma seperated pairs(for eg \"2,3\"):"

for {set i 0} {$i < $k} {incr i} {
	gets stdin str
	scan $str "%d,%d" a b
	set tcp($i) [new Agent/TCP]
	$ns attach-agent $nodes($a) $tcp($i)
	set sink($i) [new Agent/TCPSink]
	$ns attach-agent $nodes($b) $sink($i)
	$ns connect $tcp($i) $sink($i)
	$tcp($i) set fid_ $i
	set ftp($i) [new Application/FTP]
	$ftp($i) attach-agent $tcp($i)
	$ns at 0.5 "$ftp($i) start"
	$ns at 3.5 "$ftp($i) stop"
}

$ns at 5.0 "finish"
$ns run
