
%Foo = type { i32 }
%Bar = type { i32 }

define void @_EN4main3quxEP3FooP3Bar(ptr %f, ptr %b) {
  call void @_EN4main3Foo3fooE(ptr %f)
  call void @_EN4main3Bar3barE(ptr %b)
  ret void
}

define void @_EN4main3Foo3fooE(ptr %this) {
  ret void
}

define void @_EN4main3Bar3barE(ptr %this) {
  ret void
}

define i32 @main() {
  %f = alloca %Foo, align 8
  %b = alloca %Bar, align 8
  call void @_EN4main3Foo4initE(ptr %f)
  call void @_EN4main3Bar4initE(ptr %b)
  call void @_EN4main3quxEP3FooP3Bar(ptr %f, ptr %b)
  ret i32 0
}

define void @_EN4main3Foo4initE(ptr %this) {
  %i = getelementptr inbounds %Foo, ptr %this, i32 0, i32 0
  store i32 0, ptr %i, align 4
  ret void
}

define void @_EN4main3Bar4initE(ptr %this) {
  %i = getelementptr inbounds %Bar, ptr %this, i32 0, i32 0
  store i32 0, ptr %i, align 4
  ret void
}
