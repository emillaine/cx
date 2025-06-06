
define i32 @main() {
  call void @foo(ptr null)
  ret i32 0
}

declare void @foo(ptr)
