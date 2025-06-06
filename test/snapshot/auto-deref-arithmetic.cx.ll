
define i32 @main() {
  %a = alloca i32, align 4
  %p = alloca ptr, align 8
  store i32 0, ptr %a, align 4
  store ptr %a, ptr %p, align 8
  %a.load = load i32, ptr %a, align 4
  %p.load = load ptr, ptr %p, align 8
  %p.load.load = load i32, ptr %p.load, align 4
  %1 = add i32 %a.load, %p.load.load
  store i32 %1, ptr %a, align 4
  %a.load1 = load i32, ptr %a, align 4
  %p.load2 = load ptr, ptr %p, align 8
  %p.load.load3 = load i32, ptr %p.load2, align 4
  %2 = mul i32 %a.load1, %p.load.load3
  store i32 %2, ptr %a, align 4
  ret i32 0
}
