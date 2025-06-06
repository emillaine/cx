
%X = type { i32 }

define void @_EN4main3fooEP1X1X(ptr %p, %X %x) {
  store %X %x, ptr %p, align 4
  ret void
}

define void @_EN4main1X6deinitE(ptr %this) {
  ret void
}
