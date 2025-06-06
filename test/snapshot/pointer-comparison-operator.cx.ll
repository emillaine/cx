
define void @_EN4main3fooEOP3intP3int(ptr %a, ptr %b) {
  %1 = icmp eq ptr %a, %b
  %2 = icmp ne ptr %a, %b
  ret void
}
