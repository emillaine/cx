
%"F<int, bool>" = type { i32, i1 }

define i32 @main() {
  %f = alloca %"F<int, bool>", align 8
  call void @_EN4main1FI3int4boolE4initE(ptr %f)
  call void @_EN4main1FI3int4boolE3fooE(ptr %f)
  ret i32 0
}

define void @_EN4main1FI3int4boolE4initE(ptr %this) {
  ret void
}

define void @_EN4main1FI3int4boolE3fooE(ptr %this) {
  ret void
}
