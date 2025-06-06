
define void @_EN4main3fooEOP3int(ptr %p) {
  ret void
}

define i32 @main() {
  %i = alloca i32, align 4
  %x = alloca ptr, align 8
  store i32 42, ptr %i, align 4
  store ptr %i, ptr %x, align 8
  %x.load = load ptr, ptr %x, align 8
  call void @_EN4main3fooEOP3int(ptr %x.load)
  call void @_EN4main3fooEOP3int(ptr %i)
  ret i32 0
}
