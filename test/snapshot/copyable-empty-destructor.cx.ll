
%Foo = type {}

define i32 @main() {
  %foo = alloca %Foo, align 8
  call void @_EN4main3Foo6deinitE(ptr %foo)
  ret i32 0
}

define void @_EN4main3Foo6deinitE(ptr %this) {
  ret void
}
