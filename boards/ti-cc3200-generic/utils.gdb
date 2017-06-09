
define reset
  monitor reset init
  monitor reset halt
  set $sp = ((unsigned*)&__interrupt_vector )[0]
  set $pc = ((unsigned*)&__interrupt_vector )[1]
  continue
end


define lreset
  monitor reset init
  monitor reset halt
  load
  set $sp = __interrupt_vector[0]
  set $pc = __interrupt_vector[1]
end
