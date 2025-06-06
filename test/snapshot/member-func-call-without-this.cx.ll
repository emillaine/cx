
%S = type {}
%"Foo<int>" = type {}

define i32 @main() {
  %s = alloca %S, align 8
  %t = alloca %"Foo<int>", align 8
  call void @_EN4main1S4initE(ptr %s)
  call void @_EN4main1S3fooE(ptr %s)
  call void @_EN4main3FooI3intE4initE(ptr %t)
  call void @_EN4main3FooI3intE3bazE(ptr %t)
  call void @_EN4main3FooI3intE6deinitE(ptr %t)
  ret i32 0
}

define void @_EN4main1S4initE(ptr %this) {
  ret void
}

define void @_EN4main1S3fooE(ptr %this) {
  call void @_EN4main1S3barE(ptr %this)
  ret void
}

define void @_EN4main3FooI3intE4initE(ptr %this) {
  call void @_EN4main3FooI3intE3bazE(ptr %this)
  ret void
}

define void @_EN4main3FooI3intE3bazE(ptr %this) {
  call void @_EN4main3FooI3intE3quxE(ptr %this)
  ret void
}

define void @_EN4main3FooI3intE6deinitE(ptr %this) {
  call void @_EN4main3FooI3intE3bazE(ptr %this)
  ret void
}

define void @_EN4main1S3barE(ptr %this) {
  ret void
}

define void @_EN4main3FooI3intE3quxE(ptr %this) {
  ret void
}
