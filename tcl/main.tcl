
#compiler internals
set indent 0


proc out {args} {
    global indent
    for {set i 0} {$i < $indent} {incr i} {
        puts -nonewline "    "
    }
    puts [join $args " "]
}


proc unknown {args} {
    if {[llength $args] > 0} {
        puts -nonewline [lindex $args 0]
        eval [lrange $args 1 [llength args]]
    }
}

proc defer {statement body} {
    out $body
    out $statement
}
#
#proc assert {condition message} {
#    if {!$condition} {
#        error "Assertion failed: $message"
#    }
#}
#
#proc assert_equal {actual expected} {
#    set msg "Expected $expected, found $actual"
#    assert [expr {$actual == $expected}] $msg 
#}
#
##c statements
#proc const {type name equals value} {
#    assert_equal $equals "="
#    out "const $type $name = $value;"
#}
#
#proc var {type name equals value} {
#    assert_equal $equals "="
#    out "$type $name = $value;"
#}
#
#proc assign {name value} {
#    out "$name = $value;"
#}
#
#proc function {type name params body} {
#    global indent
#    out "$type $name ($params){"
#    incr indent
#    eval $body
#    incr indent -1
#    out "}"
#}
#
#proc ret {value} {
#    out "return $value;"
#}
#
##proc typedef {type name} {
##    out "typedef $type $name;"
##}
#
#proc struct {name fields} {
#    out "typedef struct $name { $fields } $name;\n"
#}
#
#
##set callerFile [info script]
##out "#define comment(str)" 
##out comment \( \"const $type $name $value - From file $callerFile \")
