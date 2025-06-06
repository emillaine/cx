
%A = type { i32, i32 }
%B = type { i32, i32 }

define i32 @main() {
  %a = alloca %A, align 8
  %b = alloca %B, align 8
  call void @_EN4main1A4initE(ptr %a)
  %1 = call i32 @_EN4main1A3fooE(ptr %a)
  call void @_EN4main1A3barE(ptr %a)
  call void @_EN4main1B4initE(ptr %b)
  %2 = call i32 @_EN4main1B3fooE(ptr %b)
  call void @_EN4main1B3barE(ptr %b)
  ret i32 0
}

define void @_EN4main1A4initE(ptr %this) {
  %j = getelementptr inbounds %A, ptr %this, i32 0, i32 1
  store i32 42, ptr %j, align 4
  ret void
}

define i32 @_EN4main1A3fooE(ptr %this) {
  %j = getelementptr inbounds %A, ptr %this, i32 0, i32 1
  %j.load = load i32, ptr %j, align 4
  ret i32 %j.load
}

define void @_EN4main1A3barE(ptr %this) {
  %j = getelementptr inbounds %A, ptr %this, i32 0, i32 1
  store i32 1, ptr %j, align 4
  ret void
}

define void @_EN4main1B4initE(ptr %this) {
  %j = getelementptr inbounds %B, ptr %this, i32 0, i32 1
  store i32 42, ptr %j, align 4
  ret void
}

define i32 @_EN4main1B3fooE(ptr %this) {
  %j = getelementptr inbounds %B, ptr %this, i32 0, i32 1
  %j.load = load i32, ptr %j, align 4
  ret i32 %j.load
}

define void @_EN4main1B3barE(ptr %this) {
  %j = getelementptr inbounds %B, ptr %this, i32 0, i32 1
  store i32 1, ptr %j, align 4
  ret void
}
