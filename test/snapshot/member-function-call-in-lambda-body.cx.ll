
define i32 @main() {
  %lambda = alloca ptr, align 8
  store ptr @_EN4main9__lambda0EP1X, ptr %lambda, align 8
  ret i32 0
}

define i32 @_EN4main9__lambda0EP1X(ptr %x) {
  %1 = call i32 @_EN4main1X3fooE(ptr %x)
  ret i32 %1
}

define i32 @_EN4main1X3fooE(ptr %this) {
  ret i32 42
}
