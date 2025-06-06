
%S = type { i32 }

@i = private global i32 0

define i32 @main() {
  %i = alloca i32, align 4
  %s = alloca %S, align 8
  store i32 0, ptr %i, align 4
  call void @_EN4main1S4initE(ptr %s)
  call void @_EN4main1S1fE(ptr %s)
  ret i32 0
}

define void @_EN4main1S4initE(ptr %this) {
  %i = getelementptr inbounds %S, ptr %this, i32 0, i32 0
  store i32 0, ptr %i, align 4
  ret void
}

define void @_EN4main1S1fE(ptr %this) {
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  ret void
}
