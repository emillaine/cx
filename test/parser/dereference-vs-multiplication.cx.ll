
define i32 @_EN4main1gE() {
  ret i32 1
}

define void @_EN4main1fEP3int3int(ptr %a, i32 %b) {
  %m = alloca i32, align 4
  %n = alloca i32, align 4
  %1 = call i32 @_EN4main1gE()
  store i32 %1, ptr %m, align 4
  store i32 %b, ptr %a, align 4
  %2 = call i32 @_EN4main1gE()
  %3 = mul i32 %2, %b
  store i32 %3, ptr %n, align 4
  ret void
}
