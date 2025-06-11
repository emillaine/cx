
define void @_EN4main3barE() {
  ret void
}

define void @_EN4main3fooE3int(i32 %bar) {
  call void @_EN4main3barE()
  ret void
}
