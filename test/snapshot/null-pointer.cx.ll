
define ptr @_EN4main3fooE() {
  ret ptr null
}

define i32 @main() {
  %isNull = alloca i1, align 1
  %isNonNull = alloca i1, align 1
  %ptr = alloca ptr, align 8
  %ptr2 = alloca ptr, align 8
  %1 = call ptr @_EN4main3fooE()
  %2 = icmp eq ptr %1, null
  store i1 %2, ptr %isNull, align 1
  %3 = call ptr @_EN4main3fooE()
  %4 = icmp ne ptr %3, null
  store i1 %4, ptr %isNonNull, align 1
  %5 = call ptr @_EN4main3fooE()
  store ptr %5, ptr %ptr, align 8
  store ptr null, ptr %ptr2, align 8
  ret i32 0
}
