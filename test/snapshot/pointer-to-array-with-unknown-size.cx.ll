
define i32 @main() {
  %a = alloca [3 x i32], align 4
  %p = alloca ptr, align 8
  %t = alloca i32, align 4
  store [3 x i32] [i32 1, i32 2, i32 3], ptr %a, align 4
  %1 = getelementptr inbounds [3 x i32], ptr %a, i32 0, i32 0
  store ptr %1, ptr %p, align 8
  store ptr %a, ptr %p, align 8
  store ptr %a, ptr %p, align 8
  %p.load = load ptr, ptr %p, align 8
  %2 = getelementptr inbounds i32, ptr %p.load, i32 1
  %.load = load i32, ptr %2, align 4
  store i32 %.load, ptr %t, align 4
  ret i32 0
}
