
define i1 @_EN4mainltEP1XP1X(ptr %a, ptr %b) {
  ret i1 true
}

define void @_EN4main2fxEP1XP1XP4voidP4void(ptr %a, ptr %b, ptr %v1, ptr %v2) {
  %1 = icmp eq ptr %a, %b
  %2 = icmp ult ptr %a, %b
  %3 = call i1 @_EN4mainltEP1XP1X(ptr %a, ptr %b)
  %4 = icmp ne ptr %v1, %v2
  %5 = icmp uge ptr %v1, %v2
  ret void
}
