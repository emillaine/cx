
define void @_EN4main1fE() {
  ret void
}

define i32 @_EN4main2f2E3int4bool(i32 %a, i1 %b) {
  ret i32 %a
}

define void @_EN4main1gEF_4void(ptr %p) {
  call void %p()
  ret void
}

define void @_EN4main2g2EF3int4bool_3int(ptr %p) {
  %a = alloca i32, align 4
  %1 = call i32 %p(i32 42, i1 false)
  %2 = add i32 %1, 1
  store i32 %2, ptr %a, align 4
  ret void
}

define i32 @main() {
  call void @_EN4main1gEF_4void(ptr @_EN4main1fE)
  call void @_EN4main2g2EF3int4bool_3int(ptr @_EN4main2f2E3int4bool)
  ret i32 0
}
