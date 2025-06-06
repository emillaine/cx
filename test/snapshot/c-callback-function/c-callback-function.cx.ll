
define i32 @_EN4main1fE3int(i32 %a) {
  ret i32 %a
}

define i32 @main() {
  call void @foo(ptr @_EN4main1fE3int)
  call void @bar(ptr @_EN4main1fE3int)
  call void @baz(ptr @_EN4main1fE3int)
  ret i32 0
}

declare void @foo(ptr)

declare void @bar(ptr)

declare void @baz(ptr)
