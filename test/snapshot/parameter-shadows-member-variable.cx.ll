
%S = type { i32 }

define i32 @main() {
  %1 = alloca %S, align 8
  call void @_EN4main1S4initE(ptr %1)
  call void @_EN4main1S3fooE3int(ptr %1, i32 30)
  ret i32 0
}

define void @_EN4main1S4initE(ptr %this) {
  ret void
}

define void @_EN4main1S3fooE3int(ptr %this, i32 %bar) {
  %1 = add i32 %bar, 42
  ret void
}
