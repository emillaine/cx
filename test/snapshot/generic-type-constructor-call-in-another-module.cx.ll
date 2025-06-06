
%"A<int>" = type { ptr }

define i32 @main() {
  %a = alloca %"A<int>", align 8
  call void @_EN33imported_generic_type_constructor1AI3intE4initE(ptr %a)
  ret i32 0
}

define void @_EN33imported_generic_type_constructor1AI3intE4initE(ptr %this) {
  %a = getelementptr inbounds %"A<int>", ptr %this, i32 0, i32 0
  store ptr null, ptr %a, align 8
  ret void
}
