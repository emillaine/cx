
%Foo = type { i32 }
%Bar = type { i32, i32 }

define i32 @main() {
  %foo = alloca %Foo, align 8
  %bar = alloca %Bar, align 8
  call void @_EN4main3Foo4initE(ptr %foo)
  call void @_EN4main3Bar4initE3int(ptr %bar, i32 -1)
  ret i32 0
}

define void @_EN4main3Foo4initE(ptr %this) {
  %i = getelementptr inbounds %Foo, ptr %this, i32 0, i32 0
  store i32 42, ptr %i, align 4
  ret void
}

define void @_EN4main3Bar4initE3int(ptr %this, i32 %j) {
  %i = getelementptr inbounds %Bar, ptr %this, i32 0, i32 0
  store i32 42, ptr %i, align 4
  %j1 = getelementptr inbounds %Bar, ptr %this, i32 0, i32 1
  store i32 %j, ptr %j1, align 4
  ret void
}
