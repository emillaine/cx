
define i32 @main() {
  %d = alloca i32, align 4
  store i32 42, ptr %d, align 4
  ret i32 0
}
