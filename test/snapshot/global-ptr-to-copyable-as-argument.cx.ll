
@sp = private global ptr null

define i32 @main() {
  %sp.load = load ptr, ptr @sp, align 8
  call void @_EN4main3fooEOP1S(ptr %sp.load)
  ret i32 0
}

define void @_EN4main3fooEOP1S(ptr %p) {
  ret void
}
