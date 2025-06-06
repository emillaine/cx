
%"A<C>" = type {}
%C = type { i32 }

define i32 @main() {
  %a = alloca %"A<C>", align 8
  call void @_EN4main1AI1CE4initE(ptr %a)
  ret i32 0
}

define void @_EN4main1AI1CE4initE(ptr %this) {
  %t = alloca %C, align 8
  call void @_EN4main1C4initE(ptr %t)
  ret void
}

define void @_EN4main1C4initE(ptr %this) {
  %i = getelementptr inbounds %C, ptr %this, i32 0, i32 0
  store i32 0, ptr %i, align 4
  ret void
}
