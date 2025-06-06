
%Foo = type { i32 }

define void @_EN4main3fooEP3Foo(ptr %ref_f) {
  ret void
}

define i32 @main() {
  %f = alloca %Foo, align 8
  call void @_EN4main3fooEP3Foo(ptr %f)
  ret i32 0
}
