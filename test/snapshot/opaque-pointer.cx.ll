
%S = type { ptr }

define i32 @main() {
  %s = alloca %S, align 8
  call void @_EN16opaque-pointer_h1S4initEOP6Opaque(ptr %s, ptr null)
  ret i32 0
}

define void @_EN16opaque-pointer_h1S4initEOP6Opaque(ptr %this, ptr %opaque) {
  %opaque1 = getelementptr inbounds %S, ptr %this, i32 0, i32 0
  store ptr %opaque, ptr %opaque1, align 8
  ret void
}
