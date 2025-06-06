
define i32 @main() {
  %b = alloca i1, align 1
  store i1 false, ptr %b, align 1
  %b.load = load i1, ptr %b, align 1
  %1 = xor i1 %b.load, true
  store i1 %1, ptr %b, align 1
  ret i32 0
}
