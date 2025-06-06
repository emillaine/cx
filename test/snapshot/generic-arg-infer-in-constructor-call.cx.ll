
%"Foo<int>" = type { i32 }
%"Foo<string>" = type { %string }
%string = type { %"ArrayRef<char>" }
%"ArrayRef<char>" = type { ptr, i32 }

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1

define i32 @main() {
  %i = alloca %"Foo<int>", align 8
  %b = alloca %"Foo<string>", align 8
  %__str = alloca %string, align 8
  call void @_EN4main3FooI3intE4initE3int(ptr %i, i32 42)
  call void @_EN3std6string4initEP4char3int(ptr %__str, ptr @0, i32 0)
  %__str.load = load %string, ptr %__str, align 8
  call void @_EN4main3FooI6stringE4initE6string(ptr %b, %string %__str.load)
  ret i32 0
}

define void @_EN4main3FooI3intE4initE3int(ptr %this, i32 %t) {
  %t1 = getelementptr inbounds %"Foo<int>", ptr %this, i32 0, i32 0
  store i32 %t, ptr %t1, align 4
  ret void
}

declare void @_EN3std6string4initEP4char3int(ptr, ptr, i32)

define void @_EN4main3FooI6stringE4initE6string(ptr %this, %string %t) {
  %t1 = getelementptr inbounds %"Foo<string>", ptr %this, i32 0, i32 0
  store %string %t, ptr %t1, align 8
  ret void
}
