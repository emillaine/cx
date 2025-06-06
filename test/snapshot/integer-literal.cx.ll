
define i32 @main() {
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %k = alloca i32, align 4
  store i32 42, ptr %i, align 4
  store i32 1, ptr %j, align 4
  store i32 105, ptr %k, align 4
  ret i32 0
}
