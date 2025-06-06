
define void @_EN4main1fEP3int(ptr %a) {
  %p = alloca ptr, align 8
  store ptr %a, ptr %p, align 8
  %p.load = load ptr, ptr %p, align 8
  %p.load.load = load i32, ptr %p.load, align 4
  %1 = add i32 %p.load.load, 1
  store i32 %1, ptr %p.load, align 4
  %a.load = load i32, ptr %a, align 4
  %2 = add i32 %a.load, -1
  store i32 %2, ptr %a, align 4
  ret void
}
