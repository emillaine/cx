
%Foo = type { i32 }

define void @_EN4main3ptrEP3Foo(ptr %p) {
  ret void
}

define i32 @main() {
  %f = alloca %Foo, align 8
  call void @_EN4main3ptrEP3Foo(ptr %f)
  ret i32 0
}
