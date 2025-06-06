
define i32 @main() {
  %i = alloca i32, align 4
  store i32 42, ptr %i, align 4
  call void @_EN4main1fI3intEEP3int(ptr %i)
  ret i32 0
}

define void @_EN4main1fI3intEEP3int(ptr %p) {
  ret void
}
