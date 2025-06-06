
@foo = private global i32 42
@qux = private global i32 42
@bar = private global i32 42

define i32 @main() {
  %d = alloca i32, align 4
  store i32 43, ptr @foo, align 4
  %bar.load = load i32, ptr @bar, align 4
  store i32 %bar.load, ptr %d, align 4
  ret i32 0
}
