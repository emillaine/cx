
define i32 @main() {
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  %i.load = load i32, ptr %i, align 4
  %1 = add i32 %i.load, 1
  store i32 %1, ptr %i, align 4
  %i.load1 = load i32, ptr %i, align 4
  %2 = sub i32 %i.load1, 1
  store i32 %2, ptr %i, align 4
  %i.load2 = load i32, ptr %i, align 4
  %3 = mul i32 %i.load2, 1
  store i32 %3, ptr %i, align 4
  %i.load3 = load i32, ptr %i, align 4
  %4 = sdiv i32 %i.load3, 1
  store i32 %4, ptr %i, align 4
  %i.load4 = load i32, ptr %i, align 4
  %5 = and i32 %i.load4, 1
  store i32 %5, ptr %i, align 4
  %i.load5 = load i32, ptr %i, align 4
  %6 = or i32 %i.load5, 1
  store i32 %6, ptr %i, align 4
  %i.load6 = load i32, ptr %i, align 4
  %7 = xor i32 %i.load6, 1
  store i32 %7, ptr %i, align 4
  %i.load7 = load i32, ptr %i, align 4
  %8 = shl i32 %i.load7, 1
  store i32 %8, ptr %i, align 4
  %i.load8 = load i32, ptr %i, align 4
  %9 = ashr i32 %i.load8, 1
  store i32 %9, ptr %i, align 4
  ret i32 0
}
