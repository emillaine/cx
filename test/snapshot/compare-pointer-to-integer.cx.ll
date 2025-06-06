
define void @_EN4main1fEP3int3int(ptr %foo, i32 %bar) {
  %foo.load = load i32, ptr %foo, align 4
  %1 = icmp slt i32 %foo.load, %bar
  ret void
}
