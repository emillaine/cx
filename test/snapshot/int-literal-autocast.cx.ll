
declare void @foo(i8)

define i32 @main() {
  %b = alloca i64, align 8
  %c = alloca i8, align 1
  call void @foo(i8 1)
  store i64 42, ptr %b, align 4
  store i8 -42, ptr %c, align 1
  %b.load = load i64, ptr %b, align 4
  %1 = add i64 %b.load, 1
  store i64 %1, ptr %b, align 4
  ret i32 0
}
