
%Foo = type { i32 }

define i32 @main() {
  %f = alloca %Foo, align 8
  %rf = alloca ptr, align 8
  store ptr %f, ptr %rf, align 8
  %rf.load = load ptr, ptr %rf, align 8
  call void @_EN4main3Foo3barE(ptr %rf.load)
  ret i32 0
}

define void @_EN4main3Foo3barE(ptr %this) {
  ret void
}
